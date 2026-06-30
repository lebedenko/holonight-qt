// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/config.h"

#include <QObject>
#include <QVariantList>
#include <QtQml/qqml.h>

class HolonightTheme : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(QString iconTheme READ iconTheme CONSTANT)
  Q_PROPERTY(QString fallbackIconTheme READ fallbackIconTheme CONSTANT)
  Q_PROPERTY(QString uiFont READ uiFont CONSTANT)
  Q_PROPERTY(QString fixedFont READ fixedFont CONSTANT)
  Q_PROPERTY(int baseFontSize READ baseFontSize CONSTANT)
  Q_PROPERTY(qreal scaleFactor READ scaleFactor CONSTANT)
  Q_PROPERTY(int captionSize READ captionSize CONSTANT)
  Q_PROPERTY(int bodySize READ bodySize CONSTANT)
  Q_PROPERTY(int titleSize READ titleSize CONSTANT)
  Q_PROPERTY(int headingSize READ headingSize CONSTANT)
  Q_PROPERTY(QVariantList themeFamilies READ themeFamilies CONSTANT)
  Q_PROPERTY(QVariantList themeVariants READ themeVariants CONSTANT)

 public:
  explicit HolonightTheme(QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(HolonightTheme)

  [[nodiscard]] QString iconTheme() const { return config_.icon_theme; }
  [[nodiscard]] QString fallbackIconTheme() const { return config_.fallback_icon_theme; }
  [[nodiscard]] QString uiFont() const { return config_.ui_font; }
  [[nodiscard]] QString fixedFont() const { return config_.fixed_font; }
  [[nodiscard]] int baseFontSize() const { return config_.base_font_size; }
  [[nodiscard]] qreal scaleFactor() const { return config_.scale_factor; }
  [[nodiscard]] int captionSize() const { return config_.captionSize(); }
  [[nodiscard]] int bodySize() const { return config_.bodySize(); }
  [[nodiscard]] int titleSize() const { return config_.titleSize(); }
  [[nodiscard]] int headingSize() const { return config_.headingSize(); }
  [[nodiscard]] QVariantList themeFamilies() const;
  [[nodiscard]] QVariantList themeVariants() const;

  Q_INVOKABLE QVariantList accentOptionsForScheme(const QString& scheme_id) const;

 private:
  Holonight::ThemeConfig config_;
};
