// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "themeresolver.h"

#include <QFile>
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QtQml/private/qqmlcontextdata_p.h>
#include <QtQml/private/qqmldata_p.h>

#include <holonight/config/config.h>
#include <memory>

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  const QString mode = qEnvironmentVariable("UQC_SELECTION_MODE", "Holonight");
  if (mode == "Haruna" || mode == "HolonightFusion") QQuickStyle::setFallbackStyle("Fusion");
  QPalette choice;
  if (mode == "config-app") {
    choice.setColor(QPalette::Base, QColor("#123456"));
    app.setPalette(choice);
  }
  QQmlEngine engine;
  const auto root = qEnvironmentVariable("UQC_IMPORT_PATH");
  engine.addImportPath(root);
  if (mode == "Haruna") engine.addImportPath(qEnvironmentVariable("UQC_HARUNA_PATH"));
  QQmlComponent component(&engine);
  component.setData(R"(
import QtQuick
import QtQuick.Controls as C
import Holonight.Core
import Holonight.Controls
C.ApplicationWindow {
    objectName: "ApplicationWindow"
    C.Label { objectName: "Label" }
    C.ToolButton { objectName: "ToolButton" }
    C.ToolBar { objectName: "ToolBar" }
    C.ToolSeparator { objectName: "ToolSeparator" }
    C.MenuSeparator { objectName: "MenuSeparator" }
    C.Popup { objectName: "Popup" }
    C.MenuBar { objectName: "MenuBar"; C.Menu { title: "File" } property var generated: itemAt(0) }
    C.MenuBarItem { objectName: "MenuBarItem" }
    C.Dialog { objectName: "Dialog" }
    C.TextField { objectName: "runtime" }
    HnSearchField { objectName: "composite" }
})",
                    QUrl());
  std::unique_ptr<QObject> window(component.create());
  if (!window) {
    qCritical().noquote() << component.errorString();
    return 1;
  }
  auto* field = window->findChild<QQuickItem*>("runtime");
  auto* composite = window->findChild<QQuickItem*>("composite");
  if (!field || !composite) return 2;
  const auto url = QQmlData::get(field)->context->url().toString();
  QString compositeUrl;
  for (auto* context = QQmlData::get(composite)->context; context; context = context->parent().data()) {
    if (context->url().toString().contains("/Holonight/Controls/HnSearchField.qml")) {
      compositeUrl = context->url().toString();
      break;
    }
  }
  fprintf(stdout, "runtime=%s\ncomposite=%s\nimport-root=%s\n", qPrintable(url), qPrintable(compositeUrl),
          qPrintable(root));
  QFile maps("/proc/self/maps");
  if (!maps.open(QIODevice::ReadOnly)) return 7;
  const auto mappings = maps.readAll();
  for (const auto& module :
       {QByteArray("/Holonight/libholonight_qml.so"), QByteArray("/Holonight/Core/libholonight_core_qml.so"),
        QByteArray("/Holonight/Controls/libholonight_controls_qml.so"),
        QByteArray("/Holonight/impl/libholonight_impl_qml.so")}) {
    if (!mappings.contains(root.toUtf8() + module)) return 8;
  }
  const bool fusion = mode == "Fusion" || mode == "Haruna";
  if (!url.contains(fusion ? "/QtQuick/Controls/Fusion/TextField.qml" : "/Holonight/TextField.qml")) return 3;
  const auto stylePath = fusion ? QStringLiteral("/QtQuick/Controls/Fusion/") : QStringLiteral("/Holonight/");
  auto checkOrigin = [](QObject* object, const QString& expected) {
    if (!object) return false;
    for (auto* context = QQmlData::get(object)->context; context; context = context->parent().data()) {
      if (context->url().toString().contains(expected)) return true;
    }
    qCritical() << "Wrong type origin" << object << expected;
    return false;
  };
  for (const QString type : {"ApplicationWindow", "Label", "ToolButton", "ToolBar", "ToolSeparator", "MenuSeparator",
                             "Popup", "MenuBar", "MenuBarItem"}) {
    auto* object = type == "ApplicationWindow" ? window.get() : window->findChild<QObject*>(type);
    if (!checkOrigin(object, stylePath + type + ".qml")) return 9;
  }
  auto* bar = window->findChild<QObject*>("MenuBar");
  if (!checkOrigin(bar->property("generated").value<QObject*>(), stylePath + "MenuBarItem.qml")) return 10;
  const auto fallback = fusion || mode == "HolonightFusion" ? "Fusion" : "Basic";
  if (!checkOrigin(window->findChild<QObject*>("Dialog"),
                   QStringLiteral("/QtQuick/Controls/%1/Dialog.qml").arg(fallback)))
    return 11;
  if (!compositeUrl.contains("/Holonight/Controls/HnSearchField.qml")) return 4;
  const auto tokens =
      Holonight::ThemeResolver::resolve(*Holonight::resolveAppearance(HoloNight::Config::defaults()).value);
  const auto base = field->property("background").value<QQuickItem*>()->property("color").value<QColor>();
  const auto expected = fusion                 ? app.palette().color(QPalette::Base)
                        : mode == "config"     ? QColor("#654321")
                        : mode == "config-app" ? QColor("#123456")
                                               : tokens.surface;
  if (base != expected) {
    qCritical() << "Wrong runtime surface" << base << expected;
    return 5;
  }
  if (composite->property("background").value<QQuickItem*>()->property("color").value<QColor>() != tokens.surface)
    return 6;
  return 0;
}
