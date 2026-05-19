// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonightstyle.h"

#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionComplex>
#include <QStyleOptionHeader>
#include <algorithm>

HoloniightStyle::HoloniightStyle() : QProxyStyle(QStringLiteral("fusion")) {}

QPalette HoloniightStyle::standardPalette() const { return Holonight::buildPalette(Holonight::darkTokens()); }

int HoloniightStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const {
  switch (metric) {
    case PM_DefaultFrameWidth:
      return 1;
    case PM_ButtonMargin:
      return 6;
    case PM_ButtonDefaultIndicator:
      return 0;
    case PM_MenuBarHMargin:
      return 4;
    case PM_MenuBarVMargin:
      return 2;
    case PM_MenuBarItemSpacing:
      return 6;
    case PM_MenuBarPanelWidth:
      return 0;
    case PM_ToolBarIconSize:
      return 16;
    case PM_SmallIconSize:
      return 16;
    case PM_LargeIconSize:
      return 32;
    case PM_ScrollBarExtent:
      return 8;
    case PM_ScrollBarSliderMin:
      return 20;
    case PM_SliderThickness:
      return 14;
    case PM_SliderLength:
      return 14;
    case PM_TabBarTabHSpace:
      return 16;
    case PM_TabBarTabVSpace:
      return 6;
    case PM_IndicatorWidth:
      return 16;
    case PM_IndicatorHeight:
      return 16;
    case PM_ExclusiveIndicatorWidth:
      return 16;
    case PM_ExclusiveIndicatorHeight:
      return 16;
    default:
      return QProxyStyle::pixelMetric(metric, option, widget);
  }
}

void HoloniightStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                                 const QWidget* widget) const {
  switch (element) {
    case CE_PushButton:
      drawControl(CE_PushButtonBevel, option, painter, widget);
      drawControl(CE_PushButtonLabel, option, painter, widget);
      return;
    case CE_PushButtonBevel:
      drawPushButtonBevelImpl(option, painter);
      return;
    case CE_CheckBox:
      drawCheckBoxImpl(option, painter, widget);
      return;
    case CE_RadioButton:
      drawRadioButtonImpl(option, painter, widget);
      return;
    case CE_MenuItem: {
      const auto* opt = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
      if (opt == nullptr) {
        break;
      }
      if (opt->menuItemType == QStyleOptionMenuItem::Separator) {
        painter->save();
        const int mid = option->rect.center().y();
        painter->setPen(option->palette.color(QPalette::Mid));
        painter->drawLine(option->rect.left() + 8, mid, option->rect.right() - 8, mid);
        painter->restore();
        return;
      }
      if ((opt->state & (State_Selected | State_MouseOver)) != 0U) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(option->palette.color(QPalette::Highlight));
        painter->drawRoundedRect(option->rect.adjusted(2, 1, -2, -1), 4.0, 4.0);
        painter->restore();
      }
      QProxyStyle::drawControl(element, option, painter, widget);
      return;
    }
    case CE_MenuBarItem: {
      if ((option->state & (State_Selected | State_Sunken)) != 0U) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(option->palette.color(QPalette::Highlight));
        painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 4.0, 4.0);
        painter->restore();
      }
      QProxyStyle::drawControl(element, option, painter, widget);
      return;
    }
    case CE_ProgressBar:
      drawControl(CE_ProgressBarGroove, option, painter, widget);
      drawControl(CE_ProgressBarContents, option, painter, widget);
      drawControl(CE_ProgressBarLabel, option, painter, widget);
      return;
    case CE_ProgressBarGroove: {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(Qt::NoPen);
      painter->setBrush(option->palette.color(QPalette::Base));
      painter->drawRoundedRect(option->rect, 3.0, 3.0);
      painter->restore();
      return;
    }
    case CE_ProgressBarContents: {
      const auto* opt = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
      if (opt == nullptr || opt->maximum == opt->minimum) {
        break;
      }
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(Qt::NoPen);
      painter->setBrush(option->palette.color(QPalette::Highlight));
      const double ratio =
          static_cast<double>(opt->progress - opt->minimum) / static_cast<double>(opt->maximum - opt->minimum);
      QRect fill = option->rect;
      fill.setWidth(static_cast<int>(fill.width() * ratio));
      painter->drawRoundedRect(fill, 3.0, 3.0);
      painter->restore();
      return;
    }
    case CE_TabBarTab:
      drawTabBarTabImpl(option, painter, widget);
      return;
    case CE_ScrollBarSlider: {
      const auto tok = tokens();
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(Qt::NoPen);
      const bool hovered = (option->state & State_MouseOver) != 0U;
      painter->setBrush(hovered ? tok.primary : tok.borderPassive);
      painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 6.0, 6.0);
      painter->restore();
      return;
    }
    case CE_ScrollBarAddLine:
    case CE_ScrollBarSubLine:
      return;
    case CE_ScrollBarAddPage:
    case CE_ScrollBarSubPage: {
      const auto tok = tokens();
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surface);
      painter->drawRect(option->rect);
      painter->restore();
      return;
    }
    case CE_Header:
      drawHeaderImpl(option, painter, widget);
      return;
    case CE_Splitter: {
      const auto tok = tokens();
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surface);
      painter->drawRect(option->rect);
      painter->setPen(QPen{tok.borderPassive, 1});
      if ((option->state & State_Horizontal) != 0U) {
        const int centerX = option->rect.center().x();
        painter->drawLine(centerX, option->rect.top() + 4, centerX, option->rect.bottom() - 4);
      } else {
        const int centerY = option->rect.center().y();
        painter->drawLine(option->rect.left() + 4, centerY, option->rect.right() - 4, centerY);
      }
      painter->restore();
      return;
    }
    default:
      break;
  }

  QProxyStyle::drawControl(element, option, painter, widget);
}

void HoloniightStyle::drawPushButtonBevelImpl(const QStyleOption* option, QPainter* painter) {
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const bool hovered = (option->state & State_MouseOver) != 0U;
  const bool pressed = (option->state & State_Sunken) != 0U;
  const bool focused = (option->state & State_HasFocus) != 0U;

  QColor fill = tok.surfaceVariant;
  if (pressed) {
    fill = fill.darker(112);
  } else if (hovered) {
    fill = fill.lighter(108);
  }
  const int borderWidth = focused ? 2 : 1;
  painter->setPen(QPen{focused ? tok.borderFocus : tok.borderPassive, static_cast<qreal>(borderWidth)});
  painter->setBrush(fill);
  painter->drawRoundedRect(option->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), 4.0, 4.0);
  painter->restore();
}

void HoloniightStyle::drawCheckBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionButton*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto& pal = option->palette;
  const bool checked = (opt->state & State_On) != 0U;
  const int top = option->rect.top() + ((option->rect.height() - 14) / 2);
  const QRect indicator{option->rect.left(), top, 14, 14};

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  if (checked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(pal.color(QPalette::Highlight));
    painter->drawRoundedRect(indicator, 3.0, 3.0);
    painter->setPen(QPen{pal.color(QPalette::HighlightedText), 2.0});
    painter->drawLine(indicator.left() + 3, indicator.top() + 7, indicator.left() + 6, indicator.top() + 10);
    painter->drawLine(indicator.left() + 6, indicator.top() + 10, indicator.left() + 11, indicator.top() + 4);
  } else {
    painter->setPen(pal.color(QPalette::Mid));
    painter->setBrush(pal.color(QPalette::Base));
    painter->drawRoundedRect(indicator, 3.0, 3.0);
  }
  painter->restore();

  QStyleOptionButton labelOpt = *opt;
  labelOpt.rect.setLeft(indicator.right() + 6);
  drawControl(CE_CheckBoxLabel, &labelOpt, painter, widget);
}

void HoloniightStyle::drawRadioButtonImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionButton*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto& pal = option->palette;
  const bool checked = (opt->state & State_On) != 0U;
  const int top = option->rect.top() + ((option->rect.height() - 16) / 2);
  const QRect indicator{option->rect.left(), top, 16, 16};

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(pal.color(QPalette::Mid));
  painter->setBrush(checked ? pal.color(QPalette::Highlight) : pal.color(QPalette::Base));
  painter->drawEllipse(indicator);
  if (checked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(pal.color(QPalette::HighlightedText));
    painter->drawEllipse(indicator.adjusted(4, 4, -4, -4));
  }
  painter->restore();

  QStyleOptionButton labelOpt = *opt;
  labelOpt.rect.setLeft(indicator.right() + 6);
  drawControl(CE_RadioButtonLabel, &labelOpt, painter, widget);
}

void HoloniightStyle::drawTabBarTabImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionTab*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  const bool selected = (opt->state & State_Selected) != 0U;
  const bool hovered = ((opt->state & State_MouseOver) != 0U) && !selected;

  painter->save();
  painter->setPen(Qt::NoPen);
  QColor bgColor = tok.surface;
  if (selected) {
    bgColor = tok.surfaceVariant;
  } else if (hovered) {
    bgColor = tok.surfaceHover;
  }
  painter->setBrush(bgColor);
  painter->drawRect(option->rect);
  if (selected) {
    painter->setPen(QPen{tok.borderActive, 2});
    const QRect tabRect = option->rect;
    painter->drawLine(tabRect.left() + 2, tabRect.bottom(), tabRect.right() - 2, tabRect.bottom());
  }
  painter->restore();
  drawControl(CE_TabBarTabLabel, option, painter, widget);
}

void HoloniightStyle::drawHeaderImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionHeader*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  painter->save();
  const bool hovered = (opt->state & State_MouseOver) != 0U;
  painter->setPen(Qt::NoPen);
  painter->setBrush(hovered ? tok.surfaceHover : tok.surfaceVariant);
  painter->drawRect(opt->rect);
  painter->setPen(QPen{tok.borderPassive, 1});
  painter->drawLine(opt->rect.bottomLeft(), opt->rect.bottomRight());
  painter->drawLine(opt->rect.topRight(), opt->rect.bottomRight());
  if (opt->sortIndicator != QStyleOptionHeader::None) {
    const QRect arrowRect = subElementRect(SE_HeaderArrow, opt, widget);
    paintArrow(painter, arrowRect, opt->sortIndicator == QStyleOptionHeader::SortDown ? 0 : 1, tok.onSurface);
  }
  const QRect labelRect = subElementRect(SE_HeaderLabel, opt, widget);
  painter->setPen(tok.onSurfaceVariant);
  painter->drawText(labelRect, Qt::AlignVCenter | Qt::AlignLeft, opt->text);
  painter->restore();
}

// ── Helpers ──────────────────────────────────────────────────────────────────

Holonight::ColorTokens HoloniightStyle::tokens() { return Holonight::darkTokens(); }

void HoloniightStyle::paintArrow(QPainter* painter, const QRect& rect, int dir, const QColor& color) {
  painter->save();
  painter->setPen(Qt::NoPen);
  painter->setBrush(color);
  const int arrowX = rect.center().x();
  const int arrowY = rect.center().y();
  QPolygon tri;
  switch (dir) {
    case 0:  // down
      tri << QPoint{arrowX - 4, arrowY - 2} << QPoint{arrowX + 4, arrowY - 2} << QPoint{arrowX, arrowY + 2};
      break;
    case 1:  // up
      tri << QPoint{arrowX - 4, arrowY + 2} << QPoint{arrowX + 4, arrowY + 2} << QPoint{arrowX, arrowY - 2};
      break;
    case 2:  // left
      tri << QPoint{arrowX + 2, arrowY - 4} << QPoint{arrowX + 2, arrowY + 4} << QPoint{arrowX - 2, arrowY};
      break;
    case 3:  // right
      tri << QPoint{arrowX - 2, arrowY - 4} << QPoint{arrowX - 2, arrowY + 4} << QPoint{arrowX + 2, arrowY};
      break;
    default:
      break;
  }
  painter->drawPolygon(tri);
  painter->restore();
}

// ── drawPrimitive helpers ─────────────────────────────────────────────────────

void HoloniightStyle::drawPanelButtonImpl(const QStyleOption* option, QPainter* painter) {
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const bool hovered = (option->state & State_MouseOver) != 0U;
  const bool pressed = (option->state & State_Sunken) != 0U;
  const bool focused = (option->state & State_HasFocus) != 0U;
  QColor fill = tok.surfaceVariant;
  if (pressed) {
    fill = fill.darker(112);
  } else if (hovered) {
    fill = fill.lighter(108);
  }
  const int borderWidth = focused ? 2 : 1;
  painter->setPen(QPen{focused ? tok.borderFocus : tok.borderPassive, static_cast<qreal>(borderWidth)});
  painter->setBrush(fill);
  painter->drawRoundedRect(option->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), 4.0, 4.0);
  painter->restore();
}

void HoloniightStyle::drawPanelItemViewImpl(const QStyleOption* option, QPainter* painter) {
  const auto* opt = qstyleoption_cast<const QStyleOptionViewItem*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);
  const bool selected = (opt->state & State_Selected) != 0U;
  const bool hovered = (opt->state & State_MouseOver) != 0U;
  const bool focused = (opt->state & State_HasFocus) != 0U;
  if (selected) {
    painter->setBrush(tok.primary);
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 2.0, 2.0);
  } else if (hovered) {
    painter->setBrush(tok.surfaceHover);
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 2.0, 2.0);
  }
  if (focused && !selected) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen{tok.focusRing, 1});
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 2.0, 2.0);
  }
  painter->restore();
}

// ── drawPrimitive ─────────────────────────────────────────────────────────────

void HoloniightStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                                   const QWidget* widget) const {
  const auto tok = tokens();

  switch (element) {
    case PE_PanelButtonCommand:
      drawPanelButtonImpl(option, painter);
      return;

    case PE_FrameDefaultButton: {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(QPen{tok.focusRing, 2});
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(option->rect.adjusted(-1, -1, 1, 1), 5.0, 5.0);
      painter->restore();
      return;
    }

    case PE_Frame:
    case PE_FrameTabWidget: {
      painter->save();
      painter->setPen(tok.borderPassive);
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
      painter->restore();
      return;
    }

    case PE_FrameTabBarBase: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surface);
      painter->drawRect(option->rect);
      painter->restore();
      return;
    }

    case PE_FrameMenu: {
      painter->save();
      painter->setPen(QPen{tok.borderPassive, 1});
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
      painter->restore();
      return;
    }

    case PE_PanelMenu: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.secondary);
      painter->drawRect(option->rect);
      painter->restore();
      return;
    }

    case PE_PanelTipLabel: {
      painter->save();
      painter->setPen(QPen{tok.borderPassive, 1});
      painter->setBrush(tok.surfaceInverse);
      painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
      painter->restore();
      return;
    }

    case PE_PanelScrollAreaCorner: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surface);
      painter->drawRect(option->rect);
      painter->restore();
      return;
    }

    case PE_FrameFocusRect: {
      painter->save();
      painter->setPen(QPen{tok.focusRing, 1, Qt::DotLine});
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(option->rect.adjusted(1, 1, -1, -1));
      painter->restore();
      return;
    }

    case PE_IndicatorArrowDown:
      paintArrow(painter, option->rect, 0, tok.onSurface);
      return;
    case PE_IndicatorArrowUp:
      paintArrow(painter, option->rect, 1, tok.onSurface);
      return;
    case PE_IndicatorArrowLeft:
      paintArrow(painter, option->rect, 2, tok.onSurface);
      return;
    case PE_IndicatorArrowRight:
      paintArrow(painter, option->rect, 3, tok.onSurface);
      return;

    case PE_IndicatorBranch: {
      const bool hasChildren = (option->state & State_Children) != 0U;
      const bool expanded = (option->state & State_Open) != 0U;
      if (hasChildren) {
        paintArrow(painter, option->rect, expanded ? 0 : 3, tok.onSurfaceVariant);
      }
      return;
    }

    case PE_IndicatorHeaderArrow: {
      const auto* hOpt = qstyleoption_cast<const QStyleOptionHeader*>(option);
      const bool sortDown = (hOpt != nullptr) && (hOpt->sortIndicator == QStyleOptionHeader::SortDown);
      paintArrow(painter, option->rect, sortDown ? 0 : 1, tok.onSurface);
      return;
    }

    case PE_IndicatorToolBarSeparator: {
      painter->save();
      painter->setPen(tok.borderPassive);
      const int centerX = option->rect.center().x();
      painter->drawLine(centerX, option->rect.top() + 4, centerX, option->rect.bottom() - 4);
      painter->restore();
      return;
    }

    case PE_PanelItemViewItem:
      drawPanelItemViewImpl(option, painter);
      return;

    case PE_PanelLineEdit: {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      const bool focused = (option->state & State_HasFocus) != 0U;
      const bool enabled = (option->state & State_Enabled) != 0U;
      painter->setBrush(tok.surface);
      const int borderWidth = focused ? 2 : 1;
      QColor border = tok.borderPassive;
      if (focused) {
        border = tok.borderFocus;
      } else if (!enabled) {
        border = tok.borderPassive;  // TODO(borderUrgent): if error state added, use tok.borderUrgent here
      }
      painter->setPen(QPen{border, static_cast<qreal>(borderWidth)});
      painter->drawRoundedRect(option->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), 4.0, 4.0);
      painter->restore();
      return;
    }

    case PE_FrameGroupBox: {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(QPen{tok.borderPassive, 1});
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(option->rect.adjusted(0, 0, -1, -1), 4.0, 4.0);
      painter->restore();
      return;
    }

    case PE_PanelStatusBar: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surface);
      painter->drawRect(option->rect);
      painter->setPen(QPen{tok.borderPassive, 1});
      painter->drawLine(option->rect.topLeft(), option->rect.topRight());
      painter->restore();
      return;
    }

    default:
      break;
  }

  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

// ── drawComplexControl ────────────────────────────────────────────────────────

void HoloniightStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
                                        const QWidget* widget) const {
  switch (control) {
    case CC_ComboBox: {
      const auto* opt = qstyleoption_cast<const QStyleOptionComboBox*>(option);
      if (opt == nullptr) {
        break;
      }
      const auto tok = tokens();
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      const bool focused = (opt->state & State_HasFocus) != 0U;
      const bool hovered = (opt->state & State_MouseOver) != 0U;
      const bool enabled = (opt->state & State_Enabled) != 0U;
      const int borderWidth = focused ? 2 : 1;
      const QColor fill = hovered ? tok.surfaceHover : tok.surface;
      QColor border = tok.borderPassive;
      if (focused) {
        border = tok.borderFocus;
      } else if (!enabled) {
        border = tok.borderPassive;  // TODO(borderUrgent): if error state added, use tok.borderUrgent here
      }
      painter->setBrush(fill);
      painter->setPen(QPen{border, static_cast<qreal>(borderWidth)});
      painter->drawRoundedRect(opt->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), 4.0, 4.0);
      const QRect arrowRect = subControlRect(CC_ComboBox, opt, SC_ComboBoxArrow, widget);
      paintArrow(painter, arrowRect, 0, tok.onSurface);
      painter->restore();
      return;
    }
    case CC_Slider:
      drawSliderImpl(option, painter, widget);
      return;
    case CC_SpinBox:
      drawSpinBoxImpl(option, painter, widget);
      return;
    case CC_ToolButton:
      drawToolButtonImpl(option, painter, widget);
      return;
    case CC_GroupBox:
      drawGroupBoxImpl(option, painter, widget);
      return;
    default:
      break;
  }

  QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void HoloniightStyle::drawSliderImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionSlider*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const bool horizontal = opt->orientation == Qt::Horizontal;
  const QRect grooveRect = subControlRect(CC_Slider, opt, SC_SliderGroove, widget);
  const QRect handleRect = subControlRect(CC_Slider, opt, SC_SliderHandle, widget);

  painter->setPen(Qt::NoPen);
  painter->setBrush(tok.borderPassive);
  if (horizontal) {
    const QRect fullGroove{grooveRect.left(), grooveRect.center().y() - 2, grooveRect.width(), 4};
    painter->drawRoundedRect(fullGroove, 2.0, 2.0);
    painter->setBrush(tok.primary);
    const QRect filledGroove{grooveRect.left(), grooveRect.center().y() - 2,
                             handleRect.center().x() - grooveRect.left(), 4};
    if (filledGroove.width() > 0) {
      painter->drawRoundedRect(filledGroove, 2.0, 2.0);
    }
  } else {
    const QRect fullGroove{grooveRect.center().x() - 2, grooveRect.top(), 4, grooveRect.height()};
    painter->drawRoundedRect(fullGroove, 2.0, 2.0);
    painter->setBrush(tok.primary);
    const int filledTop = handleRect.center().y();
    const QRect filledGroove{grooveRect.center().x() - 2, filledTop, 4, grooveRect.bottom() - filledTop};
    if (filledGroove.height() > 0) {
      painter->drawRoundedRect(filledGroove, 2.0, 2.0);
    }
  }

  const bool pressed = (opt->state & State_Sunken) != 0U;
  const bool hovered = (opt->state & State_MouseOver) != 0U;
  QColor handleColor = tok.primary;
  if (pressed) {
    handleColor = tok.primaryPressed;
  } else if (hovered) {
    handleColor = tok.primaryHover;
  }
  painter->setBrush(handleColor);
  painter->setPen(QPen{tok.borderPassive, 1});
  painter->drawRoundedRect(handleRect.adjusted(1, 1, -1, -1), 2.0, 2.0);
  painter->restore();
}

void HoloniightStyle::drawSpinBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionSpinBox*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const bool focused = (opt->state & State_HasFocus) != 0U;
  const bool enabled = (opt->state & State_Enabled) != 0U;
  const int borderWidth = focused ? 2 : 1;
  QColor border = tok.borderPassive;
  if (focused) {
    border = tok.borderFocus;
  } else if (!enabled) {
    border = tok.borderPassive;  // TODO(borderUrgent): if error state added, use tok.borderUrgent here
  }
  painter->setBrush(tok.surface);
  painter->setPen(QPen{border, static_cast<qreal>(borderWidth)});
  painter->drawRoundedRect(opt->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), 4.0, 4.0);

  const QRect upRect = subControlRect(CC_SpinBox, opt, SC_SpinBoxUp, widget);
  painter->setPen(QPen{tok.borderPassive, 1});
  painter->drawLine(upRect.left(), opt->rect.top() + 2, upRect.left(), opt->rect.bottom() - 2);

  const bool upHovered = (opt->activeSubControls & SC_SpinBoxUp) != 0U;
  const bool downHovered = (opt->activeSubControls & SC_SpinBoxDown) != 0U;
  const QRect downRect = subControlRect(CC_SpinBox, opt, SC_SpinBoxDown, widget);
  const QColor idleBg = enabled ? tok.surfaceVariant : tok.surface;
  painter->setPen(Qt::NoPen);
  painter->setBrush(upHovered ? tok.primary : idleBg);
  painter->drawRect(upRect.adjusted(1, 1, -1, 0));
  painter->setBrush(downHovered ? tok.primary : idleBg);
  painter->drawRect(downRect.adjusted(1, 0, -1, -1));

  const QColor arrowColor = enabled ? tok.onSurface : tok.onSurfaceDisabled;
  paintArrow(painter, upRect, 1, arrowColor);
  paintArrow(painter, downRect, 0, arrowColor);
  painter->restore();
}

void HoloniightStyle::drawToolButtonImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionToolButton*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const bool hovered = (opt->state & State_MouseOver) != 0U;
  const bool pressed = (opt->state & State_Sunken) != 0U;
  const bool checked = (opt->state & State_On) != 0U;
  const bool focused = (opt->state & State_HasFocus) != 0U;

  QColor bgColor = Qt::transparent;
  if (pressed || checked) {
    bgColor = tok.surfaceVariant;
  } else if (hovered) {
    bgColor = tok.surfaceHover;
  }
  painter->setPen(Qt::NoPen);
  painter->setBrush(bgColor);
  painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), 4.0, 4.0);

  if (focused) {
    painter->setPen(QPen{tok.focusRing, 1, Qt::DashLine});
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), 4.0, 4.0);
  }

  if ((opt->features & QStyleOptionToolButton::MenuButtonPopup) != 0U) {
    const QRect menuRect = subControlRect(CC_ToolButton, opt, SC_ToolButtonMenu, widget);
    painter->setPen(QPen{tok.borderPassive, 1});
    painter->drawLine(menuRect.left(), opt->rect.top() + 3, menuRect.left(), opt->rect.bottom() - 3);
    paintArrow(painter, menuRect, 0, tok.onSurface);
  }
  painter->restore();
  drawControl(CE_ToolButtonLabel, opt, painter, widget);
}

void HoloniightStyle::drawGroupBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionGroupBox*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const QRect frameRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxFrame, widget);
  painter->setPen(QPen{tok.borderPassive, 1});
  painter->setBrush(Qt::NoBrush);
  painter->drawRoundedRect(frameRect.adjusted(0, 0, -1, -1), 4.0, 4.0);

  const QRect labelRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxLabel, widget);
  painter->setPen(Qt::NoPen);
  painter->setBrush(tok.surface);
  painter->drawRect(labelRect.adjusted(-2, 0, 2, 0));
  painter->setPen(tok.onSurface);
  painter->drawText(labelRect, Qt::AlignVCenter | Qt::AlignLeft, opt->text);

  if ((opt->subControls & SC_GroupBoxCheckBox) != 0U) {
    QStyleOptionButton checkOpt;
    checkOpt.initFrom(widget);
    checkOpt.rect = subControlRect(CC_GroupBox, opt, SC_GroupBoxCheckBox, widget);
    checkOpt.state = opt->state;
    drawPrimitive(PE_IndicatorCheckBox, &checkOpt, painter, widget);
  }
  painter->restore();
}

// ── sizeFromContents ──────────────────────────────────────────────────────────

QSize HoloniightStyle::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size,
                                       const QWidget* widget) const {
  switch (type) {
    case CT_ComboBox: {
      constexpr int arrowWidth = 20;
      constexpr int hPad = 8;
      constexpr int vPad = 8;
      constexpr int minH = 32;
      return {size.width() + arrowWidth + hPad, std::max(size.height() + vPad, minH)};
    }
    case CT_SpinBox: {
      constexpr int arrowWidth = 40;
      constexpr int hPad = 8;
      constexpr int vPad = 8;
      constexpr int minH = 32;
      return {size.width() + arrowWidth + hPad, std::max(size.height() + vPad, minH)};
    }
    default:
      break;
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

// ── subElementRect ────────────────────────────────────────────────────────────

QRect HoloniightStyle::subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const {
  switch (element) {
    case SE_LineEditContents:
      return option->rect.adjusted(4, 0, -4, 0);

    case SE_CheckBoxIndicator: {
      const int side = pixelMetric(PM_IndicatorWidth, option, widget);
      const int top = option->rect.top() + ((option->rect.height() - side) / 2);
      if (option->direction == Qt::RightToLeft) {
        return {option->rect.right() - side - 2, top, side, side};
      }
      return {option->rect.left() + 2, top, side, side};
    }

    case SE_RadioButtonIndicator: {
      const int side = pixelMetric(PM_ExclusiveIndicatorWidth, option, widget);
      const int top = option->rect.top() + ((option->rect.height() - side) / 2);
      if (option->direction == Qt::RightToLeft) {
        return {option->rect.right() - side - 2, top, side, side};
      }
      return {option->rect.left() + 2, top, side, side};
    }

    default:
      break;
  }
  return QProxyStyle::subElementRect(element, option, widget);
}
