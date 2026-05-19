// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QStyleOption>
#include <QStyleOptionComboBox>
#include <QStyleOptionGroupBox>
#include <QStyleOptionSlider>
#include <QStyleOptionSpinBox>
#include <QStyleOptionToolButton>

#include <gtest/gtest.h>

class DrawComplexTest : public ::testing::Test {
 protected:
  HoloniightStyle style_;
  QPixmap pixmap_{120, 40};
  QPainter painter_{&pixmap_};
};

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
  QPixmap vPixmap{40, 120};
  QPainter vPainter{&vPixmap};
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

TEST_F(DrawComplexTest, SpinBox_Normal) {
  QStyleOptionSpinBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_Enabled;
  opt.subControls = QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown | QStyle::SC_SpinBoxEditField;
  style_.drawComplexControl(QStyle::CC_SpinBox, &opt, &painter_);
}

TEST_F(DrawComplexTest, SpinBox_Disabled) {
  QStyleOptionSpinBox opt;
  opt.rect = pixmap_.rect();
  opt.state = QStyle::State_None;
  opt.subControls = QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown | QStyle::SC_SpinBoxEditField;
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
