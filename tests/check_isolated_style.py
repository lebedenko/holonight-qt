# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

"""Fresh-process installed style acceptance with no host/build QML search roots."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

probe, staged_qml, qt_qml, staged_plugins, config_lib, patchelf = map(Path, sys.argv[1:])


def require(condition, message):
    if not condition:
        raise AssertionError(message)


with tempfile.TemporaryDirectory(prefix="uqc-isolated-") as temporary:
    root = Path(temporary)
    qt = root / "qt"
    qt.mkdir()
    for module in ("QtQml", "QtQuick"):
        shutil.copytree(qt_qml / module, qt / module)
    cases = {
        "complete": None,
        "module": 'module "Holonight" is not installed',
        "plugin": 'plugin "holonight_qml" not found',
        "core": 'module "Holonight.Core" is not installed',
        "impl": 'module "Holonight.impl" is not installed',
        "native": f"lib{root.name}_missing_config.so",
        "casing": 'module "HoloNight" is not installed',
        "explicit": None,
        "imperative": None,
        "platform": None,
    }
    for case, diagnostic in cases.items():
        fixture = root / case
        qml = fixture / "qml"
        qml.mkdir(parents=True)

        def omit(directory, names):
            excluded = set()
            if case == "plugin" and Path(directory) == staged_qml / "Holonight":
                excluded.add("libholonight_qml.so")
            if Path(directory) == staged_qml / "Holonight":
                excluded.add({"core": "Core", "impl": "impl"}.get(case, ""))
            return excluded.intersection(names)

        if case != "module":
            shutil.copytree(staged_qml / "Holonight", qml / "Holonight", ignore=omit)
        if case == "native":
            plugin = qml / "Holonight/libholonight_qml.so"
            dependencies = subprocess.check_output([patchelf, "--print-needed", plugin], text=True).splitlines()
            dependencies = [name for name in dependencies if "holonight_config" in name]
            require(len(dependencies) == 1, f"Configuration dependency not identified: {dependencies}")
            subprocess.run([patchelf, "--replace-needed", dependencies[0], diagnostic, plugin], check=True)
        source = fixture / "probe.qml"
        imports = "import QtQuick\nimport QtQuick.Controls as C\n"
        children = 'C.Button { objectName: "button" } C.TextField { objectName: "field" }'
        if case == "casing":
            imports += "import HoloNight\n"
        if case == "explicit":
            imports += "import QtQuick.Controls.Basic as B\nimport QtQuick.Controls.Fusion as F\n"
            children += ' B.Button { objectName: "basic" } F.TextField { objectName: "fusion" }'
        source.write_text(imports + "Item { " + children + " }\n")
        env = {key: value for key, value in os.environ.items()
               if not key.startswith(("QT_", "QML", "UQC_", "XDG_", "LD_", "HOLONIGHT_"))}
        runtime = fixture / "runtime"
        runtime.mkdir(mode=0o700)
        env.update(QT_QPA_PLATFORM="offscreen", QT_QUICK_BACKEND="software",
                   QT_QPA_PLATFORMTHEME="", QT_STYLE_OVERRIDE="", QT_QUICK_CONTROLS_STYLE="Holonight",
                   UQC_MODE=case, UQC_FIXTURE=str(qml), UQC_QT_MODULES=str(qt), UQC_SOURCE=str(source),
                   XDG_RUNTIME_DIR=str(runtime), XDG_CONFIG_HOME=str(fixture / "config"),
                   XDG_CACHE_HOME=str(fixture / "cache"), XDG_DATA_HOME=str(fixture / "data"),
                   HOLONIGHT_APPEARANCE_FILE=str(fixture / "missing.toml"), LD_LIBRARY_PATH=str(config_lib))
        if case == "platform":
            plugins = fixture / "plugins"
            shutil.copytree(staged_plugins / "platformthemes", plugins / "platformthemes")
            empty_conf = fixture / "empty.conf"
            empty_conf.write_text("[Controls]\n")
            env.update(QT_QPA_PLATFORMTHEME="holonight", QT_PLUGIN_PATH=str(plugins),
                       QT_QUICK_CONTROLS_CONF=str(empty_conf))
            env.pop("QT_QUICK_CONTROLS_STYLE")
        command = [str(probe)] + (["-style", "Holonight"] if case == "imperative" else [])
        default_origins = None
        if case == "platform":
            # Measure this Qt build's default in another fresh process. The platform
            # theme must preserve those origins, independent of Qt's default name.
            reference_env = env | {"QT_QPA_PLATFORMTHEME": ""}
            reference = subprocess.run(command, env=reference_env, text=True, capture_output=True, timeout=20)
            print(f"platform reference: exit={reference.returncode}\n{reference.stdout}{reference.stderr}", flush=True)
            require(reference.returncode == 0 and not reference.stderr.strip(), "Qt default reference failed")
            reference_report = json.loads(reference.stdout)
            require(not reference_report["errors"], "Qt default reference emitted errors")
            require(not any("holonight" in lib.lower() for lib in reference_report["libraries"]),
                    "Qt default reference loaded HoloNight")
            default_origins = reference_report["origins"]
        result = subprocess.run(command, env=env, text=True, capture_output=True, timeout=20)
        print(f"{case}: exit={result.returncode}\n{result.stdout}{result.stderr}", flush=True)
        require(result.returncode == (1 if diagnostic else 0), f"{case}: unexpected exit")
        report = json.loads(result.stdout)
        require(report["imports"] == [str(qml), str(qt)], f"{case}: import leak")
        for library in report["libraries"]:
            if "holonight" in Path(library).name.lower() and "config" not in Path(library).name.lower():
                require(library.startswith(str(fixture) + "/"), f"{case}: host/build library loaded: {library}")
        if diagnostic:
            require(diagnostic in report["errors"], f"{case}: missing intended diagnostic")
            continue
        require(not report["errors"] and not result.stderr.strip(), f"{case}: unexpected diagnostics")
        style = "QtQuick/Controls/Fusion" if case == "imperative" else "Holonight"
        if case == "platform":
            require(report["origins"] == default_origins, "Platform theme changed Qt default control origins")
            style = "QtQuick/Controls/"
        for name, control in (("button", "Button"), ("field", "TextField")):
            require(any((f"/{style}/{control}.qml" in url if case != "platform"
                         else "/QtQuick/Controls/" in url and url.endswith(f"/{control}.qml"))
                        for url in report["origins"][name]),
                    f"{case}: wrong {name} origin")
        if style == "Holonight":
            for module in ("libholonight_qml.so", "Core/libholonight_core_qml.so", "impl/libholonight_impl_qml.so"):
                require(str(qml / "Holonight" / module) in report["libraries"], f"{case}: missing fixture plugin {module}")
        else:
            require(not any("libholonight_qml.so" in lib for lib in report["libraries"]), f"{case}: style plugin loaded")
        if case == "explicit":
            for name, path in (("basic", "Basic/Button"), ("fusion", "Fusion/TextField")):
                require(any(f"/QtQuick/Controls/{path}.qml" in url for url in report["origins"][name]),
                        f"{case}: competing import overridden")
        if case == "platform":
            require(any(lib.startswith(str(plugins)) and "qholonight" in lib for lib in report["libraries"]),
                    "Staged platform theme did not load")
