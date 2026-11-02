// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#include "holonight/input.h"
namespace Holonight {
namespace {
QString inputPath() {
  const auto path = HoloNight::Config::resolveInputPath();
  return path ? QString::fromStdString(path.value->string()) : QString{};
}
}  // namespace
InputPreferences::InputPreferences(QObject* parent) : InputPreferences(inputPath(), parent) {}
InputPreferences::InputPreferences(QString path, QObject* parent) : QObject(parent), watcher_(std::move(path)) {
  connect(&watcher_, &DocumentWatcher::documentChanged, this, &InputPreferences::reload);
  reload();
}
bool InputPreferences::reload() {
  const auto document = watcher_.read();
  watcher_.refresh();
  if (!document) return false;
  const auto decoded = HoloNight::Config::decodeInputDocument(*document.value);
  if (!decoded) return false;
  if (profile_ != *decoded.value) {
    profile_ = *decoded.value;
    emit profileChanged();
  }
  return true;
}
}  // namespace Holonight
