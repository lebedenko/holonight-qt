// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconengine.h"
#include "hniconimageprovider.h"
#include "holonight/appearance_reader.h"
#include "holonight/palette.h"
#include "iconrenderer.h"
#include "iconthemeresolver.h"
#include "themeresolver.h"

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
    <style id="current-color-scheme">.ColorScheme-Text{color:#111111}</style>
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

TEST(IconRenderer, RestrictsStylesAndRecognizesExactAssignedRoles) {
  const QByteArray svg = R"(<svg xmlns='http://www.w3.org/2000/svg' width='30' height='10'>
    <style>.ColorScheme-Text{color:#111111}</style>
    <style type='text/css'
      id='current-color-scheme'><![CDATA[
      .ColorScheme-Accent{color:#222222;opacity:0.5}
      .ColorScheme-Background{color:#333333}
      .ColorScheme-HighlightedText{color:#444444}
      .ColorScheme-TextExtra{color:#555555}
    ]]></style>
    <rect class='other ColorScheme-Accent' fill='currentColor' width='10' height='10'/>
    <rect class='ColorScheme-Background' fill='currentColor' x='10' width='10' height='10'/>
    <rect class='ColorScheme-HighlightedText' fill='currentColor' x='20' width='10' height='10'/>
  </svg>)";
  auto colors = semanticColors();
  colors.accent = Qt::cyan;
  colors.background = Qt::yellow;
  colors.highlightedText = Qt::magenta;
  const auto bytes = Holonight::IconRenderer::applySemanticColors(svg, colors);
  EXPECT_TRUE(bytes.contains("<style>.ColorScheme-Text{color:#111111}</style>"));
  EXPECT_TRUE(bytes.contains(".ColorScheme-TextExtra{color:#555555}"));
  EXPECT_TRUE(bytes.contains("color:#00ffff;opacity:0.5"));
  EXPECT_TRUE(Holonight::IconRenderer::hasSemanticRoles(svg));
  EXPECT_FALSE(Holonight::IconRenderer::hasSemanticRoles("<path class='ColorScheme-AccentExtra'/>"));
  auto legacy = semanticColors();
  EXPECT_EQ(Holonight::IconRenderer::applySemanticColors(svg, legacy), svg);
  const auto image = Holonight::IconRenderer::renderSvg(svg, {30, 10}, colors, true);
  ASSERT_FALSE(image.isNull());
  EXPECT_EQ(image.pixelColor(15, 5), Qt::yellow);
  EXPECT_EQ(image.pixelColor(25, 5), Qt::magenta);
}

TEST(IconRenderer, SelectedAndDisabledUseSharedRolePolicy) {
  auto base = semanticColors();
  base.accent = QColor(20, 100, 200, 120);
  base.background = Qt::black;
  base.highlightedText = Qt::white;
  auto disabled = base;
  disabled.text = Qt::gray;
  disabled.accent = Holonight::blendIconColor(base.accent, base.background, 0.5);
  const auto selected = Holonight::resolveIconColors(base, disabled, Holonight::IconState::Selected);
  EXPECT_EQ(selected.text, Qt::white);
  EXPECT_EQ(selected.highlight, Qt::white);
  EXPECT_EQ(selected.positive, Qt::white);
  EXPECT_EQ(selected.background, base.highlight);
  EXPECT_EQ(selected.highlightedText, base.highlight);
  EXPECT_EQ(selected.accent.alpha(), base.accent.alpha());
  EXPECT_NE(selected.accent, selected.text);
  EXPECT_EQ(Holonight::resolveIconColors(base, disabled, Holonight::IconState::Disabled).accent, disabled.accent);
  EXPECT_EQ(Holonight::resolveIconColors(base, disabled, Holonight::IconState::Active).text, base.text);
}

TEST(HnIconImageProvider, AccentOnlyAndAlphaUpdatesPreserveLegacyAndOriginal) {
  QTemporaryDir dir;
  const auto path = dir.filePath("accent.svg");
  const QByteArray svg = R"(<svg xmlns='http://www.w3.org/2000/svg' width='24' height='24'>
    <style id='current-color-scheme'>.ColorScheme-Accent{color:#123456}</style>
    <rect class='ColorScheme-Accent' fill='currentColor' width='12' height='24'/>
    <rect x='12' width='12' height='24' fill='#abcdef'/></svg>)";
  writeFile(path, svg);
  Holonight::HnIconImageProvider provider;
  const auto legacy = providerId(path, 24, Qt::red);
  EXPECT_EQ(provider.requestImage(legacy, nullptr, {}).pixelColor(5, 5), QColor("#123456"));
  for (const auto color : {QColor(Qt::cyan), QColor(Qt::yellow), QColor(0, 255, 255, 128), QColor(Qt::cyan)}) {
    const auto id =
        legacy + "&accent=" + QString::fromLatin1(QUrl::toPercentEncoding(color.name(QColor::HexArgb))) + "&dpr=2";
    const auto image = provider.requestImage(id, nullptr, {48, 48});
    EXPECT_EQ(image.size(), QSize(48, 48));
    EXPECT_NEAR(image.pixelColor(10, 10).alpha(), color.alpha(), 1);
    EXPECT_NEAR(image.pixelColor(10, 10).green(), color.green(), 1);
    EXPECT_EQ(image.pixelColor(35, 10), QColor("#abcdef"));
  }
  EXPECT_EQ(provider.requestImage(legacy + "&semantic=0&accent=%23ffff0000", nullptr, {}).pixelColor(5, 5),
            QColor("#123456"));
}

TEST(HnIconEngine, AllRolesAcrossPaletteStateSizeAndDpr) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
  const auto saved = QGuiApplication::palette();
  struct Restore {
    QPalette palette;
    ~Restore() { QGuiApplication::setPalette(palette); }
  } restore{saved};
  QTemporaryDir dir;
  const QStringList roles{"Text",         "Highlight", "PositiveText", "NeutralText",
                          "NegativeText", "Accent",    "Background",   "HighlightedText"};
  QByteArray svg = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 80 10'><style id='current-color-scheme'>";
  for (const auto& role : roles) svg += ".ColorScheme-" + role.toUtf8() + "{color:#123456}";
  svg += "</style>";
  for (int i = 0; i < roles.size(); ++i)
    svg += "<rect class='ColorScheme-" + roles[i].toUtf8() + "' fill='currentColor' x='" + QByteArray::number(i * 10) +
           "' width='10' height='10'/>";
  svg += "</svg>";
  const auto path = dir.filePath("all-roles.svg");
  writeFile(path, svg);
  Holonight::HnIconEngine engine{path};
  Holonight::HnIconImageProvider provider;
  for (bool light : {false, true}) {
    auto tokens = Holonight::tokensForScheme(light ? Holonight::ThemeSchemeKind::HoloNightLight
                                                   : Holonight::ThemeSchemeKind::HoloNightDark);
    const auto statuses = Holonight::ThemeResolver::resolve(Holonight::AppearanceReader{}.appearance());
    auto palette = Holonight::buildPalette(tokens);
    palette.setColor(QPalette::Active, QPalette::Text, QColor("#eeeeee"));
    palette.setColor(QPalette::Active, QPalette::Accent, QColor("#12bb77"));
    palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#2255bb"));
    QGuiApplication::setPalette(palette);
    const Holonight::IconSemanticColors base{palette.color(QPalette::Active, QPalette::Text),
                                             palette.color(QPalette::Active, QPalette::Highlight),
                                             statuses.success,
                                             statuses.warning,
                                             statuses.error,
                                             palette.color(QPalette::Active, QPalette::Accent),
                                             tokens.background,
                                             tokens.onPrimary};
    auto disabled = base;
    disabled.text = tokens.textDisabled;
    disabled.highlight = palette.color(QPalette::Disabled, QPalette::Highlight);
    disabled.highlightedText = palette.color(QPalette::Disabled, QPalette::HighlightedText);
    disabled.accent = palette.color(QPalette::Disabled, QPalette::Accent);
    disabled.positive = Holonight::blendIconColor(statuses.success, tokens.background, 0.5);
    disabled.neutral = Holonight::blendIconColor(statuses.warning, tokens.background, 0.5);
    disabled.negative = Holonight::blendIconColor(statuses.error, tokens.background, 0.5);
    for (const auto mode : {QIcon::Normal, QIcon::Selected, QIcon::Disabled}) {
      const auto state = mode == QIcon::Selected   ? Holonight::IconState::Selected
                         : mode == QIcon::Disabled ? Holonight::IconState::Disabled
                                                   : Holonight::IconState::Normal;
      const auto colors = Holonight::resolveIconColors(base, disabled, state);
      const QColor expected[]{colors.text,     colors.highlight, colors.positive,   colors.neutral,
                              colors.negative, colors.accent,    colors.background, colors.highlightedText};
      for (int extent : {24, 32})
        for (int dpr : {1, 2}) {
          SCOPED_TRACE(
              QString("light=%1 mode=%2 size=%3 dpr=%4").arg(light).arg(mode).arg(extent).arg(dpr).toStdString());
          const QSize pixels{extent * dpr * 8, extent * dpr};
          const auto image = engine.scaledPixmap({extent * 8, extent}, mode, QIcon::On, dpr).toImage();
          ASSERT_EQ(image.size(), pixels);
          for (int i = 0; i < 8; ++i)
            EXPECT_EQ(image.pixelColor((i * extent + extent / 2) * dpr, extent * dpr / 2),
                      QColor::fromRgba(expected[i].rgba()));
          QUrlQuery query;
          const QStringList keys{"color",    "highlight", "positive",   "neutral",
                                 "negative", "accent",    "background", "highlightedText"};
          for (int i = 0; i < 8; ++i) query.addQueryItem(keys[i], expected[i].name(QColor::HexArgb));
          query.addQueryItem("dpr", QString::number(dpr));
          query.addQueryItem("size", QString::number(extent));
          const auto id = QString::fromLatin1(QUrl::toPercentEncoding(path)) + "?" + query.toString(QUrl::FullyEncoded);
          const auto quick = provider.requestImage(id, nullptr, pixels);
          for (int y = 0; y < pixels.height(); ++y)
            for (int x = 0; x < pixels.width(); ++x) EXPECT_EQ(quick.pixel(x, y), image.pixel(x, y));
        }
    }
  }
#endif
}

TEST(IconRenderer, PaperGlyphPrototypePreservesShadingGeometryAndSeparation) {
  QFile file(QStringLiteral(HOLONIGHT_ICON_FIXTURE_DIR "/application-xml.svg"));
  ASSERT_TRUE(file.open(QIODevice::ReadOnly));
  const auto svg = file.readAll();
  for (bool light : {false, true}) {
    auto colors = semanticColors(QColor(light ? "#202630" : "#e7edf5"));
    colors.accent = QColor("#d52be8");
    colors.highlight = QColor("#167345");
    colors.background = QColor(light ? "#f0f3f7" : "#10151c");
    colors.highlightedText = Qt::white;
    auto disabled = colors;
    disabled.text = Holonight::blendIconColor(colors.text, colors.background, 0.5);
    disabled.accent = Holonight::blendIconColor(colors.accent, colors.background, 0.5);
    for (const auto state :
         {Holonight::IconState::Normal, Holonight::IconState::Selected, Holonight::IconState::Disabled}) {
      const auto resolved = Holonight::resolveIconColors(colors, disabled, state);
      for (int size : {24, 32})
        for (int dpr : {1, 2}) {
          const QSize pixels{size * dpr, size * dpr};
          const auto image = Holonight::IconRenderer::renderSvg(svg, pixels, resolved, true);
          const auto authored = Holonight::IconRenderer::renderSvg(svg, pixels, {});
          ASSERT_FALSE(image.isNull());
          int paper = 0, glyph = 0;
          const auto expected_paper = QColor::fromRgba(resolved.text.rgba());
          const auto expected_glyph = QColor::fromRgba(resolved.accent.rgba());
          for (int y = 0; y < pixels.height(); ++y)
            for (int x = 0; x < pixels.width(); ++x) {
              const auto pixel = image.pixelColor(x, y);
              EXPECT_EQ(pixel.alpha(), authored.pixelColor(x, y).alpha());
              if (pixel == expected_paper) ++paper;
              // At 24 px glyph interiors may include antialiasing against paper.
              const auto distance = [&](const QColor& other) {
                return std::abs(pixel.red() - other.red()) + std::abs(pixel.green() - other.green()) +
                       std::abs(pixel.blue() - other.blue());
              };
              if (pixel.alpha() > 240 && distance(expected_glyph) < distance(expected_paper)) ++glyph;
            }
          EXPECT_GT(paper, 0);
          EXPECT_GT(glyph, 0);
        }
    }
  }
  EXPECT_EQ(Holonight::IconRenderer::applySemanticColors(svg, semanticColors()).count("opacity:0.2;fill:#000000"), 2);
  EXPECT_TRUE(Holonight::IconRenderer::applySemanticColors(svg, semanticColors()).contains("opacity:0.2;fill:#ffffff"));
}

TEST(IconRenderer, ParsedStyleIdAndUntaggedDeclarationsRemainIndependent) {
  const QByteArray svg = R"(<svg xmlns='http://www.w3.org/2000/svg'>
    <!-- <style id='current-color-scheme'>.ColorScheme-Text{color:#010101}</style> -->
    <style data-note='>' id='current-color-&#115;cheme'>
      .ColorScheme-Text{fill:#123456; color : #111111 ; stroke-width:2}
      .ColorScheme-AccentExtra{color:#222222}
    </style>
    <style id='other'>.ColorScheme-Text{color:#333333}</style>
    <style>.ColorScheme-Text{color:#444444}</style>
  </svg>)";
  const auto themed = Holonight::IconRenderer::applySemanticColors(svg, semanticColors());
  EXPECT_EQ(themed, QByteArray(svg).replace("color : #111111", "color : #ff0000"));
  EXPECT_FALSE(Holonight::IconRenderer::hasSemanticRoles("<svg><!-- <path class='ColorScheme-Accent'/> --></svg>"));
}

TEST(IconRenderer, PreservesCssCommentsAndReplacesRepeatedColorDeclarations) {
  const QByteArray svg = R"(<svg><style id='current-color-scheme'><![CDATA[
    /* .ColorScheme-Text{color:#123456} */
    .ColorScheme-Text{color: /* keep */ #111111; fill: #abcdef; color:#222222 !important; opacity:0.7}
  ]]></style></svg>)";
  const auto themed = Holonight::IconRenderer::applySemanticColors(svg, semanticColors());
  EXPECT_EQ(themed, QByteArray(svg).replace("#111111", "#ff0000").replace("#222222", "#ff0000"));
}

TEST(IconRenderer, RetainsGradientsAndCombinesSemanticAlphaWithAuthoredOpacity) {
  const QByteArray svg = R"(<svg xmlns='http://www.w3.org/2000/svg' width='30' height='10'>
    <!-- Decorative gradient and semantic glyph -->
    <defs><linearGradient id='paint'><stop stop-color='#123456'/><stop offset='1' stop-color='#abcdef'/></linearGradient></defs>
    <style id='current-color-scheme'>.ColorScheme-Accent{color:#112233}</style>
    <rect class='ColorScheme-Accent' fill='currentColor' fill-opacity='0.5' width='10' height='10'/>
    <rect x='10' width='10' height='10' fill='url(#paint)'/>
    <rect x='20' width='10' height='10' fill='#ffffff' opacity='0.2'/>
  </svg>)";
  auto colors = semanticColors();
  colors.accent = QColor(0, 255, 255, 128);
  EXPECT_EQ(Holonight::IconRenderer::applySemanticColors(svg, colors),
            QByteArray(svg).replace("color:#112233", "color:#8000ffff"));
  const auto image = Holonight::IconRenderer::renderSvg(svg, {30, 10}, colors, true);
  const auto authored = Holonight::IconRenderer::renderSvg(svg, {30, 10}, {});
  EXPECT_NEAR(image.pixelColor(5, 5).alpha(), 64, 1);
  EXPECT_NEAR(image.pixelColor(5, 5).green(), 255, 1);
  for (int x = 10; x < 30; ++x) EXPECT_EQ(image.pixelColor(x, 5), authored.pixelColor(x, 5));
}
