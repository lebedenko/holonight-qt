// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/palette.h"

#include <QProxyStyle>

class HoloniightStyle : public QProxyStyle {
  Q_OBJECT
 public:
  explicit HoloniightStyle();

  void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                   const QWidget* widget = nullptr) const override;

  void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                     const QWidget* widget = nullptr) const override;

  void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
                          const QWidget* widget = nullptr) const override;

  [[nodiscard]] QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size,
                                       const QWidget* widget = nullptr) const override;

  [[nodiscard]] QRect subElementRect(SubElement element, const QStyleOption* option,
                                     const QWidget* widget = nullptr) const override;

  int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr) const override;

  [[nodiscard]] QPalette standardPalette() const override;

 private:
  // Paints a solid-colored triangle arrow. dir: 0=down, 1=up, 2=left, 3=right.
  static void paintArrow(QPainter* painter, const QRect& rect, int dir, const QColor& color);

  [[nodiscard]] static Holonight::ColorTokens tokens();

  static void drawPushButtonBevelImpl(const QStyleOption* option, QPainter* painter);
  void drawCheckBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  void drawRadioButtonImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  void drawTabBarTabImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  void drawHeaderImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  static void drawPanelButtonImpl(const QStyleOption* option, QPainter* painter);
  static void drawPanelItemViewImpl(const QStyleOption* option, QPainter* painter);
  void drawSliderImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  void drawSpinBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  void drawToolButtonImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
  void drawGroupBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
};
