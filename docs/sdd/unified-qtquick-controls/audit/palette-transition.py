#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
"""Collect UQC-221 fixtures, or separately assert native appearance transitions."""

import argparse
import json
import os
from pathlib import Path
import subprocess


APPEARANCE = """
version = 1

[theme]
scheme = "holonight-dark"
accent = "blue"

[typography]
ui_family = "Inter"
ui_size = 12
monospace_family = "JetBrains Mono"
monospace_size = 12
title_family = "Audiowide"
title_size = 10
display_family = "Rajdhani"
display_size = 24

[icons]
theme = "HoloNight"
fallback = "Papirus"
cursor = "default"

[layout]
scale = 1.0

[shape]
style = "inherit"
scale = 1.0
"""


def fixture(window, native):
    imports = "import QtQuick\nimport QtQuick.Controls as C\n"
    if not native:
        imports += "import Holonight.Core\nimport Holonight.Controls\n"
    body = """
    id: fixtureRoot
    width: 360; height: 390; visible: true
    objectName: "fixtureWindow"
    property int captureRevision: 0
    Rectangle { width: 2; height: 2; color: fixtureRoot.captureRevision % 2 ? "white" : "black" }
    Column {
        x: 20; y: 20; spacing: 12
        C.TextField { objectName: "nativeText"; width: 300; text: "Native text" }
        C.SpinBox { objectName: "nativeSpin"; width: 300; value: 42 }
        C.ComboBox { objectName: "nativeCombo"; width: 300; model: ["First", "Second"] }
        C.TextField { objectName: "localOverride"; width: 300; text: "Local override"; palette.base: "#bada55" }
        C.TextField { objectName: "disabledText"; width: 300; text: "Disabled"; enabled: false }
"""
    if not native:
        body += """
        HnSearchField { objectName: "sharedSearch"; width: 300; text: "Search" }
        HnIconComboBox { objectName: "sharedCombo"; width: 300; model: ["First", "Second"] }
"""
    body += "    }\n"
    if not native:
        body += "    property color appearanceBase: HoloniightPalette.background\n"
    return imports + window + " {\n" + body + "}\n"


def collect(args):
    args.output.mkdir(parents=True, exist_ok=False)
    prefix = args.prefix.resolve()
    for native, window in [
        (False, "Window"),
        (False, "HnApplicationWindow"),
        (True, "Window"),
    ]:
        for style in ["Fusion"] if native else ["Fusion", "Holonight"]:
            for mechanism in ["application"] if native else ["file", "application"]:
                for scale in ["1", "1.25"]:
                    name = (
                        f"{'native' if native else window}-{style}-{mechanism}-{scale}"
                    )
                    case = args.output / name
                    case.mkdir()
                    qml = case / "fixture.qml"
                    qml.write_text(fixture(window, native))
                    appearance = case / "appearance.toml"
                    appearance.write_text(APPEARANCE)
                    env = {
                        k: v
                        for k, v in os.environ.items()
                        if not k.startswith(("QT_", "QML", "HOLONIGHT_", "LD_"))
                    }
                    for key in [
                        "XDG_CONFIG_HOME",
                        "XDG_DATA_HOME",
                        "XDG_CACHE_HOME",
                        "XDG_STATE_HOME",
                        "XDG_RUNTIME_DIR",
                    ]:
                        path = case / key.lower()
                        path.mkdir(mode=0o700)
                        env[key] = str(path)
                    env.update(
                        QT_QPA_PLATFORM="offscreen",
                        QT_QUICK_BACKEND="software",
                        QT_QUICK_CONTROLS_STYLE=style,
                        QT_SCALE_FACTOR=scale,
                        QT_QPA_PLATFORMTHEME="" if native else "holonight",
                        QT_PLUGIN_PATH=""
                        if native
                        else str(prefix / "lib/qt6/plugins"),
                        QML_IMPORT_PATH="" if native else str(prefix / "lib/qt6/qml"),
                        LD_LIBRARY_PATH="" if native else str(prefix / "lib"),
                        HOLONIGHT_APPEARANCE_FILE=str(appearance),
                        QT_FORCE_STDERR_LOGGING="1",
                        QML_IMPORT_TRACE="1",
                        QT_DEBUG_PLUGINS="1",
                    )
                    bus = case / "bus.conf"
                    bus.write_text(
                        "<busconfig><type>session</type><listen>unix:tmpdir=/tmp</listen>"
                        '<policy context="default"><allow user="*"/><allow send_destination="*"/>'
                        '<allow receive_sender="*"/><allow own="*"/></policy></busconfig>'
                    )
                    command = [
                        "bwrap",
                        "--bind",
                        "/",
                        "/",
                        "--dev",
                        "/dev",
                        "--die-with-parent",
                    ]
                    for host in [
                        "/usr/lib/qt6/qml/Holonight",
                        "/usr/local/lib/qt6/qml/Holonight",
                    ]:
                        if Path(host).exists():
                            command += ["--tmpfs", host]
                    host_theme = "/usr/lib/qt6/plugins/platformthemes/libqholonight.so"
                    if Path(host_theme).exists():
                        empty = case / "hidden-plugin"
                        empty.touch()
                        command += ["--ro-bind", str(empty), host_theme]
                    command += [
                        "dbus-run-session",
                        "--config-file=" + str(bus),
                        "--",
                        str(args.executable.resolve()),
                        str(qml),
                        mechanism,
                        str(case),
                    ]
                    (case / "command.json").write_text(
                        json.dumps(command, indent=2) + "\n"
                    )
                    with (case / "launch.log").open("w") as log:
                        result = subprocess.run(
                            command,
                            env=env,
                            stdout=log,
                            stderr=subprocess.STDOUT,
                            timeout=40,
                        )
                    if result.returncode:
                        raise RuntimeError(
                            f"{name}: collection failed ({result.returncode}); see launch.log"
                        )
                    maps = (case / "maps.txt").read_text()
                    libraries = sorted(
                        {
                            line.split()[-1]
                            for line in maps.splitlines()
                            if "holonight" in line.lower() and ".so" in line
                        }
                    )
                    if style == "Fusion":
                        assert "libQt6QuickControls2Fusion.so" in maps
                    if native:
                        assert not libraries, libraries
                        assert "/platformthemes/" not in maps
                    else:
                        assert any("libqholonight.so" in lib for lib in libraries)
                        assert libraries and all(
                            str(prefix) in lib for lib in libraries
                        ), libraries
                    data = json.loads((case / "measurements.json").read_text())
                    assert all(
                        r[w]["dpr"] == float(scale)
                        for r in data["records"]
                        for w in ["primary", "secondary"]
                    )
                    (case / "isolation.json").write_text(
                        json.dumps({"libraries": libraries, "verified": True}, indent=2)
                    )
                    print(name, "collected; mappings and DPR verified", flush=True)


def check(args):
    failures = []
    selected = 0
    for path in sorted(args.output.glob("*/measurements.json")):
        is_file = "-file-" in path.parent.name
        if (args.scope == "p03") != is_file:
            continue
        if args.scope == "external" and not path.parent.name.startswith("native-"):
            continue
        selected += 1
        records = json.loads(path.read_text())["records"]
        assert [r["phase"] for r in records] == [
            "dark",
            "light",
            "light-recreated",
            "dark-return",
        ]
        for window in ["primary", "secondary"]:
            states = [
                {o["name"]: o for o in r[window]["objects"] if o["name"]}
                for r in records
            ]
            for name in ["nativeText", "nativeSpin", "nativeCombo", "disabledText"]:
                role = "Button" if name == "nativeCombo" else "Base"
                group = "1" if name == "disabledText" else "0"
                colors = [s[name]["palette"][group][role] for s in states]
                # Recreated and existing controls must agree; dark must round-trip.
                passed = colors[0] != colors[1] and colors[0] == colors[3]
                if args.scope != "positive":
                    passed = passed and colors[1] == colors[2]
                if not passed:
                    failures.append(f"{path.parent.name}/{window}/{name}: {colors}")
            assert all(
                s["localOverride"]["palette"]["0"]["Base"] == "#ffbada55"
                for s in states
            )
    expected = {"p03": 8, "positive": 10, "external": 2}[args.scope]
    assert selected == expected, f"Expected {expected} cases, found {selected}"
    for failure in failures:
        print(args.scope.upper(), "FAIL", failure)
    print(
        f"{len(failures)} native palette transition failures; collection is independent"
    )
    return bool(failures)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=["collect", "assert"])
    parser.add_argument("--output", required=True, type=lambda p: Path(p).resolve())
    parser.add_argument(
        "--scope", choices=["p03", "positive", "external"], default="p03"
    )
    parser.add_argument("--prefix", type=Path)
    parser.add_argument("--executable", type=Path)
    args = parser.parse_args()
    if args.action == "collect":
        if not args.prefix or not args.executable:
            parser.error("collect requires --prefix and --executable")
        collect(args)
        return 0
    return check(args)


if __name__ == "__main__":
    raise SystemExit(main())
