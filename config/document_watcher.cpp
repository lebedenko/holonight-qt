// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#include "holonight/document_watcher.h"

#include <QAbstractEventDispatcher>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QMetaObject>

namespace Holonight {
DocumentWatcher::DocumentWatcher(QString path, QObject* parent) : QObject{parent}, path_{std::move(path)} {
  timer_.setSingleShot(true);
  timer_.setInterval(0);
  connect(&timer_, &QTimer::timeout, this, &DocumentWatcher::documentChanged);
  if (QAbstractEventDispatcher::instance() != nullptr) {
    initializeWatcher();
  } else {
    QMetaObject::invokeMethod(this, &DocumentWatcher::initializeWatcher, Qt::QueuedConnection);
  }
}
DocumentWatcher::~DocumentWatcher() = default;

HoloNight::Config::Result<HoloNight::Config::DocumentSnapshot> DocumentWatcher::read() const {
  if (path_.isEmpty()) {
    return HoloNight::Config::Result<HoloNight::Config::DocumentSnapshot>::failure({
        {
            .code = HoloNight::Config::ErrorCode::PathUnavailable,
            .severity = HoloNight::Config::Severity::Error,
            .message = "configuration path is unavailable",
            .path = std::nullopt,
            .position = std::nullopt,
        },
    });
  }
  return HoloNight::Config::readDocument(std::filesystem::path{path_.toStdString()});
}

void DocumentWatcher::initializeWatcher() {
  if (watcher_) {
    return;
  }
  watcher_ = std::make_unique<QFileSystemWatcher>();
  connect(watcher_.get(), &QFileSystemWatcher::fileChanged, this, &DocumentWatcher::scheduleReload);
  connect(watcher_.get(), &QFileSystemWatcher::directoryChanged, this, &DocumentWatcher::scheduleReload);
  refresh();
}

void DocumentWatcher::refresh() {
  if (!watcher_) {
    return;
  }
  const auto watched = watcher_->files() + watcher_->directories();
  if (!watched.isEmpty()) {
    watcher_->removePaths(watched);
  }
  const auto watch_parent = [this](const QString& file) {
    QString directory = QFileInfo{file}.absolutePath();
    while (!QFileInfo{directory}.isDir()) {
      const auto parent = QFileInfo{directory}.absolutePath();
      if (parent == directory) {
        break;
      }
      directory = parent;
    }
    if (QFileInfo{directory}.isDir() && !watcher_->directories().contains(directory)) {
      watcher_->addPath(directory);
    }
  };
  watch_parent(path_);
  if (QFileInfo{path_}.isFile()) {
    watcher_->addPath(path_);
  }
  // Resolve even an absent physical target, so its recreation is observed.
  const auto target = HoloNight::Config::resolveDocumentTarget(std::filesystem::path{path_.toStdString()});
  if (target) {
    watch_parent(QString::fromStdString(target.value->string()));
  }
}

void DocumentWatcher::scheduleReload() {
  refresh();
  timer_.start();
}
}  // namespace Holonight
