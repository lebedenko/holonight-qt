// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "appearanceconfig.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QSaveFile>
#include <QStandardPaths>

#include <algorithm>
#include <cmath>

namespace Holonight {
namespace {

constexpr qsizetype kMaximumConfigBytes = 64 * 1024;

CornerStyle parseCornerStyle(const QJsonValue& value) {
  return AppearanceConfig::cornerStyleFromName(value.toString());
}

qreal finiteNumberInRange(const QJsonValue& value, qreal fallback, qreal minimum, qreal maximum) {
  if (!value.isDouble()) {
    return fallback;
  }
  const qreal number = value.toDouble();
  return std::isfinite(number) && number >= minimum && number <= maximum ? number : fallback;
}

bool equalOptional(qreal lhs, qreal rhs) { return (std::isnan(lhs) && std::isnan(rhs)) || lhs == rhs; }

}  // namespace

ShapeTokens AppearanceConfig::shapeTokens() const {
  ShapeTokens tokens = ShapeResolver::defaultTokens();
  tokens.radius_xs *= shape_scale;
  tokens.radius_sm *= shape_scale;
  tokens.radius_md *= shape_scale;
  tokens.radius_lg *= shape_scale;
  tokens.chamfer_xs *= shape_scale;
  tokens.chamfer_sm *= shape_scale;
  tokens.chamfer_md *= shape_scale;
  tokens.chamfer_lg *= shape_scale;

  if (std::isfinite(base_radius)) {
    tokens.radius_xs = tokens.radius_sm = tokens.radius_md = tokens.radius_lg = base_radius;
  }
  if (std::isfinite(base_chamfer)) {
    tokens.chamfer_xs = tokens.chamfer_sm = tokens.chamfer_md = tokens.chamfer_lg = base_chamfer;
  }
  return tokens;
}

CornerStyle AppearanceConfig::effectiveStyle(CornerStyle component_style) const {
  return component_style == CornerStyle::Inherit ? corner_style : component_style;
}

AppearanceConfig AppearanceConfig::defaults() { return {}; }

CornerStyle AppearanceConfig::cornerStyleFromName(QStringView name) {
  const QString style = name.trimmed().toString().toLower();
  if (style == QStringLiteral("hybrid")) {
    return CornerStyle::Hybrid;
  }
  if (style == QStringLiteral("rounded")) {
    return CornerStyle::Rounded;
  }
  if (style == QStringLiteral("chamfered")) {
    return CornerStyle::Chamfered;
  }
  return CornerStyle::Inherit;
}

QString AppearanceConfig::cornerStyleName(CornerStyle style) {
  switch (style) {
    case CornerStyle::Hybrid:
      return QStringLiteral("hybrid");
    case CornerStyle::Rounded:
      return QStringLiteral("rounded");
    case CornerStyle::Chamfered:
      return QStringLiteral("chamfered");
    case CornerStyle::Inherit:
      return QStringLiteral("inherit");
  }
  return QStringLiteral("inherit");
}

AppearanceConfig AppearanceConfig::normalized() const {
  AppearanceConfig result = *this;
  result.corner_style = cornerStyleFromName(cornerStyleName(corner_style));
  if (!std::isfinite(result.shape_scale) || result.shape_scale < minimumShapeScale() ||
      result.shape_scale > maximumShapeScale()) {
    result.shape_scale = defaults().shape_scale;
  }
  if (!std::isfinite(result.base_radius) || result.base_radius < 0.0 || result.base_radius > maximumBaseExtent()) {
    result.base_radius = qQNaN();
  }
  if (!std::isfinite(result.base_chamfer) || result.base_chamfer < 0.0 || result.base_chamfer > maximumBaseExtent()) {
    result.base_chamfer = qQNaN();
  }
  return result;
}

bool AppearanceConfig::save(const QString& path, QString* error_message) const {
  const QFileInfo file_info{path};
  QDir directory = file_info.dir();
  if (!directory.exists() && !directory.mkpath(QStringLiteral("."))) {
    if (error_message != nullptr) {
      *error_message = QStringLiteral("Failed to create directory %1").arg(directory.absolutePath());
    }
    return false;
  }

  const AppearanceConfig config = normalized();
  QJsonObject object{
      {QStringLiteral("version"), 1},
      {QStringLiteral("cornerStyle"), cornerStyleName(config.corner_style)},
      {QStringLiteral("shapeScale"), config.shape_scale},
      {QStringLiteral("baseRadius"),
       std::isfinite(config.base_radius) ? QJsonValue{config.base_radius} : QJsonValue{QJsonValue::Null}},
      {QStringLiteral("baseChamfer"),
       std::isfinite(config.base_chamfer) ? QJsonValue{config.base_chamfer} : QJsonValue{QJsonValue::Null}},
  };

  QSaveFile file{path};
  if (!file.open(QIODevice::WriteOnly)) {
    if (error_message != nullptr) {
      *error_message = file.errorString();
    }
    return false;
  }
  const QByteArray contents = QJsonDocument{object}.toJson(QJsonDocument::Indented);
  if (file.write(contents) != contents.size() || !file.commit()) {
    if (error_message != nullptr) {
      *error_message = file.errorString();
    }
    return false;
  }
  return true;
}

AppearanceConfig AppearanceConfig::load(const QString& path) {
  QFile file = QFile{path};
  if (!file.open(QIODevice::ReadOnly)) {
    if (QFileInfo::exists(path)) {
      qWarning() << "Failed to open appearance configuration" << path << file.errorString();
    }
    return defaults();
  }

  const QByteArray bytes = file.read(kMaximumConfigBytes + 1);
  if (file.error() != QFileDevice::FileError::NoError) {
    qWarning() << "Failed to read appearance configuration" << path << file.errorString();
    return defaults();
  }
  if (bytes.size() > kMaximumConfigBytes) {
    qWarning() << "Appearance configuration exceeds" << kMaximumConfigBytes << "bytes:" << path;
    return defaults();
  }

  QJsonParseError error;
  const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
  if (error.error != QJsonParseError::NoError || !document.isObject()) {
    qWarning() << "Invalid appearance configuration" << path << error.errorString();
    return defaults();
  }

  const QJsonObject object = document.object();
  if (object.contains(QStringLiteral("version")) && object.value(QStringLiteral("version")).toInt(-1) != 1) {
    qWarning() << "Unsupported appearance configuration version in" << path;
    return defaults();
  }

  AppearanceConfig config;
  config.corner_style = parseCornerStyle(object.value(QStringLiteral("cornerStyle")));
  config.shape_scale =
      finiteNumberInRange(object.value(QStringLiteral("shapeScale")), 1.0, minimumShapeScale(), maximumShapeScale());
  config.base_radius =
      finiteNumberInRange(object.value(QStringLiteral("baseRadius")), qQNaN(), 0.0, maximumBaseExtent());
  config.base_chamfer =
      finiteNumberInRange(object.value(QStringLiteral("baseChamfer")), qQNaN(), 0.0, maximumBaseExtent());
  return config;
}

QString AppearanceConfig::configFilePath() {
  const QString override_path = qEnvironmentVariable("HOLONIGHT_APPEARANCE_FILE").trimmed();
  if (!override_path.isEmpty()) {
    return override_path;
  }
  return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
         QStringLiteral("/holonight/appearance.json");
}

bool operator==(const AppearanceConfig& lhs, const AppearanceConfig& rhs) {
  return lhs.corner_style == rhs.corner_style && lhs.shape_scale == rhs.shape_scale &&
         equalOptional(lhs.base_radius, rhs.base_radius) && equalOptional(lhs.base_chamfer, rhs.base_chamfer);
}

}  // namespace Holonight
