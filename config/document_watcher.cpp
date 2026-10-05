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
  QString directory = QFileInfo{path_}.absolutePath();
  while (!QFileInfo{directory}.isDir()) {
    const auto parent = QFileInfo{directory}.absolutePath();
    if (parent == directory) {
      break;
    }
    directory = parent;
  }
  if (QFileInfo{directory}.isDir()) {
    watcher_->addPath(directory);
  }
  if (QFileInfo{path_}.isFile()) {
    watcher_->addPath(path_);
  }
  // A symlink target may be replaced in a different directory from the link.
  const auto target = QFileInfo{path_}.canonicalFilePath();
  if (!target.isEmpty() && target != QFileInfo{path_}.absoluteFilePath()) {
    const auto target_parent = QFileInfo{target}.absolutePath();
    if (!watcher_->directories().contains(target_parent)) {
      watcher_->addPath(target_parent);
    }
  }
}

void DocumentWatcher::scheduleReload() {
  refresh();
  timer_.start();
}
}  // namespace Holonight
