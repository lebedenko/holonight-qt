#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
lane=$1
mkdir /work/holonight-qt
cp -a /input/. /work/holonight-qt/
cd /work/holonight-qt
export HOME=/work/build/home LC_ALL=C.UTF-8 TZ=UTC
export XDG_RUNTIME_DIR=/work/runtime QT_FORCE_STDERR_LOGGING=1
export QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software QSG_RHI_BACKEND=software
export QT_QPA_PLATFORMTHEME= QT_STYLE_OVERRIDE=
export JOBS=${JOBS:-2}
mkdir -p "$HOME"
mkdir -m 700 "$XDG_RUNTIME_DIR"
if [ "$lane" = licensing ]; then
    reuse --version
    reuse lint
    exit
fi
if [ "$lane" = static ]; then
    # Canonical formatting and tidy enumerate tracked files in this disposable copy.
    git init -q
    git add .
fi
python3 --version
cmake --version
ninja --version
c++ --version
pkg-config --modversion Qt6Core
if [ "$lane" = prepare ]; then
    cp -a /work/holonight-qt /prepared/consumer
    cd /prepared/consumer
    export HOLONIGHT_CONFIG_SOURCE=/prepared/config
    task deps PRESET=test
    cp build/deps/provider-state.json /prepared/provider-state.json
    exit
fi
export HOLONIGHT_DEPENDENCY_PREFIX=/prepared/consumer/build/deps/prefix
export LD_LIBRARY_PATH="$HOLONIGHT_DEPENDENCY_PREFIX/lib"
test -f /prepared/provider-state.json
python3 scripts/ci/test_launcher.py
python3 scripts/ci/test_dependencies.py
case "$lane" in
  full|static)
    export HOLONIGHT_BUILD_FEATURES='BUILD_WAYLAND=ON BUILD_SYNTAX_THEMES=ON BUILD_QT5_PROBES=ON'
    task build PRESET=test
    if [ "$lane" = full ]; then
        ctest --test-dir build/test --output-on-failure --no-tests=error
        cat build/test/Testing/Temporary/LastTest.log
    else
        task format-check
        task tidy
        cmake --build build/test --target all_qmllint
        python3 tooling/qml_policy.py
        python3 tooling/workflow.py qmltypes-check --preset test
    fi
    ;;
  minimal)
    export HOLONIGHT_BUILD_FEATURES='BUILD_WAYLAND=OFF BUILD_SYNTAX_THEMES=OFF BUILD_QT5_PROBES=OFF'
    task build PRESET=minimal
    ctest --test-dir build/minimal --output-on-failure --no-tests=error
    cat build/minimal/Testing/Temporary/LastTest.log
    ;;
  release)
    task build PRESET=release
    cmake --install build/release --prefix /work/install
    ;;
  *) echo "Unknown lane: $lane" >&2; exit 2 ;;
esac
