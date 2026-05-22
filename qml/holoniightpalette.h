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

  // Surface roles
  Q_PROPERTY(QColor surface READ surface CONSTANT)
  Q_PROPERTY(QColor surfaceVariant READ surfaceVariant CONSTANT)
  Q_PROPERTY(QColor surfaceContainer READ surfaceContainer CONSTANT)
  Q_PROPERTY(QColor surfaceHover READ surfaceHover CONSTANT)
  Q_PROPERTY(QColor surfaceInverse READ surfaceInverse CONSTANT)
  Q_PROPERTY(QColor onSurface READ onSurface CONSTANT)
  Q_PROPERTY(QColor onSurfaceVariant READ onSurfaceVariant CONSTANT)
  Q_PROPERTY(QColor onSurfaceDisabled READ onSurfaceDisabled CONSTANT)
  Q_PROPERTY(QColor onSurfaceInverse READ onSurfaceInverse CONSTANT)

  // Primary accent
  Q_PROPERTY(QColor primary READ primary CONSTANT)
  Q_PROPERTY(QColor primaryHover READ primaryHover CONSTANT)
  Q_PROPERTY(QColor primaryPressed READ primaryPressed CONSTANT)
  Q_PROPERTY(QColor onPrimary READ onPrimary CONSTANT)

  // Secondary
  Q_PROPERTY(QColor secondary READ secondary CONSTANT)
  Q_PROPERTY(QColor onSecondary READ onSecondary CONSTANT)

  // Outline / border (deprecated — kept for backward compatibility)
  Q_PROPERTY(QColor outline READ outline CONSTANT)
  Q_PROPERTY(QColor outlineVariant READ outlineVariant CONSTANT)

  // Semantic borders
  Q_PROPERTY(QColor borderPassive READ borderPassive CONSTANT)
  Q_PROPERTY(QColor borderHover READ borderHover CONSTANT)
  Q_PROPERTY(QColor borderFocus READ borderFocus CONSTANT)
  Q_PROPERTY(QColor borderActive READ borderActive CONSTANT)
  Q_PROPERTY(QColor borderUrgent READ borderUrgent CONSTANT)

  // State overlays
  Q_PROPERTY(QColor hover READ hover CONSTANT)
  Q_PROPERTY(QColor pressed READ pressed CONSTANT)
  Q_PROPERTY(QColor focusRing READ focusRing CONSTANT)

  // Semantic status
  Q_PROPERTY(QColor error READ error CONSTANT)
  Q_PROPERTY(QColor warning READ warning CONSTANT)
  Q_PROPERTY(QColor success READ success CONSTANT)
  Q_PROPERTY(QColor onError READ onError CONSTANT)

  // Shadow
  Q_PROPERTY(QColor shadow READ shadow CONSTANT)

  // Design-system accents and secondary text
  Q_PROPERTY(QColor accentCyan READ accentCyan CONSTANT)
  Q_PROPERTY(QColor accentBlue READ accentBlue CONSTANT)
  Q_PROPERTY(QColor accentViolet READ accentViolet CONSTANT)
  Q_PROPERTY(QColor textSubtle READ textSubtle CONSTANT)

  // Workspace indicator fills
  Q_PROPERTY(QColor workspaceOccupied READ workspaceOccupied CONSTANT)
  Q_PROPERTY(QColor workspaceActive READ workspaceActive CONSTANT)

 public:
  explicit HoloniightPalette(QObject* parent = nullptr);

  [[nodiscard]] QColor surface() const { return tok_.surface; }
  [[nodiscard]] QColor surfaceVariant() const { return tok_.surfaceVariant; }
  [[nodiscard]] QColor surfaceContainer() const { return tok_.surfaceContainer; }
  [[nodiscard]] QColor surfaceHover() const { return tok_.surfaceHover; }
  [[nodiscard]] QColor surfaceInverse() const { return tok_.surfaceInverse; }
  [[nodiscard]] QColor onSurface() const { return tok_.onSurface; }
  [[nodiscard]] QColor onSurfaceVariant() const { return tok_.onSurfaceVariant; }
  [[nodiscard]] QColor onSurfaceDisabled() const { return tok_.onSurfaceDisabled; }
  [[nodiscard]] QColor onSurfaceInverse() const { return tok_.onSurfaceInverse; }
  [[nodiscard]] QColor primary() const { return tok_.primary; }
  [[nodiscard]] QColor primaryHover() const { return tok_.primaryHover; }
  [[nodiscard]] QColor primaryPressed() const { return tok_.primaryPressed; }
  [[nodiscard]] QColor onPrimary() const { return tok_.onPrimary; }
  [[nodiscard]] QColor secondary() const { return tok_.secondary; }
  [[nodiscard]] QColor onSecondary() const { return tok_.onSecondary; }
  [[nodiscard]] QColor outline() const { return tok_.outline; }
  [[nodiscard]] QColor outlineVariant() const { return tok_.outlineVariant; }
  [[nodiscard]] QColor borderPassive() const { return tok_.borderPassive; }
  [[nodiscard]] QColor borderHover() const { return tok_.borderHover; }
  [[nodiscard]] QColor borderFocus() const { return tok_.borderFocus; }
  [[nodiscard]] QColor borderActive() const { return tok_.borderActive; }
  [[nodiscard]] QColor borderUrgent() const { return tok_.borderUrgent; }
  [[nodiscard]] QColor hover() const { return tok_.hover; }
  [[nodiscard]] QColor pressed() const { return tok_.pressed; }
  [[nodiscard]] QColor focusRing() const { return tok_.focusRing; }
  [[nodiscard]] QColor error() const { return tok_.error; }
  [[nodiscard]] QColor warning() const { return tok_.warning; }
  [[nodiscard]] QColor success() const { return tok_.success; }
  [[nodiscard]] QColor onError() const { return tok_.onError; }
  [[nodiscard]] QColor shadow() const { return tok_.shadow; }
  [[nodiscard]] QColor accentCyan() const { return tok_.accentCyan; }
  [[nodiscard]] QColor accentBlue() const { return tok_.accentBlue; }
  [[nodiscard]] QColor accentViolet() const { return tok_.accentViolet; }
  [[nodiscard]] QColor textSubtle() const { return tok_.textSubtle; }
  [[nodiscard]] QColor workspaceOccupied() const { return tok_.workspaceOccupied; }
  [[nodiscard]] QColor workspaceActive() const { return tok_.workspaceActive; }

 private:
  Holonight::ColorTokens tok_;
};
