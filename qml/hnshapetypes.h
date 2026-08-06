// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QObject>
#include <QtGlobal>
#include <QtQml/qqml.h>

namespace HnSurfaceRole {
Q_NAMESPACE
QML_NAMED_ELEMENT(HnSurfaceRole)

// QML enum namespaces require unscoped enumerators for HnSurfaceRole.Card syntax.
// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
enum Role : quint8 {
  Window,
  Panel,
  Popup,
  Card,
  Menu,
  Tooltip,
  Control,
  Pill,
  Hud,
  WorkspaceIndicator,
};
Q_ENUM_NS(Role)
}  // namespace HnSurfaceRole

namespace HnCornerStyle {
Q_NAMESPACE
QML_NAMED_ELEMENT(HnCornerStyle)

// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
enum Style : quint8 {
  Inherit,
  Hybrid,
  Rounded,
  Chamfered,
};
Q_ENUM_NS(Style)
}  // namespace HnCornerStyle

namespace HnShapeKind {
Q_NAMESPACE
QML_NAMED_ELEMENT(HnShapeKind)

// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
enum Kind : quint8 {
  Rounded,
  Chamfered,
  Hybrid,
  Pill,
};
Q_ENUM_NS(Kind)
}  // namespace HnShapeKind

namespace HnCornerMask {
Q_NAMESPACE
QML_NAMED_ELEMENT(HnCornerMask)

// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
enum Corner : int {
  Inherit = -1,
  None = 0,
  TopLeft = 1 << 0,
  TopRight = 1 << 1,
  BottomRight = 1 << 2,
  BottomLeft = 1 << 3,
  All = TopLeft | TopRight | BottomRight | BottomLeft,
};
Q_ENUM_NS(Corner)
}  // namespace HnCornerMask
