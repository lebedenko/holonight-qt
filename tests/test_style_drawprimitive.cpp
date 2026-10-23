// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"
#include "holonight/palette.h"
#include "themeresolver.h"

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
  // GTest-derived tests intentionally share protected fixture state.
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  HoloniightStyle style_;
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QPixmap pixmap_ = QPixmap{100, 100};
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QPainter painter_ = QPainter{&pixmap_};

  QStyleOption makeOpt(QStyle::State state = QStyle::State_Enabled) {
    QStyleOption opt;
    opt.rect = pixmap_.rect();
    opt.state = state;
    opt.palette = style_.standardPalette();
    return opt;
  }
};

Holonight::ColorTokens resolvedDefaultTokens() {
  Holonight::ResolvedAppearance appearance;
  appearance.theme_scheme = Holonight::ThemeSchemeKind::HoloNightDark;
  appearance.accent = QStringLiteral("blue");
  return Holonight::ThemeResolver::resolve(appearance);
}

TEST_F(DrawPrimitiveTest, PanelButtonCommand_Normal) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelButtonCommand_Focused) {
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelButtonCommand_DefaultButtonPaintsPrimary) {
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::black);
  QPainter painter = QPainter{&image};

  QStyleOptionButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionButton::DefaultButton;
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter);
  painter.end();

  const QRgb centerPixel = image.pixel(image.rect().center());
  EXPECT_NE(centerPixel, qRgb(0, 0, 0));
  EXPECT_EQ(QColor::fromRgb(centerPixel).rgb(), resolvedDefaultTokens().primary.rgb());
}

TEST_F(DrawPrimitiveTest, PanelButtonCommand_DefaultButtonHoveredPaintsPrimaryHover) {
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::black);
  QPainter painter = QPainter{&image};

  QStyleOptionButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionButton::DefaultButton;
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter);
  painter.end();

  const QRgb centerPixel = image.pixel(image.rect().center());
  EXPECT_EQ(QColor::fromRgb(centerPixel).rgb(), resolvedDefaultTokens().primaryHover.rgb());
}

TEST_F(DrawPrimitiveTest, PanelButtonCommand_DefaultButtonPressedPaintsPrimaryPressed) {
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::black);
  QPainter painter = QPainter{&image};

  QStyleOptionButton opt;
  opt.rect = image.rect();
  opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
  opt.palette = style_.standardPalette();
  opt.features = QStyleOptionButton::DefaultButton;
  style_.drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &painter);
  painter.end();

  const QRgb centerPixel = image.pixel(image.rect().center());
  EXPECT_EQ(QColor::fromRgb(centerPixel).rgb(), resolvedDefaultTokens().primaryPressed.rgb());
}

TEST_F(DrawPrimitiveTest, FlatFocusedButtonPanelIsTransparent) {
  constexpr QRgb background = qRgb(12, 34, 56);
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{40, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{12, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{12, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{12, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

  QListView view;
  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &view);
  painter.end();

  EXPECT_EQ(image.pixel(6, 0), background);
  EXPECT_EQ(image.pixel(0, 6), background);
}

TEST_F(DrawPrimitiveTest, TextEditScrollAreaFrameUsesRoundedInsetStroke) {
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  QTextEdit edit;
  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Base, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).surface);
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &edit);
  painter.end();

  EXPECT_GT(image.pixelColor(image.width() / 2, 0).alpha(), 0);
  EXPECT_GT(image.pixelColor(0, image.height() / 2).alpha(), 0);
  EXPECT_EQ(image.pixelColor(image.rect().center()),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).surface);
}

TEST_F(DrawPrimitiveTest, FocusedTextEditFrameUsesFocusColor) {
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  QTextEdit edit;
  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &edit);
  painter.end();

  const QColor focusColor = resolvedDefaultTokens().borderFocus;
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), focusColor);
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
  QImage image = QImage{20, 20, QImage::Format_ARGB32_Premultiplied};
  image.fill(background);
  QPainter painter = QPainter{&image};

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

TEST_F(DrawPrimitiveTest, PanelStatusBarUsesElevatedSurfaceWithBorder) {
  QImage image = QImage{24, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_PanelStatusBar, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceElevated);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
}

TEST_F(DrawPrimitiveTest, FrameStatusBarItemUsesElevatedSurfaceWithBorder) {
  QImage image = QImage{24, 12, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_FrameStatusBarItem, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceElevated);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
}

TEST_F(DrawPrimitiveTest, Widget_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_Widget, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, WidgetUsesPaletteWindowSurface) {
  const QColor windowColor{0x22, 0x33, 0x44};
  QImage image = QImage{20, 20, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

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
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled | QStyle::State_HasFocus);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Base, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).surface);
  style_.drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter);
  painter.end();

  const QColor focusColor = resolvedDefaultTokens().borderFocus;
  const QColor fillColor = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).surface;
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), focusColor);
  EXPECT_EQ(image.pixelColor(image.rect().center()), fillColor);
}

TEST_F(DrawPrimitiveTest, FrameLineEditUsesDarkFillAndRoundedBorder) {
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  opt.palette.setColor(QPalette::Base, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).surface);
  style_.drawPrimitive(QStyle::PE_FrameLineEdit, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surface);
  EXPECT_GT(image.pixelColor(image.width() / 2, 0).alpha(), 0);
  EXPECT_GT(image.pixelColor(0, image.height() / 2).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, FrameGroupBox_NoAssertViolation) {
  auto opt = makeOpt(QStyle::State_Enabled);
  style_.drawPrimitive(QStyle::PE_FrameGroupBox, &opt, &painter_);
}

TEST_F(DrawPrimitiveTest, PanelMenuUsesRaisedSurfaceWithBorder) {
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_PanelMenu, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceRaised);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
  EXPECT_EQ(image.pixelColor(0, 0).alpha(), 0);
  EXPECT_EQ(image.pixelColor(image.width() - 1, 0).alpha(), 0);
  EXPECT_EQ(image.pixelColor(0, image.height() - 1).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, FrameMenuUsesBorder) {
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_FrameMenu, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
  EXPECT_EQ(image.pixelColor(0, 0).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, PanelTipLabelUsesInverseHybridSurfaceWithBorder) {
  QImage image = QImage{24, 24, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceInverse);
  EXPECT_EQ(image.pixelColor(image.width() / 2, 0), tok.borderPassive);
  EXPECT_EQ(image.pixelColor(0, 0).alpha(), 0);
  EXPECT_EQ(image.pixelColor(image.width() - 1, 0).alpha(), 0);
  EXPECT_EQ(image.pixelColor(0, image.height() - 1).alpha(), 0);
}

TEST_F(DrawPrimitiveTest, SmallMenuGeometryIsClampedAndPainted) {
  QImage image = QImage{7, 5, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter = QPainter{&image};

  auto opt = makeOpt(QStyle::State_Enabled);
  opt.rect = image.rect();
  style_.drawPrimitive(QStyle::PE_PanelMenu, &opt, &painter);
  painter.end();

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(image.pixelColor(image.rect().center()), tok.surfaceRaised);
  EXPECT_GT(image.pixelColor(image.width() / 2, 0).alpha(), 0);
}

// A near-corner pixel distinguishes the checkbox radius from the general control radius.
TEST_F(DrawPrimitiveTest, CheckboxCornerCoverage) {
  for (const auto state : {QStyle::State_Off, QStyle::State_On, QStyle::State_NoChange}) {
    for (const bool enabled : {true, false}) {
      QStyleOptionButton opt;
      opt.rect = QRect{4, 4, 16, 16};
      opt.state = state | QStyle::State_Active;
      if (enabled) opt.state |= QStyle::State_Enabled;
      opt.palette = style_.standardPalette();
      for (const bool standalone : {true, false}) {
        QImage image{32, 32, QImage::Format_ARGB32_Premultiplied};
        image.fill(Qt::transparent);
        QPainter painter{&image};
        QRect indicator = opt.rect;
        if (standalone) {
          indicator = style_.subElementRect(QStyle::SE_CheckBoxIndicator, &opt);
          style_.drawControl(QStyle::CE_CheckBox, &opt, &painter);
        } else {
          style_.drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, &painter);
        }
        painter.end();
        EXPECT_GT(image.pixelColor(indicator.left() + 2, indicator.top() + 1).alpha(), 150)
            << state << " enabled=" << enabled << " standalone=" << standalone;
        EXPECT_LT(image.pixelColor(indicator.topLeft()).alpha(), 100);
      }
    }
  }
}

TEST_F(DrawPrimitiveTest, CheckboxPathsShareStatesPaletteAndScaledGeometry) {
  for (const int side : {16, 24, 32}) {
    for (const auto state : {QStyle::State_Off, QStyle::State_On, QStyle::State_NoChange}) {
      for (const auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        QStyleOptionButton opt;
        opt.rect = QRect{4, 4, side, side};
        opt.state = state;
        if (group != QPalette::Disabled) opt.state |= QStyle::State_Enabled;
        if (group == QPalette::Active) opt.state |= QStyle::State_Active;
        opt.palette.setColor(group, QPalette::Base, QColor{20, 30, 40});
        opt.palette.setColor(group, QPalette::Mid, QColor{60, 70, 80});
        opt.palette.setColor(group, QPalette::Highlight, QColor{100, 110, 120});
        opt.palette.setColor(group, QPalette::HighlightedText, QColor{230, 240, 250});
        QImage reference;
        for (const auto primitive : {QStyle::PE_IndicatorCheckBox, QStyle::PE_IndicatorItemViewItemCheck}) {
          QImage image{side + 8, side + 8, QImage::Format_ARGB32_Premultiplied};
          image.fill(Qt::transparent);
          QPainter painter{&image};
          style_.drawPrimitive(primitive, &opt, &painter);
          painter.end();
          EXPECT_EQ(image.pixelColor(4 + side / 2, 4 + side / 4),
                    opt.palette.color(group, state == QStyle::State_Off ? QPalette::Base : QPalette::Highlight));
          if (state == QStyle::State_NoChange) {
            EXPECT_EQ(image.pixelColor(opt.rect.center()), opt.palette.color(group, QPalette::HighlightedText));
          }
          if (reference.isNull())
            reference = image;
          else
            EXPECT_EQ(image, reference);
        }
      }
    }
  }
}

TEST_F(DrawPrimitiveTest, StandaloneCheckboxUsesQtIndicatorGeometryInBothDirections) {
  for (const auto direction : {Qt::LeftToRight, Qt::RightToLeft}) {
    QStyleOptionButton opt;
    opt.rect = QRect{4, 4, 100, 32};
    opt.direction = direction;
    opt.state = QStyle::State_Enabled | QStyle::State_On;
    opt.palette = style_.standardPalette();
    const QRect indicator = style_.subElementRect(QStyle::SE_CheckBoxIndicator, &opt);
    const QRect label = style_.subElementRect(QStyle::SE_CheckBoxContents, &opt);
    EXPECT_FALSE(indicator.intersects(label));
    EXPECT_EQ(indicator.center().x() > opt.rect.center().x(), direction == Qt::RightToLeft);
    QImage image{112, 44, QImage::Format_ARGB32_Premultiplied};
    image.fill(Qt::transparent);
    QPainter painter{&image};
    style_.drawControl(QStyle::CE_CheckBox, &opt, &painter);
    painter.end();
    EXPECT_GT(image.pixelColor(indicator.center()).alpha(), 240);
    EXPECT_EQ(image.pixelColor(direction == Qt::LeftToRight ? 100 : 8, 20).alpha(), 0);
  }
}

TEST_F(DrawPrimitiveTest, CheckableGroupBoxPreservesPaletteWithoutWidget) {
  QStyleOptionGroupBox opt;
  opt.rect = QRect{4, 4, 180, 80};
  opt.text = QStringLiteral("Group");
  opt.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel | QStyle::SC_GroupBoxCheckBox;
  opt.state = QStyle::State_Enabled | QStyle::State_Active | QStyle::State_On;
  opt.palette.setColor(QPalette::Active, QPalette::Highlight, QColor{210, 20, 60});
  QImage image{200, 100, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};
  style_.drawComplexControl(QStyle::CC_GroupBox, &opt, &painter);
  painter.end();
  const QRect indicator = style_.subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxCheckBox);
  EXPECT_EQ(image.pixelColor(indicator.left() + indicator.width() / 2, indicator.top() + 3), QColor(210, 20, 60));
}

TEST_F(DrawPrimitiveTest, ItemViewCheckboxReservesSpaceForIconAndTextInBothDirections) {
  for (const auto direction : {Qt::LeftToRight, Qt::RightToLeft}) {
    QStyleOptionViewItem opt;
    opt.rect = QRect{4, 4, 180, 32};
    opt.direction = direction;
    opt.state = QStyle::State_Enabled | QStyle::State_Active;
    opt.features = QStyleOptionViewItem::HasCheckIndicator | QStyleOptionViewItem::HasDecoration |
                   QStyleOptionViewItem::HasDisplay;
    opt.checkState = Qt::PartiallyChecked;
    opt.text = QStringLiteral("Checkable item");
    QPixmap icon{16, 16};
    icon.fill(Qt::red);
    opt.icon = QIcon{icon};
    opt.decorationSize = QSize{16, 16};
    opt.palette = style_.standardPalette();
    const QRect check = style_.subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt);
    const QRect decoration = style_.subElementRect(QStyle::SE_ItemViewItemDecoration, &opt);
    const QRect text = style_.subElementRect(QStyle::SE_ItemViewItemText, &opt);
    EXPECT_FALSE(check.intersects(decoration));
    EXPECT_FALSE(check.intersects(text));
    EXPECT_FALSE(decoration.intersects(text));
    EXPECT_EQ(check.center().x() > opt.rect.center().x(), direction == Qt::RightToLeft);
    QImage image{200, 44, QImage::Format_ARGB32_Premultiplied};
    image.fill(Qt::transparent);
    QPainter painter{&image};
    style_.drawControl(QStyle::CE_ItemViewItem, &opt, &painter);
    painter.end();
    EXPECT_EQ(image.pixelColor(check.center()), opt.palette.color(QPalette::Active, QPalette::HighlightedText));
    EXPECT_EQ(image.pixelColor(decoration.center()), QColor(Qt::red));
  }
}

TEST_F(DrawPrimitiveTest, RadioButtonRemainsCircular) {
  QStyleOptionButton opt;
  opt.rect = QRect{4, 4, 16, 16};
  opt.state = QStyle::State_Enabled | QStyle::State_On;
  opt.palette = style_.standardPalette();
  QImage image{32, 32, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};
  style_.drawControl(QStyle::CE_RadioButton, &opt, &painter);
  painter.end();
  EXPECT_LT(image.pixelColor(6, 5).alpha(), 40);
  EXPECT_GT(image.pixelColor(12, 5).alpha(), 150);
}

TEST_F(DrawPrimitiveTest, KeyboardFocusFollowsCheckboxIndicator) {
  QStyleOptionButton opt;
  opt.rect = QRect{8, 8, 100, 28};
  opt.state = QStyle::State_Enabled | QStyle::State_Active | QStyle::State_Off | QStyle::State_HasFocus |
              QStyle::State_KeyboardFocusChange;
  opt.palette = style_.standardPalette();
  const QRect indicator = style_.subElementRect(QStyle::SE_CheckBoxIndicator, &opt);
  QImage image{120, 48, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);
  QPainter painter{&image};
  style_.drawControl(QStyle::CE_CheckBox, &opt, &painter);
  painter.end();
  EXPECT_GT(image.pixelColor(indicator.center().x(), indicator.top() - 2).alpha(), 80);
  EXPECT_EQ(image.pixelColor(indicator.left() - 2, indicator.top() - 2).alpha(), 0);
  EXPECT_EQ(image.pixelColor(100, indicator.top() - 2).alpha(), 0);
}
