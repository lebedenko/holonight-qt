// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconengine.h"
#include "hniconimageprovider.h"
#include "iconrenderer.h"
#include "iconthemeresolver.h"

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QPalette>
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

TEST(IconRenderer, PreservesOrdinaryPaintAndRecolorsSymbolicMask) {
  const auto colors = semanticColors(QColor{QStringLiteral("#123456")});
  const QImage image = Holonight::IconRenderer::renderSvg(QByteArray{kHardcodedMonochromeSvg}, QSize{30, 10}, colors);
  ASSERT_FALSE(image.isNull());
  EXPECT_EQ(image.pixelColor(5, 5), QColor(QStringLiteral("#c0caf5")));
  EXPECT_EQ(image.pixelColor(15, 5), QColor(QStringLiteral("#112233")));
  EXPECT_EQ(image.pixelColor(25, 5), QColor(QStringLiteral("#445566")));
  const QImage symbolic =
      Holonight::IconRenderer::renderSvg(QByteArray{kHardcodedMonochromeSvg}, QSize{30, 10}, colors, true);
  EXPECT_EQ(symbolic.pixelColor(5, 5), colors.text);
  EXPECT_EQ(symbolic.pixelColor(15, 5), colors.text);
  EXPECT_EQ(symbolic.pixelColor(25, 5), colors.text);
}

TEST(IconRenderer, RendersAtRequestedImageSize) {
  const QImage image = Holonight::IconRenderer::renderSvg(QByteArray{kSemanticSvg}, QSize{25, 5}, semanticColors());
  ASSERT_FALSE(image.isNull());
  EXPECT_EQ(image.size(), QSize(25, 5));
  EXPECT_EQ(image.devicePixelRatio(), 1.0);
}

TEST(IconRenderer, KeepsFixedPaintAlongsideSemanticRoles) {
  const QByteArray svg = R"(<svg xmlns='http://www.w3.org/2000/svg' width='20' height='10'>
    <style>.ColorScheme-Text{color:#111111}</style>
    <rect class='ColorScheme-Text' fill='currentColor' width='10' height='10'/>
    <rect x='10' width='10' height='10' fill='#123456'/>
  </svg>)";
  const QImage image = Holonight::IconRenderer::renderSvg(svg, QSize{20, 10}, semanticColors());
  ASSERT_FALSE(image.isNull());
  EXPECT_EQ(image.pixelColor(5, 5), Qt::red);
  EXPECT_EQ(image.pixelColor(15, 5), QColor{QStringLiteral("#123456")});
}

TEST(HnIconImageProvider, CachesBySourceSizeAndColor) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("icon.svg"));
  writeFile(path, QByteArray{kSemanticSvg});
  const QByteArray resolved_svg =
      Holonight::IconThemeResolver::readIconBytes(Holonight::IconThemeResolver::resolveIconPath(path));
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

TEST(HnIconImageProvider, InheritedThemeAndSourceChangesInvalidateImages) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const auto old_paths = QIcon::themeSearchPaths();
  const auto old_theme = QIcon::themeName();
  struct Restore {
    QStringList paths;
    QString theme;
    ~Restore() {
      QIcon::setThemeSearchPaths(paths);
      QIcon::setThemeName(theme);
    }
  } restore{old_paths, old_theme};
  for (const auto* theme : {"child", "parent", "other"}) {
    ASSERT_TRUE(QDir(dir.path()).mkpath(QString(theme) + "/16x16/actions"));
    writeFile(dir.filePath(QString(theme) + "/index.theme"),
              "[Icon Theme]\nName=Fixture\nInherits=" +
                  QByteArray(theme == std::string("child") ? "missing,parent" : "child") +
                  "\nDirectories=16x16/actions\n[16x16/actions]\nSize=16\nType=Fixed\nContext=Actions\n");
  }
  const auto path = dir.filePath("parent/16x16/actions/uqc-inherited.svg");
  writeFile(path, QByteArray{kSemanticSvg});
  QIcon::setThemeSearchPaths({dir.path()});
  QIcon::setThemeName("child");
  ASSERT_TRUE(QIcon::hasThemeIcon("uqc-inherited"));
  Holonight::HnIconImageProvider provider;
  const auto id = providerId("uqc-inherited", 50, QColor("#ff0000"));
  const auto first = provider.requestImage(id, nullptr, {});
  EXPECT_FALSE(first.isNull());
  writeFile(path, QByteArray(kSemanticSvg).replace("width=\"10\"", "width=\"1\""));
  const auto changed = provider.requestImage(id, nullptr, {});
  EXPECT_NE(first, changed);
  writeFile(dir.filePath("other/16x16/actions/uqc-inherited.svg"), QByteArray{kHardcodedMonochromeSvg});
  QIcon::setThemeName("other");
  EXPECT_NE(changed, provider.requestImage(id, nullptr, {}));
  EXPECT_TRUE(provider.requestImage(providerId("uqc-missing", 24, Qt::white), nullptr, {}).isNull());
  writeFile(dir.filePath("other/16x16/actions/uqc-missing.svg"), QByteArray{kSemanticSvg});
  EXPECT_FALSE(provider.requestImage(providerId("uqc-missing", 24, Qt::white), nullptr, {}).isNull());
}

TEST(HnIconImageProvider, SeparatesSymbolicMaskFromOrdinaryArtworkWithIdenticalBytes) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const auto old_paths = QIcon::themeSearchPaths();
  const auto old_theme = QIcon::themeName();
  struct Restore {
    QStringList paths;
    QString theme;
    ~Restore() {
      QIcon::setThemeSearchPaths(paths);
      QIcon::setThemeName(theme);
    }
  } restore{old_paths, old_theme};
  ASSERT_TRUE(QDir{dir.path()}.mkpath(QStringLiteral("fixture/24x24/actions")));
  writeFile(dir.filePath(QStringLiteral("fixture/index.theme")),
            "[Icon Theme]\nName=Fixture\nDirectories=24x24/actions\n[24x24/actions]\nSize=24\nType=Fixed\n");
  for (const auto* name : {"hn-cache-regular", "hn-cache-regular-symbolic"}) {
    writeFile(
        dir.filePath(QStringLiteral("fixture/24x24/actions/") + QString::fromLatin1(name) + QStringLiteral(".svg")),
        kHardcodedMonochromeSvg);
  }
  QIcon::setThemeSearchPaths({dir.path()});
  QIcon::setThemeName(QStringLiteral("fixture"));
  Holonight::HnIconImageProvider provider;
  const QImage regular =
      provider.requestImage(providerId(QStringLiteral("hn-cache-regular"), 30, Qt::red), nullptr, {});
  const QImage symbolic =
      provider.requestImage(providerId(QStringLiteral("hn-cache-regular-symbolic"), 30, Qt::red), nullptr, {});
  ASSERT_FALSE(regular.isNull());
  ASSERT_FALSE(symbolic.isNull());
  EXPECT_EQ(regular.pixelColor(5, 5), QColor{QStringLiteral("#c0caf5")});
  EXPECT_EQ(symbolic.pixelColor(5, 5), Qt::red);
  EXPECT_EQ(provider.cacheSize(), 2);
}

TEST(HnIconEngine, SelectsSizeModeAndPaletteFromInheritedTheme) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const auto old_paths = QIcon::themeSearchPaths();
  const auto old_theme = QIcon::themeName();
  const auto old_palette = QGuiApplication::palette();
  struct Restore {
    QStringList paths;
    QString theme;
    QPalette palette;
    ~Restore() {
      QIcon::setThemeSearchPaths(paths);
      QIcon::setThemeName(theme);
      QGuiApplication::setPalette(palette);
    }
  } restore{old_paths, old_theme, old_palette};
  ASSERT_TRUE(QDir{dir.path()}.mkpath(QStringLiteral("child")));
  writeFile(dir.filePath(QStringLiteral("child/index.theme")),
            "[Icon Theme]\nName=Child\nInherits=parent\nDirectories=\n");
  for (const int extent : {16, 24, 32}) {
    const QString directory = QStringLiteral("parent/%1x%1/places").arg(extent);
    ASSERT_TRUE(QDir{dir.path()}.mkpath(directory));
    const QByteArray color = extent == 32 ? "#0000ff" : "#ffffff";
    writeFile(
        dir.filePath(directory + QStringLiteral("/hn-fixture-home.svg")),
        QByteArray{
            "<svg xmlns='http://www.w3.org/2000/svg' width='24' height='24'><rect width='24' height='24' fill='"} +
            color + "'/></svg>");
    writeFile(dir.filePath(directory + QStringLiteral("/hn-fixture-home-symbolic.svg")),
              "<svg xmlns='http://www.w3.org/2000/svg' width='24' height='24'><rect width='24' height='24' "
              "fill='#000'/></svg>");
  }
  writeFile(dir.filePath(QStringLiteral("parent/index.theme")),
            "[Icon Theme]\nName=Parent\nDirectories=16x16/places,24x24/places,32x32/places\n"
            "[16x16/places]\nSize=16\nType=Fixed\n"
            "[24x24/places]\nSize=24\nType=Fixed\n"
            "[32x32/places]\nSize=32\nType=Fixed\n");
  QImage bitmap{24, 24, QImage::Format_ARGB32};
  bitmap.fill(Qt::magenta);
  ASSERT_TRUE(bitmap.save(dir.filePath(QStringLiteral("parent/24x24/places/hn-fixture-bitmap.png"))));
  QIcon::setThemeSearchPaths({dir.path()});
  QIcon::setThemeName(QStringLiteral("child"));
  QPalette palette = old_palette;
  palette.setColor(QPalette::Active, QPalette::Text, QColor{QStringLiteral("#ff0000")});
  palette.setColor(QPalette::Active, QPalette::HighlightedText, QColor{QStringLiteral("#00ff00")});
  palette.setColor(QPalette::Disabled, QPalette::Text, QColor{QStringLiteral("#777777")});
  QGuiApplication::setPalette(palette);

  Holonight::HnIconEngine regular{QStringLiteral("hn-fixture-home")};
  EXPECT_EQ(regular.pixmap(QSize{16, 16}, QIcon::Normal, QIcon::Off).toImage().pixelColor(8, 8), Qt::white);
  EXPECT_EQ(regular.pixmap(QSize{24, 24}, QIcon::Normal, QIcon::Off).toImage().pixelColor(12, 12), Qt::white);
  EXPECT_EQ(regular.pixmap(QSize{32, 32}, QIcon::Normal, QIcon::Off).toImage().pixelColor(16, 16), Qt::blue);
  Holonight::HnIconEngine bitmap_engine{QStringLiteral("hn-fixture-bitmap")};
  EXPECT_EQ(bitmap_engine.pixmap(QSize{24, 24}, QIcon::Normal, QIcon::Off).toImage().pixelColor(12, 12), Qt::magenta);
  Holonight::HnIconEngine symbolic{QStringLiteral("hn-fixture-home-symbolic")};
  EXPECT_EQ(symbolic.pixmap(QSize{16, 16}, QIcon::Normal, QIcon::Off).toImage().pixelColor(8, 8), Qt::red);
  EXPECT_EQ(symbolic.pixmap(QSize{32, 32}, QIcon::Selected, QIcon::Off).toImage().pixelColor(16, 16), Qt::green);
  EXPECT_EQ(symbolic.pixmap(QSize{24, 24}, QIcon::Disabled, QIcon::Off).toImage().pixelColor(12, 12),
            QColor{QStringLiteral("#777777")});
  const QImage scaled = regular.scaledPixmap(QSize{24, 24}, QIcon::Normal, QIcon::Off, 1.5).toImage();
  EXPECT_EQ(scaled.size(), QSize(36, 36));
  EXPECT_EQ(scaled.pixelColor(18, 18), Qt::white);
  EXPECT_EQ(regular.scaledPixmap(QSize{24, 24}, QIcon::Normal, QIcon::Off, 2.0).toImage().pixelColor(24, 24),
            Qt::white);
  palette.setColor(QPalette::Active, QPalette::Text, QColor{QStringLiteral("#00ffff")});
  QGuiApplication::setPalette(palette);
  EXPECT_EQ(symbolic.pixmap(QSize{16, 16}, QIcon::Normal, QIcon::Off).toImage().pixelColor(8, 8), Qt::cyan);
}
