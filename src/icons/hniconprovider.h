// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QColor>
#include <QObject>
#include <QUrl>
#include <QtQml/qqml.h>

class HnIconProvider : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(HnIconProvider)

 public:
  explicit HnIconProvider(QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(HnIconProvider)

  Q_INVOKABLE QString sourceUrl(const QUrl& source, int size, const QColor& color, const QColor& highlight,
                                const QColor& positive, const QColor& neutral, const QColor& negative,
                                int palette_revision = 0);
  Q_INVOKABLE bool supportsSemanticColors(const QUrl& source) const;

 private:
  void ensureProviderRegistered();
};
