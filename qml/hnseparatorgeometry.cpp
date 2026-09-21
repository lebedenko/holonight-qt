// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnseparatorgeometry.h"

#include "hnseparatoralignment.h"

#include <QQuickWindow>
#include <QScreen>

#include <cmath>
#include <utility>

namespace {

constexpr qreal kGeometryTolerance = 0.000001;

[[nodiscard]] bool differs(qreal lhs, qreal rhs) {
  // A transition to/from zero must always update painting visibility, even at very large scales.
  return lhs == 0.0 || rhs == 0.0 ? lhs != rhs : std::abs(lhs - rhs) > kGeometryTolerance;
}

}  // namespace

HnSeparatorGeometry::HnSeparatorGeometry(QQuickItem* parent) : QQuickItem{parent} {
  setVisible(false);
  connect(this, &QQuickItem::parentChanged, this, &HnSeparatorGeometry::rebuildObservers);
  connect(this, &QQuickItem::windowChanged, this, &HnSeparatorGeometry::rebuildObservers);
  rebuildObservers();
}

void HnSeparatorGeometry::setOrientation(int orientation) {
  if (orientation_ == orientation) {
    return;
  }
  orientation_ = orientation;
  emit orientationChanged();
  updateGeometry();
}

void HnSeparatorGeometry::setRequestedThickness(qreal thickness) {
  if (requested_thickness_ == thickness) {
    return;
  }
  requested_thickness_ = thickness;
  emit requestedThicknessChanged();
  updateGeometry();
}

void HnSeparatorGeometry::setStandardThickness(qreal thickness) {
  if (standard_thickness_ == thickness) {
    return;
  }
  standard_thickness_ = thickness;
  emit standardThicknessChanged();
  updateGeometry();
}

void HnSeparatorGeometry::rebuildObservers() {
  for (const QMetaObject::Connection& connection : std::as_const(observer_connections_)) {
    disconnect(connection);
  }
  observer_connections_.clear();

  for (QQuickItem* item = parentItem(); item != nullptr; item = item->parentItem()) {
    observeItem(item);
  }
  observeWindow(window());
  updateGeometry();
}

void HnSeparatorGeometry::observeItem(QQuickItem* item) {
  const auto update = [this] { updateGeometry(); };
  observer_connections_.append(connect(item, &QQuickItem::xChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::yChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::widthChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::heightChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::rotationChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::scaleChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::transformOriginChanged, this, update));
  observer_connections_.append(connect(item, &QQuickItem::parentChanged, this, &HnSeparatorGeometry::rebuildObservers));
}

void HnSeparatorGeometry::observeWindow(QQuickWindow* window) {
  if (window == nullptr) {
    return;
  }

  observer_connections_.append(
      connect(window, &QWindow::screenChanged, this, [this](QScreen*) { rebuildObservers(); }));
  if (window->screen() != nullptr) {
    observer_connections_.append(
        connect(window->screen(), &QScreen::logicalDotsPerInchChanged, this, [this](qreal) { updateGeometry(); }));
  }
}

void HnSeparatorGeometry::updateGeometry() {
  qreal scene_coordinate = 0.0;
  qreal minor_axis_scale = 1.0;
  if (parentItem() != nullptr) {
    const QPointF scene_origin = parentItem()->mapToScene(QPointF{});
    const QPointF x_axis = parentItem()->mapToScene(QPointF{1, 0}) - scene_origin;
    const QPointF y_axis = parentItem()->mapToScene(QPointF{0, 1}) - scene_origin;
    if (!std::isfinite(scene_origin.x()) || !std::isfinite(scene_origin.y()) || !std::isfinite(x_axis.x()) ||
        !std::isfinite(x_axis.y()) || !std::isfinite(y_axis.x()) || !std::isfinite(y_axis.y())) {
      minor_axis_scale = 0.0;
    } else if (std::abs(x_axis.y()) <= kGeometryTolerance && std::abs(y_axis.x()) <= kGeometryTolerance) {
      minor_axis_scale = orientation_ == Qt::Vertical ? x_axis.x() : y_axis.y();
    }
    // Preserve the previous rendering for rotation/shear; pixel alignment is not guaranteed there.
    scene_coordinate = orientation_ == Qt::Vertical ? scene_origin.x() : scene_origin.y();
  }
  const Holonight::SeparatorAlignment alignment = Holonight::separatorAlignment(
      requested_thickness_, standard_thickness_, window() != nullptr ? window()->devicePixelRatio() : 1.0,
      scene_coordinate, minor_axis_scale);

  if (!differs(effective_dpr_, alignment.device_pixel_ratio) &&
      !differs(painted_thickness_, alignment.painted_thickness) &&
      !differs(painted_offset_, alignment.painted_offset)) {
    return;
  }

  effective_dpr_ = alignment.device_pixel_ratio;
  painted_thickness_ = alignment.painted_thickness;
  painted_offset_ = alignment.painted_offset;
  emit geometryChanged();
}
