# SPEC: Shared list and action controls

**Feature:** Reusable icon actions, menu icons, and elided navigation titles

**Status:** Implemented

**Date:** 2026-07-29

## Context

Conversation-list work in `holonight-ai` demonstrated reusable presentation contracts that belong in
HoloNight rather than in one application. The application retains conversation activation, rename, delete,
and confirmation state. This feature moves only generic controls, assets, and disclosure behavior.

## Requirements

### REQ-F-001: Shared action icons

`Holonight.Controls` shall package monochrome `more-vertical`, `edit`, `delete`, and `folder` SVG assets at
stable `qrc:/qt/qml/Holonight/Controls/assets/<name>.svg` URLs. The assets shall be compatible with semantic
recoloring by `HnIcon`.

### REQ-F-002: Public icon-only button

`Holonight.Controls` shall expose `HnIconButton`, based on `AbstractButton`, using the standard `icon` group.
It shall default to `HnControlSize.Normal`, normalize invalid size roles, remain square at the semantic control
height, and render a centered icon at the matching semantic icon size.

The normal surface and border shall be transparent. Hover shall use `surfaceHover`, press shall use `surface`,
and visual keyboard focus shall use `borderFocus` without adding an idle border. Disabled and caller-supplied
icon colors shall compose through `HnIcon`. Background colors shall transition over 80 ms. Standard keyboard,
pointer, signal, focus, and accessibility behavior shall be preserved.

### REQ-F-003: Styled menu-item icons

The HoloNight `MenuItem` style shall render `MenuItem.icon.source` without a consumer-provided content item.
Checkmarks and icons shall occupy separate leading columns, labels shall remain aligned in mixed menus, and
submenu arrows shall retain a separate trailing column. Leading icons shall use the Compact semantic icon
metric. Disabled, pressed, highlighted, hovered, and normal colors shall remain legible and consistent.

### REQ-F-004: Elided navigation-title disclosure

`HnNavigationDelegate` shall expose `showTitleToolTipWhenElided`, defaulting to `true`. While hovered, its
complete non-empty title shall be shown through the shared tooltip only when the internal title label reports
actual truncation. Disabling the property shall suppress the tooltip. The tooltip shall not intercept delegate
activation, and the full title shall remain the accessible name.

### REQ-I-001: Packaging

New public QML and asset files shall be present in build-tree metadata, install-tree files, and public type
inventory. Existing module imports and APIs shall remain compatible.

### REQ-I-002: Examples and documentation

The controls gallery shall demonstrate Compact, Normal, Large, disabled, pointer-state, and keyboard-focus
icon buttons; icon, plain, checkable, and submenu menu items; and a deliberately elided navigation title.
The shared-controls usage guide shall document the public APIs and resource URLs.

### REQ-V-001: Automated contracts

QML smoke tests shall cover icon-button metrics, state colors, custom icon color, accessibility, menu icon and
check columns, menu state colors, tooltip eligibility, and source-tree construction. Package-install tests
shall cover the new type and assets.

## Out of scope

- Application rename/delete state machines and hover-driven metadata replacement.
- `HnEditableNavigationDelegate`.
- A destructive menu-item role.
- Expansion into a general icon library.
- Downstream `holonight-ai` migration.
