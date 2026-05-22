// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

HolonightTheme::HolonightTheme(QObject* parent) : QObject{parent}, config_{Holonight::ThemeConfig::load()} {}
