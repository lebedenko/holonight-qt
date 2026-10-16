// SPDX-License-Identifier: GPL-3.0-or-later
#include "portalfiledialog.h"

#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDBusVariant>
#include <QEvent>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QMimeDatabase>
#include <QUuid>
#include <QWindow>
#include <qpa/qplatformintegration.h>

#include <private/qdesktopunixservices_p.h>
#include <private/qguiapplication_p.h>

namespace {
const QString kService = QStringLiteral("org.freedesktop.portal.Desktop");
const QString kDesktop = QStringLiteral("/org/freedesktop/portal/desktop");
const QString kChooser = QStringLiteral("org.freedesktop.portal.FileChooser");
const QString kRequest = QStringLiteral("org.freedesktop.portal.Request");
Q_LOGGING_CATEGORY(portalLog, "holonight.portal.filedialog")

struct Pattern {
  uint type{};
  QString value;
  bool operator==(const Pattern&) const = default;
};
using Patterns = QList<Pattern>;
struct Filter {
  QString name;
  Patterns patterns;
  bool operator==(const Filter&) const = default;
};
using Filters = QList<Filter>;

QDBusArgument& operator<<(QDBusArgument& arg, const Pattern& value) {
  arg.beginStructure();
  arg << value.type << value.value;
  arg.endStructure();
  return arg;
}
const QDBusArgument& operator>>(const QDBusArgument& arg, Pattern& value) {
  arg.beginStructure();
  arg >> value.type >> value.value;
  arg.endStructure();
  return arg;
}
QDBusArgument& operator<<(QDBusArgument& arg, const Filter& value) {
  arg.beginStructure();
  arg << value.name << value.patterns;
  arg.endStructure();
  return arg;
}
const QDBusArgument& operator>>(const QDBusArgument& arg, Filter& value) {
  arg.beginStructure();
  arg >> value.name >> value.patterns;
  arg.endStructure();
  return arg;
}
void registerTypes() {
  qDBusRegisterMetaType<Pattern>();
  qDBusRegisterMetaType<Patterns>();
  qDBusRegisterMetaType<Filter>();
  qDBusRegisterMetaType<Filters>();
}
void closeRequest(const QDBusConnection& bus, const QString& owner, const QString& path) {
  if (!path.isEmpty()) {
    bus.asyncCall(QDBusMessage::createMethodCall(owner, path, kRequest, QStringLiteral("Close")));
  }
}
bool validPath(const QString& path) {
  return path.startsWith(QStringLiteral("/org/freedesktop/portal/desktop/request/"));
}
QByteArray nativePath(const QUrl& url) {
  auto path = QFile::encodeName(url.toLocalFile());
  path.append('\0');
  return path;
}
}  // namespace

struct PortalFileSelection {
  Filters filters;
  QStringList originals;
  bool mime = false;
  bool directory = false;
  bool multiple = false;
};

// Each open has its own receiver and owner binding. Pending replies outlive the
// helper so a cancellation can close a handle returned after helper destruction.
class PortalFileRequest final : public QObject {
  Q_OBJECT
 public:
  PortalFileRequest(QDBusConnection bus, QString owner, QString path, QWindow* parent)
      : QObject(QCoreApplication::instance()),
        bus_(std::move(bus)),
        owner_(std::move(owner)),
        path_(std::move(path)),
        parent_(parent),
        service_watcher_(kService, bus_, QDBusServiceWatcher::WatchForOwnerChange) {
    connect(&service_watcher_, &QDBusServiceWatcher::serviceOwnerChanged, this,
            [this](const QString&, const QString&, const QString& owner) {
              if (owner != owner_) {
                finish(false, {}, "service-disappeared");
              }
            });
    if (parent_) {
      parent_->installEventFilter(this);
      connect(parent_, &QObject::destroyed, this, [this] { cancel(); });
    }
  }
  bool subscribe() {
    return bus_.connect(owner_, path_, kRequest, QStringLiteral("Response"), this, SLOT(response(QDBusMessage)));
  }
  void submit(const QDBusMessage& message) {
    pending_ = true;
    auto* watcher = new QDBusPendingCallWatcher(bus_.asyncCall(message, 25000), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
      QDBusPendingReply<QDBusObjectPath> reply = *watcher;
      watcher->deleteLater();
      pending_ = false;
      if (reply.isError()) {
        finish(false, {}, "submission");
      } else {
        const QString returned = reply.value().path();
        if (!validPath(returned) || !returned.startsWith(path_.left(path_.lastIndexOf(QLatin1Char('/')) + 1))) {
          finish(false, {}, "request-path");
        } else if (!active_) {
          if (cancelled_) {
            closeRequest(bus_, owner_, returned);
          }
        } else if (returned != path_) {
          unsubscribe();
          path_ = returned;
          if (!subscribe()) {
            finish(false, {}, "subscription");
          }
        }
      }
      if (!active_) {
        deleteLater();
      }
    });
  }
  void cancel() { finish(false, {}); }
 signals:
  void completed(bool /*accepted*/, const QVariantMap& /*results*/);

 protected:
  bool eventFilter(QObject* object, QEvent* event) override {
    if (object == parent_ && (event->type() == QEvent::Close || event->type() == QEvent::Hide)) {
      cancel();
    }
    return false;
  }
 private slots:
  void response(const QDBusMessage& message) {
    if (!active_) {
      return;
    }
    const auto args = message.arguments();
    if (args.size() != 2 || args.at(0).metaType() != QMetaType::fromType<uint>() ||
        args.at(1).metaType() != QMetaType::fromType<QDBusArgument>() ||
        args.at(1).value<QDBusArgument>().currentSignature() != QStringLiteral("a{sv}")) {
      finish(false, {}, "malformed-response");
      return;
    }
    const uint code = args.at(0).toUInt();
    const auto results = qdbus_cast<QVariantMap>(args.at(1));
    if (code == 0) {
      finish(true, results);
    } else {
      finish(false, {}, code == 1 ? nullptr : "response");
    }
  }

 private:
  void unsubscribe() {
    bus_.disconnect(owner_, path_, kRequest, QStringLiteral("Response"), this, SLOT(response(QDBusMessage)));
  }
  void finish(bool accepted, const QVariantMap& results, const char* error = nullptr) {
    if (!active_) {
      return;
    }
    active_ = false;
    cancelled_ = !accepted;
    unsubscribe();
    if (parent_) {
      parent_->removeEventFilter(this);
      disconnect(parent_, nullptr, this, nullptr);
    }
    if (!accepted) {
      closeRequest(bus_, owner_, path_);
    }
    if (error != nullptr) {
      qCWarning(portalLog) << error << "portal request rejected";
    }
    emit completed(accepted, results);
    if (!pending_) {
      deleteLater();
    }
  }
  QDBusConnection bus_;
  QString owner_;
  QString path_;
  QPointer<QWindow> parent_;
  QDBusServiceWatcher service_watcher_;
  bool active_ = true;
  bool pending_ = false;
  bool cancelled_ = false;
};

PortalFileDialog::~PortalFileDialog() {
  if (request_) {
    disconnect(request_, nullptr, this, nullptr);
    request_->cancel();
  }
}
void PortalFileDialog::hide() {
  if (request_) {
    request_->cancel();
  }
}
void PortalFileDialog::exec() {
  if (!request_) {
    return;
  }
  QEventLoop loop;
  connect(this, &QPlatformDialogHelper::accept, &loop, &QEventLoop::quit);
  connect(this, &QPlatformDialogHelper::reject, &loop, &QEventLoop::quit);
  connect(this, &QObject::destroyed, &loop, &QEventLoop::quit);
  loop.exec(QEventLoop::DialogExec);
}

bool PortalFileDialog::show(Qt::WindowFlags /*flags*/, Qt::WindowModality modality, QWindow* parent) {
  if (request_) {
    return true;
  }
  if (!options() || options()->testOption(QFileDialogOptions::DontUseNativeDialog) ||
      QCoreApplication::testAttribute(Qt::AA_DontUseNativeDialogs)) {
    return false;
  }
  registerTypes();
  const auto bus = QDBusConnection::sessionBus();
  auto query = QDBusMessage::createMethodCall(kService, kDesktop, QStringLiteral("org.freedesktop.DBus.Properties"),
                                              QStringLiteral("Get"));
  query << kChooser << QStringLiteral("version");
  const auto reply = bus.call(query, QDBus::Block, 3000);
  const QDBusReply<QDBusVariant> versionReply(reply);
  const bool directory = options()->fileMode() == QFileDialogOptions::Directory ||
                         options()->fileMode() == QFileDialogOptions::DirectoryOnly;
  if (!versionReply.isValid() || versionReply.value().variant().metaType() != QMetaType::fromType<uint>() ||
      versionReply.value().variant().toUInt() < (directory ? 3U : 1U)) {
    qCWarning(portalLog) << "discovery: FileChooser unavailable or unsupported; using Qt dialog";
    return false;
  }
  const QString owner = bus.interface()->serviceOwner(kService).value();
  if (!owner.startsWith(QLatin1Char(':'))) {
    qCWarning(portalLog) << "discovery: broker owner unavailable; using Qt dialog";
    return false;
  }
  QString identifier;
  QPointer<QWindow> guardedParent(parent);
  if (parent != nullptr) {
    auto* services = dynamic_cast<QDesktopUnixServices*>(QGuiApplicationPrivate::platformIntegration()->services());
    if (services != nullptr) {
      identifier = services->portalWindowIdentifier(parent);
    }
    if (!guardedParent || identifier.isEmpty()) {
      qCWarning(portalLog) << "parent-export: unavailable; using Qt dialog";
      return false;
    }
  }
  const QString token = QStringLiteral("holonight_") + QUuid::createUuid().toString(QUuid::Id128);
  QString sender = bus.baseService().mid(1);
  sender.replace(QLatin1Char('.'), QLatin1Char('_'));
  const QString path = QStringLiteral("/org/freedesktop/portal/desktop/request/") + sender + QLatin1Char('/') + token;
  const bool save = options()->acceptMode() == QFileDialogOptions::AcceptSave;
  const auto selection = filterSelection();
  const auto values = portalOptions(token, modality, selection);
  auto* request = new PortalFileRequest(bus, owner, path, parent);
  if (!request->subscribe()) {
    delete request;
    qCWarning(portalLog) << "subscription: unavailable; using Qt dialog";
    return false;
  }
  request_ = request;
  connect(request, &PortalFileRequest::completed, this,
          [this, selection](bool accepted, const QVariantMap& results) { complete(accepted, results, selection); });
  auto message = QDBusMessage::createMethodCall(owner, kDesktop, kChooser,
                                                save ? QStringLiteral("SaveFile") : QStringLiteral("OpenFile"));
  message << identifier << options()->windowTitle() << values;
  request->submit(message);
  return true;
}

PortalFileSelection PortalFileDialog::filterSelection() {
  PortalFileSelection selection;
  selection.mime = !options()->mimeTypeFilters().isEmpty();
  selection.directory = options()->fileMode() == QFileDialogOptions::Directory ||
                        options()->fileMode() == QFileDialogOptions::DirectoryOnly;
  selection.multiple = options()->acceptMode() != QFileDialogOptions::AcceptSave &&
                       options()->fileMode() == QFileDialogOptions::ExistingFiles;
  auto& filters = selection.filters;
  auto& originals = selection.originals;
  const bool mime = selection.mime;
  if (mime) {
    if (mime_filter_.isEmpty()) {
      mime_filter_ = options()->initiallySelectedMimeTypeFilter();
    }
    for (const auto& type : options()->mimeTypeFilters()) {
      const auto description = QMimeDatabase().mimeTypeForName(type).comment();
      filters.append({.name = description.isEmpty() ? type : description, .patterns = {{.type = 1, .value = type}}});
      originals.append(type);
    }
  } else {
    if (name_filter_.isEmpty()) {
      name_filter_ = options()->initiallySelectedNameFilter();
    }
    for (const auto& name : options()->nameFilters()) {
      Filter filter{.name = name.left(name.indexOf(QLatin1Char('('))).trimmed(), .patterns = {}};
      for (const auto& pattern : cleanFilterList(name)) {
        filter.patterns.append({.type = 0, .value = pattern});
      }
      if (!filter.patterns.isEmpty()) {
        filters.append(filter);
        originals.append(name);
      }
    }
  }
  return selection;
}

QVariantMap PortalFileDialog::portalOptions(const QString& token, Qt::WindowModality modality,
                                            const PortalFileSelection& selection) {
  const bool save = options()->acceptMode() == QFileDialogOptions::AcceptSave;
  QVariantMap values{{QStringLiteral("handle_token"), token}, {QStringLiteral("modal"), modality != Qt::NonModal}};
  if (!save) {
    values.insert(QStringLiteral("multiple"), options()->fileMode() == QFileDialogOptions::ExistingFiles);
    values.insert(QStringLiteral("directory"), selection.directory);
  }
  if (options()->isLabelExplicitlySet(QFileDialogOptions::Accept)) {
    QString label = options()->labelText(QFileDialogOptions::Accept);
    label.replace(QLatin1Char('&'), QLatin1Char('_'));
    values.insert(QStringLiteral("accept_label"), label);
  }
  if (directory_.isEmpty()) {
    directory_ = options()->initialDirectory();
  }
  if (files_.isEmpty()) {
    files_ = options()->initiallySelectedFiles();
  }
  if (directory_.isLocalFile()) {
    values.insert(QStringLiteral("current_folder"), nativePath(directory_));
  }
  if (save && !files_.isEmpty()) {
    values.insert(QStringLiteral("current_name"), files_.first().fileName());
    if (files_.first().isLocalFile()) {
      values.insert(QStringLiteral("current_file"), nativePath(files_.first()));
      values.insert(QStringLiteral("current_folder"),
                    nativePath(QUrl::fromLocalFile(QFileInfo(files_.first().toLocalFile()).absolutePath())));
    }
  }
  const auto& filters = selection.filters;
  const auto& originals = selection.originals;
  const bool mime = selection.mime;
  if (!filters.isEmpty()) {
    values.insert(QStringLiteral("filters"), QVariant::fromValue(filters));
    const auto index = originals.indexOf(mime ? mime_filter_ : name_filter_);
    values.insert(QStringLiteral("current_filter"), QVariant::fromValue(filters.at(index < 0 ? 0 : index)));
  }
  return values;
}

void PortalFileDialog::complete(bool accepted, const QVariantMap& results, const PortalFileSelection& selection) {
  request_ = nullptr;
  QList<QUrl> files;
  const auto uris = results.value(QStringLiteral("uris"));
  if (accepted && uris.metaType() == QMetaType::fromType<QStringList>()) {
    for (const auto& uri : uris.toStringList()) {
      const QUrl url(uri, QUrl::StrictMode);
      if (!url.isValid() || url.isRelative() || url.scheme().isEmpty()) {
        files.clear();
        break;
      }
      files.append(url);
    }
  }
  if (accepted && (files.isEmpty() || (!selection.multiple && files.size() != 1))) {
    qCWarning(portalLog) << "malformed-response: invalid selected URLs";
    accepted = false;
  }
  const auto selected = results.value(QStringLiteral("current_filter"));
  if (accepted && selected.isValid() &&
      (selected.metaType() != QMetaType::fromType<QDBusArgument>() ||
       selected.value<QDBusArgument>().currentSignature() != QStringLiteral("(sa(us))"))) {
    qCWarning(portalLog) << "malformed-response: invalid selected filter";
    accepted = false;
  }
  if (!accepted) {
    emit reject();
    return;
  }
  files_ = files;
  directory_ = selection.directory ? files.first() : files.first().adjusted(QUrl::RemoveFilename);
  if (selected.isValid()) {
    const auto filter = qdbus_cast<Filter>(selected);
    const auto index = selection.filters.indexOf(filter);
    if (index >= 0) {
      if (selection.mime) {
        mime_filter_ = selection.originals.at(index);
      } else {
        name_filter_ = selection.originals.at(index);
      }
    }
  }
  // Callers read the updated helper state from their acceptance handler.
  emit accept();
}

#include "portalfiledialog.moc"
