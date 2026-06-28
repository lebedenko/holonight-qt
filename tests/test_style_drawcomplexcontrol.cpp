// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"

#include <QApplication>
#include <QDockWidget>
#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QSplitter>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionComboBox>
#include <QStyleOptionGroupBox>
#include <QStyleOptionSlider>
#include <QStyleOptionSpinBox>
#include <QStyleOptionToolButton>
#include <QStyleOptionViewItem>

#include <gtest/gtest.h>

class DrawComplexTest : public ::testing::Test {
 protected:
  HoloniightStyle style_;
  QPixmap pixmap_ = QPixmap{120, 40};
  QPainter painter_ = QPainter{&pixmap_};
};

QColor pixelColorAfterDrawControl(HoloniightStyle* style, QStyle::ControlElement element, const QStyleOption* opt) {
  QImage image = QImage{opt->rect.size(), QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};
  style->drawControl(element, opt, &painter);
  painter.end();
  return image.pixelColor(opt->rect.center());
}

TEST_F(DrawComplexTest, ComboBox_Normal) {
  QStyleOptionComboBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  style_.drawComplexControl(QStyle::CC_ComboBox, &opt, &painter_);
}

TEST_F(DrawComplexTest, ComboBox_Focused) {
  QStyleOptionComboBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_HasFocus;
  style_.drawComplexControl(QStyle::CC_ComboBox, &opt, &painter_);
}

TEST_F(DrawComplexTest, Slider_Horizontal) {
  QStyleOptionSlider opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.orientation = Qt::Horizontal;
  opt.minimum = 0;
  opt.maximum = 100;
  opt.sliderValue = 40;
  opt.sliderPosition = 40;
  opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
  style_.drawComplexControl(QStyle::CC_Slider, &opt, &painter_);
}

TEST_F(DrawComplexTest, Slider_Vertical) {
  QPixmap vPixmap = QPixmap{40, 120};
  QPainter vPainter = QPainter{&vPixmap};
  QStyleOptionSlider opt;
  opt.rect = vPixmap.rect();
  opt.state = QStyle::State_Enabled;
  opt.orientation = Qt::Vertical;
  opt.minimum = 0;
  opt.maximum = 100;
  opt.sliderValue = 60;
  opt.sliderPosition = 60;
  opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
  style_.drawComplexControl(QStyle::CC_Slider, &opt, &vPainter);
}

TEST_F(DrawComplexTest, ScrollBar_Vertical) {
  QPixmap vPixmap = QPixmap{16, 120};
  QPainter vPainter = QPainter{&vPixmap};
  QStyleOptionSlider opt;
  opt.rect = vPixmap.rect();
  opt.state = QStyle::State_Enabled;
  opt.orientation = Qt::Vertical;
  opt.minimum = 0;
  opt.maximum = 100;
  opt.pageStep = 20;
  opt.sliderPosition = 40;
  opt.subControls = QStyle::SC_All;
  style_.drawComplexControl(QStyle::CC_ScrollBar, &opt, &vPainter);
}

TEST_F(DrawComplexTest, ScrollBar_Horizontal) {
  QStyleOptionSlider opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;
  opt.orientation = Qt::Horizontal;
  opt.minimum = 0;
  opt.maximum = 100;
  opt.pageStep = 20;
  opt.sliderPosition = 40;
  opt.subControls = QStyle::SC_All;
  style_.drawComplexControl(QStyle::CC_ScrollBar, &opt, &painter_);
}

TEST_F(DrawComplexTest, ScrollBar_SubControlRectHasNoLineButtons) {
  QStyleOptionSlider opt;
  opt.rect = QRect{0, 0, 16, 120};
  opt.state = QStyle::State_Enabled;
  opt.orientation = Qt::Vertical;
  opt.minimum = 0;
  opt.maximum = 100;
  opt.pageStep = 20;
  opt.sliderPosition = 40;
  EXPECT_TRUE(style_.subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarAddLine).isEmpty());
  EXPECT_TRUE(style_.subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSubLine).isEmpty());
  EXPECT_FALSE(style_.subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider).isEmpty());
}

TEST_F(DrawComplexTest, SpinBox_Normal) {
  QStyleOptionSpinBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.subControls =
      QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown | QStyle::SC_SpinBoxEditField;
  style_.drawComplexControl(QStyle::CC_SpinBox, &opt, &painter_);
}

TEST_F(DrawComplexTest, SpinBox_Disabled) {
  QStyleOptionSpinBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_None;
  opt.subControls =
      QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown | QStyle::SC_SpinBoxEditField;
  style_.drawComplexControl(QStyle::CC_SpinBox, &opt, &painter_);
}

TEST_F(DrawComplexTest, ToolButton_Normal) {
  QStyleOptionToolButton opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.subControls = QStyle::SC_ToolButton;
  opt.features = QStyleOptionToolButton::None;
  style_.drawComplexControl(QStyle::CC_ToolButton, &opt, &painter_);
}

TEST_F(DrawComplexTest, FlatFocusedPushButtonBevelIsTransparent) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

  QStyleOptionButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_HasFocus;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionButton::Flat;
  style_.drawControl(QStyle::CE_PushButtonBevel, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixel(image.width() / 2, 1), background);
  EXPECT_EQ(image.pixel(1, image.height() / 2), background);
  EXPECT_EQ(image.pixel(image.rect().center()), background);
}

TEST_F(DrawComplexTest, ShapedFrameSeparatorsUseSameColor) {
  const QColor borderColor = style_.standardPalette().color(QPalette::Mid);

  QStyleOptionFrame horizontalOpt;
  horizontalOpt.rect = QRect{0, 0, 40, 7};
  horizontalOpt.palette = style_.standardPalette();
  horizontalOpt.frameShape = QFrame::HLine;

  QStyleOptionFrame verticalOpt;
  verticalOpt.rect = QRect{0, 0, 7, 40};
  verticalOpt.palette = style_.standardPalette();
  verticalOpt.frameShape = QFrame::VLine;

  EXPECT_EQ(pixelColorAfterDrawControl(&style_, QStyle::CE_ShapedFrame, &horizontalOpt), borderColor);
  EXPECT_EQ(pixelColorAfterDrawControl(&style_, QStyle::CE_ShapedFrame, &verticalOpt), borderColor);
}

TEST_F(DrawComplexTest, SplitterPaintsOnlyCenteredSeparatorLine) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image = QImage{7, 40, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

  QStyleOption splitterOpt;
  splitterOpt.rect = image.rect();
  splitterOpt.state = QStyle::State_Enabled | QStyle::State_Horizontal;
  splitterOpt.palette = style_.standardPalette();
  style_.drawControl(QStyle::CE_Splitter, &splitterOpt, &painter);
  painter.end();

  EXPECT_EQ(image.pixelColor(splitterOpt.rect.center()), style_.standardPalette().color(QPalette::Mid));
  EXPECT_EQ(image.pixel(splitterOpt.rect.left(), splitterOpt.rect.center().y()), background);
  EXPECT_EQ(image.pixel(splitterOpt.rect.right(), splitterOpt.rect.center().y()), background);
}

TEST_F(DrawComplexTest, RenderedSplitterUsesOnePixelHandleBetweenDifferentSurfaces) {
  const QColor leftColor{0x20, 0x30, 0x40};
  const QColor rightColor{0x40, 0x30, 0x20};

  QSplitter splitter;
  splitter.setStyle(&style_);
  splitter.setOrientation(Qt::Horizontal);

  QWidget leftPane;
  QWidget rightPane;
  splitter.addWidget(&leftPane);
  splitter.addWidget(&rightPane);
  style_.polish(&splitter);
  splitter.resize(101, 40);
  splitter.setSizes({50, 50});
  splitter.show();
  QApplication::processEvents();

  QPalette leftPalette = style_.standardPalette();
  QPalette rightPalette = style_.standardPalette();
  leftPalette.setColor(QPalette::Window, leftColor);
  rightPalette.setColor(QPalette::Window, rightColor);
  leftPane.setPalette(leftPalette);
  leftPane.setAutoFillBackground(true);
  leftPane.setBackgroundRole(QPalette::Window);
  rightPane.setPalette(rightPalette);
  rightPane.setAutoFillBackground(true);
  rightPane.setBackgroundRole(QPalette::Window);

  const QSplitterHandle* handle = splitter.handle(1);
  ASSERT_NE(handle, nullptr);
  const QRect handleRect = handle->geometry();
  EXPECT_EQ(splitter.handleWidth(), 1);

  QImage image = QImage{splitter.size(), QImage::Format_ARGB32_Premultiplied};
  image.fill(qRgb(12, 34, 56));
  QPainter painter = QPainter{&image};
  splitter.render(&painter);
  painter.end();

  const int y = handleRect.center().y();
  EXPECT_EQ(image.pixelColor(handleRect.left(), y), leftColor);
  EXPECT_EQ(image.pixelColor(handleRect.center()), style_.standardPalette().color(QPalette::Mid));
  EXPECT_EQ(image.pixelColor(handleRect.right(), y), rightColor);
}

TEST_F(DrawComplexTest, MainWindowDockSeparatorIsOnePixelWide) {
  const QColor centralColor{0x20, 0x30, 0x40};
  const QColor dockColor{0x40, 0x30, 0x20};

  QMainWindow window;
  window.setStyle(&style_);

  QWidget central;
  QPalette centralPalette = style_.standardPalette();
  centralPalette.setColor(QPalette::Window, centralColor);
  central.setPalette(centralPalette);
  central.setAutoFillBackground(true);
  window.setCentralWidget(&central);

  auto* dock = new QDockWidget;
  dock->setTitleBarWidget(new QWidget);
  auto* dockContent = new QWidget;
  QPalette dockPalette = style_.standardPalette();
  dockPalette.setColor(QPalette::Window, dockColor);
  dockContent->setPalette(dockPalette);
  dockContent->setAutoFillBackground(true);
  dock->setWidget(dockContent);
  window.addDockWidget(Qt::LeftDockWidgetArea, dock);

  window.resize(101, 40);
  window.show();
  QApplication::processEvents();

  QImage image = QImage{window.size(), QImage::Format_ARGB32_Premultiplied};
  image.fill(qRgb(12, 34, 56));
  QPainter painter = QPainter{&image};
  window.render(&painter);
  painter.end();

  int separatorPixels = 0;
  const int y = image.height() / 2;
  const QColor separatorColor = style_.standardPalette().color(QPalette::Mid);
  for (int x = 0; x < image.width(); ++x) {
    if (image.pixelColor(x, y) == separatorColor) {
      ++separatorPixels;
    }
  }

  EXPECT_EQ(separatorPixels, 1);
}

TEST_F(DrawComplexTest, FlatIdleToolButtonDoesNotPaintBorder) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

  QStyleOptionToolButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled;
  opt.palette = style_.standardPalette();
  opt.subControls = QStyle::SC_ToolButton;
  opt.features = QStyleOptionToolButton::None;
  opt.toolButtonStyle = Qt::ToolButtonIconOnly;
  style_.drawComplexControl(QStyle::CC_ToolButton, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixel(0, 0), background);
  EXPECT_EQ(image.pixel(image.width() - 1, 0), background);
  EXPECT_EQ(image.pixel(0, image.height() - 1), background);
  EXPECT_EQ(image.pixel(image.width() - 1, image.height() - 1), background);
}

TEST_F(DrawComplexTest, FlatFocusedAutoRaiseToolButtonDoesNotPaintBorder) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

  QStyleOptionToolButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_HasFocus | QStyle::State_AutoRaise;
  opt.palette = style_.standardPalette();
  opt.subControls = QStyle::SC_ToolButton;
  opt.features = QStyleOptionToolButton::None;
  opt.toolButtonStyle = Qt::ToolButtonIconOnly;
  style_.drawComplexControl(QStyle::CC_ToolButton, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixel(image.width() / 2, 1), background);
  EXPECT_EQ(image.pixel(1, image.height() / 2), background);
}

TEST_F(DrawComplexTest, FocusedNonFlatToolButtonKeepsFocusBorder) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

  QStyleOptionToolButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_HasFocus;
  opt.palette = style_.standardPalette();
  opt.subControls = QStyle::SC_ToolButton;
  opt.features = QStyleOptionToolButton::None;
  opt.toolButtonStyle = Qt::ToolButtonIconOnly;
  style_.drawComplexControl(QStyle::CC_ToolButton, &opt, &painter);
  painter.end();

  EXPECT_NE(image.pixel(image.width() / 2, 1), background);
}

TEST_F(DrawComplexTest, ToolButton_WithMenu) {
  QStyleOptionToolButton opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.subControls = QStyle::SC_ToolButton | QStyle::SC_ToolButtonMenu;
  opt.features = QStyleOptionToolButton::MenuButtonPopup;
  style_.drawComplexControl(QStyle::CC_ToolButton, &opt, &painter_);
}

TEST_F(DrawComplexTest, GroupBox_Normal) {
  QStyleOptionGroupBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.text = "Group";
  opt.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
  style_.drawComplexControl(QStyle::CC_GroupBox, &opt, &painter_);
}

TEST_F(DrawComplexTest, ItemViewItem_Selected) {
  QStyleOptionViewItem opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_Selected;
  opt.text = "Item";
  opt.palette = style_.standardPalette();
  style_.drawControl(QStyle::CE_ItemViewItem, &opt, &painter_);
}

TEST_F(DrawComplexTest, ToolBar_NoAssertViolation) {
  QStyleOption opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.palette = style_.standardPalette();
  style_.drawControl(QStyle::CE_ToolBar, &opt, &painter_);
}

TEST_F(DrawComplexTest, SizeFromContents_ComboBox_MinHeight32) {
  QStyleOptionComboBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  const QSize result = style_.sizeFromContents(QStyle::CT_ComboBox, &opt, QSize{80, 20});
  EXPECT_GE(result.height(), 32);
  EXPECT_GT(result.width(), 80);
}

TEST_F(DrawComplexTest, SizeFromContents_SpinBox_MinHeight32) {
  QStyleOptionSpinBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  const QSize result = style_.sizeFromContents(QStyle::CT_SpinBox, &opt, QSize{80, 20});
  EXPECT_GE(result.height(), 32);
  EXPECT_GT(result.width(), 80);
}

TEST_F(DrawComplexTest, SubElementRect_LineEditContents_HasInset) {
  QStyleOption opt;
  opt.rect = QRect{0, 0, 200, 32};
  const QRect result = style_.subElementRect(QStyle::SE_LineEditContents, &opt);
  EXPECT_GT(result.left(), opt.rect.left());
  EXPECT_LT(result.right(), opt.rect.right());
}

TEST_F(DrawComplexTest, SubElementRect_CheckBoxIndicator_16x16) {
  QStyleOption opt;
  opt.rect = QRect{0, 0, 120, 24};
  const QRect result = style_.subElementRect(QStyle::SE_CheckBoxIndicator, &opt);
  EXPECT_EQ(result.width(), 16);
  EXPECT_EQ(result.height(), 16);
}

TEST_F(DrawComplexTest, SubElementRect_RadioButtonIndicator_16x16) {
  QStyleOption opt;
  opt.rect = QRect{0, 0, 120, 24};
  const QRect result = style_.subElementRect(QStyle::SE_RadioButtonIndicator, &opt);
  EXPECT_EQ(result.width(), 16);
  EXPECT_EQ(result.height(), 16);
}
