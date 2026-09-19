// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once

#include <QKeySequence>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

// Private fallback for the full Qt key vocabulary (media keys, function keys, Unicode).
class KeyHintNames : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

 public:
  explicit KeyHintNames(QObject* parent = nullptr) : QObject(parent) {}

  [[nodiscard]] Q_INVOKABLE static QString keyName(int key) {
    const auto name = QKeySequence(key).toString(QKeySequence::NativeText);
    return name.isEmpty() ? tr("Key %1").arg(key) : name;
  }
};
