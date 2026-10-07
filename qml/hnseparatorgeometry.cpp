// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnseparatorgeometry.h"

#include "hnseparatoralignment.h"

#include <QQuickWindow>
#include <QScopedValueRollback>
#include <QScreen>

#include <cmath>
#include <utility>

namespace {

constexpr qreal kGeometryTolerance = 0.000001;

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

void HnSeparatorGeometry::setRequestedThickness(int thickness) {
  if (requested_thickness_ == thickness) {
    return;
  }
  requested_thickness_ = thickness;
  emit requestedThicknessChanged();
  updateGeometry();
}

void HnSeparatorGeometry::setCrossAxisAlignment(int alignment) {
  if (cross_axis_alignment_ == alignment) {
    return;
  }
  cross_axis_alignment_ = alignment;
  emit crossAxisAlignmentChanged();
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
      connect(window, &QQuickWindow::devicePixelRatioChanged, this, &HnSeparatorGeometry::updateGeometry));
  observer_connections_.append(
      connect(window, &QWindow::screenChanged, this, [this](QScreen*) { rebuildObservers(); }));
  if (window->screen() != nullptr) {
    observer_connections_.append(
        connect(window->screen(), &QScreen::logicalDotsPerInchChanged, this, [this](qreal) { updateGeometry(); }));
  }
}

void HnSeparatorGeometry::updateGeometry() {
  // Reading the parent's geometry can evaluate its implicit-size bindings.
  // Finish that evaluation before notifying a new logical thickness.
  if (updating_geometry_) {
    QMetaObject::invokeMethod(this, &HnSeparatorGeometry::updateGeometry, Qt::QueuedConnection);
    return;
  }
  const QScopedValueRollback<bool> updating{updating_geometry_, true};
  const qreal dpr = window() != nullptr ? window()->effectiveDevicePixelRatio() : 1.0;
  QPointF origin;
  QPointF scale;
  QRectF slot;
  if (parentItem() != nullptr) {
    origin = parentItem()->mapToScene(QPointF{});
    const QPointF x_axis = parentItem()->mapToScene(QPointF{1, 0}) - origin;
    const QPointF y_axis = parentItem()->mapToScene(QPointF{0, 1}) - origin;
    // Arbitrary rotation/shear/custom transforms are outside the crispness contract.
    if (std::abs(x_axis.y()) <= kGeometryTolerance && std::abs(y_axis.x()) <= kGeometryTolerance) {
      scale = QPointF(x_axis.x(), y_axis.y());
    }
  }
  const qreal minor_scale = orientation_ == Qt::Vertical ? scale.x() : scale.y();
  qreal logical_thickness = requested_thickness_ > 0 ? requested_thickness_ / (dpr * std::abs(minor_scale)) : 0;
  if (!std::isfinite(logical_thickness) || dpr <= 0) {
    logical_thickness = 0;
  }
  if (logical_thickness_ != logical_thickness) {
    logical_thickness_ = logical_thickness;
    emit logicalThicknessChanged();
  }
  // The thickness notification may resize the slot. Paint against its new bounds.
  if (parentItem() != nullptr) {
    origin = parentItem()->mapToScene(QPointF{});
    slot = parentItem()->boundingRect();
  }
  const auto alignment = cross_axis_alignment_ >= 0 && cross_axis_alignment_ <= 2
                             ? static_cast<Holonight::SeparatorCrossAlignment>(cross_axis_alignment_)
                             : Holonight::SeparatorCrossAlignment::Leading;
  const QRectF rectangle = Holonight::separatorRectangle(slot, origin, scale, dpr, requested_thickness_,
                                                         orientation_ == Qt::Vertical, alignment);
  if (effective_dpr_ == dpr && painted_rect_ == rectangle) {
    return;
  }
  effective_dpr_ = dpr;
  painted_rect_ = rectangle;
  emit geometryChanged();
}
