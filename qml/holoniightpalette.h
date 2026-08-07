// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/palette.h"

#include <QColor>
#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqml.h>

namespace Holonight {
class AppearanceReader;
}

class HoloniightPalette : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(QColor background READ background NOTIFY paletteChanged)
  Q_PROPERTY(QColor surface READ surface NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceElevated READ surfaceElevated NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceRaised READ surfaceRaised NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceHover READ surfaceHover NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceSelected READ surfaceSelected NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceSelectedHover READ surfaceSelectedHover NOTIFY paletteChanged)
  Q_PROPERTY(QColor surfaceInverse READ surfaceInverse NOTIFY paletteChanged)

  Q_PROPERTY(QColor textPrimary READ textPrimary NOTIFY paletteChanged)
  Q_PROPERTY(QColor textSecondary READ textSecondary NOTIFY paletteChanged)
  Q_PROPERTY(QColor textMuted READ textMuted NOTIFY paletteChanged)
  Q_PROPERTY(QColor textDisabled READ textDisabled NOTIFY paletteChanged)
  Q_PROPERTY(QColor textInverse READ textInverse NOTIFY paletteChanged)
  Q_PROPERTY(QColor textAccent READ textAccent NOTIFY paletteChanged)
  Q_PROPERTY(QColor brandForeground READ brandForeground NOTIFY paletteChanged)

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
  Q_PROPERTY(QColor selectionIndicator READ selectionIndicator NOTIFY paletteChanged)

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
  Q_PROPERTY(int revision READ revision NOTIFY paletteChanged)

 public:
  Q_DISABLE_COPY_MOVE(HoloniightPalette)
  [[nodiscard]] static HoloniightPalette* create(QQmlEngine* engine, QJSEngine* script_engine);

  Q_INVOKABLE void reload();

  [[nodiscard]] QColor background() const { return tok_.background; }
  [[nodiscard]] QColor surface() const { return tok_.surface; }
  [[nodiscard]] QColor surfaceElevated() const { return tok_.surfaceElevated; }
  [[nodiscard]] QColor surfaceRaised() const { return tok_.surfaceRaised; }
  [[nodiscard]] QColor surfaceHover() const { return tok_.surfaceHover; }
  [[nodiscard]] QColor surfaceSelected() const { return tok_.surfaceSelected; }
  [[nodiscard]] QColor surfaceSelectedHover() const { return tok_.surfaceSelectedHover; }
  [[nodiscard]] QColor surfaceInverse() const { return tok_.surfaceInverse; }
  [[nodiscard]] QColor textPrimary() const { return tok_.textPrimary; }
  [[nodiscard]] QColor textSecondary() const { return tok_.textSecondary; }
  [[nodiscard]] QColor textMuted() const { return tok_.textMuted; }
  [[nodiscard]] QColor textDisabled() const { return tok_.textDisabled; }
  [[nodiscard]] QColor textInverse() const { return tok_.textInverse; }
  [[nodiscard]] QColor textAccent() const { return tok_.textAccent; }
  [[nodiscard]] QColor brandForeground() const { return tok_.brandForeground; }
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
  [[nodiscard]] QColor selectionIndicator() const { return tok_.selectionIndicator; }
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

  [[nodiscard]] int revision() const { return revision_; }

 signals:
  void paletteChanged();

 private:
  explicit HoloniightPalette(Holonight::AppearanceReader* reader, QObject* parent);

  Holonight::AppearanceReader* reader_ = nullptr;
  Holonight::ColorTokens tok_;
  int revision_ = 0;
};
