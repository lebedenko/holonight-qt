// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "iconrenderer.h"

#include <QPainter>
#include <QRegularExpression>
#include <QSvgRenderer>

#include <algorithm>

namespace Holonight {
namespace {

[[nodiscard]] QString cssColor(const QColor& color) {
  return color.alpha() == 255 ? color.name(QColor::HexRgb) : color.name(QColor::HexArgb);
}

[[nodiscard]] bool hasSemanticColorClasses(const QString& svg) {
  return svg.contains(QStringLiteral("ColorScheme-Text")) || svg.contains(QStringLiteral("ColorScheme-Highlight")) ||
         svg.contains(QStringLiteral("ColorScheme-PositiveText")) ||
         svg.contains(QStringLiteral("ColorScheme-NeutralText")) ||
         svg.contains(QStringLiteral("ColorScheme-NegativeText"));
}

void replaceClassColor(QString* svg, const QRegularExpression& block_expression, const QColor& color) {
  svg->replace(block_expression, QStringLiteral("\\1%1\\3").arg(cssColor(color)));
}

void replaceLiteralPaintColors(QString* svg, const QColor& color) {
  static const QRegularExpression paint_expression{
      QStringLiteral("((?:\\b(?:fill|stroke|stop-color|color)\\s*(?:=\\s*[\"']|:\\s*)))(#[0-9A-Fa-f]{3,8})")};
  svg->replace(paint_expression, QStringLiteral("\\1%1").arg(cssColor(color)));
}

[[nodiscard]] QSize targetSize(QSize target_size) {
  if (!target_size.isValid() || target_size.isEmpty()) {
    target_size = QSize{24, 24};
  }
  return QSize{(std::max)(1, target_size.width()), (std::max)(1, target_size.height())};
}

}  // namespace

QByteArray IconRenderer::applySemanticColors(const QByteArray& svg_bytes, const IconSemanticColors& colors) {
  static const QRegularExpression text_expression{
      QStringLiteral("(\\.ColorScheme-Text\\s*\\{[^}]*?color\\s*:\\s*)(#[0-9A-Fa-f]{3,8}|[A-Za-z]+)(\\s*;?)")};
  static const QRegularExpression highlight_expression{
      QStringLiteral("(\\.ColorScheme-Highlight\\s*\\{[^}]*?color\\s*:\\s*)(#[0-9A-Fa-f]{3,8}|[A-Za-z]+)(\\s*;?)")};
  static const QRegularExpression positive_expression{
      QStringLiteral("(\\.ColorScheme-PositiveText\\s*\\{[^}]*?color\\s*:\\s*)(#[0-9A-Fa-f]{3,8}|[A-Za-z]+)(\\s*;?)")};
  static const QRegularExpression neutral_expression{
      QStringLiteral("(\\.ColorScheme-NeutralText\\s*\\{[^}]*?color\\s*:\\s*)(#[0-9A-Fa-f]{3,8}|[A-Za-z]+)(\\s*;?)")};
  static const QRegularExpression negative_expression{
      QStringLiteral("(\\.ColorScheme-NegativeText\\s*\\{[^}]*?color\\s*:\\s*)(#[0-9A-Fa-f]{3,8}|[A-Za-z]+)(\\s*;?)")};

  QString svg = QString::fromUtf8(svg_bytes);
  if (hasSemanticColorClasses(svg)) {
    replaceClassColor(&svg, text_expression, colors.text);
    replaceClassColor(&svg, highlight_expression, colors.highlight);
    replaceClassColor(&svg, positive_expression, colors.positive);
    replaceClassColor(&svg, neutral_expression, colors.neutral);
    replaceClassColor(&svg, negative_expression, colors.negative);
  } else {
    replaceLiteralPaintColors(&svg, colors.text);
  }
  return svg.toUtf8();
}

QImage IconRenderer::renderSvg(const QByteArray& svg_bytes, QSize target_size, const IconSemanticColors& colors) {
  const QByteArray themed_svg = applySemanticColors(svg_bytes, colors);
  QSvgRenderer renderer = QSvgRenderer{themed_svg};
  if (!renderer.isValid()) {
    return {};
  }

  const QSize pixel_size = targetSize(target_size);
  QImage image = QImage{pixel_size, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);

  QPainter painter = QPainter{&image};
  renderer.render(&painter, QRectF{QPointF{0.0, 0.0}, QSizeF{pixel_size}});
  return image;
}

}  // namespace Holonight
