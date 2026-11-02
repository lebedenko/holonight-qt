// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once
#include <QElapsedTimer>
#include <QRectF>
#include <QString>
#include <QVector>

#include <functional>
namespace Holonight {
enum class NavigationCommand {
  None,
  Pending,
  Previous,
  Next,
  Left,
  Right,
  First,
  Last,
  PageUp,
  PageDown,
  HalfPageUp,
  HalfPageDown,
  Parent,
  Open,
  ToggleHidden,
  EditLocation,
  PanelLeft,
  PanelDown,
  PanelUp,
  PanelRight
};
struct NavigationIntent {
  NavigationCommand command = NavigationCommand::None;
  int count = 1;
  bool consumed() const { return command != NavigationCommand::None; }
};
// One interpreter per focus scope. Key names preserve case and encode modifiers;
// the Qt event adapter below prevents modified printable keys leaking into motions.
class NavigationInterpreter {
 public:
  NavigationIntent route(const QString& key, bool vim, bool grid = false, bool browsing = false);
  void reset();
  int takeCount();
  static QString keyName(int key, const QString& text, int modifiers);

 private:
  int count_ = 0;
  bool has_count_ = false;
  bool pending_g_ = false;
  bool vim_ = false;
  QElapsedTimer sequence_;
};
// Geometry comes from model/layout metadata, never from instantiated delegates.
// Identities can be native path bytes, device IDs, or application-owned stable keys.
struct NavigationAdapter {
  int count = 0;
  int columns = 1;
  qreal viewportHeight = 1;
  std::function<QRectF(int)> rect;
};
int navigationTarget(int current, NavigationIntent intent, const NavigationAdapter& adapter);
class NavigationCursor {
 public:
  int index() const { return index_; }
  QByteArray identity() const { return index_ >= 0 && index_ < ids_.size() ? ids_[index_] : QByteArray{}; }
  void reconcile(QVector<QByteArray> ids);
  void setIndex(qint64 index);
  void clear() { index_ = -1; }
  void move(NavigationIntent intent, const NavigationAdapter& adapter) {
    setIndex(navigationTarget(index_, intent, adapter));
  }

 private:
  QVector<QByteArray> ids_;
  int index_ = -1;
};
}  // namespace Holonight
