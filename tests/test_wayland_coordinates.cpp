// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "layersurfacebackend_p.h"

#include <QGuiApplication>
#include <QQuickItem>
#include <QQuickView>
#include <QScreen>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>

#include <gtest/gtest.h>
#include <holonight/wayland/layersurfacehost.h>
#include <private/qhighdpiscaling_p.h>

namespace Holonight::Wayland {
class LayerSurfaceHostTestAccess {
 public:
  static std::unique_ptr<LayerSurfaceHost> create(std::unique_ptr<LayerSurfaceBackend> backend) {
    return std::unique_ptr<LayerSurfaceHost>(new LayerSurfaceHost(std::move(backend)));
  }
};
}  // namespace Holonight::Wayland

TEST(WaylandCoordinates, LogicalGeometryAcrossProtocolBoundary) {
  using namespace Holonight::Wayland;
  QTemporaryFile qml;
  ASSERT_TRUE(qml.open());
  qml.write("import QtQuick\nRectangle { color: 'red' }\n");
  qml.flush();
  auto backend_owner = createLayerSurfaceBackend();
  auto* backend = backend_owner.get();
  auto host = LayerSurfaceHostTestAccess::create(std::move(backend_owner));
  LayerSurfaceSpec spec;
  spec.output = QGuiApplication::primaryScreen();
  spec.name_space = QStringLiteral("uqc215-coordinates");
  spec.qml_url = QUrl::fromLocalFile(qml.fileName());
  spec.anchors = Anchor::Top | Anchor::Left | Anchor::Right;
  spec.height = 64;
  spec.exclusive_zone = 64;
  spec.margin_top = 4;
  spec.margin_right = 8;
  spec.margin_left = 12;
  spec.input_region_policy = InputRegionPolicy::Region;
  spec.input_region = QRegion(QRect(4, 8, 100, 40));
  QSignalSpy configured(host.get(), &LayerSurfaceHost::configured);
  ASSERT_TRUE(host->open(spec)) << host->diagnostic().toStdString();
  ASSERT_TRUE(configured.wait(3000));
  auto* view = host->view();
  ASSERT_NE(view, nullptr);
  // Both output scales expose the same logical output size in the runner.
  EXPECT_EQ(view->size(), QSize(qRound(1920 / qEnvironmentVariable("QT_SCALE_FACTOR").toDouble()) - 20, 64));
  EXPECT_EQ(view->rootObject()->size(), QSizeF(view->size()));
  qInfo() << "GEOMETRY" << view->size() << "DPR" << view->devicePixelRatio();
  const double scale = qEnvironmentVariable("QT_SCALE_FACTOR").toDouble();
  EXPECT_DOUBLE_EQ(view->devicePixelRatio(), scale * qEnvironmentVariable("TEST_OUTPUT_SCALE").toDouble());
  for (int iteration = 0; iteration < 10; ++iteration) {
    backend->applyConfigureSize(qRound(400 * scale), qRound(64 * scale));
    EXPECT_EQ(view->size(), QSize(400, 64));
    backend->applyConfigureSize(0, 0);
    EXPECT_EQ(view->size(), QSize(400, 64));
  }
  ASSERT_TRUE(host->setSize(400, 80));
  ASSERT_TRUE(host->setMargins(-4, 16, 8, 20));
  ASSERT_TRUE(host->setExclusiveZone(-1));
  ASSERT_TRUE(host->setInputRegion(InputRegionPolicy::Region, QRegion(QRect(8, 4, 200, 60))));
  ASSERT_TRUE(host->setExclusiveZone(0));
  ASSERT_TRUE(host->setExclusiveZone(80));
  ASSERT_TRUE(host->setSize(0, 80));
  QTest::qWait(100);
  ASSERT_TRUE(host->setInputRegion(InputRegionPolicy::Region, QRegion(QRect(3, 5, 101, 41))));
  QTest::qWait(50);
  // Exercise scale refresh deterministically without moving a window or changing
  // any desktop output. The event is the notification Qt sends after DPR updates.
  QHighDpiScaling::setGlobalFactor(1.5);
  QEvent scale_change(QEvent::DevicePixelRatioChange);
  QCoreApplication::sendEvent(view, &scale_change);
  QTest::qWait(100);
  EXPECT_EQ(view->height(), 80);
  for (int iteration = 0; iteration < 10; ++iteration) QCoreApplication::sendEvent(view, &scale_change);
  QTest::qWait(50);
  QHighDpiScaling::setGlobalFactor(scale);
  QCoreApplication::sendEvent(view, &scale_change);
  QTest::qWait(100);
  EXPECT_EQ(view->height(), 80);
  host->close();
  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
  configured.clear();
  spec.anchors = Anchor::Top | Anchor::Bottom | Anchor::Left;
  spec.width = 440;
  spec.height = 0;
  spec.exclusive_zone = -1;
  spec.input_region_policy = InputRegionPolicy::Empty;
  ASSERT_TRUE(host->open(spec));
  ASSERT_TRUE(configured.wait(3000));
  EXPECT_EQ(host->view()->width(), 440);
  EXPECT_EQ(host->view()->height(), qRound(1080 / scale) - 4);
  host->close();
  host.reset();
  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  qputenv("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT", "1");
  // Construct before the function-static LayerShellContext, so its proxies are
  // destroyed before Qt disconnects the display. Stack-application teardown is
  // a separately recorded provider defect, not part of the coordinate test.
  static QGuiApplication app(argc, argv);
  return RUN_ALL_TESTS();
}
