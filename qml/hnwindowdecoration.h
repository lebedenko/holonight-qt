// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QObject>
#include <QPointer>
#include <QQuickWindow>
#include <QtQml/qqmlregistration.h>

#include <cstdint>

namespace Holonight::Private {
struct WindowDecorationState;
}
class HnWindowDecorationTestPeer;
class HnWindowDecoration : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QQuickWindow* window READ window WRITE setWindow NOTIFY windowChanged FINAL)
  Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged FINAL)
  Q_PROPERTY(
      bool externalDecorationPresent READ externalDecorationPresent NOTIFY externalDecorationPresentChanged FINAL)

 public:
  enum class Mode : std::uint8_t { Unknown, ServerSide, ToolkitClientSide, Undecorated };
  Q_ENUM(Mode)
  explicit HnWindowDecoration(QObject* parent = nullptr);
  ~HnWindowDecoration() override;
  Q_DISABLE_COPY_MOVE(HnWindowDecoration)
  [[nodiscard]] QQuickWindow* window() const { return window_; }
  void setWindow(QQuickWindow* window);
  [[nodiscard]] Mode mode() const { return mode_; }
  [[nodiscard]] bool externalDecorationPresent() const {
    return mode_ == Mode::ServerSide || mode_ == Mode::ToolkitClientSide;
  }

 signals:
  void windowChanged();
  void modeChanged();
  void externalDecorationPresentChanged();

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  friend class HnWindowDecorationTestPeer;
  void refresh();
  void setMode(Mode mode);
  Holonight::Private::WindowDecorationState (*probe_)(QQuickWindow*);
  QPointer<QQuickWindow> window_;
  Mode mode_ = Mode::Unknown;
  bool surface_destroying_ = false;
};
