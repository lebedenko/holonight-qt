# Scope review after manual discovery

Approved by the user on 2026-09-07. The decisions below settle the UQC-002 scope; provider implementation starts
only after the discovery handoff is published and the umbrella acceptance checkpoint is committed.

## Approved decisions for UQC-002

1. Include CheckBox, RadioButton and Switch sizing corrections in provider work. Reserve indicator dimensions when
   callers remove content/padding; preserve ordinary labeled controls and supported Switch size roles. Add geometry
   regressions with adjacent labels and trailing switches, including mirrored layout. The issue also affects the
   compared fallback compositions, so record it as a deliberate provider compatibility improvement.
2. Treat the license popup as an explicitly resolved application/Kirigami composition limitation for this initiative.
   Do not add global popup padding or alter all ScrollViews to disable horizontal scrolling. The reduced composition
   needs viewport-bounded content to wrap under HoloNight, Fusion and Basic alike. Keep horizontal scrolling valid
   for applications that require it. Upstream reporting would be separate work and needs full application comparison.
3. Extend the standard-control contract to respect application palette overrides. Accepted boundary: standard
   Qt Quick controls consume appropriate Qt palette roles, with HoloNight session defaults supplying the normal
   theme. Keep explicit HoloNight Core/composite appearance APIs governed by appearance configuration. This changes current standard-control color behavior and needs role/state mapping,
   default-theme parity checks, and dark/light round-trip verification. Do not mutate shared desktop configuration
   when an application selects its own scheme.
4. Implement all nine approved missing controls: ApplicationWindow, Label, ToolButton,
   ToolBar, ToolSeparator, MenuSeparator, Popup, MenuBar and MenuBarItem. Respect Haruna's requested Fusion fallback and application-painted sliders.
5. Do not automatically expand to Dialog/DialogButtonBox/Page/Pane/etc. The observed license dialog does not justify
   a new Dialog implementation as a clipping fix. Resolve any remaining required logged-out surfaces individually;
   unreported server-form/keyboard/state coverage must not be marked tested.

## Evidence and remaining gates

[APPLICATIONS.md](APPLICATIONS.md) records manual observations, authentication cancellation clarification, source
references, type origins, quantitative comparisons and their limits. The controlled probe uses a 400-pixel viewport
and scale 1, and exercises the application palette directly; final named-scheme and full-dialog checks remain
implementation/integration work where those behaviors are accepted.

Provider composite API recommendations in DESIGN are accepted with public APIs preserved. Consumer adoption still depends on a published,
pinned provider. Joint coverage and palette-boundary review is complete. Publish this discovery handoff before recording umbrella
acceptance and activating UQC-101. Unobserved states stay explicit UQC-201 gates, not discovery pass claims.

## Reproduce the controlled comparison

From the provider root, using the existing prepared provider prefix:

```sh
cmake -S docs/sdd/unified-qtquick-controls/audit -B /tmp/uqc-20260907-discovery-build -G Ninja
cmake --build /tmp/uqc-20260907-discovery-build --parallel 4
python3 docs/sdd/unified-qtquick-controls/audit/check-layout.py \
  /var/tmp/uqc-auth-test-20260907/prefix /tmp/uqc-20260907-discovery-build/uqc-layout-probe
```

The runner prints the temporary log directory. It runs Holonight/Fusion/Basic, each with unconstrained and
viewport-constrained content, in fresh empty XDG profiles. No desktop input or configuration is changed.
