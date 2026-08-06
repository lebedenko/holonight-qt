# SDD Tasks — holonight-style-shapes

- [x] T-001: Add a private QPainter path adapter for `ResolvedShape`
  - REQs: REQ-F-004
  - Check: All shape kinds and corner masks produce a closed, valid path with clamped geometry.

- [x] T-002: Replace local ordinary-control radius sources with `ShapeResolver`
  - REQs: REQ-F-001
  - Check: Ordinary controls remain rounded at the existing default and small controls clamp safely.

- [x] T-003: Migrate menu frame and panel primitives
  - REQs: REQ-F-002
  - Check: Fill and border share the semantic Menu path and preserve existing colors.

- [x] T-004: Migrate tooltip panel rendering
  - REQs: REQ-F-003
  - Check: Tooltip fill and border share the semantic Tooltip path and preserve existing colors.

- [x] T-005: Add focused pixel-level regression tests
  - REQs: REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004
  - Check: Tests distinguish rounded and chamfered signature corners and cover small geometry.

- [x] T-006: Run focused and full verification
  - REQs: all
  - Check: Style targets build; focused and complete primitive tests pass offscreen; full CTest remains green.
