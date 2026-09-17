// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>
#include <QQuickWindow>
#include <QtQml/qqmlregistration.h>

namespace Holonight {
class AppearanceReader;
}

// Implementation support for HnApplicationWindow, not a palette-selection API.
class HnWindowPalette : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QQuickWindow* window READ window WRITE setWindow NOTIFY windowChanged FINAL)

 public:
  explicit HnWindowPalette(QObject* parent = nullptr);
  ~HnWindowPalette() override;
  [[nodiscard]] QQuickWindow* window() const;
  void setWindow(QQuickWindow* window);
  void classBegin() override {}
  void componentComplete() override;

 signals:
  void windowChanged();

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  void refresh();
  QPointer<QQuickWindow> window_;
  QPointer<Holonight::AppearanceReader> reader_;
  bool refreshing_ = false;
  bool refresh_queued_ = false;
};
