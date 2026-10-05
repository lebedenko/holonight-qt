// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "hnwindowdecoration.h"

// Snapshot of the private platform boundary; no protocol ownership.
namespace Holonight::Private {
struct WindowDecorationState {
  bool supported = false;
  bool configured = false;
  bool undecorated = false;
  bool toolkit_decoration = false;
  bool wants_decoration = false;
};
HnWindowDecoration::Mode decorationMode(const WindowDecorationState& state);
WindowDecorationState windowDecorationState(QQuickWindow* window);
}  // namespace Holonight::Private
