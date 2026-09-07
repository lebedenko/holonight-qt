// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QColor>
#include <QQmlComponent>
#include <QQmlEngine>

#include <gtest/gtest.h>
#include <memory>

TEST(ControlPalette, BlendsRgbaAndLeavesUnrelatedRolesIndependent) {
  QQmlEngine engine;
  engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  QQmlComponent component(&engine);
  component.setData(R"(
import QtQuick
import Holonight.impl
ControlPalette {
    textRole: ControlPalette.ButtonText
    fillRole: ControlPalette.Button
    palette: Palette {
        base: "#800000ff"
        button: "#00000000"
        buttonText: "#ffffffff"
        highlight: "#ffff0000"
        highlightedText: "#ffffffff"
        mid: "#ff00ff00"
        link: "#40556677"
        shadow: "#80663399"
    }
})",
                    QUrl());
  std::unique_ptr<QObject> object(component.create());
  ASSERT_TRUE(object) << component.errorString().toStdString();
  const auto colors = object->property("colors").toMap();
  auto near = [](QColor actual, QColor expected) {
    EXPECT_NEAR(actual.redF(), expected.redF(), 0.00002);
    EXPECT_NEAR(actual.greenF(), expected.greenF(), 0.00002);
    EXPECT_NEAR(actual.blueF(), expected.blueF(), 0.00002);
    EXPECT_NEAR(actual.alphaF(), expected.alphaF(), 0.00002);
  };
  near(colors.value("surfaceHover").value<QColor>(), QColor::fromRgbF(0.1, 0.1, 0.1, 0.1));
  near(colors.value("primaryPressed").value<QColor>(), QColor::fromRgbF(0.8, 0, 0.2, 0.8 + 0.2 * 128.0 / 255));
  near(colors.value("surfaceSelected").value<QColor>(), QColor::fromRgbF(0.12, 0, 0.88, 0.12 + 0.88 * 128.0 / 255));
  near(colors.value("surfaceSelectedHover").value<QColor>(), QColor::fromRgbF(0.2, 0, 0.8, 0.2 + 0.8 * 128.0 / 255));
  near(colors.value("borderSubtle").value<QColor>(), QColor::fromRgbF(0, 0.5, 0.5, 0.5 + 0.5 * 128.0 / 255));
  EXPECT_EQ(colors.value("surfaceRaised").value<QColor>(), QColor(Qt::transparent));
  EXPECT_EQ(colors.value("surface").value<QColor>(), QColor("#800000ff"));
  EXPECT_EQ(colors.value("borderFocus").value<QColor>(), QColor(Qt::red));
  EXPECT_EQ(colors.value("link").value<QColor>(), QColor("#40556677"));
  near(colors.value("modalOverlay").value<QColor>(), QColor::fromRgbF(0.4, 0.2, 0.6, 128.0 / 255 * 0.5));
  near(colors.value("modelessOverlay").value<QColor>(), QColor::fromRgbF(0.4, 0.2, 0.6, 128.0 / 255 * 0.12));
}
