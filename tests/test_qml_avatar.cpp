// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QDir>
#include <QImage>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickImageProvider>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QTest>
#include <QThread>

#include <gtest/gtest.h>
#include <memory>

namespace {
class AvatarImages : public QQuickImageProvider {
 public:
  AvatarImages() : QQuickImageProvider(Image, ForceAsynchronousImageLoading) {}
  QImage requestImage(const QString& image_id, QSize* size, const QSize& /*requestedSize*/) override {
    // Keep loading observable without depending on disk cache or network
    // timing.
    QThread::msleep(50);
    if (image_id == "missing") {
      return {};
    }
    QSize image_size{80, 80};
    if (image_id == "wide") {
      image_size = QSize{240, 80};
    } else if (image_id == "tall") {
      image_size = QSize{80, 240};
    }
    QImage image{image_size, QImage::Format_ARGB32_Premultiplied};
    QColor color = Qt::blue;
    if (image_id == "fallback") {
      color = Qt::yellow;
    } else if (image_id == "transparent") {
      color = QColor{0, 0, 255, 128};
    }
    image.fill(color);
    *size = image.size();
    return image;
  }
};

class QmlAvatar : public testing::Test {
 protected:
  // GTest-derived tests intentionally share protected fixture state.
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QQmlEngine engine;
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  std::unique_ptr<QQuickWindow> window;
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QObject* avatar{};
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QObject* image{};
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QObject* fallback{};
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  QObject* effect{};

  void SetUp() override {
    engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
    engine.addImageProvider(QStringLiteral("avatars"), new AvatarImages);
    QQmlComponent component{&engine};
    component.setData(R"(
      import QtQuick
      import Holonight.Controls
      Window {
        visible: true
        width: 160; height: 160; color: "black"
        HnAvatar {
          objectName: "avatar"
          size: 132; imageInset: 4
          backgroundColor: "red"; ringColor: "lime"; ringWidth: 1
          fallbackSource: "image://avatars/fallback"
        }
      }
    )",
                      QUrl{});
    ASSERT_EQ(component.status(), QQmlComponent::Ready) << component.errorString().toStdString();
    window.reset(qobject_cast<QQuickWindow*>(component.create()));
    ASSERT_NE(window, nullptr);
    avatar = window->findChild<QObject*>(QStringLiteral("avatar"));
    ASSERT_NE(avatar, nullptr);
    image = avatar->findChild<QObject*>(QStringLiteral("hnAvatarImage"));
    fallback = avatar->findChild<QObject*>(QStringLiteral("hnAvatarFallback"));
    effect = avatar->findChild<QObject*>(QStringLiteral("hnAvatarEffect"));
    ASSERT_NE(image, nullptr);
    ASSERT_NE(fallback, nullptr);
    ASSERT_NE(effect, nullptr);
    ASSERT_TRUE(QTest::qWaitFor([&] { return fallback->property("status").toInt() == 1; }));
  }
  void load(const char* name) {
    avatar->setProperty("source", QUrl{QStringLiteral("image://avatars/") + QString::fromLatin1(name)});
  }
  QObject* displayed() { return effect->property("source").value<QObject*>(); }
};

TEST_F(QmlAvatar, LoadingFailureEmptyAndAccountSwitchNeverShowStaleImage) {
  EXPECT_EQ(displayed(), fallback);
  load("square");
  EXPECT_EQ(image->property("status").toInt(), 2);
  EXPECT_EQ(displayed(), fallback);
  ASSERT_TRUE(QTest::qWaitFor([&] { return image->property("status").toInt() == 1; }));
  EXPECT_EQ(displayed(), image);
  load("wide");
  EXPECT_EQ(image->property("status").toInt(), 2);
  EXPECT_EQ(displayed(), fallback);
  ASSERT_TRUE(QTest::qWaitFor([&] { return image->property("status").toInt() == 1; }));
  EXPECT_EQ(displayed(), image);
  load("missing");
  EXPECT_EQ(displayed(), fallback);
  ASSERT_TRUE(QTest::qWaitFor([&] { return image->property("status").toInt() == 3; }));
  EXPECT_EQ(displayed(), fallback);
  avatar->setProperty("source", QUrl{});
  EXPECT_EQ(displayed(), fallback);
  avatar->setProperty("fallbackSource", QUrl{QStringLiteral("image://avatars/missing")});
  ASSERT_TRUE(QTest::qWaitFor([&] { return fallback->property("status").toInt() == 3; }));
  EXPECT_FALSE(effect->property("visible").toBool());
}

TEST_F(QmlAvatar, ShaderMaskCropRingAndTransparencyAtBothSizes) {
  if (window->rendererInterface()->graphicsApi() == QSGRendererInterface::Software) {
    GTEST_SKIP() << "Masking requires a shader-capable rendering backend";
  }
  for (const int size : {56, 132}) {
    avatar->setProperty("size", size);
    for (const char* shape : {"square", "wide", "tall", "transparent"}) {
      load(shape);
      ASSERT_TRUE(QTest::qWaitFor([&] { return image->property("status").toInt() == 1; }));
      QTest::qWait(80);
      const QImage rendered = window->grabWindow();
      ASSERT_FALSE(rendered.isNull());
      const QString capture_dir = qEnvironmentVariable("HOLONIGHT_AVATAR_CAPTURE_DIR");
      if (!capture_dir.isEmpty()) {
        ASSERT_TRUE(rendered.save(
            QDir{capture_dir}.filePath(QStringLiteral("avatar-%1-%2.png").arg(size).arg(QString::fromLatin1(shape)))));
      }
      const qreal scale = static_cast<qreal>(rendered.width()) / window->width();
      const auto pixel = [&](qreal pixel_x, qreal pixel_y) {
        return rendered.pixelColor(qRound(pixel_x * scale), qRound(pixel_y * scale));
      };
      EXPECT_EQ(pixel(0, 0), QColor{Qt::black});
      EXPECT_EQ(pixel(4, 4), QColor{Qt::black});
      EXPECT_GT(pixel(size / 2.0, 0).green(), 100);
      EXPECT_EQ(pixel(size / 2.0, 2), QColor{Qt::red});
      const QColor center = pixel(size / 2.0, size / 2.0);
      if (QString::fromLatin1(shape) == QStringLiteral("transparent")) {
        EXPECT_NEAR(center.blue(), 128, 8);
        EXPECT_NEAR(center.red(), 127, 8);
      } else {
        EXPECT_EQ(center, QColor{Qt::blue});
        // Crop fills the circle even for non-square sources.
        EXPECT_EQ(pixel(size / 2.0, 7), QColor{Qt::blue});
        EXPECT_EQ(pixel(7, size / 2.0), QColor{Qt::blue});
        bool blended_edge = false;
        for (int pixel_y = qRound(4 * scale); pixel_y < qRound((size - 4) * scale); ++pixel_y) {
          for (int pixel_x = qRound(4 * scale); pixel_x < qRound((size - 4) * scale); ++pixel_x) {
            const QColor color = rendered.pixelColor(pixel_x, pixel_y);
            blended_edge |= color.blue() > 10 && color.blue() < 245 && color.red() > 10;
          }
        }
        EXPECT_TRUE(blended_edge) << "Circular image edge must preserve antialiasing";
      }
    }
  }
}
}  // namespace
