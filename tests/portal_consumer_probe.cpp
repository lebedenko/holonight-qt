// SPDX-License-Identifier: GPL-3.0-or-later
// This executable deliberately links only Qt and loads the installed plugin.
#include <QApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFileDialog>
#include <QProcess>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>

#include <memory>
#include <span>

namespace {
int checkWidgetsDialog() {
  int result = 0;
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFile);
  QTimer::singleShot(5000, &dialog, &QFileDialog::reject);
  if (dialog.exec() != QDialog::Accepted ||
      dialog.selectedUrls() != QList<QUrl>{QUrl::fromLocalFile(qEnvironmentVariable("PORTAL_TEST_FILE"))}) {
    result = 3;
  }
  return result;
}

int checkQuickDialog(const QString& mode) {
  int result = 0;
  QQmlEngine engine;
  QQmlComponent component(&engine);
  component.setData(mode == QStringLiteral("folder") ? "import QtQuick.Dialogs\nFolderDialog {}"
                                                     : "import QtQuick.Dialogs\nFileDialog {}",
                    QUrl());
  std::unique_ptr<QObject> dialog(component.create());
  if (!dialog) {
    qWarning() << component.errors();
    result = 4;
  } else {
    QSignalSpy accepted(dialog.get(), SIGNAL(accepted()));
    QMetaObject::invokeMethod(dialog.get(), "open");
    if (!QTest::qWaitFor([&] { return accepted.count() == 1; }, 5000)) {
      result = 5;
    }
    const char* property = mode == QStringLiteral("folder") ? "selectedFolder" : "selectedFile";
    if (dialog->property(property).toUrl() != QUrl::fromLocalFile(qEnvironmentVariable("PORTAL_TEST_FILE"))) {
      qWarning() << property << dialog->property(property);
      result = 6;
    }
  }
  return result;
}
}  // namespace

int main(int argc, char** argv) {
  if (argc != 4) {
    return 1;
  }
  const std::span<char*> arguments(argv, static_cast<std::size_t>(argc));
  qputenv("QT_QPA_PLATFORMTHEME", "holonight");
  qputenv("QT_PLUGIN_PATH", arguments[2]);
  QApplication app(argc, argv);
  QProcess fake;
  fake.start(QString::fromLocal8Bit(arguments[1]));
  if (!fake.waitForReadyRead() || fake.readLine().trimmed() != QByteArray("ready")) {
    return 2;
  }
  const QString mode = QString::fromLocal8Bit(arguments[3]);
  int result = mode == QStringLiteral("widgets") ? checkWidgetsDialog() : checkQuickDialog(mode);
  auto inspect = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Desktop"),
                                                QStringLiteral("/org/freedesktop/portal/desktop"),
                                                QStringLiteral("test.Portal"), QStringLiteral("Inspect"));
  QDBusReply<QVariantMap> reply = QDBusConnection::sessionBus().call(inspect);
  if (!reply.isValid() || reply.value().value(QStringLiteral("method")) != QStringLiteral("OpenFile")) {
    result = 7;
  }
  if (mode == QStringLiteral("folder") && !reply.value().value(QStringLiteral("directory")).toBool()) {
    result = 8;
  }
  fake.terminate();
  fake.waitForFinished();
  if (result != 0) {
    qWarning() << "Portal consumer failed" << mode << result;
  }
  return result;
}
