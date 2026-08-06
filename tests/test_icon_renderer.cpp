// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconimageprovider.h"
#include "iconrenderer.h"
#include "iconthemeresolver.h"

#include <QFile>
#include <QTemporaryDir>
#include <QUrl>
#include <QUrlQuery>

#include <gtest/gtest.h>

namespace {

constexpr auto kSemanticSvg = R"(
<svg xmlns=")"
                              "http:"
                              R"(//www.w3.org/2000/svg" width="50" height="10" viewBox="0 0 50 10">
  <style type="text/css" id="current-color-scheme">
    .ColorScheme-Text { color:#111111; }
    .ColorScheme-Highlight { color:#222222; }
    .ColorScheme-PositiveText { color:#333333; }
    .ColorScheme-NeutralText { color:#444444; }
    .ColorScheme-NegativeText { color:#555555; }
  </style>
  <rect class="ColorScheme-Text" fill="currentColor" x="0" y="0" width="10" height="10"/>
  <rect class="ColorScheme-Highlight" fill="currentColor" x="10" y="0" width="10" height="10"/>
  <rect class="ColorScheme-PositiveText" fill="currentColor" x="20" y="0" width="10" height="10"/>
  <rect class="ColorScheme-NeutralText" fill="currentColor" x="30" y="0" width="10" height="10"/>
  <rect class="ColorScheme-NegativeText" fill="currentColor" x="40" y="0" width="10" height="10"/>
</svg>
)";

constexpr auto kHardcodedMonochromeSvg = R"(
<svg xmlns=")"
                                         "http:"
                                         R"(//www.w3.org/2000/svg" width="30" height="10" viewBox="0 0 30 10">
  <rect style="opacity:0;fill:#808080;" x="0" y="0" width="30" height="10"/>
  <rect style="fill:#c0caf5;" x="0" y="0" width="10" height="10"/>
  <rect fill="#112233" x="10" y="0" width="10" height="10"/>
  <line x1="20" y1="5" x2="30" y2="5" stroke="#445566" stroke-width="10"/>
</svg>
)";

[[nodiscard]] Holonight::IconSemanticColors semanticColors(const QColor& text = QColor{QStringLiteral("#ff0000")}) {
  return {
      .text = text,
      .highlight = QColor{QStringLiteral("#00ff00")},
      .positive = QColor{QStringLiteral("#0000ff")},
      .neutral = QColor{QStringLiteral("#ffff00")},
      .negative = QColor{QStringLiteral("#ff00ff")},
  };
}

void writeFile(const QString& path, const QByteArray& contents) {
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  ASSERT_TRUE(opened);
  ASSERT_EQ(file.write(contents), contents.size());
}

[[nodiscard]] QString providerId(const QString& source, int size, const QColor& color) {
  QUrlQuery query;
  query.addQueryItem(QStringLiteral("size"), QString::number(size));
  query.addQueryItem(QStringLiteral("color"), color.name(QColor::HexArgb));
  query.addQueryItem(QStringLiteral("highlight"), QStringLiteral("#ff00ff00"));
  query.addQueryItem(QStringLiteral("positive"), QStringLiteral("#ff0000ff"));
  query.addQueryItem(QStringLiteral("neutral"), QStringLiteral("#ffffff00"));
  query.addQueryItem(QStringLiteral("negative"), QStringLiteral("#ffff00ff"));
  query.addQueryItem(QStringLiteral("palette"), QStringLiteral("test"));
  return QString::fromLatin1(QUrl::toPercentEncoding(source)) + QLatin1Char('?') + query.toString(QUrl::FullyEncoded);
}

}  // namespace

TEST(IconRenderer, RendersExactSemanticClassColors) {
  const QImage image = Holonight::IconRenderer::renderSvg(QByteArray{kSemanticSvg}, QSize{50, 10}, semanticColors());
  ASSERT_FALSE(image.isNull());

  EXPECT_EQ(image.pixelColor(5, 5), QColor(QStringLiteral("#ff0000")));
  EXPECT_EQ(image.pixelColor(15, 5), QColor(QStringLiteral("#00ff00")));
  EXPECT_EQ(image.pixelColor(25, 5), QColor(QStringLiteral("#0000ff")));
  EXPECT_EQ(image.pixelColor(35, 5), QColor(QStringLiteral("#ffff00")));
  EXPECT_EQ(image.pixelColor(45, 5), QColor(QStringLiteral("#ff00ff")));
}

TEST(IconRenderer, RecolorsHardcodedMonochromePaints) {
  const QImage image = Holonight::IconRenderer::renderSvg(QByteArray{kHardcodedMonochromeSvg}, QSize{30, 10},
                                                          semanticColors(QColor{QStringLiteral("#123456")}));
  ASSERT_FALSE(image.isNull());

  EXPECT_EQ(image.pixelColor(5, 5), QColor(QStringLiteral("#123456")));
  EXPECT_EQ(image.pixelColor(15, 5), QColor(QStringLiteral("#123456")));
  EXPECT_EQ(image.pixelColor(25, 5), QColor(QStringLiteral("#123456")));
}

TEST(IconRenderer, RendersAtRequestedImageSize) {
  const QImage image = Holonight::IconRenderer::renderSvg(QByteArray{kSemanticSvg}, QSize{25, 5}, semanticColors());
  ASSERT_FALSE(image.isNull());
  EXPECT_EQ(image.size(), QSize(25, 5));
  EXPECT_EQ(image.devicePixelRatio(), 1.0);
}

TEST(HnIconImageProvider, CachesBySourceSizeAndColor) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("icon.svg"));
  writeFile(path, QByteArray{kSemanticSvg});
  const QByteArray resolved_svg = Holonight::IconThemeResolver::resolveSvgBytes(path);
  ASSERT_FALSE(resolved_svg.isEmpty());
  ASSERT_FALSE(Holonight::IconRenderer::renderSvg(resolved_svg, QSize{50, 50}, semanticColors()).isNull());

  Holonight::HnIconImageProvider provider;
  QSize image_size;

  const QImage first = provider.requestImage(providerId(path, 50, QColor{QStringLiteral("#ff0000")}), &image_size, {});
  ASSERT_FALSE(first.isNull());
  EXPECT_EQ(provider.cacheSize(), 1);

  const QImage same = provider.requestImage(providerId(path, 50, QColor{QStringLiteral("#ff0000")}), &image_size, {});
  ASSERT_FALSE(same.isNull());
  EXPECT_EQ(provider.cacheSize(), 1);

  const QImage other_color =
      provider.requestImage(providerId(path, 50, QColor{QStringLiteral("#00ffff")}), &image_size, {});
  ASSERT_FALSE(other_color.isNull());
  EXPECT_EQ(provider.cacheSize(), 2);

  const QImage other_size =
      provider.requestImage(providerId(path, 24, QColor{QStringLiteral("#00ffff")}), &image_size, {});
  ASSERT_FALSE(other_size.isNull());
  EXPECT_EQ(provider.cacheSize(), 3);
}

TEST(HnIconImageProvider, RejectsOversizedRenderDimensions) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("icon.svg"));
  writeFile(path, QByteArray{kSemanticSvg});

  Holonight::HnIconImageProvider provider;
  QSize image_size = QSize{1, 1};
  const QImage image =
      provider.requestImage(providerId(path, 24, QColor{QStringLiteral("#ff0000")}), &image_size, QSize{1025, 1});

  EXPECT_TRUE(image.isNull());
  EXPECT_TRUE(image_size.isEmpty());
  EXPECT_EQ(provider.cacheSize(), 0);
}

TEST(HnIconImageProvider, RejectsOversizedSvgSource) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("icon.svg"));
  writeFile(path, QByteArray(1024 * 1024 + 1, ' '));

  Holonight::HnIconImageProvider provider;
  const QImage image = provider.requestImage(providerId(path, 24, QColor{QStringLiteral("#ff0000")}), nullptr, {});

  EXPECT_TRUE(image.isNull());
  EXPECT_EQ(provider.cacheSize(), 0);
}

TEST(HnIconImageProvider, EvictsImagesWhenCacheCostLimitIsReached) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("icon.svg"));
  writeFile(path, QByteArray{kSemanticSvg});

  Holonight::HnIconImageProvider provider;
  for (int channel = 1; channel <= 6; ++channel) {
    const QColor color{channel, 0, 0};
    const QImage image = provider.requestImage(providerId(path, 1024, color), nullptr, {});
    ASSERT_FALSE(image.isNull());
  }

  EXPECT_LE(provider.cacheSize(), 4);
}
