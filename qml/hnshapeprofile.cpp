// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnshapeprofile.h"

#include "hnshapetypes.h"
#include "shaperesolver.h"

#include <QSizeF>

namespace {

static_assert(HnSurfaceRole::WorkspaceIndicator == static_cast<int>(Holonight::SurfaceRole::WorkspaceIndicator));
static_assert(HnCornerStyle::Chamfered == static_cast<int>(Holonight::CornerStyle::Chamfered));
static_assert(HnShapeKind::Pill == static_cast<int>(Holonight::ShapeKind::Pill));

[[nodiscard]] Holonight::SurfaceRole surfaceRole(int role) {
  if (role < HnSurfaceRole::Window || role > HnSurfaceRole::WorkspaceIndicator) {
    return Holonight::SurfaceRole::Card;
  }
  return static_cast<Holonight::SurfaceRole>(role);
}

[[nodiscard]] Holonight::CornerStyle cornerStyle(int style) {
  if (style < HnCornerStyle::Inherit || style > HnCornerStyle::Chamfered) {
    return Holonight::CornerStyle::Inherit;
  }
  return static_cast<Holonight::CornerStyle>(style);
}

}  // namespace

HnShapeProfile::HnShapeProfile(QObject* parent) : QObject{parent} {}

QVariantMap HnShapeProfile::resolve(
    int role, int style, qreal width, qreal height, qreal radius_override,
    qreal chamfer_override) const {  // NOLINT(readability-convert-member-functions-to-static)
  return resolve(role, style, width, height, radius_override, chamfer_override, HnCornerMask::Inherit);
}

QVariantMap HnShapeProfile::resolve(
    int role, int style, qreal width, qreal height, qreal radius_override, qreal chamfer_override,
    int chamfered_corners_override) const {  // NOLINT(readability-convert-member-functions-to-static)
  const Holonight::ResolvedShape shape =
      Holonight::ShapeResolver::resolve(surfaceRole(role), cornerStyle(style), QSizeF{width, height}, radius_override,
                                        chamfer_override, chamfered_corners_override);
  return {
      {QStringLiteral("kind"), static_cast<int>(shape.kind)},
      {QStringLiteral("radius"), shape.radius},
      {QStringLiteral("chamfer"), shape.chamfer},
      {QStringLiteral("corners"), shape.chamfered_corners.toInt()},
  };
}
