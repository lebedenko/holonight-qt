// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

// Opt-in read-only observer. Never reads deferred popup/background/content properties.
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSet>
#include <QTimer>
#include <QWidget>
#include <QtQuick/private/qquickpalette_p.h>

namespace {
QString identity(QObject* object) {
  return QString::number(reinterpret_cast<quintptr>(object), 16) + ":" + object->metaObject()->className();
}
QJsonObject paletteState(const QPalette& palette) {
  QJsonObject result{{"resolveMask", QString::number(palette.resolveMask(), 16)}};
  const auto roles = QMetaEnum::fromType<QPalette::ColorRole>();
  for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    QJsonObject colors;
    for (int index = 0; index < QPalette::NColorRoles; ++index) {
      const auto role = static_cast<QPalette::ColorRole>(index);
      colors[QString::fromLatin1(roles.valueToKey(index))] = palette.color(group, role).name(QColor::HexArgb);
    }
    result[QString::number(group)] = colors;
  }
  return result;
}
class PaletteObserver : public QObject {
 public:
  explicit PaletteObserver(QObject* parent) : QObject(parent) {
    qApp->installEventFilter(this);
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this] { sample(); });
    timer->start(100);
  }
  bool eventFilter(QObject* object, QEvent* event) override {
    if (event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::PaletteChange) {
      emitRecord({{"id", identity(object)},
                  {"event", int(event->type())},
                  {"applicationPalette", paletteState(QGuiApplication::palette())}});
    }
    return false;
  }

 private:
  QHash<QString, QByteArray> previous_;
  static void emitRecord(QJsonObject state) {
    state["time_ms"] = QDateTime::currentMSecsSinceEpoch();
    qInfo().noquote() << "HN_PALETTE" << QJsonDocument(state).toJson(QJsonDocument::Compact);
  }
  void report(const QJsonObject& state) {
    const auto key = state["id"].toString();
    const auto bytes = QJsonDocument(state).toJson(QJsonDocument::Compact);
    if (previous_.value(key) == bytes) return;
    previous_[key] = bytes;
    emitRecord(state);
  }
  void walk(QObject* object, QSet<QObject*>& visited) {
    if (visited.contains(object)) return;
    visited.insert(object);
    QJsonObject state{{"id", identity(object)}};
    if (object->parent()) state["parent"] = identity(object->parent());
    if (auto* context = qmlContext(object)) state["origin"] = context->baseUrl().toString();
    if (auto* widget = qobject_cast<QWidget*>(object)) {
      state["palette"] = paletteState(widget->palette());
      state["widgetPaletteExplicit"] = widget->testAttribute(Qt::WA_SetPalette);
      state["backend"] = "QWidget";
    } else if (object->metaObject()->indexOfProperty("palette") >= 0) {
      // palette getters do not execute deferred control implementations. Read the
      // resolved QPalette without creating active/inactive/disabled group objects.
      if (auto* palette = object->property("palette").value<QQuickPalette*>())
        state["palette"] = paletteState(palette->toQPalette());
    }
    for (const auto* name : {"visible", "enabled", "color", "currentIndex"}) {
      if (object->metaObject()->indexOfProperty(name) >= 0) {
        const auto value = object->property(name);
        state[name] = value.metaType() == QMetaType::fromType<QColor>()
                          ? QJsonValue(value.value<QColor>().name(QColor::HexArgb))
                          : QJsonValue::fromVariant(value);
      }
    }
    if (auto* window = qobject_cast<QWindow*>(object)) state["dpr"] = window->devicePixelRatio();
    if (state.contains("palette") || state.contains("color") ||
        QByteArray(object->metaObject()->className()).contains("Dialog"))
      report(state);
    for (auto* child : object->children()) walk(child, visited);
    if (auto* item = qobject_cast<QQuickItem*>(object))
      for (auto* child : item->childItems()) walk(child, visited);
  }
  void sample() {
    report({{"id", "application"}, {"palette", paletteState(QGuiApplication::palette())}});
    QSet<QObject*> visited;
    for (auto* window : QGuiApplication::allWindows()) walk(window, visited);
    if (qobject_cast<QApplication*>(QCoreApplication::instance()))
      for (auto* widget : QApplication::topLevelWidgets()) walk(widget, visited);
  }
};
void startPaletteObserver() {
  if (!qEnvironmentVariableIsSet("HOLONIGHT_PALETTE_DIAGNOSTICS")) return;
  QMetaObject::invokeMethod(qApp, [] { new PaletteObserver(qApp); }, Qt::QueuedConnection);
}
Q_COREAPP_STARTUP_FUNCTION(startPaletteObserver)
}  // namespace
