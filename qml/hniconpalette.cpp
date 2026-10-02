// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#include "hniconpalette.h"

#include "appearancecontext.h"
#include "holonight/appearance_reader.h"
#include "themeresolver.h"

#include <QEvent>
#include <QGuiApplication>
#include <QQmlEngine>

HnIconPalette::HnIconPalette(QObject* parent) : QObject(parent) {}
void HnIconPalette::setPalette(QQuickPalette* palette) {
  if (palette_ == palette) {
    return;
  }
  if (palette_) {
    disconnect(palette_, nullptr, this, nullptr);
  }
  palette_ = palette;
  if (palette_) {
    connect(palette_, &QQuickPalette::changed, this, &HnIconPalette::changed);
    connect(palette_, &QObject::destroyed, this, &HnIconPalette::changed);
  }
  emit changed();
}
void HnIconPalette::componentComplete() {
  reader_ = Holonight::appearanceReaderForEngine(qmlEngine(this));
  connect(reader_, &Holonight::AppearanceReader::paletteChanged, this, &HnIconPalette::changed);
  qGuiApp->installEventFilter(this);
  emit changed();
}
bool HnIconPalette::eventFilter(QObject* watched, QEvent* event) {
  if (watched == qGuiApp && event->type() == QEvent::ApplicationPaletteChange) {
    emit changed();
  }
  return QObject::eventFilter(watched, event);
}
QVariantMap HnIconPalette::colors() const {
  if (!reader_) {
    return {};
  }
  const auto tokens = Holonight::ThemeResolver::resolve(reader_->appearance());
  const auto defaults = Holonight::buildPalette(tokens);
  auto palette = QGuiApplication::palette().resolve(defaults);
  if (palette_) {
    palette = palette_->toQPalette().resolve(palette);
  }
  QVariantMap result;
  for (const auto group : {QPalette::Active, QPalette::Disabled}) {
    const QString prefix = group == QPalette::Disabled ? QStringLiteral("disabled") : QString{};
    auto add = [&](const char* key, QPalette::ColorRole role) {
      result.insert(prefix + QLatin1String(key), palette.color(group, role));
    };
    add("Text", QPalette::WindowText);
    add("Background", QPalette::Window);
    add("Highlight", QPalette::Highlight);
    add("HighlightedText", QPalette::HighlightedText);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    add("Accent", QPalette::Accent);
#else
    add("Accent", QPalette::Highlight);
#endif
    const auto application = QGuiApplication::palette();
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    result.insert(prefix + QStringLiteral("ExplicitAccent"),
                  application.isBrushSet(group, QPalette::Accent) ||
                      (palette_ && palette_->toQPalette().isBrushSet(group, QPalette::Accent)) ||
                      palette.color(group, QPalette::Accent) != defaults.color(group, QPalette::Accent));
#endif
    result.insert(prefix + QStringLiteral("ExplicitText"),
                  application.isBrushSet(group, QPalette::WindowText) ||
                      (palette_ && palette_->toQPalette().isBrushSet(group, QPalette::WindowText)) ||
                      palette.color(group, QPalette::WindowText) != defaults.color(group, QPalette::WindowText));
  }
  return result;
}
