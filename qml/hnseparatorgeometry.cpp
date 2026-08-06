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

[[nodiscard]] bool differs(qreal lhs, qreal rhs) { return std::abs(lhs - rhs) > kGeometryTolerance; }

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
  if (parentItem() != nullptr) {
    const QPointF scene_origin = parentItem()->mapToScene(QPointF{});
    scene_coordinate = orientation_ == Qt::Vertical ? scene_origin.x() : scene_origin.y();
  }
  const Holonight::SeparatorAlignment alignment =
      Holonight::separatorAlignment(requested_thickness_, standard_thickness_,
                                    window() != nullptr ? window()->devicePixelRatio() : 1.0, scene_coordinate);

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
