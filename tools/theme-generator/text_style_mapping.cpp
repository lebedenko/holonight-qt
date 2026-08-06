// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "text_style_mapping.h"

namespace HolonightThemeGen {

QColor colorForTextStyle(KSyntaxHighlighting::Theme::TextStyle style, const Holonight::ColorTokens& tokens) {
  using TS = KSyntaxHighlighting::Theme;
  switch (style) {
    case TS::Normal:
      return tokens.textPrimary;  // default code text
    case TS::Keyword:
      return tokens.accentViolet;  // language keywords, distinct from flow
    case TS::Function:
      return tokens.accentBlue;  // calls/definitions
    case TS::Variable:
      return tokens.textSecondary;  // identifiers, slightly de-emphasized
    case TS::ControlFlow:
      return tokens.primary;  // if/else/return -- most prominent accent
    case TS::Operator:
      return tokens.textSecondary;
    case TS::BuiltIn:
      return tokens.accentCyan;
    case TS::Extension:
      return tokens.accentCyan;  // Qt/boost-style extensions, group with BuiltIn
    case TS::Preprocessor:
      return tokens.warning;  // directives, attention-grabbing
    case TS::Attribute:
      return tokens.accentYellow;
    case TS::Char:
      return tokens.success;  // group with String
    case TS::SpecialChar:
      return tokens.accentYellow;  // escapes inside strings, stand out
    case TS::String:
      return tokens.success;
    case TS::VerbatimString:
      return tokens.success;
    case TS::SpecialString:
      return tokens.warning;  // regex/etc., attention
    case TS::Import:
      return tokens.accentViolet;  // group with Keyword
    case TS::DataType:
      return tokens.accentBlue;  // types, group with Function
    case TS::DecVal:
      return tokens.accentYellow;
    case TS::BaseN:
      return tokens.accentYellow;
    case TS::Float:
      return tokens.accentYellow;
    case TS::Constant:
      return tokens.accentCyan;  // true/false/null, group with BuiltIn
    case TS::Comment:
      return tokens.textMuted;
    case TS::Documentation:
      return tokens.textSecondary;  // doc comments, one step above plain
    case TS::Annotation:
      return tokens.accentViolet;
    case TS::CommentVar:
      return tokens.textSecondary;
    case TS::RegionMarker:
      return tokens.textMuted;
    case TS::Information:
      return tokens.primary;
    case TS::Warning:
      return tokens.warning;
    case TS::Alert:
      return tokens.warning;
    case TS::Error:
      return tokens.error;
    case TS::Others:
      return tokens.textPrimary;
  }
  return tokens.textPrimary;
}

}  // namespace HolonightThemeGen
