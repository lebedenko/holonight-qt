// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QString>

#include <holonight/palette.h>

namespace HolonightThemeGen {

// Serializes {schemeId, tokens} into a Kate/KSyntaxHighlighting theme JSON file at outputPath.
// schemeId also becomes the theme's internal "metadata.name" field, since
// KSyntaxHighlighting::Repository::theme(name) looks up by that field, not by filename.
void writeThemeJson(const QString& schemeId, const Holonight::ColorTokens& tokens, const QString& outputPath);

}  // namespace HolonightThemeGen
