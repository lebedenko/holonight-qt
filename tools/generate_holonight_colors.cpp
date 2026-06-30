// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <QColor>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

std::string rgb(const QColor& color) {
  std::ostringstream out;
  out << color.red() << ',' << color.green() << ',' << color.blue();
  return out.str();
}

void writeCommonForegrounds(std::ostringstream& out, const Holonight::ColorTokens& tok) {
  out << "ForegroundNormal=" << rgb(tok.textPrimary) << '\n';
  out << "ForegroundInactive=" << rgb(tok.textMuted) << '\n';
  out << "ForegroundActive=" << rgb(tok.primary) << '\n';
  out << "ForegroundLink=" << rgb(tok.primary) << '\n';
  out << "ForegroundVisited=" << rgb(tok.error) << '\n';
  out << "ForegroundNegative=" << rgb(tok.error) << '\n';
  out << "ForegroundNeutral=" << rgb(tok.warning) << '\n';
  out << "ForegroundPositive=" << rgb(tok.success) << '\n';
  out << "DecorationFocus=" << rgb(tok.borderFocus) << '\n';
  out << "DecorationHover=" << rgb(tok.primaryHover) << '\n';
}

Holonight::ThemeSchemeKind parseScheme(std::string_view value) {
  if (value == "holonight-dark") {
    return Holonight::ThemeSchemeKind::HoloNightDark;
  }
  if (value == "holonight-light") {
    return Holonight::ThemeSchemeKind::HoloNightLight;
  }
  if (value == "holonight-mocha") {
    return Holonight::ThemeSchemeKind::HoloNightMocha;
  }
  if (value == "holonight-latte") {
    return Holonight::ThemeSchemeKind::HoloNightLatte;
  }
  if (value == "tokyonight-storm") {
    return Holonight::ThemeSchemeKind::TokyoNightStorm;
  }
  if (value == "tokyonight-day") {
    return Holonight::ThemeSchemeKind::TokyoNightDay;
  }
  if (value == "holonight-ember") {
    return Holonight::ThemeSchemeKind::HoloNightEmber;
  }
  if (value == "holonight-sol") {
    return Holonight::ThemeSchemeKind::HoloNightSol;
  }
  throw std::runtime_error{
      "invalid scheme '" + std::string{value} +
      "'; expected holonight-dark, holonight-light, holonight-mocha, holonight-latte, tokyonight-storm, "
      "tokyonight-day, holonight-ember, or holonight-sol"};
}

std::string schemeName(Holonight::ThemeSchemeKind scheme) {
  switch (scheme) {
    case Holonight::ThemeSchemeKind::HoloNightDark:
      return "HoloNight Dark";
    case Holonight::ThemeSchemeKind::HoloNightLight:
      return "HoloNight Light";
    case Holonight::ThemeSchemeKind::HoloNightMocha:
      return "HoloNight Mocha";
    case Holonight::ThemeSchemeKind::HoloNightLatte:
      return "HoloNight Latte";
    case Holonight::ThemeSchemeKind::TokyoNightStorm:
      return "TokyoNight Storm";
    case Holonight::ThemeSchemeKind::TokyoNightDay:
      return "TokyoNight Day";
    case Holonight::ThemeSchemeKind::HoloNightEmber:
      return "HoloNight Ember";
    case Holonight::ThemeSchemeKind::HoloNightSol:
      return "HoloNight Sol";
  }
  return "HoloNight Dark";
}

std::string schemeId(Holonight::ThemeSchemeKind scheme) {
  switch (scheme) {
    case Holonight::ThemeSchemeKind::HoloNightDark:
      return "holonight-dark";
    case Holonight::ThemeSchemeKind::HoloNightLight:
      return "holonight-light";
    case Holonight::ThemeSchemeKind::HoloNightMocha:
      return "holonight-mocha";
    case Holonight::ThemeSchemeKind::HoloNightLatte:
      return "holonight-latte";
    case Holonight::ThemeSchemeKind::TokyoNightStorm:
      return "tokyonight-storm";
    case Holonight::ThemeSchemeKind::TokyoNightDay:
      return "tokyonight-day";
    case Holonight::ThemeSchemeKind::HoloNightEmber:
      return "holonight-ember";
    case Holonight::ThemeSchemeKind::HoloNightSol:
      return "holonight-sol";
  }
  return "holonight-dark";
}

std::string generatedColors(Holonight::ThemeSchemeKind scheme) {
  const auto tok = Holonight::tokensForScheme(scheme);
  const std::string name = schemeName(scheme);
  const std::string id = schemeId(scheme);
  std::ostringstream out;

  out << "[ColorScheme]\n";
  out << "Name=" << name << '\n';
  out << "ColorSchemeVersion=2\n\n";

  out << "[General]\n";
  out << "ColorScheme=" << name << '\n';
  out << "Name=" << name << '\n';
  out << "X-HoloNight-Scheme=" << id << '\n';
  out << "shadeSortColumn=true\n\n";

  out << "[Colors:Window]\n";
  out << "BackgroundNormal=" << rgb(tok.background) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.surface) << '\n';
  writeCommonForegrounds(out, tok);
  out << '\n';

  out << "[Colors:Button]\n";
  out << "BackgroundNormal=" << rgb(tok.surfaceRaised) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.surfaceElevated) << '\n';
  writeCommonForegrounds(out, tok);
  out << '\n';

  out << "[Colors:Selection]\n";
  out << "BackgroundNormal=" << rgb(tok.primary) << '\n';
  out << "BackgroundAlternate=" << rgb(tok.primaryHover) << '\n';
  out << "ForegroundNormal=" << rgb(tok.onPrimary) << '\n';
  out << "ForegroundInactive=" << rgb(tok.textPrimary) << '\n';
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
  out << "BackgroundAlternate=" << rgb(tok.surfaceElevated) << '\n';
  writeCommonForegrounds(out, tok);
  out << '\n';

  out << "[Colors:Tooltip]\n";
  out << "BackgroundNormal=" << rgb(tok.surfaceInverse) << '\n';
  out << "ForegroundNormal=" << rgb(tok.textInverse) << '\n';
  out << '\n';

  out << "[Colors:Complementary]\n";
  out << "BackgroundNormal=" << rgb(tok.surfaceInverse) << '\n';
  out << "ForegroundNormal=" << rgb(tok.textInverse) << '\n';
  out << '\n';

  out << "[DecorationColors]\n";
  out << "Focus=" << rgb(tok.borderFocus) << '\n';
  out << "Hover=" << rgb(tok.primaryHover) << '\n';

  return out.str();
}

std::string readFile(const std::filesystem::path& path) {
  std::ifstream in = std::ifstream{path};
  if (!in) {
    throw std::runtime_error{"failed to open " + path.string()};
  }
  std::ostringstream data;
  data << in.rdbuf();
  if (in.bad() || !data) {
    throw std::runtime_error{"failed to read " + path.string()};
  }
  return data.str();
}

void writeFile(const std::filesystem::path& path, std::string_view content) {
  std::ofstream out = std::ofstream{path};
  if (!out) {
    throw std::runtime_error{"failed to open " + path.string() + " for writing"};
  }
  out << content;
  out.flush();
  if (!out) {
    throw std::runtime_error{"failed to write " + path.string()};
  }
}

struct Arguments {
  Holonight::ThemeSchemeKind scheme = Holonight::ThemeSchemeKind::HoloNightDark;
  bool check = false;
  std::filesystem::path output;
};

Arguments parseArguments(int argc, char** argv) {
  Arguments arguments;
  for (int index = 1; index < argc; ++index) {
    const std::string_view arg{argv[index]};
    if (arg == "--scheme") {
      if (index + 1 >= argc) {
        throw std::runtime_error{"--scheme requires a scheme ID"};
      }
      arguments.scheme = parseScheme(argv[++index]);
    } else if (arg == "--mode") {
      if (index + 1 >= argc) {
        throw std::runtime_error{"--mode requires dark or light"};
      }
      const std::string_view mode = argv[++index];
      if (mode == "dark") {
        arguments.scheme = Holonight::ThemeSchemeKind::TokyoNightStorm;
      } else if (mode == "light") {
        arguments.scheme = Holonight::ThemeSchemeKind::TokyoNightDay;
      } else {
        throw std::runtime_error{"invalid mode '" + std::string{mode} + "'; expected dark or light"};
      }
    } else if (arg == "--check") {
      arguments.check = true;
    } else if (arguments.output.empty()) {
      arguments.output = argv[index];
    } else {
      throw std::runtime_error{"unexpected argument '" + std::string{arg} + "'"};
    }
  }
  return arguments;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Arguments arguments = parseArguments(argc, argv);
    const std::string content = generatedColors(arguments.scheme);
    if (arguments.check) {
      if (arguments.output.empty()) {
        throw std::runtime_error{"--check requires OUTPUT"};
      }
      if (readFile(arguments.output) != content) {
        std::cerr << arguments.output << " is stale; regenerate it with generate_holonight_colors --scheme "
                  << schemeId(arguments.scheme) << '\n';
        return 1;
      }
      return 0;
    }
    if (!arguments.output.empty()) {
      writeFile(arguments.output, content);
      return 0;
    }
    std::cout << content;
    return 0;
  } catch (const std::exception& error) {
    std::cerr << error.what() << "\n"
              << "usage: generate_holonight_colors [--scheme SCHEME_ID] [OUTPUT]\n"
              << "       generate_holonight_colors [--scheme SCHEME_ID] --check OUTPUT\n"
              << "       generate_holonight_colors [--mode dark|light] [OUTPUT]\n";
    return 2;
  }
}
