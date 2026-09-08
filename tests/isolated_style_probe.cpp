// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPluginLoader>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QtQml/private/qqmlcontextdata_p.h>
#include <QtQml/private/qqmldata_p.h>

#include <memory>

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  if (qEnvironmentVariable("UQC_MODE") == "imperative") QQuickStyle::setStyle("Fusion");
  // Qt embeds its IndirectBasic forwarding module in the Controls plugin. Copy
  // that Qt-owned qmldir into the filtered tree instead of adding a resource root.
  const auto qtRoot = qEnvironmentVariable("UQC_QT_MODULES");
  QPluginLoader qtControls(qtRoot + "/QtQuick/Controls/libqtquickcontrols2plugin.so");
  if (!qtControls.load()) {
    qCritical() << "Cannot load isolated Qt Controls plugin:" << qtControls.errorString();
    return 2;
  }
  QFile indirect(":/qt-project.org/imports/QtQuick/Controls/IndirectBasic/qmldir");
  if (indirect.exists()) {
    const auto destination = qtRoot + "/QtQuick/Controls/IndirectBasic";
    if (!QDir().mkpath(destination)) return 2;
    if (!QFile::exists(destination + "/qmldir") && !indirect.copy(destination + "/qmldir")) return 2;
  }
  QQmlEngine engine;
  engine.setImportPathList({qEnvironmentVariable("UQC_FIXTURE"), qEnvironmentVariable("UQC_QT_MODULES")});
  QQmlComponent component(&engine, QUrl::fromLocalFile(qEnvironmentVariable("UQC_SOURCE")),
                          QQmlComponent::PreferSynchronous);
  std::unique_ptr<QObject> root(component.isReady() ? component.create() : nullptr);
  QJsonObject origins;
  if (root) {
    for (const auto& name : {"button", "field", "basic", "fusion"}) {
      auto* object = root->findChild<QObject*>(name);
      if (!object) continue;
      QJsonArray urls;
      auto* data = QQmlData::get(object);
      for (auto context = data ? data->context : nullptr; context; context = context->parent().data())
        urls.append(context->url().toString());
      origins[name] = urls;
    }
  }
  QFile maps("/proc/self/maps");
  if (!maps.open(QIODevice::ReadOnly)) return 2;
  QJsonArray libraries;
  QStringList seen;
  for (const auto& line : maps.readAll().split('\n')) {
    const auto path = QString::fromUtf8(line.mid(line.indexOf('/')));
    if (path.contains(".so") && (path.contains("plugin") || path.contains("holonight")) && !seen.contains(path)) {
      seen.append(path);
      libraries.append(path);
    }
  }
  const QJsonObject report{{"imports", QJsonArray::fromStringList(engine.importPathList())},
                           {"errors", component.errorString()},
                           {"origins", origins},
                           {"libraries", libraries}};
  puts(QJsonDocument(report).toJson(QJsonDocument::Compact).constData());
  return root ? 0 : 1;
}
