// SPDX-License-Identifier: GPL-3.0-or-later
#include "qt5themeplugin.h"

#include "qt5theme.h"
QPlatformTheme* HoloniightQt5ThemePlugin::create(const QString& key, const QStringList&) {
  return key.compare(QStringLiteral("holonight"), Qt::CaseInsensitive) == 0 ? new HoloniightQt5Theme : nullptr;
}
