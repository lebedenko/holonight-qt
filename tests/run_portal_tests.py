#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Use a private bus with no service activation directories."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

with tempfile.TemporaryDirectory(prefix="holonight-portal-") as directory:
    config = Path(directory) / "bus.conf"
    config.write_text("""<busconfig><type>session</type><listen>unix:tmpdir=/tmp</listen>
<auth>EXTERNAL</auth><policy context="default"><allow send_destination="*"/>
<allow receive_sender="*"/><allow own="*"/></policy></busconfig>""")
    selected = Path(directory) / "a é.md"
    selected.write_text("portal fixture")
    environment = dict(os.environ, QT_QPA_PLATFORM="offscreen", QT_QPA_PLATFORMTHEME="", QT_STYLE_OVERRIDE="", QT_FORCE_STDERR_LOGGING="1", PORTAL_TEST_FILE=str(selected))
    result = subprocess.run(["dbus-run-session", "--config-file", str(config), "--", *sys.argv[1:]],
                            env=environment, timeout=60, check=False)
    sys.exit(result.returncode)
