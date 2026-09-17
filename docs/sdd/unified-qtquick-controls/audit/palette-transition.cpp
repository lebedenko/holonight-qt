// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

// Standalone diagnostic: collection errors fail here; P03 assertions are separate.
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSaveFile>
#include <QSet>
#include <QtQuick/private/qquickpalette_p.h>
#include <QtTest/QTest>

#include <memory>

namespace {
QJsonObject paletteState(const QPalette& palette) {
  QJsonObject result{{"resolveMask", QString::number(palette.resolveMask(), 16)}};
  const auto roles = QMetaEnum::fromType<QPalette::ColorRole>();
  for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    QJsonObject colors;
    for (int index = 0; index < QPalette::NColorRoles; ++index)
      colors[QString::fromLatin1(roles.valueToKey(index))] =
          palette.color(group, static_cast<QPalette::ColorRole>(index)).name(QColor::HexArgb);
    result[QString::number(group)] = colors;
  }
  return result;
}
void require(bool condition, const char* message) {
  if (!condition) qFatal("%s", message);
}
void writeAppearance(bool light) {
  QFile source(qEnvironmentVariable("HOLONIGHT_APPEARANCE_FILE"));
  require(source.open(QIODevice::ReadOnly), "Cannot read isolated appearance file");
  auto contents = source.readAll();
  contents.replace(light ? "holonight-dark" : "holonight-light", light ? "holonight-light" : "holonight-dark");
  source.close();
  QSaveFile file(source.fileName());
  require(file.open(QIODevice::WriteOnly), "Cannot open isolated appearance file");
  file.write(contents);
  require(file.commit(), "Cannot commit appearance file");
}
QPalette explicitPalette(bool light) {
  QPalette palette;
  for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    for (auto role : {QPalette::Window, QPalette::Base, QPalette::Button, QPalette::AlternateBase})
      palette.setColor(group, role, light ? QColor("#eeeeee") : QColor("#222222"));
    for (auto role : {QPalette::Text, QPalette::WindowText, QPalette::ButtonText})
      palette.setColor(group, role, light ? QColor("#222222") : QColor("#eeeeee"));
  }
  return palette;
}
QJsonObject objectState(QObject* object) {
  QJsonObject state{{"class", object->metaObject()->className()}, {"name", object->objectName()}};
  if (auto* context = qmlContext(object)) state["origin"] = context->baseUrl().toString();
  if (auto* palette = object->property("palette").value<QQuickPalette*>())
    state["palette"] = paletteState(palette->toQPalette());
  for (const auto* name : {"enabled", "visible", "color"}) {
    const auto value = object->property(name);
    if (value.isValid())
      state[name] = value.metaType() == QMetaType::fromType<QColor>()
                        ? QJsonValue(value.value<QColor>().name(QColor::HexArgb))
                        : QJsonValue::fromVariant(value);
  }
  return state;
}
void walk(QQuickItem* item, QJsonArray& objects, QSet<QQuickItem*>& visited, const QImage& image, qreal dpr) {
  if (visited.contains(item)) return;
  visited.insert(item);
  auto state = objectState(item);
  if (item->parentItem()) state["parentClass"] = item->parentItem()->metaObject()->className();
  // These fixtures deliberately instantiate backgrounds; observing them is safe here.
  if (!item->objectName().isEmpty()) {
    if (auto* background = item->property("background").value<QQuickItem*>())
      state["background"] = objectState(background);
    const auto point = item->mapToScene(QPointF(item->width() - 25, item->height() / 2)) * dpr;
    if (image.rect().contains(point.toPoint()))
      state["pixel"] = image.pixelColor(point.toPoint()).name(QColor::HexArgb);
  }
  objects.append(state);
  for (auto* child : item->childItems()) walk(child, objects, visited, image, dpr);
}
QJsonObject snapshot(QQuickWindow* window, const QString& output, const QString& phase, const QString& name) {
  int frames = 0;
  const auto connection = QObject::connect(window, &QQuickWindow::frameSwapped, window, [&] { ++frames; });
  window->setProperty("captureRevision", window->property("captureRevision").toInt() + 1);
  window->update();
  require(QTest::qWaitFor([&] { return frames > 0; }, 5000), "Frame transition timeout");
  QObject::disconnect(connection);
  const auto image = window->grabWindow();
  require(!image.isNull(), "Rendered capture unavailable");
  require(image.save(output + "/" + phase + "-" + name + ".png"), "Cannot save capture");
  QJsonArray objects;
  QSet<QQuickItem*> visited;
  walk(window->contentItem(), objects, visited, image, window->devicePixelRatio());
  auto result = objectState(window);
  result["dpr"] = window->devicePixelRatio();
  result["objects"] = objects;
  return result;
}
}  // namespace
int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  require(argc == 4, "Usage: palette-transition fixture.qml file|application output-directory");
  const bool application = QString::fromLocal8Bit(argv[2]) == "application";
  const auto output = QString::fromLocal8Bit(argv[3]);
  QQmlEngine engine;
  QQmlComponent component(&engine, QUrl::fromLocalFile(QString::fromLocal8Bit(argv[1])));
  auto create = [&]() {
    auto window = std::unique_ptr<QQuickWindow>(qobject_cast<QQuickWindow*>(component.create()));
    if (!window) qFatal("Fixture failed: %s", qPrintable(component.errorString()));
    require(QTest::qWaitFor([&] { return window->isExposed(); }, 5000), "Window exposure timeout");
    return window;
  };
  auto primary = create();
  auto secondary = create();
  if (application) QGuiApplication::setPalette(explicitPalette(false));
  QJsonArray records;
  auto collect = [&](const QString& phase) {
    records.append(QJsonObject{{"phase", phase},
                               {"application", paletteState(QGuiApplication::palette())},
                               {"primary", snapshot(primary.get(), output, phase, "primary")},
                               {"secondary", snapshot(secondary.get(), output, phase, "secondary")}});
  };
  collect("dark");
  for (bool light : {true, false}) {
    const auto previous = primary->property("appearanceBase");
    if (application) {
      QGuiApplication::setPalette(explicitPalette(light));
      require(QTest::qWaitFor([&] { return QGuiApplication::palette() == explicitPalette(light); }, 5000),
              "Application palette transition timeout");
    } else {
      writeAppearance(light);
      require(QTest::qWaitFor([&] { return primary->property("appearanceBase") != previous; }, 5000),
              "Appearance transition timeout");
    }
    collect(light ? "light" : "dark-return");
    if (light) {
      secondary.reset();
      secondary = create();
      collect("light-recreated");
    }
  }
  QFile maps("/proc/self/maps");
  require(maps.open(QIODevice::ReadOnly), "Cannot read mappings");
  QFile mapped(output + "/maps.txt");
  require(mapped.open(QIODevice::WriteOnly), "Cannot save mappings");
  mapped.write(maps.readAll());
  QFile results(output + "/measurements.json");
  require(results.open(QIODevice::WriteOnly), "Cannot save measurements");
  results.write(QJsonDocument(QJsonObject{{"qt", qVersion()}, {"records", records}}).toJson());
}
