// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once
#include <QObject>

#include <holonight/config/input.h>
#include <holonight/document_watcher.h>
namespace Holonight {
class InputPreferences : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString diagnostics READ diagnostics NOTIFY diagnosticsChanged)
  Q_PROPERTY(bool vimNavigation READ vimNavigation NOTIFY profileChanged)
 public:
  explicit InputPreferences(QObject* parent = nullptr);
  explicit InputPreferences(QString path, QObject* parent = nullptr);
  bool vimNavigation() const { return profile_ == HoloNight::Config::InputProfile::VimNavigation; }
  QString diagnostics() const { return diagnostics_; }
  bool reload();
 signals:
  void profileChanged();
  void diagnosticsChanged();

 private:
  void setDiagnostics(const std::vector<HoloNight::Config::Diagnostic>& diagnostics);
  QString diagnostics_;
  DocumentWatcher watcher_;
  HoloNight::Config::InputProfile profile_ = HoloNight::Config::InputProfile::Standard;
};
}  // namespace Holonight
