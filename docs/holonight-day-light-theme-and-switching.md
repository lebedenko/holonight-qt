Yes. Your current `ColorTokens` shape is already suitable for light/dark because it is semantic: `background`, `surface`, `textPrimary`, `borderSubtle`, `primary`, `error`, ANSI colors, etc. The header currently exposes `darkTokens()` and `buildPalette(const ColorTokens&)`, so I’d add `lightTokens()` beside it rather than creating a separate struct. ([GitHub][1])

## Suggested light palette

```cpp
[[nodiscard]] ColorTokens lightTokens()
{
    ColorTokens t;

    // Surfaces
    t.background      = QColor("#f4f7fb");
    t.surface         = QColor("#edf2f8");
    t.surfaceElevated = QColor("#ffffff");
    t.surfaceRaised   = QColor("#e6edf6");
    t.surfaceHover    = QColor("#dbe7f4");
    t.surfaceInverse  = QColor("#151827");

    // Text
    t.textPrimary   = QColor("#1f2335");
    t.textSecondary = QColor("#3b4261");
    t.textMuted     = QColor("#6b7394");
    t.textDisabled  = QColor("#a7aec4");
    t.textInverse   = QColor("#c0caf5");

    // Primary accent
    t.primary        = QColor("#2f6fe4");
    t.primaryHover   = QColor("#008fc7");
    t.primaryPressed = QColor("#2757b8");
    t.onPrimary      = QColor("#ffffff");

    // Borders
    t.borderSubtle  = QColor("#d6deec");
    t.borderPassive = QColor("#aeb9cf");
    t.borderStrong  = QColor("#7f8dad");
    t.borderHover   = QColor("#008fc766");
    t.borderFocus   = QColor("#00a6d6");
    t.borderActive  = QColor("#2f6fe4");
    t.borderUrgent  = QColor("#d93f5c");

    // Effects
    t.glowCyanSoft   = QColor("#00a6d633");
    t.glowBlueSoft   = QColor("#2f6fe433");
    t.glowVioletSoft = QColor("#7b61d133");
    t.scrim          = QColor("#1f233599");
    t.glassTint      = QColor("#ffffffcc");

    t.hoverOverlay    = QColor("#00a6d61a");
    t.pressedOverlay  = QColor("#00a6d626");
    t.disabledOverlay = QColor("#f4f7fb80");
    t.focusRing       = QColor("#00a6d6");
    t.shadow          = QColor("#000000");

    // Status
    t.error   = QColor("#d93f5c");
    t.warning = QColor("#c47a1c");
    t.success = QColor("#4d8f31");
    t.onError = QColor("#ffffff");

    // Accents
    t.accentCyan   = QColor("#008fc7");
    t.accentBlue   = QColor("#2f6fe4");
    t.accentViolet = QColor("#7b61d1");
    t.accentYellow = QColor("#b87900");

    // Workspace pills
    t.workspaceOccupied = QColor("#e3eaf5");
    t.workspaceActive   = QColor("#d7e4f6");

    // ANSI
    t.ansiBlack   = QColor("#d6deec");
    t.ansiRed     = QColor("#d93f5c");
    t.ansiGreen   = QColor("#4d8f31");
    t.ansiYellow  = QColor("#b87900");
    t.ansiBlue    = QColor("#2f6fe4");
    t.ansiMagenta = QColor("#7b61d1");
    t.ansiCyan    = QColor("#008fc7");
    t.ansiWhite   = QColor("#1f2335");

    t.ansiBrightBlack   = QColor("#8c96ad");
    t.ansiBrightRed     = QColor("#b91f3d");
    t.ansiBrightGreen   = QColor("#36751f");
    t.ansiBrightYellow  = QColor("#946200");
    t.ansiBrightBlue    = QColor("#1e55c8");
    t.ansiBrightMagenta = QColor("#6648c3");
    t.ansiBrightCyan    = QColor("#0076a6");
    t.ansiBrightWhite   = QColor("#10131f");

    // Compatibility aliases
    t.surfaceVariant     = t.surface;
    t.surfaceContainer   = t.surfaceElevated;
    t.onSurface          = t.textPrimary;
    t.onSurfaceVariant   = t.textMuted;
    t.onSurfaceDisabled  = t.textDisabled;
    t.onSurfaceInverse   = t.textInverse;
    t.secondary          = t.surfaceRaised;
    t.onSecondary        = t.textPrimary;
    t.outline            = t.borderPassive;
    t.outlineVariant     = t.borderSubtle;
    t.hover              = t.hoverOverlay;
    t.pressed            = t.pressedOverlay;
    t.textSubtle         = t.textSecondary;

    t.ansi0  = t.ansiBlack;
    t.ansi1  = t.ansiRed;
    t.ansi2  = t.ansiGreen;
    t.ansi3  = t.ansiYellow;
    t.ansi4  = t.ansiBlue;
    t.ansi5  = t.ansiMagenta;
    t.ansi6  = t.ansiCyan;
    t.ansi7  = t.ansiWhite;
    t.ansi8  = t.ansiBrightBlack;
    t.ansi9  = t.ansiBrightRed;
    t.ansi10 = t.ansiBrightGreen;
    t.ansi11 = t.ansiBrightYellow;
    t.ansi12 = t.ansiBrightBlue;
    t.ansi13 = t.ansiBrightMagenta;
    t.ansi14 = t.ansiBrightCyan;
    t.ansi15 = t.ansiBrightWhite;

    return t;
}
```

## Header change

```cpp
enum class ColorMode {
    Dark,
    Light,
};

[[nodiscard]] ColorTokens darkTokens();
[[nodiscard]] ColorTokens lightTokens();
[[nodiscard]] ColorTokens tokensForMode(ColorMode mode);
[[nodiscard]] QPalette buildPalette(const ColorTokens& tokens);
```

Implementation:

```cpp
ColorTokens tokensForMode(ColorMode mode)
{
    switch (mode) {
    case ColorMode::Dark:
        return darkTokens();
    case ColorMode::Light:
        return lightTokens();
    }

    return darkTokens();
}
```

## Code organization I’d use

```text
palette/
  holonight/
    color_mode.h
    color_tokens.h
    dark_tokens.cpp
    light_tokens.cpp
    palette_builder.cpp
    palette.h
```

Or, simpler for now:

```text
palette/holonight/
  palette.h
  dark_palette.cpp
  light_palette.cpp
  palette.cpp
```

Keep `buildPalette()` mode-agnostic. It should only consume `ColorTokens`. Do not put dark/light branches inside `buildPalette()` unless absolutely necessary.

## Shell switching model

In `holonight-shell`, store user intent:

```toml
[appearance]
mode = "system" # dark | light | system
```

Resolve it:

```cpp
ColorMode ThemeManager::resolvedMode() const
{
    if (config.mode == "dark")
        return ColorMode::Dark;

    if (config.mode == "light")
        return ColorMode::Light;

    return systemPrefersDark() ? ColorMode::Dark : ColorMode::Light;
}
```

Then apply:

```cpp
const auto tokens = Holonight::tokensForMode(resolvedMode());

qApp->setPalette(Holonight::buildPalette(tokens));
themeSingleton->setTokens(tokens);
```

For live reload, emit one signal from the shell-side theme singleton:

```cpp
Q_EMIT tokensChanged();
```

Then QML should bind to semantic properties, not hardcoded colors.

## Main rule

Dark and light should differ only in **token values**, not in component logic. Your QML should not ask:

```qml
Theme.mode === "dark" ? darkColor : lightColor
```

It should just use:

```qml
Theme.colors.surface
Theme.colors.textPrimary
Theme.colors.borderSubtle
```

[1]: https://github.com/lebedenko/holonight-qt/blob/main/palette/holonight/palette.h "holonight-qt/palette/holonight/palette.h at main · lebedenko/holonight-qt · GitHub"
