# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

"""Bounded offscreen startup checks of the actual example executables.

These check QML resolution, plugin origins and startup diagnostics, not desktop
interaction or visual acceptance. Every gallery page is eagerly instantiated.
"""

import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("executable", type=Path)
parser.add_argument("qml_root", type=Path)
parser.add_argument("mode", choices=["default", "environment", "command-line", "configuration"])
args = parser.parse_args()

with tempfile.TemporaryDirectory(prefix="holonight-example-") as directory:
    root = Path(directory)
    env = os.environ.copy()
    for key in ("QT_QUICK_CONTROLS_STYLE", "QT_QUICK_CONTROLS_CONF",
                "QT_QUICK_CONTROLS_FALLBACK_STYLE", "QML_IMPORT_PATH", "QML2_IMPORT_PATH",
                "QT_PLUGIN_PATH"):
        env.pop(key, None)
    env.update(QT_QPA_PLATFORM="offscreen", QT_QPA_PLATFORMTHEME="", QT_STYLE_OVERRIDE="",
               QT_QUICK_BACKEND="software", QT_FORCE_STDERR_LOGGING="1", QML_IMPORT_TRACE="1",
               QT_LOGGING_RULES="*.debug=false;qt.qml.import.debug=true;*.warning=true;*.critical=true",
               HOLONIGHT_APPEARANCE_FILE=str(root / "appearance.toml"), XDG_RUNTIME_DIR=str(root))
    command = [str(args.executable.resolve())]
    expected_style = "Holonight" if args.mode == "default" else "Fusion"
    if args.mode == "environment":
        env["QT_QUICK_CONTROLS_STYLE"] = "Fusion"
    elif args.mode == "command-line":
        env["QT_QUICK_CONTROLS_STYLE"] = "Holonight"
        command += ["-style", "Fusion"]
    elif args.mode == "configuration":
        config = root / "qtquickcontrols2.conf"
        config.write_text("[Controls]\nStyle=Fusion\n")
        env["QT_QUICK_CONTROLS_CONF"] = str(config)

    process = subprocess.Popen(command, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    try:
        try:
            output, _ = process.communicate(timeout=3)
        except subprocess.TimeoutExpired:
            mappings = Path(f"/proc/{process.pid}/maps").read_text()
            process.terminate()
            output, _ = process.communicate(timeout=5)
        else:
            raise AssertionError(f"Example exited during startup ({process.returncode}):\n{output.decode()}")
    finally:
        if process.poll() is None:
            process.kill()
            process.communicate()

    trace = output.decode()
    diagnostics = [line for line in trace.splitlines()
                   if not line.startswith("qt.qml.import:")]
    assert not diagnostics, "Unexpected startup diagnostics:\n" + "\n".join(diagnostics)
    style_path = "/Holonight/" if expected_style == "Holonight" else "/QtQuick/Controls/Fusion/"
    assert re.search(r'resolveType: .* "Controls.Button".*' + re.escape(style_path + "Button.qml"), trace), trace
    modules = ["Core/libholonight_core_qml.so", "Controls/libholonight_controls_qml.so",
               "impl/libholonight_impl_qml.so"]
    if expected_style == "Holonight":
        modules.append("libholonight_qml.so")
    else:
        assert "/Holonight/libholonight_qml.so" not in mappings, "Fusion loaded the HoloNight style plugin"
    for module in modules:
        suffix = "/Holonight/" + module
        paths = [line.split()[-1] for line in mappings.splitlines() if suffix in line]
        expected = str(args.qml_root.resolve()) + suffix
        assert paths and all(path == expected for path in paths), (expected, paths)
    assert not (root / "appearance.toml").exists(), "Startup wrote appearance configuration"
    print(f"{args.executable.name}: {args.mode} selects {expected_style}; QML and plugin origins verified")
