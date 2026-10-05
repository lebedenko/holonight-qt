// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/appearance.h"
#include "holonight/appearance_reader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <gtest/gtest.h>
#include <holonight/config/config.h>

namespace {

std::filesystem::path path(const QString& value) { return std::filesystem::path{value.toStdString()}; }

void writeInvalid(const QString& file_path) {
  QFile file{file_path};
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
  ASSERT_GT(file.write("not = [valid"), 0);
}

}  // namespace

TEST(ResolvedAppearance, MapsCanonicalDefaultsAndDerivesCatalogValues) {
  const Holonight::AppearanceResolution resolution = Holonight::resolveAppearance(HoloNight::Config::defaults());
  ASSERT_TRUE(resolution);
  EXPECT_TRUE(resolution.diagnostics.isEmpty());
  EXPECT_EQ(resolution.value->scheme, QStringLiteral("holonight-dark"));
  EXPECT_EQ(resolution.value->accent, QStringLiteral("blue"));
  EXPECT_EQ(resolution.value->color_mode, Holonight::ColorMode::Dark);
  EXPECT_EQ(resolution.value->ui_font, QStringLiteral("Inter"));
  EXPECT_EQ(resolution.value->ui_font_size, 12);
  EXPECT_EQ(resolution.value->monospace_font, QStringLiteral("JetBrains Mono"));
  EXPECT_EQ(resolution.value->title_font, QStringLiteral("Audiowide"));
  EXPECT_EQ(resolution.value->title_font_size, 10);
  EXPECT_EQ(resolution.value->display_font, QStringLiteral("Rajdhani"));
  EXPECT_EQ(resolution.value->display_font_size, 24);
  EXPECT_EQ(resolution.value->bodySize(), 12);
  EXPECT_EQ(resolution.value->headingSize(), 18);
}

TEST(ResolvedAppearance, RejectsUnsupportedCatalogSelections) {
  HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  appearance.theme.scheme = "missing";
  Holonight::AppearanceResolution resolution = Holonight::resolveAppearance(appearance);
  ASSERT_FALSE(resolution);
  ASSERT_EQ(resolution.diagnostics.size(), 1);
  EXPECT_EQ(resolution.diagnostics.front().code, Holonight::AppearanceDiagnosticCode::UnsupportedScheme);

  appearance = HoloNight::Config::defaults();
  appearance.theme.accent = "missing";
  resolution = Holonight::resolveAppearance(appearance);
  ASSERT_FALSE(resolution);
  EXPECT_EQ(resolution.diagnostics.front().code, Holonight::AppearanceDiagnosticCode::UnsupportedAccent);
}

TEST(ResolvedAppearance, MapsOptionalShapeSelectionWithoutSentinels) {
  HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  appearance.shape.style = HoloNight::Config::ShapeStyle::Chamfered;
  appearance.shape.scale = 1.5;
  appearance.shape.base_radius = 7.0;
  appearance.shape.base_chamfer = 9.0;
  const Holonight::AppearanceResolution resolution = Holonight::resolveAppearance(appearance);
  ASSERT_TRUE(resolution);
  EXPECT_EQ(resolution.value->shape_style, Holonight::ResolvedShapeStyle::Chamfered);
  EXPECT_EQ(resolution.value->shape_scale, 1.5);
  EXPECT_EQ(resolution.value->base_radius, 7.0);
  EXPECT_EQ(resolution.value->base_chamfer, 9.0);
}

TEST(AppearanceReader, MissingStartupUsesCanonicalDefaults) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  Holonight::AppearanceReader reader{directory.filePath(QStringLiteral("appearance.toml"))};
  EXPECT_EQ(reader.appearance().scheme, QStringLiteral("holonight-dark"));
  EXPECT_EQ(reader.revision(), 0);
}

TEST(AppearanceReader, InvalidStartupReportsDiagnosticAndUsesDefaults) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString file_path = directory.filePath(QStringLiteral("appearance.toml"));
  writeInvalid(file_path);
  Holonight::AppearanceReader reader{file_path};
  EXPECT_EQ(reader.appearance().scheme, QStringLiteral("holonight-dark"));
  ASSERT_FALSE(reader.diagnostics().isEmpty());
  EXPECT_EQ(reader.diagnostics().front().code, Holonight::AppearanceDiagnosticCode::Configuration);
}

TEST(AppearanceReader, PublishesValidReloadAndRetainsItAfterInvalidReplacement) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString file_path = directory.filePath(QStringLiteral("appearance.toml"));
  Holonight::AppearanceReader reader{file_path};
  QSignalSpy appearance_spy{&reader, &Holonight::AppearanceReader::appearanceChanged};
  QSignalSpy palette_spy{&reader, &Holonight::AppearanceReader::paletteChanged};

  HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  appearance.theme.scheme = "holonight-light";
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path(file_path)));
  EXPECT_TRUE(reader.reload());
  EXPECT_EQ(reader.appearance().scheme, QStringLiteral("holonight-light"));
  EXPECT_EQ(reader.revision(), 1);
  EXPECT_EQ(appearance_spy.count(), 1);
  EXPECT_EQ(palette_spy.count(), 1);

  writeInvalid(file_path);
  EXPECT_FALSE(reader.reload());
  EXPECT_EQ(reader.appearance().scheme, QStringLiteral("holonight-light"));
  EXPECT_EQ(reader.revision(), 1);
  EXPECT_EQ(appearance_spy.count(), 1);
}

TEST(AppearanceReader, WatchesLateCreationAndRepeatedAtomicReplacement) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString file_path = directory.filePath(QStringLiteral("nested/appearance.toml"));
  Holonight::AppearanceReader reader{file_path};
  QSignalSpy appearance_spy{&reader, &Holonight::AppearanceReader::appearanceChanged};

  ASSERT_TRUE(QDir{}.mkpath(QFileInfo{file_path}.absolutePath()));
  HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  appearance.theme.scheme = "holonight-light";
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path(file_path)));
  QTRY_COMPARE_WITH_TIMEOUT(reader.appearance().scheme, QStringLiteral("holonight-light"), 2000);

  appearance.theme.scheme = "holonight-storm";
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path(file_path)));
  QTRY_COMPARE_WITH_TIMEOUT(reader.appearance().scheme, QStringLiteral("holonight-storm"), 2000);
  EXPECT_EQ(reader.revision(), 2);
  EXPECT_EQ(appearance_spy.count(), 2);
}

TEST(AppearanceReader, SemanticallyUnchangedReplacementDoesNotPublishRevision) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString file_path = directory.filePath(QStringLiteral("appearance.toml"));
  const HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path(file_path)));
  Holonight::AppearanceReader reader{file_path};
  QSignalSpy appearance_spy{&reader, &Holonight::AppearanceReader::appearanceChanged};

  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path(file_path)));
  QTest::qWait(100);
  EXPECT_EQ(reader.revision(), 0);
  EXPECT_EQ(appearance_spy.count(), 0);
}

TEST(AppearanceReader, SparseV2WarningsArePreservedAndInvalidKnownValuesRetainLastValid) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const auto file_path = directory.filePath(QStringLiteral("appearance.toml"));
  {
    QFile file(file_path);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    ASSERT_GT(file.write("version=2\n[theme]\nscheme='holonight-light'\nunknown=7\n"), 0);
  }
  Holonight::AppearanceReader reader{file_path};
  EXPECT_EQ(reader.appearance().scheme, QStringLiteral("holonight-light"));
  ASSERT_EQ(reader.diagnostics().size(), 1);
  EXPECT_EQ(reader.diagnostics().front().severity, Holonight::AppearanceDiagnosticSeverity::Warning);
  QSignalSpy changes{&reader, &Holonight::AppearanceReader::appearanceChanged};
  {
    QFile file(file_path);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    ASSERT_GT(file.write("version=2\n[typography]\nui_size=1\n"), 0);
  }
  EXPECT_FALSE(reader.reload());
  EXPECT_EQ(reader.appearance().scheme, QStringLiteral("holonight-light"));
  EXPECT_EQ(changes.count(), 0);
  EXPECT_FALSE(reader.diagnostics().isEmpty());
}

TEST(AppearanceReader, V2DeletionAndNestedParentRecreationRecoverWithoutReadTimeWrites) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const auto file_path = directory.filePath(QStringLiteral("nested/appearance.toml"));
  const auto saved = HoloNight::Config::saveAppearanceDocument(
      path(file_path), {
                           {
                               .key = {"theme", "scheme"},
                               .baseline = std::nullopt,
                               .pending = HoloNight::Config::Value{std::string{"holonight-light"}},
                           },
                       });
  ASSERT_EQ(saved.status, HoloNight::Config::SaveStatus::Success);
  Holonight::AppearanceReader reader{file_path};
  QSignalSpy changes{&reader, &Holonight::AppearanceReader::appearanceChanged};
  ASSERT_TRUE(QFile::remove(file_path));
  ASSERT_TRUE(QDir{QFileInfo{file_path}.absolutePath()}.removeRecursively());
  QTRY_COMPARE_WITH_TIMEOUT(reader.appearance().scheme, QStringLiteral("holonight-dark"), 2000);
  EXPECT_FALSE(QFileInfo::exists(file_path));
  EXPECT_TRUE(reader.diagnostics().isEmpty());
  ASSERT_EQ(HoloNight::Config::saveAppearanceDocument(
                path(file_path),
                {{{"theme", "scheme"}, std::nullopt, HoloNight::Config::Value{std::string{"holonight-storm"}}}})
                .status,
            HoloNight::Config::SaveStatus::Success);
  QTRY_COMPARE_WITH_TIMEOUT(reader.appearance().scheme, QStringLiteral("holonight-storm"), 2000);
  EXPECT_EQ(changes.count(), 2);
}

TEST(AppearanceReader, V1ToV2MetadataUpgradeDoesNotEmitEffectiveChange) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const auto file_path = directory.filePath(QStringLiteral("appearance.toml"));
  ASSERT_TRUE(HoloNight::Config::writeAtomically(HoloNight::Config::defaults(), path(file_path)));
  Holonight::AppearanceReader reader{file_path};
  QSignalSpy changes{&reader, &Holonight::AppearanceReader::appearanceChanged};
  ASSERT_EQ(HoloNight::Config::saveAppearanceDocument(path(file_path), {}).status,
            HoloNight::Config::SaveStatus::Success);
  EXPECT_TRUE(reader.reload());
  EXPECT_EQ(reader.revision(), 0);
  EXPECT_EQ(changes.count(), 0);
}
