// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QImage>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickGraphicsDevice>
#include <QQuickItem>
#include <QQuickRenderControl>
#include <QQuickRenderTarget>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QTest>

#include <cmath>
#include <gtest/gtest.h>
#include <memory>

namespace {

// OpenGL uses an explicit target: offscreen QWindow backing buffers are not reliable
// at fractional DPR. Both paths render the same public QML into real pixels.
class SeparatorScene {
 public:
  Q_DISABLE_COPY_MOVE(SeparatorScene)
  explicit SeparatorScene(const QByteArray& qml) {
    engine_.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
    QObject::connect(&engine_, &QQmlEngine::warnings, &engine_, [](const QList<QQmlError>& warnings) {
      for (const auto& warning : warnings) {
        if (warning.description().contains(QStringLiteral("Binding loop"))) {
          ADD_FAILURE() << warning.toString().toStdString();
        }
      }
    });
    if (!initializeWindow()) {
      return;
    }
    QQmlComponent component(&engine_);
    component.setData(qml, QUrl());
    if (!component.isReady()) {
      ADD_FAILURE() << component.errorString().toStdString();
      return;
    }
    root_.reset(qobject_cast<QQuickItem*>(component.create()));
    if (!root_) {
      return;
    }
    window_->setColor(QColor("#202020"));
    window_->resize(qRound(root_->width()), qRound(root_->height()));
    window_->contentItem()->setSize(root_->size());
    setDpr(qEnvironmentVariable("QT_SCALE_FACTOR", "1").toDouble());
    root_->setParentItem(window_->contentItem());
    if (!gpu_) {
      window_->show();
      EXPECT_TRUE(QTest::qWaitForWindowExposed(window_.get()));
    }
    EXPECT_EQ(window_->rendererInterface()->graphicsApi(),
              gpu_ ? QSGRendererInterface::OpenGL : QSGRendererInterface::Software);
    qInfo("Separator backend=%d effectiveDpr=%g windowDpr=%g", window_->rendererInterface()->graphicsApi(),
          window_->effectiveDevicePixelRatio(), window_->devicePixelRatio());
  }

  ~SeparatorScene() {
    root_.reset();
    if (control_) {
      context_.makeCurrent(&surface_);
      control_->invalidate();
    }
    window_.reset();
    control_.reset();
  }

  void setDpr(qreal dpr) {
    if (!gpu_) {
      return;
    }
    context_.makeCurrent(&surface_);
    auto buffer = std::make_unique<QOpenGLFramebufferObject>(window_->size() * dpr,
                                                             QOpenGLFramebufferObject::CombinedDepthStencil);
    auto target = QQuickRenderTarget::fromOpenGLTexture(buffer->texture(), buffer->size());
    target.setDevicePixelRatio(dpr);
    window_->setRenderTarget(target);
    // Redirected targets have no platform screen event. Deliver the same public event
    // a native window receives on DPR change (setRenderTarget itself emits no signal).
    QEvent dpr_changed(QEvent::DevicePixelRatioChange);
    QCoreApplication::sendEvent(window_.get(), &dpr_changed);
    buffer_ = std::move(buffer);
  }

  QImage grab() {
    QCoreApplication::processEvents();
    if (!gpu_) {
      return window_->grabWindow();
    }
    context_.makeCurrent(&surface_);
    control_->polishItems();
    control_->beginFrame();
    control_->sync();
    control_->render();
    control_->endFrame();
    return buffer_->toImage();
  }

  [[nodiscard]] QQuickItem* root() const { return root_.get(); }
  [[nodiscard]] QQuickWindow* window() const { return window_.get(); }
  [[nodiscard]] bool gpu() const { return gpu_; }

 private:
  bool initializeWindow() {
    if (gpu_) {
      QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
      if (!context_.create()) {
        ADD_FAILURE() << "OpenGL context creation failed; fallback is not accepted";
        return false;
      }
      surface_.setFormat(context_.format());
      surface_.create();
      if (!context_.makeCurrent(&surface_)) {
        ADD_FAILURE() << "OpenGL makeCurrent failed";
        return false;
      }
      const auto* renderer = context_.functions()->glGetString(GL_RENDERER);
      qInfo("Separator OpenGL renderer: %s", renderer);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      if (QByteArray(reinterpret_cast<const char*>(renderer)).toLower().contains("llvmpipe")) {
        ADD_FAILURE() << "Software OpenGL is not accelerated acceptance evidence";
        return false;
      }
      control_ = std::make_unique<QQuickRenderControl>();
      window_ = std::make_unique<QQuickWindow>(control_.get());
      window_->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(&context_));
      if (!control_->initialize()) {
        ADD_FAILURE() << "Render control initialization failed";
        return false;
      }
    } else {
      window_ = std::make_unique<QQuickWindow>();
    }
    return true;
  }

  bool gpu_ = qEnvironmentVariable("HN_SEPARATOR_BACKEND") == QStringLiteral("opengl");
  QQmlEngine engine_;
  QOpenGLContext context_;
  QOffscreenSurface surface_;
  std::unique_ptr<QOpenGLFramebufferObject> buffer_;
  std::unique_ptr<QQuickRenderControl> control_;
  std::unique_ptr<QQuickWindow> window_;
  std::unique_ptr<QQuickItem> root_;
};

qreal snap(qreal value) { return std::floor(value + 0.5); }

// Oracle uses public root dimensions/transforms and requested pixel count, never helper output.
QRectF physicalBounds(QQuickItem* separator, qreal dpr, bool vertical, int stroke, int alignment) {
  const QPointF origin = separator->mapToScene({});
  const QPointF end = separator->mapToScene(QPointF(separator->width(), separator->height()));
  QPointF first(snap(origin.x() * dpr), snap(origin.y() * dpr));
  QPointF last(snap(end.x() * dpr), snap(end.y() * dpr));
  qreal& leading = vertical ? first.rx() : first.ry();
  qreal& trailing = vertical ? last.rx() : last.ry();
  const qreal start = vertical ? origin.x() : origin.y();
  const qreal finish = vertical ? end.x() : end.y();
  const qreal pixels = std::copysign(stroke, finish - start);
  if (alignment == 1) {
    leading = snap(((start + finish) * dpr / 2) - (pixels / 2));
  }
  if (alignment == 2) {
    leading = trailing - pixels;
  } else {
    trailing = leading + pixels;
  }
  return QRectF(first, last).normalized();
}

qreal fadeStrength(int mode, qreal position) {
  switch (mode) {
    case 1:
      return 1 - std::abs((2 * position) - 1);
    case 2:
      return std::min(1.0, 2 * position);
    case 3:
      return std::min(1.0, 2 * (1 - position));
    default:
      return 1;
  }
}

// Assertion macro branches dominate this full-coverage pixel oracle.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void expectStroke(const QImage& image, QQuickItem* separator, qreal dpr, bool vertical, int mode, int stroke,
                  int alignment, qreal alpha) {
  const QPointF origin = separator->mapToScene({});
  const QPointF end = separator->mapToScene(QPointF(separator->width(), separator->height()));
  const QRectF bounds = physicalBounds(separator, dpr, vertical, stroke, alignment);
  const qreal major_start = snap((vertical ? origin.y() : origin.x()) * dpr);
  const qreal major_end = snap((vertical ? end.y() : end.x()) * dpr);
  const int minor_start = qRound(vertical ? bounds.left() : bounds.top());
  const int minor_end = qRound(vertical ? bounds.right() : bounds.bottom());
  const int major_first = qRound(std::min(major_start, major_end));
  const int major_last = qRound(std::max(major_start, major_end));
  // Entire stroke, including both endpoints and adjacent rows/columns, catches overshoot and clipping.
  for (int major = major_first - 1; major <= major_last; ++major) {
    for (int minor = minor_start - 1; minor <= minor_end; ++minor) {
      const int pixel_x = vertical ? minor : major;
      const int pixel_y = vertical ? major : minor;
      ASSERT_TRUE(image.rect().contains(pixel_x, pixel_y));
      const bool painted =
          stroke > 0 && major >= major_first && major < major_last && minor >= minor_start && minor < minor_end;
      const qreal position = major_end == major_start ? 0 : (major + 0.5 - major_start) / (major_end - major_start);
      const qreal profile = fadeStrength(mode, position);
      const QColor pixel = image.pixelColor(pixel_x, pixel_y);
      const qreal expected = 32 + (painted ? 223 * alpha * profile : 0);
      ASSERT_NEAR(pixel.red(), expected, 3) << "pixel=" << pixel_x << ',' << pixel_y << " t=" << position;
      ASSERT_EQ(pixel.red(), pixel.green());
      ASSERT_EQ(pixel.red(), pixel.blue());
    }
  }
}

}  // namespace

// GTest assertion macros contribute branches to the complexity metric.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(SeparatorRendering, PhysicalCoverageAndFadeProfiles) {
  SeparatorScene scene(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      width: 180; height: 180
      Item {
        objectName: "ancestor"; opacity: 0.9
        HnSeparator {
          objectName: "separator"; x: 20.3; y: 20.3; width: 120.3; height: 120.3
          color: "#80ffffff"; opacity: 0.8
        }
      }
    }
  )");
  ASSERT_NE(scene.root(), nullptr);
  auto* separator = scene.root()->findChild<QQuickItem*>("separator");
  ASSERT_NE(separator, nullptr);
  const qreal dpr = scene.window()->effectiveDevicePixelRatio();
  ASSERT_DOUBLE_EQ(dpr, qEnvironmentVariable("QT_SCALE_FACTOR", "1").toDouble());
  for (bool vertical : {false, true}) {
    separator->setProperty("orientation", vertical ? Qt::Vertical : Qt::Horizontal);
    for (int align : {0, 1, 2}) {
      separator->setProperty("crossAxisAlignment", align);
      for (int mode = 0; mode < 4; ++mode) {
        separator->setProperty("fadeMode", mode);
        for (int stroke : {1, 2, 3, 0, -1}) {
          separator->setProperty("thickness", stroke);
          for (qreal offset : {-0.75, -0.5, -0.001, 0.0, 0.125, 0.5, 0.501, 0.75}) {
            SCOPED_TRACE(::testing::Message() << "DPR=" << dpr << " vertical=" << vertical << " mode=" << mode
                                              << " stroke=" << stroke << " offset=" << offset << " align=" << align);
            separator->setPosition(QPointF(20.3 + offset, 20.3 + offset));
            const auto image = scene.grab();
            ASSERT_EQ(image.size(), QSize(qRound(180 * dpr), qRound(180 * dpr)));
            expectStroke(image, separator, dpr, vertical, mode, std::max(0, stroke), align, (128.0 / 255) * 0.8 * 0.9);
            if (::testing::Test::HasFatalFailure()) {
              return;
            }
          }
        }
      }
    }
  }
}

// GTest assertion macros contribute branches to the complexity metric.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(SeparatorRendering, SignedAncestorScaleAndLifecycle) {
  SeparatorScene scene(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      width: 420; height: 420
      Item {
        objectName: "outer"; x: 190.125; y: 190.375; width: 80; height: 80
        Item {
          objectName: "inner"; x: -20.25; y: -20.125; width: 60; height: 60
          HnSeparator {
            objectName: "separator"; x: -10.125; y: -10.375; width: 40.3; height: 40.3
            color: "#80ffffff"; opacity: 0.8
          }
        }
      }
      Item { objectName: "alternative"; x: 170.375; y: 170.125; width: 70; height: 70; scale: -1.25 }
    }
  )");
  ASSERT_NE(scene.root(), nullptr);
  auto* outer = scene.root()->findChild<QQuickItem*>("outer");
  auto* inner = scene.root()->findChild<QQuickItem*>("inner");
  auto* separator = scene.root()->findChild<QQuickItem*>("separator");
  auto* alternative = scene.root()->findChild<QQuickItem*>("alternative");
  ASSERT_TRUE(outer && inner && separator && alternative);
  const qreal dpr = scene.window()->effectiveDevicePixelRatio();
  const auto check = [&](bool vertical, int mode, int stroke, int align) {
    expectStroke(scene.grab(), separator, dpr, vertical, mode, stroke, align, (128.0 / 255) * 0.8);
  };
  for (bool vertical : {false, true}) {
    separator->setProperty("orientation", vertical ? Qt::Vertical : Qt::Horizontal);
    for (int align : {0, 1, 2}) {
      separator->setProperty("crossAxisAlignment", align);
      for (int mode = 0; mode < 4; ++mode) {
        separator->setProperty("fadeMode", mode);
        for (int stroke : {1, 2, 3}) {
          separator->setProperty("thickness", stroke);
          for (qreal scale : {0.5, -0.5, 1.0, -1.0, 1.25, -1.25, 2.0, -2.0}) {
            SCOPED_TRACE(::testing::Message() << "dpr=" << dpr << " vertical=" << vertical << " mode=" << mode
                                              << " stroke=" << stroke << " scale=" << scale << " align=" << align);
            outer->setScale(scale);
            check(vertical, mode, stroke, align);
            if (::testing::Test::HasFatalFailure()) {
              return;
            }
          }
        }
      }
    }
    separator->setProperty("crossAxisAlignment", 2);
    separator->setProperty("fadeMode", 2);
    separator->setProperty("thickness", 2);
    outer->setScale(0);
    const auto blank = scene.grab();
    for (int pixel_y = 4; pixel_y < blank.height() - 4; ++pixel_y) {
      for (int pixel_x = 4; pixel_x < blank.width() - 4; ++pixel_x) {
        ASSERT_EQ(blank.pixelColor(pixel_x, pixel_y), QColor("#202020"));
      }
    }
    outer->setScale(1.25);
    inner->setScale(-0.5);
    check(vertical, 2, 2, 2);
    separator->setScale(-2);
    check(vertical, 2, 2, 2);
    separator->setScale(1);
    outer->setSize(QSizeF(83.3, 85.7));
    inner->setSize(QSizeF(63.7, 65.3));
    check(vertical, 2, 2, 2);
    outer->setTransformOrigin(QQuickItem::BottomRight);
    check(vertical, 2, 2, 2);
    inner->setParentItem(alternative);
    check(vertical, 2, 2, 2);
    alternative->setSize(QSizeF(73.3, 75.7));
    check(vertical, 2, 2, 2);
    inner->setParentItem(outer);
    inner->setScale(1);
    outer->setTransformOrigin(QQuickItem::Center);
  }
}

// GTest assertion macros contribute branches to the complexity metric.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(SeparatorRendering, ConnectedBoundariesHaveSingleCoverage) {
  SeparatorScene scene(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      width: 220; height: 180; clip: true
      Item {
        id: frame; objectName: "frame"; x: 20.25; y: 20.375; width: 160.3; height: 120.3
        // Panels model real ownership; clipping belongs to the outer viewport, not a rounded boundary.
        Item {
          id: header; width: parent.width; height: 24.3
          HnSeparator { objectName: "header"; color: "#80ffffff"
            crossAxisAlignment: HnSeparator.Trailing
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom } }
        }
        Item {
          id: content; y: header.height; width: parent.width; height: parent.height - y - 20.3
          Item {
            width: 40.3; height: parent.height
            HnSeparator { objectName: "sidebar"; color: "#80ffffff"; orientation: Qt.Vertical
              crossAxisAlignment: HnSeparator.Trailing
              anchors { right: parent.right; top: parent.top; bottom: parent.bottom } }
          }
          Item {
            id: columns; x: 40.3; width: parent.width - x; height: 28.3
            HnSeparator { id: bottom; objectName: "bottom"; color: "#80ffffff"
              crossAxisAlignment: HnSeparator.Trailing
              anchors { left: parent.left; right: parent.right; bottom: parent.bottom } }
            HnSeparator { objectName: "column"; color: "#80ffffff"; orientation: Qt.Vertical
              x: 55.3; anchors.top: parent.top; anchors.bottom: bottom.top }
          }
        }
        Item {
          y: content.y + content.height; width: parent.width; height: 20.3
          HnSeparator { objectName: "footer"; color: "#80ffffff"
            anchors { left: parent.left; right: parent.right; top: parent.top } }
        }
      }
    }
  )");
  ASSERT_NE(scene.root(), nullptr);
  auto* frame = scene.root()->findChild<QQuickItem*>("frame");
  const qreal dpr = scene.window()->effectiveDevicePixelRatio();
  for (qreal offset : {-0.75, -0.5, 0.0, 0.125, 0.5, 0.75}) {
    frame->setPosition(QPointF(20.25 + offset, 20.375 + offset));
    for (qreal width : {160.3, 145.7}) {
      frame->setWidth(width);
      const QImage image = scene.grab();
      QList<QRectF> expected;
      for (const char* name : {"header", "sidebar", "bottom", "column", "footer"}) {
        auto* item = scene.root()->findChild<QQuickItem*>(name);
        ASSERT_NE(item, nullptr);
        expected.append(physicalBounds(item, dpr, item->property("orientation").toInt() == Qt::Vertical, 1,
                                       item->property("crossAxisAlignment").toInt()));
      }
      // Entire fixture includes all four junctions and surrounding pixels. Exclude only
      // the offscreen window backing-buffer padding at fractional framebuffer sizes.
      for (int pixel_y = 4; pixel_y < image.height() - 4; ++pixel_y) {
        for (int pixel_x = 4; pixel_x < image.width() - 4; ++pixel_x) {
          const QPointF center(pixel_x + 0.5, pixel_y + 0.5);
          int owners = 0;
          for (const auto& rect : expected) {
            owners += rect.contains(center) ? 1 : 0;
          }
          ASSERT_LE(owners, 1) << "Overlapping boundary ownership at " << center.x() << ',' << center.y();
          ASSERT_NEAR(image.pixelColor(pixel_x, pixel_y).red(), 32 + (owners ? 223 * (128.0 / 255) : 0), 2)
              << "DPR=" << dpr << " offset=" << offset << " pixel=" << pixel_x << ',' << pixel_y;
        }
      }
      EXPECT_DOUBLE_EQ(expected.at(0).bottom(), expected.at(1).top());
      EXPECT_DOUBLE_EQ(expected.at(0).bottom(), expected.at(3).top());
      EXPECT_DOUBLE_EQ(expected.at(3).bottom(), expected.at(2).top());
      EXPECT_DOUBLE_EQ(expected.at(1).bottom(), expected.at(4).top());
    }
  }
}

// GTest assertion macros contribute branches to the complexity metric.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(SeparatorRendering, LayoutAttachmentAndDprChanges) {
  SeparatorScene scene(R"(
    import QtQuick
    import QtQuick.Layouts
    import Holonight.Controls
    Item {
      width: 180; height: 180
      ColumnLayout {
        x: 20.3; y: 20.3; width: 120.3; spacing: 0
        HnSeparator { objectName: "horizontal"; Layout.fillWidth: true }
      }
      RowLayout {
        x: 30.3; y: 30.3; height: 120.3; spacing: 0
        HnSeparator { objectName: "vertical"; orientation: Qt.Vertical; Layout.fillHeight: true }
      }
    }
  )");
  ASSERT_NE(scene.root(), nullptr);
  for (qreal ratio : {1.0, 1.25, 1.5, 1.5625, 1.75, 2.0, 1.0}) {
    scene.setDpr(ratio);
    const auto image = scene.grab();
    ASSERT_FALSE(image.isNull());
    const qreal dpr = scene.window()->effectiveDevicePixelRatio();
    if (scene.gpu()) {
      ASSERT_DOUBLE_EQ(dpr, ratio);
    }
    for (const char* name : {"horizontal", "vertical"}) {
      auto* item = scene.root()->findChild<QQuickItem*>(name);
      ASSERT_NE(item, nullptr);
      const bool vertical = item->property("orientation").toInt() == Qt::Vertical;
      // Layouts may round their slot dimensions; the preferred occupancy stays logical.
      EXPECT_NEAR(vertical ? item->implicitWidth() : item->implicitHeight(), 1 / dpr, 1e-6);
      auto* line = item->findChild<QQuickItem*>("separatorLine");
      ASSERT_NE(line, nullptr);
      const QRectF bounds = line->mapRectToScene(line->boundingRect());
      EXPECT_NEAR((vertical ? bounds.width() : bounds.height()) * dpr, 1, 1e-6);
      QQuickItem* parent = item->parentItem();
      item->setParentItem(nullptr);
      EXPECT_NEAR(vertical ? item->implicitWidth() : item->implicitHeight(), 1, 1e-6);
      item->setParentItem(parent);
      EXPECT_NEAR(vertical ? item->implicitWidth() : item->implicitHeight(), 1 / dpr, 1e-6);
      item->setProperty("thickness", 0);
      EXPECT_FALSE(line->isVisible());
      item->setProperty("thickness", 3);
      EXPECT_TRUE(line->isVisible());
      item->setProperty("thickness", 1);
    }
  }
}

// GTest assertion macros contribute branches to the complexity metric.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(SeparatorRendering, AnchorDefaultsAndZeroLengthRecovery) {
  SeparatorScene scene(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      width: 180; height: 180
      Item {
        x: 20.3; y: 20.3; width: 120.3
        HnSeparator { objectName: "horizontal"; color: "#80ffffff"
          anchors { left: parent.left; right: parent.right; top: parent.top } }
      }
      Item {
        x: 40.3; y: 40.3; height: 120.3
        HnSeparator { objectName: "vertical"; color: "#80ffffff"; orientation: Qt.Vertical
          anchors { top: parent.top; bottom: parent.bottom; left: parent.left } }
      }
    }
  )");
  ASSERT_NE(scene.root(), nullptr);
  const qreal dpr = scene.window()->effectiveDevicePixelRatio();
  for (const char* name : {"horizontal", "vertical"}) {
    auto* item = scene.root()->findChild<QQuickItem*>(name);
    ASSERT_NE(item, nullptr);
    const bool vertical = item->property("orientation").toInt() == Qt::Vertical;
    auto* line = item->findChild<QQuickItem*>("separatorLine");
    ASSERT_NE(line, nullptr);
    for (int stroke : {1, 2, 3, 0, -1, 1}) {
      item->setProperty("thickness", stroke);
      const auto image = scene.grab();
      EXPECT_NEAR(vertical ? item->width() : item->height(), std::max(0, stroke) / dpr, 1e-6);
      EXPECT_EQ(line->isVisible(), stroke > 0);
      expectStroke(image, item, dpr, vertical, 0, std::max(0, stroke), 0, 128.0 / 255);
    }
    for (qreal length : {0.0, 120.3, 0.0, 120.3}) {
      if (vertical) {
        item->parentItem()->setHeight(length);
      } else {
        item->parentItem()->setWidth(length);
      }
      scene.grab();
      EXPECT_EQ(line->isVisible(), length > 0);
    }
    item->setVisible(false);
    scene.grab();
    EXPECT_FALSE(line->isVisible());
    item->setVisible(true);
    expectStroke(scene.grab(), item, dpr, vertical, 0, 1, 0, 128.0 / 255);
  }
}
