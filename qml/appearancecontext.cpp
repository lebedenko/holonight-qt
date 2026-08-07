// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "appearancecontext.h"

#include "holonight/appearance_reader.h"

#include <QHash>
#include <QPointer>
#include <QQmlEngine>

namespace Holonight {

AppearanceReader* appearanceReaderForEngine(QQmlEngine* engine) {
  static QHash<QQmlEngine*, QPointer<AppearanceReader>> readers;
  if (const QPointer<AppearanceReader> existing = readers.value(engine); !existing.isNull()) {
    return existing;
  }
  auto* reader = new AppearanceReader{engine};
  readers.insert(engine, reader);
  QObject::connect(engine, &QObject::destroyed, [engine] { readers.remove(engine); });
  return reader;
}

}  // namespace Holonight
