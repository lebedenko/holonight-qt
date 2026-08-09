// SPDX-License-Identifier: GPL-3.0-or-later

#include "holonight/palette.h"
#include "holonight/theme_catalog.h"
#include "holonightstyle.h"
#include "qt5probeappearance.h"
#include "qt5theme.h"
#include "themeresolver.h"

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QPushButton>
#include <QStyleFactory>
#include <QStyleOptionButton>

#include <cmath>
#include <iostream>
#include <set>

namespace {
int failures = 0;
void expect(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}
double luminance(const QColor& color) {
  const auto channel = [](double value) {
    value /= 255.0;
    return value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4);
  };
  return 0.2126 * channel(color.red()) + 0.7152 * channel(color.green()) + 0.0722 * channel(color.blue());
}
double contrast(const QColor& first, const QColor& second) {
  const double light = (std::max)(luminance(first), luminance(second));
  const double dark = (std::min)(luminance(first), luminance(second));
  return (light + 0.05) / (dark + 0.05);
}
}  // namespace

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  std::set<std::string> contrast_failures;
  for (const auto& variant : Holonight::themeVariants()) {
    for (const QString& accent : {QStringLiteral("default"), QStringLiteral("cyan"), QStringLiteral("blue"),
                                  QStringLiteral("violet"), QStringLiteral("yellow")}) {
      qputenv("HOLONIGHT_QT5_SCHEME", variant.id.toUtf8());
      qputenv("HOLONIGHT_QT5_ACCENT", accent.toUtf8());
      const auto appearance = Holonight::qt5ProbeAppearance();
      const auto tokens = Holonight::ThemeResolver::resolve(appearance);
      const auto palette = Holonight::buildPalette(tokens);
      expect(appearance.theme_scheme == variant.scheme, "scheme resolution is deterministic");
      expect(palette.color(QPalette::Window) == tokens.background, "window role maps exactly");
      expect(palette.color(QPalette::Base) == tokens.surface, "base role maps exactly");
      expect(palette.color(QPalette::Highlight) == tokens.primary, "selection maps to resolved accent");
      expect(palette.color(QPalette::HighlightedText) == tokens.onPrimary, "selected text maps exactly");
      expect(palette.color(QPalette::Disabled, QPalette::Text) == tokens.textDisabled, "disabled text maps exactly");
      expect(contrast(tokens.textPrimary, tokens.background) >= 4.5, "primary text contrast passes WCAG AA");
      if (contrast(tokens.onPrimary, tokens.primary) < 3.0)
        contrast_failures.insert(variant.id.toStdString() + '/' + accent.toStdString());
    }
  }
  expect(contrast_failures ==
             std::set<std::string>{"holonight-latte/cyan", "holonight-latte/yellow", "holonight-light/cyan"},
         "the documented accent contrast gate failures remain explicit");

  qunsetenv("HOLONIGHT_QT5_SCHEME");
  qunsetenv("HOLONIGHT_QT5_ACCENT");
  HoloniightStyle style;
  expect(style.standardPalette().color(QPalette::Window) == QColor(QStringLiteral("#0C1118")),
         "style exposes the default semantic palette");
  QPushButton button(QStringLiteral("Probe"));
  button.setStyle(&style);
  button.resize(120, 36);
  QImage image(button.size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  QPainter painter(&image);
  QStyleOptionButton option;
  option.initFrom(&button);
  option.rect = button.rect();
  option.text = button.text();
  option.state |= QStyle::State_HasFocus | QStyle::State_MouseOver;
  style.drawControl(QStyle::CE_PushButton, &option, &painter, &button);
  painter.end();
  expect(!image.isNull() && image.constBits() != nullptr, "representative control renders offscreen");

  expect(QStyleFactory::keys().contains(QStringLiteral("Holonight"), Qt::CaseInsensitive),
         "Qt 5 discovers the build-tree style plugin");
  HoloniightQt5Theme theme;
  expect(theme.themeHint(QPlatformTheme::StyleNames).toStringList().first() == QStringLiteral("Holonight"),
         "platform theme requests the optional style");
  expect(theme.themeHint(QPlatformTheme::SystemIconThemeName).toString() == QStringLiteral("holonight"),
         "platform theme exposes icon hint");
  expect(theme.font(QPlatformTheme::SystemFont)->pointSize() == 12, "platform theme exposes font hint");
  expect(theme.palette()->color(QPalette::Highlight) == QColor(QStringLiteral("#5EA2FF")),
         "platform theme exposes semantic palette");
  return failures == 0 ? 0 : 1;
}
