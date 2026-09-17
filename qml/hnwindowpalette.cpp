// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnwindowpalette.h"

#include "appearancecontext.h"
#include "holonight/appearance_reader.h"
#include "holonight/palette.h"
#include "themeresolver.h"

#include <QEvent>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QScopedValueRollback>
#include <QtQuick/private/qquickpalette_p.h>
#include <QtQuick/private/qquickwindow_p.h>

HnWindowPalette::HnWindowPalette(QObject* parent) : QObject(parent) {}

HnWindowPalette::~HnWindowPalette() {
  if (window_) {
    window_->removeEventFilter(this);
    disconnect(window_, nullptr, this, nullptr);
  }
  if (qGuiApp) qGuiApp->removeEventFilter(this);
}

QQuickWindow* HnWindowPalette::window() const { return window_; }

void HnWindowPalette::setWindow(QQuickWindow* window) {
  if (window_ == window) return;
  if (window_) {
    window_->removeEventFilter(this);
    disconnect(window_, nullptr, this, nullptr);
  }
  window_ = window;
  if (window_) {
    window_->installEventFilter(this);
    connect(window_, &QQuickWindow::paletteChanged, this, &HnWindowPalette::refresh);
    connect(window_, &QObject::destroyed, this, [this] { emit windowChanged(); });
  }
  refresh();
  emit windowChanged();
}

void HnWindowPalette::componentComplete() {
  reader_ = Holonight::appearanceReaderForEngine(qmlEngine(this));
  connect(reader_, &Holonight::AppearanceReader::paletteChanged, this, &HnWindowPalette::refresh);
  qGuiApp->installEventFilter(this);
  refresh();
}

bool HnWindowPalette::eventFilter(QObject* watched, QEvent* event) {
  if ((watched == qGuiApp || watched == window_) && event->type() == QEvent::ApplicationPaletteChange &&
      !refresh_queued_) {
    refresh_queued_ = true;
    // Qt may replace inherited defaults while delivering the application event.
    // Reapply after delivery; the QObject context cancels this on destruction.
    QMetaObject::invokeMethod(
        this,
        [this] {
          refresh_queued_ = false;
          refresh();
        },
        Qt::QueuedConnection);
  }
  return QObject::eventFilter(watched, event);
}

void HnWindowPalette::refresh() {
  if (refreshing_ || !window_ || !reader_) return;
  const QScopedValueRollback guard(refreshing_, true);
  const auto defaults = Holonight::buildPalette(Holonight::ThemeResolver::resolve(reader_->appearance()));
  const auto application = QGuiApplication::palette();
  auto inherited = application.resolve(defaults);
  // QPalette::resolve keeps the left-hand mask. These roles must participate in
  // inheritance, while remaining absent from the window's requested palette.
  inherited.setResolveMask(application.resolveMask() | defaults.resolveMask());
  // Inheritance keeps caller-owned roles and group masks intact, including reset.
  QQuickWindowPrivate::get(window_)->palette()->inheritPalette(inherited);
}
