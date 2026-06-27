// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holoniightpalette.h"

#include "holonight/config.h"

HoloniightPalette::HoloniightPalette(QObject* parent)
    : QObject{parent}, tok_{Holonight::tokensForMode(Holonight::ThemeConfig::load().resolvedColorMode())} {}

void HoloniightPalette::reload() {
  tok_ = Holonight::tokensForMode(Holonight::ThemeConfig::load().resolvedColorMode());
  Q_EMIT paletteChanged();
}
