// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/appearance.h"

#include <QFileSystemWatcher>
#include <QObject>
#include <QTimer>

namespace Holonight {

class AppearanceReader : public QObject {
  Q_OBJECT

 public:
  explicit AppearanceReader(QObject* parent = nullptr);
  explicit AppearanceReader(QString config_file, QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(AppearanceReader)

  [[nodiscard]] const ResolvedAppearance& appearance() const noexcept { return appearance_; }
  [[nodiscard]] const QVector<AppearanceDiagnostic>& diagnostics() const noexcept { return diagnostics_; }
  [[nodiscard]] QString configFile() const { return config_file_; }
  [[nodiscard]] int revision() const noexcept { return revision_; }

  Q_INVOKABLE bool reload();

 Q_SIGNALS:
  void appearanceChanged();
  void paletteChanged();
  void themeChanged();
  void shapeChanged();
  void diagnosticsChanged();

 private:
  void initialize();
  void scheduleReload();
  void rearmWatcher();
  void publishDiagnostics(QVector<AppearanceDiagnostic> diagnostics);

  ResolvedAppearance appearance_;
  QVector<AppearanceDiagnostic> diagnostics_;
  QString config_file_;
  QFileSystemWatcher watcher_;
  QTimer reload_timer_;
  int revision_ = 0;
};

}  // namespace Holonight
