// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QMouseEvent>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTest>
#include <QWheelEvent>

#include <gtest/gtest.h>
#include <memory>

TEST(InputInteraction, KeyboardClearsStaleButtonHoverFeedback) {
  if (qEnvironmentVariable("QT_QUICK_CONTROLS_STYLE") == "Fusion") GTEST_SKIP() << "Fusion owns standard rendering";
  QQmlEngine engine;
  engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
  QQmlComponent component(&engine);
  component.setData(R"(
import QtQuick
import QtQuick.Controls as C
Window {
    width: 400; height: 300; visible: true
    property color normalColor: button.controlColors.colors.surfaceRaised
    C.Button { id: button; objectName: "button"; x: 20; y: 20; width: 140; text: "Test" }
})",
                    QUrl());
  std::unique_ptr<QObject> root(component.create());
  ASSERT_NE(root, nullptr) << component.errorString().toStdString();
  auto* window = qobject_cast<QQuickWindow*>(root.get());
  ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
  auto* button = root->findChild<QQuickItem*>("button");
  ASSERT_NE(button, nullptr);
  auto* background = button->property("background").value<QObject*>();
  ASSERT_NE(background, nullptr);
  QTest::mouseMove(window, QPoint(350, 250));
  const auto normal = root->property("normalColor");
  ASSERT_TRUE(QTest::qWaitFor([&] { return background->property("color") == normal; }));
  QTest::mouseMove(window, QPoint(60, 35));
  ASSERT_TRUE(
      QTest::qWaitFor([&] { return button->property("hovered").toBool() && background->property("color") != normal; }));
  button->forceActiveFocus(Qt::TabFocusReason);
  QTest::keyClick(window, Qt::Key_Right);
  EXPECT_TRUE(QTest::qWaitFor([&] { return background->property("color") == normal; }, 500));
}

namespace {
class InputInteractionTest : public testing::Test {
 protected:
  QQmlEngine engine;
  std::unique_ptr<QObject> root;
  QQuickWindow* window = nullptr;

  std::unique_ptr<QObject> createWindow() {
    QQmlComponent component(&engine);
    component.setData(R"(
import QtQuick
import QtQuick.Controls as C
import Holonight.Core
import Holonight.Controls
Window {
    id: root
    width: 400; height: 300; visible: true
    property bool allowed: HnInputInteraction.hoverAllowed
    property int movements: 0
    property point lastMovement
    function suppress() { HnInputInteraction.suppressHover() }
    HnInputInteraction.onPointerMoved: function(point) {
        movements++
        lastMovement = point
    }
    Item {
        objectName: "attachment"
        property bool allowed: HnInputInteraction.hoverAllowed
    }
    function animate() { movementAnimation.start() }
    NumberAnimation { id: movementAnimation; target: button; property: "x"; to: 25; duration: 30 }
    C.Button { id: button; objectName: "button"; x: 20; y: 20; width: 140; text: "Test" }
    HnListDelegate { objectName: "delegate"; x: 20; y: 100; width: 200; text: "Selected"; checked: true }
})",
                      QUrl());
    auto result = std::unique_ptr<QObject>(component.create());
    EXPECT_NE(result, nullptr) << component.errorString().toStdString();
    return result;
  }

  void SetUp() override {
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
    root = createWindow();
    ASSERT_NE(root, nullptr);
    window = qobject_cast<QQuickWindow*>(root.get());
    ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
  }
  bool allowed() const { return root->property("allowed").toBool(); }
  void suppress() { ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "suppress")); }
  void move(QPoint point) {
    QTest::mouseMove(window, point);
    QCoreApplication::processEvents();
  }
};
}  // namespace

TEST_F(InputInteractionTest, OnlyChangedScreenCoordinatesRestoreHover) {
  move(QPoint(350, 250));
  move(QPoint(60, 35));
  ASSERT_TRUE(allowed());
  const int movements = root->property("movements").toInt();
  QTest::keyClick(window, Qt::Key_Right);
  ASSERT_FALSE(allowed());
  const QPoint position(60, 35);
  const QPoint global = window->mapToGlobal(position);
  QMouseEvent stationary(QEvent::MouseMove, position, global, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::sendEvent(window, &stationary);
  QEnterEvent enter(position, position, global);
  QCoreApplication::sendEvent(window, &enter);
  QWheelEvent wheel(position, global, QPoint(), QPoint(0, -120), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase,
                    false);
  QCoreApplication::sendEvent(window, &wheel);
  auto* button = root->findChild<QQuickItem*>("button");
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "animate"));
  ASSERT_TRUE(QTest::qWaitFor([&] { return button->x() == 25; }));
  EXPECT_FALSE(allowed());
  EXPECT_EQ(root->property("movements").toInt(), movements);
  move(QPoint(61, 35));
  EXPECT_TRUE(allowed());
  EXPECT_EQ(root->property("movements").toInt(), movements + 1);
  const auto reported = root->property("lastMovement").toPointF();
  EXPECT_NEAR(reported.x(), 61, 0.5);
  EXPECT_NEAR(reported.y(), 35, 0.5);
}

TEST_F(InputInteractionTest, ModifierKeysDoNotSuppressAndClicksRemainImmediate) {
  move(QPoint(60, 35));
  QTest::keyClick(window, Qt::Key_Shift);
  EXPECT_TRUE(allowed());
  suppress();
  ASSERT_FALSE(allowed());
  auto* button = root->findChild<QQuickItem*>("button");
  QSignalSpy clicked(button, SIGNAL(clicked()));
  QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, QPoint(60, 35));
  EXPECT_EQ(clicked.count(), 1);
  EXPECT_FALSE(allowed());
}

TEST_F(InputInteractionTest, WindowsAndReparentedAttachmentsHaveIndependentAuthority) {
  auto other = createWindow();
  ASSERT_NE(other, nullptr);
  auto* other_window = qobject_cast<QQuickWindow*>(other.get());
  ASSERT_TRUE(QTest::qWaitForWindowExposed(other_window));
  suppress();
  EXPECT_FALSE(allowed());
  EXPECT_TRUE(other->property("allowed").toBool());
  auto* attachment = root->findChild<QQuickItem*>("attachment");
  EXPECT_FALSE(attachment->property("allowed").toBool());
  attachment->setParentItem(other_window->contentItem());
  EXPECT_TRUE(attachment->property("allowed").toBool());
  QTest::keyClick(other_window, Qt::Key_Down);
  EXPECT_FALSE(attachment->property("allowed").toBool());
  other.reset();
  EXPECT_FALSE(attachment->property("allowed").toBool());
  attachment->setParentItem(window->contentItem());
  move(QPoint(75, 50));
  EXPECT_TRUE(attachment->property("allowed").toBool());
}

TEST_F(InputInteractionTest, CheckedDelegateKeepsSelectionDuringKeyboardUse) {
  auto* delegate = root->findChild<QQuickItem*>("delegate");
  ASSERT_NE(delegate, nullptr);
  auto* overlay = delegate->findChild<QQuickItem*>("hnSelectableDelegateSelectedOverlay");
  ASSERT_NE(overlay, nullptr);
  move(QPoint(350, 250));
  const auto selected_color = overlay->property("color");
  move(QPoint(70, 115));
  ASSERT_TRUE(QTest::qWaitFor([&] { return overlay->property("color") != selected_color; }));
  QTest::keyClick(window, Qt::Key_Right);
  EXPECT_TRUE(overlay->isVisible());
  EXPECT_TRUE(delegate->property("checked").toBool());
  EXPECT_EQ(overlay->property("color"), selected_color);
}
