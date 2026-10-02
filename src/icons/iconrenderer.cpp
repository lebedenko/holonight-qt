// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "iconrenderer.h"

#include <QPainter>
#include <QRegularExpression>
#include <QSvgRenderer>
#include <QXmlStreamReader>

#include <algorithm>
#include <array>
#include <ranges>

namespace Holonight {
namespace {

[[nodiscard]] QString cssColor(const QColor& color) {
  return color.alpha() == 255 ? color.name(QColor::HexRgb) : color.name(QColor::HexArgb);
}

[[nodiscard]] QSize targetSize(QSize target_size) {
  if (!target_size.isValid() || target_size.isEmpty()) {
    target_size = QSize{24, 24};
  }
  return QSize{(std::max)(1, target_size.width()), (std::max)(1, target_size.height())};
}

}  // namespace

QColor blendIconColor(const QColor& source, const QColor& background, qreal amount) {
  if (!source.isValid() || !background.isValid()) {
    return source;
  }
  return QColor::fromRgbF(static_cast<float>((source.redF() * (1 - amount)) + (background.redF() * amount)),
                          static_cast<float>((source.greenF() * (1 - amount)) + (background.greenF() * amount)),
                          static_cast<float>((source.blueF() * (1 - amount)) + (background.blueF() * amount)),
                          source.alphaF());
}

IconSemanticColors resolveIconColors(IconSemanticColors base, const IconSemanticColors& disabled, IconState state) {
  if (state == IconState::Disabled) {
    return disabled;
  }
  if (state == IconState::Selected) {
    base.text = base.highlightedText;
    base.positive = base.neutral = base.negative = base.highlightedText;
    base.background = base.highlight;
    base.accent = blendIconColor(base.accent, base.highlightedText, 0.15);
    base.highlightedText = base.highlight;
    base.highlight = base.text;
  }
  return base;
}

namespace {
struct Declaration {
  qsizetype start;
  qsizetype length;
  QColor color;
};

QList<Declaration> colorDeclarations(const QString& parsed_css, const IconSemanticColors& colors) {
  const std::array roles{
      std::pair{QStringLiteral("Text"), colors.text},
      std::pair{QStringLiteral("Highlight"), colors.highlight},
      std::pair{QStringLiteral("PositiveText"), colors.positive},
      std::pair{QStringLiteral("NeutralText"), colors.neutral},
      std::pair{QStringLiteral("NegativeText"), colors.negative},
      std::pair{QStringLiteral("Accent"), colors.accent},
      std::pair{QStringLiteral("Background"), colors.background},
      std::pair{QStringLiteral("HighlightedText"), colors.highlightedText},
  };
  QList<Declaration> declarations;
  for (const auto& [name, value] : roles) {
    if (!value.isValid()) {
      continue;
    }
    const QRegularExpression rule{QStringLiteral("\\.ColorScheme-%1\\s*\\{([^}]*)\\}").arg(name)};
    auto rules = rule.globalMatch(parsed_css);
    while (rules.hasNext()) {
      const auto block = rules.next();
      static const QRegularExpression color_declaration{
          QStringLiteral("(?:^|;)\\s*color\\s*:\\s*([^;]*?)(\\s*(?:!important\\s*)?)(?=;|$)")};
      auto matches = color_declaration.globalMatch(block.captured(1));
      while (matches.hasNext()) {
        const auto declaration = matches.next();
        declarations.append({
            .start = block.capturedStart(1) + declaration.capturedStart(1),
            .length = declaration.capturedLength(1),
            .color = value,
        });
      }
    }
  }
  return declarations;
}

QString recolorCss(QString css, const IconSemanticColors& colors, bool rendering) {
  QString parsed_css = css;
  static const QRegularExpression comments{QStringLiteral("/\\*[\\s\\S]*?\\*/")};
  auto comment_matches = comments.globalMatch(parsed_css);
  while (comment_matches.hasNext()) {
    const auto comment = comment_matches.next();
    for (qsizetype index = comment.capturedStart(); index < comment.capturedEnd(); ++index) {
      parsed_css[index] = QLatin1Char(' ');
    }
  }
  auto declarations = colorDeclarations(parsed_css, colors);
  std::ranges::sort(declarations,
                    [](const Declaration& left, const Declaration& right) { return left.start > right.start; });
  for (const auto& declaration : declarations) {
    QString replacement = cssColor(declaration.color);
    // QtSvg does not parse RGBA CSS colors. Its color-opacity property affects
    // currentColor alone, leaving authored layer/fill/stroke opacity intact.
    if (rendering && declaration.color.alpha() != 255) {
      replacement = declaration.color.name(QColor::HexRgb) +
                    QStringLiteral(";color-opacity:%1").arg(declaration.color.alphaF(), 0, 'g', 8);
    }
    css.replace(declaration.start, declaration.length, replacement);
  }
  return css;
}

QByteArray recolorSvg(const QByteArray& svg_bytes, const IconSemanticColors& colors, bool rendering) {
  QString svg = QString::fromUtf8(svg_bytes);
  struct Block {
    qsizetype start;
    qsizetype length;
  };
  QList<Block> blocks;
  QXmlStreamReader xml{svg};
  while (!xml.atEnd()) {
    xml.readNext();
    if (!xml.isStartElement() || xml.name() != QStringLiteral("style") ||
        xml.attributes().value(QStringLiteral("id")) != QStringLiteral("current-color-scheme")) {
      continue;
    }
    const qsizetype start = xml.characterOffset();
    xml.readElementText(QXmlStreamReader::SkipChildElements);
    const qsizetype end = svg.lastIndexOf(QStringLiteral("</"), xml.characterOffset() - 1);
    if (end >= start) {
      blocks.append({.start = start, .length = end - start});
    }
  }
  if (xml.hasError()) {
    return svg_bytes;
  }
  for (auto block : std::views::reverse(blocks)) {
    const QString css = recolorCss(svg.mid(block.start, block.length), colors, rendering);
    svg.replace(block.start, block.length, css);
  }
  return svg.toUtf8();
}
}  // namespace

QByteArray IconRenderer::applySemanticColors(const QByteArray& svg_bytes, const IconSemanticColors& colors) {
  return recolorSvg(svg_bytes, colors, false);
}

bool IconRenderer::hasSemanticRoles(const QByteArray& svg_bytes) {
  static const QRegularExpression role{QStringLiteral(
      "^ColorScheme-(?:Text|Highlight|PositiveText|NeutralText|NegativeText|Accent|Background|HighlightedText)$")};
  QXmlStreamReader xml{svg_bytes};
  while (!xml.atEnd()) {
    xml.readNext();
    if (!xml.isStartElement()) {
      continue;
    }
    const auto tokens = xml.attributes()
                            .value(QStringLiteral("class"))
                            .toString()
                            .split(QRegularExpression{QStringLiteral("\\s+")}, Qt::SkipEmptyParts);
    for (const auto& token : tokens) {
      if (role.match(token).hasMatch()) {
        return true;
      }
    }
  }
  return false;
}

QImage IconRenderer::renderSvg(const QByteArray& svg_bytes, QSize target_size, const IconSemanticColors& colors,
                               bool symbolic) {
  const QByteArray themed_svg = recolorSvg(svg_bytes, colors, true);
  QSvgRenderer renderer = QSvgRenderer{themed_svg};
  if (!renderer.isValid()) {
    return {};
  }

  const QSize pixel_size = targetSize(target_size);
  QImage image = QImage{pixel_size, QImage::Format_ARGB32_Premultiplied};
  image.fill(Qt::transparent);

  QPainter painter = QPainter{&image};
  renderer.render(&painter, QRectF{QPointF{0.0, 0.0}, QSizeF{pixel_size}});
  painter.end();
  if (symbolic && !hasSemanticRoles(svg_bytes)) {
    QPainter mask_painter{&image};
    mask_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    mask_painter.fillRect(image.rect(), colors.text);
  }
  return image;
}

}  // namespace Holonight
