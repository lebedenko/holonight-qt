// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnappearance.h"

#include "appearancecontext.h"
#include "hnshapetypes.h"
#include "shaperesolver.h"

#include <QSizeF>

namespace {

Holonight::SurfaceRole surfaceRole(int role) {
  if (role < HnSurfaceRole::Window || role > HnSurfaceRole::WorkspaceIndicator) {
    return Holonight::SurfaceRole::Card;
  }
  return static_cast<Holonight::SurfaceRole>(role);
}

Holonight::CornerStyle parsedCornerStyle(int style) {
  if (style < HnCornerStyle::Inherit || style > HnCornerStyle::Chamfered) {
    return Holonight::CornerStyle::Inherit;
  }
  return static_cast<Holonight::CornerStyle>(style);
}

Holonight::CornerStyle globalCornerStyle(const Holonight::ResolvedAppearance& appearance) {
  switch (appearance.shape_style) {
    case Holonight::ResolvedShapeStyle::Hybrid:
      return Holonight::CornerStyle::Hybrid;
    case Holonight::ResolvedShapeStyle::Rounded:
      return Holonight::CornerStyle::Rounded;
    case Holonight::ResolvedShapeStyle::Chamfered:
      return Holonight::CornerStyle::Chamfered;
    case Holonight::ResolvedShapeStyle::Inherit:
      return Holonight::CornerStyle::Inherit;
  }
  return Holonight::CornerStyle::Inherit;
}

Holonight::ShapeTokens shapeTokens(const Holonight::ResolvedAppearance& appearance) {
  Holonight::ShapeTokens tokens = Holonight::ShapeResolver::defaultTokens();
  tokens.radius_xs *= appearance.shape_scale;
  tokens.radius_sm *= appearance.shape_scale;
  tokens.radius_md *= appearance.shape_scale;
  tokens.radius_lg *= appearance.shape_scale;
  tokens.chamfer_xs *= appearance.shape_scale;
  tokens.chamfer_sm *= appearance.shape_scale;
  tokens.chamfer_md *= appearance.shape_scale;
  tokens.chamfer_lg *= appearance.shape_scale;
  if (appearance.base_radius.has_value()) {
    tokens.radius_xs = tokens.radius_sm = tokens.radius_md = tokens.radius_lg = *appearance.base_radius;
  }
  if (appearance.base_chamfer.has_value()) {
    tokens.chamfer_xs = tokens.chamfer_sm = tokens.chamfer_md = tokens.chamfer_lg = *appearance.base_chamfer;
  }
  return tokens;
}

}  // namespace

HnAppearance::HnAppearance(Holonight::AppearanceReader* reader, QObject* parent) : QObject{parent}, reader_{reader} {
  connect(reader_, &Holonight::AppearanceReader::appearanceChanged, this, &HnAppearance::appearanceChanged);
}

HnAppearance* HnAppearance::create(QQmlEngine* engine, QJSEngine* script_engine) {
  Q_UNUSED(script_engine)
  return new HnAppearance{Holonight::appearanceReaderForEngine(engine), engine};
}

int HnAppearance::cornerStyle() const { return static_cast<int>(globalCornerStyle(reader_->appearance())); }

qreal HnAppearance::baseRadius() const { return reader_->appearance().base_radius.value_or(qQNaN()); }

qreal HnAppearance::baseChamfer() const { return reader_->appearance().base_chamfer.value_or(qQNaN()); }

void HnAppearance::reload() { reader_->reload(); }

qreal HnAppearance::roundedRadius(int role, qreal width, qreal height, int revision_dependency) const {
  Q_UNUSED(revision_dependency)
  return Holonight::ShapeResolver::resolve(surfaceRole(role), Holonight::CornerStyle::Rounded, QSizeF{width, height},
                                           qQNaN(), qQNaN(), shapeTokens(reader_->appearance()))
      .radius;
}

QVariantMap HnAppearance::resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                  qreal chamfer_override, int revision_dependency) const {
  return resolve(role, style, width, height, radius_override, chamfer_override, HnCornerMask::Inherit,
                 revision_dependency);
}

QVariantMap HnAppearance::resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                  qreal chamfer_override, int chamfered_corners_override,
                                  int revision_dependency) const {
  Q_UNUSED(revision_dependency)
  const Holonight::ResolvedShape shape = Holonight::ShapeResolver::resolve(
      surfaceRole(role),
      parsedCornerStyle(style) == Holonight::CornerStyle::Inherit ? globalCornerStyle(reader_->appearance())
                                                                  : parsedCornerStyle(style),
      QSizeF{width, height}, radius_override, chamfer_override, chamfered_corners_override,
      shapeTokens(reader_->appearance()));
  return {
      {QStringLiteral("kind"), static_cast<int>(shape.kind)},
      {QStringLiteral("radius"), shape.radius},
      {QStringLiteral("chamfer"), shape.chamfer},
      {QStringLiteral("corners"), shape.chamfered_corners.toInt()},
  };
}
