#include <QDebug>
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickStyle>

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  if (qEnvironmentVariableIsSet("UQC_FUSION_FALLBACK")) QQuickStyle::setFallbackStyle("Fusion");
  QQmlEngine engine;
  qInfo() << "style" << QQuickStyle::name() << "imports" << engine.importPathList();
  QQmlComponent component(&engine);
  component.setData(R"(
import QtQuick
import QtQuick.Controls as C
C.ApplicationWindow {
    width: 320; height: 240; visible: false
    C.Button { text: "Audit" }
    C.TextField { text: "sample" }
    C.Label { text: "Label" }
    C.ToolButton { text: "Tool" }
    C.Dialog { title: "Dialog" }
})",
                    QUrl("file:///uqc-discovery.qml"));
  auto* object = component.create();
  if (!object) {
    qCritical() << component.errors();
    return 1;
  }
  qInfo() << "fixture-created";
  delete object;
  return 0;
}
