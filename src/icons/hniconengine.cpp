// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconengine.h"

#include "iconrenderer.h"
#include "iconthemeresolver.h"

#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSvgRenderer>

#include <algorithm>

namespace Holonight {
namespace {

[[nodiscard]] IconSemanticColors colorsForMode(QIcon::Mode mode) {
  const QPalette palette = QGuiApplication::palette();
  const QPalette::ColorGroup group = mode == QIcon::Disabled ? QPalette::Disabled : QPalette::Active;
  const QPalette::ColorRole foreground = mode == QIcon::Selected ? QPalette::HighlightedText : QPalette::Text;
  return {.text = palette.color(group, foreground),
          .highlight = palette.color(group, QPalette::Highlight),
          .positive = palette.color(group, QPalette::Text),
          .neutral = palette.color(group, QPalette::Text),
          .negative = palette.color(group, QPalette::Text)};
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
