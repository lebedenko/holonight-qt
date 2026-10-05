// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once

#include <QObject>
#include <QTimer>

#include <holonight/config/document.h>
#include <memory>

class QFileSystemWatcher;

namespace Holonight {
// Watches replacement/deletion/recreation, including initially missing parents.
// Domain owners decide whether a changed document is valid/effectively different.
class DocumentWatcher : public QObject {
  Q_OBJECT
 public:
  explicit DocumentWatcher(QString path, QObject* parent = nullptr);
  ~DocumentWatcher() override;
  Q_DISABLE_COPY_MOVE(DocumentWatcher)
  [[nodiscard]] QString path() const { return path_; }
  [[nodiscard]] HoloNight::Config::Result<HoloNight::Config::DocumentSnapshot> read() const;
  void refresh();
 Q_SIGNALS:
  void documentChanged();

 private:
  void initializeWatcher();
  void scheduleReload();
  QString path_;
  std::unique_ptr<QFileSystemWatcher> watcher_;
  QTimer timer_;
};
}  // namespace Holonight
