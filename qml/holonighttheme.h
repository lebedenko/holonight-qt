// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/config.h"

#include <QFileSystemWatcher>
#include <QObject>
#include <QVariantList>
#include <QtQml/qqml.h>

class HolonightTheme : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(QString iconTheme READ iconTheme NOTIFY themeChanged)
  Q_PROPERTY(QString fallbackIconTheme READ fallbackIconTheme NOTIFY themeChanged)
  Q_PROPERTY(QString uiFont READ uiFont NOTIFY themeChanged)
  Q_PROPERTY(QString fixedFont READ fixedFont NOTIFY themeChanged)
  Q_PROPERTY(QString headerFont READ headerFont NOTIFY themeChanged)
  Q_PROPERTY(QString displayFont READ displayFont NOTIFY themeChanged)
  Q_PROPERTY(int baseFontSize READ baseFontSize NOTIFY themeChanged)
  Q_PROPERTY(int fixedFontSize READ fixedFontSize NOTIFY themeChanged)
  Q_PROPERTY(qreal scaleFactor READ scaleFactor NOTIFY themeChanged)
  Q_PROPERTY(int microSize READ microSize NOTIFY themeChanged)
  Q_PROPERTY(int captionSize READ captionSize NOTIFY themeChanged)
  Q_PROPERTY(int bodySize READ bodySize NOTIFY themeChanged)
  Q_PROPERTY(int subheadingSize READ subheadingSize NOTIFY themeChanged)
  Q_PROPERTY(int titleSize READ titleSize NOTIFY themeChanged)
  Q_PROPERTY(int appTitleSize READ appTitleSize NOTIFY themeChanged)
  Q_PROPERTY(int headingSize READ headingSize NOTIFY themeChanged)
  Q_PROPERTY(int displaySize READ displaySize NOTIFY themeChanged)
  Q_PROPERTY(QVariantList themeFamilies READ themeFamilies CONSTANT)
  Q_PROPERTY(QVariantList themeVariants READ themeVariants CONSTANT)

 public:
  explicit HolonightTheme(QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(HolonightTheme)

  [[nodiscard]] QString iconTheme() const { return config_.icon_theme; }
  [[nodiscard]] QString fallbackIconTheme() const { return config_.fallback_icon_theme; }
  [[nodiscard]] QString uiFont() const { return config_.ui_font; }
  [[nodiscard]] QString fixedFont() const { return config_.fixed_font; }
  [[nodiscard]] QString headerFont() const { return config_.resolvedHeaderFont(); }
  [[nodiscard]] QString displayFont() const { return config_.resolvedDisplayFont(); }
  [[nodiscard]] int baseFontSize() const { return config_.base_font_size; }
  [[nodiscard]] int fixedFontSize() const { return config_.fixedFontSize(); }
  [[nodiscard]] qreal scaleFactor() const { return config_.scale_factor; }
  [[nodiscard]] int microSize() const { return config_.microSize(); }
  [[nodiscard]] int captionSize() const { return config_.captionSize(); }
  [[nodiscard]] int bodySize() const { return config_.bodySize(); }
  [[nodiscard]] int subheadingSize() const { return config_.subheadingSize(); }
  [[nodiscard]] int titleSize() const { return config_.titleSize(); }
  [[nodiscard]] int appTitleSize() const { return config_.appTitleSize(); }
  [[nodiscard]] int headingSize() const { return config_.headingSize(); }
  [[nodiscard]] int displaySize() const { return config_.displaySize(); }
  [[nodiscard]] QVariantList themeFamilies() const;
  [[nodiscard]] QVariantList themeVariants() const;

  Q_INVOKABLE QVariantList accentOptionsForScheme(const QString& scheme_id) const;
  Q_INVOKABLE QColor accentColorForScheme(const QString& scheme_id, const QString& accent_id) const;
  Q_INVOKABLE void reload();

 Q_SIGNALS:
  void themeChanged();

 private:
  void rearmWatcher();

  Holonight::ThemeConfig config_;
  QFileSystemWatcher watcher_;
};
