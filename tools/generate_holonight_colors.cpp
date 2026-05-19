// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <QColor>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

std::string rgb(const QColor& color) {
  std::ostringstream out;
  out << color.red() << ',' << color.green() << ',' << color.blue();
  return out.str();
}

void writeCommonForegrounds(std::ostringstream& out, const Holonight::ColorTokens& tok) {
  out << "ForegroundNormal=" << rgb(tok.onSurface) << '\n';
  out << "ForegroundInactive=" << rgb(tok.onSurfaceVariant) << '\n';
  out << "ForegroundActive=" << rgb(tok.primary) << '\n';
  out << "ForegroundLink=" << rgb(tok.primary) << '\n';
  out << "ForegroundVisited=" << rgb(tok.error) << '\n';
  out << "ForegroundNegative=" << rgb(tok.error) << '\n';
  out << "ForegroundNeutral=" << rgb(tok.warning) << '\n';
  out << "ForegroundPositive=" << rgb(tok.success) << '\n';
  out << "DecorationFocus=" << rgb(tok.borderFocus) << '\n';
  out << "DecorationHover=" << rgb(tok.primaryHover) << '\n';
}

std::string generatedColors() {
  const auto tok = Holonight::darkTokens();
  std::ostringstream out;

  out << "[ColorScheme]\n";
  out << "Name=Holonight\n";
  out << "ColorSchemeVersion=2\n\n";

  out << "[General]\n";
  out << "ColorScheme=Holonight\n";
  out << "Name=Holonight\n";
  out << "shadeSortColumn=true\n\n";

  out << "[Colors:Window]\n";
  out << "BackgroundNormal=" << rgb(tok.surface) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.surfaceVariant) << '\n';
  writeCommonForegrounds(out, tok);
  out << '\n';

  out << "[Colors:Button]\n";
  out << "BackgroundNormal=" << rgb(tok.surfaceVariant) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.surfaceContainer) << '\n';
  writeCommonForegrounds(out, tok);
  out << '\n';

  out << "[Colors:Selection]\n";
  out << "BackgroundNormal=" << rgb(tok.primary) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.primaryHover) << '\n';
  out << "ForegroundNormal=" << rgb(tok.onPrimary) << '\n';
  out << "ForegroundInactive=" << rgb(tok.onSurface) << '\n';
  out << "ForegroundActive=" << rgb(tok.onPrimary) << '\n';
  out << "ForegroundLink=" << rgb(tok.onPrimary) << '\n';
  out << "ForegroundVisited=" << rgb(tok.error) << '\n';
  out << "ForegroundNegative=" << rgb(tok.error) << '\n';
  out << "ForegroundNeutral=" << rgb(tok.warning) << '\n';
  out << "ForegroundPositive=" << rgb(tok.success) << '\n';
  out << "DecorationFocus=" << rgb(tok.borderFocus) << '\n';
  out << "DecorationHover=" << rgb(tok.primaryHover) << '\n';
  out << '\n';

  out << "[Colors:View]\n";
  out << "BackgroundNormal=" << rgb(tok.surface) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.surfaceVariant) << '\n';
  writeCommonForegrounds(out, tok);
  out << '\n';

  out << "[Colors:Tooltip]\n";
  out << "BackgroundNormal=" << rgb(tok.surfaceInverse) << '\n';
  out << "ForegroundNormal=" << rgb(tok.onSurfaceInverse) << '\n';
  out << '\n';

  out << "[Colors:Complementary]\n";
  out << "BackgroundNormal=" << rgb(tok.surfaceInverse) << '\n';
  out << "ForegroundNormal=" << rgb(tok.onSurfaceInverse) << '\n';
  out << '\n';

  out << "[DecorationColors]\n";
  out << "Focus=" << rgb(tok.borderFocus) << '\n';
  out << "Hover=" << rgb(tok.primaryHover) << '\n';

  return out.str();
}

std::string readFile(const std::filesystem::path& path) {
  std::ifstream in{path};
  if (!in) {
    throw std::runtime_error{"failed to open " + path.string()};
  }
  std::ostringstream data;
  data << in.rdbuf();
  return data.str();
}

void writeFile(const std::filesystem::path& path, std::string_view content) {
  std::ofstream out{path};
  if (!out) {
    throw std::runtime_error{"failed to open " + path.string() + " for writing"};
  }
  out << content;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const std::string content = generatedColors();
    if (argc == 3 && std::string_view{argv[1]} == "--check") {
      const std::filesystem::path path{argv[2]};
      if (readFile(path) != content) {
        std::cerr << path << " is stale; regenerate it with generate_holonight_colors\n";
        return 1;
      }
      return 0;
    }
    if (argc == 2) {
      writeFile(argv[1], content);
      return 0;
    }
    if (argc == 1) {
      std::cout << content;
      return 0;
    }
    std::cerr << "usage: generate_holonight_colors [OUTPUT]\n"
              << "       generate_holonight_colors --check OUTPUT\n";
    return 2;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
