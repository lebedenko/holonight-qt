// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconengine.h"

#include "holonight/appearance_reader.h"
#include "iconrenderer.h"
#include "iconthemeresolver.h"
#include "themeresolver.h"

#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSvgRenderer>

#include <algorithm>

namespace Holonight {
namespace {

[[nodiscard]] IconSemanticColors colorsForMode(QIcon::Mode mode) {
  static AppearanceReader reader;
  const auto tokens = ThemeResolver::resolve(reader.appearance());
  const QPalette palette = QGuiApplication::palette().resolve(buildPalette(tokens));
  auto colors = [&](QPalette::ColorGroup group) -> IconSemanticColors {
    const auto background = palette.color(group, QPalette::Window);
    const bool disabled = group == QPalette::Disabled;
    auto status = [&](const QColor& color) { return disabled ? blendIconColor(color, background, 0.5) : color; };
    return {palette.color(group, QPalette::Text),
            palette.color(group, QPalette::Highlight),
            status(tokens.success),
            status(tokens.warning),
            status(tokens.error),
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            palette.color(group, QPalette::Accent),
#else
            status(tokens.primary),
#endif
            background,
            palette.color(group, QPalette::HighlightedText)};
  };
  // QIconEngine has no owning-widget palette; use the live application palette.
  return resolveIconColors(colors(QPalette::Active), colors(QPalette::Disabled),
                           mode == QIcon::Selected   ? IconState::Selected
                           : mode == QIcon::Disabled ? IconState::Disabled
                                                     : IconState::Normal);
}

}  // namespace

HnIconEngine::HnIconEngine(QString name) : name_{std::move(name)} {}

QIconEngine* HnIconEngine::clone() const { return new HnIconEngine{name_}; }

void HnIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
  const QPixmap image = pixmap(rect.size(), mode, state);
  painter->drawPixmap(rect, image);
}

QPixmap HnIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
  return scaledPixmap(size, mode, state, 1.0);
}

QPixmap HnIconEngine::scaledPixmap(const QSize& size, QIcon::Mode mode, QIcon::State /*state*/, qreal scale) {
  if (size.isEmpty() || scale <= 0 || scale > 8) return {};
  const QSize pixels{(std::max)(1, qRound(size.width() * scale)), (std::max)(1, qRound(size.height() * scale))};
  if (pixels.width() > 4096 || pixels.height() > 4096) return {};
  const QString path = IconThemeResolver::resolveIconPath(name_, size, scale);
  if (path.isEmpty()) return {};
  QImage image;
  if (path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
    const QByteArray bytes = IconThemeResolver::readIconBytes(path);
    image = IconRenderer::renderSvg(bytes, pixels, colorsForMode(mode), name_.endsWith(QStringLiteral("-symbolic")));
  } else {
    image = QImage{path}.scaled(pixels, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (!image.isNull() && name_.endsWith(QStringLiteral("-symbolic"))) {
      QPainter painter{&image};
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.fillRect(image.rect(), colorsForMode(mode).text);
    }
  }
  if (image.isNull()) return {};
  image.setDevicePixelRatio(scale);
  return QPixmap::fromImage(image);
}

QString HnIconEngine::iconName() { return name_; }

bool HnIconEngine::isNull() { return IconThemeResolver::resolveIconPath(name_).isEmpty(); }

}  // namespace Holonight
