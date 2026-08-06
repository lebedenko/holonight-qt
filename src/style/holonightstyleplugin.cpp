// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonightstyleplugin.h"

#include "holonightstyle.h"

QStyle* HoloniightStylePlugin::create(const QString& key) {
  if (key.toLower() == QLatin1String("holonight")) {
    return new HoloniightStyle;
  }
  return nullptr;
}
