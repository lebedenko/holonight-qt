// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QFlags>
#include <QSizeF>
#include <QtGlobal>

namespace Holonight {

enum class SurfaceRole : quint8 {
  Window,
  Panel,
  Popup,
  Card,
  Menu,
  Tooltip,
  Control,
  Pill,
  Hud,
  WorkspaceIndicator,
};

enum class CornerStyle : quint8 {
  Inherit,
  Hybrid,
  Rounded,
  Chamfered,
};

enum class ShapeKind : quint8 {
  Rounded,
  Chamfered,
  Hybrid,
  Pill,
};

enum class Corner : quint8 {
  None = 0,
  TopLeft = 1 << 0,
  TopRight = 1 << 1,
  BottomRight = 1 << 2,
  BottomLeft = 1 << 3,
  All = 0x0f,
};
Q_DECLARE_FLAGS(CornerMask, Corner)
Q_DECLARE_OPERATORS_FOR_FLAGS(CornerMask)

struct ShapeTokens {
  qreal radius_xs = 4.0;
  qreal radius_sm = 6.0;
  qreal radius_md = 8.0;
  qreal radius_lg = 12.0;
  qreal chamfer_xs = 4.0;
  qreal chamfer_sm = 6.0;
  qreal chamfer_md = 10.0;
  qreal chamfer_lg = 16.0;

  friend constexpr bool operator==(const ShapeTokens&, const ShapeTokens&) = default;
};

struct ResolvedShape {
  ShapeKind kind = ShapeKind::Rounded;
  qreal radius = 0.0;
  qreal chamfer = 0.0;
  CornerMask chamfered_corners = Corner::None;

  friend constexpr bool operator==(const ResolvedShape&, const ResolvedShape&) = default;
};

class ShapeResolver {
 public:
  ShapeResolver() = delete;

  [[nodiscard]] static ShapeTokens defaultTokens();
  [[nodiscard]] static ResolvedShape resolve(SurfaceRole role, CornerStyle style = CornerStyle::Inherit,
                                             const QSizeF& size = {}, qreal radius_override = qQNaN(),
                                             qreal chamfer_override = qQNaN(),
                                             const ShapeTokens& tokens = defaultTokens());
  [[nodiscard]] static ResolvedShape resolve(SurfaceRole role, CornerStyle style, const QSizeF& size,
                                             qreal radius_override, qreal chamfer_override,
                                             int chamfered_corners_override,
                                             const ShapeTokens& tokens = defaultTokens());
};

}  // namespace Holonight
