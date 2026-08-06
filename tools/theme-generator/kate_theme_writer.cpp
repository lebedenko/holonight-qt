// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "kate_theme_writer.h"

#include "text_style_mapping.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>

#include <KSyntaxHighlighting/Theme>
#include <stdexcept>
#include <string>

namespace HolonightThemeGen {

namespace {

using TS = KSyntaxHighlighting::Theme;

// Only the TextStyle enumerators; QMetaEnum::valueToKey() keeps this list in sync with
// theme.h without duplicating the 31 names as string literals here.
const QMetaEnum kTextStyleEnum = QMetaEnum::fromType<TS::TextStyle>();

QJsonObject textStyleEntry(TS::TextStyle style, const Holonight::ColorTokens& tokens) {
  QJsonObject entry;
  entry[QStringLiteral("text-color")] = colorForTextStyle(style, tokens).name();
  return entry;
}

QJsonObject buildTextStyles(const Holonight::ColorTokens& tokens) {
  QJsonObject styles;
  for (int index = 0; index < kTextStyleEnum.keyCount(); ++index) {
    const auto style = static_cast<TS::TextStyle>(kTextStyleEnum.value(index));
    styles[QString::fromLatin1(kTextStyleEnum.key(index))] = textStyleEntry(style, tokens);
  }
  return styles;
}

// Minimal set required by the Kate theme schema. SyntaxHighlighter driving a QTextDocument from
// QML never reads this block itself -- HnCodeBlock.qml draws its own background -- so only the
// keys the schema requires are populated, mapped straightforwardly to the closest ColorTokens role.
QJsonObject buildEditorColors(const Holonight::ColorTokens& tokens) {
  QJsonObject colors;
  colors[QStringLiteral("background-color")] = tokens.surface.name();
  colors[QStringLiteral("selection")] = tokens.primary.name();
  colors[QStringLiteral("current-line")] = tokens.surfaceHover.name();
  colors[QStringLiteral("line-numbers")] = tokens.textMuted.name();
  colors[QStringLiteral("current-line-number")] = tokens.textPrimary.name();
  colors[QStringLiteral("icon-border")] = tokens.surface.name();
  colors[QStringLiteral("indentation-line")] = tokens.borderSubtle.name();
  colors[QStringLiteral("separator")] = tokens.borderSubtle.name();
  return colors;
}

}  // namespace

void writeThemeJson(const QString& schemeId, const Holonight::ColorTokens& tokens, const QString& outputPath) {
  QJsonObject metadata;
  metadata[QStringLiteral("name")] = schemeId;
  metadata[QStringLiteral("revision")] = 1;

  QJsonObject root;
  root[QStringLiteral("metadata")] = metadata;
  root[QStringLiteral("text-styles")] = buildTextStyles(tokens);
  root[QStringLiteral("editor-colors")] = buildEditorColors(tokens);

  QFile file{outputPath};
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    throw std::runtime_error{"failed to open " + outputPath.toStdString() + " for writing"};
  }
  file.write(QJsonDocument{root}.toJson(QJsonDocument::Indented));
}

}  // namespace HolonightThemeGen
