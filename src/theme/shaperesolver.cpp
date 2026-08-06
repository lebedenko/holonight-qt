// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "shaperesolver.h"

#include <algorithm>
#include <cmath>

namespace Holonight {
namespace {

struct RoleShape {
  ShapeKind kind;
  qreal radius;
  qreal chamfer;
};

[[nodiscard]] RoleShape shapeForRole(SurfaceRole role, const ShapeTokens& tokens, qreal height) {
  switch (role) {
    case SurfaceRole::Window:
      return {.kind = ShapeKind::Hybrid, .radius = tokens.radius_lg, .chamfer = tokens.chamfer_md};
    case SurfaceRole::Panel:
      return {.kind = ShapeKind::Chamfered, .radius = tokens.radius_lg, .chamfer = tokens.chamfer_lg};
    case SurfaceRole::Popup:
      return {.kind = ShapeKind::Hybrid, .radius = tokens.radius_lg, .chamfer = tokens.chamfer_md};
    case SurfaceRole::Card:
      return {.kind = ShapeKind::Rounded, .radius = tokens.radius_md, .chamfer = tokens.chamfer_xs};
    case SurfaceRole::Menu:
    case SurfaceRole::Tooltip:
      return {.kind = ShapeKind::Hybrid, .radius = tokens.radius_sm, .chamfer = tokens.chamfer_xs};
    case SurfaceRole::Control:
      return {.kind = ShapeKind::Rounded, .radius = tokens.radius_sm, .chamfer = tokens.chamfer_xs};
    case SurfaceRole::Pill:
      return {.kind = ShapeKind::Pill, .radius = (std::max)(0.0, height / 2.0), .chamfer = tokens.chamfer_xs};
    case SurfaceRole::Hud:
      return {.kind = ShapeKind::Hybrid, .radius = tokens.radius_lg, .chamfer = tokens.chamfer_md};
    case SurfaceRole::WorkspaceIndicator:
      return {.kind = ShapeKind::Chamfered, .radius = tokens.radius_sm, .chamfer = tokens.chamfer_sm};
  }
  return {.kind = ShapeKind::Rounded, .radius = tokens.radius_md, .chamfer = tokens.chamfer_xs};
}

[[nodiscard]] bool validOverride(qreal value) { return std::isfinite(value) && value >= 0.0; }

}  // namespace

ShapeTokens ShapeResolver::defaultTokens() { return {}; }

ResolvedShape ShapeResolver::resolve(SurfaceRole role, CornerStyle style, const QSizeF& size, qreal radius_override,
                                     qreal chamfer_override, const ShapeTokens& tokens) {
  return resolve(role, style, size, radius_override, chamfer_override, -1, tokens);
}

ResolvedShape ShapeResolver::resolve(SurfaceRole role, CornerStyle style, const QSizeF& size, qreal radius_override,
                                     qreal chamfer_override, int chamfered_corners_override,
                                     const ShapeTokens& tokens) {
  const qreal width = (std::max)(0.0, size.width());
  const qreal height = (std::max)(0.0, size.height());
  RoleShape role_shape = shapeForRole(role, tokens, height);

  if (style == CornerStyle::Rounded) {
    role_shape.kind = role == SurfaceRole::Pill ? ShapeKind::Pill : ShapeKind::Rounded;
  } else if (style == CornerStyle::Chamfered) {
    role_shape.kind = ShapeKind::Chamfered;
  }

  if (validOverride(radius_override)) {
    role_shape.radius = radius_override;
  }
  if (validOverride(chamfer_override)) {
    role_shape.chamfer = chamfer_override;
  }

  const qreal maximum_extent = (std::max)(0.0, (std::min)(width, height) / 2.0);
  if (width <= 0.0 || height <= 0.0) {
    role_shape.radius = 0.0;
    role_shape.chamfer = 0.0;
  } else {
    role_shape.radius = std::clamp(role_shape.radius, 0.0, maximum_extent);
    role_shape.chamfer = std::clamp(role_shape.chamfer, 0.0, maximum_extent);
  }

  CornerMask corners = Corner::None;
  if (role_shape.kind == ShapeKind::Chamfered) {
    corners = Corner::All;
  } else if (role_shape.kind == ShapeKind::Hybrid) {
    corners = Corner::TopRight | Corner::BottomLeft;
  }

  if (chamfered_corners_override >= static_cast<int>(Corner::None) &&
      chamfered_corners_override <= static_cast<int>(Corner::All)) {
    corners = CornerMask::fromInt(chamfered_corners_override);
  }

  ShapeKind final_kind = ShapeKind::Hybrid;
  if (corners == Corner::None) {
    final_kind = role_shape.kind == ShapeKind::Pill ? ShapeKind::Pill : ShapeKind::Rounded;
  } else if (corners == Corner::All) {
    final_kind = ShapeKind::Chamfered;
  }

  return {.kind = final_kind, .radius = role_shape.radius, .chamfer = role_shape.chamfer, .chamfered_corners = corners};
}

}  // namespace Holonight
