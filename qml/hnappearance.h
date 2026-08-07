// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/appearance_reader.h"

#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <QVariantMap>
#include <QtQml/qqml.h>

class HnAppearance : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT
  Q_PROPERTY(QString configFile READ configFile CONSTANT)
  Q_PROPERTY(int cornerStyle READ cornerStyle NOTIFY appearanceChanged)
  Q_PROPERTY(qreal shapeScale READ shapeScale NOTIFY appearanceChanged)
  Q_PROPERTY(qreal baseRadius READ baseRadius NOTIFY appearanceChanged)
  Q_PROPERTY(qreal baseChamfer READ baseChamfer NOTIFY appearanceChanged)
  Q_PROPERTY(int revision READ revision NOTIFY appearanceChanged)

 public:
  ~HnAppearance() override = default;
  Q_DISABLE_COPY_MOVE(HnAppearance)
  [[nodiscard]] static HnAppearance* create(QQmlEngine* engine, QJSEngine* script_engine);

  [[nodiscard]] QString configFile() const { return reader_->configFile(); }
  [[nodiscard]] int cornerStyle() const;
  [[nodiscard]] qreal shapeScale() const { return reader_->appearance().shape_scale; }
  [[nodiscard]] qreal baseRadius() const;
  [[nodiscard]] qreal baseChamfer() const;
  [[nodiscard]] int revision() const { return reader_->revision(); }

  Q_INVOKABLE void reload();
  [[nodiscard]] Q_INVOKABLE qreal roundedRadius(int role, qreal width, qreal height, int revision_dependency = 0) const;
  [[nodiscard]] Q_INVOKABLE QVariantMap resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                                qreal chamfer_override, int revision_dependency = 0) const;
  [[nodiscard]] Q_INVOKABLE QVariantMap resolve(int role, int style, qreal width, qreal height, qreal radius_override,
                                                qreal chamfer_override, int chamfered_corners_override,
                                                int revision_dependency) const;

 signals:
  void appearanceChanged();

 private:
  explicit HnAppearance(Holonight::AppearanceReader* reader, QObject* parent);

  Holonight::AppearanceReader* reader_ = nullptr;
};
