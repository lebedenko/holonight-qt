// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/appearance_reader.h"

#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <QVariantList>
#include <QtQml/qqml.h>

class HolonightTheme : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(QString iconTheme READ iconTheme NOTIFY themeChanged)
  Q_PROPERTY(QString fallbackIconTheme READ fallbackIconTheme NOTIFY themeChanged)
  Q_PROPERTY(QString uiFont READ uiFont NOTIFY themeChanged)
  Q_PROPERTY(QString displayFont READ displayFont NOTIFY themeChanged)
  Q_PROPERTY(qreal layoutScale READ layoutScale NOTIFY themeChanged)
  Q_PROPERTY(int microSize READ microSize NOTIFY themeChanged)
  Q_PROPERTY(int captionSize READ captionSize NOTIFY themeChanged)
  Q_PROPERTY(int bodySize READ bodySize NOTIFY themeChanged)
  Q_PROPERTY(int subheadingSize READ subheadingSize NOTIFY themeChanged)
  Q_PROPERTY(int appTitleSize READ appTitleSize NOTIFY themeChanged)
  Q_PROPERTY(int headingSize READ headingSize NOTIFY themeChanged)
  Q_PROPERTY(QString monospaceFont READ monospaceFont NOTIFY themeChanged)
  Q_PROPERTY(QString titleFont READ titleFont NOTIFY themeChanged)
  Q_PROPERTY(int uiFontSize READ uiFontSize NOTIFY themeChanged)
  Q_PROPERTY(int monospaceFontSize READ monospaceFontSize NOTIFY themeChanged)
  Q_PROPERTY(int titleFontSize READ titleFontSize NOTIFY themeChanged)
  Q_PROPERTY(int displayFontSize READ displayFontSize NOTIFY themeChanged)
  Q_PROPERTY(QVariantList themeFamilies READ themeFamilies CONSTANT)
  Q_PROPERTY(QVariantList themeVariants READ themeVariants CONSTANT)

 public:
  Q_DISABLE_COPY_MOVE(HolonightTheme)
  [[nodiscard]] static HolonightTheme* create(QQmlEngine* engine, QJSEngine* script_engine);

  [[nodiscard]] QString iconTheme() const { return appearance().icon_theme; }
  [[nodiscard]] QString fallbackIconTheme() const { return appearance().fallback_icon_theme; }
  [[nodiscard]] QString uiFont() const { return appearance().ui_font; }
  [[nodiscard]] QString displayFont() const { return appearance().display_font; }
  [[nodiscard]] QString monospaceFont() const { return appearance().monospace_font; }
  [[nodiscard]] QString titleFont() const { return appearance().title_font; }
  [[nodiscard]] int uiFontSize() const { return appearance().ui_font_size; }
  [[nodiscard]] int monospaceFontSize() const { return appearance().monospace_font_size; }
  [[nodiscard]] int titleFontSize() const { return appearance().title_font_size; }
  [[nodiscard]] int displayFontSize() const { return appearance().display_font_size; }
  [[nodiscard]] qreal layoutScale() const { return appearance().layout_scale; }
  [[nodiscard]] int microSize() const { return appearance().microSize(); }
  [[nodiscard]] int captionSize() const { return appearance().captionSize(); }
  [[nodiscard]] int bodySize() const { return appearance().bodySize(); }
  [[nodiscard]] int subheadingSize() const { return appearance().subheadingSize(); }
  [[nodiscard]] int appTitleSize() const { return appearance().appTitleSize(); }
  [[nodiscard]] int headingSize() const { return appearance().headingSize(); }
  [[nodiscard]] QVariantList themeFamilies() const;
  [[nodiscard]] QVariantList themeVariants() const;

  Q_INVOKABLE QVariantList accentOptionsForScheme(const QString& scheme_id) const;
  Q_INVOKABLE QColor accentColorForScheme(const QString& scheme_id, const QString& accent_id) const;
  Q_INVOKABLE void reload();

 Q_SIGNALS:
  void themeChanged();

 private:
  explicit HolonightTheme(Holonight::AppearanceReader* reader, QObject* parent);
  [[nodiscard]] const Holonight::ResolvedAppearance& appearance() const { return reader_->appearance(); }

  Holonight::AppearanceReader* reader_ = nullptr;
};
