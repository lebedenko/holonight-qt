// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnwindowdecoration.h"

#include "hnwindowdecoration_p.h"

#include <QAbstractEventDispatcher>
#include <QEvent>
#include <QPlatformSurfaceEvent>

namespace Holonight::Private {
HnWindowDecoration::Mode decorationMode(const WindowDecorationState& state) {
  if (!state.supported) {
    return HnWindowDecoration::Mode::Unknown;
  }
  if (state.undecorated) {
    return HnWindowDecoration::Mode::Undecorated;
  }
  if (!state.configured) {
    return HnWindowDecoration::Mode::Unknown;
  }
  if (state.toolkit_decoration) {
    return HnWindowDecoration::Mode::ToolkitClientSide;
  }
  return state.wants_decoration ? HnWindowDecoration::Mode::Undecorated : HnWindowDecoration::Mode::ServerSide;
}
#ifndef HOLONIGHT_WINDOW_DECORATION_WAYLAND
WindowDecorationState windowDecorationState(QQuickWindow* /*unused*/) { return {}; }
#endif
}  // namespace Holonight::Private

HnWindowDecoration::HnWindowDecoration(QObject* parent)
    : QObject(parent), probe_(Holonight::Private::windowDecorationState) {
  if (auto* dispatcher = QAbstractEventDispatcher::instance()) {
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &HnWindowDecoration::refresh);
    connect(dispatcher, &QAbstractEventDispatcher::aboutToBlock, this, &HnWindowDecoration::refresh);
  }
}

HnWindowDecoration::~HnWindowDecoration() {
  if (window_) {
    window_->removeEventFilter(this);
  }
}

void HnWindowDecoration::setWindow(QQuickWindow* window) {
  if (window_ == window) {
    return;
  }
  if (window_) {
    window_->removeEventFilter(this);
    disconnect(window_, nullptr, this, nullptr);
  }
  window_ = window;
  surface_destroying_ = false;
  setMode(Mode::Unknown);
  if (window_) {
    window_->installEventFilter(this);
    connect(window_, &QWindow::visibilityChanged, this, &HnWindowDecoration::refresh);
    connect(window_, &QWindow::windowStateChanged, this, &HnWindowDecoration::refresh);
    connect(window_, &QObject::destroyed, this, [this] {
      window_ = nullptr;
      setMode(Mode::Unknown);
      emit windowChanged();
    });
  }
  refresh();
  emit windowChanged();
}

bool HnWindowDecoration::eventFilter(QObject* watched, QEvent* event) {
  if (watched == window_) {
    if (event->type() == QEvent::PlatformSurface) {
      const auto* surface = dynamic_cast<QPlatformSurfaceEvent*>(event);
      surface_destroying_ = surface->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed;
      if (surface_destroying_) {
        setMode(Mode::Unknown);
      }
    }
    if (event->type() == QEvent::PlatformSurface || event->type() == QEvent::Expose || event->type() == QEvent::Show ||
        event->type() == QEvent::Hide || event->type() == QEvent::WindowStateChange) {
      // Inspect after Qt has applied the event. The context cancels stale callbacks.
      QMetaObject::invokeMethod(this, &HnWindowDecoration::refresh, Qt::QueuedConnection);
    }
  }
  return QObject::eventFilter(watched, event);
}

void HnWindowDecoration::refresh() {
  setMode(window_ && !surface_destroying_ ? Holonight::Private::decorationMode(probe_(window_)) : Mode::Unknown);
}

void HnWindowDecoration::setMode(Mode mode) {
  if (mode_ == mode) {
    return;
  }
  const bool previous = externalDecorationPresent();
  mode_ = mode;
  emit modeChanged();
  if (previous != externalDecorationPresent()) {
    emit externalDecorationPresentChanged();
  }
}
