// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "shaperesolver.h"

#include <QSizeF>

#include <array>
#include <gtest/gtest.h>
#include <limits>

namespace {

using Holonight::Corner;
using Holonight::CornerStyle;
using Holonight::ShapeKind;
using Holonight::ShapeResolver;
using Holonight::SurfaceRole;

[[nodiscard]] bool validGeometry(const Holonight::ResolvedShape& shape, qreal maximum_extent) {
  return std::isfinite(shape.radius) && std::isfinite(shape.chamfer) && shape.radius >= 0.0 && shape.chamfer >= 0.0 &&
         shape.radius <= maximum_extent && shape.chamfer <= maximum_extent;
}

TEST(ShapeResolver, DefaultTokensDefineSharedPrimitiveScale) {
  const Holonight::ShapeTokens tokens = ShapeResolver::defaultTokens();

  EXPECT_DOUBLE_EQ(tokens.radius_xs, 4.0);
  EXPECT_DOUBLE_EQ(tokens.radius_sm, 6.0);
  EXPECT_DOUBLE_EQ(tokens.radius_md, 8.0);
  EXPECT_DOUBLE_EQ(tokens.radius_lg, 12.0);
  EXPECT_DOUBLE_EQ(tokens.chamfer_xs, 4.0);
  EXPECT_DOUBLE_EQ(tokens.chamfer_sm, 6.0);
  EXPECT_DOUBLE_EQ(tokens.chamfer_md, 10.0);
  EXPECT_DOUBLE_EQ(tokens.chamfer_lg, 16.0);
}

TEST(ShapeResolver, HybridProfilePreservesVisualHierarchy) {
  const Holonight::ResolvedShape popup =
      ShapeResolver::resolve(SurfaceRole::Popup, CornerStyle::Inherit, QSizeF{200, 100});
  const Holonight::ResolvedShape card =
      ShapeResolver::resolve(SurfaceRole::Card, CornerStyle::Inherit, QSizeF{200, 100});
  const Holonight::ResolvedShape workspace =
      ShapeResolver::resolve(SurfaceRole::WorkspaceIndicator, CornerStyle::Inherit, QSizeF{40, 20});

  EXPECT_EQ(popup.kind, ShapeKind::Hybrid);
  EXPECT_DOUBLE_EQ(popup.radius, 12.0);
  EXPECT_DOUBLE_EQ(popup.chamfer, 10.0);
  EXPECT_EQ(popup.chamfered_corners, Corner::TopRight | Corner::BottomLeft);

  EXPECT_EQ(card.kind, ShapeKind::Rounded);
  EXPECT_DOUBLE_EQ(card.radius, 8.0);
  EXPECT_EQ(card.chamfered_corners, Corner::None);

  EXPECT_EQ(workspace.kind, ShapeKind::Chamfered);
  EXPECT_DOUBLE_EQ(workspace.chamfer, 6.0);
  EXPECT_EQ(workspace.chamfered_corners, Corner::All);
}

TEST(ShapeResolver, ExplicitStyleOverridesSemanticKind) {
  const Holonight::ResolvedShape rounded =
      ShapeResolver::resolve(SurfaceRole::Panel, CornerStyle::Rounded, QSizeF{200, 100});
  const Holonight::ResolvedShape chamfered =
      ShapeResolver::resolve(SurfaceRole::Control, CornerStyle::Chamfered, QSizeF{100, 40});

  EXPECT_EQ(rounded.kind, ShapeKind::Rounded);
  EXPECT_EQ(rounded.chamfered_corners, Corner::None);
  EXPECT_EQ(chamfered.kind, ShapeKind::Chamfered);
  EXPECT_EQ(chamfered.chamfered_corners, Corner::All);
}

TEST(ShapeResolver, PillRadiusComesFromRenderedHeight) {
  const Holonight::ResolvedShape pill = ShapeResolver::resolve(SurfaceRole::Pill, CornerStyle::Inherit, QSizeF{80, 24});

  EXPECT_EQ(pill.kind, ShapeKind::Pill);
  EXPECT_DOUBLE_EQ(pill.radius, 12.0);
  EXPECT_EQ(pill.chamfered_corners, Corner::None);
}

TEST(ShapeResolver, GeometryIsClampedToAvailableDimensions) {
  const Holonight::ResolvedShape shape = ShapeResolver::resolve(SurfaceRole::Panel, CornerStyle::Inherit, QSizeF{8, 6});
  const Holonight::ResolvedShape empty =
      ShapeResolver::resolve(SurfaceRole::Popup, CornerStyle::Inherit, QSizeF{-1, 20});

  EXPECT_DOUBLE_EQ(shape.radius, 3.0);
  EXPECT_DOUBLE_EQ(shape.chamfer, 3.0);
  EXPECT_DOUBLE_EQ(empty.radius, 0.0);
  EXPECT_DOUBLE_EQ(empty.chamfer, 0.0);
}

TEST(ShapeResolver, FiniteNonNegativeOverridesTakePrecedence) {
  const qreal nan = std::numeric_limits<qreal>::quiet_NaN();
  const Holonight::ResolvedShape overridden =
      ShapeResolver::resolve(SurfaceRole::Card, CornerStyle::Inherit, QSizeF{100, 60}, 5.0, 7.0);
  const Holonight::ResolvedShape ignored =
      ShapeResolver::resolve(SurfaceRole::Card, CornerStyle::Inherit, QSizeF{100, 60}, -1.0, nan);

  EXPECT_DOUBLE_EQ(overridden.radius, 5.0);
  EXPECT_DOUBLE_EQ(overridden.chamfer, 7.0);
  EXPECT_DOUBLE_EQ(ignored.radius, 8.0);
  EXPECT_DOUBLE_EQ(ignored.chamfer, 4.0);
}

TEST(ShapeResolver, EveryCornerMaskOverridesResolvedTopologyAndClassifiesFinalKind) {
  for (int mask = static_cast<int>(Corner::None); mask <= static_cast<int>(Corner::All); ++mask) {
    const Holonight::ResolvedShape shape =
        ShapeResolver::resolve(SurfaceRole::Panel, CornerStyle::Chamfered, QSizeF{200, 100}, qQNaN(), qQNaN(), mask);

    EXPECT_EQ(shape.chamfered_corners.toInt(), mask);
    if (mask == static_cast<int>(Corner::None)) {
      EXPECT_EQ(shape.kind, ShapeKind::Rounded);
    } else if (mask == static_cast<int>(Corner::All)) {
      EXPECT_EQ(shape.kind, ShapeKind::Chamfered);
    } else {
      EXPECT_EQ(shape.kind, ShapeKind::Hybrid);
    }
  }
}

TEST(ShapeResolver, InvalidCornerMasksInheritResolvedTopology) {
  const Holonight::ResolvedShape expected =
      ShapeResolver::resolve(SurfaceRole::Popup, CornerStyle::Rounded, QSizeF{200, 100});
  const Holonight::ResolvedShape below_range =
      ShapeResolver::resolve(SurfaceRole::Popup, CornerStyle::Rounded, QSizeF{200, 100}, qQNaN(), qQNaN(), -2);
  const Holonight::ResolvedShape above_range =
      ShapeResolver::resolve(SurfaceRole::Popup, CornerStyle::Rounded, QSizeF{200, 100}, qQNaN(), qQNaN(), 16);

  EXPECT_EQ(below_range, expected);
  EXPECT_EQ(above_range, expected);
}

TEST(ShapeResolver, CornerMaskWinsOverRoleAndStyleWithoutChangingClampedDimensions) {
  const int mixed_mask = static_cast<int>(Corner::TopLeft | Corner::BottomRight);
  const Holonight::ResolvedShape shape =
      ShapeResolver::resolve(SurfaceRole::Panel, CornerStyle::Rounded, QSizeF{8, 6}, 20.0, 20.0, mixed_mask);

  EXPECT_EQ(shape.kind, ShapeKind::Hybrid);
  EXPECT_EQ(shape.chamfered_corners.toInt(), mixed_mask);
  EXPECT_DOUBLE_EQ(shape.radius, 3.0);
  EXPECT_DOUBLE_EQ(shape.chamfer, 3.0);
}

TEST(ShapeResolver, AllRoundedPillRemainsPill) {
  const Holonight::ResolvedShape inherited =
      ShapeResolver::resolve(SurfaceRole::Pill, CornerStyle::Inherit, QSizeF{80, 24});
  const Holonight::ResolvedShape explicit_none =
      ShapeResolver::resolve(SurfaceRole::Pill, CornerStyle::Inherit, QSizeF{80, 24}, qQNaN(), qQNaN(), 0);

  EXPECT_EQ(inherited.kind, ShapeKind::Pill);
  EXPECT_EQ(explicit_none.kind, ShapeKind::Pill);
}

TEST(ShapeResolver, EveryRoleAndStyleProducesFiniteNonNegativeGeometry) {
  constexpr std::array roles = {
      SurfaceRole::Window,  SurfaceRole::Panel,
      SurfaceRole::Popup,   SurfaceRole::Card,
      SurfaceRole::Menu,    SurfaceRole::Tooltip,
      SurfaceRole::Control, SurfaceRole::Pill,
      SurfaceRole::Hud,     SurfaceRole::WorkspaceIndicator,
  };
  constexpr std::array styles = {CornerStyle::Hybrid, CornerStyle::Rounded, CornerStyle::Chamfered};

  for (const SurfaceRole role : roles) {
    for (const CornerStyle style : styles) {
      const Holonight::ResolvedShape shape = ShapeResolver::resolve(role, style, QSizeF{32.5, 17.5});
      EXPECT_TRUE(validGeometry(shape, 8.75));
    }
  }
}

}  // namespace
