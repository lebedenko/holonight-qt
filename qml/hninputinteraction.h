// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once

#include <QObject>
#include <QPointF>
#include <QPointer>
#include <QtQml/qqml.h>
#include <QtQml/qqmlregistration.h>

class QQuickWindow;
class HnWindowInputState;

class HnInputInteraction : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("HnInputInteraction is an attached property")
  QML_ATTACHED(HnInputInteraction)
  Q_PROPERTY(bool hoverAllowed READ hoverAllowed NOTIFY hoverAllowedChanged FINAL)

 public:
  explicit HnInputInteraction(QObject* parent = nullptr);
  static HnInputInteraction* qmlAttachedProperties(QObject* object);
  [[nodiscard]] bool hoverAllowed() const;
  Q_INVOKABLE void suppressHover();

 signals:
  void hoverAllowedChanged();
  void pointerMoved(QPointF scenePosition);

 private:
  void setWindow(QQuickWindow* window);
  QPointer<HnWindowInputState> state_;
  bool pending_suppression_ = false;
};

QML_DECLARE_TYPEINFO(HnInputInteraction, QML_HAS_ATTACHED_PROPERTIES)
