// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holoniightpalette.h"

#include "themeloader.h"
#include "themeresolver.h"

HoloniightPalette::HoloniightPalette(QObject* parent)
    : QObject{parent}, tok_{Holonight::ThemeResolver::resolve(Holonight::ThemeLoader::load())} {}

void HoloniightPalette::reload() {
  const Holonight::ColorTokens newTok = Holonight::ThemeResolver::resolve(Holonight::ThemeLoader::load());
  if (newTok == tok_) {
    return;
  }
  tok_ = newTok;
  Q_EMIT paletteChanged();
}
