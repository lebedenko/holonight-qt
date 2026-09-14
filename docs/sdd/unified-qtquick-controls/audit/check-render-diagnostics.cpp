// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTest>

int main(int argc, char** argv) {
  QList<QJsonObject> observations;
  static QList<QJsonObject>* records = &observations;
  static bool error = false;
  qInstallMessageHandler([](QtMsgType, const QMessageLogContext&, const QString& message) {
    if (message.startsWith("HN_RENDER ")) records->append(QJsonDocument::fromJson(message.mid(10).toUtf8()).object());
    if (message.contains("TypeError") || message.contains("ReferenceError")) error = true;
  });
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH"));
  engine.loadData(R"(
    import QtQuick
    import QtQuick.Controls as C
    import QtQuick.Templates as T
    Window {
      id: window; visible: true; width: 480; height: 400
      property int deferredCreations: 0
      T.ComboBox {
        width: 120; height: 32
        popup: T.Popup { Component.onCompleted: window.deferredCreations++ }
      }
      C.ComboBox { id: live; width: 360; y: 60; model: 20 }
      property alias live: live
      function openPopup() { live.popup.open() }
    }
  )");
  if (engine.rootObjects().isEmpty()) return 1;
  auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
  if (!QTest::qWaitForWindowExposed(window)) return 2;
  QTest::qWait(200);
  if (window->property("deferredCreations").toInt() != 0) return 3;
  if (!QMetaObject::invokeMethod(window, "openPopup")) return 4;
  auto* combo = window->property("live").value<QObject*>();
  auto* popup = combo->property("popup").value<QObject*>();
  if (!QTest::qWaitFor([&] { return popup->property("opened").toBool(); })) return 5;
  QTest::qWait(200);
  auto* background = popup->property("background").value<QQuickItem*>();
  if (!background || background->width() != popup->property("width").toDouble() ||
      background->height() != popup->property("height").toDouble())
    return 6;
  if (window->property("deferredCreations").toInt() != 0) return 7;
  bool measured = false;
  for (const auto& record : observations)
    if (record["popupVisible"].toBool() && record["popup.width"].toDouble() == 360 &&
        record["width"].toDouble() == 360 && record["content.count"].toInt() == 20)
      measured = true;
  if (measured != qEnvironmentVariableIsSet("HOLONIGHT_RENDER_DIAGNOSTICS")) return 8;
  return error ? 9 : 0;
}
