// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnwindowdecoration_p.h"

#include <QAbstractEventDispatcher>
#include <QCoreApplication>
#include <QPlatformSurfaceEvent>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QSignalSpy>

#include <gtest/gtest.h>

class HnWindowDecorationTestPeer {
 public:
  static inline Holonight::Private::WindowDecorationState state;
  static void attach(HnWindowDecoration& helper) {
    helper.probe_ = [](QQuickWindow*) { return state; };
  }
  static void refresh(HnWindowDecoration& /*helper*/) {
    QMetaObject::invokeMethod(QAbstractEventDispatcher::instance(), "aboutToBlock", Qt::DirectConnection);
  }
};

TEST(WindowDecoration, ReportsPlatformTransitionsAndOnlyChangedNotifications) {
  HnWindowDecoration helper;
  HnWindowDecorationTestPeer::attach(helper);
  HnWindowDecorationTestPeer::state = {.supported = true};
  QQuickWindow window;
  helper.setWindow(&window);
  QSignalSpy modes(&helper, &HnWindowDecoration::modeChanged);
  QSignalSpy decorated(&helper, &HnWindowDecoration::externalDecorationPresentChanged);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Unknown);
  EXPECT_FALSE(helper.externalDecorationPresent());
  HnWindowDecorationTestPeer::state.configured = true;
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::ServerSide);
  EXPECT_TRUE(helper.externalDecorationPresent());
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(modes.size(), 1);
  HnWindowDecorationTestPeer::state.toolkit_decoration = true;
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::ToolkitClientSide);
  EXPECT_EQ(decorated.size(), 1);
  HnWindowDecorationTestPeer::state.toolkit_decoration = false;
  HnWindowDecorationTestPeer::state.wants_decoration = true;
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Undecorated);
  EXPECT_FALSE(helper.externalDecorationPresent());
  HnWindowDecorationTestPeer::state = {.supported = true, .undecorated = true};
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Undecorated);
  HnWindowDecorationTestPeer::state = {};
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Unknown);
}

TEST(WindowDecoration, ReplacementDestructionAndSurfaceRecreationClearState) {
  HnWindowDecoration helper;
  HnWindowDecorationTestPeer::attach(helper);
  HnWindowDecorationTestPeer::state = {.supported = true, .configured = true};
  auto window = std::make_unique<QQuickWindow>();
  helper.setWindow(window.get());
  EXPECT_TRUE(helper.externalDecorationPresent());
  QPlatformSurfaceEvent destroyed(QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed);
  QCoreApplication::sendEvent(window.get(), &destroyed);
  HnWindowDecorationTestPeer::refresh(helper);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Unknown);
  QPlatformSurfaceEvent created(QPlatformSurfaceEvent::SurfaceCreated);
  QCoreApplication::sendEvent(window.get(), &created);
  QCoreApplication::sendPostedEvents(&helper);
  EXPECT_TRUE(helper.externalDecorationPresent());
  QQuickWindow replacement;
  HnWindowDecorationTestPeer::state = {.supported = true};
  helper.setWindow(&replacement);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Unknown);
  window.reset();
  EXPECT_EQ(helper.window(), &replacement);
  HnWindowDecorationTestPeer::state.configured = true;
  helper.setWindow(nullptr);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Unknown);
  window = std::make_unique<QQuickWindow>();
  helper.setWindow(window.get());
  window.reset();
  EXPECT_EQ(helper.window(), nullptr);
  EXPECT_EQ(helper.mode(), HnWindowDecoration::Mode::Unknown);
}

TEST(WindowDecoration, RegisteredQmlApiReportsUnknownOffscreen) {
  QQmlEngine engine;
  engine.addImportPath(QStringLiteral(HOLONIGHT_DECORATION_QML_PATH));
  QQmlComponent component(&engine);
  component.setData(R"(
    import QtQuick
    import Holonight.Core
    Window {
      id: nativeWindow
      property HnWindowDecoration decoration: HnWindowDecoration { window: nativeWindow }
      property int unknownMode: HnWindowDecoration.Unknown
    }
  )",
                    QUrl());
  std::unique_ptr<QObject> object(component.create());
  ASSERT_NE(object, nullptr) << component.errorString().toStdString();
  auto* helper = object->property("decoration").value<QObject*>();
  ASSERT_NE(helper, nullptr);
  EXPECT_EQ(helper->property("window").value<QObject*>(), object.get());
  EXPECT_EQ(helper->property("mode").toInt(), object->property("unknownMode").toInt());
  EXPECT_FALSE(helper->property("externalDecorationPresent").toBool());
}
