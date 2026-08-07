// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/appearance_reader.h"

#include <holonight/config/config.h>

#include <QDebug>
#include <QFileInfo>

namespace Holonight {
namespace {

[[nodiscard]] QString canonicalConfigPath() {
  const HoloNight::Config::Result<std::filesystem::path> result = HoloNight::Config::resolveAppearancePath();
  return result ? QString::fromStdString(result.value->string()) : QString{};
}

[[nodiscard]] QVector<AppearanceDiagnostic> qtDiagnostics(const std::vector<HoloNight::Config::Diagnostic>& source) {
  QVector<AppearanceDiagnostic> result;
  result.reserve(static_cast<qsizetype>(source.size()));
  for (const HoloNight::Config::Diagnostic& diagnostic : source) {
    result.append(qtDiagnostic(diagnostic));
  }
  return result;
}

[[nodiscard]] bool paletteDiffers(const ResolvedAppearance& lhs, const ResolvedAppearance& rhs) {
  return lhs.scheme != rhs.scheme || lhs.accent != rhs.accent;
}

[[nodiscard]] bool themeDiffers(const ResolvedAppearance& lhs, const ResolvedAppearance& rhs) {
  return lhs.scheme != rhs.scheme || lhs.accent != rhs.accent || lhs.ui_font != rhs.ui_font ||
         lhs.ui_font_size != rhs.ui_font_size || lhs.monospace_font != rhs.monospace_font ||
         lhs.monospace_font_size != rhs.monospace_font_size || lhs.title_font != rhs.title_font ||
         lhs.title_font_size != rhs.title_font_size || lhs.display_font != rhs.display_font ||
         lhs.display_font_size != rhs.display_font_size || lhs.icon_theme != rhs.icon_theme ||
         lhs.fallback_icon_theme != rhs.fallback_icon_theme || lhs.cursor_theme != rhs.cursor_theme ||
         lhs.layout_scale != rhs.layout_scale;
}

[[nodiscard]] bool shapeDiffers(const ResolvedAppearance& lhs, const ResolvedAppearance& rhs) {
  return lhs.shape_style != rhs.shape_style || lhs.shape_scale != rhs.shape_scale ||
         lhs.base_radius != rhs.base_radius || lhs.base_chamfer != rhs.base_chamfer;
}

}  // namespace

AppearanceReader::AppearanceReader(QObject* parent) : AppearanceReader{canonicalConfigPath(), parent} {}

AppearanceReader::AppearanceReader(QString config_file, QObject* parent)
    : QObject{parent}, config_file_{std::move(config_file)} {
  reload_timer_.setSingleShot(true);
  reload_timer_.setInterval(0);
  connect(&reload_timer_, &QTimer::timeout, this, &AppearanceReader::reload);
  connect(&watcher_, &QFileSystemWatcher::fileChanged, this, &AppearanceReader::scheduleReload);
  connect(&watcher_, &QFileSystemWatcher::directoryChanged, this, &AppearanceReader::scheduleReload);
  initialize();
}

void AppearanceReader::initialize() {
  const HoloNight::Config::Result<HoloNight::Config::LoadedAppearance> loaded =
      HoloNight::Config::load(std::filesystem::path{config_file_.toStdString()});
  QVector<AppearanceDiagnostic> diagnostics = qtDiagnostics(loaded.diagnostics);
  HoloNight::Config::Appearance candidate = HoloNight::Config::defaults();
  if (loaded) {
    candidate = loaded.value->appearance;
  }
  AppearanceResolution resolution = resolveAppearance(candidate);
  diagnostics.append(resolution.diagnostics);
  if (resolution) {
    appearance_ = std::move(*resolution.value);
  }
  publishDiagnostics(std::move(diagnostics));
  rearmWatcher();
}

bool AppearanceReader::reload() {
  const HoloNight::Config::Result<HoloNight::Config::LoadedAppearance> loaded =
      HoloNight::Config::load(std::filesystem::path{config_file_.toStdString()});
  rearmWatcher();
  if (!loaded) {
    publishDiagnostics(qtDiagnostics(loaded.diagnostics));
    return false;
  }

  AppearanceResolution resolution = resolveAppearance(loaded.value->appearance);
  if (!resolution) {
    publishDiagnostics(std::move(resolution.diagnostics));
    return false;
  }

  publishDiagnostics(qtDiagnostics(loaded.diagnostics));
  const ResolvedAppearance candidate = std::move(*resolution.value);
  if (candidate == appearance_) {
    return true;
  }

  const bool palette_changed = paletteDiffers(candidate, appearance_);
  const bool theme_changed = themeDiffers(candidate, appearance_);
  const bool shape_changed = shapeDiffers(candidate, appearance_);
  appearance_ = candidate;
  ++revision_;
  Q_EMIT appearanceChanged();
  if (palette_changed) {
    Q_EMIT paletteChanged();
  }
  if (theme_changed) {
    Q_EMIT themeChanged();
  }
  if (shape_changed) {
    Q_EMIT shapeChanged();
  }
  return true;
}

void AppearanceReader::scheduleReload() {
  rearmWatcher();
  reload_timer_.start();
}

void AppearanceReader::rearmWatcher() {
  const QStringList watched = watcher_.files() + watcher_.directories();
  if (!watched.isEmpty()) {
    watcher_.removePaths(watched);
  }

  QString directory = QFileInfo{config_file_}.absolutePath();
  while (!QFileInfo{directory}.isDir()) {
    const QString parent = QFileInfo{directory}.absolutePath();
    if (parent == directory) {
      break;
    }
    directory = parent;
  }
  if (QFileInfo{directory}.isDir()) {
    watcher_.addPath(directory);
  }
  if (QFileInfo{config_file_}.isFile()) {
    watcher_.addPath(config_file_);
  }
}

void AppearanceReader::publishDiagnostics(QVector<AppearanceDiagnostic> diagnostics) {
  if (diagnostics == diagnostics_) {
    return;
  }
  diagnostics_ = std::move(diagnostics);
  for (const AppearanceDiagnostic& diagnostic : diagnostics_) {
    if (diagnostic.severity == AppearanceDiagnosticSeverity::Error) {
      qWarning().noquote() << diagnostic.message;
    }
  }
  Q_EMIT diagnosticsChanged();
}

}  // namespace Holonight
