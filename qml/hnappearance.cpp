// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnappearance.h"

#include "hnshapetypes.h"

#include <QFileInfo>
#include <QSizeF>

namespace {

Holonight::SurfaceRole surfaceRole(int role) {
  if (role < HnSurfaceRole::Window || role > HnSurfaceRole::WorkspaceIndicator) {
    return Holonight::SurfaceRole::Card;
  }
  return static_cast<Holonight::SurfaceRole>(role);
}

Holonight::CornerStyle parsedCornerStyle(int style) {
  if (style < HnCornerStyle::Inherit || style > HnCornerStyle::Chamfered) {
    return Holonight::CornerStyle::Inherit;
  }
  return static_cast<Holonight::CornerStyle>(style);
}

}  // namespace

HnAppearance::HnAppearance(QObject* parent)
    : QObject{parent},
      config_{Holonight::AppearanceConfig::load()},
      config_file_{Holonight::AppearanceConfig::configFilePath()},
      config_directory_{QFileInfo{config_file_}.absolutePath()},
      config_parent_directory_{QFileInfo{config_directory_}.absolutePath()} {
  connect(&watcher_, &QFileSystemWatcher::fileChanged, this, &HnAppearance::onPathChanged);
  connect(&watcher_, &QFileSystemWatcher::directoryChanged, this, &HnAppearance::onPathChanged);
  armWatch();
}

void HnAppearance::reload() {
  const Holonight::AppearanceConfig new_config = Holonight::AppearanceConfig::load(config_file_);
  if (new_config == config_) {
    return;
  }
  config_ = new_config;
  ++revision_;
  Q_EMIT appearanceChanged();
}

qreal HnAppearance::roundedRadius(int role, qreal width, qreal height, int revision_dependency) const {
  Q_UNUSED(revision_dependency)
  return Holonight::ShapeResolver::resolve(surfaceRole(role), Holonight::CornerStyle::Rounded, QSizeF{width, height},
                                           qQNaN(), qQNaN(), config_.shapeTokens())
      .radius;
}

QVariantMap HnAppearance::resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                  qreal chamfer_override, int revision_dependency) const {
  return resolve(role, style, width, height, radius_override, chamfer_override, HnCornerMask::Inherit,
                 revision_dependency);
}

QVariantMap HnAppearance::resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                  qreal chamfer_override, int chamfered_corners_override,
                                  int revision_dependency) const {
  Q_UNUSED(revision_dependency)
  const Holonight::ResolvedShape shape = Holonight::ShapeResolver::resolve(
      surfaceRole(role), config_.effectiveStyle(parsedCornerStyle(style)), QSizeF{width, height}, radius_override,
      chamfer_override, chamfered_corners_override, config_.shapeTokens());
  return {
      {QStringLiteral("kind"), static_cast<int>(shape.kind)},
      {QStringLiteral("radius"), shape.radius},
      {QStringLiteral("chamfer"), shape.chamfer},
      {QStringLiteral("corners"), shape.chamfered_corners.toInt()},
  };
}

void HnAppearance::armWatch() {
  if (QFileInfo::exists(config_parent_directory_) && !watcher_.directories().contains(config_parent_directory_)) {
    watcher_.addPath(config_parent_directory_);
  }
  if (QFileInfo::exists(config_directory_) && !watcher_.directories().contains(config_directory_)) {
    watcher_.addPath(config_directory_);
  }
  if (QFileInfo::exists(config_file_) && !watcher_.files().contains(config_file_)) {
    watcher_.addPath(config_file_);
  }
}

void HnAppearance::onPathChanged(const QString& path) {
  if (path != config_file_ && path != config_directory_ && path != config_parent_directory_) {
    return;
  }
  armWatch();
  reload();
}
