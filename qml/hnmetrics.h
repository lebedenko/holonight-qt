// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/metrics.h"

#include <QObject>
#include <QtQml/qqml.h>

class HnMetrics : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(int borderWidth READ borderWidth CONSTANT)
  Q_PROPERTY(int focusBorderWidth READ focusBorderWidth CONSTANT)
  Q_PROPERTY(int separatorWidth READ separatorWidth CONSTANT)
  Q_PROPERTY(int headerHeight READ headerHeight CONSTANT)
  Q_PROPERTY(int appTitleIconSize READ appTitleIconSize CONSTANT)
  Q_PROPERTY(int appTitleIconSpacing READ appTitleIconSpacing CONSTANT)
  Q_PROPERTY(int appTitleTextSpacing READ appTitleTextSpacing CONSTANT)

 public:
  explicit HnMetrics(QObject* parent = nullptr) : QObject{parent} {}
  Q_DISABLE_COPY_MOVE(HnMetrics)

  [[nodiscard]] int borderWidth() const { return tokens().border_width; }
  [[nodiscard]] int focusBorderWidth() const { return tokens().focus_border_width; }
  [[nodiscard]] int separatorWidth() const { return tokens().separator_width; }
  [[nodiscard]] int headerHeight() const { return tokens().header_height; }
  [[nodiscard]] int appTitleIconSize() const { return tokens().app_title_icon_size; }
  [[nodiscard]] int appTitleIconSpacing() const { return tokens().app_title_icon_spacing; }
  [[nodiscard]] int appTitleTextSpacing() const { return tokens().app_title_text_spacing; }

  [[nodiscard]] Q_INVOKABLE int normalizedSizeRole(int size_role) const;
  [[nodiscard]] Q_INVOKABLE int controlHeight(int size_role) const;
  [[nodiscard]] Q_INVOKABLE int horizontalPadding(int size_role) const;
  [[nodiscard]] Q_INVOKABLE int iconSize(int size_role) const;
  [[nodiscard]] Q_INVOKABLE int internalSpacing(int size_role) const;

 private:
  [[nodiscard]] static const Holonight::MetricTokens& tokens() { return Holonight::metricTokens(); }
};
