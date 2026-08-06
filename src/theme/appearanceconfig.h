// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "shaperesolver.h"

#include <QString>
#include <QStringView>

namespace Holonight {

struct AppearanceConfig {
  CornerStyle corner_style = CornerStyle::Inherit;
  qreal shape_scale = 1.0;
  qreal base_radius = qQNaN();
  qreal base_chamfer = qQNaN();

  [[nodiscard]] ShapeTokens shapeTokens() const;
  [[nodiscard]] CornerStyle effectiveStyle(CornerStyle component_style) const;
  [[nodiscard]] AppearanceConfig normalized() const;
  [[nodiscard]] bool save(const QString& path = configFilePath(), QString* error_message = nullptr) const;

  [[nodiscard]] static AppearanceConfig defaults();
  [[nodiscard]] static AppearanceConfig load(const QString& path = configFilePath());
  [[nodiscard]] static QString configFilePath();
  [[nodiscard]] static CornerStyle cornerStyleFromName(QStringView name);
  [[nodiscard]] static QString cornerStyleName(CornerStyle style);
  [[nodiscard]] static constexpr qreal minimumShapeScale() { return 0.25; }
  [[nodiscard]] static constexpr qreal maximumShapeScale() { return 4.0; }
  [[nodiscard]] static constexpr qreal maximumBaseExtent() { return 128.0; }

  friend bool operator==(const AppearanceConfig& lhs, const AppearanceConfig& rhs);
};

}  // namespace Holonight
