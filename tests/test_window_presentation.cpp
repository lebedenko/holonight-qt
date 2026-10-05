#include "hnwindowpresentation_p.h"

#include <QCoreApplication>
#include <QPlatformSurfaceEvent>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QSignalSpy>

#include <gtest/gtest.h>

class HnWindowPresentationTestPeer {
 public:
  static quint64 generation(const HnWindowPresentation& helper) { return helper.generation_; }
  static void deliver(HnWindowPresentation& helper, const CompositorSnapshot& snapshot, quint64 generation) {
    helper.acceptSnapshot(snapshot, generation);
  }
  static ExternalTitleBarState observation(const HnWindowPresentation& helper) { return helper.observation_; }
};
TEST(WindowPresentation, OldAsynchronousSnapshotsCannotSurviveWindowOrSurfaceReplacement) {
  QGuiApplication::setDesktopFileName(QStringLiteral("org.holonight.Viewer"));
  HnWindowPresentation helper;
  QQuickWindow first;
  QQuickWindow replacement;
  helper.setWindow(&first);
  const auto old_generation = HnWindowPresentationTestPeer::generation(helper);
  CompositorSnapshot snapshot{.connected = true};
  snapshot.windows.append({
      .app_id = "org.holonight.Viewer",
      .pid = static_cast<quint32>(QCoreApplication::applicationPid()),
      .external_title_bar = ExternalTitleBarState::Present,
  });
  HnWindowPresentationTestPeer::deliver(helper, snapshot, old_generation);
  EXPECT_EQ(HnWindowPresentationTestPeer::observation(helper), ExternalTitleBarState::Present);
  helper.setWindow(&replacement);
  HnWindowPresentationTestPeer::deliver(helper, snapshot, old_generation);
  EXPECT_EQ(HnWindowPresentationTestPeer::observation(helper), ExternalTitleBarState::Unknown);
  const auto new_generation = HnWindowPresentationTestPeer::generation(helper);
  HnWindowPresentationTestPeer::deliver(helper, snapshot, new_generation);
  EXPECT_EQ(HnWindowPresentationTestPeer::observation(helper), ExternalTitleBarState::Present);
  const CompositorSnapshot disconnected;
  HnWindowPresentationTestPeer::deliver(helper, disconnected, new_generation);
  EXPECT_EQ(HnWindowPresentationTestPeer::observation(helper), ExternalTitleBarState::Unknown);
  QPlatformSurfaceEvent destroyed(QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed);
  QCoreApplication::sendEvent(&replacement, &destroyed);
  HnWindowPresentationTestPeer::deliver(helper, snapshot, new_generation);
  EXPECT_EQ(HnWindowPresentationTestPeer::observation(helper), ExternalTitleBarState::Unknown);
}

TEST(WindowPresentation, ToolkitCsdAndRawSsdAreDistinctFromRenderedTitleEvidence) {
  using Mode = HnWindowDecoration::Mode;
  using State = HnWindowPresentation::State;
  using Holonight::Private::titleBarState;
  EXPECT_EQ(titleBarState(Mode::ToolkitClientSide, ExternalTitleBarState::Unknown, true), State::Present);
  EXPECT_EQ(titleBarState(Mode::ServerSide, ExternalTitleBarState::Unknown, true), State::Unknown);
  EXPECT_EQ(titleBarState(Mode::ServerSide, ExternalTitleBarState::Present, true), State::Present);
  EXPECT_EQ(titleBarState(Mode::ServerSide, ExternalTitleBarState::Absent, true), State::Absent);
  EXPECT_EQ(titleBarState(Mode::Unknown, ExternalTitleBarState::Present, true), State::Unknown);
  EXPECT_EQ(titleBarState(Mode::Undecorated, ExternalTitleBarState::Present, true), State::Unknown);
  EXPECT_EQ(titleBarState(Mode::ToolkitClientSide, ExternalTitleBarState::Present, false), State::Unknown);
}
TEST(WindowPresentation, FramelessReplacementDestructionAndSurfaceRecreationDeduplicateNotifications) {
  HnWindowPresentation helper;
  auto window = std::make_unique<QQuickWindow>();
  window->setFlags(Qt::FramelessWindowHint);
  QSignalSpy states(&helper, &HnWindowPresentation::externalTitleBarStateChanged);
  helper.setWindow(window.get());
  EXPECT_EQ(helper.externalTitleBarState(), HnWindowPresentation::State::Absent);
  const auto count = states.size();
  helper.setWindow(window.get());
  QCoreApplication::sendPostedEvents(&helper);
  EXPECT_EQ(states.size(), count);
  QPlatformSurfaceEvent destroyed(QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed);
  QCoreApplication::sendEvent(window.get(), &destroyed);
  EXPECT_EQ(helper.externalTitleBarState(), HnWindowPresentation::State::Unknown);
  QPlatformSurfaceEvent created(QPlatformSurfaceEvent::SurfaceCreated);
  QCoreApplication::sendEvent(window.get(), &created);
  QCoreApplication::sendPostedEvents(&helper);
  EXPECT_EQ(helper.externalTitleBarState(), HnWindowPresentation::State::Absent);
  QQuickWindow replacement;
  helper.setWindow(&replacement);
  EXPECT_EQ(helper.externalTitleBarState(), HnWindowPresentation::State::Unknown);
  window.reset();
  EXPECT_EQ(helper.window(), &replacement);
  helper.setWindow(nullptr);
  window = std::make_unique<QQuickWindow>();
  helper.setWindow(window.get());
  window.reset();
  EXPECT_EQ(helper.window(), nullptr);
  EXPECT_EQ(helper.externalTitleBarState(), HnWindowPresentation::State::Unknown);
}
TEST(WindowPresentation, RegisteredQmlApiFallsBackToUnknownOffscreen) {
  QQmlEngine engine;
  engine.addImportPath(QStringLiteral(HOLONIGHT_DECORATION_QML_PATH));
  QQmlComponent component(&engine);
  component.setData(R"(import QtQuick
    import Holonight.Core
    Window {
      id: nativeWindow
      property HnWindowPresentation presentation: HnWindowPresentation { window: nativeWindow }
    })",
                    QUrl());
  std::unique_ptr<QObject> object(component.create());
  ASSERT_NE(object, nullptr) << component.errorString().toStdString();
  auto* helper = object->property("presentation").value<QObject*>();
  ASSERT_NE(helper, nullptr);
  EXPECT_EQ(helper->property("window").value<QObject*>(), object.get());
  EXPECT_EQ(helper->property("externalTitleBarState").toInt(), 0);
  EXPECT_FALSE(helper->setProperty("externalTitleBarState", 1));
}
