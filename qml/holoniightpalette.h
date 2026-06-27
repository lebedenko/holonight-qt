// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/palette.h"

#include <QColor>
#include <QObject>
#include <QtQml/qqml.h>

class HoloniightPalette : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(QColor background READ background NOTIFY paletteChanged)
  Q_PROPERTY(QColor surface READ surface NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceElevated READ surfaceElevated NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceRaised READ surfaceRaised NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceHover READ surfaceHover NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceInverse READ surfaceInverse NOTIFY paletteChanged)

  Q_PROPERTY(QColor textPrimary READ textPrimary NOTIFY paletteChanged)
  Q_PROPERTY(QColor textSecondary READ textSecondary NOTIFY paletteChanged)
  Q_PROPERTY(QColor textMuted READ textMuted NOTIFY paletteChanged)
  Q_PROPERTY(QColor textDisabled READ textDisabled NOTIFY paletteChanged)
  Q_PROPERTY(QColor textInverse READ textInverse NOTIFY paletteChanged)

  // Primary accent
  Q_PROPERTY(QColor primary READ primary NOTIFY paletteChanged)
  Q_PROPERTY(QColor primaryHover READ primaryHover NOTIFY paletteChanged)
  Q_PROPERTY(QColor primaryPressed READ primaryPressed NOTIFY paletteChanged)
  Q_PROPERTY(QColor onPrimary READ onPrimary NOTIFY paletteChanged)

  // Semantic borders
  Q_PROPERTY(QColor borderSubtle READ borderSubtle NOTIFY paletteChanged)
  Q_PROPERTY(QColor borderPassive READ borderPassive NOTIFY paletteChanged)
  Q_PROPERTY(QColor borderStrong READ borderStrong NOTIFY paletteChanged)
  Q_PROPERTY(QColor borderHover READ borderHover NOTIFY paletteChanged)
  Q_PROPERTY(QColor borderFocus READ borderFocus NOTIFY paletteChanged)
  Q_PROPERTY(QColor borderActive READ borderActive NOTIFY paletteChanged)
  Q_PROPERTY(QColor borderUrgent READ borderUrgent NOTIFY paletteChanged)

  Q_PROPERTY(QColor glowCyanSoft READ glowCyanSoft NOTIFY paletteChanged)
  Q_PROPERTY(QColor glowBlueSoft READ glowBlueSoft NOTIFY paletteChanged)
  Q_PROPERTY(QColor glowVioletSoft READ glowVioletSoft NOTIFY paletteChanged)
  Q_PROPERTY(QColor scrim READ scrim NOTIFY paletteChanged)
  Q_PROPERTY(QColor glassTint READ glassTint NOTIFY paletteChanged)
  Q_PROPERTY(QColor hoverOverlay READ hoverOverlay NOTIFY paletteChanged)
  Q_PROPERTY(QColor pressedOverlay READ pressedOverlay NOTIFY paletteChanged)
  Q_PROPERTY(QColor disabledOverlay READ disabledOverlay NOTIFY paletteChanged)
  Q_PROPERTY(QColor focusRing READ focusRing NOTIFY paletteChanged)
  Q_PROPERTY(QColor shadow READ shadow NOTIFY paletteChanged)

  // Semantic status
  Q_PROPERTY(QColor error READ error NOTIFY paletteChanged)
  Q_PROPERTY(QColor warning READ warning NOTIFY paletteChanged)
  Q_PROPERTY(QColor success READ success NOTIFY paletteChanged)
  Q_PROPERTY(QColor onError READ onError NOTIFY paletteChanged)

  // Design-system accents and secondary text
  Q_PROPERTY(QColor accentCyan READ accentCyan NOTIFY paletteChanged)
  Q_PROPERTY(QColor accentBlue READ accentBlue NOTIFY paletteChanged)
  Q_PROPERTY(QColor accentViolet READ accentViolet NOTIFY paletteChanged)
  Q_PROPERTY(QColor accentYellow READ accentYellow NOTIFY paletteChanged)

  // Workspace indicator fills
  Q_PROPERTY(QColor workspaceOccupied READ workspaceOccupied NOTIFY paletteChanged)
  Q_PROPERTY(QColor workspaceActive READ workspaceActive NOTIFY paletteChanged)

  Q_PROPERTY(int radiusTight READ radiusTight NOTIFY paletteChanged)
  Q_PROPERTY(int radiusControl READ radiusControl NOTIFY paletteChanged)
  Q_PROPERTY(int radiusPopup READ radiusPopup NOTIFY paletteChanged)
  Q_PROPERTY(int radiusPill READ radiusPill NOTIFY paletteChanged)
  Q_PROPERTY(int borderWidth READ borderWidth NOTIFY paletteChanged)
  Q_PROPERTY(int focusBorderWidth READ focusBorderWidth NOTIFY paletteChanged)
  Q_PROPERTY(int separatorWidth READ separatorWidth NOTIFY paletteChanged)
  Q_PROPERTY(int controlHeight READ controlHeight NOTIFY paletteChanged)
  Q_PROPERTY(int controlPadding READ controlPadding NOTIFY paletteChanged)

  Q_PROPERTY(QColor ansiBlack READ ansiBlack NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiRed READ ansiRed NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiGreen READ ansiGreen NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiYellow READ ansiYellow NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBlue READ ansiBlue NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiMagenta READ ansiMagenta NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiCyan READ ansiCyan NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiWhite READ ansiWhite NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightBlack READ ansiBrightBlack NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightRed READ ansiBrightRed NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightGreen READ ansiBrightGreen NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightYellow READ ansiBrightYellow NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightBlue READ ansiBrightBlue NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightMagenta READ ansiBrightMagenta NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightCyan READ ansiBrightCyan NOTIFY paletteChanged)
  Q_PROPERTY(QColor ansiBrightWhite READ ansiBrightWhite NOTIFY paletteChanged)

  // Deprecated aliases kept for compatibility.
  Q_PROPERTY(QColor surfaceVariant READ surfaceVariant NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceContainer READ surfaceContainer NOTIFY paletteChanged)
  Q_PROPERTY(QColor onSurface READ onSurface NOTIFY paletteChanged)
  Q_PROPERTY(QColor onSurfaceVariant READ onSurfaceVariant NOTIFY paletteChanged)
  Q_PROPERTY(QColor onSurfaceDisabled READ onSurfaceDisabled NOTIFY paletteChanged)
  Q_PROPERTY(QColor onSurfaceInverse READ onSurfaceInverse NOTIFY paletteChanged)
  Q_PROPERTY(QColor secondary READ secondary NOTIFY paletteChanged)
  Q_PROPERTY(QColor onSecondary READ onSecondary NOTIFY paletteChanged)
  Q_PROPERTY(QColor outline READ outline NOTIFY paletteChanged)
  Q_PROPERTY(QColor outlineVariant READ outlineVariant NOTIFY paletteChanged)
  Q_PROPERTY(QColor hover READ hover NOTIFY paletteChanged)
  Q_PROPERTY(QColor pressed READ pressed NOTIFY paletteChanged)
  Q_PROPERTY(QColor textSubtle READ textSubtle NOTIFY paletteChanged)

 public:
  explicit HoloniightPalette(QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(HoloniightPalette)

  Q_INVOKABLE void reload();

  [[nodiscard]] QColor background() const { return tok_.background; }
  [[nodiscard]] QColor surface() const { return tok_.surface; }
  [[nodiscard]] QColor surfaceElevated() const { return tok_.surfaceElevated; }
  [[nodiscard]] QColor surfaceRaised() const { return tok_.surfaceRaised; }
  [[nodiscard]] QColor surfaceHover() const { return tok_.surfaceHover; }
  [[nodiscard]] QColor surfaceInverse() const { return tok_.surfaceInverse; }
  [[nodiscard]] QColor textPrimary() const { return tok_.textPrimary; }
  [[nodiscard]] QColor textSecondary() const { return tok_.textSecondary; }
  [[nodiscard]] QColor textMuted() const { return tok_.textMuted; }
  [[nodiscard]] QColor textDisabled() const { return tok_.textDisabled; }
  [[nodiscard]] QColor textInverse() const { return tok_.textInverse; }
  [[nodiscard]] QColor primary() const { return tok_.primary; }
  [[nodiscard]] QColor primaryHover() const { return tok_.primaryHover; }
  [[nodiscard]] QColor primaryPressed() const { return tok_.primaryPressed; }
  [[nodiscard]] QColor onPrimary() const { return tok_.onPrimary; }
  [[nodiscard]] QColor borderSubtle() const { return tok_.borderSubtle; }
  [[nodiscard]] QColor borderPassive() const { return tok_.borderPassive; }
  [[nodiscard]] QColor borderStrong() const { return tok_.borderStrong; }
  [[nodiscard]] QColor borderHover() const { return tok_.borderHover; }
  [[nodiscard]] QColor borderFocus() const { return tok_.borderFocus; }
  [[nodiscard]] QColor borderActive() const { return tok_.borderActive; }
  [[nodiscard]] QColor borderUrgent() const { return tok_.borderUrgent; }
  [[nodiscard]] QColor glowCyanSoft() const { return tok_.glowCyanSoft; }
  [[nodiscard]] QColor glowBlueSoft() const { return tok_.glowBlueSoft; }
  [[nodiscard]] QColor glowVioletSoft() const { return tok_.glowVioletSoft; }
  [[nodiscard]] QColor scrim() const { return tok_.scrim; }
  [[nodiscard]] QColor glassTint() const { return tok_.glassTint; }
  [[nodiscard]] QColor hoverOverlay() const { return tok_.hoverOverlay; }
  [[nodiscard]] QColor pressedOverlay() const { return tok_.pressedOverlay; }
  [[nodiscard]] QColor disabledOverlay() const { return tok_.disabledOverlay; }
  [[nodiscard]] QColor focusRing() const { return tok_.focusRing; }
  [[nodiscard]] QColor error() const { return tok_.error; }
  [[nodiscard]] QColor warning() const { return tok_.warning; }
  [[nodiscard]] QColor success() const { return tok_.success; }
  [[nodiscard]] QColor onError() const { return tok_.onError; }
  [[nodiscard]] QColor shadow() const { return tok_.shadow; }
  [[nodiscard]] QColor accentCyan() const { return tok_.accentCyan; }
  [[nodiscard]] QColor accentBlue() const { return tok_.accentBlue; }
  [[nodiscard]] QColor accentViolet() const { return tok_.accentViolet; }
  [[nodiscard]] QColor accentYellow() const { return tok_.accentYellow; }
  [[nodiscard]] QColor workspaceOccupied() const { return tok_.workspaceOccupied; }
  [[nodiscard]] QColor workspaceActive() const { return tok_.workspaceActive; }
  [[nodiscard]] int radiusTight() const { return tok_.radiusTight; }
  [[nodiscard]] int radiusControl() const { return tok_.radiusControl; }
  [[nodiscard]] int radiusPopup() const { return tok_.radiusPopup; }
  [[nodiscard]] int radiusPill() const { return tok_.radiusPill; }
  [[nodiscard]] int borderWidth() const { return tok_.borderWidth; }
  [[nodiscard]] int focusBorderWidth() const { return tok_.focusBorderWidth; }
  [[nodiscard]] int separatorWidth() const { return tok_.separatorWidth; }
  [[nodiscard]] int controlHeight() const { return tok_.controlHeight; }
  [[nodiscard]] int controlPadding() const { return tok_.controlPadding; }
  [[nodiscard]] QColor ansiBlack() const { return tok_.ansiBlack; }
  [[nodiscard]] QColor ansiRed() const { return tok_.ansiRed; }
  [[nodiscard]] QColor ansiGreen() const { return tok_.ansiGreen; }
  [[nodiscard]] QColor ansiYellow() const { return tok_.ansiYellow; }
  [[nodiscard]] QColor ansiBlue() const { return tok_.ansiBlue; }
  [[nodiscard]] QColor ansiMagenta() const { return tok_.ansiMagenta; }
  [[nodiscard]] QColor ansiCyan() const { return tok_.ansiCyan; }
  [[nodiscard]] QColor ansiWhite() const { return tok_.ansiWhite; }
  [[nodiscard]] QColor ansiBrightBlack() const { return tok_.ansiBrightBlack; }
  [[nodiscard]] QColor ansiBrightRed() const { return tok_.ansiBrightRed; }
  [[nodiscard]] QColor ansiBrightGreen() const { return tok_.ansiBrightGreen; }
  [[nodiscard]] QColor ansiBrightYellow() const { return tok_.ansiBrightYellow; }
  [[nodiscard]] QColor ansiBrightBlue() const { return tok_.ansiBrightBlue; }
  [[nodiscard]] QColor ansiBrightMagenta() const { return tok_.ansiBrightMagenta; }
  [[nodiscard]] QColor ansiBrightCyan() const { return tok_.ansiBrightCyan; }
  [[nodiscard]] QColor ansiBrightWhite() const { return tok_.ansiBrightWhite; }

  [[nodiscard]] QColor surfaceVariant() const { return tok_.surfaceVariant; }
  [[nodiscard]] QColor surfaceContainer() const { return tok_.surfaceContainer; }
  [[nodiscard]] QColor onSurface() const { return tok_.onSurface; }
  [[nodiscard]] QColor onSurfaceVariant() const { return tok_.onSurfaceVariant; }
  [[nodiscard]] QColor onSurfaceDisabled() const { return tok_.onSurfaceDisabled; }
  [[nodiscard]] QColor onSurfaceInverse() const { return tok_.onSurfaceInverse; }
  [[nodiscard]] QColor secondary() const { return tok_.secondary; }
  [[nodiscard]] QColor onSecondary() const { return tok_.onSecondary; }
  [[nodiscard]] QColor outline() const { return tok_.outline; }
  [[nodiscard]] QColor outlineVariant() const { return tok_.outlineVariant; }
  [[nodiscard]] QColor hover() const { return tok_.hover; }
  [[nodiscard]] QColor pressed() const { return tok_.pressed; }
  [[nodiscard]] QColor textSubtle() const { return tok_.textSubtle; }

 signals:
  void paletteChanged();

 private:
  Holonight::ColorTokens tok_;
};
