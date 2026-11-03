// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include <holonight/input.h>
#include <holonight/navigation.h>
class HnInputPreferences : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  Q_PROPERTY(QString diagnostics READ diagnostics NOTIFY diagnosticsChanged)
  Q_PROPERTY(bool vimNavigation READ vimNavigation NOTIFY profileChanged)
 public:
  explicit HnInputPreferences(QObject* parent = nullptr) : QObject(parent) {
    connect(&preferences_, &Holonight::InputPreferences::diagnosticsChanged, this,
            &HnInputPreferences::diagnosticsChanged);
    connect(&preferences_, &Holonight::InputPreferences::profileChanged, this, &HnInputPreferences::profileChanged);
  }
  QString diagnostics() const { return preferences_.diagnostics(); }
  bool vimNavigation() const { return preferences_.vimNavigation(); }
 signals:
  void profileChanged();
  void diagnosticsChanged();

 private:
  Holonight::InputPreferences preferences_;
};
// Instances belong to a collection focus scope. Consumers supply model geometry,
// bind active only to that scope, and execute non-movement intentions themselves.
class HnNavigation : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY cursorChanged)
  Q_PROPERTY(QByteArray currentIdentity READ currentIdentity NOTIFY cursorChanged)
  Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
  Q_PROPERTY(bool vimNavigation MEMBER vim_ WRITE setVim NOTIFY profileChanged)
 public:
  explicit HnNavigation(QObject* parent = nullptr) : QObject(parent) {}
  int currentIndex() const { return cursor_.index(); }
  QByteArray currentIdentity() const { return cursor_.identity(); }
  void setCurrentIndex(int value) {
    const int previous = cursor_.index();
    cursor_.setIndex(value);
    if (previous != cursor_.index()) emit cursorChanged();
  }
  Q_INVOKABLE void reconcile(const QVariantList& identities) {
    QVector<QByteArray> ids;
    for (const auto& id : identities) ids.append(id.toByteArray());
    const auto previousIdentity = cursor_.identity();
    const int previousIndex = cursor_.index();
    cursor_.reconcile(std::move(ids));
    if (previousIdentity != cursor_.identity() || previousIndex != cursor_.index()) emit cursorChanged();
  }
  bool active() const { return active_; }
  void setActive(bool value) {
    if (active_ != value) {
      active_ = value;
      reset();
      emit activeChanged();
    }
  }
  void setVim(bool value) {
    if (vim_ != value) {
      vim_ = value;
      reset();
      emit profileChanged();
    }
  }
  Q_INVOKABLE void reset() { interpreter_.reset(); }
  Q_INVOKABLE QVariantMap interpret(int key, const QString& text, int modifiers, bool grid = false,
                                    bool browsing = false) {
    if (!active_) {
      reset();
      return {};
    }
    const auto intent =
        interpreter_.route(Holonight::NavigationInterpreter::keyName(key, text, modifiers), vim_, grid, browsing);
    return {{"command", int(intent.command)}, {"count", intent.count}, {"consumed", intent.consumed()}};
  }
  Q_INVOKABLE int target(int current, const QVariantMap& intent, int count, int columns, qreal viewportHeight,
                         qreal rowHeight, const QVariantList& tops = {}) {
    Holonight::NavigationAdapter adapter{
        count, columns, viewportHeight, [=](int i) {
          return QRectF(0, i < tops.size() ? tops[i].toReal() : (i / qMax(1, columns)) * rowHeight, 1, rowHeight);
        }};
    return Holonight::navigationTarget(
        current, {Holonight::NavigationCommand(intent.value("command").toInt()), intent.value("count", 1).toInt()},
        adapter);
  }
  enum Command {
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
  Q_ENUM(Command)
 signals:
  void cursorChanged();
  void activeChanged();
  void profileChanged();

 private:
  Holonight::NavigationCursor cursor_;
  Holonight::NavigationInterpreter interpreter_;
  bool active_ = false;
  bool vim_ = false;
};
