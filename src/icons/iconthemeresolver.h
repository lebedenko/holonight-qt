// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QByteArray>
#include <QString>

namespace Holonight {

class IconThemeResolver {
 public:
  [[nodiscard]] static QByteArray resolveSvgBytes(const QString& source);
};

}  // namespace Holonight
