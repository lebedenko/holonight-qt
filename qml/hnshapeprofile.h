// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QObject>
#include <QVariantMap>
#include <QtQml/qqml.h>

class HnShapeProfile : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

 public:
  explicit HnShapeProfile(QObject* parent = nullptr);
  ~HnShapeProfile() override = default;
  Q_DISABLE_COPY_MOVE(HnShapeProfile)

  [[nodiscard]] Q_INVOKABLE QVariantMap resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                                qreal chamfer_override) const;
  [[nodiscard]] Q_INVOKABLE QVariantMap resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                                qreal chamfer_override, int chamfered_corners_override) const;
};
