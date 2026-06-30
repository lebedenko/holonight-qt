// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holoniightpalette.h"

#include "holonight/config.h"
#include "themeloader.h"
#include "themeresolver.h"

#include <QFileInfo>

HoloniightPalette::HoloniightPalette(QObject* parent)
    : QObject{parent},
      tok_{Holonight::ThemeResolver::resolve(Holonight::ThemeLoader::load())},
      theme_config_path_{Holonight::ThemeConfig::configFilePath()},
      theme_config_dir_path_{QFileInfo{theme_config_path_}.absolutePath()} {
  connect(&theme_config_watcher_, &QFileSystemWatcher::fileChanged, this,
          &HoloniightPalette::onThemeConfigPathChanged);
  connect(&theme_config_watcher_, &QFileSystemWatcher::directoryChanged, this,
          &HoloniightPalette::onThemeConfigPathChanged);
  armThemeConfigWatch();
}

void HoloniightPalette::reload() {
  const Holonight::ColorTokens newTok = Holonight::ThemeResolver::resolve(Holonight::ThemeLoader::load());
  if (newTok == tok_) {
    return;
  }
  tok_ = newTok;
  Q_EMIT paletteChanged();
}

void HoloniightPalette::armThemeConfigWatch() {
  if (QFileInfo::exists(theme_config_dir_path_) &&
      !theme_config_watcher_.directories().contains(theme_config_dir_path_)) {
    theme_config_watcher_.addPath(theme_config_dir_path_);
  }
  if (QFileInfo::exists(theme_config_path_) && !theme_config_watcher_.files().contains(theme_config_path_)) {
    theme_config_watcher_.addPath(theme_config_path_);
  }
}

void HoloniightPalette::onThemeConfigPathChanged(const QString& path) {
  if (path != theme_config_path_ && path != theme_config_dir_path_) {
    return;
  }
  armThemeConfigWatch();
  reload();
}
