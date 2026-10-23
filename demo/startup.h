// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once

#include <QCoreApplication>
#include <QObject>
#include <QStringList>
#include <QTemporaryDir>

#include <span>
#include <utility>

namespace Demo {
QString findThemeArgument(std::span<char*> arguments);
bool writeAppearance(const QTemporaryDir& directory, const QString& theme);
void addPluginPaths();
bool parseArguments(QCoreApplication& app);
QString widgetsExecutable(const QString& directory);
QStringList widgetsArguments(const QString& theme);
QString launchWidgets(const QString& directory, const QString& theme);

class WidgetsLauncher : public QObject {
  Q_OBJECT
 public:
  WidgetsLauncher(QObject* root, QString directory, QString theme)
      : root_(root), directory_(std::move(directory)), theme_(std::move(theme)) {}
 public slots:
  void open() { root_->setProperty("widgetsLaunchError", launchWidgets(directory_, theme_)); }

 private:
  QObject* root_;
  QString directory_;
  QString theme_;
};
}  // namespace Demo
