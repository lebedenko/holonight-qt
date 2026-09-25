// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QDir>
#include <QFile>
#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QPalette>
#include <QStyleHints>
#include <QTemporaryDir>

#include <cstdio>

int main(int argc, char* argv[]) {
  QGuiApplication application{argc, argv};

  const QPalette palette = application.palette();
  if (!palette.color(QPalette::Window).isValid() || application.font().family().isEmpty() ||
      application.styleHints()->colorScheme() == Qt::ColorScheme::Unknown) {
    return 1;
  }

  QTemporaryDir fixture;
  if (!fixture.isValid() || !QDir{fixture.path()}.mkpath(QStringLiteral("HoloNight/16x16/actions"))) return 2;
  const auto write_file = [](const QString& path, const QByteArray& bytes) {
    QFile file{path};
    return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size();
  };
  if (!write_file(fixture.filePath(QStringLiteral("HoloNight/index.theme")),
                  "[Icon Theme]\nName=Fixture\nDirectories=16x16/actions\n[16x16/actions]\nSize=16\nType=Fixed\n") ||
      !write_file(fixture.filePath(QStringLiteral("HoloNight/16x16/actions/hn-bootstrap-symbolic.svg")),
                  "<svg xmlns='http://www.w3.org/2000/svg' width='16' height='16'>"
                  "<rect width='16' height='16' fill='#000'/></svg>")) {
    return 3;
  }
  QIcon::setThemeSearchPaths({fixture.path()});
  QPalette test_palette = application.palette();
  test_palette.setColor(QPalette::Active, QPalette::Text, Qt::red);
  application.setPalette(test_palette);
  const QImage icon = QIcon::fromTheme(QStringLiteral("hn-bootstrap-symbolic")).pixmap(QSize{16, 16}).toImage();
  if (icon.isNull() || icon.pixelColor(8, 8) != QColor{Qt::red}) {
    std::fprintf(stderr, "theme icon: null=%d color=%s\n", icon.isNull(),
                 icon.isNull() ? "none" : qPrintable(icon.pixelColor(8, 8).name()));
    return 4;
  }

  return 0;
}
