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
  const QString primary = QIcon::themeName();
  if (!fixture.isValid() || primary.isEmpty() || !QDir{fixture.path()}.mkpath(primary + QStringLiteral("/places/24")) ||
      !QDir{fixture.path()}.mkpath(QStringLiteral("hn-bootstrap-parent/16x16/places")))
    return 2;
  const auto write_file = [](const QString& path, const QByteArray& bytes) {
    QFile file{path};
    return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size();
  };
  if (!write_file(fixture.filePath(primary + QStringLiteral("/index.theme")),
                  "[Icon Theme]\nName=Fixture\nInherits=hn-bootstrap-parent\nDirectories=places/24\n"
                  "ScaledDirectories=places/24/.\n[places/24]\nSize=24\nType=Scalable\nMinSize=16\nMaxSize=32\n"
                  "[places/24/.]\nSize=24\nScale=2\nType=Scalable\nMinSize=16\nMaxSize=32\n") ||
      !write_file(fixture.filePath(QStringLiteral("hn-bootstrap-parent/index.theme")),
                  "[Icon Theme]\nName=Parent\nDirectories=16x16/places\n[16x16/places]\nSize=16\nType=Fixed\n") ||
      !write_file(fixture.filePath(primary + QStringLiteral("/places/24/folder.svg")),
                  "<svg xmlns='http://www.w3.org/2000/svg' width='16' height='16'>"
                  "<rect width='16' height='16' fill='#ff0000'/></svg>") ||
      !write_file(fixture.filePath(QStringLiteral("hn-bootstrap-parent/16x16/places/folder.svg")),
                  "<svg xmlns='http://www.w3.org/2000/svg' width='16' height='16'>"
                  "<rect width='16' height='16' fill='#00bcd4'/></svg>") ||
      !write_file(fixture.filePath(QStringLiteral("hn-bootstrap-parent/16x16/places/hn-bootstrap-symbolic.svg")),
                  "<svg xmlns='http://www.w3.org/2000/svg' width='16' height='16'>"
                  "<rect width='16' height='16' fill='#000'/></svg>")) {
    return 3;
  }
  QIcon::setThemeSearchPaths({fixture.path()});
  const QImage folder = QIcon::fromTheme(QStringLiteral("folder")).pixmap(QSize{16, 16}).toImage();
  if (folder.isNull() || folder.pixelColor(8, 8) != QColor{Qt::red}) {
    std::fprintf(stderr, "primary theme: %s, folder color: %s\n", qPrintable(primary),
                 folder.isNull() ? "none" : qPrintable(folder.pixelColor(8, 8).name()));
    return 5;
  }
  const QImage inherited = QIcon::fromTheme(QStringLiteral("hn-bootstrap-symbolic")).pixmap(QSize{16, 16}).toImage();
  if (inherited.isNull()) return 6;
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
