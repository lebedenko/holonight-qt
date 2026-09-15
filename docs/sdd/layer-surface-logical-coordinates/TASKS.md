# UQC-215 tasks

- [x] Verify canonical baseline and audit callers; settle logical coordinate contract.
- [x] Demonstrate real-backend regression failure before repair.
- [x] Convert requests/configure/regions and refresh scale changes.
- [x] Run focused and full verification; record limits and teardown disposition.
- [x] Prepare verified provider handoff; umbrella records publication before pinning.

Before repair: Qt scale 1 passes at output 1 and 2; Qt 1.25 fails both.
Real protocol requests and configure/window assertions fail; all processes exit
0 (pass) or 1 (GTest failure), with no teardown signal. Evidence retained in
umbrella `.cache/uqc215/before-corrected/`.

## Verification — 2026-09-15

- `cmake --build build -j4`: pass.
- `python3 tests/run_wayland_coordinates.py build/tests/holonight_wayland_coordinate_tests <logs>`:
  four combinations pass with normal exit 0, 11 configure events each. Qt scales
  1/1.25, output scales 1/2, repeated 1.5/back scale refresh, rounded region edges,
  zero-width bar/zero-height panel, reopen and orderly teardown pass. The runner
  uses private headless Sway; it never sends pointer, focus or keyboard input.
  Scale-change notifications are deterministic fixture events, not a physical
  monitor hotplug acceptance claim. Final evidence: umbrella `.cache/uqc215/reviewed/`.
- Focused `holonight_wayland_tests`: pass; full offscreen CTest: **87/87**,
  including `holonight_package_install_test` and runtime/startup matrices.
- `tidy-src` and direct clang-tidy for backend/test: complete with existing
  style/header warnings; introduced narrowing/cast warnings addressed. Format,
  QML import policy, Python syntax, diff whitespace and REUSE licensing pass.
- Rebuilt shell hosting tests against installed repaired static provider:
  **164/164** pass. Rebuilt AI panel tests against repaired provider: **11/11**
  pass in a private bus/profile. No consumer source changes.
- No system installation. The existing stack-application context teardown
  defect is separate (see SPEC); static-lifetime reduced probe exits normally.
- S01 human acceptance remains open: fractional Hyprland startup, hover/tooltip
  stability, right edge, popup placement, click alignment and reserved space.
  F05 and final initiative integration are not accepted by these checks.
