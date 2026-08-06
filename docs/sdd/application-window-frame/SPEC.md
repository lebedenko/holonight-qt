# SPEC: HnApplicationWindow

**Feature:** Shared HoloNight application-window layout
**Status:** Implemented
**Date:** 2026-07-23

## Scope

Add a public QML window component that gives HoloNight applications a consistent client background, optional
application header, and padded content area. The compositor remains responsible for the physical outer window
border, title bar, shadow, rounding, movement, resizing, and native window controls.

## Requirements

- `HnApplicationWindow` shall be exported by both `Holonight` and the lowercase compatibility module.
- It shall remain an ordinarily decorated top-level Qt window and shall not set frameless flags.
- It shall not draw an inner window frame or border that duplicates compositor decoration.
- It shall provide an optional application header with leading, center, and trailing content slots.
- The application header shall be hidden by default.
- When no custom center content is supplied, the window title shall be displayed.
- Consumer content shall be parented into a client-content item below the visible application header.
- Application-header height and content padding shall be configurable with non-negative effective values.
- The component shall not set `Qt.FramelessWindowHint`.

## Constraints

- No shell imports, layer-shell behavior, blur, glow, drag protocol, or compositor-specific API.
- No built-in minimize, maximize, or close buttons; those remain compositor decoration responsibilities.
- No attempt to alter the physical window or input-region shape.

## Acceptance

The component loads through both module spellings, exposes stable content slots, responds to application-header
visibility and sizing, retains normal compositor decoration, and passes focused and complete QML smoke suites.
