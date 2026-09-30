// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QByteArray>
#include <QColor>
#include <QImage>
#include <QSize>

namespace Holonight {

struct IconSemanticColors {
  QColor text;
  QColor highlight;
  QColor positive;
  QColor neutral;
  QColor negative;
  // Appended roles preserve five-field positional initializers. Invalid colors
  // leave the corresponding authored CSS declarations unchanged.
  QColor accent;
  QColor background;
  QColor highlightedText;
};

enum class IconState { Normal = 0, Muted = 1, Disabled = 2, Active = 3, Selected = 4 };

[[nodiscard]] IconSemanticColors resolveIconColors(IconSemanticColors base, const IconSemanticColors& disabled,
                                                   IconState state);
[[nodiscard]] QColor blendIconColor(const QColor& source, const QColor& background, qreal amount);

class IconRenderer {
 public:
  [[nodiscard]] static QImage renderSvg(const QByteArray& svg_bytes, QSize target_size,
                                        const IconSemanticColors& colors, bool symbolic = false);
  [[nodiscard]] static QByteArray applySemanticColors(const QByteArray& svg_bytes, const IconSemanticColors& colors);
  [[nodiscard]] static bool hasSemanticRoles(const QByteArray& svg_bytes);
};

}  // namespace Holonight
