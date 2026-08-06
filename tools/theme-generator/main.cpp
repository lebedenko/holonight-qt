// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "kate_theme_writer.h"

#include <QDir>
#include <QString>

#include <exception>
#include <holonight/config.h>
#include <holonight/palette.h>
#include <holonight/theme_catalog.h>
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: holonight-theme-generator OUTPUT_DIR\n";
    return 2;
  }

  const QString outputDir = QString::fromLocal8Bit(argv[1]);
  if (!QDir{}.mkpath(outputDir)) {
    std::cerr << "failed to create output directory " << outputDir.toStdString() << '\n';
    return 1;
  }

  try {
    for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
      const Holonight::ThemeSchemeKind scheme = variant.scheme;
      const Holonight::ColorTokens tokens = Holonight::tokensForScheme(scheme);
      const QString schemeId = Holonight::schemeIdForKind(scheme);
      HolonightThemeGen::writeThemeJson(schemeId, tokens,
                                        outputDir + QStringLiteral("/") + schemeId + QStringLiteral(".theme"));
    }
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
