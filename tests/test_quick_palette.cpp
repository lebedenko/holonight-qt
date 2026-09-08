// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "themeresolver.h"

#include <QGuiApplication>
#include <QPalette>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTest>
#include <QtQuickTemplates2/private/qquickcontrol_p.h>

#include <gtest/gtest.h>
#include <holonight/config/config.h>
#include <memory>

namespace {
QColor logicalPixel(const QImage& image, const QQuickWindow* window, int x, int y) {
  // grabWindow() returns physical pixels, but the software backend leaves the
  // image's DPR metadata at 1. Use the window's actual rendering ratio.
  return image.pixelColor(qRound(x * window->devicePixelRatio()), qRound(y * window->devicePixelRatio()));
}

class QuickPalette : public testing::Test {
 protected:
  QPalette saved = QGuiApplication::palette();
  QQmlEngine engine;
  void SetUp() override {
    QCoreApplication::processEvents();
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
  }
  void TearDown() override {
    QGuiApplication::setPalette(saved);
    QCoreApplication::processEvents();
  }
  std::unique_ptr<QObject> create(const QByteArray& source) {
    QQmlComponent component(&engine);
    component.setData(source, QUrl());
    auto result = std::unique_ptr<QObject>(component.create());
    EXPECT_TRUE(result) << component.errorString().toStdString();
    return result;
  }
};

TEST_F(QuickPalette, ApplicationBlackWhiteBlackUpdatesEditableColors) {
  auto window = create(
      "import QtQuick; import Holonight; Window { TextField { objectName: 'field'; text: 'sample'; placeholderText: "
      "'hint' } }");
  ASSERT_TRUE(window);
  auto* field = window->findChild<QObject*>("field");
  ASSERT_TRUE(field);
  auto* background = field->property("background").value<QQuickItem*>();
  ASSERT_TRUE(background);
  for (const QColor base : {QColor(Qt::black), QColor(Qt::white), QColor(Qt::black)}) {
    QPalette palette;
    const QColor text = base == Qt::black ? Qt::white : Qt::black;
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::PlaceholderText, QColor("#9374ab"));
    palette.setColor(QPalette::Highlight, QColor("#557799"));
    palette.setColor(QPalette::HighlightedText, QColor("#ffeedd"));
    QGuiApplication::setPalette(palette);
    QCoreApplication::processEvents();
    EXPECT_EQ(background->property("color").value<QColor>(), base);
    EXPECT_EQ(field->property("color").value<QColor>(), text);
    EXPECT_EQ(field->property("placeholderTextColor").value<QColor>(), QColor("#9374ab"));
    EXPECT_EQ(field->property("selectionColor").value<QColor>(), QColor("#557799"));
    EXPECT_EQ(field->property("selectedTextColor").value<QColor>(), QColor("#ffeedd"));
  }
}
}  // namespace

namespace {
TEST_F(QuickPalette, PartialInheritanceChildOverrideSiblingIsolationAndReset) {
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    ItemDelegate {
        palette.base: "#123456"
        TextField { objectName: "first"; palette.text: "#fedcba" }
        TextField { objectName: "second" }
        TextField {
            objectName: "child"
            palette.base: "transparent"
            function resetBase() { palette.base = undefined }
        }
    }
})");
  ASSERT_TRUE(window);
  auto* first = window->findChild<QObject*>("first");
  auto* second = window->findChild<QObject*>("second");
  auto* child = window->findChild<QObject*>("child");
  ASSERT_TRUE(first && second && child);
  auto background = [](QObject* object) {
    return object->property("background").value<QQuickItem*>()->property("color").value<QColor>();
  };
  EXPECT_EQ(background(first), QColor("#123456"));
  EXPECT_EQ(background(second), QColor("#123456"));
  EXPECT_EQ(background(child), QColor(Qt::transparent));
  EXPECT_EQ(first->property("color").value<QColor>(), QColor("#fedcba"));
  EXPECT_NE(second->property("color").value<QColor>(), QColor("#fedcba"));
  ASSERT_TRUE(QMetaObject::invokeMethod(child, "resetBase"));
  EXPECT_EQ(background(child), QColor("#123456"));
}

TEST_F(QuickPalette, DisabledAndInactiveGroupsPreserveExplicitAlpha) {
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    TextField {
        objectName: "field"
        palette.active.text: "#123456"
        palette.inactive.text: "#abcdef"
        palette.disabled.text: "#80654321"
        palette.disabled.base: "#40224466"
    }
})");
  ASSERT_TRUE(window);
  auto* field = window->findChild<QObject*>("field");
  ASSERT_TRUE(field);
  EXPECT_EQ(field->property("color").value<QColor>(), QColor("#abcdef"));
  field->setProperty("enabled", false);
  EXPECT_EQ(field->property("color").value<QColor>(), QColor("#80654321"));
  auto* background = field->property("background").value<QQuickItem*>();
  EXPECT_EQ(background->property("color").value<QColor>(), QColor("#40224466"));
  EXPECT_EQ(background->opacity(), 1.0);
}

TEST_F(QuickPalette, AccentOverrideDoesNotChangeUnrelatedSurfaces) {
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    TextField { objectName: "defaultField" }
    TextField { objectName: "accentField"; palette.highlight: "#80882266" }
})");
  ASSERT_TRUE(window);
  auto* normal = window->findChild<QObject*>("defaultField");
  auto* accent = window->findChild<QObject*>("accentField");
  ASSERT_TRUE(normal && accent);
  auto background = [](QObject* object) {
    return object->property("background").value<QQuickItem*>()->property("color").value<QColor>();
  };
  EXPECT_EQ(background(normal), background(accent));
  EXPECT_EQ(normal->property("color"), accent->property("color"));
  EXPECT_EQ(accent->property("selectionColor").value<QColor>(), QColor("#80882266"));
}
}  // namespace

namespace {
Holonight::ColorTokens appearanceTokens() {
  auto loaded = HoloNight::Config::load(*HoloNight::Config::resolveAppearancePath().value);
  return Holonight::ThemeResolver::resolve(*Holonight::resolveAppearance(loaded.value->appearance).value);
}

TEST_F(QuickPalette, DefaultSemanticColorsRemainExact) {
  const auto tokens = appearanceTokens();
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    TextField { objectName: "field" }
    Button { objectName: "button" }
    ToolTip { objectName: "tooltip" }
    ItemDelegate { objectName: "selected"; highlighted: true }
})");
  ASSERT_TRUE(window);
  auto* field = window->findChild<QObject*>("field");
  auto* button = window->findChild<QObject*>("button");
  auto* tooltip = window->findChild<QObject*>("tooltip");
  ASSERT_TRUE(field && button && tooltip);
  auto background = [](QObject* object) {
    return object->property("background").value<QQuickItem*>()->property("color").value<QColor>();
  };
  EXPECT_EQ(background(field), tokens.surface);
  EXPECT_EQ(field->property("color").value<QColor>(), tokens.textPrimary);
  EXPECT_EQ(field->property("placeholderTextColor").value<QColor>(), tokens.textMuted);
  auto selection = tokens.primary;
  selection.setAlphaF(0.3);
  EXPECT_EQ(field->property("selectionColor").value<QColor>(), selection);
  EXPECT_EQ(background(button), tokens.surfaceRaised);
  EXPECT_EQ(background(tooltip), tokens.surfaceRaised);
  EXPECT_EQ(tooltip->property("contentItem").value<QQuickItem*>()->property("color").value<QColor>(),
            tokens.textPrimary);
  // Semantic resolver outputs include states whose values intentionally differ from palette roles.
  auto* resolver = button->property("controlColors").value<QObject*>();
  ASSERT_TRUE(resolver);
  const auto colors = resolver->property("colors").toMap();
  EXPECT_EQ(colors.value("primaryHover").value<QColor>(), tokens.primaryHover);
  EXPECT_EQ(colors.value("primaryPressed").value<QColor>(), tokens.primaryPressed);
  EXPECT_EQ(colors.value("surfaceHover").value<QColor>(), tokens.surfaceHover);
  EXPECT_EQ(colors.value("surfaceSelected").value<QColor>(), tokens.surfaceSelected);
  EXPECT_EQ(colors.value("surfaceSelectedHover").value<QColor>(), tokens.surfaceSelectedHover);
  EXPECT_EQ(colors.value("borderFocus").value<QColor>(), tokens.borderFocus);
  EXPECT_EQ(colors.value("borderSubtle").value<QColor>(), tokens.borderSubtle);
  field->setProperty("enabled", false);
  EXPECT_EQ(field->property("color").value<QColor>(), tokens.textDisabled);
  EXPECT_EQ(field->property("background").value<QQuickItem*>()->opacity(), 0.5);
}

TEST_F(QuickPalette, CompositesKeepAppearanceWhileStandardControlsFollowApplication) {
  QPalette palette;
  palette.setColor(QPalette::Base, QColor("#010203"));
  palette.setColor(QPalette::Text, QColor("#abcdef"));
  QGuiApplication::setPalette(palette);
  auto window = create(R"(
import QtQuick
import Holonight
import Holonight.Controls
Window {
    TextField { objectName: "standard" }
    HnSearchField { objectName: "composite" }
    HnIconComboBox { objectName: "combo"; model: ["one", "two"] }
})");
  ASSERT_TRUE(window);
  auto* standard = window->findChild<QObject*>("standard");
  auto* composite = window->findChild<QObject*>("composite");
  ASSERT_TRUE(standard && composite);
  QCoreApplication::processEvents();
  EXPECT_EQ(standard->property("color").value<QColor>(), QColor("#abcdef"));
  EXPECT_EQ(composite->property("color").value<QColor>(), appearanceTokens().textPrimary);
  EXPECT_EQ(composite->property("background").value<QQuickItem*>()->property("color").value<QColor>(),
            appearanceTokens().surface);
}

TEST_F(QuickPalette, MenusPopupsAndAttachedScrollBarsInheritPalette) {
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    width: 500; height: 500
    palette.button: "#654321"
    palette.buttonText: "#abcdef"
    ItemDelegate {
        width: 400; height: 400
        palette.base: "#123456"
        palette.button: "#654321"
        palette.buttonText: "#abcdef"
        palette.placeholderText: "#556677"
        Menu { objectName: "menu"; MenuItem { objectName: "menuItem"; text: "one" } }
        ComboBox { objectName: "combo"; model: ["one", "two"] }
        ScrollView {
            objectName: "scrollView"; width: 100; height: 100
            contentWidth: 500; contentHeight: 500
            property var attachedBar: ScrollBar.vertical
        }
    }
})");
  ASSERT_TRUE(window);
  auto* menu = window->findChild<QObject*>("menu");
  auto* item = window->findChild<QObject*>("menuItem");
  auto* combo = window->findChild<QObject*>("combo");
  auto* scroll = window->findChild<QObject*>("scrollView");
  ASSERT_TRUE(menu && item && combo && scroll);
  ASSERT_TRUE(QMetaObject::invokeMethod(menu, "open"));
  auto* popup = combo->property("popup").value<QObject*>();
  ASSERT_TRUE(popup);
  ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
  QCoreApplication::processEvents();
  EXPECT_EQ(menu->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#654321"));
  EXPECT_EQ(item->property("foregroundColor").value<QColor>(), QColor("#abcdef"));
  EXPECT_EQ(popup->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#123456"));
  auto* bar = scroll->property("attachedBar").value<QObject*>();
  ASSERT_TRUE(bar);
  EXPECT_EQ(bar->property("contentItem").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#556677"));
}
}  // namespace

namespace {
TEST_F(QuickPalette, AppearanceReloadKeepsOverridesAndUpdatesUnspecifiedRolesAndComposites) {
  const auto path = *HoloNight::Config::resolveAppearancePath().value;
  auto loaded = HoloNight::Config::load(path);
  ASSERT_TRUE(loaded);
  const auto original = loaded.value->appearance;
  auto window = create(R"(
import QtQuick
import Holonight
import Holonight.Controls
Window {
    TextField { objectName: "standard" }
    HnSearchField { objectName: "composite" }
    Label { objectName: "newLabel" }
    ToolBar { objectName: "newBar" }
    Popup { objectName: "newPopup" }
})");
  ASSERT_TRUE(window);
  auto* standard = window->findChild<QObject*>("standard");
  auto* composite = window->findChild<QObject*>("composite");
  ASSERT_TRUE(standard && composite);
  ASSERT_TRUE(QMetaObject::invokeMethod(window->findChild<QObject*>("newPopup"), "open"));
  QCoreApplication::processEvents();  // Initialize the GUI-thread style watcher.
  QPalette palette;
  palette.setColor(QPalette::Base, QColor("#010203"));
  QGuiApplication::setPalette(palette);
  const auto mask = QGuiApplication::palette().resolveMask();
  auto changed = original;
  changed.theme.scheme = original.theme.scheme == "holonight-light" ? "holonight-dark" : "holonight-light";
  const auto expected = Holonight::ThemeResolver::resolve(*Holonight::resolveAppearance(changed).value);
  ASSERT_TRUE(HoloNight::Config::writeAtomically(changed, path));
  EXPECT_TRUE(QTest::qWaitFor(
      [&] {
        return standard->property("color").value<QColor>() == expected.textPrimary &&
               composite->property("background").value<QQuickItem*>()->property("color").value<QColor>() ==
                   expected.surface;
      },
      3000));
  EXPECT_EQ(standard->property("background").value<QQuickItem*>()->property("color").value<QColor>(),
            QColor("#010203"));
  EXPECT_EQ(window->findChild<QObject*>("newLabel")->property("color").value<QColor>(), expected.textPrimary);
  for (const char* name : {"newBar", "newPopup"})
    EXPECT_EQ(window->findChild<QObject*>(name)
                  ->property("background")
                  .value<QQuickItem*>()
                  ->property("color")
                  .value<QColor>(),
              expected.surfaceRaised);
  EXPECT_EQ(QGuiApplication::palette().resolveMask(), mask);
  EXPECT_EQ(QGuiApplication::palette().color(QPalette::Base), QColor("#010203"));
  ASSERT_TRUE(HoloNight::Config::writeAtomically(original, path));
  const auto restored = Holonight::ThemeResolver::resolve(*Holonight::resolveAppearance(original).value);
  EXPECT_TRUE(
      QTest::qWaitFor([&] { return standard->property("color").value<QColor>() == restored.textPrimary; }, 3000));
}
}  // namespace

namespace {
TEST_F(QuickPalette, ExplicitDisabledColorsEqualToDefaultsAreNotDimmedAgain) {
  const auto tokens = appearanceTokens();
  QPalette palette;
  palette.setColor(QPalette::Disabled, QPalette::Base, tokens.surface);
  palette.setColor(QPalette::Disabled, QPalette::Text, tokens.textDisabled);
  QGuiApplication::setPalette(palette);
  auto window =
      create("import QtQuick; import Holonight; Window { TextField { objectName: 'field'; enabled: false } }");
  ASSERT_TRUE(window);
  QCoreApplication::processEvents();
  auto* field = window->findChild<QObject*>("field");
  ASSERT_TRUE(field);
  EXPECT_EQ(field->property("color").value<QColor>(), tokens.textDisabled);
  EXPECT_EQ(field->property("background").value<QQuickItem*>()->opacity(), 1.0);
}

TEST_F(QuickPalette, EveryStandardControlAcceptsWindowPaletteWithoutQmlErrors) {
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    palette.base: "#201020"
    palette.button: "#302030"
    palette.text: "#fedcba"
    palette.windowText: "#abcdef"
    palette.buttonText: "#f0e0d0"
    palette.highlight: "#774499"
    Button {} CheckBox {} ComboBox { model: ["one", "two"] } ItemDelegate {}
    Menu { MenuItem {} } ProgressBar {} RadioButton {} ScrollBar {} ScrollView {}
    Slider {} SpinBox {} Switch {} TabBar { TabButton {} } TextArea {} TextField {} ToolTip {}
})");
  ASSERT_TRUE(window);
}
}  // namespace

namespace {
TEST_F(QuickPalette, OffscreenRenderingUsesHoverPressedSelectedAndDisabledPaletteColors) {
  auto object = create(R"(
import QtQuick
import Holonight
Window {
    width: 240; height: 60; visible: true; color: "white"
    palette.base: "blue"
    palette.button: "black"
    palette.buttonText: "white"
    palette.highlight: "red"
    palette.highlightedText: "white"
    palette.disabled.button: "#336699"
    Button { objectName: "hover"; width: 50; height: 50 }
    Button { x: 60; width: 50; height: 50; highlighted: true; down: true }
    ItemDelegate { x: 120; width: 50; height: 50; highlighted: true }
    Button { x: 180; width: 50; height: 50; enabled: false }
})");
  ASSERT_TRUE(object);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_TRUE(window);
  auto* hover = window->findChild<QQuickControl*>("hover");
  ASSERT_TRUE(hover);
  // Set an isolated control state directly; no mouse, focus or desktop automation.
  hover->setHovered(true);
  QImage rendered;
  EXPECT_TRUE(QTest::qWaitFor(
      [&] {
        rendered = window->grabWindow();
        return !rendered.isNull() && logicalPixel(rendered, window, 25, 25) == QColor("#1a1a1a");
      },
      2000));
  ASSERT_FALSE(rendered.isNull());
  EXPECT_EQ(rendered.size(), QSize(qRound(window->width() * window->devicePixelRatio()),
                                   qRound(window->height() * window->devicePixelRatio())));
  EXPECT_EQ(logicalPixel(rendered, window, 85, 25), QColor("#cc0033"));
  EXPECT_EQ(logicalPixel(rendered, window, 145, 25), QColor("#1f00e0"));
  EXPECT_EQ(logicalPixel(rendered, window, 205, 25), QColor("#336699"));
}
}  // namespace

namespace {
TEST_F(QuickPalette, ActiveGroupAndNestedPopupOverridesRemainLocal) {
  auto field = create(R"(
import QtQuick
import Holonight
TextField { palette.active.text: "#123456"; palette.inactive.text: "#abcdef" }
)");
  ASSERT_TRUE(field);
  EXPECT_EQ(field->property("color").value<QColor>(), QColor("#123456"));
  auto window = create(R"(
import QtQuick
import Holonight
Window {
    width: 500; height: 500
    ComboBox {
        objectName: "combo"; model: ["one", "two"]
        palette.base: "#123456"
        popup.palette.base: "#654321"
        function resetPopupBase() { popup.palette.base = undefined }
    }
}
)");
  ASSERT_TRUE(window);
  auto* combo = window->findChild<QObject*>("combo");
  ASSERT_TRUE(combo);
  auto* popup = combo->property("popup").value<QObject*>();
  ASSERT_TRUE(popup);
  ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
  QCoreApplication::processEvents();
  EXPECT_EQ(combo->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#123456"));
  EXPECT_EQ(popup->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#654321"));
  ASSERT_TRUE(combo->property("palette").value<QObject*>()->setProperty("base", QColor("#102030")));
  EXPECT_EQ(combo->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#102030"));
  EXPECT_EQ(popup->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#654321"));
  ASSERT_TRUE(QMetaObject::invokeMethod(combo, "resetPopupBase"));
  EXPECT_EQ(popup->property("background").value<QQuickItem*>()->property("color").value<QColor>(), QColor("#102030"));
}
}  // namespace

namespace {
TEST_F(QuickPalette, WindowLabelAndToolbarDefaultsAndOverrides) {
  auto window = create(R"(
import QtQuick
import Holonight
ApplicationWindow {
    width: 400; height: 300
    header: ToolBar { objectName: "toolbar"; Label { text: "Header" } }
    footer: ToolBar { objectName: "footer" }
    Label { objectName: "label"; text: "A long label with a link" }
    Label { objectName: "override"; palette.windowText: "transparent"
        function resetText() { palette.windowText = undefined } }
    ToolButton { objectName: "tool"; text: "Action" }
    ToolSeparator { objectName: "vertical" }
    ToolSeparator { objectName: "horizontal"; orientation: Qt.Horizontal }
    MenuSeparator { objectName: "separator" }
})");
  ASSERT_TRUE(window);
  const auto tokens = appearanceTokens();
  EXPECT_EQ(window->property("color").value<QColor>(), tokens.background);
  auto* label = window->findChild<QObject*>("label");
  auto* local = window->findChild<QObject*>("override");
  auto* tool = window->findChild<QObject*>("tool");
  ASSERT_TRUE(label && local && tool);
  EXPECT_EQ(label->property("color").value<QColor>(), tokens.textPrimary);
  EXPECT_EQ(label->property("linkColor").value<QColor>(), Holonight::buildPalette(tokens).color(QPalette::Link));
  EXPECT_EQ(local->property("color").value<QColor>(), QColor(Qt::transparent));
  for (const char* name : {"toolbar", "footer"}) {
    auto* bar = window->findChild<QObject*>(name);
    ASSERT_TRUE(bar);
    EXPECT_EQ(bar->property("background").value<QQuickItem*>()->property("color").value<QColor>(),
              tokens.surfaceRaised);
    EXPECT_GT(bar->property("implicitHeight").toReal(), 0);
  }
  for (const char* name : {"vertical", "horizontal", "separator"}) {
    auto* separator = window->findChild<QObject*>(name);
    ASSERT_TRUE(separator);
    EXPECT_EQ(separator->property("contentItem").value<QQuickItem*>()->property("color").value<QColor>(),
              tokens.borderSubtle);
  }
  auto* vertical = window->findChild<QObject*>("vertical");
  auto* horizontal = window->findChild<QObject*>("horizontal");
  EXPECT_EQ(vertical->property("implicitWidth"), horizontal->property("implicitHeight"));
  EXPECT_EQ(vertical->property("implicitHeight"), horizontal->property("implicitWidth"));
  auto* separator = window->findChild<QObject*>("separator");
  EXPECT_EQ(separator->property("implicitWidth").toReal(), 180);
  separator->setProperty("padding", 10);
  EXPECT_EQ(separator->property("implicitWidth").toReal(), 196);
  auto* background = tool->property("background").value<QQuickItem*>();
  EXPECT_EQ(background->property("color").value<QColor>(), QColor(Qt::transparent));
  tool->setProperty("checked", true);
  EXPECT_EQ(background->property("color").value<QColor>(), tokens.surfaceSelected);
  tool->setProperty("highlighted", true);
  EXPECT_EQ(background->property("color").value<QColor>(), tokens.primary);
  QPalette palette;
  palette.setColor(QPalette::Window, QColor("#123456"));
  palette.setColor(QPalette::WindowText, QColor("#abcdef"));
  palette.setColor(QPalette::Link, QColor("#80553377"));
  QGuiApplication::setPalette(palette);
  QCoreApplication::processEvents();
  EXPECT_EQ(window->property("color").value<QColor>(), QColor("#123456"));
  EXPECT_EQ(label->property("color").value<QColor>(), QColor("#abcdef"));
  EXPECT_EQ(label->property("linkColor").value<QColor>(), QColor("#80553377"));
  EXPECT_EQ(local->property("color").value<QColor>(), QColor(Qt::transparent));
  ASSERT_TRUE(QMetaObject::invokeMethod(local, "resetText"));
  EXPECT_EQ(local->property("color").value<QColor>(), QColor("#abcdef"));
}
}  // namespace

namespace {
TEST_F(QuickPalette, PopupAndMenuBarDefaultsSizingAndLifecycle) {
  auto window = create(R"(
import QtQuick
import Holonight
ApplicationWindow {
    width: 400; height: 300
    menuBar: MenuBar {
        objectName: "bar"
        Menu { title: "File"; MenuItem { text: "Open" } }
        Menu { title: "Edit" }
        property var first: itemAt(0)
        property var second: itemAt(1)
    }
    ItemDelegate {
        palette.button: "#123456"
        Popup { objectName: "popup"; contentItem: Item { implicitWidth: 73; implicitHeight: 41 } }
        Popup { objectName: "local"; palette.button: "transparent"; padding: 0
            contentItem: Item { implicitWidth: 23; implicitHeight: 17 }
            function resetButton() { palette.button = undefined }
        }
    }
})");
  ASSERT_TRUE(window);
  const auto tokens = appearanceTokens();
  auto* popup = window->findChild<QObject*>("popup");
  auto* local = window->findChild<QObject*>("local");
  auto* bar = window->findChild<QObject*>("bar");
  ASSERT_TRUE(popup && local && bar);
  auto background = [](QObject* item) { return item->property("background").value<QQuickItem*>(); };
  EXPECT_EQ(background(popup)->property("color").value<QColor>(), tokens.surfaceRaised);
  EXPECT_EQ(background(bar)->property("color").value<QColor>(), tokens.surfaceRaised);
  EXPECT_EQ(popup->property("implicitWidth").toReal(), 97);
  EXPECT_EQ(popup->property("implicitHeight").toReal(), 65);
  EXPECT_EQ(local->property("implicitWidth").toReal(), 23);
  EXPECT_EQ(local->property("implicitHeight").toReal(), 17);
  EXPECT_EQ(background(local)->property("color").value<QColor>(), QColor(Qt::transparent));
  auto* first = bar->property("first").value<QObject*>();
  auto* second = bar->property("second").value<QObject*>();
  ASSERT_TRUE(first && second);
  EXPECT_EQ(first->property("text").toString(), "File");
  EXPECT_EQ(second->property("text").toString(), "Edit");
  EXPECT_EQ(background(first)->property("color").value<QColor>(), QColor(Qt::transparent));
  first->setProperty("highlighted", true);
  EXPECT_EQ(background(first)->property("color").value<QColor>(), tokens.primary);
  first->setProperty("down", true);
  EXPECT_EQ(background(first)->property("color").value<QColor>(), tokens.primaryPressed);
  auto* palette = window->property("palette").value<QObject*>();
  ASSERT_TRUE(palette);
  ASSERT_TRUE(palette->setProperty("button", QColor("#654321")));
  EXPECT_EQ(background(popup)->property("color").value<QColor>(), QColor("#654321"));
  EXPECT_EQ(background(local)->property("color").value<QColor>(), QColor(Qt::transparent));
  ASSERT_TRUE(QMetaObject::invokeMethod(local, "resetButton"));
  EXPECT_EQ(background(local)->property("color").value<QColor>(), QColor("#654321"));
  ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
  EXPECT_TRUE(popup->property("visible").toBool());
  ASSERT_TRUE(QMetaObject::invokeMethod(popup, "close"));
  EXPECT_FALSE(popup->property("visible").toBool());
}
}  // namespace

namespace {
TEST_F(QuickPalette, NewControlGroupsPreserveExplicitColorsAndVisualReplacements) {
  for (const auto* type :
       {"Label", "ToolButton", "ToolBar", "ToolSeparator", "MenuSeparator", "Popup", "MenuBar", "MenuBarItem"}) {
    SCOPED_TRACE(type);
    auto control = create(QByteArray("import QtQuick; import Holonight; ") + type + R"( {
        palette.active.windowText: "#112233"; palette.disabled.windowText: "#80445566"
        palette.active.buttonText: "#112233"; palette.disabled.buttonText: "#80445566"
        palette.active.button: "#334455"; palette.disabled.button: "#80667788"
        palette.active.link: "#123456"; palette.disabled.link: "transparent"
    })");
    ASSERT_TRUE(control);
    if (QByteArray(type) == "Label") {
      EXPECT_EQ(control->property("color").value<QColor>(), QColor("#112233"));
      control->setProperty("enabled", false);
      EXPECT_EQ(control->property("color").value<QColor>(), QColor("#80445566"));
      EXPECT_EQ(control->property("linkColor").value<QColor>(), QColor(Qt::transparent));
    } else if (QByteArray(type) == "ToolButton" || QByteArray(type) == "MenuBarItem") {
      control->setProperty("down", true);
      control->setProperty("enabled", false);
      auto* background = control->property("background").value<QQuickItem*>();
      EXPECT_EQ(background->property("color").value<QColor>(), QColor("#80667788"));
      EXPECT_EQ(background->opacity(), 1.0);
      EXPECT_EQ(control->property("contentItem").value<QQuickItem*>()->property("color").value<QColor>(),
                QColor("#80445566"));
    } else if (QByteArray(type) != "ToolSeparator" && QByteArray(type) != "MenuSeparator") {
      control->setProperty("enabled", false);
      EXPECT_EQ(control->property("background").value<QQuickItem*>()->property("color").value<QColor>(),
                QColor("#80667788"));
    }
  }
  auto window = create(R"(
import QtQuick
import Holonight
ApplicationWindow {
    width: 400; height: 300
    header: ToolBar { objectName: "header"; height: 35 }
    footer: ToolBar { objectName: "footer"; height: 27 }
    Label { objectName: "label"; width: 40; wrapMode: Text.WordWrap; text: "A long label that wraps onto several lines" }
    ToolButton { objectName: "custom"; padding: 0; background: null
        contentItem: Item { implicitWidth: 71; implicitHeight: 53 } }
    ToolButton { objectName: "icon"; display: AbstractButton.IconOnly; text: "Ignored long text"
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/search.svg" }
    ToolButton { objectName: "text"; display: AbstractButton.TextOnly; text: "A much longer action label" }
    ToolButton { objectName: "mirror"; LayoutMirroring.enabled: true; text: "Mirrored" }
    MenuBar { objectName: "bar"; LayoutMirroring.enabled: true
        Menu { title: "File" } Menu { title: "Edit" }
        property var first: itemAt(0); property var second: itemAt(1) }
})");
  ASSERT_TRUE(window);
  QCoreApplication::processEvents();
  auto* custom = window->findChild<QObject*>("custom");
  EXPECT_EQ(custom->property("implicitWidth").toReal(), 71);
  EXPECT_EQ(custom->property("implicitHeight").toReal(), 53);
  auto* label = window->findChild<QObject*>("label");
  EXPECT_GT(label->property("implicitHeight").toReal(), 40);
  EXPECT_LT(window->findChild<QObject*>("icon")->property("implicitWidth").toReal(),
            window->findChild<QObject*>("text")->property("implicitWidth").toReal());
  auto* mirror = window->findChild<QObject*>("mirror");
  EXPECT_TRUE(mirror->property("contentItem").value<QQuickItem*>()->property("mirrored").toBool());
  auto* bar = window->findChild<QObject*>("bar");
  EXPECT_GT(bar->property("first").value<QObject*>()->property("x").toReal(),
            bar->property("second").value<QObject*>()->property("x").toReal());
  auto* content = window->property("contentItem").value<QQuickItem*>();
  EXPECT_EQ(content->y(), 35);
  EXPECT_EQ(content->height(), 300 - 35 - 27);
}

TEST_F(QuickPalette, NewButtonStatesRenderEffectivePaletteAndFocusBorder) {
  auto object = create(R"(
import QtQuick
import Holonight
ApplicationWindow {
    width: 360; height: 60; visible: true; color: "white"
    palette.base: "blue"; palette.button: "black"; palette.buttonText: "white"
    palette.highlight: "red"; palette.highlightedText: "white"
    palette.disabled.highlight: "#336699"
    ToolButton { width: 50; height: 50 }
    ToolButton { objectName: "hover"; x: 60; width: 50; height: 50 }
    ToolButton { x: 120; width: 50; height: 50; checked: true }
    ToolButton { x: 180; width: 50; height: 50; highlighted: true; down: true }
    MenuBarItem { x: 240; width: 50; height: 50; highlighted: true; enabled: false }
    MenuBarItem { objectName: "focus"; x: 300; width: 50; height: 50 }
})");
  ASSERT_TRUE(object);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  window->findChild<QQuickControl*>("hover")->setHovered(true);
  QImage rendered;
  ASSERT_TRUE(QTest::qWaitFor(
      [&] {
        rendered = window->grabWindow();
        return !rendered.isNull() && logicalPixel(rendered, window, 85, 25) == QColor("#1a1a1a");
      },
      2000));
  EXPECT_EQ(logicalPixel(rendered, window, 25, 25), QColor(Qt::white));
  EXPECT_EQ(logicalPixel(rendered, window, 145, 25), QColor("#1f00e0"));
  EXPECT_EQ(logicalPixel(rendered, window, 205, 25), QColor("#cc0033"));
  EXPECT_EQ(logicalPixel(rendered, window, 265, 25), QColor("#336699"));
  // Inspect the live border binding without assigning focus to any window/control.
  auto* background = window->findChild<QObject*>("focus")->property("background").value<QQuickItem*>();
  auto* border = background->property("border").value<QObject*>();
  ASSERT_TRUE(border);
  EXPECT_EQ(border->property("color").value<QColor>(), QColor(Qt::red));
  EXPECT_EQ(border->property("width").toReal(), 0);
}
}  // namespace

namespace {
TEST_F(QuickPalette, PopupOverlayComponentsMultiplyEffectiveShadowAlpha) {
  auto window = create(R"(
import QtQuick
import QtQuick.Templates as T
import Holonight
ApplicationWindow {
    palette.shadow: "#80663399"
    Popup {
        objectName: "popup"
        property var modalComponent: T.Overlay.modal
        property var modelessComponent: T.Overlay.modeless
        Label { objectName: "contentLabel"; text: "Popup content" }
    }
})");
  ASSERT_TRUE(window);
  auto* popup = window->findChild<QObject*>("popup");
  ASSERT_TRUE(popup);
  for (const auto& entry : {std::pair{"modalComponent", 0.5}, std::pair{"modelessComponent", 0.12}}) {
    auto* component = popup->property(entry.first).value<QQmlComponent*>();
    ASSERT_TRUE(component);
    std::unique_ptr<QObject> overlay(component->create(component->creationContext()));
    ASSERT_TRUE(overlay) << component->errorString().toStdString();
    auto color = overlay->property("color").value<QColor>();
    EXPECT_EQ(color.red(), 0x66);
    EXPECT_EQ(color.green(), 0x33);
    EXPECT_EQ(color.blue(), 0x99);
    EXPECT_NEAR(color.alphaF(), 128.0 / 255 * entry.second, 0.00002);
    popup->property("palette").value<QObject*>()->setProperty("shadow", QColor(Qt::transparent));
    EXPECT_EQ(overlay->property("color").value<QColor>().alpha(), 0);
    popup->property("palette").value<QObject*>()->setProperty("shadow", QColor("#80663399"));
  }
  EXPECT_EQ(window->findChild<QObject*>("contentLabel")->property("color").value<QColor>(),
            appearanceTokens().textPrimary);
}
}  // namespace
