// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "controlpalette.h"

#include "appearancecontext.h"
#include "holonight/appearance_reader.h"
#include "themeresolver.h"

#include <QGuiApplication>
#include <QQmlEngine>

namespace {
QColor blend(const QColor& from, const QColor& to, qreal amount) {
  return QColor::fromRgbF(
      from.redF() * (1 - amount) + to.redF() * amount, from.greenF() * (1 - amount) + to.greenF() * amount,
      from.blueF() * (1 - amount) + to.blueF() * amount, from.alphaF() * (1 - amount) + to.alphaF() * amount);
}
}  // namespace
void ControlPalette::setPalette(QQuickPalette* palette) {
  if (source_ == palette) return;
  if (source_) disconnect(source_, nullptr, this, nullptr);
  source_ = palette;
  if (source_) connect(source_, &QQuickPalette::changed, this, &ControlPalette::refreshPalette);
  refreshPalette();
}
void ControlPalette::setInheritFrom(QQuickPalette* palette) {
  if (inherit_from_ == palette) return;
  if (inherit_from_) disconnect(inherit_from_, nullptr, this, nullptr);
  inherit_from_ = palette;
  if (inherit_from_) connect(inherit_from_, &QQuickPalette::changed, this, &ControlPalette::refreshPalette);
  refreshPalette();
}
void ControlPalette::refreshPalette() {
  if (refreshing_) return;
  refreshing_ = true;
  // ComboBox owns its popup: inherit its palette through Qt's resolve masks,
  // rather than assigning a palette that would replace popup-local roles.
  if (source_ && inherit_from_) source_->inheritPalette(inherit_from_->toQPalette());
  palette_ = source_ ? source_->toQPalette() : QPalette{};
  refreshing_ = false;
  emit changed();
}

void ControlPalette::componentComplete() {
  auto* reader = Holonight::appearanceReaderForEngine(qmlEngine(this));
  tokens_ = Holonight::ThemeResolver::resolve(reader->appearance());
  appearance_palette_.fromQPalette(appearancePalette());
  connect(reader, &Holonight::AppearanceReader::paletteChanged, this, [this, reader] {
    tokens_ = Holonight::ThemeResolver::resolve(reader->appearance());
    appearance_palette_.fromQPalette(appearancePalette());
    emit changed();
  });
  emit changed();
}
bool ControlPalette::defaultDisabled() const {
  const auto defaults = appearancePalette();
  for (const auto role : {static_cast<QPalette::ColorRole>(fill_role_), QPalette::Mid}) {
    if (QGuiApplication::palette().isBrushSet(QPalette::Disabled, role) ||
        palette_.isBrushSet(QPalette::Disabled, role) ||
        palette_.color(QPalette::Disabled, role) != defaults.color(QPalette::Disabled, role))
      return false;
  }
  return true;
}
QVariantMap ControlPalette::colors() const {
  const auto group = static_cast<QPalette::ColorGroup>(group_);
  const auto defaults = appearancePalette();
  const auto text = static_cast<QPalette::ColorRole>(text_role_);
  const auto fill = static_cast<QPalette::ColorRole>(fill_role_);
  auto color = [&](QPalette::ColorRole role) { return palette_.color(group, role); };
  auto matches = [&](std::initializer_list<QPalette::ColorRole> roles) {
    for (auto role : roles)
      if (color(role) != defaults.color(group, role)) return false;
    return true;
  };
  QVariantMap result;
  result.insert(QStringLiteral("link"), color(QPalette::Link));
  auto overlay = [&](qreal opacity) {
    auto shadow = color(QPalette::Shadow);
    shadow.setAlphaF(shadow.alphaF() * opacity);
    return shadow;
  };
  result.insert(QStringLiteral("modalOverlay"), overlay(0.5));
  result.insert(QStringLiteral("modelessOverlay"), overlay(0.12));
  auto direct = [&](const char* name, QPalette::ColorRole role, const QColor& token) {
    result.insert(QLatin1String(name), matches({role}) ? token : color(role));
  };
  auto derived = [&](const char* name, QPalette::ColorRole a, QPalette::ColorRole b, qreal amount,
                     const QColor& token) {
    result.insert(QLatin1String(name), matches({a, b}) ? token : blend(color(a), color(b), amount));
  };
  direct("background", QPalette::Window, tokens_.background);
  direct("surface", QPalette::Base, tokens_.surface);
  direct("surfaceElevated", QPalette::AlternateBase, tokens_.surfaceElevated);
  direct("surfaceRaised", QPalette::Button, tokens_.surfaceRaised);
  direct("buttonPressed", QPalette::Button, tokens_.surface);
  direct("disabledAccent", QPalette::Highlight, tokens_.textDisabled);
  direct("disabledOnPrimary", QPalette::HighlightedText, tokens_.textDisabled);
  direct("disabledBorder", QPalette::Mid, tokens_.textDisabled);
  direct("disabledThumb", QPalette::PlaceholderText, tokens_.textDisabled);
  direct("buttonText", QPalette::ButtonText, tokens_.textPrimary);
  result.insert(QStringLiteral("stepperDisabledText"), palette_.color(QPalette::Disabled, QPalette::ButtonText) ==
                                                               defaults.color(QPalette::Disabled, QPalette::ButtonText)
                                                           ? tokens_.textDisabled
                                                           : palette_.color(QPalette::Disabled, QPalette::ButtonText));
  direct("textPrimary", text, tokens_.textPrimary);
  direct("textDisabled", text, tokens_.textDisabled);
  direct("textMuted", QPalette::PlaceholderText, tokens_.textMuted);
  direct("primary", QPalette::Highlight, tokens_.primary);
  direct("onPrimary", QPalette::HighlightedText, tokens_.onPrimary);
  direct("borderPassive", QPalette::Mid, tokens_.borderPassive);
  direct("borderFocus", QPalette::Highlight, tokens_.borderFocus);
  direct("borderActive", QPalette::Highlight, tokens_.borderActive);
  direct("tooltipBase", QPalette::ToolTipBase, tokens_.surfaceRaised);
  direct("tooltipText", QPalette::ToolTipText, tokens_.textPrimary);
  QColor selection = tokens_.primary;
  selection.setAlphaF(0.3);
  direct("selection", QPalette::Highlight, selection);
  derived("buttonHover", QPalette::Button, QPalette::ButtonText, 0.1, tokens_.surfaceHover);
  derived("surfaceHover", fill, text, 0.1, tokens_.surfaceHover);
  derived("primaryHover", QPalette::Highlight, QPalette::HighlightedText, 0.1, tokens_.primaryHover);
  derived("primaryPressed", QPalette::Highlight, QPalette::Base, 0.2, tokens_.primaryPressed);
  derived("surfaceSelected", QPalette::Base, QPalette::Highlight, 0.12, tokens_.surfaceSelected);
  derived("surfaceSelectedHover", QPalette::Base, QPalette::Highlight, 0.2, tokens_.surfaceSelectedHover);
  derived("borderSubtle", QPalette::Mid, QPalette::Base, 0.5, tokens_.borderSubtle);
  result.insert(QStringLiteral("borderUrgent"), tokens_.borderUrgent);
  return result;
}
