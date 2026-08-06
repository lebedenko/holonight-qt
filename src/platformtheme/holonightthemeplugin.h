// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <qpa/qplatformthemeplugin.h>

class HoloniightThemePlugin : public QPlatformThemePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "qholonight.json")
 public:
  HoloniightThemePlugin() = default;
  Q_DISABLE_COPY_MOVE(HoloniightThemePlugin)

  [[nodiscard]] QPlatformTheme* create(const QString& key, const QStringList& params) override;
};
