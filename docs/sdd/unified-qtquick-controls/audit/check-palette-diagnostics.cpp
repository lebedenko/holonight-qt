// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QTest>
#include <QWidget>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickpalette_p.h>
#include <QtQuick/private/qquickwindow_p.h>

class UnsupportedPalette : public QObject {
  Q_OBJECT
  Q_PROPERTY(QQuickPalette* palette READ palette)
 public:
  using QObject::QObject;
  mutable int reads = 0;
  QQuickPalette* palette() const {
    ++reads;
    return nullptr;
  }
};

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
      Item { objectName: "unallocatedItem" }
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
  auto* content = qobject_cast<QQuickWindow*>(root)->contentItem();
  if (QQuickItemPrivate::get(content)->providesPalette()) return 7;
  QQmlComponent deferred(&engine);
  deferred.setData(R"(
    import QtQuick
    import QtQuick.Templates as T
    T.Control {
      id: probe
      property int creations: 0
      background: Rectangle { Component.onCompleted: probe.creations++ }
    }
  )",
                   QUrl());
  auto* deferredControl = deferred.beginCreate(engine.rootContext());
  if (!deferredControl) return 16;
  deferredControl->setParent(content);
  if (deferredControl->property("creations").toInt() != 0) return 17;
  UnsupportedPalette unsupported(content);
  QWidget widget;
  auto widgetPalette = widget.palette();
  widgetPalette.setColor(QPalette::Disabled, QPalette::Text, QColor("#123456"));
  widget.setPalette(widgetPalette);
  QQuickWindow unallocatedWindow;
  if (QQuickWindowPrivate::get(&unallocatedWindow)->providesPalette()) return 9;
  const auto items = root->findChildren<QQuickItem*>();
  QHash<QQuickItem*, QPalette> existing;
  QSet<QQuickItem*> unallocated;
  for (auto* item : items) {
    auto* data = QQuickItemPrivate::get(item);
    if (data->providesPalette())
      existing.insert(item, data->palette()->toQPalette());
    else
      unallocated.insert(item);
  }
  const auto applicationPalette = app.palette();
  QTest::qWait(350);
  if (QQuickItemPrivate::get(content)->providesPalette()) return 8;
  if (root->property("deferredCreations").toInt() != 0) return 2;
  if (app.palette() != applicationPalette || app.palette().resolveMask() != applicationPalette.resolveMask()) return 3;
  if (source->toQPalette() != windowPalette || source->toQPalette().resolveMask() != windowPalette.resolveMask())
    return 4;
  if (field->toQPalette() != fieldPalette || field->toQPalette().resolveMask() != fieldPalette.resolveMask()) return 5;
  if (deferredControl->property("creations").toInt() != 0) return 18;
  if (unsupported.reads != 0) return 10;
  if (QQuickWindowPrivate::get(&unallocatedWindow)->providesPalette()) return 11;
  for (auto* item : unallocated)
    if (QQuickItemPrivate::get(item)->providesPalette()) return 12;
  for (auto it = existing.cbegin(); it != existing.cend(); ++it) {
    const auto current = QQuickItemPrivate::get(it.key())->palette()->toQPalette();
    if (current != it.value() || current.resolveMask() != it.value().resolveMask()) return 13;
  }
  if (widget.palette() != widgetPalette || widget.palette().resolveMask() != widgetPalette.resolveMask()) return 14;
  bool allocationRecorded = false;
  bool measured = false;
  for (const auto& record : records) {
    if (record["name"] == "unallocatedItem" && record.contains("paletteAllocated") &&
        !record["paletteAllocated"].toBool() && !record.contains("palette"))
      allocationRecorded = true;
    const auto palette = record["palette"].toObject();
    if (palette["0"].toObject()["Light"] == "#80445566" && palette.contains("resolveMask")) measured = true;
  }
  if (measured != qEnvironmentVariableIsSet("HOLONIGHT_PALETTE_DIAGNOSTICS")) return 6;
  if (allocationRecorded != qEnvironmentVariableIsSet("HOLONIGHT_PALETTE_DIAGNOSTICS")) return 15;
  deferred.completeCreate();
  return 0;
}

#include "check-palette-diagnostics.moc"
