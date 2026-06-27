// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holoniightpalette.h"

HoloniightPalette::HoloniightPalette(QObject* parent) : QObject{parent}, tok_{Holonight::darkTokens()} {}

void HoloniightPalette::reload() {
  tok_ = Holonight::darkTokens();
  Q_EMIT paletteChanged();
}
