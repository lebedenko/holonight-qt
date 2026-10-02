#!/bin/sh
set -eu
lane=$1
mkdir /work/source
cp -a /input/. /work/source/
cd /work/source
export HOME=/work/build/home LC_ALL=C.UTF-8 TZ=UTC
mkdir -p "$HOME"
case "$lane" in
  licensing)
    reuse --version
    reuse lint
    ;;
  build-test)
    export XDG_RUNTIME_DIR=/work/runtime QT_FORCE_STDERR_LOGGING=1
    mkdir -m 700 "$XDG_RUNTIME_DIR"
    python3 --version
    git --version
    cmake --version
    ninja --version
    c++ --version
    clang-format --version
    clang-tidy --version
    pkg-config --modversion Qt6Core tomlplusplus
    python3 scripts/ci/test_launcher.py
    python3 scripts/ci/prepare-tools.py
    export PATH=/work/tools/usr/bin:$PATH
    export LD_LIBRARY_PATH=/work/tools/usr/lib
    rg --version
    patchelf --version
    /work/tools/usr/bin/qmake -query QT_VERSION
    test ! -e /usr/include/holonight/config/config.h
    test ! -e /usr/local/include/holonight/config/config.h
    config_revision=fe69a59e6b73167fd5349223a4d265d75386c139
    config_source=/work/providers/config
    git init "$config_source"
    git -C "$config_source" remote add origin https://github.com/lebedenko/holonight-config.git
    git -C "$config_source" fetch --depth=1 origin "$config_revision"
    git -C "$config_source" checkout --detach FETCH_HEAD
    test "$(git -C "$config_source" rev-parse HEAD)" = "$config_revision"
    cmake -S "$config_source" -B /work/providers/config-build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
    cmake --build /work/providers/config-build --parallel
    cmake --install /work/providers/config-build --prefix /work/providers/prefix
    export LD_LIBRARY_PATH=/work/providers/prefix/lib:/work/tools/usr/lib
    cmake -S . -B build/verification -G Ninja -DCMAKE_BUILD_TYPE=Release \
      '-DCMAKE_PREFIX_PATH=/work/providers/prefix;/work/tools/usr' \
      -DBUILD_TESTS=ON -DBUILD_DEMO=ON -DBUILD_CONTROLS_GALLERY=ON -DBUILD_QT5_PROBES=ON
    cmake --build build/verification --parallel
    cat > build/verification/qt5/qt.conf <<'QTPATHS'
[Paths]
Prefix=/work/tools/usr
Plugins=lib/qt/plugins
Libraries=lib
QTPATHS
    ctest --test-dir build/verification --output-on-failure --no-tests=error
    cmake --build build/verification --target format-check
    cmake --build build/verification --target tidy-src
    ;;
  *) echo "Unknown lane: $lane" >&2; exit 2 ;;
esac
