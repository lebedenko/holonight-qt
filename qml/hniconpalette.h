// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#pragma once

#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include <QtQuick/private/qquickpalette_p.h>

namespace Holonight {
class AppearanceReader;
}

class HnIconPalette : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QQuickPalette* palette READ palette WRITE setPalette NOTIFY changed)
  Q_PROPERTY(QVariantMap colors READ colors NOTIFY changed)
 public:
  explicit HnIconPalette(QObject* parent = nullptr);
  QQuickPalette* palette() const { return palette_; }
  void setPalette(QQuickPalette* palette);
  QVariantMap colors() const;
  void classBegin() override {}
  void componentComplete() override;
 signals:
  void changed();

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  QPointer<QQuickPalette> palette_;
  QPointer<Holonight::AppearanceReader> reader_;
};
