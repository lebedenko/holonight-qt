// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonightthemeplugin.h"

#include "holonighttheme.h"

QPlatformTheme* HoloniightThemePlugin::create(const QString& key, const QStringList& /*params*/) {
  if (key.toLower() == QLatin1String("holonight")) {
    return new HoloniightTheme;
  }
  return nullptr;
}
