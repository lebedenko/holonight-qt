// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QAccessible>
#include <QFile>
#include <QFont>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QTest>
#include <QtQml/private/qqmlcontextdata_p.h>
#include <QtQml/private/qqmldata_p.h>
#include <QtQuickTemplates2/private/qquickscrollbar_p.h>

#include <gtest/gtest.h>
#include <holonight/config/config.h>
#include <memory>

namespace {
class RuntimeComposites : public testing::Test {
 protected:
  QQmlEngine engine;
  void SetUp() override {
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
  }
  std::unique_ptr<QObject> create(const QByteArray& source) {
    QQmlComponent component(&engine);
    component.setData(source, QUrl());
    auto result = std::unique_ptr<QObject>(component.create());
    EXPECT_NE(result, nullptr) << component.errorString().toStdString();
    return result;
  }
};

void expectOrigin(QObject* object, const QString& suffix) {
  ASSERT_NE(object, nullptr);
  auto* data = QQmlData::get(object);
  ASSERT_NE(data, nullptr);
  QStringList origins;
  for (auto context = data->context; context; context = context->parent().data()) {
    const auto url = context->url().toString();
    origins.append(url);
    if (url.endsWith(suffix)) return;
  }
  FAIL() << "Expected " << suffix.toStdString() << " in " << origins.join(", ").toStdString();
}

QObject* objectProperty(QObject* object, const char* property) {
  return object ? object->property(property).value<QObject*>() : nullptr;
}

using CoreIsolation = RuntimeComposites;
TEST_F(CoreIsolation, LabelKeepsTypographyLinksWrappingAndOverridesWithoutControls) {
  auto root = create(R"(
import QtQuick
import Holonight.Core
Item {
    property alias body: body
    property alias prefix: prefix
    property alias custom: custom
    property real expectedSize: HolonightTheme.bodySize
    HnLabel { id: body; rawText: "A long label that wraps onto several lines"; width: 70; wrapMode: Text.WordWrap }
    HnLabel { id: prefix; role: HnTypographyRole.MicroHeader; showPrefix: true; prefixText: "# "; rawText: "Header" }
    HnLabel {
        id: custom
        rawText: "ignored"; text: "<a href='https://example.test'>Link</a>"; textFormat: Text.StyledText
        color: "#123456"; palette.link: "#654321"; font.pointSize: 23; font.family: "Monospace"
        property int accessibilityRole: Accessible.role
    }
})");
  ASSERT_NE(root, nullptr);
  auto* body = objectProperty(root.get(), "body");
  auto* prefix = objectProperty(root.get(), "prefix");
  auto* custom = objectProperty(root.get(), "custom");
  ASSERT_NE(body, nullptr);
  ASSERT_NE(prefix, nullptr);
  ASSERT_NE(custom, nullptr);
  EXPECT_EQ(body->property("font").value<QFont>().pointSizeF(), root->property("expectedSize").toReal());
  EXPECT_GT(body->property("implicitHeight").toReal(), body->property("font").value<QFont>().pointSizeF() * 2);
  EXPECT_TRUE(prefix->property("text").toString().contains("# </font>Header"));
  EXPECT_EQ(prefix->property("font").value<QFont>().capitalization(), QFont::AllUppercase);
  EXPECT_EQ(custom->property("color").value<QColor>(), QColor("#123456"));
  EXPECT_EQ(custom->property("linkColor").value<QColor>(), QColor("#654321"));
  EXPECT_EQ(custom->property("font").value<QFont>().pointSize(), 23);
  EXPECT_EQ(custom->property("font").value<QFont>().family(), "Monospace");
  EXPECT_TRUE(custom->property("text").toString().contains("href="));
  auto* accessible = QAccessible::queryAccessibleInterface(custom);
  ASSERT_NE(accessible, nullptr);
  EXPECT_EQ(accessible->role(), QAccessible::StaticText);
  expectOrigin(custom, "/Holonight/Core/HnLabel.qml");
  QFile maps("/proc/self/maps");
  ASSERT_TRUE(maps.open(QIODevice::ReadOnly));
  const auto mappings = maps.readAll();
  EXPECT_FALSE(mappings.contains("/Holonight/libholonight_qml.so"));
  EXPECT_FALSE(mappings.contains("/Holonight/Controls/"));
  EXPECT_FALSE(mappings.contains("/QtQuick/Controls/"));
}

TEST_F(RuntimeComposites, EveryPublicCompositeLoadsAndHonorsDisabledState) {
  for (const auto& type : QStringLiteral(HOLONIGHT_COMPOSITE_TYPES).split(',')) {
    SCOPED_TRACE(type.toStdString());
    const auto required = type == "HnAppTitle" ? "applicationName: \"Acceptance\"" : "";
    auto control = create(QString("import QtQuick\nimport Holonight.Controls\n%1 { %2 }").arg(type, required).toUtf8());
    ASSERT_NE(control, nullptr);
    expectOrigin(control.get(), "/Holonight/Controls/" + type + ".qml");
    if (qobject_cast<QQuickItem*>(control.get())) {
      ASSERT_TRUE(control->setProperty("enabled", false));
      EXPECT_FALSE(control->property("enabled").toBool());
      ASSERT_TRUE(control->setProperty("enabled", true));
    }
  }
}

TEST_F(RuntimeComposites, SelectedOriginsIncludeEditorsPopupsDelegatesAndScrollBars) {
  auto root = create(R"(
import QtQuick
import QtQuick.Controls as C
import Holonight.Controls
Window {
    width: 600; height: 600; visible: true
    property alias search: search
    property alias area: area
    property alias combo: combo
    property alias loading: loading
    property alias picker: picker
    HnSearchField { id: search }
    HnTextArea { id: area; y: 50; width: 220 }
    HnIconComboBox { id: combo; x: 280; model: 30 }
    HnLoadingState { id: loading; y: 300 }
    HnColorPicker { id: picker; y: 400 }
})");
  ASSERT_NE(root, nullptr);
  const QString style =
      qEnvironmentVariable("QT_QUICK_CONTROLS_STYLE") == "Fusion" ? "/QtQuick/Controls/Fusion/" : "/Holonight/";
  expectOrigin(objectProperty(root.get(), "search"), style + "TextField.qml");
  const auto searchFont = objectProperty(root.get(), "search")->property("font").value<QFont>();
  EXPECT_EQ(searchFont.pointSize(), 12);
  EXPECT_EQ(searchFont.family(), "Inter");
  auto* combo = objectProperty(root.get(), "combo");
  expectOrigin(combo, style + "ComboBox.qml");
  EXPECT_EQ(combo->property("font").value<QFont>(), searchFont);
  auto* popup = objectProperty(combo, "popup");
  expectOrigin(popup, style + "Popup.qml");
  ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
  QCoreApplication::processEvents();
  auto* list = objectProperty(popup, "contentItem");
  ASSERT_NE(list, nullptr);
  auto* item = objectProperty(list, "currentItem");
  ASSERT_NE(item, nullptr);
  expectOrigin(item, style + "ItemDelegate.qml");
  auto* attached = qmlAttachedPropertiesObject<QQuickScrollBar>(list);
  ASSERT_NE(attached, nullptr);
  expectOrigin(objectProperty(attached, "vertical"), style + "ScrollBar.qml");
  for (const auto name : {"area", "picker"}) {
    auto* composite = objectProperty(root.get(), name);
    QObject* scroll = nullptr;
    QObject* editor = nullptr;
    for (auto* child : composite->findChildren<QObject*>()) {
      if (child->inherits("QQuickScrollView")) scroll = child;
      if (child->inherits("QQuickTextArea")) editor = child;
    }
    ASSERT_NE(scroll, nullptr) << name;
    expectOrigin(scroll, style + "ScrollView.qml");
    attached = qmlAttachedPropertiesObject<QQuickScrollBar>(scroll);
    ASSERT_NE(attached, nullptr);
    expectOrigin(objectProperty(attached, "vertical"), style + "ScrollBar.qml");
    if (QString(name) == "area") {
      expectOrigin(editor, style + "TextArea.qml");
      EXPECT_EQ(editor->property("font").value<QFont>(), searchFont);
    }
  }
  expectOrigin(root->findChild<QObject*>("hnLoadingProgress"), style + "ProgressBar.qml");
  QFile maps("/proc/self/maps");
  ASSERT_TRUE(maps.open(QIODevice::ReadOnly));
  const auto mappings = maps.readAll();
  const auto importRoot = qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)).toUtf8();
  for (const auto module :
       {"/Holonight/Core/libholonight_core_qml.so", "/Holonight/Controls/libholonight_controls_qml.so",
        "/Holonight/impl/libholonight_impl_qml.so"})
    EXPECT_TRUE(mappings.contains(importRoot + module)) << module;
}

TEST_F(RuntimeComposites, PublicVisualHooksAndCallerOverridesRemainAuthoritative) {
  auto root = create(R"(
import QtQuick
import Holonight.Controls
Item {
    property alias search: search
    property alias combo: combo
    property alias area: area
    function undoEdit() { area.undo() }
    function redoEdit() { area.redo() }
    function insertText() { area.insert(0, "hello") }
    function removeText() { area.remove(1, 4) }
    HnSearchField {
        id: search; font.pointSize: 22; color: "#123456"
        background: Rectangle { objectName: "replacement"; implicitWidth: 300; color: "#654321" }
    }
    HnIconComboBox {
        id: combo; font.pointSize: 21; model: ["first", "last"]
        contentItem: Item { objectName: "replacementContent"; implicitWidth: 150; implicitHeight: 25 }
        indicator: Item { implicitWidth: 16; implicitHeight: 16 }
        delegateHeight: 42; maximumVisibleItems: 3
    }
    HnTextArea { id: area; font.pointSize: 20 }
})");
  ASSERT_NE(root, nullptr);
  auto* search = objectProperty(root.get(), "search");
  auto* combo = objectProperty(root.get(), "combo");
  auto* area = objectProperty(root.get(), "area");
  ASSERT_NE(search, nullptr);
  ASSERT_NE(combo, nullptr);
  ASSERT_NE(area, nullptr);
  EXPECT_EQ(search->property("font").value<QFont>().pointSize(), 22);
  EXPECT_EQ(search->property("color").value<QColor>(), QColor("#123456"));
  EXPECT_EQ(objectProperty(search, "background")->objectName(), "replacement");
  EXPECT_EQ(combo->property("font").value<QFont>().pointSize(), 21);
  EXPECT_EQ(objectProperty(combo, "contentItem")->objectName(), "replacementContent");
  EXPECT_EQ(combo->property("delegateHeight").toReal(), 42);
  EXPECT_EQ(combo->property("maximumVisibleItems").toInt(), 3);
  EXPECT_EQ(area->property("font").value<QFont>().pointSize(), 20);
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "insertText"));
  EXPECT_EQ(area->property("text").toString(), "hello");
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "undoEdit"));
  EXPECT_EQ(area->property("text").toString(), "");
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "redoEdit"));
  EXPECT_EQ(area->property("text").toString(), "hello");
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "removeText"));
  EXPECT_EQ(area->property("text").toString(), "ho");
}

TEST_F(RuntimeComposites, EditingErrorFramesContentReplacementAndReload) {
  auto root = create(R"(
import QtQuick
import Holonight.Core
import Holonight.Controls
Window {
    width: 500; height: 500; visible: true
    property alias search: search
    property alias area: area
    property color urgent: HoloniightPalette.borderUrgent
    property color surface: HoloniightPalette.surface
    function reload() { HoloniightPalette.reload(); HolonightTheme.reload(); HnAppearance.reload() }
    function edit() { area.insert(0, "abcdef"); area.select(1, 4) }
    function unwrap() { area.wrapMode = TextEdit.NoWrap }
    function wrap() { area.wrapMode = TextEdit.Wrap }
    function replaceSlots() { search.leadingContent = slot; search.trailingContent = slot; area.footerContent = slot }
    Component { id: slot; Item { implicitWidth: 24; implicitHeight: 20 } }
    HnSearchField { id: search; width: 240; text: "query"; hasError: true }
    HnTextArea { id: area; y: 60; width: 130; minimumVisibleLines: 1; maximumVisibleLines: 2; maximumLength: 5; hasError: true }
})");
  ASSERT_NE(root, nullptr);
  auto* search = objectProperty(root.get(), "search");
  auto* area = objectProperty(root.get(), "area");
  ASSERT_NE(search, nullptr);
  ASSERT_NE(area, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "edit"));
  EXPECT_EQ(area->property("text").toString(), "abcde");
  EXPECT_EQ(area->property("selectedText").toString(), "bcd");
  EXPECT_EQ(area->property("selectionStart").toInt(), 1);
  EXPECT_EQ(area->property("selectionEnd").toInt(), 4);
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "replaceSlots"));
  QCoreApplication::processEvents();
  EXPECT_GT(search->property("leftPadding").toReal(), 24);
  EXPECT_GT(search->property("rightPadding").toReal(), 24);
  EXPECT_EQ(area->findChild<QQuickItem*>("hnTextAreaFooter")->implicitHeight(), 20);
  auto* clear = search->findChild<QObject*>("hnSearchClearButton");
  ASSERT_NE(clear, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(clear, "clicked"));
  EXPECT_TRUE(search->property("text").toString().isEmpty());
  ASSERT_TRUE(area->setProperty("maximumLength", -1));
  ASSERT_TRUE(area->setProperty("text", QString("long words ").repeated(50)));
  QCoreApplication::processEvents();
  EXPECT_TRUE(area->property("overflow").toBool());
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "unwrap"));
  QCoreApplication::processEvents();
  EXPECT_FALSE(area->property("overflow").toBool());
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "wrap"));
  QCoreApplication::processEvents();
  EXPECT_TRUE(area->property("overflow").toBool());
  auto* background = qobject_cast<QQuickItem*>(objectProperty(search, "background"));
  ASSERT_NE(background, nullptr);
  ASSERT_TRUE(QTest::qWaitFor(
      [&] { return background->property("color").value<QColor>() == root->property("surface").value<QColor>(); }));
  const auto initial = background->property("color").value<QColor>();
  auto appearance = HoloNight::Config::defaults();
  appearance.theme.scheme = "holonight-light";
  const auto path = qEnvironmentVariable("HOLONIGHT_APPEARANCE_FILE").toStdString();
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path));
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "reload"));
  ASSERT_TRUE(QTest::qWaitFor([&] { return background->property("color").value<QColor>() != initial; }));
  ASSERT_TRUE(QTest::qWaitFor(
      [&] { return background->property("color").value<QColor>() == root->property("surface").value<QColor>(); }));
  auto* border = objectProperty(background, "border");
  ASSERT_NE(border, nullptr);
  ASSERT_TRUE(QTest::qWaitFor(
      [&] { return border->property("color").value<QColor>() == root->property("urgent").value<QColor>(); }));
  search->setProperty("enabled", false);
  EXPECT_EQ(background->opacity(), 0.5);
  EXPECT_FALSE(search->property("clearButtonVisible").toBool());
  appearance.theme.scheme = "holonight-dark";
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, path));
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "reload"));
}
}  // namespace
