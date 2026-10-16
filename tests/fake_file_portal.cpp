// SPDX-License-Identifier: GPL-3.0-or-later
#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QDBusVirtualObject>
#include <QTimer>
#include <QUrl>

#include <cstdio>

class FakePortal : public QDBusVirtualObject {
 public:
  [[nodiscard]] QString introspect(const QString& /*path*/) const override { return {}; }
  bool handleMessage(const QDBusMessage& message, const QDBusConnection& bus) override {
    const auto member = message.member();
    if (member == QStringLiteral("Configure")) {
      mode_ = message.arguments().first().toString();
      closed_ = 0;
      bus.send(message.createReply());
    } else if (member == QStringLiteral("Inspect")) {
      auto result = last_;
      result.insert(QStringLiteral("closed"), closed_);
      bus.send(message.createReply(QList<QVariant>{result}));
    } else if (member == QStringLiteral("Get")) {
      if (mode_ != QStringLiteral("timeout")) {
        bus.send(message.createReply({QVariant::fromValue(QDBusVariant(mode_ == QStringLiteral("old") ? 2U : 4U))}));
      }
    } else if (member == QStringLiteral("Close")) {
      ++closed_;
      bus.send(message.createReply());
    } else if (member == QStringLiteral("OpenFile") || member == QStringLiteral("SaveFile")) {
      openChooser(message, bus);
    } else {
      return false;
    }
    return true;
  }

 private:
  static void respond(const QDBusConnection& bus, const QString& path, const QString& mode,
                      const QVariantMap& options) {
    auto signal =
        QDBusMessage::createSignal(path, QStringLiteral("org.freedesktop.portal.Request"), QStringLiteral("Response"));
    QVariantMap results;
    results.insert(
        QStringLiteral("uris"),
        QStringList{QUrl::fromLocalFile(qEnvironmentVariable("PORTAL_TEST_FILE")).toString(QUrl::FullyEncoded)});
    if (mode == QStringLiteral("malformed")) {
      results.insert(QStringLiteral("uris"), 42);
    }
    if (options.contains(QStringLiteral("current_filter"))) {
      results.insert(QStringLiteral("current_filter"), options.value(QStringLiteral("current_filter")));
    }
    if (mode == QStringLiteral("bad-filter")) {
      results.insert(QStringLiteral("current_filter"), 42);
    }
    if (mode == QStringLiteral("bad-signal")) {
      signal << QStringLiteral("invalid");
    } else {
      signal << (mode == QStringLiteral("cancel") ? 1U : 0U) << results;
    }
    bus.send(signal);
  }

  void openChooser(const QDBusMessage& message, const QDBusConnection& bus) {
    last_ = qdbus_cast<QVariantMap>(message.arguments().at(2));
    last_.insert(QStringLiteral("method"), message.member());
    last_.insert(QStringLiteral("parent"), message.arguments().at(0));
    last_.insert(QStringLiteral("title"), message.arguments().at(1));
    QString sender = message.service().mid(1);
    sender.replace('.', '_');
    QString path = QStringLiteral("/org/freedesktop/portal/desktop/request/") + sender + '/' +
                   last_.value(QStringLiteral("handle_token")).toString();
    if (mode_ == QStringLiteral("alternate") || mode_ == QStringLiteral("pending")) {
      path += QStringLiteral("_other");
    }
    if (mode_ == QStringLiteral("bad-path")) {
      path = QStringLiteral("/invalid");
    }
    const auto response = [bus, path, mode = mode_, options = last_] { respond(bus, path, mode, options); };
    const auto reply = [bus, message, path] {
      bus.send(message.createReply({QVariant::fromValue(QDBusObjectPath(path))}));
    };
    if (mode_ == QStringLiteral("error")) {
      bus.send(message.createErrorReply(QStringLiteral("org.freedesktop.portal.Error.Failed"), QStringLiteral("test")));
    } else if (mode_ == QStringLiteral("early")) {
      response();
      QTimer::singleShot(100, this, reply);
    } else {
      QTimer::singleShot(mode_ == QStringLiteral("pending") ? 150 : 0, this, reply);
      if (mode_ != QStringLiteral("pending") && mode_ != QStringLiteral("hold")) {
        QTimer::singleShot(mode_ == QStringLiteral("late") ? 150 : 30, this, response);
      }
    }
  }

  QString mode_;
  QVariantMap last_;
  int closed_ = 0;
};

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  auto bus = QDBusConnection::sessionBus();
  FakePortal portal;
  if (!bus.registerVirtualObject(QStringLiteral("/org/freedesktop/portal/desktop"), &portal,
                                 QDBusConnection::SubPath) ||
      !bus.registerService(QStringLiteral("org.freedesktop.portal.Desktop"))) {
    return 1;
  }
  std::puts("ready");
  std::fflush(stdout);
  return QCoreApplication::exec();
}
