# HoloNight Typography Architecture & Usage Guide

## Overview

HoloNight provides a unified, semantically structured typography system designed for desktop applications and system workspace decorators (HoloNight Shell).

This document details the architectural decisions, font size unit policies, the 4-font family role hierarchy, configuration options, and component usage.

---

## Architectural Decision: Font Size Units (`pt` vs `px`)

> [!IMPORTANT]
> **Strict Policy**: All HoloNight components and HoloNight ecosystem applications **MUST strictly use `font.pointSize` (`pt`)** derived from `HolonightTheme` point size tokens or `HnLabel.role`. Using `font.pixelSize` (`px`) is strictly **prohibited**.

### Why `font.pointSize` (`pt`) is Mandatory

1. **Automatic DPI & Accessibility Scaling**:
   - In Qt/QML, `1 pt` represents 1/72 of an inch and automatically scales with Qt's logical DPI engine (`QPaintDevice::logicalDpiX()`), fractional display scaling (e.g. 125%, 150%, 200% on Wayland/KDE/Hyprland), and user accessibility font scale settings.
   - `font.pixelSize` (`px`) specifies raw device pixels. On High-DPI displays or when fractional scaling is active, fixed pixel sizes fail to scale proportionally with system fonts.

2. **Why Mixing `px` and `pt` is Prohibited**:
   - When one component specifies `font.pointSize: 12` (which scales with display DPI) and an adjacent component specifies `font.pixelSize: 12` (which does not scale with font DPI), their relative proportions collapse whenever display scaling or font size preferences change.
   - Mixing units causes baseline misalignments, text truncation, and container height clipping.

---

## The 4-Font Family Architecture

HoloNight Shell decorates a desktop workspace and requires distinct typographic character for UI elements, technical content, micro headers, and hero numbers/clocks.

| Role | Property (`HolonightTheme`) | Primary Purpose | Default / Fallback Cascade |
| --- | --- | --- | --- |
| **Interface Font** | `uiFont` | Standard UI controls, buttons, list delegates, form text. | `"Inter"` $\rightarrow$ `"Noto Sans"` $\rightarrow$ System Sans-Serif |
| **Monospace Font** | `fixedFont` | Code snippets, terminal, file paths, keyhint badges. | `"JetBrains Mono"` $\rightarrow$ `"monospace"` |
| **Micro-Header Font** | `headerFont` | Tracked uppercase category headers (`// CALENDAR`, `BEST MATCH`). | `header_font` $\rightarrow$ `uiFont` |
| **Display / Hero Font** | `displayFont` | Tab bar clock, weather temp (`14°C`), hero numbers, big widgets. | `display_font` $\rightarrow$ `uiFont` |

---

## Typography Scale Tokens

Font sizes are derived dynamically from `baseFontSize` (default: 10pt) and `fixedFontSize` (default: 10pt) in `ThemeConfig`:

| Size Role | Property (`HolonightTheme`) | Formula | Point Size (at 10pt base) | Usage |
| --- | --- | --- | --- | --- |
| **Display** | `displaySize` | `base + 14` | 24 pt | Clock displays, weather temp (`14°C`), hero numbers |
| **Heading** | `headingSize` | `base + 6` | 16 pt | Main window / page titles ("Appearance") |
| **App Title** | `appTitleSize` | `base + 4` | 14 pt | Brand app bar headers ("HoloNight Settings") |
| **Title** | `titleSize` | `base + 3` | 13 pt | Card titles, group section headers |
| **Subheading** | `subheadingSize` | `base + 1` | 11 pt | Setting labels, card subtitles |
| **Body** | `bodySize` | `base` | 10 pt | Regular body text, inputs, buttons |
| **Caption** | `captionSize` | `base - 1` | 9 pt | Muted subtext, metadata, timestamps |
| **Micro Header** | `microSize` | `base - 1` | 9 pt | Tracked all-caps headers (`// CALENDAR`) |
| **Code** | `fixedFontSize` | `fixed_font_size` | 10 pt | Monospace code, keyhint badges, terminal |

---

## Configuration Tiering

### 1. User-Facing Settings GUI (Tier 1)
Exposes simple, essential readability controls:
- **Interface font** (`uiFont`)
- **Interface font size** (`baseFontSize`, range 8–18pt)
- **Monospace font** (`fixedFont`)
- **Monospace font size** (`fixedFontSize`, range 8–18pt)

### 2. Advanced `config.toml` & Environment Variables (Tier 2)
Desktop power users and theme creators can override specialized shell fonts in `~/.config/holonight/config.toml`:

```toml
[fonts]
ui = "Inter"
fixed = "JetBrainsMono Nerd Font"
header = "Space Grotesk"
display = "Outfit"
baseSize = 12
fixedSize = 14
```

Environment variables:
- `HOLONIGHT_FONT` or `HOLONIGHT_UI_FONT`
- `HOLONIGHT_FIXED_FONT`
- `HOLONIGHT_HEADER_FONT`
- `HOLONIGHT_DISPLAY_FONT`
- `HOLONIGHT_FONT_SIZE` or `HOLONIGHT_BASE_FONT_SIZE`
- `HOLONIGHT_FIXED_FONT_SIZE` or `HOLONIGHT_FIXED_SIZE`

---

## QML Usage (`HnLabel`)

The recommended way to render text in HoloNight Quick Controls is `HnLabel`:

```qml
import QtQuick
import Holonight.Core

// Standard Body Text
HnLabel {
    role: HnTypographyRole.Body
    text: "Standard interface body text"
}

// Tracked Micro-Header with accent prefixing
HnLabel {
    role: HnTypographyRole.MicroHeader
    rawText: "CALENDAR"
    showPrefix: true // Displays "// CALENDAR" with accent-tinted prefix
}

// Hero Display Number
HnLabel {
    role: HnTypographyRole.Display
    text: "14°C"
}

// Monospace Code Label
HnLabel {
    role: HnTypographyRole.Code
    text: "~/.config/holonight/config.toml"
}
```
