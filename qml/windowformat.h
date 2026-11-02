// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once

#include <QObject>
#include <QPointer>
#include <QWindow>
#include <QtQml/qqmlregistration.h>

// Applied while the QML window is constructed, before its native surface exists.
class WindowFormat : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QWindow* window READ window WRITE setWindow NOTIFY windowChanged FINAL)

 public:
  using QObject::QObject;
  QWindow* window() const { return window_; }
  void setWindow(QWindow* window) {
    if (window_ == window) {
      return;
    }
    window_ = window;
    if (window) {
      auto format = window->requestedFormat();
      format.setRedBufferSize(qMax(8, format.redBufferSize()));
      format.setGreenBufferSize(qMax(8, format.greenBufferSize()));
      format.setBlueBufferSize(qMax(8, format.blueBufferSize()));
      window->setFormat(format);
    }
    emit windowChanged();
  }

 signals:
  void windowChanged();

 private:
  QPointer<QWindow> window_;
};
