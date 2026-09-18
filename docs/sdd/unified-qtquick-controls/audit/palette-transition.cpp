// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

// Standalone diagnostic: collection errors fail here; P03 assertions are separate.
#include <QFile>
#include <QGuiApplication>
#include <QHoverEvent>
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
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickpalette_p.h>
#include <QtQuick/private/qquickwindow_p.h>
#include <QtTest/QTest>
#include <qpa/qwindowsysteminterface.h>

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
  QJsonObject state{{"class", object->metaObject()->className()},
                    {"name", object->objectName()},
                    {"identity", QString::number(reinterpret_cast<quintptr>(object), 16)}};
  if (auto* context = qmlContext(object)) state["origin"] = context->baseUrl().toString();
  // Reading Item.palette can allocate it. Stability observation must not create
  // an inheritance boundary; allocation is a separate explicit experiment.
  bool allocated = true;
  if (!qEnvironmentVariableIsEmpty("UQC_STABILITY_TRIGGER")) {
    if (auto* item = qobject_cast<QQuickItem*>(object)) allocated = QQuickItemPrivate::get(item)->providesPalette();
    if (auto* window = qobject_cast<QQuickWindow*>(object))
      allocated = QQuickWindowPrivate::get(window)->providesPalette();
  }
  state["paletteAllocated"] = allocated;
  if (allocated) {
    if (auto* palette = object->property("palette").value<QQuickPalette*>()) {
      state["palette"] = paletteState(palette->toQPalette());
      state["currentColorGroup"] = int(palette->currentColorGroup());
    }
  }
  for (const auto* name :
       {"enabled", "visible", "color", "active", "hovered", "activeFocus", "appearanceRevision", "appearanceBase"}) {
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
  result["contentItem"] = objectState(window->contentItem());
  result["dpr"] = window->devicePixelRatio();
  result["objects"] = objects;
  return result;
}
// UQC-223 is opt-in and may dispatch only inside a disposable offscreen process.
class Observer : public QObject {
 public:
  QJsonArray events;
  QString operation;
  bool eventFilter(QObject* object, QEvent* event) override {
    switch (event->type()) {
      case QEvent::WindowActivate:
      case QEvent::WindowDeactivate:
      case QEvent::FocusIn:
      case QEvent::FocusOut:
      case QEvent::HoverEnter:
      case QEvent::HoverLeave:
      case QEvent::PaletteChange:
      case QEvent::ApplicationPaletteChange:
        events.append(QJsonObject{
            {"operation", operation}, {"type", int(event->type())}, {"beforeDelivery", objectState(object)}});
        break;
      default:
        break;
    }
    return false;
  }
  void watch(QObject* object) {
    if (auto* item = qobject_cast<QQuickItem*>(object); item && !QQuickItemPrivate::get(item)->providesPalette())
      return;
    if (auto* palette = object->property("palette").value<QQuickPalette*>())
      connect(palette, &QQuickPalette::changed, this, [this, object] {
        events.append(
            QJsonObject{{"operation", operation}, {"signal", "palette.changed"}, {"state", objectState(object)}});
      });
    if (auto* window = qobject_cast<QQuickWindow*>(object))
      connect(window, &QWindow::activeChanged, this, [this, window] {
        events.append(
            QJsonObject{{"operation", operation}, {"signal", "activeChanged"}, {"state", objectState(window)}});
      });
  }
};
QJsonObject immediate(QQuickWindow* window) {
  auto state = objectState(window);
  QJsonArray objects;
  QSet<QQuickItem*> visited;
  walk(window->contentItem(), objects, visited, QImage{}, window->devicePixelRatio());
  state["objects"] = objects;
  return state;
}
QJsonArray stability(QQmlComponent& component, const QString& output) {
  require(QGuiApplication::platformName() == "offscreen", "Synthetic diagnostics require offscreen");
  Observer observer;
  qGuiApp->installEventFilter(&observer);
  const auto trigger = qEnvironmentVariable("UQC_STABILITY_TRIGGER");
  const bool lightHistory = qEnvironmentVariable("UQC_SECONDARY_HISTORY") == "light";
  const auto overrideScope = qEnvironmentVariable("UQC_OVERRIDE_SCOPE");
  auto create = [&](const QString& name) {
    auto window = std::unique_ptr<QQuickWindow>(qobject_cast<QQuickWindow*>(component.create()));
    if (!window) qFatal("Fixture failed: %s", qPrintable(component.errorString()));
    window->setObjectName(name);
    require(QTest::qWaitFor([&] { return window->isExposed(); }, 5000), "Exposure timeout");
    if (qEnvironmentVariable("UQC_CONTENT_PALETTE") == "read") {
      // Reproduce exactly the retained observer's palette getter, independently
      // of the passive recorder. This intentionally allocates a root palette.
      window->contentItem()->property("palette");
    }
    if (overrideScope == "window" || overrideScope == "control") {
      QObject* target =
          overrideScope == "window" ? static_cast<QObject*>(window.get()) : window->findChild<QObject*>("nativeText");
      auto* palette = target->property("palette").value<QQuickPalette*>();
      palette->setBase(QColor("#bada55"));
      palette->disabled()->setText(QColor("#112233"));
    }
    observer.watch(window.get());
    for (auto* child : window->findChildren<QObject*>())
      if (!child->objectName().isEmpty()) observer.watch(child);
    return window;
  };
  const bool native = qEnvironmentVariable("UQC_BOUNDARY") == "native";
  if (native) QGuiApplication::setPalette(explicitPalette(false));
  if (overrideScope == "application") {
    QPalette palette;
    palette.setColor(QPalette::Base, QColor("#bada55"));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#112233"));
    QGuiApplication::setPalette(palette);
  }
  auto primary = create("main-created-dark");
  const auto initialPrimary = snapshot(primary.get(), output, "construction", "primary");
  std::unique_ptr<QQuickWindow> secondary;
  if (!lightHistory) secondary = create("secondary-created-dark");
  auto transition = [&](bool light) {
    const auto revision = primary->property("appearanceRevision").toInt();
    writeAppearance(light);
    if (native) {
      QGuiApplication::setPalette(explicitPalette(light));
      primary->setProperty("appearanceRevision", revision + 1);
      if (secondary) secondary->setProperty("appearanceRevision", revision + 1);
    }
    require(QTest::qWaitFor([&] { return primary->property("appearanceRevision").toInt() > revision; }, 5000),
            "Appearance revision timeout");
  };
  QJsonArray records;
  auto collect = [&](const QString& phase) {
    QFile appearance(qEnvironmentVariable("HOLONIGHT_APPEARANCE_FILE"));
    require(appearance.open(QIODevice::ReadOnly), "Appearance read failed");
    records.append(QJsonObject{{"phase", phase},
                               {"requestedAppearance", QString::fromUtf8(appearance.readAll())},
                               {"application", paletteState(QGuiApplication::palette())},
                               {"immediatePrimary", immediate(primary.get())},
                               {"immediateSecondary", immediate(secondary.get())},
                               {"primary", snapshot(primary.get(), output, phase, "primary")},
                               {"secondary", snapshot(secondary.get(), output, phase, "secondary")}});
  };
  transition(true);
  const auto beforeSecondary = snapshot(primary.get(), output, "transition", "primary");
  if (lightHistory) secondary = create("secondary-created-light");
  for (bool light : {true, false}) {
    if (!light) transition(false);
    const QString scheme = light ? "light" : "dark-return";
    observer.operation = scheme + "-before";
    collect(observer.operation);
    for (bool enter : {true, false}) {
      observer.operation = scheme + (enter ? "-enter" : "-leave");
      if (trigger == "activation") {
        QWindowSystemInterface::handleFocusWindowChanged(enter ? primary.get() : secondary.get(),
                                                         Qt::ActiveWindowFocusReason);
        QWindowSystemInterface::sendWindowSystemEvents(QEventLoop::AllEvents);
      } else if (trigger == "application-event") {
        QEvent event(QEvent::ApplicationPaletteChange);
        QCoreApplication::sendEvent(qGuiApp, &event);
      } else {
        for (auto* window : {primary.get(), secondary.get()}) {
          for (const auto* name : {"nativeButton", "nativeText", "nativeSpin", "nativeCombo", "disabledText",
                                   "localOverride", "sharedSearch", "sharedCombo"}) {
            auto* item = window->findChild<QQuickItem*>(name);
            if (native && QString::fromLatin1(name).startsWith("shared")) continue;
            require(item, "Missing named control");
            if (trigger == "hover") {
              QHoverEvent event(enter ? QEvent::HoverEnter : QEvent::HoverLeave, QPointF(10, 10), QPointF(10, 10),
                                QPointF(-1, -1));
              QCoreApplication::sendEvent(item, &event);
            } else if (trigger == "resolve") {
              // Mutating an unasserted Link role forces normal palette re-resolution.
              auto* palette = item->property("palette").value<QQuickPalette*>();
              if (enter)
                palette->setLink(QColor("#123456"));
              else
                palette->resetLink();
            } else if (trigger == "enabled" && QString::fromLatin1(name) != "disabledText") {
              item->setEnabled(!enter);
            }
          }
        }
      }
      collect(observer.operation);
    }
  }
  if (overrideScope != "none" && !overrideScope.isEmpty()) {
    observer.operation = "override-reset";
    if (overrideScope == "application")
      QGuiApplication::setPalette(QPalette{});
    else
      for (auto* window : {primary.get(), secondary.get()}) {
        QObject* target =
            overrideScope == "window" ? static_cast<QObject*>(window) : window->findChild<QObject*>("nativeText");
        target->property("palette").value<QQuickPalette*>()->reset();
      }
    QFile reset(output + "/override-reset.json");
    require(reset.open(QIODevice::WriteOnly), "Cannot save override reset");
    reset.write(QJsonDocument(QJsonObject{{"primary", snapshot(primary.get(), output, "reset", "primary")},
                                          {"secondary", snapshot(secondary.get(), output, "reset", "secondary")}})
                    .toJson());
  }
  QFile events(output + "/events.json");
  require(events.open(QIODevice::WriteOnly), "Cannot save events");
  events.write(QJsonDocument(observer.events).toJson());
  auto first = records.first().toObject();
  first["initialPrimary"] = initialPrimary;
  first["beforeSecondaryPrimary"] = beforeSecondary;
  records[0] = first;
  return records;
}
}  // namespace
int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  require(argc == 4, "Usage: palette-transition fixture.qml file|application|stability output-directory");
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
  QJsonArray records;
  if (QString::fromLocal8Bit(argv[2]) == "stability") {
    records = stability(component, output);
  } else {
    auto primary = create();
    auto secondary = create();
    if (application) QGuiApplication::setPalette(explicitPalette(false));
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
