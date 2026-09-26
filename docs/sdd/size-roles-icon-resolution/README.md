# Extra-small controls and icon-theme resolution

Status: Locally verified; publication pending. Baseline: `0d5bdf094e6e759210531c4babdd2023e5d56485`.

## Contract

- `HnControlSize.Xs` is appended as value 4. Compact, Normal, Large, and Hero retain values 0–3.
- Xs controls use 24 px height, 16 px icons, 6 px horizontal padding, and 4 px internal spacing.
- `HnMetrics.headerHeightForSize(role)` returns 42, 48, 56, 64, or 72 px for Xs through Hero; invalid roles normalize to Normal. `headerHeight` remains 56 px.
- `HnHeaderBar.sizeRole` defaults to Normal and fixes its height and layout limits to the role height.
- The platform icon engine resolves a name in the selected theme before inherited themes, including icon themes with slash-separated directory sections and scaled directory metadata. It honors the platform theme without a caller setting `QIcon::themeName()`.

## Implementation and verification

`palette/metrics.cpp`, `palette/holonight/metrics.h`, `qml/`, and `src/icons/iconthemeresolver.cpp` own the behavior. Metric, QML smoke, and platform bootstrap tests cover the contract. The platform bootstrap fixture has a primary `folder` and inherited fallback, and the installed-theme offscreen probe checks actual HoloNight folder rendering.

Verification on 2026-09-26: focused metric, header, Switch, icon, and bootstrap tests passed. `task verify` completed build and lint; 97 of 103 CTests passed in the sandbox, where six OpenGL tests could not create a context. Those six passed outside the sandbox. The installed-theme probe returned `theme=HoloNight-Dark folder=#131e2c` after the fix, versus Papirus cyan `#00bcd4` before it. REUSE lint and C++ formatting passed.
