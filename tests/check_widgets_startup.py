# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
"""Bounded Widgets startup and plugin-origin checks, independent of host plugins."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

executable, plugin_root, mode = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[3]
with tempfile.TemporaryDirectory(prefix="holonight-widgets-") as directory:
    root = Path(directory)
    env = os.environ.copy()
    for key in ("QT_PLUGIN_PATH", "QT_QPA_PLATFORMTHEME", "QT_STYLE_OVERRIDE"):
        env.pop(key, None)
    env.update(QT_QPA_PLATFORM="offscreen", QT_FORCE_STDERR_LOGGING="1",
               HOLONIGHT_APPEARANCE_FILE=str(root / "missing.toml"), XDG_RUNTIME_DIR=str(root))
    command = [str(executable.resolve())]
    expected_style = "holonight" if mode == "default" else "fusion"
    if mode == "environment":
        env["QT_STYLE_OVERRIDE"] = "Fusion"
    elif mode == "command-line":
        env["QT_STYLE_OVERRIDE"] = "holonight"
        command += ["-style", "Fusion"]
    elif mode == "platform-environment":
        env["QT_QPA_PLATFORMTHEME"] = ""
    process = subprocess.Popen(command, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    try:
        try:
            output, _ = process.communicate(timeout=2)
        except subprocess.TimeoutExpired:
            mappings = Path(f"/proc/{process.pid}/maps").read_text()
            process.terminate()
            output, _ = process.communicate(timeout=5)
        else:
            raise AssertionError(f"Exited at startup ({process.returncode}): {output.decode()}")
    finally:
        if process.poll() is None:
            process.kill()
            process.communicate()
    trace = output.decode()
    assert f"Active Widgets style: {expected_style}" in trace, trace
    diagnostics = [line for line in trace.splitlines() if not line.startswith("Active Widgets style:")
                   and line != "This plugin does not support propagateSizeHints()"]
    assert not diagnostics, diagnostics
    modules = [] if mode == "platform-environment" else ["platformthemes/libqholonight.so"]
    if expected_style == "holonight":
        modules.append("styles/libholonight.so")
    else:
        assert "/styles/libholonight.so" not in mappings, mappings
    for module in modules:
        paths = [line.split()[-1] for line in mappings.splitlines() if line.split()[-1].endswith("/" + module)]
        expected = str(plugin_root.resolve() / module)
        assert paths and all(path == expected for path in paths), (expected, paths)
    if mode == "platform-environment":
        assert "/platformthemes/libqholonight.so" not in mappings
    assert not (root / "missing.toml").exists()
    print(f"{executable.name}: {mode} selects {expected_style}; plugin origins verified")
