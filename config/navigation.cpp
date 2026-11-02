// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#include "holonight/navigation.h"

#include <Qt>

#include <algorithm>
#include <limits>
namespace Holonight {
void NavigationInterpreter::reset() {
  count_ = 0;
  has_count_ = false;
  pending_g_ = false;
}
int NavigationInterpreter::takeCount() {
  const int result = has_count_ ? count_ : 1;
  count_ = 0;
  has_count_ = false;
  return result;
}
NavigationIntent NavigationInterpreter::route(const QString& key, bool vim, bool grid, bool browsing) {
  using C = NavigationCommand;
  if (vim_ != vim) {
    reset();
    vim_ = vim;
  }
  if (pending_g_ && sequence_.elapsed() > 600) pending_g_ = false;
  if (key == "Escape") {
    reset();
    return {};
  }
  if (vim && key.size() == 1 && key[0] >= u'0' && key[0] <= u'9') {
    count_ = int(std::min(qint64(std::numeric_limits<int>::max()), qint64(count_) * 10 + key[0].unicode() - u'0'));
    has_count_ = true;
    pending_g_ = false;
    return {C::Pending};
  }
  if (vim && key == "g") {
    if (pending_g_) {
      reset();
      return {C::First};
    }
    pending_g_ = true;
    sequence_.start();
    return {C::Pending};
  }
  pending_g_ = false;
  C command = C::None;
  if (key == "ArrowUp" || (vim && key == "k"))
    command = C::Previous;
  else if (key == "ArrowDown" || (vim && key == "j"))
    command = C::Next;
  else if (key == "Home" || (vim && key == "G"))
    command = key == "G" ? C::Last : C::First;
  else if (key == "End")
    command = C::Last;
  else if (key == "PageUp")
    command = C::PageUp;
  else if (key == "PageDown")
    command = C::PageDown;
  else if (vim && key == "Ctrl+U")
    command = C::HalfPageUp;
  else if (vim && key == "Ctrl+D")
    command = C::HalfPageDown;
  else if (vim && key == "Ctrl+H")
    command = C::PanelLeft;
  else if (vim && key == "Ctrl+J")
    command = C::PanelDown;
  else if (vim && key == "Ctrl+K")
    command = C::PanelUp;
  else if (vim && key == "Ctrl+L")
    command = C::PanelRight;
  else if (browsing && ((!vim && key == "Ctrl+H") || (vim && key == ".")))
    command = C::ToggleHidden;
  else if (browsing && (key == "Alt+D" || (!vim && key == "Ctrl+L")))
    command = C::EditLocation;
  else if (key == "ArrowLeft" || (vim && key == "h"))
    command = grid ? C::Left : C::Parent;
  else if (key == "ArrowRight" || (vim && key == "l"))
    command = grid ? C::Right : C::Open;
  if (command == C::None) {
    takeCount();
    return {};
  }
  const int count = takeCount();
  return {command, command == C::First || command == C::Last ? 1 : count};
}
QString NavigationInterpreter::keyName(int key, const QString& text, int modifiers) {
  if (modifiers == Qt::ControlModifier && key >= Qt::Key_A && key <= Qt::Key_Z) return "Ctrl+" + QString(QChar(key));
  if (modifiers == Qt::AltModifier && key == Qt::Key_D) return "Alt+D";
  if (modifiers != Qt::NoModifier && modifiers != Qt::ShiftModifier) return {};
  if (modifiers == Qt::ShiftModifier) return text == "G" ? text : QString{};
  switch (key) {
    case Qt::Key_Up:
      return "ArrowUp";
    case Qt::Key_Down:
      return "ArrowDown";
    case Qt::Key_Left:
      return "ArrowLeft";
    case Qt::Key_Right:
      return "ArrowRight";
    case Qt::Key_Home:
      return "Home";
    case Qt::Key_End:
      return "End";
    case Qt::Key_PageUp:
      return "PageUp";
    case Qt::Key_PageDown:
      return "PageDown";
    case Qt::Key_Escape:
      return "Escape";
    default:
      return text;
  }
}
int navigationTarget(int current, NavigationIntent intent, const NavigationAdapter& a) {
  using C = NavigationCommand;
  if (a.count <= 0) return -1;
  if (intent.command == C::First) return 0;
  if (intent.command == C::Last) return a.count - 1;
  if (current < 0) return 0;
  current = std::clamp(current, 0, a.count - 1);
  const auto clamp = [&](qint64 value) { return int(std::clamp(value, qint64{0}, qint64{a.count - 1})); };
  const auto command = intent.command;
  const qint64 count = std::max(0, intent.count);
  const int columns = std::max(1, a.columns);
  if (command == C::First) return 0;
  if (command == C::Last) return a.count - 1;
  if (command == C::Left) return clamp(qint64(current) - count);
  if (command == C::Right) return clamp(qint64(current) + count);
  if (command == C::Previous || command == C::Next) {
    const qint64 row = std::clamp(qint64(current / columns) + (command == C::Next ? count : -count), qint64{0},
                                  qint64{(a.count - 1) / columns});
    return clamp(row * columns + current % columns);
  }
  const bool down = command == C::PageDown || command == C::HalfPageDown;
  const bool half = command == C::HalfPageDown || command == C::HalfPageUp;
  if (down || command == C::PageUp || command == C::HalfPageUp) {
    if (count == 0) return current;
    const auto rect = [&](int i) { return a.rect ? a.rect(i) : QRectF(0, i / columns, 1, 1); };
    const qreal destination =
        rect(current).top() + (down ? 1 : -1) * std::max(qreal{1}, a.viewportHeight * (half ? .5 : 1)) * count;
    int target = current;
    // Traverse model geometry across sections, retaining the column in grids.
    while (true) {
      const int next = navigationTarget(target, {down ? C::Next : C::Previous, 1}, a);
      if (next == target) break;
      target = next;
      if (down ? rect(target).top() >= destination : rect(target).top() <= destination) break;
    }
    return target;
  }
  return current;
}
void NavigationCursor::setIndex(qint64 index) {
  index_ = ids_.isEmpty() ? -1 : int(std::clamp(index, qint64{0}, qint64{ids_.size() - 1}));
}
void NavigationCursor::reconcile(QVector<QByteArray> ids) {
  const auto previous = identity();
  const auto old = ids_;
  const int position = index_;
  ids_ = std::move(ids);
  if (!previous.isEmpty()) {
    const int found = int(ids_.indexOf(previous));
    if (found >= 0) {
      index_ = found;
      return;
    }
    // Prefer the next surviving neighbor, then the previous at equal distance.
    for (int distance = 1; distance < old.size(); ++distance) {
      for (int candidate : {position + distance, position - distance}) {
        if (candidate >= 0 && candidate < old.size()) {
          const int survivor = int(ids_.indexOf(old[candidate]));
          if (survivor >= 0) {
            index_ = survivor;
            return;
          }
        }
      }
    }
  }
  setIndex(position);
}
}  // namespace Holonight
