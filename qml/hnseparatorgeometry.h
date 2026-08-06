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
      qreal requestedThickness READ requestedThickness WRITE setRequestedThickness NOTIFY requestedThicknessChanged)
  Q_PROPERTY(qreal standardThickness READ standardThickness WRITE setStandardThickness NOTIFY standardThicknessChanged)
  Q_PROPERTY(qreal effectiveDevicePixelRatio READ effectiveDevicePixelRatio NOTIFY geometryChanged)
  Q_PROPERTY(qreal paintedThickness READ paintedThickness NOTIFY geometryChanged)
  Q_PROPERTY(qreal paintedOffset READ paintedOffset NOTIFY geometryChanged)

 public:
  explicit HnSeparatorGeometry(QQuickItem* parent = nullptr);
  ~HnSeparatorGeometry() override = default;
  Q_DISABLE_COPY_MOVE(HnSeparatorGeometry)

  [[nodiscard]] int orientation() const { return orientation_; }
  void setOrientation(int orientation);

  [[nodiscard]] qreal requestedThickness() const { return requested_thickness_; }
  void setRequestedThickness(qreal thickness);

  [[nodiscard]] qreal standardThickness() const { return standard_thickness_; }
  void setStandardThickness(qreal thickness);

  [[nodiscard]] qreal effectiveDevicePixelRatio() const { return effective_dpr_; }
  [[nodiscard]] qreal paintedThickness() const { return painted_thickness_; }
  [[nodiscard]] qreal paintedOffset() const { return painted_offset_; }

 signals:
  void orientationChanged();
  void requestedThicknessChanged();
  void standardThicknessChanged();
  void geometryChanged();

 private:
  void rebuildObservers();
  void updateGeometry();
  void observeItem(QQuickItem* item);
  void observeWindow(QQuickWindow* window);

  int orientation_ = Qt::Horizontal;
  qreal requested_thickness_ = 1.0;
  qreal standard_thickness_ = 1.0;
  qreal effective_dpr_ = 1.0;
  qreal painted_thickness_ = 1.0;
  qreal painted_offset_ = 0.0;
  QVector<QMetaObject::Connection> observer_connections_;
};
