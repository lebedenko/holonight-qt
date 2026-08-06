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
};

class IconRenderer {
 public:
  [[nodiscard]] static QImage renderSvg(const QByteArray& svg_bytes, QSize target_size,
                                        const IconSemanticColors& colors);
  [[nodiscard]] static QByteArray applySemanticColors(const QByteArray& svg_bytes, const IconSemanticColors& colors);
};

}  // namespace Holonight
