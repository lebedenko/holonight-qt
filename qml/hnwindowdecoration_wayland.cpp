// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnwindowdecoration_p.h"

#include <QGuiApplication>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include <any>

struct xdg_toplevel;

namespace Holonight::Private {
WindowDecorationState windowDecorationState(QQuickWindow* window) {
  if (!QGuiApplication::platformName().startsWith(QStringLiteral("wayland")) || (window->handle() == nullptr)) {
    return {};
  }
  auto* native = dynamic_cast<QtWaylandClient::QWaylandWindow*>(window->handle());
  if (native == nullptr) {
    return {};
  }
  auto* shell = native->shellSurface();
  if (shell == nullptr) {
    return {};
  }
  const auto role = shell->surfaceRole();
  const auto* toplevel = std::any_cast<xdg_toplevel*>(&role);
  if ((toplevel == nullptr) || ((*toplevel) == nullptr)) {
    return {};
  }
  return {
      .supported = true,
      .configured = shell->isExposed(),
      .undecorated =
          window->flags().testFlag(Qt::FramelessWindowHint) || native->windowStates().testFlag(Qt::WindowFullScreen),
      .toolkit_decoration = native->decoration() != nullptr,
      .wants_decoration = shell->wantsDecorations(),
  };
}
}  // namespace Holonight::Private
