// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "config.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Holonight"));
    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
    engine.loadFromModule(QStringLiteral("HoloniightDemo"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty()) { return 1; }
    return QGuiApplication::exec();
}
