#include <QGuiApplication>
#include <QPalette>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QDebug>

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  if (argc != 2) return 2;
  QQmlApplicationEngine engine(QUrl::fromLocalFile(QString::fromLocal8Bit(argv[1])));
  if (engine.rootObjects().isEmpty()) return 1;
  auto* root = engine.rootObjects().first();
  root->setProperty("constrainContent", qEnvironmentVariableIsSet("UQC_CONSTRAIN_CONTENT"));
  auto report = [&](const char* method) {
    QVariant result;
    QMetaObject::invokeMethod(root, method, Q_RETURN_ARG(QVariant, result));
    qInfo().noquote() << method << result.toString();
  };
  QTimer::singleShot(250, [&] {
    report("report");
    for (const auto& color : {QColor(Qt::black), QColor(Qt::white), QColor(Qt::black)}) {
      auto palette = app.palette();
      palette.setColor(QPalette::Window, color);
      palette.setColor(QPalette::Base, color);
      palette.setColor(QPalette::Text, color == Qt::black ? Qt::white : Qt::black);
      palette.setColor(QPalette::WindowText, palette.color(QPalette::Text));
      app.setPalette(palette);
      QCoreApplication::processEvents();
      report("paletteReport");
    }
    app.quit();
  });
  return app.exec();
}
