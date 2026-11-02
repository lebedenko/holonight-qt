// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once
#include <QObject>

#include <holonight/config/input.h>
#include <holonight/document_watcher.h>
namespace Holonight {
class InputPreferences : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool vimNavigation READ vimNavigation NOTIFY profileChanged)
 public:
  explicit InputPreferences(QObject* parent = nullptr);
  explicit InputPreferences(QString path, QObject* parent = nullptr);
  bool vimNavigation() const { return profile_ == HoloNight::Config::InputProfile::VimNavigation; }
  bool reload();
 signals:
  void profileChanged();

 private:
  DocumentWatcher watcher_;
  HoloNight::Config::InputProfile profile_ = HoloNight::Config::InputProfile::Standard;
};
}  // namespace Holonight
