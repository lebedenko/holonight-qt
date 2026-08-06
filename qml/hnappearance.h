// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "appearanceconfig.h"

#include <QFileSystemWatcher>
#include <QObject>
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
  explicit HnAppearance(QObject* parent = nullptr);
  ~HnAppearance() override = default;
  Q_DISABLE_COPY_MOVE(HnAppearance)

  [[nodiscard]] QString configFile() const { return config_file_; }
  [[nodiscard]] int cornerStyle() const { return static_cast<int>(config_.corner_style); }
  [[nodiscard]] qreal shapeScale() const { return config_.shape_scale; }
  [[nodiscard]] qreal baseRadius() const { return config_.base_radius; }
  [[nodiscard]] qreal baseChamfer() const { return config_.base_chamfer; }
  [[nodiscard]] int revision() const { return revision_; }

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
  void armWatch();
  void onPathChanged(const QString& path);

  Holonight::AppearanceConfig config_;
  QString config_file_;
  QString config_directory_;
  QString config_parent_directory_;
  QFileSystemWatcher watcher_;
  int revision_ = 0;
};
