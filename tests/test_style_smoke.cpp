// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"
#include "holonight/palette.h"
#include "themeresolver.h"

#include <QByteArray>
#include <QDockWidget>
#include <QFile>
#include <QListView>
#include <QSplitter>
#include <QStatusBar>
#include <QTemporaryDir>
#include <QTextEdit>
#include <QtTest/QTest>

#include <gtest/gtest.h>
#include <holonight/config/config.h>

namespace {

class EnvGuard {
 public:
  explicit EnvGuard(const char* name) : name_{name}, had_value_{qEnvironmentVariableIsSet(name)} {
    if (had_value_) {
      old_value_ = qgetenv(name);
    }
  }
  EnvGuard(const EnvGuard&) = delete;
  EnvGuard& operator=(const EnvGuard&) = delete;
  EnvGuard(EnvGuard&&) = delete;
  EnvGuard& operator=(EnvGuard&&) = delete;

  ~EnvGuard() {
    if (had_value_) {
      qputenv(name_, old_value_);
    } else {
      qunsetenv(name_);
    }
  }

 private:
  const char* name_;
  bool had_value_;
  QByteArray old_value_;
};

void writeFile(const QString& path, const QByteArray& contents) {
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
  ASSERT_TRUE(opened);
  ASSERT_EQ(file.write(contents), contents.size());
}

}  // namespace

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
  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(view.palette().color(QPalette::Base), tok.surfaceElevated);
  EXPECT_EQ(view.viewport()->palette().color(QPalette::Base), tok.surfaceElevated);
}

TEST(StyleSmoke, PolishMainViewUsesDarkerSurface) {
  HoloniightStyle style;
  QListView view;
  style.polish(&view);
  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(view.palette().color(QPalette::Window), tok.surface);
  EXPECT_EQ(view.palette().color(QPalette::Base), tok.surface);
  EXPECT_EQ(view.viewport()->palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishInformationPanelUsesElevatedSurface) {
  HoloniightStyle style;
  QWidget panel;
  panel.setObjectName(QStringLiteral("infoDock"));

  style.polish(&panel);

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(panel.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(panel.palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishDockWidgetUsesElevatedSurface) {
  HoloniightStyle style;
  QDockWidget dock;

  style.polish(&dock);

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
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

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
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

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
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

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
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

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(edit.palette().color(QPalette::Window), tok.surfaceElevated);
  EXPECT_EQ(edit.palette().color(QPalette::Base), tok.surface);
  EXPECT_EQ(edit.viewport()->palette().color(QPalette::Base), tok.surface);
}

TEST(StyleSmoke, PolishStatusBarUsesElevatedSurface) {
  HoloniightStyle style;
  QStatusBar statusBar;
  style.polish(&statusBar);
  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(statusBar.palette().color(QPalette::Window), tok.surfaceElevated);
}

TEST(StyleSmoke, PolishNamedStatusWidgetUsesElevatedSurface) {
  HoloniightStyle style;
  QWidget statusWidget;
  statusWidget.setObjectName(QStringLiteral("statusBar"));

  style.polish(&statusWidget);

  const auto tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
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

TEST(StyleSmoke, MenuPanelWidthIsOne) {
  HoloniightStyle style;
  EXPECT_EQ(style.pixelMetric(QStyle::PM_MenuPanelWidth), 1);
}

TEST(StyleSmoke, ReloadPreservesApplicationPaletteRolesAndResolveMask) {
  EnvGuard guard("HOLONIGHT_APPEARANCE_FILE");
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString path = directory.filePath("appearance.toml");
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toUtf8());
  auto appearance = HoloNight::Config::defaults();
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, std::filesystem::path(path.toStdString())));
  const QString savedStyle = QApplication::style()->name();
  auto* style = new HoloniightStyle;
  QApplication::setStyle(style);
  auto* reader = style->findChild<Holonight::AppearanceReader*>();
  ASSERT_TRUE(reader);
  const QPalette saved = QApplication::palette();
  QPalette chosen;
  chosen.setColor(QPalette::Active, QPalette::Base, QColor("#010203"));
  chosen.setColor(QPalette::Inactive, QPalette::Text, QColor("#abcdef"));
  chosen.setColor(QPalette::Disabled, QPalette::Text, QColor("#80654321"));
  QApplication::setPalette(chosen);
  const auto mask = QApplication::palette().resolveMask();
  for (const QByteArray scheme :
       {QByteArray("holonight-light"), QByteArray("holonight-dark"), QByteArray("holonight-light")}) {
    appearance.theme.scheme = scheme.toStdString();
    ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, std::filesystem::path(path.toStdString())));
    ASSERT_TRUE(reader->reload());
    const auto actual = QApplication::palette();
    EXPECT_EQ(actual.resolveMask(), mask);
    EXPECT_EQ(actual.color(QPalette::Active, QPalette::Base), QColor("#010203"));
    EXPECT_EQ(actual.color(QPalette::Inactive, QPalette::Text), QColor("#abcdef"));
    EXPECT_EQ(actual.color(QPalette::Disabled, QPalette::Text), QColor("#80654321"));
    EXPECT_EQ(actual.color(QPalette::Active, QPalette::Window),
              style->standardPalette().color(QPalette::Active, QPalette::Window));
  }
  QApplication::setStyle(savedStyle);
  QApplication::setPalette(saved);
}
