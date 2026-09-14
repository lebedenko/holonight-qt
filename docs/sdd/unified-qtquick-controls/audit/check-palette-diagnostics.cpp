// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTest>
#include <QtQuick/private/qquickpalette_p.h>

int main(int argc, char** argv) {
  static QList<QJsonObject> records;
  qInstallMessageHandler([](QtMsgType, const QMessageLogContext&, const QString& message) {
    if (message.startsWith("HN_PALETTE ")) records.append(QJsonDocument::fromJson(message.mid(11).toUtf8()).object());
  });
  QApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH"));
  engine.loadData(R"(
    import QtQuick
    import QtQuick.Controls as C
    import QtQuick.Templates as T
    C.ApplicationWindow {
      id: root; visible: true; width: 400; height: 300
      palette.light: "#80445566"
      property int deferredCreations: 0
      T.ComboBox { popup: T.Popup { Component.onCompleted: root.deferredCreations++ } }
      C.TextField { objectName: "field"; palette.disabled.text: "#40556677" }
    }
  )");
  if (engine.rootObjects().isEmpty()) return 1;
  auto* root = engine.rootObjects().first();
  auto* source = root->property("palette").value<QQuickPalette*>();
  const auto windowPalette = source->toQPalette();
  auto* field = root->findChild<QObject*>("field")->property("palette").value<QQuickPalette*>();
  const auto fieldPalette = field->toQPalette();
  const auto applicationPalette = app.palette();
  QTest::qWait(350);
  if (root->property("deferredCreations").toInt() != 0) return 2;
  if (app.palette() != applicationPalette || app.palette().resolveMask() != applicationPalette.resolveMask()) return 3;
  if (source->toQPalette() != windowPalette || source->toQPalette().resolveMask() != windowPalette.resolveMask())
    return 4;
  if (field->toQPalette() != fieldPalette || field->toQPalette().resolveMask() != fieldPalette.resolveMask()) return 5;
  bool measured = false;
  for (const auto& record : records) {
    const auto palette = record["palette"].toObject();
    if (palette["0"].toObject()["Light"] == "#80445566" && palette.contains("resolveMask")) measured = true;
  }
  if (measured != qEnvironmentVariableIsSet("HOLONIGHT_PALETTE_DIAGNOSTICS")) return 6;
  return 0;
}
