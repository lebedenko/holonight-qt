// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTest>

#include <gtest/gtest.h>
#include <memory>

namespace {
class FormFocus : public testing::TestWithParam<const char*> {
 private:
  QQmlEngine engine;
  std::unique_ptr<QObject> object;
  QQuickWindow* window = nullptr;

 protected:
  QObject* root() { return object.get(); }

  void SetUp() override {
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
    QQmlComponent component(&engine);
    component.setData(QString(R"(
import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Controls
Window {
    width: 600; height: 600; visible: true
    property bool replacement: false
    ColumnLayout {
        width: parent.width
        C.Button { objectName: "before"; text: "Before" }
        %1 {
            objectName: "singleRow"; Layout.fillWidth: true
            control: replacement ? replacementControl : originalControl
        }
        %1 {
            objectName: "contextRow"; Layout.fillWidth: true
            C.SpinBox { objectName: "context" }
        }
        %1 {
            objectName: "compoundRow"; Layout.fillWidth: true
            RowLayout {
                C.SpinBox { objectName: "temperature" }
                C.Slider { objectName: "slider"; Layout.fillWidth: true }
            }
        }
        %1 { objectName: "disabledRow"; C.Button { enabled: false } }
        %1 { objectName: "hiddenRow"; C.Button { visible: false } }
        %1 { objectName: "missingRow" }
        %1 { objectName: "noTabRow"; C.Button { activeFocusOnTab: false } }
        C.Button { objectName: "after"; text: "After" }
    }
    Component { id: originalControl; C.ComboBox { objectName: "single"; model: ["One", "Two"] } }
    Component { id: replacementControl; C.Button { objectName: "replacement"; text: "Replacement" } }
})")
                          .arg(GetParam())
                          .toUtf8(),
                      QUrl());
    object.reset(component.create());
    ASSERT_NE(object, nullptr) << component.errorString().toStdString();
    window = qobject_cast<QQuickWindow*>(object.get());
    ASSERT_NE(window, nullptr);
    QCoreApplication::processEvents();
  }

  QQuickItem* item(const char* name) {
    auto* result = object->findChild<QQuickItem*>(QString::fromLatin1(name));
    EXPECT_NE(result, nullptr) << name;
    return result;
  }

  void expectFocus(const char* name, Qt::FocusReason reason) {
    QCoreApplication::processEvents();
    auto* expected = item(name);
    ASSERT_NE(expected, nullptr);
    auto* focusItem = window->activeFocusItem();
    // SpinBox is itself a focus scope: native keyboard entry focuses its editor.
    auto* content = expected->property("contentItem").value<QQuickItem*>();
    EXPECT_TRUE(focusItem == expected || (expected->isFocusScope() && focusItem == content))
        << name << ": " << (focusItem != nullptr ? focusItem->metaObject()->className() : "null");
    EXPECT_TRUE(expected->hasActiveFocus()) << name;
    EXPECT_EQ(expected->property("focusReason").toInt(), reason) << name;
    EXPECT_EQ(expected->property("visualFocus").toBool(),
              reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason)
        << name;
  }

  void step(const char* name, bool reverse = false) {
    QTest::keyClick(window, reverse ? Qt::Key_Backtab : Qt::Key_Tab, reverse ? Qt::ShiftModifier : Qt::NoModifier);
    expectFocus(name, reverse ? Qt::BacktabFocusReason : Qt::TabFocusReason);
  }
};

TEST_P(FormFocus, FirstAndRepeatedKeyboardCyclesSkipIneligibleAndCompoundStops) {
  for (const auto* name : {"disabledRow", "hiddenRow", "missingRow", "noTabRow", "compoundRow"}) {
    EXPECT_FALSE(item(name)->activeFocusOnTab()) << name;
  }
  item("before")->forceActiveFocus();
  for (int cycle = 0; cycle < 3; ++cycle) {
    SCOPED_TRACE(cycle);
    for (const auto* name : {"single", "context", "temperature", "slider", "after"}) {
      step(name);
    }
    for (const auto* name : {"slider", "temperature", "context", "single", "before"}) {
      step(name, true);
    }
  }
}

TEST_P(FormFocus, DescendantEntryRetainsReasonAndOwner) {
  for (const auto* name : {"single", "context", "temperature", "slider"}) {
    item("before")->forceActiveFocus();
    item(name)->forceActiveFocus(Qt::BacktabFocusReason);
    expectFocus(name, Qt::BacktabFocusReason);
  }
}

TEST_P(FormFocus, ProgrammaticEntryAndLoaderReplacementPreserveGeometry) {
  auto* row = item("singleRow");
  const QSizeF geometry(row->width(), row->height());
  item("before")->forceActiveFocus();
  row->forceActiveFocus(Qt::TabFocusReason);
  expectFocus("single", Qt::TabFocusReason);
  EXPECT_EQ(QSizeF(row->width(), row->height()), geometry);
  item("before")->forceActiveFocus();
  ASSERT_TRUE(root()->setProperty("replacement", true));
  QCoreApplication::processEvents();
  step("replacement");
  step("context");
  step("replacement", true);
  item("before")->forceActiveFocus();
  row->forceActiveFocus(Qt::OtherFocusReason);
  expectFocus("replacement", Qt::OtherFocusReason);
}

TEST_P(FormFocus, EligibilityChangesDoNotLeaveWrapperStops) {
  auto* control = item("single");
  auto* row = item("singleRow");
  for (const auto* property : {"enabled", "visible", "activeFocusOnTab"}) {
    SCOPED_TRACE(property);
    item("before")->forceActiveFocus();
    ASSERT_TRUE(control->setProperty(property, false));
    EXPECT_FALSE(row->activeFocusOnTab());
    step("context");
    ASSERT_TRUE(control->setProperty(property, true));
    EXPECT_TRUE(row->activeFocusOnTab());
    item("before")->forceActiveFocus();
    step("single");
    step("context");
    step("single", true);
  }
}

INSTANTIATE_TEST_SUITE_P(Wrappers, FormFocus, testing::Values("HnSettingsRow", "HnFormField"));
}  // namespace
