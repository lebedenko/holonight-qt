// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonightstyle.h"

#include <QAbstractItemView>
#include <QDockWidget>
#include <QFrame>
#include <QPainter>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleOption>
#include <QStyleOptionComplex>
#include <QStyleOptionHeader>
#include <QTextEdit>
#include <QToolButton>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace {
constexpr qreal kRadiusControl = 6.0;  // unified control corner radius
constexpr qreal kRadiusTight = 2.0;    // tight radius for item-view selection/focus rings
constexpr qreal kRadiusGroove = 3.0;   // progress-bar groove (not a control shape)

enum class SurfaceRole : uint8_t {
  Window,
  View,
  SidePanel,
  Container,
  Popup,
};

bool nameContains(const QString& name, QLatin1StringView needle) { return name.contains(needle, Qt::CaseInsensitive); }

bool isNamedSidePanel(const QWidget* widget) {
  if (widget == nullptr) {
    return false;
  }
  const QString className = QString::fromLatin1(widget->metaObject()->className());
  const QString objectName = widget->objectName();
  return nameContains(className, QLatin1StringView{"PlacesPanel"}) ||
         nameContains(className, QLatin1StringView{"InformationPanel"}) ||
         nameContains(objectName, QLatin1StringView{"placesPanel"}) ||
         nameContains(objectName, QLatin1StringView{"informationPanel"}) ||
         nameContains(objectName, QLatin1StringView{"placesDock"}) ||
         nameContains(objectName, QLatin1StringView{"infoDock"});
}

bool isNamedStatusPanel(const QWidget* widget) {
  if (widget == nullptr) {
    return false;
  }
  const QString className = QString::fromLatin1(widget->metaObject()->className());
  const QString objectName = widget->objectName();
  return nameContains(className, QLatin1StringView{"StatusBar"}) ||
         nameContains(objectName, QLatin1StringView{"statusBar"}) ||
         nameContains(objectName, QLatin1StringView{"statusLabel"});
}

SurfaceRole classifyWidgetSurface(const QWidget* widget) {
  if (widget == nullptr) {
    return SurfaceRole::Window;
  }
  if (isNamedSidePanel(widget)) {
    return SurfaceRole::SidePanel;
  }
  if (qobject_cast<const QDockWidget*>(widget) != nullptr) {
    return SurfaceRole::SidePanel;
  }
  if (qobject_cast<const QAbstractItemView*>(widget) != nullptr) {
    return SurfaceRole::View;
  }
  if (qobject_cast<const QStatusBar*>(widget) != nullptr || isNamedStatusPanel(widget)) {
    return SurfaceRole::Container;
  }
  return SurfaceRole::Window;
}

QColor surfaceColor(SurfaceRole role, const Holonight::ColorTokens& tok) {
  switch (role) {
    case SurfaceRole::View:
      return tok.surface;
    case SurfaceRole::SidePanel:
    case SurfaceRole::Container:
      return tok.surfaceVariant;
    case SurfaceRole::Popup:
      return tok.secondary;
    case SurfaceRole::Window:
      return tok.surface;
  }
  return tok.surface;
}

QColor widgetSurfaceColor(const QWidget* widget, const Holonight::ColorTokens& tok) {
  return surfaceColor(classifyWidgetSurface(widget), tok);
}

bool parentUsesSidePanelSurface(const QWidget* widget, const Holonight::ColorTokens& tok) {
  for (const QWidget* parent = widget != nullptr ? widget->parentWidget() : nullptr; parent != nullptr;
       parent = parent->parentWidget()) {
    if (parent->palette().color(QPalette::Window) == tok.surfaceVariant) {
      return true;
    }
  }
  return false;
}

QRect centeredSeparatorRect(const QRect& rect, Qt::Orientation orientation) {
  QRect lineRect = rect;
  if (orientation == Qt::Horizontal) {
    lineRect.setTop(rect.center().y());
    lineRect.setHeight(1);
  } else {
    lineRect.setLeft(rect.center().x());
    lineRect.setWidth(1);
  }
  return lineRect;
}

void drawSeparator(QPainter* painter, const QRect& rect, Qt::Orientation orientation, const QColor& color) {
  painter->fillRect(centeredSeparatorRect(rect, orientation), color);
}

bool shouldSuppressScrollAreaFrame(const QWidget* widget) {
  if (widget == nullptr) {
    return false;
  }
  if (qobject_cast<const QAbstractItemView*>(widget) != nullptr) {
    return true;
  }

  const QString className = QString::fromLatin1(widget->metaObject()->className());
  return nameContains(className, QLatin1StringView{"KItemListContainer"});
}

bool isTextEditLikeWidget(const QWidget* widget) {
  return qobject_cast<const QTextEdit*>(widget) != nullptr || qobject_cast<const QPlainTextEdit*>(widget) != nullptr;
}

QRectF strokedRectInside(const QRect& rect, qreal penWidth) {
  const qreal inset = penWidth / 2.0;
  return QRectF{rect}.adjusted(inset, inset, -inset, -inset);
}

void drawPlainFrame(QPainter* painter, const QRect& rect, const QColor& color) {
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->setPen(QPen{color, 1.0});
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(strokedRectInside(rect, 1.0));
  painter->restore();
}

void drawInputFrame(QPainter* painter, const QRect& rect, const QColor& color, qreal penWidth) {
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(QPen{color, penWidth});
  painter->setBrush(Qt::NoBrush);
  painter->drawRoundedRect(strokedRectInside(rect, penWidth), kRadiusControl, kRadiusControl);
  painter->restore();
}

void drawInputPanel(QPainter* painter, const QRect& rect, const QColor& fill, const QColor& border, qreal penWidth) {
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);
  painter->setBrush(fill);
  painter->drawRoundedRect(strokedRectInside(rect, penWidth), kRadiusControl, kRadiusControl);
  painter->restore();
  drawInputFrame(painter, rect, border, penWidth);
}

void makeSplitterHandleTransparent(QSplitterHandle* handle) {
  if (handle == nullptr) {
    return;
  }
  handle->setAutoFillBackground(false);
  handle->setAttribute(Qt::WA_NoSystemBackground, true);
  handle->setAttribute(Qt::WA_OpaquePaintEvent, false);
}

bool isFlatAutoRaiseToolButton(const QStyleOptionToolButton* option, const QWidget* widget) {
  if ((option->features & QStyleOptionToolButton::MenuButtonPopup) != 0U) {
    return false;
  }

  bool autoRaise = (option->state & QStyle::State_AutoRaise) != 0U;
  if (const auto* toolButton = qobject_cast<const QToolButton*>(widget); toolButton != nullptr) {
    autoRaise = autoRaise || toolButton->autoRaise();
  }
  return autoRaise;
}

bool isAutoRaiseToolButton(const QStyleOption* option, const QWidget* widget) {
  bool autoRaise = (option->state & QStyle::State_AutoRaise) != 0U;
  if (const auto* toolButton = qobject_cast<const QToolButton*>(widget); toolButton != nullptr) {
    autoRaise = autoRaise || toolButton->autoRaise();
  }
  return autoRaise;
}

bool isFlatButtonOption(const QStyleOption* option) {
  const auto* buttonOpt = qstyleoption_cast<const QStyleOptionButton*>(option);
  return buttonOpt != nullptr && (buttonOpt->features & QStyleOptionButton::Flat) != 0U;
}

bool drawFlatButtonPanelIfNeeded(const QStyleOption* option, QPainter* painter) {
  if (!isFlatButtonOption(option)) {
    return false;
  }

  const bool hovered = (option->state & QStyle::State_MouseOver) != 0U;
  const bool pressed = (option->state & QStyle::State_Sunken) != 0U;
  const bool checked = (option->state & QStyle::State_On) != 0U;
  if (!hovered && !pressed && !checked) {
    return true;
  }

  const auto tok = Holonight::darkTokens();
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);
  painter->setBrush((pressed || checked) ? tok.surfaceVariant : tok.surfaceHover);
  painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
  painter->restore();
  return true;
}
}  // namespace

HoloniightStyle::HoloniightStyle() : QProxyStyle(QStringLiteral("fusion")), config_{Holonight::ThemeConfig::load()} {}

QPalette HoloniightStyle::standardPalette() const { return Holonight::buildPalette(Holonight::darkTokens()); }

void HoloniightStyle::polish(QPalette& palette) {
  palette = standardPalette();
  QProxyStyle::polish(palette);
}

void HoloniightStyle::polish(QWidget* widget) {
  QProxyStyle::polish(widget);
  if (widget == nullptr) {
    return;
  }

  const auto tok = tokens();
  QPalette palette = widget->palette();
  SurfaceRole surfaceRole = classifyWidgetSurface(widget);
  QColor windowSurface = widgetSurfaceColor(widget, tok);
  if (surfaceRole == SurfaceRole::Window &&
      (palette.color(QPalette::Window) == tok.surfaceVariant || parentUsesSidePanelSurface(widget, tok))) {
    surfaceRole = SurfaceRole::SidePanel;
    windowSurface = surfaceColor(surfaceRole, tok);
  }
  palette.setColor(QPalette::Window, windowSurface);
  palette.setColor(QPalette::WindowText, tok.onSurface);
  palette.setColor(QPalette::Base, surfaceRole == SurfaceRole::SidePanel ? tok.surface : windowSurface);
  palette.setColor(QPalette::AlternateBase, palette.color(QPalette::Base));
  palette.setColor(QPalette::Text, tok.onSurface);
  palette.setColor(QPalette::Button, tok.surfaceVariant);
  palette.setColor(QPalette::ButtonText, tok.onSurface);
  palette.setColor(QPalette::Highlight, tok.primary);
  palette.setColor(QPalette::HighlightedText, tok.onPrimary);
  palette.setColor(QPalette::Mid, tok.borderPassive);
  palette.setColor(QPalette::Shadow, tok.borderPassive);

  if (auto* splitter = qobject_cast<QSplitter*>(widget); splitter != nullptr) {
    widget->setPalette(palette);
    splitter->setHandleWidth(pixelMetric(PM_SplitterWidth, nullptr, splitter));
    for (int i = 1; i < splitter->count(); ++i) {
      makeSplitterHandleTransparent(splitter->handle(i));
    }
  } else if (auto* handle = qobject_cast<QSplitterHandle*>(widget); handle != nullptr) {
    widget->setPalette(palette);
    makeSplitterHandleTransparent(handle);
  } else if (auto* view = qobject_cast<QAbstractItemView*>(widget); view != nullptr) {
    palette.setColor(QPalette::Base, surfaceRole == SurfaceRole::SidePanel ? windowSurface : tok.surface);
    palette.setColor(QPalette::AlternateBase, palette.color(QPalette::Base));
    view->setPalette(palette);
    view->viewport()->setPalette(palette);
    view->viewport()->setAutoFillBackground(true);
    view->viewport()->setBackgroundRole(QPalette::Base);
  } else if (auto* textEdit = qobject_cast<QTextEdit*>(widget); textEdit != nullptr) {
    palette.setColor(QPalette::Base, tok.surface);
    palette.setColor(QPalette::AlternateBase, tok.surface);
    textEdit->setPalette(palette);
    textEdit->viewport()->setPalette(palette);
    textEdit->viewport()->setAutoFillBackground(true);
    textEdit->viewport()->setBackgroundRole(QPalette::Base);
  } else if (auto* plainTextEdit = qobject_cast<QPlainTextEdit*>(widget); plainTextEdit != nullptr) {
    palette.setColor(QPalette::Base, tok.surface);
    palette.setColor(QPalette::AlternateBase, tok.surface);
    plainTextEdit->setPalette(palette);
    plainTextEdit->viewport()->setPalette(palette);
    plainTextEdit->viewport()->setAutoFillBackground(true);
    plainTextEdit->viewport()->setBackgroundRole(QPalette::Base);
  } else if (surfaceRole == SurfaceRole::Container) {
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);
    widget->setBackgroundRole(QPalette::Window);
  } else {
    widget->setPalette(palette);
    if (surfaceRole == SurfaceRole::SidePanel) {
      widget->setAutoFillBackground(true);
      widget->setBackgroundRole(QPalette::Window);
    }
  }
}

int HoloniightStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const {
  switch (metric) {
    case PM_DefaultFrameWidth:
      return 1;
    case PM_SplitterWidth:
      return 1;
    case PM_DockWidgetSeparatorExtent:
      return 1;
    case PM_ButtonMargin:
      return scaledMetric(6);
    case PM_ButtonDefaultIndicator:
      return 0;
    case PM_MenuBarHMargin:
      return scaledMetric(4);
    case PM_MenuBarVMargin:
      return scaledMetric(2);
    case PM_MenuBarItemSpacing:
      return scaledMetric(6);
    case PM_MenuBarPanelWidth:
      return 0;
    case PM_ToolBarIconSize:
      return scaledMetric(16);
    case PM_SmallIconSize:
      return scaledMetric(16);
    case PM_LargeIconSize:
      return scaledMetric(32);
    case PM_ScrollBarExtent:
      return scaledMetric(8);
    case PM_ScrollBarSliderMin:
      return scaledMetric(20);
    case PM_SliderThickness:
      return scaledMetric(14);
    case PM_SliderLength:
      return scaledMetric(14);
    case PM_TabBarTabHSpace:
      return scaledMetric(16);
    case PM_TabBarTabVSpace:
      return scaledMetric(6);
    case PM_IndicatorWidth:
      return scaledMetric(16);
    case PM_IndicatorHeight:
      return scaledMetric(16);
    case PM_ExclusiveIndicatorWidth:
      return scaledMetric(16);
    case PM_ExclusiveIndicatorHeight:
      return scaledMetric(16);
    default:
      return QProxyStyle::pixelMetric(metric, option, widget);
  }
}

int HoloniightStyle::scaledMetric(int value) const {
  return std::max(1, static_cast<int>(std::lround(static_cast<qreal>(value) * config_.scale_factor)));
}

void HoloniightStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                                  const QWidget* widget) const {
  switch (element) {
    case CE_ShapedFrame: {
      const auto* frameOpt = qstyleoption_cast<const QStyleOptionFrame*>(option);
      if (frameOpt != nullptr) {
        const int frameShape = frameOpt->frameShape;
        if (frameShape == QFrame::HLine || frameShape == QFrame::VLine) {
          const auto tok = tokens();
          painter->save();
          const Qt::Orientation orientation = frameShape == QFrame::HLine ? Qt::Horizontal : Qt::Vertical;
          drawSeparator(painter, option->rect, orientation, tok.borderPassive);
          painter->restore();
          return;
        }
      }
      break;
    }
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
        painter->drawRoundedRect(option->rect.adjusted(2, 1, -2, -1), kRadiusControl, kRadiusControl);
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
        painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
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
      painter->drawRoundedRect(option->rect, kRadiusGroove, kRadiusGroove);
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
      painter->drawRoundedRect(fill, kRadiusGroove, kRadiusGroove);
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
      painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
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
      painter->setRenderHint(QPainter::Antialiasing, false);
      if ((option->state & State_Horizontal) != 0U) {
        drawSeparator(painter, option->rect, Qt::Vertical, tok.borderPassive);
      } else {
        drawSeparator(painter, option->rect, Qt::Horizontal, tok.borderPassive);
      }
      painter->restore();
      return;
    }
    case CE_ItemViewItem: {
      drawItemViewItemImpl(option, painter, widget);
      return;
    }
    default:
      break;
  }

  QProxyStyle::drawControl(element, option, painter, widget);
}

void HoloniightStyle::drawPushButtonBevelImpl(const QStyleOption* option, QPainter* painter) {
  if (drawFlatButtonPanelIfNeeded(option, painter)) {
    return;
  }

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
  painter->drawRoundedRect(option->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), kRadiusControl,
                           kRadiusControl);
  painter->restore();
}

void HoloniightStyle::drawCheckBoxImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionButton*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto& pal = option->palette;
  const bool checked = (opt->state & State_On) != 0U;
  const int side = pixelMetric(PM_IndicatorWidth, option, widget);
  const int top = option->rect.top() + ((option->rect.height() - side) / 2);
  const QRect indicator{option->rect.left(), top, side, side};
  const QRectF borderRect = QRectF(indicator).adjusted(0.5, 0.5, -0.5, -0.5);

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  if (checked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(pal.color(QPalette::Highlight));
    painter->drawRoundedRect(borderRect, kRadiusControl, kRadiusControl);
    painter->setPen(QPen{pal.color(QPalette::HighlightedText), 2.0});
    painter->drawLine(indicator.left() + 4, indicator.top() + 8, indicator.left() + 7, indicator.top() + 11);
    painter->drawLine(indicator.left() + 7, indicator.top() + 11, indicator.left() + 12, indicator.top() + 5);
  } else {
    painter->setPen(QPen{pal.color(QPalette::Mid), 1.0});
    painter->setBrush(pal.color(QPalette::Base));
    painter->drawRoundedRect(borderRect, kRadiusControl, kRadiusControl);
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
  const int side = pixelMetric(PM_ExclusiveIndicatorWidth, option, widget);
  const int top = option->rect.top() + ((option->rect.height() - side) / 2);
  const QRect indicator{option->rect.left(), top, side, side};
  const QRectF borderRect = QRectF(indicator).adjusted(0.5, 0.5, -0.5, -0.5);

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(QPen{pal.color(QPalette::Mid), 1.0});
  painter->setBrush(checked ? pal.color(QPalette::Highlight) : pal.color(QPalette::Base));
  painter->drawEllipse(borderRect);
  if (checked) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(pal.color(QPalette::HighlightedText));
    painter->drawEllipse(QRectF(indicator).adjusted(4.5, 4.5, -4.5, -4.5));
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
  if (drawFlatButtonPanelIfNeeded(option, painter)) {
    return;
  }

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
  painter->drawRoundedRect(option->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), kRadiusControl,
                           kRadiusControl);
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
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusTight, kRadiusTight);
  } else if (hovered) {
    painter->setBrush(tok.surfaceHover);
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusTight, kRadiusTight);
  }
  if (focused && !selected) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen{tok.focusRing, 1});
    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusTight, kRadiusTight);
  }
  painter->restore();
}

void HoloniightStyle::drawItemViewItemImpl(const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionViewItem*>(option);
  if (opt == nullptr) {
    return;
  }

  const auto tok = tokens();
  const bool selected = (opt->state & State_Selected) != 0U;
  const bool hovered = (opt->state & State_MouseOver) != 0U;
  const bool focused = (opt->state & State_HasFocus) != 0U;
  const bool enabled = (opt->state & State_Enabled) != 0U;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);

  if (selected) {
    painter->setBrush(tok.primary);
    painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
  } else if (hovered) {
    painter->setBrush(tok.surfaceHover);
    painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
  } else if ((opt->features & QStyleOptionViewItem::Alternate) != 0U) {
    painter->setBrush(tok.surfaceVariant);
    painter->drawRect(opt->rect);
  }

  QRect iconRect;
  QRect textRect;
  constexpr int kGap = 6;
  const bool hasIcon = !opt->icon.isNull() && ((opt->features & QStyleOptionViewItem::HasDecoration) != 0U);
  const bool decorationOnTop = opt->decorationPosition == QStyleOptionViewItem::Top;
  const QSize iconSize = opt->decorationSize.isValid() ? opt->decorationSize : QSize{16, 16};

  if (hasIcon) {
    if (decorationOnTop) {
      iconRect = QRect{0, 0, iconSize.width(), iconSize.height()};
      iconRect.moveTop(opt->rect.top() + 4);
      iconRect.moveLeft(opt->rect.left() + ((opt->rect.width() - iconRect.width()) / 2));
      textRect = opt->rect.adjusted(4, iconSize.height() + kGap + 4, -4, -2);
    } else {
      iconRect = QRect{0, 0, iconSize.width(), iconSize.height()};
      iconRect.moveLeft(opt->rect.left() + 6);
      iconRect.moveTop(opt->rect.top() + ((opt->rect.height() - iconRect.height()) / 2));
      textRect = opt->rect.adjusted(iconSize.width() + kGap + 10, 0, -4, 0);
    }
    const QIcon::Mode mode = enabled ? QIcon::Normal : QIcon::Disabled;
    const QIcon::State state = selected ? QIcon::On : QIcon::Off;
    opt->icon.paint(painter, iconRect, opt->decorationAlignment, mode, state);
  } else {
    textRect = opt->rect.adjusted(6, 0, -4, 0);
  }

  QColor textColor = enabled ? tok.onSurface : tok.onSurfaceDisabled;
  if (selected) {
    textColor = tok.onPrimary;
  } else if (!enabled) {
    textColor = tok.onSurfaceDisabled;
  } else if ((opt->state & State_Active) == 0U) {
    textColor = tok.onSurfaceVariant;
  }

  painter->setPen(textColor);
  const QFont oldFont = painter->font();
  if (!opt->font.defaultFamily().isEmpty()) {
    painter->setFont(opt->font);
  }
  const QFontMetrics fm{painter->font()};
  const Qt::Alignment alignment =
      decorationOnTop ? Qt::AlignHCenter | Qt::AlignTop : (opt->displayAlignment | Qt::AlignVCenter);
  if ((opt->features & QStyleOptionViewItem::WrapText) != 0U) {
    painter->drawText(textRect, alignment | Qt::TextWordWrap, opt->text);
  } else {
    painter->drawText(textRect, alignment, fm.elidedText(opt->text, opt->textElideMode, textRect.width()));
  }
  painter->setFont(oldFont);

  if (focused && !selected) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen{tok.focusRing, 1});
    painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), kRadiusTight, kRadiusTight);
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

    case PE_PanelButtonTool: {
      const auto* toolOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option);
      const bool flatAutoRaise =
          toolOpt != nullptr ? isFlatAutoRaiseToolButton(toolOpt, widget) : isAutoRaiseToolButton(option, widget);
      const bool hovered = (option->state & State_MouseOver) != 0U;
      const bool pressed = (option->state & State_Sunken) != 0U;
      const bool checked = (option->state & State_On) != 0U;

      if (flatAutoRaise) {
        if (!hovered && !pressed && !checked) {
          return;
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush((pressed || checked) ? tok.surfaceVariant : tok.surfaceHover);
        painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
        painter->restore();
        return;
      }

      drawPanelButtonImpl(option, painter);
      return;
    }

    case PE_FrameDefaultButton: {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(QPen{tok.focusRing, 2});
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(option->rect.adjusted(-1, -1, 1, 1), kRadiusControl, kRadiusControl);
      painter->restore();
      return;
    }

    case PE_Frame: {
      if (shouldSuppressScrollAreaFrame(widget)) {
        return;
      }
      if (isTextEditLikeWidget(widget)) {
        const bool focused = (option->state & State_HasFocus) != 0U;
        const qreal borderWidth = focused ? 2.0 : 1.0;
        drawInputPanel(painter, option->rect, option->palette.color(QPalette::Base),
                       focused ? tok.borderFocus : tok.borderPassive, borderWidth);
        return;
      }
      drawPlainFrame(painter, option->rect, tok.borderPassive);
      return;
    }

    case PE_FrameLineEdit: {
      const bool focused = (option->state & State_HasFocus) != 0U;
      const qreal borderWidth = focused ? 2.0 : 1.0;
      drawInputPanel(painter, option->rect, option->palette.color(QPalette::Base),
                     focused ? tok.borderFocus : tok.borderPassive, borderWidth);
      return;
    }

    case PE_FrameTabWidget: {
      drawPlainFrame(painter, option->rect, tok.borderPassive);
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

    case PE_Widget:
    case PE_FrameWindow: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(option->palette.color(QPalette::Window));
      painter->drawRect(option->rect);
      painter->restore();
      return;
    }

    case PE_PanelToolBar: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surface);
      painter->drawRect(option->rect);

      // Draw bottom separator line using fillRect to prevent clipping and ensure consistent thickness
      QRect borderRect = option->rect;
      borderRect.setTop(option->rect.bottom());
      borderRect.setHeight(1);
      painter->fillRect(borderRect, tok.borderPassive);

      painter->restore();
      return;
    }

    case PE_FrameMenu: {
      drawPlainFrame(painter, option->rect, tok.borderPassive);
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
      painter->setBrush(tok.surfaceInverse);
      painter->setPen(Qt::NoPen);
      painter->drawRect(option->rect);
      painter->restore();
      drawPlainFrame(painter, option->rect, tok.borderPassive);
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

    case PE_PanelItemViewRow: {
      const bool selected = (option->state & State_Selected) != 0U;
      const bool hovered = (option->state & State_MouseOver) != 0U;
      if (!selected && !hovered) {
        return;
      }

      painter->save();
      painter->setPen(Qt::NoPen);
      if (selected) {
        painter->setBrush(tok.primary);
      } else {
        painter->setBrush(tok.surfaceHover);
      }
      painter->drawRect(option->rect);
      painter->restore();
      return;
    }

    case PE_PanelLineEdit: {
      const bool focused = (option->state & State_HasFocus) != 0U;
      const bool enabled = (option->state & State_Enabled) != 0U;
      const int borderWidth = focused ? 2 : 1;
      QColor border = tok.borderPassive;
      if (focused) {
        border = tok.borderFocus;
      } else if (!enabled) {
        border = tok.borderPassive;  // TODO(borderUrgent): if error state added, use tok.borderUrgent here
      }
      drawInputPanel(painter, option->rect, option->palette.color(QPalette::Base), border, borderWidth);
      return;
    }

    case PE_FrameGroupBox: {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(QPen{tok.borderPassive, 1});
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(strokedRectInside(option->rect, 1.0), kRadiusControl, kRadiusControl);
      painter->restore();
      return;
    }

    case PE_PanelStatusBar: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surfaceVariant);
      painter->drawRect(option->rect);
      painter->restore();
      drawPlainFrame(painter, option->rect, tok.borderPassive);
      return;
    }

    case PE_FrameStatusBarItem: {
      painter->save();
      painter->setPen(Qt::NoPen);
      painter->setBrush(tok.surfaceVariant);
      painter->drawRect(option->rect);
      painter->restore();
      drawPlainFrame(painter, option->rect, tok.borderPassive);
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
    case CC_ScrollBar:
      drawScrollBarImpl(option, painter, widget);
      return;
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
      painter->drawRoundedRect(opt->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), kRadiusControl,
                               kRadiusControl);
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

QRect HoloniightStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl,
                                      const QWidget* widget) const {
  if (control == CC_ScrollBar) {
    const auto* opt = qstyleoption_cast<const QStyleOptionSlider*>(option);
    if (opt == nullptr) {
      return {};
    }
    const bool horizontal = opt->orientation == Qt::Horizontal;
    const QRect grooveRect =
        option->rect.adjusted(horizontal ? 2 : 1, horizontal ? 1 : 2, horizontal ? -2 : -1, horizontal ? -1 : -2);
    if (subControl == SC_ScrollBarGroove || subControl == SC_ScrollBarAddPage || subControl == SC_ScrollBarSubPage) {
      return grooveRect;
    }
    if (subControl == SC_ScrollBarAddLine || subControl == SC_ScrollBarSubLine || subControl == SC_ScrollBarFirst ||
        subControl == SC_ScrollBarLast) {
      return {};
    }
    if (subControl == SC_ScrollBarSlider) {
      const int range = std::max(0, opt->maximum - opt->minimum);
      const int pageStep = std::max(1, opt->pageStep);
      const int span = horizontal ? grooveRect.width() : grooveRect.height();
      const int minSliderLength = pixelMetric(PM_ScrollBarSliderMin, opt, widget);
      int sliderLength = range == 0 ? span : (span * pageStep) / (range + pageStep);
      sliderLength = std::clamp(sliderLength, std::min(minSliderLength, span), span);

      const int available = std::max(0, span - sliderLength);
      const int sliderPos = range == 0 ? 0 : (available * (opt->sliderPosition - opt->minimum)) / range;
      QRect handleRect = grooveRect;
      if (horizontal) {
        handleRect.setLeft(grooveRect.left() + sliderPos);
        handleRect.setWidth(sliderLength);
        return handleRect.adjusted(0, 1, 0, -1);
      }
      handleRect.setTop(grooveRect.top() + sliderPos);
      handleRect.setHeight(sliderLength);
      return handleRect.adjusted(1, 0, -1, 0);
    }
  }
  return QProxyStyle::subControlRect(control, option, subControl, widget);
}

void HoloniightStyle::drawScrollBarImpl(const QStyleOptionComplex* option, QPainter* painter,
                                        const QWidget* widget) const {
  const auto* opt = qstyleoption_cast<const QStyleOptionSlider*>(option);
  if (opt == nullptr) {
    return;
  }
  const auto tok = tokens();
  const bool horizontal = opt->orientation == Qt::Horizontal;
  const QRect grooveRect =
      option->rect.adjusted(horizontal ? 2 : 1, horizontal ? 1 : 2, horizontal ? -2 : -1, horizontal ? -1 : -2);
  if (!grooveRect.isValid()) {
    return;
  }

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);
  painter->setBrush(tok.surface);
  painter->drawRect(option->rect);

  QRect trackRect = grooveRect;
  if (horizontal) {
    trackRect.setHeight(4);
    trackRect.moveCenter({grooveRect.center().x(), grooveRect.center().y()});
  } else {
    trackRect.setWidth(4);
    trackRect.moveCenter({grooveRect.center().x(), grooveRect.center().y()});
  }
  painter->setBrush(tok.surfaceVariant);
  painter->drawRoundedRect(trackRect, kRadiusTight, kRadiusTight);

  const QRect handleRect = subControlRect(CC_ScrollBar, opt, SC_ScrollBarSlider, widget);

  QColor handleColor = tok.borderPassive;
  if (((opt->state & State_Sunken) != 0U) && ((opt->activeSubControls & SC_ScrollBarSlider) != 0U)) {
    handleColor = tok.primaryPressed;
  } else if (((opt->state & State_MouseOver) != 0U) || ((opt->activeSubControls & SC_ScrollBarSlider) != 0U)) {
    handleColor = tok.primaryHover;
  }
  painter->setBrush(handleColor);
  painter->drawRoundedRect(handleRect, kRadiusControl, kRadiusControl);
  painter->restore();
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
    painter->drawRoundedRect(fullGroove, kRadiusTight, kRadiusTight);
    painter->setBrush(tok.primary);
    const QRect filledGroove{grooveRect.left(), grooveRect.center().y() - 2,
                             handleRect.center().x() - grooveRect.left(), 4};
    if (filledGroove.width() > 0) {
      painter->drawRoundedRect(filledGroove, kRadiusTight, kRadiusTight);
    }
  } else {
    const QRect fullGroove{grooveRect.center().x() - 2, grooveRect.top(), 4, grooveRect.height()};
    painter->drawRoundedRect(fullGroove, kRadiusTight, kRadiusTight);
    painter->setBrush(tok.primary);
    const int filledTop = handleRect.center().y();
    const QRect filledGroove{grooveRect.center().x() - 2, filledTop, 4, grooveRect.bottom() - filledTop};
    if (filledGroove.height() > 0) {
      painter->drawRoundedRect(filledGroove, kRadiusTight, kRadiusTight);
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
  painter->drawRoundedRect(handleRect.adjusted(1, 1, -1, -1), kRadiusTight, kRadiusTight);
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
  painter->drawRoundedRect(opt->rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth), kRadiusControl,
                           kRadiusControl);

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
  const bool idleFlatAutoRaise = isFlatAutoRaiseToolButton(opt, widget) && !hovered && !pressed && !checked;

  QColor bgColor = Qt::transparent;
  if (pressed || checked) {
    bgColor = tok.surfaceVariant;
  } else if (hovered) {
    bgColor = tok.surfaceHover;
  }
  painter->setPen(Qt::NoPen);
  painter->setBrush(bgColor);
  painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);

  if (focused && !idleFlatAutoRaise) {
    painter->setPen(QPen{tok.focusRing, 1, Qt::DashLine});
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(opt->rect.adjusted(1, 1, -1, -1), kRadiusControl, kRadiusControl);
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
  painter->drawRoundedRect(frameRect.adjusted(0, 0, -1, -1), kRadiusControl, kRadiusControl);

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
