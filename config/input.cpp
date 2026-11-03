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
void InputPreferences::setDiagnostics(const std::vector<HoloNight::Config::Diagnostic>& diagnostics) {
  QStringList messages;
  for (const auto& diagnostic : diagnostics) messages << QString::fromStdString(diagnostic.message);
  const auto text = messages.join(QLatin1Char('\n'));
  if (diagnostics_ == text) return;
  diagnostics_ = text;
  emit diagnosticsChanged();
}
bool InputPreferences::reload() {
  const auto document = watcher_.read();
  watcher_.refresh();
  if (!document) {
    setDiagnostics(document.diagnostics);
    return false;
  }
  const auto decoded = HoloNight::Config::decodeInputDocument(*document.value);
  setDiagnostics(decoded.diagnostics);
  if (!decoded) return false;
  if (profile_ != *decoded.value) {
    profile_ = *decoded.value;
    emit profileChanged();
  }
  return true;
}
}  // namespace Holonight
