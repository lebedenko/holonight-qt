// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once
#include "holonight/palette.h"

#include <QObject>
#include <QPalette>
#include <QQmlParserStatus>
#include <QVariantMap>
#include <QtQml/qqml.h>
#include <QtQuick/private/qquickpalette_p.h>

class ControlPalette : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QQuickPalette* palette READ palette WRITE setPalette NOTIFY changed)
  Q_PROPERTY(QQuickPalette* inheritFrom READ inheritFrom WRITE setInheritFrom NOTIFY changed)
  Q_PROPERTY(int colorGroup MEMBER group_ NOTIFY changed)
  Q_PROPERTY(int textRole MEMBER text_role_ NOTIFY changed)
  Q_PROPERTY(int fillRole MEMBER fill_role_ NOTIFY changed)
  Q_PROPERTY(QVariantMap colors READ colors NOTIFY changed)
  Q_PROPERTY(QQuickPalette* appearancePalette READ qmlAppearancePalette NOTIFY changed)
  Q_PROPERTY(bool defaultDisabled READ defaultDisabled NOTIFY changed)
 public:
  enum ColorGroup { Active = QPalette::Active, Disabled = QPalette::Disabled, Inactive = QPalette::Inactive };
  Q_ENUM(ColorGroup)
  enum Role {
    WindowText = QPalette::WindowText,
    Text = QPalette::Text,
    ButtonText = QPalette::ButtonText,
    Base = QPalette::Base,
    Button = QPalette::Button
  };
  Q_ENUM(Role)
  explicit ControlPalette(QObject* parent = nullptr) : QObject(parent) {}
  QQuickPalette* palette() const { return source_; }
  void setPalette(QQuickPalette* palette);
  QQuickPalette* inheritFrom() const { return inherit_from_; }
  void setInheritFrom(QQuickPalette* palette);
  void classBegin() override {}
  void componentComplete() override;
  QVariantMap colors() const;
  QQuickPalette* qmlAppearancePalette() { return &appearance_palette_; }
  QPalette appearancePalette() const { return Holonight::buildPalette(tokens_); }
  bool defaultDisabled() const;
 signals:
  void changed();

 private:
  void refreshPalette();
  bool refreshing_ = false;
  QPointer<QQuickPalette> inherit_from_;
  QQuickPalette appearance_palette_;
  QPointer<QQuickPalette> source_;
  QPalette palette_;
  int group_ = QPalette::Active;
  int text_role_ = QPalette::WindowText;
  int fill_role_ = QPalette::Base;
  Holonight::ColorTokens tokens_ = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
};
