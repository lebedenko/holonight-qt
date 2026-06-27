// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"
#include "holonight/palette.h"

#include <QDockWidget>
#include <QListView>
#include <QSplitter>
#include <QStatusBar>
#include <QTextEdit>

#include <gtest/gtest.h>

TEST(StyleSmoke, InstantiatesWithoutCrash) {
  HoloniightStyle style;
  SUCCEED();
}

TEST(StyleSmoke, StandardPaletteNonDefault) {
  HoloniightStyle style;
  const QPalette pal = style.standardPalette();
  const QPalette defaultPal;
  EXPECT_NE(pal.color(QPalette::Active, QPalette::Window), defaultPal.color(QPalette::Active, QPalette::Window));
}

TEST(StyleSmoke, ScrollBarExtentIsEight) {
  HoloniightStyle style;
  EXPECT_EQ(style.pixelMetric(QStyle::PM_ScrollBarExtent), 8);
}

TEST(StyleSmoke, ButtonMarginIsSix) {
  HoloniightStyle style;
  EXPECT_EQ(style.pixelMetric(QStyle::PM_ButtonMargin), 6);
}

TEST(StyleSmoke, PolishPlacesViewUsesElevatedSurface) {
  HoloniightStyle style;
  QListView view;
  view.setObjectName(QStringLiteral("placesPanel"));
  style.polish(&view);
  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(view.palette().color(QPalette::Base), tok.surfaceElevated);
  EXPECT_EQ(view.viewport()->palette().color(QPalette::Base), tok.surfaceElevated);
}

TEST(StyleSmoke, PolishMainViewUsesDarkerSurface) {
  HoloniightStyle style;
  QListView view;
  style.polish(&view);
  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(view.palette().color(QPalette::Window), tok.surface);
  EXPECT_EQ(view.palette().color(QPalette::Base), tok.surface);
  EXPECT_EQ(view.viewport()->palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishInformationPanelUsesElevatedSurface) {
  HoloniightStyle style;
  QWidget panel;
  panel.setObjectName(QStringLiteral("infoDock"));

  style.polish(&panel);

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(panel.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(panel.palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishDockWidgetUsesElevatedSurface) {
  HoloniightStyle style;
  QDockWidget dock;

  style.polish(&dock);

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(dock.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(dock.palette().color(QPalette::Base), tok.surface);
  EXPECT_TRUE(dock.autoFillBackground());
  EXPECT_EQ(dock.backgroundRole(), QPalette::Window);
}

TEST(StyleSmoke, PolishSidePanelChildKeepsInheritedSurface) {
  HoloniightStyle style;
  QWidget panel;
  panel.setObjectName(QStringLiteral("informationPanel"));
  style.polish(&panel);

  QWidget child(&panel);
  style.polish(&child);

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(child.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(child.palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishNestedSidePanelChildKeepsInheritedSurface) {
  HoloniightStyle style;
  QDockWidget dock;
  style.polish(&dock);

  QWidget content(&dock);
  style.polish(&content);
  QWidget details(&content);
  style.polish(&details);

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(content.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(details.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_TRUE(content.autoFillBackground());
  EXPECT_TRUE(details.autoFillBackground());
}

TEST(StyleSmoke, PolishSidePanelChildOverridesMainWindowSurface) {
  HoloniightStyle style;
  QWidget panel;
  panel.setObjectName(QStringLiteral("informationPanel"));
  style.polish(&panel);

  const auto tok = Holonight::darkTokens();
  QWidget child(&panel);
  QPalette childPalette = style.standardPalette();
  childPalette.setColor(QPalette::Window, tok.surface);
  child.setPalette(childPalette);

  style.polish(&child);

  EXPECT_EQ(child.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(child.palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishSidePanelTextEditUsesDarkerBase) {
  HoloniightStyle style;
  QWidget panel;
  panel.setObjectName(QStringLiteral("informationPanel"));
  style.polish(&panel);

  QTextEdit edit(&panel);
  style.polish(&edit);

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(edit.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(edit.palette().color(QPalette::Base), tok.surface);
  EXPECT_EQ(edit.viewport()->palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishStatusBarUsesElevatedSurface) {
  HoloniightStyle style;
  QStatusBar statusBar;
  style.polish(&statusBar);
  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(statusBar.palette().color(QPalette::Window), tok.surfaceElevated);
}

TEST(StyleSmoke, PolishNamedStatusWidgetUsesElevatedSurface) {
  HoloniightStyle style;
  QWidget statusWidget;
  statusWidget.setObjectName(QStringLiteral("statusBar"));

  style.polish(&statusWidget);

  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(statusWidget.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_TRUE(statusWidget.autoFillBackground());
  EXPECT_EQ(statusWidget.backgroundRole(), QPalette::Window);
}

TEST(StyleSmoke, PolishSplitterUsesOnePixelHandle) {
  HoloniightStyle style;
  QSplitter splitter;
  QWidget first;
  QWidget second;
  splitter.addWidget(&first);
  splitter.addWidget(&second);
  splitter.setHandleWidth(5);

  style.polish(&splitter);

  EXPECT_EQ(splitter.handleWidth(), 1);
  QSplitterHandle* handle = splitter.handle(1);
  ASSERT_NE(handle, nullptr);
  EXPECT_FALSE(handle->autoFillBackground());
  EXPECT_TRUE(handle->testAttribute(Qt::WA_NoSystemBackground));
  EXPECT_FALSE(handle->testAttribute(Qt::WA_OpaquePaintEvent));
}

TEST(StyleSmoke, DockWidgetSeparatorExtentIsOne) {
  HoloniightStyle style;
  EXPECT_EQ(style.pixelMetric(QStyle::PM_DockWidgetSeparatorExtent), 1);
}
