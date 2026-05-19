// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QStyleOption>
#include <QStyleOptionViewItem>

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

TEST_F(DrawPrimitiveTest, FrameDefaultButton_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameDefaultButton, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, Frame_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter_);
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

TEST_F(DrawPrimitiveTest, PanelLineEdit_Normal) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelLineEdit_Focused) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  style_.drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, FrameGroupBox_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameGroupBox, &opt, &painter_);
}
