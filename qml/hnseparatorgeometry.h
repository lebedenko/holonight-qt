// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QMetaObject>
#include <QQuickItem>
#include <QVector>
#include <QtQml/qqml.h>

class HnSeparatorGeometry : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
  Q_PROPERTY(
      int requestedThickness READ requestedThickness WRITE setRequestedThickness NOTIFY requestedThicknessChanged)
  Q_PROPERTY(
      int crossAxisAlignment READ crossAxisAlignment WRITE setCrossAxisAlignment NOTIFY crossAxisAlignmentChanged)
  Q_PROPERTY(qreal effectiveDevicePixelRatio READ effectiveDevicePixelRatio NOTIFY geometryChanged)
  Q_PROPERTY(qreal logicalThickness READ logicalThickness NOTIFY logicalThicknessChanged)
  Q_PROPERTY(QRectF paintedRect READ paintedRect NOTIFY geometryChanged)

 public:
  explicit HnSeparatorGeometry(QQuickItem* parent = nullptr);
  ~HnSeparatorGeometry() override = default;
  Q_DISABLE_COPY_MOVE(HnSeparatorGeometry)

  [[nodiscard]] int orientation() const { return orientation_; }
  void setOrientation(int orientation);

  [[nodiscard]] int requestedThickness() const { return requested_thickness_; }
  void setRequestedThickness(int thickness);

  [[nodiscard]] int crossAxisAlignment() const { return cross_axis_alignment_; }
  void setCrossAxisAlignment(int alignment);

  [[nodiscard]] qreal effectiveDevicePixelRatio() const { return effective_dpr_; }
  [[nodiscard]] qreal logicalThickness() const { return logical_thickness_; }
  [[nodiscard]] QRectF paintedRect() const { return painted_rect_; }

 signals:
  void orientationChanged();
  void requestedThicknessChanged();
  void crossAxisAlignmentChanged();
  void logicalThicknessChanged();
  void geometryChanged();

 private:
  void rebuildObservers();
  void updateGeometry();
  void observeItem(QQuickItem* item);
  void observeWindow(QQuickWindow* window);

  int orientation_ = Qt::Horizontal;
  int requested_thickness_ = 1;
  int cross_axis_alignment_ = 0;
  qreal effective_dpr_ = 1.0;
  qreal logical_thickness_ = 1.0;
  QRectF painted_rect_;
  QVector<QMetaObject::Connection> observer_connections_;
};
