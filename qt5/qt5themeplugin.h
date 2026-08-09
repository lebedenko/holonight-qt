// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <qpa/qplatformthemeplugin.h>
class HoloniightQt5ThemePlugin : public QPlatformThemePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "qt5theme.json")
 public:
  [[nodiscard]] QPlatformTheme* create(const QString& key, const QStringList& params) override;
};
