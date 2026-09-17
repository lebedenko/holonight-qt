// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTest>
#include <QtQuick/private/qquickpalette_p.h>

#include <gtest/gtest.h>
#include <holonight/config/config.h>
#include <memory>

namespace {
class WindowPalette : public testing::Test {
 protected:
  QPalette saved_ = QGuiApplication::palette();
  QQmlEngine engine_;
  void SetUp() override {
    engine_.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
    writeAppearance(false);
    QGuiApplication::setPalette(QPalette{});
  }
  void TearDown() override {
    QGuiApplication::setPalette(saved_);
    writeAppearance(false);
    QCoreApplication::processEvents();
  }
  void writeAppearance(bool light) {
    auto config = HoloNight::Config::defaults();
    config.theme.scheme = light ? "holonight-light" : "holonight-dark";
    ASSERT_TRUE(
        HoloNight::Config::writeAtomically(config, qEnvironmentVariable("HOLONIGHT_APPEARANCE_FILE").toStdString()));
  }
  static QPalette defaults(bool light) {
    return Holonight::buildPalette(Holonight::tokensForScheme(light ? Holonight::ThemeSchemeKind::HoloNightLight
                                                                    : Holonight::ThemeSchemeKind::HoloNightDark));
  }
  std::unique_ptr<QObject> create(const QByteArray& source) {
    QQmlComponent component(&engine_);
    component.setData(source, QUrl());
    auto object = std::unique_ptr<QObject>(component.create());
    EXPECT_TRUE(object) << component.errorString().toStdString();
    return object;
  }
  std::unique_ptr<QObject> window(const QByteArray& extra = {}) {
    return create(R"(
import QtQuick
import QtQuick.Controls as C
import Holonight.Core
import Holonight.Controls
HnApplicationWindow {
    id: root
    width: 360; height: 420; visible: true
    property color appearanceBackground: HoloniightPalette.background
    property int revision: 0
    Rectangle { width: 2; height: 2; color: root.revision % 2 ? "white" : "black" }
    Column {
        x: 20; y: 20; spacing: 12
        C.TextField { objectName: "field"; width: 300; text: "Text" }
        C.SpinBox { objectName: "spin"; width: 300; value: 42 }
        C.ComboBox { objectName: "combo"; width: 300; model: ["First", "Second"] }
        C.TextField { objectName: "disabled"; width: 300; enabled: false }
        C.TextField { objectName: "local"; width: 300; palette.base: "#bada55"; palette.disabled.text: "#112233" }
        HnSearchField { objectName: "search"; width: 300 }
        HnIconComboBox { objectName: "sharedCombo"; width: 300; model: ["First", "Second"] }
    }
)" + extra + "\n}");
  }
  static QQuickPalette* palette(QObject* object) { return object->property("palette").value<QQuickPalette*>(); }
  void transition(QObject* object, bool light) {
    writeAppearance(light);
    ASSERT_TRUE(QTest::qWaitFor(
        [&] {
          return object->property("appearanceBackground").value<QColor>() == defaults(light).color(QPalette::Window);
        },
        5000));
  }
  void expectRoles(QObject* object, const QPalette& expected) {
    for (const auto* name : {"field", "spin", "combo", "disabled"}) {
      SCOPED_TRACE(name);
      auto* control = object->findChild<QObject*>(name);
      ASSERT_TRUE(control);
      ASSERT_TRUE(palette(control));
      for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        for (auto role : {QPalette::Base, QPalette::Button, QPalette::Text, QPalette::Window, QPalette::Highlight})
          EXPECT_EQ(palette(control)->toQPalette().color(group, role), expected.color(group, role));
      }
    }
  }
};

TEST_F(WindowPalette, ExistingControlsFollowAppearance) {
  auto root = window();
  ASSERT_TRUE(root);
  for (bool light : {false, true, false}) {
    transition(root.get(), light);
    expectRoles(root.get(), defaults(light));
  }
}

TEST_F(WindowPalette, LightCreatedControlsInheritCurrentAppearance) {
  auto primary = window();
  ASSERT_TRUE(primary);
  transition(primary.get(), true);
  auto secondary = window();
  ASSERT_TRUE(secondary);
  expectRoles(secondary.get(), defaults(true));
  transition(primary.get(), false);
  expectRoles(secondary.get(), defaults(false));
}

TEST_F(WindowPalette, WindowAndControlOverridesSurviveTransitionsAndReset) {
  auto root = window("palette.base: '#345678'; palette.disabled.text: '#876543'");
  ASSERT_TRUE(root);
  for (bool light : {false, true, false}) {
    transition(root.get(), light);
    auto expected = defaults(light);
    expected.setColor(QPalette::Base, QColor("#345678"));
    expected.setColor(QPalette::Disabled, QPalette::Text, QColor("#876543"));
    expectRoles(root.get(), expected);
    const auto local = palette(root->findChild<QObject*>("local"))->toQPalette();
    EXPECT_EQ(local.color(QPalette::Active, QPalette::Base), QColor("#bada55"));
    EXPECT_EQ(local.color(QPalette::Disabled, QPalette::Text), QColor("#112233"));
    EXPECT_EQ(local.color(QPalette::Active, QPalette::Button), expected.color(QPalette::Button));
  }
  palette(root.get())->resetBase();
  auto expected = defaults(false);
  expected.setColor(QPalette::Disabled, QPalette::Text, QColor("#876543"));
  expectRoles(root.get(), expected);
  palette(root.get())->reset();
  expectRoles(root.get(), defaults(false));
  palette(root->findChild<QObject*>("local"))->reset();
  EXPECT_EQ(palette(root->findChild<QObject*>("local"))->toQPalette().color(QPalette::Base),
            defaults(false).color(QPalette::Base));
  transition(root.get(), true);
  expectRoles(root.get(), defaults(true));
}

TEST_F(WindowPalette, CompleteWindowPaletteRemainsCallerOwned) {
  auto root = window();
  ASSERT_TRUE(root);
  auto custom = defaults(false);
  for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    for (int role = 0; role < QPalette::NColorRoles; ++role) {
      if (role != QPalette::NoRole) custom.setColor(group, static_cast<QPalette::ColorRole>(role), QColor("#456789"));
    }
  }
  palette(root.get())->fromQPalette(custom);
  for (bool light : {true, false}) {
    transition(root.get(), light);
    EXPECT_EQ(palette(root.get())->toQPalette(), custom);
    expectRoles(root.get(), custom);
  }
  palette(root.get())->reset();
  expectRoles(root.get(), defaults(false));
}

TEST_F(WindowPalette, ApplicationOverridesAndResetApplyToExistingAndNewWindows) {
  auto root = window("palette.text: '#aabbcc'");
  ASSERT_TRUE(root);
  QPalette application;
  application.setColor(QPalette::Base, QColor("#657483"));
  application.setColor(QPalette::Disabled, QPalette::Button, QColor("#cba987"));
  const auto unchanged = QGuiApplication::palette();
  transition(root.get(), true);
  EXPECT_EQ(QGuiApplication::palette(), unchanged);
  for (const auto& overridePalette : {application, defaults(false), QPalette{}}) {
    QGuiApplication::setPalette(overridePalette);
    auto expected = overridePalette.resolve(defaults(true));
    expected.setColor(QPalette::Text, QColor("#aabbcc"));
    ASSERT_TRUE(QTest::qWaitFor(
        [&] { return palette(root.get())->toQPalette().color(QPalette::Base) == expected.color(QPalette::Base); },
        5000));
    expectRoles(root.get(), expected);
    auto recreated = window("palette.text: '#aabbcc'");
    ASSERT_TRUE(recreated);
    expectRoles(recreated.get(), expected);
    transition(root.get(), false);
    auto darkExpected = overridePalette.resolve(defaults(false));
    darkExpected.setColor(QPalette::Text, QColor("#aabbcc"));
    expectRoles(root.get(), darkExpected);
    expectRoles(recreated.get(), darkExpected);
    transition(root.get(), true);
  }
}

TEST_F(WindowPalette, BackgroundsAndRenderedSamplesRoundTripIncludingRecreation) {
  auto root = window();
  ASSERT_TRUE(root);
  const auto capture = [&](QObject* object) {
    auto* quickWindow = qobject_cast<QQuickWindow*>(object);
    EXPECT_TRUE(QTest::qWaitFor([&] { return quickWindow->isExposed(); }, 5000));
    QSignalSpy frames(quickWindow, &QQuickWindow::frameSwapped);
    object->setProperty("revision", object->property("revision").toInt() + 1);
    quickWindow->update();
    EXPECT_TRUE(QTest::qWaitFor([&] { return !frames.isEmpty(); }, 5000));
    const auto image = quickWindow->grabWindow();
    EXPECT_FALSE(image.isNull());
    EXPECT_EQ(quickWindow->devicePixelRatio(), qEnvironmentVariable("QT_SCALE_FACTOR", "1").toDouble());
    QMap<QString, QPair<QColor, QColor>> result;
    for (const auto* name : {"field", "spin", "combo", "disabled", "search", "sharedCombo"}) {
      auto* item = object->findChild<QQuickItem*>(name);
      auto* background = item->property("background").value<QQuickItem*>();
      EXPECT_NE(background, nullptr);
      const auto backgroundColor = background->property("color").value<QColor>();
      EXPECT_TRUE(backgroundColor.isValid());
      const auto point =
          item->mapToScene(QPointF(item->width() - 25, item->height() / 2)) * quickWindow->devicePixelRatio();
      const auto pixel = image.pixelColor(point.toPoint());
      result.insert(name, {backgroundColor, pixel});
      // Fusion ComboBox uses a native gradient; other enabled fills are flat.
      if (QString(name) != "combo" && QString(name) != "disabled") EXPECT_EQ(pixel, backgroundColor);
    }
    return result;
  };
  transition(root.get(), false);
  const auto dark = capture(root.get());
  transition(root.get(), true);
  const auto light = capture(root.get());
  auto recreated = window();
  ASSERT_TRUE(recreated);
  EXPECT_EQ(capture(recreated.get()), light);
  for (auto it = dark.cbegin(); it != dark.cend(); ++it) {
    SCOPED_TRACE(it.key().toStdString());
    EXPECT_NE(it.value().first, light.value(it.key()).first);
    EXPECT_NE(it.value().second, light.value(it.key()).second);
  }
  transition(root.get(), false);
  EXPECT_EQ(capture(root.get()), dark);
  EXPECT_EQ(capture(recreated.get()), dark);
}

TEST_F(WindowPalette, SharedCompositeOverridesRetainEstablishedBehavior) {
  auto root = create(R"(
import QtQuick
import Holonight.Core
import Holonight.Controls
HnApplicationWindow {
    property color appearanceBackground: HoloniightPalette.background
    HnSearchField { objectName: "search"; palette: Palette { base: "#bada55" } }
    HnIconComboBox { objectName: "sharedCombo"; palette: Palette { base: "#bada55" } }
})");
  ASSERT_TRUE(root);
  for (bool light : {true, false}) {
    transition(root.get(), light);
    for (const auto* name : {"search", "sharedCombo"}) {
      auto* control = root->findChild<QObject*>(name);
      EXPECT_EQ(palette(control)->toQPalette().color(QPalette::Base), QColor("#bada55"));
      EXPECT_EQ(palette(control)->toQPalette().color(QPalette::Button), defaults(light).color(QPalette::Button));
    }
  }
}
}  // namespace
