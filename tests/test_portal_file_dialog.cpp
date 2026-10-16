// SPDX-License-Identifier: GPL-3.0-or-later
#include "portalfiledialog.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QElapsedTimer>
#include <QFile>
#include <QProcess>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QWindow>

#include <gtest/gtest.h>

namespace {
QDBusMessage control(const QString& method, const QList<QVariant>& args = {}) {
  auto message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Desktop"),
                                                QStringLiteral("/org/freedesktop/portal/desktop"),
                                                QStringLiteral("test.Portal"), method);
  message.setArguments(args);
  return QDBusConnection::sessionBus().call(message);
}
class PortalDialog : public testing::Test {
 protected:
  void SetUp() override {
    process.start(QString::fromLocal8Bit(PORTAL_FAKE));
    ASSERT_TRUE(process.waitForStarted());
    ASSERT_TRUE(process.waitForReadyRead());
    ASSERT_EQ(process.readLine().trimmed(), QByteArray("ready"));
    helper.setOptions(QFileDialogOptions::create());
    helper.options()->setFileMode(QFileDialogOptions::ExistingFile);
  }
  void TearDown() override {
    helper.hide();
    if (process.state() != QProcess::NotRunning) {
      process.terminate();
      ASSERT_TRUE(process.waitForFinished());
    }
  }
  static void mode(const QString& name) {
    ASSERT_NE(control(QStringLiteral("Configure"), {name}).type(), QDBusMessage::ErrorMessage);
  }
  static QVariantMap inspect() { return QDBusReply<QVariantMap>(control(QStringLiteral("Inspect"))).value(); }
  QProcess process;
  PortalFileDialog helper;
};
TEST_F(PortalDialog, AcceptsUrlsAndRoundTripsFiltersOnRepeatedOpens) {
  helper.options()->setWindowTitle(QStringLiteral("Choose a document"));
  helper.options()->setNameFilters({QStringLiteral("Markdown (*.md *.markdown)"), QStringLiteral("All (*)")});
  helper.selectNameFilter(QStringLiteral("Markdown (*.md *.markdown)"));
  helper.setDirectory(QUrl::fromLocalFile(QStringLiteral("/tmp/é dir")));
  for (const auto& behavior : {"normal", "early", "alternate"}) {
    mode(QString::fromLatin1(behavior));
    QSignalSpy accepted(&helper, &QPlatformDialogHelper::accept);
    ASSERT_TRUE(helper.show({}, Qt::ApplicationModal, nullptr));
    ASSERT_TRUE(QTest::qWaitFor([&] { return accepted.count() == 1; }));
    EXPECT_EQ(helper.selectedFiles(), QList<QUrl>{QUrl::fromLocalFile(qEnvironmentVariable("PORTAL_TEST_FILE"))});
    EXPECT_EQ(helper.selectedNameFilter(), QStringLiteral("Markdown (*.md *.markdown)"));
    EXPECT_EQ(inspect().value(QStringLiteral("title")).toString(), QStringLiteral("Choose a document"));
    EXPECT_TRUE(inspect().value(QStringLiteral("modal")).toBool());
  }
}
TEST_F(PortalDialog, SaveDirectoryAndMultipleOptions) {
  helper.options()->setAcceptMode(QFileDialogOptions::AcceptSave);
  helper.selectFile(QUrl::fromLocalFile(QStringLiteral("/tmp/é name.md")));
  helper.options()->setMimeTypeFilters({QStringLiteral("text/markdown")});
  helper.selectMimeTypeFilter(QStringLiteral("text/markdown"));
  helper.options()->setLabelText(QFileDialogOptions::Accept, QStringLiteral("&Save"));
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  helper.exec();
  auto values = inspect();
  EXPECT_EQ(values.value(QStringLiteral("method")), QStringLiteral("SaveFile"));
  EXPECT_EQ(values.value(QStringLiteral("current_name")), QStringLiteral("é name.md"));
  EXPECT_EQ(values.value(QStringLiteral("current_file")).toByteArray(),
            QFile::encodeName(QStringLiteral("/tmp/é name.md")) + QByteArray(1, '\0'));
  EXPECT_EQ(values.value(QStringLiteral("accept_label")), QStringLiteral("_Save"));
  EXPECT_FALSE(values.value(QStringLiteral("modal")).toBool());
  EXPECT_EQ(helper.selectedMimeTypeFilter(), QStringLiteral("text/markdown"));
  helper.options()->setAcceptMode(QFileDialogOptions::AcceptOpen);
  for (auto fileMode : {QFileDialogOptions::Directory, QFileDialogOptions::ExistingFiles}) {
    helper.options()->setFileMode(fileMode);
    ASSERT_TRUE(helper.show({}, Qt::WindowModal, nullptr));
    helper.exec();
    values = inspect();
    EXPECT_EQ(values.value(QStringLiteral("directory")).toBool(), fileMode == QFileDialogOptions::Directory);
    if (fileMode == QFileDialogOptions::Directory) {
      EXPECT_EQ(helper.directory(), QUrl::fromLocalFile(qEnvironmentVariable("PORTAL_TEST_FILE")));
    }
    EXPECT_EQ(values.value(QStringLiteral("multiple")).toBool(), fileMode == QFileDialogOptions::ExistingFiles);
  }
}
TEST_F(PortalDialog, TerminalErrorsRejectAndExitExecWithoutFallback) {
  for (const auto& behavior : {"cancel", "malformed", "bad-filter", "bad-signal", "bad-path", "error"}) {
    mode(QString::fromLatin1(behavior));
    QSignalSpy rejected(&helper, &QPlatformDialogHelper::reject);
    ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
    helper.exec();
    EXPECT_EQ(rejected.count(), 1);
    helper.hide();
    EXPECT_EQ(rejected.count(), 1);
  }
}
TEST_F(PortalDialog, CancelPendingClosesLateHandleAndAllowsReopen) {
  mode(QStringLiteral("pending"));
  QSignalSpy rejected(&helper, &QPlatformDialogHelper::reject);
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  helper.hide();
  EXPECT_EQ(rejected.count(), 1);
  ASSERT_TRUE(QTest::qWaitFor([&] { return inspect().value(QStringLiteral("closed")).toInt() >= 2; }));
  mode(QStringLiteral("normal"));
  QSignalSpy accepted(&helper, &QPlatformDialogHelper::accept);
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  helper.exec();
  EXPECT_EQ(accepted.count(), 1);
  EXPECT_EQ(rejected.count(), 1);
}
TEST_F(PortalDialog, HideExitsExecAndLateResponsesCannotCompleteNextOpen) {
  mode(QStringLiteral("late"));
  QSignalSpy rejected(&helper, &QPlatformDialogHelper::reject);
  QSignalSpy accepted(&helper, &QPlatformDialogHelper::accept);
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  QTimer::singleShot(10, &helper, &PortalFileDialog::hide);
  helper.exec();
  EXPECT_EQ(rejected.count(), 1);
  mode(QStringLiteral("hold"));
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  QTimer::singleShot(250, &helper, &PortalFileDialog::hide);
  helper.exec();
  EXPECT_EQ(accepted.count(), 0);
  EXPECT_EQ(rejected.count(), 2);
}
TEST_F(PortalDialog, DestructionClosesPendingHandle) {
  mode(QStringLiteral("pending"));
  {
    PortalFileDialog temporary;
    temporary.setOptions(QFileDialogOptions::create());
    ASSERT_TRUE(temporary.show({}, Qt::NonModal, nullptr));
  }
  ASSERT_TRUE(QTest::qWaitFor([&] { return inspect().value(QStringLiteral("closed")).toInt() >= 2; }));
}
TEST_F(PortalDialog, IndependentDialogs) {
  PortalFileDialog other;
  other.setOptions(QFileDialogOptions::create());
  QSignalSpy first(&helper, &QPlatformDialogHelper::accept);
  QSignalSpy second(&other, &QPlatformDialogHelper::accept);
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  ASSERT_TRUE(other.show({}, Qt::NonModal, nullptr));
  ASSERT_TRUE(QTest::qWaitFor([&] { return first.count() == 1 && second.count() == 1; }));
}
TEST_F(PortalDialog, ServiceDisappearanceTerminatesExec) {
  mode(QStringLiteral("hold"));
  QSignalSpy rejected(&helper, &QPlatformDialogHelper::reject);
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  QTimer::singleShot(50, &process, &QProcess::terminate);
  helper.exec();
  EXPECT_EQ(rejected.count(), 1);
  process.waitForFinished();
  process.start(QString::fromLocal8Bit(PORTAL_FAKE));
  ASSERT_TRUE(process.waitForReadyRead());
  process.readLine();
  QSignalSpy accepted(&helper, &QPlatformDialogHelper::accept);
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  helper.exec();
  EXPECT_EQ(accepted.count(), 1);
}
TEST_F(PortalDialog, FallbackBeforeSubmission) {
  helper.options()->setOption(QFileDialogOptions::DontUseNativeDialog);
  EXPECT_FALSE(helper.show({}, Qt::NonModal, nullptr));
  helper.options()->setOption(QFileDialogOptions::DontUseNativeDialog, false);
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
  EXPECT_FALSE(helper.show({}, Qt::NonModal, nullptr));
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs, false);
  QWindow parent;
  EXPECT_FALSE(helper.show({}, Qt::NonModal, &parent));
  mode(QStringLiteral("old"));
  helper.options()->setFileMode(QFileDialogOptions::Directory);
  EXPECT_FALSE(helper.show({}, Qt::NonModal, nullptr));
  mode(QStringLiteral("timeout"));
  QElapsedTimer timer;
  timer.start();
  EXPECT_FALSE(helper.show({}, Qt::NonModal, nullptr));
  EXPECT_GE(timer.elapsed(), 2800);
  EXPECT_LT(timer.elapsed(), 4500);
  mode(QStringLiteral("normal"));
  ASSERT_TRUE(helper.show({}, Qt::NonModal, nullptr));
  helper.exec();
  process.terminate();
  ASSERT_TRUE(process.waitForFinished());
  EXPECT_FALSE(helper.show({}, Qt::NonModal, nullptr));
}
}  // namespace
