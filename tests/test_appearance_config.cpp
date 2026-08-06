// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "appearanceconfig.h"

#include <QFile>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTest>

#include <cmath>
#include <gtest/gtest.h>

namespace {

void writeFile(const QString& path, const QByteArray& contents) {
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  ASSERT_TRUE(opened);
  ASSERT_EQ(file.write(contents), contents.size());
}

}  // namespace

TEST(AppearanceConfig, MissingFileUsesSemanticDefaults) {
  QTest::failOnWarning(QRegularExpression{QStringLiteral("Failed to open appearance configuration.*")});
  const Holonight::AppearanceConfig config = Holonight::AppearanceConfig::load(QStringLiteral("/missing/file.json"));
  EXPECT_EQ(config.corner_style, Holonight::CornerStyle::Inherit);
  EXPECT_DOUBLE_EQ(config.shape_scale, 1.0);
  EXPECT_TRUE(std::isnan(config.base_radius));
  EXPECT_TRUE(std::isnan(config.base_chamfer));
}

TEST(AppearanceConfig, LoadsValidShapePreferences) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.json"));
  writeFile(path, R"({
    "version": 1,
    "cornerStyle": "chamfered",
    "shapeScale": 1.5,
    "baseRadius": 9,
    "baseChamfer": 13
  })");

  const Holonight::AppearanceConfig config = Holonight::AppearanceConfig::load(path);
  EXPECT_EQ(config.corner_style, Holonight::CornerStyle::Chamfered);
  EXPECT_DOUBLE_EQ(config.shape_scale, 1.5);
  EXPECT_DOUBLE_EQ(config.base_radius, 9.0);
  EXPECT_DOUBLE_EQ(config.base_chamfer, 13.0);
  EXPECT_DOUBLE_EQ(config.shapeTokens().radius_lg, 9.0);
  EXPECT_DOUBLE_EQ(config.shapeTokens().chamfer_xs, 13.0);
}

TEST(AppearanceConfig, SavesAtomicVersionOneConfiguration) {
  QTemporaryDir temp_dir;
  ASSERT_TRUE(temp_dir.isValid());
  const QString path = temp_dir.filePath(QStringLiteral("nested/appearance.json"));

  Holonight::AppearanceConfig config;
  config.corner_style = Holonight::CornerStyle::Chamfered;
  config.shape_scale = 1.5;
  config.base_radius = 0.0;
  config.base_chamfer = qQNaN();

  QString error;
  ASSERT_TRUE(config.save(path, &error)) << error.toStdString();
  EXPECT_EQ(Holonight::AppearanceConfig::load(path), config);
}

TEST(AppearanceConfig, SaveReportsUnwritableTarget) {
  QTemporaryDir temp_dir;
  ASSERT_TRUE(temp_dir.isValid());
  const QString path = temp_dir.filePath(QStringLiteral("directory"));
  ASSERT_TRUE(QDir().mkpath(path));

  QString error;
  EXPECT_FALSE(Holonight::AppearanceConfig{}.save(path, &error));
  EXPECT_FALSE(error.isEmpty());
}

TEST(AppearanceConfig, InvalidFieldsFallBackIndependently) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.json"));
  writeFile(path, R"({
    "version": 1,
    "cornerStyle": "rounded",
    "shapeScale": 100,
    "baseRadius": -1,
    "baseChamfer": "large"
  })");

  const Holonight::AppearanceConfig config = Holonight::AppearanceConfig::load(path);
  EXPECT_EQ(config.corner_style, Holonight::CornerStyle::Rounded);
  EXPECT_DOUBLE_EQ(config.shape_scale, 1.0);
  EXPECT_TRUE(std::isnan(config.base_radius));
  EXPECT_TRUE(std::isnan(config.base_chamfer));
}

TEST(AppearanceConfig, OversizedFileUsesDefaults) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.json"));
  writeFile(path, QByteArray(64 * 1024 + 1, ' '));

  const Holonight::AppearanceConfig config = Holonight::AppearanceConfig::load(path);
  EXPECT_EQ(config.corner_style, Holonight::CornerStyle::Inherit);
  EXPECT_DOUBLE_EQ(config.shape_scale, 1.0);
}

TEST(AppearanceConfig, ScalePreservesTokenHierarchy) {
  Holonight::AppearanceConfig config;
  config.shape_scale = 2.0;
  const Holonight::ShapeTokens tokens = config.shapeTokens();
  EXPECT_DOUBLE_EQ(tokens.radius_xs, 8.0);
  EXPECT_DOUBLE_EQ(tokens.radius_lg, 24.0);
  EXPECT_DOUBLE_EQ(tokens.chamfer_sm, 12.0);
  EXPECT_DOUBLE_EQ(tokens.chamfer_lg, 32.0);
}

TEST(AppearanceConfig, ComponentStyleOverridesGlobalStyle) {
  Holonight::AppearanceConfig config;
  config.corner_style = Holonight::CornerStyle::Chamfered;
  EXPECT_EQ(config.effectiveStyle(Holonight::CornerStyle::Inherit), Holonight::CornerStyle::Chamfered);
  EXPECT_EQ(config.effectiveStyle(Holonight::CornerStyle::Rounded), Holonight::CornerStyle::Rounded);
}
