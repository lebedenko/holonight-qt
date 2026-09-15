#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
"""Exercise real provider requests on private headless Sway outputs; no input automation."""
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import time

binary = Path(sys.argv[1]).resolve()
logs = Path(sys.argv[2]).resolve()
logs.mkdir(parents=True, exist_ok=True)
failures = []
for output_scale in (1, 2):
    runtime = Path(tempfile.mkdtemp(prefix="uqc215-"))
    env = dict(os.environ, XDG_RUNTIME_DIR=str(runtime), WLR_BACKENDS="headless",
               WLR_RENDERER="pixman", WLR_LIBINPUT_NO_DEVICES="1")
    for key in ("DISPLAY", "WAYLAND_DISPLAY", "SWAYSOCK", "HYPRLAND_INSTANCE_SIGNATURE",
                "QT_SCALE_FACTOR", "QT_SCREEN_SCALE_FACTORS", "QT_QPA_PLATFORMTHEME",
                "QT_PLUGIN_PATH", "QML_IMPORT_PATH", "LD_PRELOAD"):
        env.pop(key, None)
    config = runtime / "sway.conf"
    config.write_text(f"output HEADLESS-1 mode {1920 * output_scale}x{1080 * output_scale} scale {output_scale}\n")
    with (logs / f"sway-{output_scale}.log").open("w") as log:
        compositor = subprocess.Popen(["sway", "--config", str(config)], env=env,
                                      stdout=log, stderr=subprocess.STDOUT)
        try:
            sockets = []
            for _ in range(100):
                sockets = [p for p in runtime.glob("wayland-*") if p.is_socket()]
                if sockets or compositor.poll() is not None:
                    break
                time.sleep(.05)
            if not sockets:
                raise RuntimeError("headless compositor did not start")
            env.update(WAYLAND_DISPLAY=sockets[0].name, QT_QPA_PLATFORM="wayland",
                       QT_QPA_PLATFORMTHEME="", WAYLAND_DEBUG="1", QT_QUICK_BACKEND="software",
                       TEST_OUTPUT_SCALE=str(output_scale))
            for scale in (1, 1.25):
                result = subprocess.run([str(binary)], env=dict(env, QT_SCALE_FACTOR=str(scale)),
                                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=15)
                (logs / f"qt-{scale}-output-{output_scale}.log").write_text(result.stdout)
                expected = [f"set_size(0, {round(64 * scale)})",
                            f"set_size({round(400 * scale)}, {round(80 * scale)})",
                            f"set_size(0, {round(80 * scale)})",
                            f"set_margin({round(4 * scale)}, {round(8 * scale)}, 0, {round(12 * scale)})",
                            f"set_margin({round(-4 * scale)}, {round(16 * scale)}, {round(8 * scale)}, {round(20 * scale)})",
                            f"set_exclusive_zone({round(64 * scale)})", "set_exclusive_zone(-1)",
                            "set_exclusive_zone(0)", f"set_exclusive_zone({round(80 * scale)})",
                            f"add({round(4 * scale)}, {round(8 * scale)}, {round(100 * scale)}, {round(40 * scale)})",
                            f"add({round(8 * scale)}, {round(4 * scale)}, {round(200 * scale)}, {round(60 * scale)})"]
                expected += [f"set_size({round(440 * scale)}, 0)", "set_size(0, 120)", "set_margin(-6, 24, 12, 30)",
                             "set_exclusive_zone(120)", "add(5, 8, 151, 61)"]
                expected += ["add(3, 5, 101, 41)" if scale == 1 else "add(4, 6, 126, 52)"]
                missing = [request for request in expected if request not in result.stdout]
                # A configure feedback loop must not be hidden by correct sampled sizes.
                configures = len(re.findall(r"zwlr_layer_surface_v1[#@]\d+\.configure\(", result.stdout))
                passed = result.returncode == 0 and not missing and 1 <= configures <= 12
                print(f"Qt={scale} output={output_scale}: exit={result.returncode}, configures={configures}, missing={missing}", flush=True)
                if not passed:
                    failures.append((scale, output_scale))
        finally:
            compositor.terminate()
            try:
                compositor.wait(timeout=5)
            except subprocess.TimeoutExpired:
                compositor.kill()
                compositor.wait()
if failures:
    raise SystemExit(f"Failed coordinate cases: {failures}")
