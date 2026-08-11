// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QFont>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

int main(int argc, char* argv[]) {
  QGuiApplication application{argc, argv};

  const QPalette palette = application.palette();
  if (!palette.color(QPalette::Window).isValid() || application.font().family().isEmpty() ||
      application.styleHints()->colorScheme() == Qt::ColorScheme::Unknown) {
    return 1;
  }

  return 0;
}
