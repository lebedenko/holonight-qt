// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QColor>

#include <KSyntaxHighlighting/Theme>
#include <holonight/palette.h>

namespace HolonightThemeGen {

// The ONE place TextStyle -> ColorTokens correspondence is defined. Called once per (style,
// scheme) pair for each of the 10 schemes -- never duplicated per-scheme.
[[nodiscard]] QColor colorForTextStyle(KSyntaxHighlighting::Theme::TextStyle style,
                                       const Holonight::ColorTokens& tokens);

}  // namespace HolonightThemeGen
