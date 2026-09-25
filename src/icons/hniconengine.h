// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QIconEngine>

namespace Holonight {

class HnIconEngine final : public QIconEngine {
 public:
  explicit HnIconEngine(QString name);
  [[nodiscard]] QIconEngine* clone() const override;
  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  [[nodiscard]] QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
  [[nodiscard]] QPixmap scaledPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state, qreal scale) override;
  [[nodiscard]] QString iconName() override;
  [[nodiscard]] bool isNull() override;

 private:
  QString name_;
};

}  // namespace Holonight
