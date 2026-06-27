// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"
#include "holonight/palette.h"

#include <QApplication>
#include <QImage>
#include <QListView>
#include <QPainter>
#include <QPixmap>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionToolButton>
#include <QStyleOptionViewItem>
#include <QTextEdit>

#include <gtest/gtest.h>

class DrawPrimitiveTest : public ::testing::Test {
 protected:
  HoloniightStyle style_;
  QPixmap pixmap_{100, 100};
  QPainter painter_{&pixmap_};

  QStyleOption makeOpt(QStyle::State state = QStyle::State_Enabled) {
    QStyleOption opt;
    opt.rect = pixmap_.rect();
    opt.state = state;
    opt.palette = style_.standardPalette();
    return opt;
  }
};

TEST_F(DrawPrimitiveTest, PanelButtonCommand_Normal) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelButtonCommand_Focused) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, FlatFocusedButtonPanelIsTransparent) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter{&image};

  QStyleOptionButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_HasFocus;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionButton::Flat;
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixel(image.width() / 2, 1), background);
  EXPECT_EQ(image.pixel(1, image.height() / 2), background);
  EXPECT_EQ(image.pixel(image.rect().center()), background);
}

TEST_F(DrawPrimitiveTest, FlatHoveredButtonPanelPaintsFillOnly) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter{&image};

  QStyleOptionButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionButton::Flat;
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter);
  painter.end();

  EXPECT_NE(image.pixel(image.rect().center()), background);
  EXPECT_EQ(image.pixel(0, 0), background);
}

TEST_F(DrawPrimitiveTest, FlatFocusedAutoRaiseToolButtonPanelIsTransparent) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter{&image};

  QStyleOptionToolButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_HasFocus | QStyle::State_AutoRaise;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionToolButton::None;
  style_.drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixel(image.width() / 2, 1), background);
  EXPECT_EQ(image.pixel(1, image.height() / 2), background);
  EXPECT_EQ(image.pixel(image.rect().center()), background);
}

TEST_F(DrawPrimitiveTest, FlatHoveredAutoRaiseToolButtonPanelPaintsFillOnly) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter{&image};

  QStyleOptionToolButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_AutoRaise | QStyle::State_MouseOver;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionToolButton::None;
  style_.drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &painter);
  painter.end();

  EXPECT_NE(image.pixel(image.rect().center()), background);
  EXPECT_EQ(image.pixel(0, 0), background);
}

TEST_F(DrawPrimitiveTest, FrameDefaultButton_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameDefaultButton, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, Frame_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, FramePaintsInsetBorder) {
  QImage image{12, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter);
  painter.end();

  const QColor borderColor = style_.standardPalette().color(QPalette::Mid);
  EXPECT_EQ(image.pixelColor(6, 0), borderColor);
  EXPECT_EQ(image.pixelColor(6, image.height() - 1), borderColor);
  EXPECT_EQ(image.pixelColor(0, 6), borderColor);
  EXPECT_EQ(image.pixelColor(image.width() - 1, 6), borderColor);
  EXPECT_EQ(image.pixelColor(image.rect().center()).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, FramePaintsOnlyOnePixelEdges) {
  QImage image{12, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter);
  painter.end();

  const QColor borderColor = style_.standardPalette().color(QPalette::Mid);
  EXPECT_EQ(image.pixelColor(6, 0), borderColor);
  EXPECT_EQ(image.pixelColor(6, 1).alpha(), 0);
  EXPECT_EQ(image.pixelColor(0, 6), borderColor);
  EXPECT_EQ(image.pixelColor(1, 6).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, ItemViewFrameIsSuppressed) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image{12, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter{&image};

  QListView view;
  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &view);
  painter.end();

  EXPECT_EQ(image.pixel(6, 0), background);
  EXPECT_EQ(image.pixel(0, 6), background);
}

TEST_F(DrawPrimitiveTest, TextEditScrollAreaFrameUsesRoundedInsetStroke) {
  QImage image{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  QTextEdit edit;
  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Base, Holonight::darkTokens().surface);
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &edit);
  painter.end();

  EXPECT_GT(image.pixelColor(image.width() / 2, 0).alpha(), 0);
  EXPECT_GT(image.pixelColor(0, image.height() / 2).alpha(), 0);
  EXPECT_EQ(image.pixelColor(image.rect().center()), Holonight::darkTokens().surface);
}

TEST_F(DrawPrimitiveTest, FocusedTextEditFrameUsesFocusColor) {
  QImage image{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  QTextEdit edit;
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &edit);
  painter.end();

  const QColor focusColor = Holonight::darkTokens().borderFocus;
  EXPECT_EQ(image.pixelColor(image.width() / 2, 1), focusColor);
}

TEST_F(DrawPrimitiveTest, FrameFocusRect_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameFocusRect, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorArrowDown_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorArrowUp_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_IndicatorArrowUp, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorArrowLeft_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_IndicatorArrowLeft, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorArrowRight_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_IndicatorArrowRight, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorBranch_Expanded) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_Children | QStyle::State_Open);
  style_.drawPrimitive(QStyle::PE_IndicatorBranch, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorBranch_Collapsed) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_Children);
  style_.drawPrimitive(QStyle::PE_IndicatorBranch, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, IndicatorToolBarSeparator_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelItemViewItem_Selected) {
  QStyleOptionViewItem opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_Selected;
  style_.drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelItemViewItem_Hover) {
  QStyleOptionViewItem opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;
  style_.drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelItemViewRow_Selected) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_Selected);
  style_.drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelItemViewRow_IdleIsTransparent) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image{20, 20, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixel(image.rect().center()), background);
}

TEST_F(DrawPrimitiveTest, PanelToolBar_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_PanelToolBar, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelStatusBarUsesVariantSurfaceWithBorder) {
  QImage image{24, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_PanelStatusBar, &opt, &painter);
  painter.end();

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceVariant);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
}

TEST_F(DrawPrimitiveTest, FrameStatusBarItemUsesVariantSurfaceWithBorder) {
  QImage image{24, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_FrameStatusBarItem, &opt, &painter);
  painter.end();

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceVariant);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
}

TEST_F(DrawPrimitiveTest, Widget_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_Widget, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, WidgetUsesPaletteWindowSurface) {
  const QColor windowColor{0x22, 0x33, 0x44};
  QImage image{20, 20, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Window, windowColor);
  style_.drawPrimitive(QStyle::PE_Widget, &opt, &painter);
  painter.end();

  EXPECT_EQ(image.pixelColor(image.rect().center()), windowColor);
}

TEST_F(DrawPrimitiveTest, FrameWindow_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameWindow, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelLineEdit_Normal) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelLineEdit_Focused) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  style_.drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelLineEditFocusedBorderUsesInsetStroke) {
  QImage image{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Base, Holonight::darkTokens().surface);
  style_.drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter);
  painter.end();

  const QColor focusColor = Holonight::darkTokens().borderFocus;
  const QColor fillColor = Holonight::darkTokens().surface;
  EXPECT_EQ(image.pixelColor(image.width() / 2, 1), focusColor);
  EXPECT_EQ(image.pixelColor(image.rect().center()), fillColor);
}

TEST_F(DrawPrimitiveTest, FrameLineEditUsesDarkFillAndRoundedBorder) {
  QImage image{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Base, Holonight::darkTokens().surface);
  style_.drawPrimitive(QStyle::PE_FrameLineEdit, &opt, &painter);
  painter.end();

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surface);
  EXPECT_GT(image.pixelColor(image.width() / 2, 0).alpha(), 0);
  EXPECT_GT(image.pixelColor(0, image.height() / 2).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, FrameGroupBox_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameGroupBox, &opt, &painter_);
}
