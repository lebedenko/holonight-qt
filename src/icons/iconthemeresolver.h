// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QByteArray>
#include <QSize>
#include <QString>

namespace Holonight {

class IconThemeResolver {
 public:
  [[nodiscard]] static QString resolveIconPath(const QString& source, QSize size = QSize{24, 24}, qreal scale = 1.0);
  [[nodiscard]] static QByteArray readIconBytes(const QString& path);
};

}  // namespace Holonight
