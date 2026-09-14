// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

// Opt-in LD_PRELOAD observer for human-operated acceptance. Never changes focus/input.
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QPointer>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>

namespace {
QString identity(QObject* object) {
  return object ? QString::number(reinterpret_cast<quintptr>(object), 16) + ":" + object->metaObject()->className() +
                      ":" + object->objectName()
                : QStringLiteral("null");
}

class Observer : public QObject {
 public:
  explicit Observer(QObject* parent) : QObject(parent) {
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this] { sample("state"); });
    timer->start(50);
    qApp->installEventFilter(this);
  }

  bool eventFilter(QObject* object, QEvent* event) override {
    if (qobject_cast<QQuickWindow*>(object) &&
        (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) &&
        static_cast<QKeyEvent*>(event)->key() == Qt::Key_Space) {
      sample(event->type() == QEvent::KeyPress ? "space-before-press" : "space-before-release", true);
      QTimer::singleShot(0, this, [this] { sample("space-after-delivery", true); });
    }
    return false;
  }

 private:
  QHash<QString, QByteArray> previous_;
  QHash<QString, QByteArray> popups_;
  void report(QJsonObject state, const char* phase, bool force) {
    const auto key = state["id"].toString();
    const auto bytes = QJsonDocument(state).toJson(QJsonDocument::Compact);
    if (!force && previous_.value(key) == bytes) return;
    previous_[key] = bytes;
    state["phase"] = phase;
    state["time_ms"] = QDateTime::currentMSecsSinceEpoch();
    qInfo().noquote() << "HN_RENDER" << QJsonDocument(state).toJson(QJsonDocument::Compact);
  }
  void popup(QQuickItem* control, const char* phase) {
    auto* popup = control->property("popup").value<QObject*>();
    if (!popup || !popup->property("visible").toBool()) return;
    auto* background = popup->property("background").value<QQuickItem*>();
    if (!background || !background->window()) return;
    const auto origin = background->mapToScene(QPointF());
    QJsonObject state{{"id", identity(popup)},
                      {"background", identity(background)},
                      {"x", origin.x()},
                      {"y", origin.y()},
                      {"width", background->width()},
                      {"height", background->height()},
                      {"visible", background->isVisible()},
                      {"opacity", background->opacity()},
                      {"color", background->property("color").value<QColor>().name(QColor::HexArgb)}};
    if (auto* context = qmlContext(background)) state["origin"] = context->baseUrl().toString();
    for (const auto* role : {"base", "window", "button", "text", "highlight"})
      state[QString("palette.") + role] =
          QQmlProperty(popup, QString("palette.") + role).read().value<QColor>().name(QColor::HexArgb);
    const auto signature = QJsonDocument(state).toJson(QJsonDocument::Compact);
    const auto key = identity(popup);
    if (popups_.value(key) == signature) return;
    popups_[key] = signature;
    const auto pixels = background->window()->grabWindow();
    const auto dpr = background->window()->devicePixelRatio();
    state["dpr"] = dpr;
    const auto sample = background->mapToScene(QPointF(background->width() / 2, 2));
    const QPoint pixel(qRound(sample.x() * dpr), qRound(sample.y() * dpr));
    if (pixels.rect().contains(pixel)) state["renderedPixel"] = pixels.pixelColor(pixel).name(QColor::HexArgb);
    report(state, phase, true);
  }
  void walk(QQuickItem* item, const char* phase, bool force) {
    popup(item, phase);
    const auto focus = item->property("visualFocus");
    if (focus.isValid() && item->property("down").isValid() && item->isVisible()) {
      QJsonObject state{{"id", identity(item)},
                        {"text", item->property("text").toString()},
                        {"enabled", item->isEnabled()},
                        {"down", item->property("down").toBool()},
                        {"activeFocus", item->hasActiveFocus()},
                        {"visualFocus", focus.toBool()},
                        {"focusReason", item->property("focusReason").toInt()},
                        {"highlighted", item->property("highlighted").toBool()},
                        {"checked", item->property("checked").toBool()},
                        {"current", QQmlProperty(item, "ListView.isCurrentItem", qmlContext(item)).read().toBool()}};
      if (auto* context = qmlContext(item)) state["origin"] = context->baseUrl().toString();
      report(state, phase, force);
    }
    for (auto* child : item->childItems()) walk(child, phase, force);
  }
  void sample(const char* phase, bool force = false) {
    for (auto* window : QGuiApplication::allWindows()) {
      auto* quick = qobject_cast<QQuickWindow*>(window);
      if (!quick || !quick->isVisible()) continue;
      report({{"id", identity(quick)},
              {"activeFocusItem", identity(quick->activeFocusItem())},
              {"dpr", quick->devicePixelRatio()},
              {"active", quick->isActive()}},
             phase, force);
      walk(quick->contentItem(), phase, force);
    }
  }
};

void startObserver() {
  if (!qEnvironmentVariableIsSet("HOLONIGHT_RENDER_DIAGNOSTICS")) return;
  QMetaObject::invokeMethod(qApp, [] { new Observer(qApp); }, Qt::QueuedConnection);
}
Q_COREAPP_STARTUP_FUNCTION(startObserver)
}  // namespace
