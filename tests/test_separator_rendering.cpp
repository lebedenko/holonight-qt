// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QImage>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QTest>

#include <cmath>
#include <gtest/gtest.h>
#include <memory>

TEST(SeparatorRendering, PhysicalCoverageAndFadeProfiles) {
  QQmlEngine engine;
  engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  QQmlComponent component(&engine);
  component.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      id: root
      width: 180; height: 180; color: "#202020"
      property bool vertical: false
      property real offset: 0
      property int mode: 0
      property real stroke: 1
      property bool raw: false
      Item {
        x: 20 + (root.vertical ? root.offset : 0)
        y: 20 + (root.vertical ? 0 : root.offset)
        HnSeparator {
          width: 120.3; height: 120.3
          orientation: root.vertical ? Qt.Vertical : Qt.Horizontal
          fadeMode: root.mode
          thickness: root.stroke
          visible: !root.raw
          color: "#80ffffff"; opacity: 0.8
          centerOpacity: 0.9; edgeOpacity: 0.1
        }
        Rectangle {
          visible: root.raw
          width: root.vertical ? 1 : 120.3
          height: root.vertical ? 120.3 : 1
          color: "#80ffffff"; opacity: 0.72
        }
      }
    }
  )",
                    QUrl());
  ASSERT_TRUE(component.isReady()) << component.errorString().toStdString();
  std::unique_ptr<QObject> object(component.create());
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  window->show();
  ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
  const double dpr = qEnvironmentVariable("QT_SCALE_FACTOR").toDouble();
  ASSERT_DOUBLE_EQ(window->effectiveDevicePixelRatio(), dpr);
  ASSERT_EQ(window->rendererInterface()->graphicsApi(), QSGRendererInterface::Software);
  bool raw_has_extra_coverage = false;
  for (bool raw : {false, true}) {
    window->setProperty("raw", raw);
    for (bool vertical : {false, true}) {
      window->setProperty("vertical", vertical);
      for (int mode = 0; mode < (raw ? 1 : 4); ++mode) {
        window->setProperty("mode", mode);
        for (double stroke : {1.0, 2.0, 3.0, 0.0, -1.0}) {
          if (raw && stroke != 1.0) continue;
          window->setProperty("stroke", stroke);
          for (double offset : {-1.0, -0.75, -0.5, -0.001, 0.0, 0.125, 0.375, 0.5, 0.501, 0.75, 1.0}) {
            SCOPED_TRACE(::testing::Message() << "DPR=" << dpr << " vertical=" << vertical << " mode=" << mode
                                              << " stroke=" << stroke << " offset=" << offset << " raw=" << raw);
            window->setProperty("offset", offset);
            QCoreApplication::processEvents();
            const QImage image = window->grabWindow();
            ASSERT_FALSE(image.isNull());
            ASSERT_EQ(image.size(), QSize(qRound(180 * dpr), qRound(180 * dpr)));
            for (double fraction : {0.1, 0.25, 0.5, 0.75, 0.9}) {
              const int major = static_cast<int>((20 + 120.3 * fraction) * dpr);
              const double t = ((major + 0.5) / dpr - 20) / 120.3;
              const double profile = mode == 0   ? 0.9
                                     : mode == 1 ? 0.1 + 0.8 * (1 - std::abs(2 * t - 1))
                                     : mode == 2 ? 0.1 + 0.8 * t
                                                 : 0.9 - 0.8 * t;
              const int leading = qRound((20 + offset) * dpr);
              int coverage = 0;
              for (int minor = leading - 4; minor <= leading + 7; ++minor) {
                const QColor pixel = image.pixelColor(vertical ? minor : major, vertical ? major : minor);
                if (pixel.red() > 35) ++coverage;
                if (!raw) {
                  const bool painted = minor >= leading && minor < leading + std::max(0.0, stroke);
                  const double expected = 32 + (painted ? 223 * (128.0 / 255) * 0.8 * profile : 0);
                  EXPECT_NEAR(pixel.red(), expected, 3) << "minor=" << minor << " t=" << t;
                  EXPECT_EQ(pixel.red(), pixel.green());
                  EXPECT_EQ(pixel.red(), pixel.blue());
                }
              }
              if (raw)
                raw_has_extra_coverage |= coverage > 1;
              else
                EXPECT_EQ(coverage, std::max(0.0, stroke));
            }
          }
        }
      }
    }
  }
  if (dpr > 1) EXPECT_TRUE(raw_has_extra_coverage) << "Raw Rectangle must expose the thickness regression";
}

TEST(SeparatorRendering, SignedAncestorScaleAndLifecycle) {
  QQmlEngine engine;
  engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  QQmlComponent component(&engine);
  component.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      width: 420; height: 420; color: "#202020"
      Item {
        objectName: "outer"; x: 190.125; y: 190.375; width: 80; height: 80
        Item {
          objectName: "inner"; x: -20.25; y: -20.125; width: 60; height: 60
          HnSeparator {
            objectName: "separator"; x: -10.125; y: -10.375
            width: 40.3; height: 40.3
            color: "#80ffffff"; opacity: 0.8
            centerOpacity: 0.9; edgeOpacity: 0.1
          }
        }
      }
      Item { objectName: "alternative"; x: 170.375; y: 170.125; width: 70; height: 70; scale: -1.25 }
    }
  )",
                    QUrl());
  ASSERT_TRUE(component.isReady()) << component.errorString().toStdString();
  std::unique_ptr<QObject> object(component.create());
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  auto* outer = window->findChild<QQuickItem*>("outer");
  auto* inner = window->findChild<QQuickItem*>("inner");
  auto* separator = window->findChild<QQuickItem*>("separator");
  auto* alternative = window->findChild<QQuickItem*>("alternative");
  ASSERT_TRUE(outer && inner && separator && alternative);
  window->show();
  ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
  const qreal dpr = window->effectiveDevicePixelRatio();
  ASSERT_EQ(window->rendererInterface()->graphicsApi(), QSGRendererInterface::Software);

  // Expected bounds use only the public root transform and requested pixel count.
  const auto check = [&](bool vertical, int mode, int stroke, qreal scale) {
    QCoreApplication::processEvents();
    const QImage image = window->grabWindow();
    ASSERT_FALSE(image.isNull());
    const QPointF origin = separator->mapToScene(QPointF{});
    const qreal minor_origin = vertical ? origin.x() : origin.y();
    const qreal major_origin = vertical ? origin.y() : origin.x();
    const int edge = static_cast<int>(std::round(minor_origin * dpr));
    const int first = edge - (scale < 0 ? stroke : 0);
    if (scale == 0) {
      for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x) ASSERT_EQ(image.pixelColor(x, y), QColor("#202020"));
      return;
    }
    for (qreal fraction : {0.1, 0.25, 0.5, 0.75, 0.9}) {
      const int major = static_cast<int>(std::floor((major_origin + scale * 40.3 * fraction) * dpr));
      const qreal t = ((major + 0.5) / dpr - major_origin) / (scale * 40.3);
      const qreal profile = mode == 0   ? 0.9
                            : mode == 1 ? 0.1 + 0.8 * (1 - std::abs(2 * t - 1))
                            : mode == 2 ? 0.1 + 0.8 * t
                                        : 0.9 - 0.8 * t;
      int coverage = 0;
      for (int minor = first - 4; minor < first + stroke + 4; ++minor) {
        const int x = vertical ? minor : major;
        const int y = vertical ? major : minor;
        ASSERT_TRUE(image.rect().contains(x, y));
        const QColor pixel = image.pixelColor(x, y);
        const bool painted = minor >= first && minor < first + stroke;
        EXPECT_NEAR(pixel.red(), 32 + (painted ? 223 * (128.0 / 255) * 0.8 * profile : 0), 3);
        EXPECT_EQ(pixel.red(), pixel.green());
        EXPECT_EQ(pixel.red(), pixel.blue());
        if (pixel.red() > 35) ++coverage;
      }
      EXPECT_EQ(coverage, stroke);
    }
  };
  for (bool vertical : {false, true}) {
    separator->setProperty("orientation", vertical ? Qt::Vertical : Qt::Horizontal);
    for (int mode = 0; mode < 4; ++mode) {
      separator->setProperty("fadeMode", mode);
      for (int stroke : {1, 2, 3}) {
        separator->setProperty("thickness", stroke);
        for (qreal scale : {0.5, -0.5, 1.0, -1.0, 1.25, -1.25, 2.0, -2.0, 0.0, 1.25}) {
          SCOPED_TRACE(::testing::Message() << "dpr=" << dpr << " vertical=" << vertical << " mode=" << mode
                                            << " stroke=" << stroke << " scale=" << scale);
          outer->setScale(scale);
          check(vertical, mode, stroke, scale);
        }
      }
    }
    separator->setProperty("fadeMode", 2);
    separator->setProperty("thickness", 2);
    outer->setScale(1.25);
    inner->setScale(-0.5);
    check(vertical, 2, 2, -0.625);
    separator->setScale(-2);
    check(vertical, 2, 2, 1.25);
    separator->setScale(1);
    outer->setWidth(83.3);
    outer->setHeight(85.7);
    check(vertical, 2, 2, -0.625);
    inner->setWidth(63.7);
    inner->setHeight(65.3);
    check(vertical, 2, 2, -0.625);
    outer->setTransformOrigin(QQuickItem::BottomRight);
    check(vertical, 2, 2, -0.625);
    inner->setParentItem(alternative);
    check(vertical, 2, 2, 0.625);
    alternative->setWidth(73.3);
    alternative->setHeight(75.7);
    check(vertical, 2, 2, 0.625);
    inner->setParentItem(outer);
    inner->setScale(1);
    outer->setTransformOrigin(QQuickItem::Center);
  }
}
