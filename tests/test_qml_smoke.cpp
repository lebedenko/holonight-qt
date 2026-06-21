// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QQmlComponent>
#include <QQmlEngine>
#include <QString>

#include <gtest/gtest.h>

class QmlSmoke : public ::testing::Test {
 protected:
  void SetUp() override { engine_.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)); }

  QQmlEngine engine_;
};

static void checkComponent(QQmlEngine& engine, const char* qml) {
  QQmlComponent comp{&engine};
  comp.setData(qml, QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, Button_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Button { text: "test" })"); }

TEST_F(QmlSmoke, LowercaseCompatibilityModule_LoadsWithoutError) {
  checkComponent(engine_, R"(import holonight; Button { text: "test" })");
}

TEST_F(QmlSmoke, ScrollBar_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; ScrollBar {})"); }

TEST_F(QmlSmoke, TextField_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; TextField { placeholderText: "hint" })");
}

TEST_F(QmlSmoke, CheckBox_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; CheckBox { text: "check" })");
}

TEST_F(QmlSmoke, RadioButton_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; RadioButton { text: "option" })");
}

TEST_F(QmlSmoke, ComboBox_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ComboBox { model: ["a", "b"] })");
}

TEST_F(QmlSmoke, Slider_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Slider {})"); }

TEST_F(QmlSmoke, Switch_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; Switch { text: "toggle" })");
}

TEST_F(QmlSmoke, ProgressBar_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ProgressBar { value: 0.5 })");
}

TEST_F(QmlSmoke, TabBar_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; TabBar {})"); }

TEST_F(QmlSmoke, TabButton_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; TabButton { text: "Tab 1" })");
}

TEST_F(QmlSmoke, Menu_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Menu {})"); }

TEST_F(QmlSmoke, MenuItem_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; MenuItem { text: "Action" })");
}

TEST_F(QmlSmoke, ToolTip_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ToolTip { text: "hint" })");
}

TEST_F(QmlSmoke, HoloniightPalette_PrimaryIsValid) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item { property color c: HoloniightPalette.primary }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HoloniightPalette_DesignSystemAccentsAreValid) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property color cyan: HoloniightPalette.accentCyan
      property color blue: HoloniightPalette.accentBlue
      property color violet: HoloniightPalette.accentViolet
      property color yellow: HoloniightPalette.accentYellow
      property color subtle: HoloniightPalette.textSubtle
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HolonightTheme_ConfigPropertiesAreValid) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property string iconTheme: HolonightTheme.iconTheme
      property string uiFont: HolonightTheme.uiFont
      property int bodySize: HolonightTheme.bodySize
      property int captionSize: HolonightTheme.captionSize
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}
