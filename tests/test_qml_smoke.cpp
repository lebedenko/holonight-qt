// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnseparatoralignment.h"
#include "holonight/palette.h"
#include "themeresolver.h"

#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QImage>
#include <QMetaObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QString>
#include <QTemporaryDir>
#include <QTest>
#include <QVariant>

#include <array>
#include <cmath>
#include <gtest/gtest.h>
#include <memory>

namespace {

class EnvGuard {
 public:
  explicit EnvGuard(const char* name) : name_{name}, had_value_{qEnvironmentVariableIsSet(name)} {
    if (had_value_) {
      old_value_ = qgetenv(name);
    }
  }
  EnvGuard(const EnvGuard&) = delete;
  EnvGuard& operator=(const EnvGuard&) = delete;
  EnvGuard(EnvGuard&&) = delete;
  EnvGuard& operator=(EnvGuard&&) = delete;

  ~EnvGuard() {
    if (had_value_) {
      qputenv(name_, old_value_);
    } else {
      qunsetenv(name_);
    }
  }

 private:
  const char* name_;
  bool had_value_;
  QByteArray old_value_;
};

void writeFile(const QString& path, const QByteArray& contents) {
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
  ASSERT_TRUE(opened);
  ASSERT_EQ(file.write(contents), contents.size());
}

bool waitForPropertyColor(QObject* object, const char* propertyName, const QColor& expected) {
  QElapsedTimer timer;
  timer.start();
  while (timer.elapsed() < 2000) {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    if (object->property(propertyName).value<QColor>() == expected) {
      return true;
    }
  }
  return object->property(propertyName).value<QColor>() == expected;
}

bool waitForPropertyInt(QObject* object, const char* propertyName, int expected) {
  QElapsedTimer timer;
  timer.start();
  while (timer.elapsed() < 2000) {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    if (object->property(propertyName).toInt() == expected) {
      return true;
    }
  }
  return object->property(propertyName).toInt() == expected;
}

void expectSeparatorAlignment(qreal dpr) {
  constexpr std::array<qreal, 5> kOffsets = {0.0, 0.2, 0.5, 1.1, 7.75};
  for (const qreal offset : kOffsets) {
    const Holonight::SeparatorAlignment alignment = Holonight::separatorAlignment(1.0, 1.0, dpr, offset);
    const qreal aligned_scene_coordinate = offset + alignment.painted_offset;
    EXPECT_NEAR(alignment.painted_thickness * dpr, 1.0, 0.000001) << "DPR " << dpr << ", offset " << offset;
    EXPECT_NEAR(std::round(aligned_scene_coordinate * dpr), aligned_scene_coordinate * dpr, 0.000001)
        << "DPR " << dpr << ", offset " << offset;
  }
}

void collectColorPickerSwatches(QQuickItem* item, QList<QQuickItem*>& result) {
  for (QQuickItem* child : item->childItems()) {
    if (child->property("index").isValid() && child->property("modelData").canConvert<QColor>()) {
      result.append(child);
    }
    collectColorPickerSwatches(child, result);
  }
}

QList<QQuickItem*> colorPickerSwatches(QObject* picker) {
  QList<QQuickItem*> result;
  auto* item = qobject_cast<QQuickItem*>(picker);
  if (item != nullptr) {
    collectColorPickerSwatches(item, result);
  }
  return result;
}

QQuickItem* colorPickerSwatchAt(const QList<QQuickItem*>& swatches, int index) {
  for (QQuickItem* swatch : swatches) {
    if (swatch->property("index").toInt() == index) {
      return swatch;
    }
  }
  return nullptr;
}

void expectAllSwatchesDisabled(const QList<QQuickItem*>& swatches) {
  for (QQuickItem* swatch : swatches) {
    EXPECT_FALSE(swatch->property("enabled").toBool());
  }
}

}  // namespace

class QmlSmoke : public ::testing::Test {
 protected:
  void SetUp() override { engine_.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)); }

  QQmlEngine engine_;
};

static void checkComponent(QQmlEngine& engine, const char* qml) {
  QQmlComponent comp = QQmlComponent{&engine};
  comp.setData(qml, QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, Button_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Button { text: "test" })"); }

TEST_F(QmlSmoke, Button_UsesSemanticIconSizeAndIconOnlyPadding) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls
    import Holonight
    Item {
      property alias textButton: textButton
      property alias textAndIconButton: textAndIconButton
      property alias iconOnlyButton: iconOnlyButton
      property int textAndIconWidth: textAndIconButton.icon.width
      property int textAndIconHeight: textAndIconButton.icon.height
      property int iconOnlyWidth: iconOnlyButton.icon.width
      property int iconOnlyHeight: iconOnlyButton.icon.height

      Button {
        id: textButton
        text: "Text"
      }
      Button {
        id: textAndIconButton
        text: "Text and icon"
        icon.source: "missing-test-icon.svg"
      }
      Button {
        id: iconOnlyButton
        implicitWidth: implicitHeight
        display: AbstractButton.IconOnly
        icon.source: "missing-test-icon.svg"
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* text_button = root->property("textButton").value<QObject*>();
  QObject* text_and_icon_button = root->property("textAndIconButton").value<QObject*>();
  QObject* icon_only_button = root->property("iconOnlyButton").value<QObject*>();
  ASSERT_NE(text_button, nullptr);
  ASSERT_NE(text_and_icon_button, nullptr);
  ASSERT_NE(icon_only_button, nullptr);

  EXPECT_EQ(text_button->property("implicitHeight").toInt(), 32);
  EXPECT_EQ(text_and_icon_button->property("implicitHeight").toInt(), 32);
  EXPECT_EQ(icon_only_button->property("implicitHeight").toInt(), 32);
  EXPECT_EQ(icon_only_button->property("implicitWidth").toInt(), 32);

  EXPECT_EQ(text_button->property("leftPadding").toInt(), 12);
  EXPECT_EQ(text_button->property("rightPadding").toInt(), 12);
  EXPECT_EQ(text_and_icon_button->property("leftPadding").toInt(), 12);
  EXPECT_EQ(text_and_icon_button->property("rightPadding").toInt(), 12);
  EXPECT_EQ(icon_only_button->property("leftPadding").toInt(), 8);
  EXPECT_EQ(icon_only_button->property("rightPadding").toInt(), 8);
  EXPECT_GE(icon_only_button->property("availableWidth").toInt(), 16);

  EXPECT_EQ(root->property("textAndIconWidth").toInt(), 16);
  EXPECT_EQ(root->property("textAndIconHeight").toInt(), 16);
  EXPECT_EQ(root->property("iconOnlyWidth").toInt(), 16);
  EXPECT_EQ(root->property("iconOnlyHeight").toInt(), 16);
}

TEST_F(QmlSmoke, Button_HoverEnabledAndStateColors) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property alias defaultBtn: defaultBtn
      property alias defaultPressedBtn: defaultPressedBtn
      property alias primaryBtn: primaryBtn
      property alias primaryPressedBtn: primaryPressedBtn
      property alias disabledBtn: disabledBtn

      property color defaultIconColor: defaultBtn.icon.color
      property color primaryIconColor: primaryBtn.icon.color
      property color disabledIconColor: disabledBtn.icon.color

      Button { id: defaultBtn; text: "Default" }
      Button { id: defaultPressedBtn; text: "Default Pressed"; down: true }
      Button { id: primaryBtn; text: "Primary"; highlighted: true }
      Button { id: primaryPressedBtn; text: "Primary Pressed"; highlighted: true; down: true }
      Button { id: disabledBtn; text: "Disabled"; enabled: false }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* defaultBtn = root->property("defaultBtn").value<QObject*>();
  QObject* defaultPressedBtn = root->property("defaultPressedBtn").value<QObject*>();

  QObject* primaryBtn = root->property("primaryBtn").value<QObject*>();
  QObject* primaryPressedBtn = root->property("primaryPressedBtn").value<QObject*>();

  ASSERT_NE(defaultBtn, nullptr);
  ASSERT_NE(primaryBtn, nullptr);

  EXPECT_TRUE(defaultBtn->property("hoverEnabled").toBool());
  EXPECT_TRUE(primaryBtn->property("hoverEnabled").toBool());

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());

  // Default button colors
  EXPECT_EQ(defaultBtn->property("background").value<QObject*>()->property("color").value<QColor>(), tok.surfaceRaised);
  EXPECT_EQ(defaultPressedBtn->property("background").value<QObject*>()->property("color").value<QColor>(),
            tok.surface);

  // Primary button colors
  EXPECT_EQ(primaryBtn->property("background").value<QObject*>()->property("color").value<QColor>(), tok.primary);
  EXPECT_EQ(primaryPressedBtn->property("background").value<QObject*>()->property("color").value<QColor>(),
            tok.primaryPressed);

  // Button icons follow the same foreground colors as their labels.
  EXPECT_EQ(root->property("defaultIconColor").value<QColor>(), tok.textPrimary);
  EXPECT_EQ(root->property("primaryIconColor").value<QColor>(), tok.onPrimary);
  EXPECT_EQ(root->property("disabledIconColor").value<QColor>(), tok.textDisabled);
}

TEST_F(QmlSmoke, Core_ControlSizeAndMetricsContract) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQml
    import Holonight.Core
    QtObject {
      property int compact: HnControlSize.Compact
      property int normal: HnControlSize.Normal
      property int large: HnControlSize.Large
      property int hero: HnControlSize.Hero
      property var heights: [HnControlMetrics.controlHeight(compact),
                             HnControlMetrics.controlHeight(normal),
                             HnControlMetrics.controlHeight(large),
                             HnControlMetrics.controlHeight(hero)]
      property var paddings: [HnControlMetrics.horizontalPadding(compact),
                              HnControlMetrics.horizontalPadding(normal),
                              HnControlMetrics.horizontalPadding(large),
                              HnControlMetrics.horizontalPadding(hero)]
      property var icons: [HnControlMetrics.iconSize(compact),
                           HnControlMetrics.iconSize(normal),
                           HnControlMetrics.iconSize(large),
                           HnControlMetrics.iconSize(hero)]
      property var spacings: [HnControlMetrics.internalSpacing(compact),
                              HnControlMetrics.internalSpacing(normal),
                              HnControlMetrics.internalSpacing(large),
                              HnControlMetrics.internalSpacing(hero)]
      property int invalidRole: HnControlMetrics.normalizedSizeRole(999)
      property int invalidHeight: HnControlMetrics.controlHeight(-1)
      property int headerHeight: HnControlMetrics.headerHeight
      property int appTitleIconSize: HnControlMetrics.appTitleIconSize
      property int appTitleIconSpacing: HnControlMetrics.appTitleIconSpacing
      property int appTitleTextSpacing: HnControlMetrics.appTitleTextSpacing
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->property("compact").toInt(), 0);
  EXPECT_EQ(object->property("normal").toInt(), 1);
  EXPECT_EQ(object->property("large").toInt(), 2);
  EXPECT_EQ(object->property("hero").toInt(), 3);
  EXPECT_EQ(object->property("heights").toList(), QVariantList({28, 32, 40, 52}));
  EXPECT_EQ(object->property("paddings").toList(), QVariantList({8, 8, 12, 16}));
  EXPECT_EQ(object->property("icons").toList(), QVariantList({16, 16, 20, 24}));
  EXPECT_EQ(object->property("spacings").toList(), QVariantList({4, 6, 8, 10}));
  EXPECT_EQ(object->property("invalidRole").toInt(), 1);
  EXPECT_EQ(object->property("invalidHeight").toInt(), 32);
  EXPECT_EQ(object->property("headerHeight").toInt(), 56);
  EXPECT_EQ(object->property("appTitleIconSize").toInt(), 32);
  EXPECT_EQ(object->property("appTitleIconSpacing").toInt(), 12);
  EXPECT_EQ(object->property("appTitleTextSpacing").toInt(), 8);
}

TEST_F(QmlSmoke, Controls_HeaderBarKeepsFixedHeightAndLoadsContent) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias bar: bar
      HnHeaderBar {
        id: bar
        width: 320
        content: Rectangle { objectName: "headerContent" }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  QObject* bar = object->property("bar").value<QObject*>();
  ASSERT_NE(bar, nullptr);
  QQuickItem* content_item = qobject_cast<QQuickItem*>(bar->property("contentItem").value<QObject*>());
  QQuickItem* content_loader = bar->findChild<QQuickItem*>(QStringLiteral("headerContentLoader"));
  QQuickItem* divider = bar->findChild<QQuickItem*>(QStringLiteral("headerDivider"));
  ASSERT_NE(content_item, nullptr);
  ASSERT_NE(content_loader, nullptr);
  ASSERT_NE(divider, nullptr);
  EXPECT_EQ(bar->property("implicitHeight").toInt(), 56);
  EXPECT_EQ(bar->property("height").toInt(), 56);
  EXPECT_TRUE(bar->property("dividerVisible").toBool());
  EXPECT_TRUE(divider->isVisible());
  EXPECT_EQ(bar->property("horizontalPadding").toInt(), 12);
  EXPECT_EQ(bar->property("verticalPadding").toInt(), 8);
  EXPECT_EQ(bar->property("dividerInset").toInt(), 0);

  bar->setProperty("dividerVisible", false);
  bar->setProperty("horizontalPadding", 20);
  bar->setProperty("verticalPadding", 6);
  bar->setProperty("dividerInset", 24);
  QCoreApplication::processEvents();
  EXPECT_EQ(bar->property("height").toInt(), 56);
  EXPECT_FALSE(divider->isVisible());
  EXPECT_EQ(bar->property("horizontalPadding").toInt(), 20);
  EXPECT_EQ(bar->property("verticalPadding").toInt(), 6);
  EXPECT_EQ(bar->property("dividerInset").toInt(), 24);
  EXPECT_EQ(content_loader->x(), 20);
  EXPECT_EQ(content_loader->y(), 6);
  EXPECT_EQ(content_loader->width(), 280);
  EXPECT_EQ(content_loader->height(), 44);
  EXPECT_EQ(divider->x(), 24);
  EXPECT_EQ(divider->width(), 272);

  bar->setProperty("horizontalPadding", -1);
  bar->setProperty("verticalPadding", 100);
  bar->setProperty("dividerInset", 1000);
  QCoreApplication::processEvents();
  EXPECT_EQ(content_loader->x(), 0);
  EXPECT_EQ(content_loader->width(), 320);
  EXPECT_EQ(content_loader->height(), 0);
  EXPECT_EQ(divider->x(), 160);
  EXPECT_EQ(divider->width(), 0);
}

TEST_F(QmlSmoke, Core_MigratedTypesExposeCanonicalContracts) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property bool paletteValid: HoloniightPalette.background.valid
      property bool themeValid: HolonightTheme.baseFontSize > 0
      property bool appearanceValid: HnAppearance.resolve(HnSurfaceRole.Card, HnCornerStyle.Inherit,
                                                          80, 40, NaN, NaN, HnCornerMask.Inherit,
                                                          HnAppearance.revision).radius >= 0
      property bool profileValid: HnShapeProfile.resolve(HnSurfaceRole.Card, HnCornerStyle.Rounded,
                                                        80, 40, NaN, NaN).kind === HnShapeKind.Rounded
      property bool providerValid: !HnIconProvider.supportsSemanticColors("file:///missing.svg")
      property int cornerMask: HnCornerMask.All
      property HnIcon icon: HnIcon {
        source: "file:///missing.svg"
        iconState: HnIcon.Active
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_TRUE(object->property("paletteValid").toBool());
  EXPECT_TRUE(object->property("themeValid").toBool());
  EXPECT_TRUE(object->property("appearanceValid").toBool());
  EXPECT_TRUE(object->property("profileValid").toBool());
  EXPECT_TRUE(object->property("providerValid").toBool());
  EXPECT_EQ(object->property("cornerMask").toInt(), 15);
  QObject* icon = object->property("icon").value<QObject*>();
  ASSERT_NE(icon, nullptr);
  EXPECT_EQ(icon->property("iconState").toInt(), 3);
}

TEST_F(QmlSmoke, Core_HnLabelAndTypographyRoles) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property HnLabel bodyLabel: HnLabel {
        role: HnTypographyRole.Body
        rawText: "Body Text"
      }
      property HnLabel microLabel: HnLabel {
        role: HnTypographyRole.MicroHeader
        rawText: "CALENDAR"
        showPrefix: true
      }
      property HnLabel displayLabel: HnLabel {
        role: HnTypographyRole.Display
        rawText: "14°C"
      }
      property HnLabel codeLabel: HnLabel {
        role: HnTypographyRole.Code
        rawText: "code"
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* body = root->property("bodyLabel").value<QObject*>();
  QObject* micro = root->property("microLabel").value<QObject*>();
  QObject* display = root->property("displayLabel").value<QObject*>();
  QObject* code = root->property("codeLabel").value<QObject*>();
  ASSERT_NE(body, nullptr);
  ASSERT_NE(micro, nullptr);
  ASSERT_NE(display, nullptr);
  ASSERT_NE(code, nullptr);

  EXPECT_EQ(body->property("role").toInt(), 4);
  EXPECT_EQ(micro->property("role").toInt(), 6);
  EXPECT_EQ(display->property("role").toInt(), 0);
  EXPECT_EQ(code->property("role").toInt(), 7);

  EXPECT_TRUE(micro->property("text").toString().contains(QStringLiteral("CALENDAR")));
  EXPECT_TRUE(micro->property("text").toString().contains(QStringLiteral("//")));
}

TEST_F(QmlSmoke, StyleModule_DoesNotExportSharedTypes) {
  for (const char* qml : {
           R"(import Holonight; HnIcon {})",
           R"(import Holonight; HnSurfaceFrame {})",
           R"(import Holonight; HnApplicationWindow {})",
       }) {
    QQmlComponent comp = QQmlComponent{&engine_};
    comp.setData(qml, QUrl{});
    EXPECT_EQ(comp.status(), QQmlComponent::Error);
  }
}

TEST_F(QmlSmoke, Controls_MigratedFrameAndWindowExposeCanonicalContracts) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property HnSurfaceFrame frame: HnSurfaceFrame {
        width: 100
        height: 60
        Rectangle { objectName: "canonicalFrameContent" }
      }
      property Component windowComponent: Component {
        HnApplicationWindow {
          applicationHeaderVisible: true
          applicationHeaderLeadingData: Item { objectName: "canonicalLeading" }
          Item { objectName: "canonicalClient" }
        }
      }
      property var windowObject: windowComponent.createObject(null)
      Component.onDestruction: if (windowObject) windowObject.destroy()
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  QObject* frame = object->property("frame").value<QObject*>();
  QObject* window = object->property("windowObject").value<QObject*>();
  ASSERT_NE(frame, nullptr);
  ASSERT_NE(window, nullptr);
  EXPECT_NE(frame->findChild<QObject*>(QStringLiteral("canonicalFrameContent")), nullptr);
  EXPECT_NE(window->findChild<QObject*>(QStringLiteral("canonicalLeading")), nullptr);
  EXPECT_NE(window->findChild<QObject*>(QStringLiteral("canonicalClient")), nullptr);
  EXPECT_TRUE(window->property("applicationHeaderVisible").toBool());
}

TEST_F(QmlSmoke, Controls_ActionBarKeepsOptionalSlotsAligned) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias bar: bar

      Component { id: leadingSlot; Rectangle { implicitWidth: 120; implicitHeight: 24 } }
      Component { id: centerSlot; Rectangle { implicitWidth: 80; implicitHeight: 20 } }
      Component { id: trailingSlot; Rectangle { implicitWidth: 200; implicitHeight: 28 } }

      function setSlots(mask) {
        bar.leadingContent = mask & 1 ? leadingSlot : null
        bar.centerContent = mask & 2 ? centerSlot : null
        bar.trailingContent = mask & 4 ? trailingSlot : null
      }
      function leadingX() { return bar.leadingItem.mapToItem(bar, 0, 0).x }
      function centerMidpoint() {
        const point = bar.centerItem.mapToItem(bar, bar.centerItem.width / 2, 0)
        return point.x
      }
      function trailingRight() {
        const point = bar.trailingItem.mapToItem(bar, bar.trailingItem.width, 0)
        return point.x
      }

      HnActionBar {
        id: bar
        width: 640
        height: 56
        dividerVisible: false
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  QQuickItem* root_item = qobject_cast<QQuickItem*>(object.get());
  ASSERT_NE(root_item, nullptr);
  QQuickWindow window;
  root_item->setParentItem(window.contentItem());
  window.show();
  QCoreApplication::processEvents();
  QObject* bar = object->property("bar").value<QObject*>();
  ASSERT_NE(bar, nullptr);

  const auto setSlots = [&](int mask) {
    ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "setSlots", Qt::DirectConnection, Q_ARG(QVariant, mask)));
    QCoreApplication::processEvents();
  };
  const auto coordinate = [&](const char* method) {
    QVariant result;
    EXPECT_TRUE(QMetaObject::invokeMethod(object.get(), method, Qt::DirectConnection, Q_RETURN_ARG(QVariant, result)));
    return result.toDouble();
  };

  for (int mask = 0; mask < 8; ++mask) {
    setSlots(mask);
    EXPECT_EQ(bar->property("leadingItem").value<QObject*>() != nullptr, (mask & 1) != 0);
    EXPECT_EQ(bar->property("centerItem").value<QObject*>() != nullptr, (mask & 2) != 0);
    EXPECT_EQ(bar->property("trailingItem").value<QObject*>() != nullptr, (mask & 4) != 0);
    if (mask & 1) {
      EXPECT_NEAR(coordinate("leadingX"), 0.0, 0.01);
    }
    if (mask & 2) {
      EXPECT_NEAR(coordinate("centerMidpoint"), bar->property("width").toDouble() / 2, 0.01);
    }
    if (mask & 4) {
      EXPECT_NEAR(coordinate("trailingRight"), bar->property("width").toDouble(), 0.01);
    }
  }

  setSlots(7);
  ASSERT_TRUE(bar->setProperty("width", 360.0));
  window.update();
  QTest::qWait(20);
  ASSERT_DOUBLE_EQ(bar->property("width").toDouble(), 360.0);
  EXPECT_NEAR(coordinate("leadingX"), 0.0, 0.01);
  EXPECT_NEAR(coordinate("centerMidpoint"), 180.0, 0.01);
  EXPECT_NEAR(coordinate("trailingRight"), 360.0, 0.01);

  bar->setProperty("dividerVisible", true);
  QCoreApplication::processEvents();
  EXPECT_NEAR(coordinate("leadingX"), 0.0, 0.01);
  EXPECT_NEAR(coordinate("centerMidpoint"), 180.0, 0.01);
  EXPECT_NEAR(coordinate("trailingRight"), 360.0, 0.01);

  root_item->setParentItem(nullptr);
}

TEST_F(QmlSmoke, Controls_SearchFieldSizesAndInvalidFallback) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias hero: hero
      property alias invalid: invalid
      HnSearchField { id: compact; sizeRole: HnControlSize.Compact }
      HnSearchField { id: normal }
      HnSearchField { id: large; sizeRole: HnControlSize.Large }
      HnSearchField { id: hero; sizeRole: HnControlSize.Hero }
      HnSearchField { id: invalid; sizeRole: 999 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  EXPECT_EQ(root->property("compact").value<QObject*>()->property("implicitHeight").toInt(), 28);
  EXPECT_EQ(root->property("normal").value<QObject*>()->property("implicitHeight").toInt(), 32);
  EXPECT_EQ(root->property("large").value<QObject*>()->property("implicitHeight").toInt(), 40);
  EXPECT_EQ(root->property("hero").value<QObject*>()->property("implicitHeight").toInt(), 52);
  EXPECT_EQ(root->property("invalid").value<QObject*>()->property("resolvedSizeRole").toInt(), 1);
  EXPECT_EQ(root->property("compact").value<QObject*>()->property("verticalAlignment").toInt(), Qt::AlignVCenter);
  EXPECT_EQ(root->property("normal").value<QObject*>()->property("verticalAlignment").toInt(), Qt::AlignVCenter);
  EXPECT_EQ(root->property("large").value<QObject*>()->property("verticalAlignment").toInt(), Qt::AlignVCenter);
  EXPECT_EQ(root->property("hero").value<QObject*>()->property("verticalAlignment").toInt(), Qt::AlignVCenter);
  QObject* defaultIcon =
      root->property("normal").value<QObject*>()->findChild<QObject*>(QStringLiteral("hnSearchDefaultIcon"));
  ASSERT_NE(defaultIcon, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(defaultIcon->property("visible").toBool());
  EXPECT_FALSE(defaultIcon->property("hasError").toBool());
}

TEST_F(QmlSmoke, Controls_IconComboBoxContracts) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias hero: hero
      property alias invalid: invalid
      property alias listCombo: listCombo
      HnIconComboBox { id: compact; sizeRole: HnControlSize.Compact }
      HnIconComboBox { id: normal }
      HnIconComboBox { id: large; sizeRole: HnControlSize.Large }
      HnIconComboBox { id: hero; sizeRole: HnControlSize.Hero }
      HnIconComboBox { id: invalid; sizeRole: -20 }
      ListModel {
        id: choices
        ListElement { title: "First"; key: 10; icon: "" }
        ListElement { title: "Second"; key: 20; icon: "missing-icon.svg" }
      }
      HnIconComboBox {
        id: listCombo
        model: choices
        textRole: "title"
        valueRole: "key"
        currentIndex: 1
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  EXPECT_EQ(root->property("compact").value<QObject*>()->property("implicitHeight").toInt(), 28);
  EXPECT_EQ(root->property("normal").value<QObject*>()->property("implicitHeight").toInt(), 32);
  EXPECT_EQ(root->property("large").value<QObject*>()->property("implicitHeight").toInt(), 40);
  EXPECT_EQ(root->property("hero").value<QObject*>()->property("implicitHeight").toInt(), 52);
  EXPECT_EQ(root->property("invalid").value<QObject*>()->property("resolvedSizeRole").toInt(), 1);
  QObject* listCombo = root->property("listCombo").value<QObject*>();
  EXPECT_EQ(listCombo->property("currentText").toString(), QStringLiteral("Second"));
  EXPECT_EQ(listCombo->property("currentValue").toInt(), 20);
  EXPECT_EQ(listCombo->property("iconRole").toString(), QStringLiteral("icon"));
}

TEST_F(QmlSmoke, Controls_TextAreaNormalizesBoundsAndForwardsEditing) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnTextArea {
      width: 300
      minimumVisibleLines: 0
      maximumVisibleLines: -4
      text: "abc"
      accessibleName: "Notes"
      footerContent: Component { Item { implicitHeight: 12 } }
      Component.onCompleted: {
        insert(3, "def")
        select(1, 4)
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> area{comp.create()};
  ASSERT_NE(area, nullptr);

  EXPECT_EQ(area->property("resolvedSizeRole").toInt(), 1);
  EXPECT_EQ(area->property("resolvedMinimumVisibleLines").toInt(), 1);
  EXPECT_EQ(area->property("resolvedMaximumVisibleLines").toInt(), 1);
  EXPECT_EQ(area->property("text").toString(), QStringLiteral("abcdef"));
  EXPECT_EQ(area->property("selectedText").toString(), QStringLiteral("bcd"));
  EXPECT_GT(area->property("implicitHeight").toReal(), 12.0);
  QObject* footer = area->findChild<QObject*>(QStringLiteral("hnTextAreaFooter"));
  ASSERT_NE(footer, nullptr);
  EXPECT_EQ(footer->property("x").toReal(), 8.0);
  EXPECT_EQ(footer->property("width").toReal(), 284.0);
}

TEST_F(QmlSmoke, Controls_FormFieldOwnsOneControlAndSelectsSupportingText) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnFormField {
      labelText: "Name"
      helperText: "Helpful"
      errorText: "Invalid"
      required: true
      hasError: true
      control: Component {
        HnSearchField {
          objectName: "field"
          hasError: true
        }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> field{comp.create()};
  ASSERT_NE(field, nullptr);

  QObject* control = field->property("controlItem").value<QObject*>();
  ASSERT_NE(control, nullptr);
  EXPECT_EQ(control->objectName(), QStringLiteral("field"));
  EXPECT_TRUE(field->property("hasError").toBool());
  EXPECT_TRUE(control->property("hasError").toBool());
}

TEST_F(QmlSmoke, Controls_SearchFieldSlotsAndIconsCompose) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnSearchField {
      text: "query"
      leadingContent: Rectangle { objectName: "customLeadingItem"; implicitWidth: 22; implicitHeight: 18 }
      trailingContent: Rectangle { objectName: "customTrailingItem"; implicitWidth: 30; implicitHeight: 18 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> field{comp.create()};
  ASSERT_NE(field, nullptr);

  QObject* leadingLoader = field->findChild<QObject*>(QStringLiteral("hnSearchCustomLeading"));
  QObject* trailingLoader = field->findChild<QObject*>(QStringLiteral("hnSearchCustomTrailing"));
  QObject* defaultIcon = field->findChild<QObject*>(QStringLiteral("hnSearchDefaultIcon"));
  QObject* clearButton = field->findChild<QObject*>(QStringLiteral("hnSearchClearButton"));
  ASSERT_NE(leadingLoader, nullptr);
  ASSERT_NE(trailingLoader, nullptr);
  ASSERT_NE(defaultIcon, nullptr);
  ASSERT_NE(clearButton, nullptr);
  ASSERT_NE(field->findChild<QObject*>(QStringLiteral("customLeadingItem")), nullptr);
  ASSERT_NE(field->findChild<QObject*>(QStringLiteral("customTrailingItem")), nullptr);
  EXPECT_FALSE(defaultIcon->property("visible").toBool());
  EXPECT_TRUE(trailingLoader->property("active").toBool());
  EXPECT_TRUE(clearButton->property("visible").toBool());
  EXPECT_EQ(clearButton->property("accessibleName").toString(), QStringLiteral("Clear search"));
}

TEST_F(QmlSmoke, Controls_SearchFieldClearActionRestoresFocusAndHonorsStates) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      visible: true
      width: 320
      height: 120
      property alias field: field
      HnSearchField { id: field; text: "query" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);
  QObject* field = window->property("field").value<QObject*>();
  QObject* clearButton = field->findChild<QObject*>(QStringLiteral("hnSearchClearButton"));
  ASSERT_NE(clearButton, nullptr);

  ASSERT_TRUE(QMetaObject::invokeMethod(clearButton, "clicked", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(field->property("text").toString().isEmpty());
  EXPECT_TRUE(field->property("activeFocus").toBool());

  field->setProperty("text", QStringLiteral("read only"));
  field->setProperty("readOnly", true);
  EXPECT_FALSE(clearButton->property("visible").toBool());
  EXPECT_TRUE(field->property("hasError").isValid());
  field->setProperty("readOnly", false);
  field->setProperty("enabled", false);
  EXPECT_FALSE(clearButton->property("visible").toBool());
}

TEST_F(QmlSmoke, Controls_SearchFieldEscapeClearsThenPropagatesWhenEmpty) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      visible: true
      width: 320
      height: 120
      property alias field: field
      property alias propagatedEscapes: container.propagatedEscapes
      Item {
        id: container
        anchors.fill: parent
        property int propagatedEscapes: 0
        Keys.onEscapePressed: event => {
          propagatedEscapes += 1
          event.accepted = true
        }
        HnSearchField { id: field; text: "query"; focus: true }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  QObject* field = object->property("field").value<QObject*>();
  ASSERT_NE(field, nullptr);
  field->setProperty("focus", true);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  QTest::keyClick(window, Qt::Key_Escape);
  EXPECT_TRUE(field->property("text").toString().isEmpty());
  EXPECT_EQ(object->property("propagatedEscapes").toInt(), 0);

  QTest::keyClick(window, Qt::Key_Escape);
  EXPECT_EQ(object->property("propagatedEscapes").toInt(), 1);
}

TEST_F(QmlSmoke, ScrollBar_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; ScrollBar {})"); }

TEST_F(QmlSmoke, ScrollBar_HoverEnabledAndOrientations) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property alias vBar: vBar
      property alias hBar: hBar

      ScrollBar { id: vBar; orientation: Qt.Vertical; active: true }
      ScrollBar { id: hBar; orientation: Qt.Horizontal; active: true }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* vBar = root->property("vBar").value<QObject*>();
  QObject* hBar = root->property("hBar").value<QObject*>();
  ASSERT_NE(vBar, nullptr);
  ASSERT_NE(hBar, nullptr);

  EXPECT_TRUE(vBar->property("hoverEnabled").toBool());
  EXPECT_TRUE(hBar->property("hoverEnabled").toBool());
}

TEST_F(QmlSmoke, ItemDelegate_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ItemDelegate { text: "Item" })");
}

TEST_F(QmlSmoke, ItemDelegate_SelectionAndStateColors) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property alias normalDelegate: normalDelegate
      property alias highlightedDelegate: highlightedDelegate
      property alias pressedDelegate: pressedDelegate

      ItemDelegate { id: normalDelegate; text: "Normal" }
      ItemDelegate { id: highlightedDelegate; text: "Highlighted"; highlighted: true }
      ItemDelegate { id: pressedDelegate; text: "Pressed"; down: true }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* normalDelegate = root->property("normalDelegate").value<QObject*>();
  QObject* highlightedDelegate = root->property("highlightedDelegate").value<QObject*>();
  QObject* pressedDelegate = root->property("pressedDelegate").value<QObject*>();

  ASSERT_NE(normalDelegate, nullptr);
  ASSERT_NE(highlightedDelegate, nullptr);
  ASSERT_NE(pressedDelegate, nullptr);

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());

  // Highlighted delegates use a quiet surface while retaining primary text.
  QObject* highlightedBase = highlightedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateBase"));
  QObject* highlightedSelection =
      highlightedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateSelectedOverlay"));
  ASSERT_NE(highlightedBase, nullptr);
  ASSERT_NE(highlightedSelection, nullptr);
  EXPECT_EQ(highlightedBase->property("color").value<QColor>(), QColor{Qt::transparent});
  EXPECT_EQ(highlightedSelection->property("color").value<QColor>(), tok.surfaceSelected);
  EXPECT_TRUE(highlightedSelection->property("visible").toBool());
  EXPECT_EQ(highlightedDelegate->property("contentItem").value<QObject*>()->property("color").value<QColor>(),
            tok.textPrimary);

  // Pressed item delegates use the same opaque resolved surface as segmented controls.
  QObject* pressedBase = pressedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateBase"));
  ASSERT_NE(pressedBase, nullptr);
  EXPECT_EQ(pressedBase->property("color").value<QColor>(), tok.surfaceElevated);
}

TEST_F(QmlSmoke, ScrollView_LoadsWithoutError) {
  checkComponent(engine_, R"(import QtQuick; import Holonight; ScrollView { Item { width: 100; height: 500 } } )");
}

TEST_F(QmlSmoke, TextField_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; TextField { placeholderText: "hint" })");
}

TEST_F(QmlSmoke, TextField_ImplicitWidthIncludesLongPlaceholder) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight
    TextField { placeholderText: "A placeholder that is substantially wider than the default background width" }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_GT(object->property("implicitWidth").toReal(), 200.0);
}

TEST_F(QmlSmoke, TextArea_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; TextArea { placeholderText: "hint" })");
}

TEST_F(QmlSmoke, SpinBox_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; SpinBox { value: 42; from: 0; to: 100 })");
}

TEST_F(QmlSmoke, CheckBox_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; CheckBox { text: "check" })");
}

TEST_F(QmlSmoke, RadioButton_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; RadioButton { text: "option" })");
}

TEST_F(QmlSmoke, ComboBox_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ComboBox { model: ["a", "b"] })");
}

TEST_F(QmlSmoke, ComboBox_InstantiatesAndOpensPopup) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Window {
      width: 320
      height: 240
      visible: true

      property bool popupVisible: combo.popup.visible
      property real popupHeight: combo.popup.height

      function openPopup() {
        combo.popup.open()
      }

      ComboBox {
        id: combo
        model: ["item1", "item2", "item3"]
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "openPopup", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(object->property("popupVisible").toBool());
  EXPECT_GT(object->property("popupHeight").toReal(), 0.0);
}

TEST_F(QmlSmoke, ComboBox_BoundsPopupByModelAndMaximumVisibleItems) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Window {
      width: 400
      height: 600
      visible: true

      property var shortCombo: shortCombo
      property var defaultCombo: defaultCombo
      property var fourCombo: fourCombo
      property var invalidCombo: invalidCombo

      function openCombo(combo) {
        combo.popup.open()
      }

      ComboBox { id: shortCombo; model: ["1", "2", "3"] }
      ComboBox { id: defaultCombo; x: 130; model: 20 }
      ComboBox { id: fourCombo; x: 260; model: 20; maximumVisibleItems: 4 }
      ComboBox { id: invalidCombo; y: 320; model: 20; maximumVisibleItems: -5 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);

  auto verifyPopup = [&](const char* comboProperty, qreal expectedRows) {
    QObject* combo = window->property(comboProperty).value<QObject*>();
    ASSERT_NE(combo, nullptr);
    ASSERT_TRUE(QMetaObject::invokeMethod(window.get(), "openCombo", Qt::DirectConnection,
                                          Q_ARG(QVariant, QVariant::fromValue(combo))));
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    QObject* popup = combo->property("popup").value<QObject*>();
    ASSERT_NE(popup, nullptr);
    QObject* list = popup->property("contentItem").value<QObject*>();
    ASSERT_NE(list, nullptr);
    EXPECT_NEAR(list->property("height").toReal(), expectedRows * 28.0, 0.5);
    EXPECT_NEAR(popup->property("height").toReal(), expectedRows * 28.0 + 8.0, 0.5);
    popup->setProperty("visible", false);
  };

  verifyPopup("shortCombo", 3);
  verifyPopup("defaultCombo", 8);
  verifyPopup("fourCombo", 4);
  verifyPopup("invalidCombo", 1);
  EXPECT_EQ(window->property("invalidCombo").value<QObject*>()->property("resolvedMaximumVisibleItems").toInt(), 1);
}

TEST_F(QmlSmoke, ComboBox_ConstrainsPopupToWindowAndKeepsOverflowInteractive) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Window {
      width: 240
      height: 120
      visible: true

      property var combo: combo

      function navigateToLast() {
        navigationTimer.start()
      }

      Timer {
        id: navigationTimer
        interval: 1
        repeat: true
        onTriggered: {
          if (combo.highlightedIndex < combo.count - 1)
            combo.incrementCurrentIndex()
          else
            stop()
        }
      }

      ComboBox {
        id: combo
        y: 72
        model: 20
        Component.onCompleted: popup.open()
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  QObject* combo = window->property("combo").value<QObject*>();
  ASSERT_NE(combo, nullptr);
  QObject* popup = combo->property("popup").value<QObject*>();
  ASSERT_NE(popup, nullptr);
  QObject* list = popup->property("contentItem").value<QObject*>();
  ASSERT_NE(list, nullptr);

  EXPECT_LE(popup->property("height").toReal(), popup->property("availableVerticalSpace").toReal() + 0.5);
  EXPECT_TRUE(popup->property("opensAbove").toBool());
  EXPECT_GT(list->property("contentHeight").toReal(), list->property("height").toReal());
  EXPECT_TRUE(list->property("interactive").toBool());

  ASSERT_TRUE(QMetaObject::invokeMethod(window.get(), "navigateToLast", Qt::DirectConnection));
  EXPECT_TRUE(waitForPropertyInt(list, "currentIndex", 19));
  EXPECT_GT(list->property("contentY").toReal(), 0.0);
}

TEST_F(QmlSmoke, Controls_IconComboBoxPopupUsesSemanticDelegateHeight) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Window {
      width: 360
      height: 700
      visible: true

      property var compact: compact
      property var large: large

      HnIconComboBox {
        id: compact
        model: 20
        sizeRole: HnControlSize.Compact
      }
      HnIconComboBox {
        id: large
        x: 160
        model: 20
        sizeRole: HnControlSize.Large
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);

  auto popupListHeight = [&](const char* comboProperty) {
    QObject* combo = window->property(comboProperty).value<QObject*>();
    EXPECT_NE(combo, nullptr);
    QObject* popup = combo->property("popup").value<QObject*>();
    EXPECT_NE(popup, nullptr);
    QMetaObject::invokeMethod(popup, "open", Qt::DirectConnection);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    QObject* list = popup->property("contentItem").value<QObject*>();
    EXPECT_NE(list, nullptr);
    const qreal height = list->property("height").toReal();
    QMetaObject::invokeMethod(popup, "close", Qt::DirectConnection);
    return height;
  };

  EXPECT_NEAR(popupListHeight("compact"), 8.0 * 28.0, 0.5);
  EXPECT_NEAR(popupListHeight("large"), 8.0 * 40.0, 0.5);
}

TEST_F(QmlSmoke, Slider_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Slider {})"); }

TEST_F(QmlSmoke, Switch_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; Switch { text: "toggle" })");
}

TEST_F(QmlSmoke, Switch_SizeRoles) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    import Holonight.Core

    Item {
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias hero: hero
      property alias invalid: invalid

      Switch { id: compact; sizeRole: HnControlSize.Compact }
      Switch { id: normal }
      Switch { id: large; sizeRole: HnControlSize.Large }
      Switch { id: hero; sizeRole: HnControlSize.Hero }
      Switch { id: invalid; sizeRole: 999 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* compact = root->property("compact").value<QObject*>();
  QObject* normal = root->property("normal").value<QObject*>();
  QObject* large = root->property("large").value<QObject*>();
  QObject* hero = root->property("hero").value<QObject*>();
  QObject* invalid = root->property("invalid").value<QObject*>();

  ASSERT_NE(compact, nullptr);
  ASSERT_NE(normal, nullptr);
  ASSERT_NE(large, nullptr);
  ASSERT_NE(hero, nullptr);
  ASSERT_NE(invalid, nullptr);

  EXPECT_EQ(compact->property("resolvedSizeRole").toInt(), 0);
  EXPECT_EQ(normal->property("resolvedSizeRole").toInt(), 1);
  EXPECT_EQ(large->property("resolvedSizeRole").toInt(), 2);
  EXPECT_EQ(hero->property("resolvedSizeRole").toInt(), 3);
  EXPECT_EQ(invalid->property("resolvedSizeRole").toInt(), 1);

  EXPECT_EQ(compact->property("indicatorTrackWidth").toDouble(), 28.0);
  EXPECT_EQ(normal->property("indicatorTrackWidth").toDouble(), 34.0);
  EXPECT_EQ(large->property("indicatorTrackWidth").toDouble(), 44.0);
  EXPECT_EQ(hero->property("indicatorTrackWidth").toDouble(), 56.0);

  EXPECT_EQ(compact->property("indicatorTrackHeight").toDouble(), 14.0);
  EXPECT_EQ(normal->property("indicatorTrackHeight").toDouble(), 16.0);
  EXPECT_EQ(large->property("indicatorTrackHeight").toDouble(), 22.0);
  EXPECT_EQ(hero->property("indicatorTrackHeight").toDouble(), 28.0);

  EXPECT_EQ(compact->property("indicatorThumbSize").toDouble(), 10.0);
  EXPECT_EQ(normal->property("indicatorThumbSize").toDouble(), 12.0);
  EXPECT_EQ(large->property("indicatorThumbSize").toDouble(), 16.0);
  EXPECT_EQ(hero->property("indicatorThumbSize").toDouble(), 22.0);
}

TEST_F(QmlSmoke, ProgressBar_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ProgressBar { value: 0.5 })");
}

TEST_F(QmlSmoke, ProgressBar_IndeterminateIndicatorEntersTravelsAndExitsTrack) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight
    ProgressBar {
      width: 200
      indeterminate: true
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> progress{comp.create()};
  ASSERT_NE(progress, nullptr);

  QObject* indicator = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndicator"));
  QObject* enter = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndeterminateEnter"));
  QObject* travel = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndeterminateTravel"));
  QObject* exit = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndeterminateExit"));
  ASSERT_NE(indicator, nullptr);
  ASSERT_NE(enter, nullptr);
  ASSERT_NE(travel, nullptr);
  ASSERT_NE(exit, nullptr);

  const qreal track_width = progress->property("contentItem").value<QObject*>()->property("width").toReal();
  const qreal nominal_indicator_width = track_width * 0.3;

  EXPECT_DOUBLE_EQ(enter->property("from").toReal(), 0.0);
  EXPECT_DOUBLE_EQ(enter->property("to").toReal(), nominal_indicator_width);
  EXPECT_DOUBLE_EQ(travel->property("from").toReal(), 0.0);
  EXPECT_DOUBLE_EQ(travel->property("to").toReal(), track_width - nominal_indicator_width);
  EXPECT_DOUBLE_EQ(exit->property("from").toReal(), nominal_indicator_width);
  EXPECT_DOUBLE_EQ(exit->property("to").toReal(), 0.0);
  EXPECT_GT(indicator->property("radius").toReal(), 0.0);
}

TEST_F(QmlSmoke, TabBar_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; TabBar {})"); }

TEST_F(QmlSmoke, TabButton_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; TabButton { text: "Tab 1" })");
}

TEST_F(QmlSmoke, Menu_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Menu {})"); }

TEST_F(QmlSmoke, Menu_InstantiatesAndOpens) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Menu {
      MenuItem { text: "Item 1" }
      MenuItem { text: "Item 2" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  object->setProperty("opened", true);
}

TEST_F(QmlSmoke, MenuItem_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; MenuItem { text: "Action" })");
}

TEST_F(QmlSmoke, MenuItem_RendersStandardIconAndIndependentCheckmark) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property alias plain: plain
      property alias checkedIcon: checkedIcon
      property alias highlighted: highlighted
      property alias pressedHighlighted: pressedHighlighted
      MenuItem { id: plain; text: "Plain" }
      MenuItem {
        id: checkedIcon
        text: "Open folder"
        checkable: true
        checked: true
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
      }
      MenuItem {
        id: highlighted
        text: "Highlighted"
        highlighted: true
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg"
      }
      MenuItem {
        id: pressedHighlighted
        text: "Pressed"
        highlighted: true
        down: true
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* plain = root->property("plain").value<QObject*>();
  QObject* checked_icon = root->property("checkedIcon").value<QObject*>();
  QObject* highlighted = root->property("highlighted").value<QObject*>();
  QObject* pressed_highlighted = root->property("pressedHighlighted").value<QObject*>();
  ASSERT_NE(plain, nullptr);
  ASSERT_NE(checked_icon, nullptr);
  ASSERT_NE(highlighted, nullptr);
  ASSERT_NE(pressed_highlighted, nullptr);

  QObject* icon = checked_icon->findChild<QObject*>(QStringLiteral("hnMenuItemIcon"));
  ASSERT_NE(icon, nullptr);
  EXPECT_TRUE(icon->property("visible").toBool());
  EXPECT_DOUBLE_EQ(icon->property("size").toReal(), 16.0);
  EXPECT_FALSE(plain->findChild<QObject*>(QStringLiteral("hnMenuItemIcon"))->property("visible").toBool());

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  EXPECT_EQ(highlighted->property("background").value<QObject*>()->property("color").value<QColor>(), tok.primary);
  EXPECT_EQ(pressed_highlighted->property("background").value<QObject*>()->property("color").value<QColor>(),
            tok.primaryPressed);
  EXPECT_EQ(highlighted->findChild<QObject*>(QStringLiteral("hnMenuItemLabel"))->property("color").value<QColor>(),
            tok.onPrimary);
}

TEST_F(QmlSmoke, ToolTip_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ToolTip { text: "hint" })");
}

TEST_F(QmlSmoke, Controls_IconButtonUsesSemanticMetricsAndStandardIconApi) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias invalid: invalid
      property alias disabled: disabled
      HnIconButton {
        id: compact
        sizeRole: HnControlSize.Compact
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/more-vertical.svg"
      }
      HnIconButton {
        id: normal
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg"
        icon.color: "#ff3366"
        Accessible.name: "Edit"
        property string reportedAccessibleName: Accessible.name
      }
      HnIconButton {
        id: large
        sizeRole: HnControlSize.Large
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
      }
      HnIconButton { id: invalid; sizeRole: -50; icon.source: "file:///missing.svg" }
      HnIconButton {
        id: disabled
        enabled: false
        icon.source: "qrc:/qt/qml/Holonight/Controls/assets/delete.svg"
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  QObject* compact = root->property("compact").value<QObject*>();
  QObject* normal = root->property("normal").value<QObject*>();
  QObject* large = root->property("large").value<QObject*>();
  QObject* invalid = root->property("invalid").value<QObject*>();
  QObject* disabled = root->property("disabled").value<QObject*>();
  ASSERT_NE(compact, nullptr);
  ASSERT_NE(normal, nullptr);
  ASSERT_NE(large, nullptr);
  ASSERT_NE(invalid, nullptr);
  ASSERT_NE(disabled, nullptr);

  EXPECT_DOUBLE_EQ(compact->property("implicitWidth").toReal(), 28.0);
  EXPECT_DOUBLE_EQ(compact->property("implicitHeight").toReal(), 28.0);
  QObject* compact_icon = compact->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  QObject* normal_icon = normal->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  QObject* large_icon = large->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  QObject* disabled_icon = disabled->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  ASSERT_NE(compact_icon, nullptr);
  ASSERT_NE(normal_icon, nullptr);
  ASSERT_NE(large_icon, nullptr);
  ASSERT_NE(disabled_icon, nullptr);
  EXPECT_DOUBLE_EQ(compact_icon->property("size").toReal(), 16.0);
  EXPECT_DOUBLE_EQ(compact_icon->property("width").toReal(), 16.0);
  EXPECT_DOUBLE_EQ(compact_icon->property("height").toReal(), 16.0);
  EXPECT_DOUBLE_EQ(normal->property("implicitWidth").toReal(), 32.0);
  EXPECT_DOUBLE_EQ(large->property("implicitWidth").toReal(), 40.0);
  EXPECT_DOUBLE_EQ(large_icon->property("size").toReal(), 20.0);
  EXPECT_DOUBLE_EQ(large_icon->property("width").toReal(), 20.0);
  EXPECT_DOUBLE_EQ(invalid->property("implicitWidth").toReal(), 32.0);
  EXPECT_EQ(normal->property("reportedAccessibleName").toString(), QStringLiteral("Edit"));
  EXPECT_EQ(normal_icon->property("resolvedColor").value<QColor>(), QColor{QStringLiteral("#ff3366")});

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  EXPECT_EQ(disabled_icon->property("resolvedColor").value<QColor>(), tok.textDisabled);
  EXPECT_EQ(normal->property("background").value<QObject*>()->property("color").value<QColor>(),
            QColor{Qt::transparent});
  EXPECT_TRUE(normal->property("hoverEnabled").toBool());
}

TEST_F(QmlSmoke, Controls_NavigationTitleReportsActualTruncationForTooltipPolicy) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias shortDelegate: shortDelegate
      property alias longDelegate: longDelegate
      HnNavigationDelegate {
        id: shortDelegate
        width: 320
        title: "Short title"
      }
      HnNavigationDelegate {
        id: longDelegate
        width: 80
        title: "A deliberately long navigation title"
        showTitleToolTipWhenElided: false
        property string reportedAccessibleName: Accessible.name
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QCoreApplication::processEvents();

  QObject* short_delegate = root->property("shortDelegate").value<QObject*>();
  QObject* long_delegate = root->property("longDelegate").value<QObject*>();
  ASSERT_NE(short_delegate, nullptr);
  ASSERT_NE(long_delegate, nullptr);
  QObject* short_label = short_delegate->findChild<QObject*>(QStringLiteral("hnNavigationDelegateTitle"));
  QObject* long_label = long_delegate->findChild<QObject*>(QStringLiteral("hnNavigationDelegateTitle"));
  ASSERT_NE(short_label, nullptr);
  ASSERT_NE(long_label, nullptr);
  EXPECT_FALSE(short_label->property("truncated").toBool());
  EXPECT_TRUE(long_label->property("truncated").toBool());
  EXPECT_TRUE(short_delegate->property("showTitleToolTipWhenElided").toBool());
  EXPECT_FALSE(long_delegate->property("showTitleToolTipWhenElided").toBool());
  EXPECT_EQ(long_delegate->property("reportedAccessibleName").toString(), long_delegate->property("title").toString());
}

TEST_F(QmlSmoke, HnIcon_LoadsWithCompatibleApi) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    HnIcon {
      source: ""
      size: 32
      iconState: HnIcon.Active
      activeColor: "#123456"
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("size").toInt(), 32);
  EXPECT_EQ(object->property("iconState").toInt(), 3);
  EXPECT_EQ(object->property("resolvedColor").value<QColor>(), QColor(QStringLiteral("#123456")));
}

TEST_F(QmlSmoke, HnIcon_ExposesLoadFailure) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight.Core
    HnIcon {
      source: "file:///holonight/definitely-missing.svg"
      tinted: false
      property bool loadFailed: hasError
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  QElapsedTimer timer;
  timer.start();
  while (!object->property("loadFailed").toBool() && timer.elapsed() < 2000) {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  }
  EXPECT_TRUE(object->property("loadFailed").toBool());
}

TEST_F(QmlSmoke, HnSurfaceFrame_ResolvesSemanticGeometry) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 200
      height: 100
      surfaceRole: HnSurfaceRole.Popup
      property bool fillMatchesPalette: fillColor === HoloniightPalette.surfaceRaised
      property bool borderMatchesPalette: borderColor === HoloniightPalette.borderPassive
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->property("shapeKind").toInt(), 2);
  EXPECT_DOUBLE_EQ(object->property("effectiveRadius").toDouble(), 12.0);
  EXPECT_DOUBLE_EQ(object->property("effectiveChamfer").toDouble(), 10.0);
  EXPECT_EQ(object->property("chamferedCorners").toInt(), 10);
  EXPECT_FALSE(object->property("pathData").toString().isEmpty());
  EXPECT_TRUE(object->property("fillMatchesPalette").toBool());
  EXPECT_TRUE(object->property("borderMatchesPalette").toBool());
  EXPECT_TRUE(std::isnan(object->property("radiusOverride").toDouble()));
  EXPECT_TRUE(std::isnan(object->property("chamferOverride").toDouble()));
  EXPECT_EQ(object->property("chamferedCornersOverride").toInt(), -1);
}

TEST_F(QmlSmoke, HnSurfaceFrame_UsesSemanticFillColorsAndAllowsOverrides) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property bool windowMatches: Qt.colorEqual(windowFrame.fillColor, HoloniightPalette.background)
      property bool panelMatches: Qt.colorEqual(panelFrame.fillColor, HoloniightPalette.surfaceRaised)
      property bool cardMatches: Qt.colorEqual(cardFrame.fillColor, HoloniightPalette.surfaceElevated)
      property bool tooltipMatches: Qt.colorEqual(tooltipFrame.fillColor, popupFrame.fillColor)
      property bool controlMatches: Qt.colorEqual(controlFrame.fillColor, HoloniightPalette.surface)
      property bool overrideMatches: Qt.colorEqual(overrideFrame.fillColor, "#123456")

      HnSurfaceFrame { id: windowFrame; surfaceRole: HnSurfaceRole.Window }
      HnSurfaceFrame { id: panelFrame; surfaceRole: HnSurfaceRole.Panel }
      HnSurfaceFrame { id: cardFrame; surfaceRole: HnSurfaceRole.Card }
      HnSurfaceFrame { id: popupFrame; surfaceRole: HnSurfaceRole.Popup }
      HnSurfaceFrame { id: tooltipFrame; surfaceRole: HnSurfaceRole.Tooltip }
      HnSurfaceFrame { id: controlFrame; surfaceRole: HnSurfaceRole.Control }
      HnSurfaceFrame { id: overrideFrame; surfaceRole: HnSurfaceRole.Card; fillColor: "#123456" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_TRUE(object->property("windowMatches").toBool());
  EXPECT_TRUE(object->property("panelMatches").toBool());
  EXPECT_TRUE(object->property("cardMatches").toBool());
  EXPECT_TRUE(object->property("tooltipMatches").toBool());
  EXPECT_TRUE(object->property("controlMatches").toBool());
  EXPECT_TRUE(object->property("overrideMatches").toBool());
}

TEST_F(QmlSmoke, HnApplicationWindow_UsesCompositorChromeByDefault) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnApplicationWindow {
      id: applicationWindow
      width: 640
      height: 480
      title: "Workspace"
      contentPadding: 20
      applicationHeaderVisible: true
      applicationHeaderHeight: 44
      Item { id: consumerContent; objectName: "consumerContent" }
      applicationHeaderLeadingData: Item { id: leadingContent; objectName: "leadingContent"; implicitWidth: 24 }
      applicationHeaderTrailingData: Item { id: trailingContent; objectName: "trailingContent"; implicitWidth: 48 }
      property bool contentIsVisuallyParented: consumerContent.parent === clientContentItem
      property bool leadingIsVisuallySlotted: leadingContent.parent !== applicationWindow
      property bool trailingIsVisuallySlotted: trailingContent.parent !== applicationWindow
      property bool usesFramelessHint: (flags & Qt.FramelessWindowHint) !== 0
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_DOUBLE_EQ(object->property("effectiveContentPadding").toDouble(), 20.0);
  EXPECT_DOUBLE_EQ(object->property("effectiveApplicationHeaderHeight").toDouble(), 44.0);
  EXPECT_FALSE(object->property("usesFramelessHint").toBool());

  EXPECT_TRUE(object->property("contentIsVisuallyParented").toBool());
  EXPECT_TRUE(object->property("leadingIsVisuallySlotted").toBool());
  EXPECT_TRUE(object->property("trailingIsVisuallySlotted").toBool());
  QObject* title_label = object->findChild<QObject*>(QStringLiteral("hnApplicationWindowHeaderTitle"));
  ASSERT_NE(title_label, nullptr);
  EXPECT_TRUE(title_label->property("visible").toBool());
  EXPECT_EQ(title_label->property("text").toString(), QStringLiteral("Workspace"));
}

TEST_F(QmlSmoke, HnApplicationWindow_HidesHeaderByDefaultAndNormalizesSizes) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnApplicationWindow {
      applicationHeaderHeight: -10
      contentPadding: -8
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_FALSE(object->property("applicationHeaderVisible").toBool());
  EXPECT_DOUBLE_EQ(object->property("effectiveApplicationHeaderHeight").toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(object->property("effectiveContentPadding").toDouble(), 0.0);
}

TEST_F(QmlSmoke, HnAppearance_ReloadsExistingSurfaceFrame) {
  EnvGuard appearanceFileGuard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.json"));
  writeFile(path, R"({"version":1,"cornerStyle":"inherit","shapeScale":1})");
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 200
      height: 100
      surfaceRole: HnSurfaceRole.Card
      chamferedCornersOverride: HnCornerMask.TopRight
      property int appearanceRevisionValue: HnAppearance.revision
      function reloadAppearance() { HnAppearance.reload() }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("shapeKind").toInt(), 2);
  EXPECT_DOUBLE_EQ(object->property("effectiveRadius").toDouble(), 8.0);
  EXPECT_EQ(object->property("chamferedCorners").toInt(), 2);

  writeFile(path, R"({"version":1,"cornerStyle":"chamfered","shapeScale":2})");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadAppearance"));
  QCoreApplication::processEvents();

  EXPECT_EQ(object->property("appearanceRevisionValue").toInt(), 1);
  EXPECT_EQ(object->property("shapeKind").toInt(), 2);
  EXPECT_EQ(object->property("chamferedCorners").toInt(), 2);
  EXPECT_DOUBLE_EQ(object->property("effectiveRadius").toDouble(), 16.0);
  EXPECT_DOUBLE_EQ(object->property("effectiveChamfer").toDouble(), 8.0);
}

TEST_F(QmlSmoke, Button_ReloadsSemanticRadiusFromAppearance) {
  EnvGuard appearanceFileGuard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.json"));
  writeFile(path, R"({"version":1,"shapeScale":1})");
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    import Holonight.Core
    Button {
      width: 100
      height: 32
      property real backgroundRadius: background.radius
      function reloadAppearance() { HnAppearance.reload() }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_DOUBLE_EQ(object->property("backgroundRadius").toDouble(), 6.0);

  writeFile(path, R"({"version":1,"shapeScale":2})");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadAppearance"));
  QCoreApplication::processEvents();

  EXPECT_DOUBLE_EQ(object->property("backgroundRadius").toDouble(), 12.0);
}

TEST_F(QmlSmoke, HnAppearance_RoundedRadiusUsesSemanticRoles) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property real controlRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control, 100, 32)
      property real popupRadius: HnAppearance.roundedRadius(HnSurfaceRole.Popup, 100, 60)
      property real tooltipRadius: HnAppearance.roundedRadius(HnSurfaceRole.Tooltip, 100, 30)
      property real clampedRadius: HnAppearance.roundedRadius(HnSurfaceRole.Popup, 8, 6)
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_DOUBLE_EQ(object->property("controlRadius").toDouble(), 6.0);
  EXPECT_DOUBLE_EQ(object->property("popupRadius").toDouble(), 12.0);
  EXPECT_DOUBLE_EQ(object->property("tooltipRadius").toDouble(), 6.0);
  EXPECT_DOUBLE_EQ(object->property("clampedRadius").toDouble(), 3.0);
}

TEST_F(QmlSmoke, HnSurfaceFrame_ClampsGeometryAndAppliesOverrides) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 10
      height: 6
      borderWidth: 1
      surfaceRole: HnSurfaceRole.Panel
      radiusOverride: 20
      chamferOverride: 20
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_DOUBLE_EQ(object->property("effectiveRadius").toDouble(), 2.5);
  EXPECT_DOUBLE_EQ(object->property("effectiveChamfer").toDouble(), 2.5);
}

TEST_F(QmlSmoke, HnSurfaceFrame_AppliesDirectionalAndArbitraryCornerMasks) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property int leftCorners: leftPanel.chamferedCorners
      property int leftKind: leftPanel.shapeKind
      property int rightCorners: rightPanel.chamferedCorners
      property int oneChamferCorners: oneChamfer.chamferedCorners
      property int threeChamferCorners: threeChamfer.chamferedCorners

      HnSurfaceFrame {
        id: leftPanel
        width: 100
        height: 60
        surfaceRole: HnSurfaceRole.Panel
        chamferedCornersOverride: HnCornerMask.TopRight | HnCornerMask.BottomRight
      }
      HnSurfaceFrame {
        id: rightPanel
        width: 100
        height: 60
        surfaceRole: HnSurfaceRole.Panel
        chamferedCornersOverride: HnCornerMask.TopLeft | HnCornerMask.BottomLeft
      }
      HnSurfaceFrame {
        id: oneChamfer
        width: 100
        height: 60
        chamferedCornersOverride: HnCornerMask.TopRight
      }
      HnSurfaceFrame {
        id: threeChamfer
        width: 100
        height: 60
        chamferedCornersOverride: HnCornerMask.All & ~HnCornerMask.BottomLeft
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->property("leftCorners").toInt(), 6);
  EXPECT_EQ(object->property("leftKind").toInt(), 2);
  EXPECT_EQ(object->property("rightCorners").toInt(), 9);
  EXPECT_EQ(object->property("oneChamferCorners").toInt(), 2);
  EXPECT_EQ(object->property("threeChamferCorners").toInt(), 7);
}

TEST_F(QmlSmoke, HnSurfaceFrame_InvalidCornerMaskInheritsResolvedTopology) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 100
      height: 60
      surfaceRole: HnSurfaceRole.Popup
      chamferedCornersOverride: 16
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->property("shapeKind").toInt(), 2);
  EXPECT_EQ(object->property("chamferedCorners").toInt(), 10);
}

TEST_F(QmlSmoke, HnSurfaceFrame_PillRadiusUsesDrawableHeight) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 80
      height: 25
      borderWidth: 1
      surfaceRole: HnSurfaceRole.Pill
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->property("shapeKind").toInt(), 3);
  EXPECT_DOUBLE_EQ(object->property("effectiveRadius").toDouble(), 12.0);
}

TEST_F(QmlSmoke, HnSurfaceFrame_InvalidEnumsFallBackToCardAndInheritedStyle) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 100
      height: 50
      borderWidth: 0
      surfaceRole: 999
      cornerStyleOverride: 999
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->property("shapeKind").toInt(), 0);
  EXPECT_DOUBLE_EQ(object->property("effectiveRadius").toDouble(), 8.0);
  EXPECT_DOUBLE_EQ(object->property("effectiveChamfer").toDouble(), 4.0);
}

TEST_F(QmlSmoke, HnSurfaceFrame_LoadsEveryRoleAndStyle) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      id: matrixRoot
      width: 400
      height: 400
      property var roles: [HnSurfaceRole.Window, HnSurfaceRole.Panel, HnSurfaceRole.Popup,
                           HnSurfaceRole.Card, HnSurfaceRole.Menu, HnSurfaceRole.Tooltip,
                           HnSurfaceRole.Control, HnSurfaceRole.Pill, HnSurfaceRole.Hud,
                           HnSurfaceRole.WorkspaceIndicator]
      property var styles: [HnCornerStyle.Hybrid, HnCornerStyle.Rounded, HnCornerStyle.Chamfered]

      Repeater {
        model: matrixRoot.roles.length * matrixRoot.styles.length
        HnSurfaceFrame {
          required property int index
          width: 32.5
          height: 17.5
          surfaceRole: matrixRoot.roles[index % matrixRoot.roles.length]
          cornerStyleOverride: matrixRoot.styles[Math.floor(index / matrixRoot.roles.length)]
        }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
}

TEST_F(QmlSmoke, HnSurfaceFrame_ParentsConsumerContent) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSurfaceFrame {
      width: 100
      height: 50
      Item { objectName: "consumerContent" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  QObject* content = object->findChild<QObject*>(QStringLiteral("consumerContent"));
  ASSERT_NE(content, nullptr);
  QQuickItem* content_item = qobject_cast<QQuickItem*>(content);
  QQuickItem* frame_item = qobject_cast<QQuickItem*>(object.get());
  ASSERT_NE(content_item, nullptr);
  ASSERT_NE(frame_item, nullptr);
  EXPECT_NE(content_item->parentItem(), frame_item);
}

TEST_F(QmlSmoke, HnIcon_ProviderUrlUpdatesWhenColorChanges) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    HnIcon {
      source: "qrc:/Holonight/test.svg"
      normalColor: "#ff0000"
      property string initialRenderSource: ""
      Component.onCompleted: {
        initialRenderSource = _renderSource
        normalColor = "#00ff00"
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  const QString initial = object->property("initialRenderSource").toString();
  const QString updated = object->property("_renderSource").toString();
  EXPECT_TRUE(initial.startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_TRUE(updated.startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_NE(initial, updated);
}

TEST_F(QmlSmoke, HnIcon_ProviderUrlUpdatesWhenPaletteReloads) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[appearance]\nscheme=holonight-dark\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    HnIcon {
      source: "qrc:/Holonight/test.svg"
      property string initialRenderSource: ""
      property int initialRevision: -1
      property int currentRevision: HoloniightPalette.revision
      Component.onCompleted: {
        initialRenderSource = _renderSource
        initialRevision = HoloniightPalette.revision
      }
      function reloadPalette() {
        HoloniightPalette.reload()
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  const QString initial = object->property("initialRenderSource").toString();
  writeFile(path, "[appearance]\nscheme=holonight-light\n");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadPalette", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  const QString updated = object->property("_renderSource").toString();
  EXPECT_GT(object->property("currentRevision").toInt(), object->property("initialRevision").toInt());
  EXPECT_TRUE(initial.startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_TRUE(updated.startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_NE(initial, updated);

  writeFile(path, R"({"version":1,"appearance":{"scheme":"holonight-dark"}})");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadPalette", Qt::DirectConnection));
}

TEST_F(QmlSmoke, HoloniightPalette_PrimaryIsValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item { property color c: HoloniightPalette.primary }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HoloniightPalette_DesignSystemAccentsAreValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property color cyan: HoloniightPalette.accentCyan
      property color blue: HoloniightPalette.accentBlue
      property color violet: HoloniightPalette.accentViolet
      property color yellow: HoloniightPalette.accentYellow
      property color subtle: HoloniightPalette.textSubtle
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HoloniightPalette_CanonicalPropertiesAreValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property color background: HoloniightPalette.background
      property color surfaceElevated: HoloniightPalette.surfaceElevated
      property color surfaceRaised: HoloniightPalette.surfaceRaised
      property color textPrimary: HoloniightPalette.textPrimary
      property color textSecondary: HoloniightPalette.textSecondary
      property color textMuted: HoloniightPalette.textMuted
      property color textDisabled: HoloniightPalette.textDisabled
      property color textAccent: HoloniightPalette.textAccent
      property color borderSubtle: HoloniightPalette.borderSubtle
      property color borderStrong: HoloniightPalette.borderStrong
      property color hoverOverlay: HoloniightPalette.hoverOverlay
      property int focusBorderWidth: HoloniightPalette.focusBorderWidth
      property color ansiBlue: HoloniightPalette.ansiBlue
      property color ansiBrightWhite: HoloniightPalette.ansiBrightWhite
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}



TEST_F(QmlSmoke, HoloniightPalette_ReloadEmitsNotificationOnChange) {
  // Force singleton creation in dark mode first.
  {
    QQmlComponent init = QQmlComponent{&engine_};
    init.setData(R"(import QtQuick; import Holonight.Core; Item { property color c: HoloniightPalette.primary })",
                 QUrl{});
    std::unique_ptr<QObject> tmp{init.create()};
    ASSERT_NE(tmp, nullptr);
  }

  // Switching to light forces a token change → signal must fire.
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qputenv("HOLONIGHT_APPEARANCE_MODE", "light");

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property int changedCount: 0
      Connections {
        target: HoloniightPalette
        function onPaletteChanged() { changedCount += 1 }
      }
      Component.onCompleted: HoloniightPalette.reload()
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_GE(object->property("changedCount").toInt(), 1);
}

TEST_F(QmlSmoke, HoloniightPalette_ReloadUsesLightAppearanceMode) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qputenv("HOLONIGHT_APPEARANCE_MODE", "light");

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property color background: HoloniightPalette.background
      Component.onCompleted: {
        HoloniightPalette.reload()
        background = HoloniightPalette.background
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("background").value<QColor>(), QColor(QStringLiteral("#E7EEF5")));
}

TEST_F(QmlSmoke, HoloniightPalette_ReloadUsesSchemeBeforeModeAndAppliesAccent) {
  EnvGuard configGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[appearance]\nscheme=holonight-light\naccent=blue\nmode=dark\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property color background: "transparent"
      property color primary: "transparent"
      property color brandForeground: "transparent"
      Component.onCompleted: {
        HoloniightPalette.reload()
        background = HoloniightPalette.background
        primary = HoloniightPalette.primary
        brandForeground = HoloniightPalette.brandForeground
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("background").value<QColor>(), QColor(QStringLiteral("#E7EEF5")));
  EXPECT_EQ(object->property("primary").value<QColor>(), QColor(QStringLiteral("#3e7bdb")));
  EXPECT_EQ(object->property("brandForeground").value<QColor>(), QColor(QStringLiteral("#685BBD")));
}

TEST_F(QmlSmoke, HoloniightPalette_WatchesThemeConfigChanges) {
  EnvGuard configGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[appearance]\nscheme=holonight-dark\nmode=dark\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property color background: HoloniightPalette.background
      Connections {
        target: HoloniightPalette
        function onPaletteChanged() { background = HoloniightPalette.background }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("background").value<QColor>(), QColor(QStringLiteral("#0C1118")));

  writeFile(path, "[appearance]\nscheme=holonight-light\nmode=light\n");
  EXPECT_TRUE(waitForPropertyColor(object.get(), "background", QColor(QStringLiteral("#E7EEF5"))));
}

TEST_F(QmlSmoke, HolonightTheme_ConfigPropertiesAreValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property string iconTheme: HolonightTheme.iconTheme
      property string uiFont: HolonightTheme.uiFont
      property int bodySize: HolonightTheme.bodySize
      property int captionSize: HolonightTheme.captionSize
      property int appTitleSize: HolonightTheme.appTitleSize
      property color brandForeground: HoloniightPalette.brandForeground
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("appTitleSize").toInt(), 14);
  EXPECT_EQ(object->property("brandForeground").value<QColor>(), QColor(QStringLiteral("#9A8CFF")));
}

TEST_F(QmlSmoke, HolonightTheme_ReloadUpdatesExistingLabelTypography) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[fonts]\nui=Initial Sans\nbaseSize=10\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      HnLabel { id: label; rawText: "Existing label" }
      property string labelFamily: label.font.family
      property real labelSize: label.font.pointSize
      function reloadTheme() { HolonightTheme.reload() }
      Component.onCompleted: reloadTheme()
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("labelFamily").toString(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(object->property("labelSize").toInt(), 10);

  writeFile(path, "[fonts]\nui=Reloaded Sans\nbaseSize=14\n");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadTheme"));
  QCoreApplication::processEvents();

  EXPECT_EQ(object->property("labelFamily").toString(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(object->property("labelSize").toInt(), 14);
}

TEST_F(QmlSmoke, HolonightTheme_ReloadUpdatesSharedControlsTypography) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[fonts]\nui=Initial Sans\nbaseSize=10\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight
    import Holonight.Controls
    Item {
      id: container
      Button { id: btn; objectName: "btn"; text: "Button" }
      HnNavigationDelegate { id: nav; objectName: "nav"; title: "Nav Title"; badgeText: "Badge" }
      HnSettingsRow { id: row; objectName: "row"; titleText: "Row Title"; descriptionText: "Row Description" }
      HnStatusIndicator { id: status; objectName: "status"; text: "Status Text" }
      CheckBox { id: check; objectName: "check"; text: "CheckBox" }

      function reloadTheme() { HolonightTheme.reload() }
      Component.onCompleted: reloadTheme()
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  QObject* btn = object->findChild<QObject*>(QStringLiteral("btn"));
  QObject* nav = object->findChild<QObject*>(QStringLiteral("nav"));
  QObject* row = object->findChild<QObject*>(QStringLiteral("row"));
  QObject* status = object->findChild<QObject*>(QStringLiteral("status"));
  QObject* check = object->findChild<QObject*>(QStringLiteral("check"));

  ASSERT_NE(btn, nullptr);
  ASSERT_NE(nav, nullptr);
  ASSERT_NE(row, nullptr);
  ASSERT_NE(status, nullptr);
  ASSERT_NE(check, nullptr);

  QObject* nav_title = nav->findChild<QObject*>(QStringLiteral("hnNavigationDelegateTitle"));
  QObject* row_title = row->findChild<QObject*>(QStringLiteral("hnSettingsRowTitle"));
  QObject* row_desc = row->findChild<QObject*>(QStringLiteral("hnSettingsRowDescription"));
  QObject* status_text = status->findChild<QObject*>(QStringLiteral("hnStatusIndicatorText"));

  ASSERT_NE(nav_title, nullptr);
  ASSERT_NE(row_title, nullptr);
  ASSERT_NE(row_desc, nullptr);
  ASSERT_NE(status_text, nullptr);

  // Initial expectations
  EXPECT_EQ(btn->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(btn->property("font").value<QFont>().pointSize(), 10);
  EXPECT_EQ(nav_title->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(nav_title->property("font").value<QFont>().pointSize(), 10);
  EXPECT_EQ(row_title->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(row_title->property("font").value<QFont>().pointSize(), 10);
  EXPECT_EQ(row_desc->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(status_text->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(check->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(check->property("font").value<QFont>().pointSize(), 10);

  // Update theme configuration file and reload
  writeFile(path, "[fonts]\nui=Reloaded Sans\nbaseSize=14\n");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadTheme"));
  QCoreApplication::processEvents();

  // Reloaded expectations (existing instances updated without recreation)
  EXPECT_EQ(btn->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(btn->property("font").value<QFont>().pointSize(), 14);
  EXPECT_EQ(nav_title->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(nav_title->property("font").value<QFont>().pointSize(), 14);
  EXPECT_EQ(row_title->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(row_title->property("font").value<QFont>().pointSize(), 14);
  EXPECT_EQ(row_desc->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(status_text->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(check->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(check->property("font").value<QFont>().pointSize(), 14);
}

TEST_F(QmlSmoke, HolonightTheme_ExplicitFontOverridesPreservedOnReload) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[fonts]\nui=Initial Sans\nbaseSize=10\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight
    import Holonight.Controls
    Item {
      Button { id: btn; objectName: "customBtn"; font.pointSize: 22 }
      HnNavigationDelegate { id: nav; objectName: "customNav"; font.family: "Custom Nav Font"; title: "Nav" }
      HnCardDelegate { id: card; objectName: "customCard"; font.pointSize: 18; title: "Card" }

      function reloadTheme() { HolonightTheme.reload() }
      Component.onCompleted: reloadTheme()
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  QObject* custom_btn = object->findChild<QObject*>(QStringLiteral("customBtn"));
  QObject* custom_nav = object->findChild<QObject*>(QStringLiteral("customNav"));
  QObject* custom_card = object->findChild<QObject*>(QStringLiteral("customCard"));

  ASSERT_NE(custom_btn, nullptr);
  ASSERT_NE(custom_nav, nullptr);
  ASSERT_NE(custom_card, nullptr);

  EXPECT_EQ(custom_btn->property("font").value<QFont>().pointSize(), 22);
  EXPECT_EQ(custom_btn->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(custom_nav->property("font").value<QFont>().family(), QStringLiteral("Custom Nav Font"));
  EXPECT_EQ(custom_card->property("font").value<QFont>().pointSize(), 18);

  // Reload theme
  writeFile(path, "[fonts]\nui=Reloaded Sans\nbaseSize=14\n");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadTheme"));
  QCoreApplication::processEvents();

  // Explicit font overrides are preserved!
  EXPECT_EQ(custom_btn->property("font").value<QFont>().pointSize(), 22);
  EXPECT_EQ(custom_btn->property("font").value<QFont>().family(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(custom_nav->property("font").value<QFont>().family(), QStringLiteral("Custom Nav Font"));
  EXPECT_EQ(custom_card->property("font").value<QFont>().pointSize(), 18);
}

TEST_F(QmlSmoke, Controls_AppTitleRequiresApplicationName) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData("import Holonight.Controls; HnAppTitle {}", QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> title{comp.create()};
  EXPECT_EQ(title, nullptr);
}

TEST_F(QmlSmoke, Controls_AppTitleUsesSemanticPresentationAndAccessibility) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnAppTitle {
      applicationName: "Settings"
      iconSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
      iconTinted: true
      property string reportedAccessibleName: Accessible.name
      property int reportedAccessibleRole: Accessible.role
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> title{comp.create()};
  ASSERT_NE(title, nullptr);
  QCoreApplication::processEvents();

  QObject* brand = title->findChild<QObject*>(QStringLiteral("brandLabel"));
  QObject* application = title->findChild<QObject*>(QStringLiteral("applicationLabel"));
  QObject* icon = title->findChild<QObject*>(QStringLiteral("appTitleIcon"));
  ASSERT_NE(brand, nullptr);
  ASSERT_NE(application, nullptr);
  ASSERT_NE(icon, nullptr);

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  EXPECT_EQ(brand->property("text").toString(), QStringLiteral("HoloNight"));
  EXPECT_EQ(brand->property("color").value<QColor>(), tok.brandForeground);
  EXPECT_EQ(application->property("color").value<QColor>(), tok.textPrimary);
  EXPECT_EQ(brand->property("font").value<QFont>().family(), QStringLiteral("Inter"));
  EXPECT_EQ(brand->property("font").value<QFont>().pointSize(), 14);
  EXPECT_EQ(brand->property("font").value<QFont>().weight(), QFont::DemiBold);
  EXPECT_EQ(application->property("font").value<QFont>(), brand->property("font").value<QFont>());
  EXPECT_EQ(icon->property("size").toInt(), 32);
  EXPECT_TRUE(icon->property("tinted").toBool());
  EXPECT_TRUE(icon->property("accessibilityIgnored").toBool());
  EXPECT_EQ(title->property("reportedAccessibleName").toString(), QStringLiteral("HoloNight Settings"));
  EXPECT_EQ(title->property("reportedAccessibleRole").toInt(), QAccessible::StaticText);
}

TEST_F(QmlSmoke, Controls_AppTitleIconFailureAndConstraintsUseNoReservedSpace) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias emptyTitle: emptyTitle
      property alias failedTitle: failedTitle
      property alias constrainedTitle: constrainedTitle
      HnAppTitle { id: emptyTitle; applicationName: "Settings" }
      HnAppTitle {
        id: failedTitle
        applicationName: "Settings"
        iconSource: "file:///definitely/missing/holonight-app-title.svg"
      }
      HnAppTitle {
        id: constrainedTitle
        applicationName: "An application name that must elide"
        width: 160
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QTest::qWait(50);

  QObject* empty = root->property("emptyTitle").value<QObject*>();
  QObject* failed = root->property("failedTitle").value<QObject*>();
  QObject* constrained = root->property("constrainedTitle").value<QObject*>();
  ASSERT_NE(empty, nullptr);
  ASSERT_NE(failed, nullptr);
  ASSERT_NE(constrained, nullptr);
  EXPECT_DOUBLE_EQ(failed->property("implicitWidth").toDouble(), empty->property("implicitWidth").toDouble());
  EXPECT_GT(empty->property("implicitWidth").toDouble(), 0.0);
  EXPECT_GT(empty->property("implicitHeight").toDouble(), 0.0);
  QObject* application = constrained->findChild<QObject*>(QStringLiteral("applicationLabel"));
  ASSERT_NE(application, nullptr);
  EXPECT_EQ(application->property("elide").toInt(), Qt::ElideRight);
  EXPECT_LE(application->property("width").toDouble(), constrained->property("width").toDouble());
  constrained->setProperty("width", 400);
  EXPECT_DOUBLE_EQ(constrained->property("width").toDouble(), 400.0);
}

TEST_F(QmlSmoke, Controls_AppTitleSkipBrandingOmitsBrandAndSpace) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property alias defaultTitle: defaultTitle
      property alias skippedTitle: skippedTitle
      HnAppTitle {
        id: defaultTitle
        applicationName: "Settings"
        property string reportedAccessibleName: Accessible.name
      }
      HnAppTitle {
        id: skippedTitle
        applicationName: "Settings"
        skipBranding: true
        property string reportedAccessibleName: Accessible.name
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QTest::qWait(50);

  QObject* defaultTitle = root->property("defaultTitle").value<QObject*>();
  QObject* skippedTitle = root->property("skippedTitle").value<QObject*>();
  ASSERT_NE(defaultTitle, nullptr);
  ASSERT_NE(skippedTitle, nullptr);

  EXPECT_FALSE(defaultTitle->property("skipBranding").toBool());
  EXPECT_TRUE(skippedTitle->property("skipBranding").toBool());

  QObject* defaultBrand = defaultTitle->findChild<QObject*>(QStringLiteral("brandLabel"));
  QObject* skippedBrand = skippedTitle->findChild<QObject*>(QStringLiteral("brandLabel"));
  ASSERT_NE(defaultBrand, nullptr);
  ASSERT_NE(skippedBrand, nullptr);

  EXPECT_TRUE(defaultBrand->property("visible").toBool());
  EXPECT_FALSE(skippedBrand->property("visible").toBool());

  EXPECT_EQ(defaultTitle->property("reportedAccessibleName").toString(), QStringLiteral("HoloNight Settings"));
  EXPECT_EQ(skippedTitle->property("reportedAccessibleName").toString(), QStringLiteral("Settings"));

  EXPECT_GT(defaultTitle->property("implicitWidth").toDouble(), skippedTitle->property("implicitWidth").toDouble());
}

TEST_F(QmlSmoke, HolonightTheme_CatalogPropertiesAreReadable) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property int familyCount: HolonightTheme.themeFamilies.length
      property int variantCount: HolonightTheme.themeVariants.length
      property var accents: HolonightTheme.accentOptionsForScheme("holonight-latte")
      property string firstAccentId: accents[0].id
      property color firstAccentColor: accents[0].color
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("familyCount").toInt(), 6);
  EXPECT_EQ(object->property("variantCount").toInt(), 12);
  EXPECT_EQ(object->property("firstAccentId").toString(), QStringLiteral("cyan"));
  EXPECT_TRUE(object->property("firstAccentColor").value<QColor>().isValid());
}

TEST_F(QmlSmoke, Controls_SettingsRowContractsAndComposition) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    HnSettingsRow {
      width: 480
      sizeRole: 999
      titleText: "Updates"
      descriptionText: "Install updates automatically when available."
      leadingContent: Component { Item { objectName: "leading"; implicitWidth: 20; implicitHeight: 20 } }
      control: Component { HnSearchField { objectName: "control"; enabled: parent.enabled } }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> row{comp.create()};
  ASSERT_NE(row, nullptr);
  QObject* leading = row->property("leadingItem").value<QObject*>();
  QObject* control = row->property("controlItem").value<QObject*>();
  ASSERT_NE(leading, nullptr);
  ASSERT_NE(control, nullptr);
  EXPECT_EQ(leading->objectName(), QStringLiteral("leading"));
  EXPECT_EQ(control->objectName(), QStringLiteral("control"));
  EXPECT_EQ(row->property("resolvedSizeRole").toInt(), 1);
  EXPECT_FALSE(row->property("stacked").toBool());
  EXPECT_EQ(row->property("contentHorizontalPadding").toReal(), 0);
  row->setProperty("stacked", true);
  QCoreApplication::processEvents();
  EXPECT_GE(control->property("y").toReal(), leading->property("y").toReal());
  row->setProperty("enabled", false);
  EXPECT_FALSE(control->property("enabled").toBool());
}

TEST_F(QmlSmoke, Controls_SettingsRowPadsContentWithoutNarrowingDivider) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Layouts
    import Holonight.Controls
    Item {
      width: 480
      height: 240
      property alias defaultRow: defaultRow
      property alias paddedRow: paddedRow
      HnSettingsRow {
        id: defaultRow
        objectName: "defaultRow"
        width: parent.width
        height: 96
        titleText: "Default"
        dividerVisible: true
        control: Component {
          Item {
            objectName: "defaultControl"
            implicitWidth: 80
            implicitHeight: 24
          }
        }
      }
      HnSettingsRow {
        id: paddedRow
        objectName: "paddedRow"
        y: 112
        width: parent.width
        height: 96
        titleText: "Padded"
        contentHorizontalPadding: 24
        dividerVisible: true
        control: Component {
          Item {
            objectName: "paddedControl"
            implicitWidth: 80
            implicitHeight: 24
          }
        }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* default_row = qobject_cast<QQuickItem*>(object->property("defaultRow").value<QObject*>());
  auto* padded_row = qobject_cast<QQuickItem*>(object->property("paddedRow").value<QObject*>());
  ASSERT_NE(default_row, nullptr);
  ASSERT_NE(padded_row, nullptr);
  auto* default_control = qobject_cast<QQuickItem*>(default_row->property("controlItem").value<QObject*>());
  auto* padded_control = qobject_cast<QQuickItem*>(padded_row->property("controlItem").value<QObject*>());
  auto* default_divider = default_row->findChild<QQuickItem*>(QStringLiteral("settingsRowDivider"));
  auto* padded_divider = padded_row->findChild<QQuickItem*>(QStringLiteral("settingsRowDivider"));
  ASSERT_NE(default_control, nullptr);
  ASSERT_NE(padded_control, nullptr);
  ASSERT_NE(default_divider, nullptr);
  ASSERT_NE(padded_divider, nullptr);

  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_NEAR(default_control->mapToItem(default_row, QPointF{}).x() + default_control->width(), default_row->width(),
              0.01);
  EXPECT_NEAR(padded_control->mapToItem(padded_row, QPointF{}).x() + padded_control->width(), padded_row->width() - 24,
              0.01);
  EXPECT_NEAR(default_control->mapToItem(default_row, QPointF{}).y() + default_control->height() / 2,
              default_row->height() / 2, 0.51);
  EXPECT_NEAR(padded_control->mapToItem(padded_row, QPointF{}).y() + padded_control->height() / 2,
              padded_row->height() / 2, 0.51);
  EXPECT_NEAR(default_divider->width(), default_row->width(), 0.01);
  EXPECT_NEAR(padded_divider->width(), padded_row->width(), 0.01);
}

TEST_F(QmlSmoke, Controls_SettingsRowStackedControlsShareResponsiveGeometry) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls.Basic
    import QtQuick.Layouts
    import Holonight.Core
    import Holonight.Controls
    Window {
      visible: true
      width: 640
      height: content.implicitHeight
      property alias compact: compact
      property alias normal: normal
      property alias inlineRow: inlineRow

      component CompoundControl: RowLayout {
        objectName: "compound"
        Switch {
          objectName: "switch"
          Layout.alignment: Qt.AlignVCenter
        }
        Slider {
          objectName: "slider"
          Layout.fillWidth: true
          Layout.alignment: Qt.AlignVCenter
        }
        Label {
          objectName: "value"
          text: "12 px"
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: 52
          Layout.alignment: Qt.AlignVCenter
        }
      }

      ColumnLayout {
        id: content
        width: parent.width

        HnSettingsRow {
          id: compact
          titleText: "Shape"
          descriptionText: "Compact row"
          sizeRole: HnControlSize.Compact
          stacked: true
          Layout.fillWidth: true
          control: Component { CompoundControl {} }
        }
        HnSettingsRow {
          id: normal
          titleText: "Override the base chamfer used by every application surface when translated"
          descriptionText: "This description wraps at narrow widths without moving the controls."
          sizeRole: HnControlSize.Normal
          enabled: false
          stacked: true
          Layout.fillWidth: true
          leadingContent: Component {
            Item {
              objectName: "leading"
              implicitWidth: 36
              implicitHeight: 24
            }
          }
          control: Component { CompoundControl {} }
        }
        HnSettingsRow {
          id: inlineRow
          titleText: "Inline"
          Layout.fillWidth: true
          control: Component {
            Switch {
              objectName: "inlineControl"
            }
          }
        }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);

  auto* compact = qobject_cast<QQuickItem*>(object->property("compact").value<QObject*>());
  auto* normal = qobject_cast<QQuickItem*>(object->property("normal").value<QObject*>());
  auto* inline_row = qobject_cast<QQuickItem*>(object->property("inlineRow").value<QObject*>());
  ASSERT_NE(compact, nullptr);
  ASSERT_NE(normal, nullptr);
  ASSERT_NE(inline_row, nullptr);

  auto* compact_control = qobject_cast<QQuickItem*>(compact->property("controlItem").value<QObject*>());
  auto* normal_control = qobject_cast<QQuickItem*>(normal->property("controlItem").value<QObject*>());
  auto* inline_control = qobject_cast<QQuickItem*>(inline_row->property("controlItem").value<QObject*>());
  ASSERT_NE(compact_control, nullptr);
  ASSERT_NE(normal_control, nullptr);
  ASSERT_NE(inline_control, nullptr);

  const auto verify_stacked_geometry = [&]() {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    EXPECT_NEAR(compact_control->mapToScene(QPointF{}).x(), compact->mapToScene(QPointF{}).x(), 0.01);
    EXPECT_NEAR(normal_control->mapToScene(QPointF{}).x(), normal->mapToScene(QPointF{}).x(), 0.01);
    EXPECT_NEAR(compact_control->mapToScene(QPointF{}).x(), normal_control->mapToScene(QPointF{}).x(), 0.01);
    EXPECT_NEAR(compact_control->width(), compact->width(), 0.01);
    EXPECT_NEAR(normal_control->width(), normal->width(), 0.01);
    EXPECT_NEAR(compact_control->width(), normal_control->width(), 0.01);

    for (QQuickItem* control : {compact_control, normal_control}) {
      auto* switch_item = control->findChild<QQuickItem*>(QStringLiteral("switch"));
      auto* slider = control->findChild<QQuickItem*>(QStringLiteral("slider"));
      auto* value = control->findChild<QQuickItem*>(QStringLiteral("value"));
      ASSERT_NE(switch_item, nullptr);
      ASSERT_NE(slider, nullptr);
      ASSERT_NE(value, nullptr);
      EXPECT_NEAR(switch_item->mapToScene(QPointF{0, switch_item->height() / 2}).y(),
                  slider->mapToScene(QPointF{0, slider->height() / 2}).y(), 0.51);
      EXPECT_NEAR(value->mapToScene(QPointF{value->width(), 0}).x(),
                  control->mapToScene(QPointF{control->width(), 0}).x(), 0.01);
    }
  };

  verify_stacked_geometry();
  window->setWidth(360);
  verify_stacked_geometry();
  window->setWidth(760);
  verify_stacked_geometry();

  EXPECT_GT(inline_control->mapToScene(QPointF{}).x(), inline_row->mapToScene(QPointF{}).x());
}

TEST_F(QmlSmoke, Controls_SettingsRowForwardsSingleFocusAndSkipsCompoundRoot) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls.Basic
    import QtQuick.Layouts
    import Holonight.Controls
    Window {
      visible: true
      width: 480
      height: content.implicitHeight
      property alias before: before
      property alias singleRow: singleRow
      property alias compoundRow: compoundRow

      ColumnLayout {
        id: content
        width: parent.width
        Button { id: before; text: "Before" }
        HnSettingsRow {
          id: singleRow
          titleText: "Single"
          Layout.fillWidth: true
          control: Component {
            Switch { objectName: "singleControl" }
          }
        }
        HnSettingsRow {
          id: compoundRow
          titleText: "Compound"
          stacked: true
          Layout.fillWidth: true
          control: Component {
            RowLayout {
              objectName: "compoundRoot"
              Switch {
                objectName: "compoundSwitch"
                Layout.alignment: Qt.AlignVCenter
              }
              Slider {
                objectName: "compoundSlider"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
              }
            }
          }
        }
        Button { text: "After" }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);

  auto* before = qobject_cast<QQuickItem*>(object->property("before").value<QObject*>());
  QObject* single_row = object->property("singleRow").value<QObject*>();
  QObject* compound_row = object->property("compoundRow").value<QObject*>();
  ASSERT_NE(before, nullptr);
  ASSERT_NE(single_row, nullptr);
  ASSERT_NE(compound_row, nullptr);
  auto* single = qobject_cast<QQuickItem*>(single_row->property("controlItem").value<QObject*>());
  auto* compound = qobject_cast<QQuickItem*>(compound_row->property("controlItem").value<QObject*>());
  ASSERT_NE(single, nullptr);
  ASSERT_NE(compound, nullptr);
  auto* compound_switch = compound->findChild<QQuickItem*>(QStringLiteral("compoundSwitch"));
  auto* compound_slider = compound->findChild<QQuickItem*>(QStringLiteral("compoundSlider"));
  ASSERT_NE(compound_switch, nullptr);
  ASSERT_NE(compound_slider, nullptr);

  EXPECT_TRUE(single_row->property("activeFocusOnTab").toBool());
  EXPECT_FALSE(compound_row->property("activeFocusOnTab").toBool());
  EXPECT_FALSE(compound->property("activeFocusOnTab").toBool());

  before->forceActiveFocus();
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  QTest::keyClick(window, Qt::Key_Tab);
  EXPECT_EQ(window->activeFocusItem(), single);
  QTest::keyClick(window, Qt::Key_Tab);
  EXPECT_EQ(window->activeFocusItem(), compound_switch);
  EXPECT_NE(window->activeFocusItem(), compound);
  QTest::keyClick(window, Qt::Key_Tab);
  EXPECT_EQ(window->activeFocusItem(), compound_slider);
  EXPECT_NE(window->activeFocusItem(), compound);
}

TEST_F(QmlSmoke, Controls_SettingsRowSemanticSizesChangeGeometry) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Item {
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias hero: hero
      HnSettingsRow {
        id: compact
        width: 480
        sizeRole: HnControlSize.Compact
        titleText: "Setting"
        descriptionText: "Description"
      }
      HnSettingsRow {
        id: normal
        width: 480
        sizeRole: HnControlSize.Normal
        titleText: "Setting"
        descriptionText: "Description"
      }
      HnSettingsRow {
        id: large
        width: 480
        sizeRole: HnControlSize.Large
        titleText: "Setting"
        descriptionText: "Description"
      }
      HnSettingsRow {
        id: hero
        width: 480
        sizeRole: HnControlSize.Hero
        titleText: "Setting"
        descriptionText: "Description"
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  const qreal compact_height = root->property("compact").value<QObject*>()->property("implicitHeight").toReal();
  const qreal normal_height = root->property("normal").value<QObject*>()->property("implicitHeight").toReal();
  const qreal large_height = root->property("large").value<QObject*>()->property("implicitHeight").toReal();
  const qreal hero_height = root->property("hero").value<QObject*>()->property("implicitHeight").toReal();
  EXPECT_LT(compact_height, normal_height);
  EXPECT_LT(normal_height, large_height);
  EXPECT_LT(large_height, hero_height);
}

TEST_F(QmlSmoke, Controls_SectionHeaderCollapsesAndOwnsSlots) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls.Basic
    import Holonight.Core
    import Holonight.Controls
    Item {
      property alias empty: empty
      property alias composed: composed
      property alias compactHeader: compactHeader
      property alias categoryHeader: categoryHeader
      property color expectedCategoryColor: HoloniightPalette.textAccent
      HnSectionHeader { id: empty; dividerVisible: false }
      HnSectionHeader {
        id: composed
        width: 400
        titleText: "Network"
        descriptionText: "Connection settings"
        leadingContent: Component { Item { objectName: "leading"; implicitWidth: 12; implicitHeight: 12 } }
        trailingContent: Component { Button { objectName: "action"; text: "Reset" } }
      }
      HnSectionHeader {
        id: compactHeader
        width: 400
        titleText: "Compact Section"
        sizeRole: HnControlSize.Compact
      }
      HnSectionHeader {
        id: categoryHeader
        objectName: "categoryHeader"
        width: 400
        titleText: "Theme"
        isCategoryMode: true
        showPrefix: true
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QObject* empty = root->property("empty").value<QObject*>();
  QObject* composed = root->property("composed").value<QObject*>();
  QObject* compactHeader = root->property("compactHeader").value<QObject*>();
  QObject* categoryHeader = root->property("categoryHeader").value<QObject*>();
  ASSERT_NE(empty, nullptr);
  ASSERT_NE(composed, nullptr);
  ASSERT_NE(compactHeader, nullptr);
  ASSERT_NE(categoryHeader, nullptr);
  EXPECT_EQ(empty->property("implicitHeight").toReal(), 0.0);
  ASSERT_NE(composed->property("leadingItem").value<QObject*>(), nullptr);
  ASSERT_NE(composed->property("trailingItem").value<QObject*>(), nullptr);
  EXPECT_FALSE(composed->property("activeFocusOnTab").toBool());
  EXPECT_EQ(compactHeader->property("resolvedSizeRole").toInt(), 0);
  QObject* categoryTitle = categoryHeader->findChild<QObject*>(QStringLiteral("sectionTitleLabel"));
  ASSERT_NE(categoryTitle, nullptr);
  EXPECT_EQ(categoryTitle->property("rawText").toString(), QStringLiteral("// Theme"));
  EXPECT_EQ(categoryTitle->property("color").value<QColor>(), root->property("expectedCategoryColor").value<QColor>());
  EXPECT_EQ(categoryTitle->property("font").value<QFont>().pointSizeF(), 9.0);
  EXPECT_EQ(categoryTitle->property("font").value<QFont>().letterSpacing(), 0.0);
}

TEST_F(QmlSmoke, Controls_EmptyStateGraphicPrecedenceAndAccessibility) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls.Basic
    import Holonight.Controls
    HnEmptyState {
      width: 360
      titleText: "Nothing here"
      descriptionText: "A deliberately long explanation that must wrap within the available width."
      readonly property string reportedAccessibleName: Accessible.name
      iconSource: "missing.svg"
      graphicContent: Component { Item { objectName: "graphic"; implicitWidth: 40; implicitHeight: 30 } }
      actionContent: Component { Button { objectName: "action"; text: "Create" } }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> state{comp.create()};
  ASSERT_NE(state, nullptr);
  ASSERT_NE(state->property("graphicItem").value<QObject*>(), nullptr);
  ASSERT_NE(state->property("actionItem").value<QObject*>(), nullptr);
  EXPECT_FALSE(state->property("hasDefaultIcon").toBool());
  EXPECT_EQ(state->property("reportedAccessibleName").toString(), QStringLiteral("Nothing here"));
  state->setProperty("enabled", false);
  EXPECT_FALSE(state->property("actionItem").value<QObject*>()->property("enabled").toBool());
}

TEST_F(QmlSmoke, Controls_LoadingStateNormalizesProgressAndGatesIndicator) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls.Basic
    import Holonight.Controls
    HnLoadingState {
      width: 360
      titleText: "Loading"
      actionContent: Component { Button { objectName: "cancel"; text: "Cancel" } }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> state{comp.create()};
  ASSERT_NE(state, nullptr);
  QObject* progress = state->findChild<QObject*>(QStringLiteral("hnLoadingProgress"));
  ASSERT_NE(progress, nullptr);
  EXPECT_EQ(state->property("normalizedProgress").toReal(), -1.0);
  EXPECT_TRUE(progress->property("indeterminate").toBool());
  state->setProperty("progress", 1.5);
  EXPECT_EQ(state->property("normalizedProgress").toReal(), 1.0);
  EXPECT_FALSE(progress->property("indeterminate").toBool());
  state->setProperty("progress", -0.1);
  state->setProperty("running", false);
  EXPECT_FALSE(progress->property("visible").toBool());
  EXPECT_FALSE(progress->property("indeterminate").toBool());
  ASSERT_NE(state->property("actionItem").value<QObject*>(), nullptr);
}

TEST_F(QmlSmoke, Controls_SharedSelectionTypesConstructAndOwnSlots) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias navigation: navigation
      property alias list: list
      property alias card: card
      property alias action: action
      HnNavigationDelegate {
        id: navigation
        title: "Navigation"
        checked: true
        leadingContent: Component { Item { objectName: "navLeading" } }
      }
      HnListDelegate {
        id: list
        title: "List"
        subtitle: "Description"
        trailingContent: Component { Item { objectName: "listTrailing" } }
      }
      HnCardDelegate { id: card; title: "Card"; statusText: "Ready" }
      HnActionDelegate { id: action; title: "Action"; description: "Opens a page" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QObject* navigation = root->property("navigation").value<QObject*>();
  QObject* list = root->property("list").value<QObject*>();
  ASSERT_NE(navigation, nullptr);
  ASSERT_NE(list, nullptr);
  EXPECT_TRUE(navigation->property("selected").toBool());
  EXPECT_EQ(navigation->property("leadingItem").value<QObject*>()->objectName(), QStringLiteral("navLeading"));
  EXPECT_EQ(list->property("trailingItem").value<QObject*>()->objectName(), QStringLiteral("listTrailing"));
  EXPECT_NE(root->property("card").value<QObject*>(), nullptr);
  EXPECT_NE(root->property("action").value<QObject*>(), nullptr);
}

TEST_F(QmlSmoke, Controls_ListDelegateLeadingContentAlignmentPolicy) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    import Holonight.Controls
    Window {
      visible: true
      width: 360
      height: 200
      property alias list: list
      property alias centeredList: centeredList
      property int alignTop: Qt.AlignTop
      property int alignVCenter: Qt.AlignVCenter
      Component {
        id: leadingComponent
        Item {
          objectName: "listLeading"
          implicitWidth: 28
          implicitHeight: 28
        }
      }
      HnListDelegate {
        id: list
        width: parent.width
        sizeRole: HnControlSize.Large
        title: "Built-in Audio Analog Stereo"
        subtitle: "alsa_output.pci-0000_00_1f.3.analog-stereo"
        leadingContent: leadingComponent
      }
      HnListDelegate {
        id: centeredList
        y: 100
        width: parent.width
        sizeRole: HnControlSize.Large
        title: "Built-in Audio Analog Stereo"
        subtitle: "alsa_output.pci-0000_00_1f.3.analog-stereo"
        leadingContentAlignment: Qt.AlignVCenter
        leadingContent: leadingComponent
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(root.get());
  ASSERT_NE(window, nullptr);
  QObject* list = root->property("list").value<QObject*>();
  QObject* centered_list = root->property("centeredList").value<QObject*>();
  ASSERT_NE(list, nullptr);
  ASSERT_NE(centered_list, nullptr);
  QObject* leading = list->property("leadingItem").value<QObject*>();
  QObject* leading_loader = list->findChild<QObject*>(QStringLiteral("hnListDelegateLeadingLoader"));
  QObject* centered_loader = centered_list->findChild<QObject*>(QStringLiteral("hnListDelegateLeadingLoader"));
  ASSERT_NE(leading, nullptr);
  ASSERT_NE(leading_loader, nullptr);
  ASSERT_NE(centered_loader, nullptr);

  QCoreApplication::processEvents();
  EXPECT_EQ(list->property("leadingContentAlignment").toInt(), root->property("alignTop").toInt());
  EXPECT_EQ(leading->objectName(), QStringLiteral("listLeading"));
  EXPECT_DOUBLE_EQ(leading->property("width").toReal(), 28.0);
  EXPECT_DOUBLE_EQ(leading->property("height").toReal(), 28.0);
  EXPECT_DOUBLE_EQ(leading_loader->property("y").toReal(), 0.0);
  const qreal expected_centered_y =
      std::round((centered_list->property("contentItem").value<QObject*>()->property("height").toReal() - 28.0) / 2.0);
  EXPECT_DOUBLE_EQ(centered_loader->property("height").toReal(), 28.0);
  EXPECT_DOUBLE_EQ(centered_loader->property("y").toReal(), expected_centered_y);

  list->setProperty("leadingContentAlignment", root->property("alignVCenter"));
  EXPECT_EQ(list->property("leadingContentAlignment").toInt(), root->property("alignVCenter").toInt());
  EXPECT_EQ(leading_loader->property("effectiveAlignment").toInt(), root->property("alignVCenter").toInt());
}

TEST_F(QmlSmoke, Controls_SelectionGradientClipsNavigationAccent) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      visible: true
      width: 200
      height: 100
      property alias navigation: navigation
      property alias list: list
      HnNavigationDelegate {
        id: navigation
        width: 160
        height: 40
        checked: true
      }
      HnListDelegate {
        id: list
        y: 48
        width: 160
        height: 40
        checked: true
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(root.get());
  ASSERT_NE(window, nullptr);

  QObject* navigation = root->property("navigation").value<QObject*>();
  QObject* list = root->property("list").value<QObject*>();
  ASSERT_NE(navigation, nullptr);
  ASSERT_NE(list, nullptr);
  QObject* overlay = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSelectedOverlay"));
  QObject* gradient = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSelectionGradient"));
  QObject* accent_start = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateAccentStartStop"));
  QObject* accent_end = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateAccentEndStop"));
  QObject* surface_start = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSurfaceStartStop"));
  QObject* surface_end = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSurfaceEndStop"));
  ASSERT_NE(overlay, nullptr);
  ASSERT_NE(gradient, nullptr);
  ASSERT_NE(accent_start, nullptr);
  ASSERT_NE(accent_end, nullptr);
  ASSERT_NE(surface_start, nullptr);
  ASSERT_NE(surface_end, nullptr);

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  EXPECT_EQ(gradient->property("orientation").toInt(), Qt::Horizontal);
  EXPECT_EQ(accent_start->property("color").value<QColor>(), tok.selectionIndicator);
  EXPECT_EQ(accent_end->property("color").value<QColor>(), tok.selectionIndicator);
  const qreal expected_stop = (std::max)(2.0, tok.borderWidth * 2.0) / 160.0;
  EXPECT_DOUBLE_EQ(accent_end->property("position").toReal(), expected_stop);
  EXPECT_DOUBLE_EQ(surface_start->property("position").toReal(), expected_stop + 0.25 / 160.0);
  EXPECT_EQ(surface_start->property("color").value<QColor>(), tok.surfaceSelected);
  EXPECT_EQ(surface_end->property("color").value<QColor>(), tok.surfaceSelected);

  window->requestUpdate();
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  const QImage rendered = window->grabWindow();
  EXPECT_EQ(rendered.pixelColor(1, 20), tok.selectionIndicator);
  EXPECT_NE(rendered.pixelColor(8, 35), tok.selectionIndicator);
  EXPECT_EQ(rendered.pixelColor(8, 35), rendered.pixelColor(120, 35));

  QTest::mouseMove(window, QPoint{80, 20});
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(navigation->property("hovered").toBool());
  EXPECT_EQ(surface_start->property("color").value<QColor>(), tok.surfaceSelectedHover);
  EXPECT_EQ(surface_end->property("color").value<QColor>(), tok.surfaceSelectedHover);

  QObject* list_overlay = list->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSelectedOverlay"));
  ASSERT_NE(list_overlay, nullptr);
  EXPECT_EQ(list_overlay->property("gradient").value<QObject*>(), nullptr);
  EXPECT_EQ(list_overlay->property("color").value<QColor>(), tok.surfaceSelected);
}

TEST_F(QmlSmoke, Controls_ListDelegateSubtitlePresentationPolicies) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      id: root
      width: 360
      height: 300
      property alias wrapped: wrapped
      property alias singleLine: singleLine
      property int noWrap: Text.NoWrap
      property int wordWrap: Text.WordWrap
      property int elideRight: Text.ElideRight
      property string reportedAccessibleDescription: singleLine.Accessible.description
      property bool showTrailing: true
      HnListDelegate {
        id: wrapped
        width: 180
        title: "Wrapped"
        subtitle: "A long subtitle that needs more than one line at this width"
      }
      HnListDelegate {
        id: singleLine
        y: 100
        width: 180
        height: 64
        title: "Single line"
        subtitle: "alsa_output.pci-0000_00_1f.3.analog-stereo-with-a-long-identifier"
        subtitlePresentation: HnListDelegate.SingleLine
        trailingContent: Component { Item { visible: root.showTrailing; implicitWidth: 60 } }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QObject* wrapped = root->property("wrapped").value<QObject*>();
  QObject* single_line = root->property("singleLine").value<QObject*>();
  ASSERT_NE(wrapped, nullptr);
  ASSERT_NE(single_line, nullptr);
  QObject* wrapped_label = wrapped->findChild<QObject*>(QStringLiteral("hnListDelegateSubtitle"));
  QObject* single_line_label = single_line->findChild<QObject*>(QStringLiteral("hnListDelegateSubtitle"));
  ASSERT_NE(wrapped_label, nullptr);
  ASSERT_NE(single_line_label, nullptr);

  QCoreApplication::processEvents();
  EXPECT_EQ(wrapped_label->property("wrapMode").toInt(), root->property("wordWrap").toInt());
  EXPECT_GT(wrapped_label->property("lineCount").toInt(), 1);
  EXPECT_EQ(single_line_label->property("wrapMode").toInt(), root->property("noWrap").toInt());
  EXPECT_EQ(single_line_label->property("elide").toInt(), root->property("elideRight").toInt());
  EXPECT_EQ(single_line_label->property("maximumLineCount").toInt(), 1);
  EXPECT_EQ(single_line_label->property("lineCount").toInt(), 1);
  EXPECT_TRUE(single_line_label->property("truncated").toBool());

  root->setProperty("showTrailing", false);
  QCoreApplication::processEvents();
  EXPECT_EQ(single_line_label->property("lineCount").toInt(), 1);
  root->setProperty("showTrailing", true);
  single_line->setProperty("width", 140);
  QCoreApplication::processEvents();
  EXPECT_EQ(single_line_label->property("lineCount").toInt(), 1);
  EXPECT_EQ(root->property("reportedAccessibleDescription").toString(),
            QStringLiteral("alsa_output.pci-0000_00_1f.3.analog-stereo-with-a-long-identifier"));

  single_line->setProperty("subtitle", QString{});
  QCoreApplication::processEvents();
  EXPECT_FALSE(single_line_label->property("visible").toBool());
}

TEST_F(QmlSmoke, Controls_PressedStatesMatchSegmentedControlSurface) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight as Hn
    import Holonight.Controls
    Item {
      property alias choice: choice
      property alias card: card
      property alias navigation: navigation
      property alias itemDelegate: itemDelegate

      HnChoiceCard { id: choice; width: 160; height: 80 }
      HnCardDelegate { id: card; width: 160; height: 80 }
      HnNavigationDelegate { id: navigation; width: 160; height: 40 }
      Hn.ItemDelegate { id: itemDelegate; width: 160; height: 40 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  const auto verify_state = [&tok](QObject* control, const QString& base_name, const QString& selection_name,
                                   const QColor& idle_base_color) {
    ASSERT_NE(control, nullptr);
    QObject* base = control->findChild<QObject*>(base_name);
    QObject* selection = control->findChild<QObject*>(selection_name);
    ASSERT_NE(base, nullptr);
    ASSERT_NE(selection, nullptr);

    EXPECT_EQ(base->property("color").value<QColor>(), idle_base_color);
    EXPECT_FALSE(selection->property("visible").toBool());

    ASSERT_TRUE(control->setProperty("down", true));
    EXPECT_EQ(base->property("color").value<QColor>(), tok.surfaceElevated);

    ASSERT_TRUE(control->setProperty("checked", true));
    EXPECT_EQ(base->property("color").value<QColor>(), tok.surfaceElevated);
    EXPECT_EQ(selection->property("color").value<QColor>(), tok.surfaceSelected);
    EXPECT_FALSE(selection->property("visible").toBool());

    ASSERT_TRUE(control->setProperty("down", false));
    EXPECT_EQ(base->property("color").value<QColor>(), idle_base_color);
    EXPECT_TRUE(selection->property("visible").toBool());

    ASSERT_TRUE(control->setProperty("enabled", false));
    EXPECT_FALSE(control->property("down").toBool());
  };

  verify_state(root->property("choice").value<QObject*>(), QStringLiteral("hnChoiceCardBase"),
               QStringLiteral("hnChoiceCardSelectedOverlay"), tok.surfaceElevated);
  verify_state(root->property("card").value<QObject*>(), QStringLiteral("hnCardDelegateBase"),
               QStringLiteral("hnCardDelegateSelectedOverlay"), tok.surfaceElevated);
  verify_state(root->property("navigation").value<QObject*>(), QStringLiteral("hnSelectableDelegateBase"),
               QStringLiteral("hnSelectableDelegateSelectedOverlay"), QColor{Qt::transparent});
  verify_state(root->property("itemDelegate").value<QObject*>(), QStringLiteral("hnItemDelegateBase"),
               QStringLiteral("hnItemDelegateSelectedOverlay"), QColor{Qt::transparent});
}

TEST_F(QmlSmoke, Controls_ChoiceCardPressedRenderingDoesNotDependOnAncestorColor) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      visible: true
      width: 120
      height: 120
      color: "#ff0000"
      property alias ancestorColor: background.color
      property alias choice: choice
      Rectangle {
        id: background
        anchors.fill: parent
        color: "#ff0000"
        HnChoiceCard {
          id: choice
          anchors.centerIn: parent
          width: 80
          height: 80
          title: ""
          description: ""
          checked: true
        }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  QObject* choice = object->property("choice").value<QObject*>();
  ASSERT_NE(choice, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  QTest::mouseMove(window, QPoint{60, 60});
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(choice->property("hovered").toBool());
  QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, QPoint{60, 60});
  EXPECT_TRUE(choice->property("down").toBool());

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  const QColor expected = tok.surfaceElevated;
  const auto sample_center = [window]() {
    window->requestUpdate();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    return window->grabWindow().pixelColor(60, 60);
  };
  const auto expect_close = [](const QColor& actual, const QColor& expected_color) {
    EXPECT_LE(std::abs(actual.red() - expected_color.red()), 2);
    EXPECT_LE(std::abs(actual.green() - expected_color.green()), 2);
    EXPECT_LE(std::abs(actual.blue() - expected_color.blue()), 2);
  };

  const QColor over_red = sample_center();
  ASSERT_TRUE(object->setProperty("ancestorColor", QColor{QStringLiteral("#00ff00")}));
  const QColor over_green = sample_center();

  expect_close(over_red, expected);
  expect_close(over_green, expected);
  expect_close(over_red, over_green);
  QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, QPoint{60, 60});
  EXPECT_FALSE(choice->property("down").toBool());
}

TEST_F(QmlSmoke, Controls_SegmentedControlSynchronizesValueAndActivation) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnSegmentedControl {
      model: [{ text: "List", value: "list" }, { text: "Grid", value: "grid" }]
      property int activationCount: 0
      property var lastValue
      onActivated: (index, value) => { activationCount++; lastValue = value }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> control{comp.create()};
  ASSERT_NE(control, nullptr);
  auto* segment_row = qobject_cast<QQuickItem*>(control->property("contentItem").value<QObject*>());
  ASSERT_NE(segment_row, nullptr);
  QObject* first_segment = nullptr;
  QObject* last_segment = nullptr;
  for (QQuickItem* segment : segment_row->childItems()) {
    const QVariant index = segment->property("index");
    if (!index.isValid()) {
      continue;
    }
    if (index.toInt() == 0) {
      first_segment = segment;
    } else if (index.toInt() == 1) {
      last_segment = segment;
    }
  }
  ASSERT_NE(first_segment, nullptr);
  ASSERT_NE(last_segment, nullptr);
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  EXPECT_GE(control->property("implicitHeight").toReal(), tok.controlHeight);
  EXPECT_GE(control->property("implicitWidth").toReal(), 160.0);
  EXPECT_GT(first_segment->property("width").toReal(), 0.0);
  EXPECT_GT(first_segment->property("height").toReal(), 0.0);
  EXPECT_EQ(control->property("currentIndex").toInt(), 0);
  EXPECT_EQ(control->property("currentValue").toString(), QStringLiteral("list"));
  EXPECT_TRUE(first_segment->property("firstSegment").toBool());
  EXPECT_FALSE(first_segment->property("lastSegment").toBool());
  EXPECT_FALSE(last_segment->property("firstSegment").toBool());
  EXPECT_TRUE(last_segment->property("lastSegment").toBool());
  EXPECT_EQ(first_segment->property("segmentColor").value<QColor>(), tok.surfaceHover);
  EXPECT_EQ(first_segment->property("segmentColor").value<QColor>().alpha(), 255);
  EXPECT_EQ(last_segment->property("segmentColor").value<QColor>(), tok.surface);
  ASSERT_TRUE(QMetaObject::invokeMethod(control.get(), "activate", Q_ARG(int, 1)));
  EXPECT_EQ(control->property("currentIndex").toInt(), 1);
  EXPECT_EQ(control->property("currentValue").toString(), QStringLiteral("grid"));
  EXPECT_EQ(control->property("activationCount").toInt(), 1);
  EXPECT_EQ(control->property("lastValue").toString(), QStringLiteral("grid"));
  EXPECT_EQ(first_segment->property("segmentColor").value<QColor>(), tok.surface);
  EXPECT_EQ(last_segment->property("segmentColor").value<QColor>(), tok.surfaceHover);
  ASSERT_TRUE(last_segment->setProperty("down", true));
  EXPECT_EQ(last_segment->property("segmentColor").value<QColor>(), tok.surfaceElevated);
  ASSERT_TRUE(last_segment->setProperty("down", false));
}

TEST_F(QmlSmoke, Controls_ChoiceCardsUseStandardButtonGroupExclusivity) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls.Basic
    import Holonight.Controls
    Item {
      property alias first: first
      property alias second: second
      ButtonGroup { id: choices }
      HnChoiceCard { id: first; title: "Dark"; checked: true; ButtonGroup.group: choices }
      HnChoiceCard { id: second; title: "Light"; ButtonGroup.group: choices }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QObject* first = root->property("first").value<QObject*>();
  QObject* second = root->property("second").value<QObject*>();
  second->setProperty("checked", true);
  EXPECT_FALSE(first->property("checked").toBool());
  EXPECT_TRUE(second->property("checked").toBool());
}

TEST_F(QmlSmoke, HnSeparator_ExposesDefaultsAndNormalizesSizing) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias separator: separator
      property bool ignored: separator.Accessible.ignored
      property int solid: HnSeparator.Solid
      property int fadeEnd: HnSeparator.FadeEnd
      HnSeparator { id: separator; width: 120 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* separator = root->property("separator").value<QObject*>();
  ASSERT_NE(separator, nullptr);

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  EXPECT_EQ(separator->property("orientation").toInt(), Qt::Horizontal);
  EXPECT_EQ(separator->property("fadeMode").toInt(), root->property("solid").toInt());
  EXPECT_EQ(root->property("fadeEnd").toInt(), 3);
  EXPECT_EQ(separator->property("color").value<QColor>(), tok.borderSubtle);
  EXPECT_DOUBLE_EQ(separator->property("thickness").toDouble(), tok.separatorWidth);
  EXPECT_DOUBLE_EQ(separator->property("implicitWidth").toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(separator->property("implicitHeight").toDouble(), tok.separatorWidth);
  EXPECT_TRUE(root->property("ignored").toBool());

  ASSERT_TRUE(separator->setProperty("orientation", Qt::Vertical));
  EXPECT_DOUBLE_EQ(separator->property("implicitWidth").toDouble(), tok.separatorWidth);
  EXPECT_DOUBLE_EQ(separator->property("implicitHeight").toDouble(), 0.0);
  ASSERT_TRUE(separator->setProperty("orientation", 999));
  EXPECT_EQ(separator->property("effectiveOrientation").toInt(), Qt::Horizontal);
  ASSERT_TRUE(separator->setProperty("thickness", -4.0));
  EXPECT_DOUBLE_EQ(separator->property("implicitWidth").toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(separator->property("implicitHeight").toDouble(), 0.0);
}

TEST_F(QmlSmoke, HnSeparator_ResolvesFadeProfilesAndClampsOpacity) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnSeparator {
      width: 120
      color: Qt.rgba(0.2, 0.4, 0.6, 0.5)
      centerOpacity: 2
      edgeOpacity: -1
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> separator{comp.create()};
  ASSERT_NE(separator, nullptr);
  auto* start = separator->findChild<QObject*>(QStringLiteral("separatorStartStop"));
  auto* middle = separator->findChild<QObject*>(QStringLiteral("separatorMiddleStop"));
  auto* end = separator->findChild<QObject*>(QStringLiteral("separatorEndStop"));
  ASSERT_NE(start, nullptr);
  ASSERT_NE(middle, nullptr);
  ASSERT_NE(end, nullptr);

  EXPECT_DOUBLE_EQ(separator->property("effectiveCenterOpacity").toDouble(), 1.0);
  EXPECT_DOUBLE_EQ(separator->property("effectiveEdgeOpacity").toDouble(), 0.0);
  EXPECT_NEAR(start->property("color").value<QColor>().alphaF(), 0.5, 0.0001);
  EXPECT_NEAR(end->property("color").value<QColor>().alphaF(), 0.5, 0.0001);

  ASSERT_TRUE(separator->setProperty("fadeMode", 1));
  EXPECT_EQ(start->property("color").value<QColor>().alphaF(), 0.0);
  EXPECT_NEAR(middle->property("color").value<QColor>().alphaF(), 0.5, 0.0001);
  EXPECT_EQ(end->property("color").value<QColor>().alphaF(), 0.0);

  ASSERT_TRUE(separator->setProperty("fadeMode", 2));
  EXPECT_EQ(start->property("color").value<QColor>().alphaF(), 0.0);
  EXPECT_NEAR(middle->property("color").value<QColor>().alphaF(), 0.25, 0.01);
  EXPECT_NEAR(end->property("color").value<QColor>().alphaF(), 0.5, 0.0001);

  ASSERT_TRUE(separator->setProperty("fadeMode", 3));
  EXPECT_NEAR(start->property("color").value<QColor>().alphaF(), 0.5, 0.0001);
  EXPECT_EQ(end->property("color").value<QColor>().alphaF(), 0.0);
  ASSERT_TRUE(separator->setProperty("fadeMode", 999));
  EXPECT_EQ(separator->property("effectiveFadeMode").toInt(), 0);
}

TEST_F(QmlSmoke, HnSeparator_TracksScenePositionAndPaintedGeometry) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      id: ancestor
      y: 0.2
      property alias separator: separator
      HnSeparator { id: separator; y: 0.3; width: 100 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* separator = qobject_cast<QQuickItem*>(root->property("separator").value<QObject*>());
  ASSERT_NE(separator, nullptr);
  auto* geometry = separator->findChild<QObject*>(QStringLiteral("separatorGeometry"));
  auto* line = separator->findChild<QQuickItem*>(QStringLiteral("separatorLine"));
  ASSERT_NE(geometry, nullptr);
  ASSERT_NE(line, nullptr);

  const qreal dpr = geometry->property("effectiveDevicePixelRatio").toReal();
  const qreal scene_y = separator->mapToScene(QPointF{}).y();
  EXPECT_NEAR(std::round((scene_y + line->y()) * dpr), (scene_y + line->y()) * dpr, 0.000001);
  EXPECT_NEAR(line->height() * dpr, 1.0, 0.000001);

  ASSERT_TRUE(root->setProperty("y", 0.6));
  const qreal moved_scene_y = separator->mapToScene(QPointF{}).y();
  EXPECT_NEAR(std::round((moved_scene_y + line->y()) * dpr), (moved_scene_y + line->y()) * dpr, 0.000001);

  ASSERT_TRUE(separator->setProperty("orientation", Qt::Vertical));
  EXPECT_FALSE(line->isVisible());
  ASSERT_TRUE(separator->setProperty("height", 80.0));
  EXPECT_TRUE(line->isVisible());
  EXPECT_NEAR(line->width() * dpr, 1.0, 0.000001);
  EXPECT_DOUBLE_EQ(line->height(), 80.0);
  ASSERT_TRUE(separator->setProperty("thickness", 0.0));
  EXPECT_FALSE(line->isVisible());
}

TEST_F(QmlSmoke, HnSeparator_AlignmentIsStableAcrossFractionalDprAndOffsets) {
  constexpr std::array<qreal, 5> kDprs = {1.0, 1.25, 1.5, 1.75, 2.0};
  for (const qreal dpr : kDprs) {
    expectSeparatorAlignment(dpr);
  }

  EXPECT_DOUBLE_EQ(Holonight::separatorAlignment(0.0, 1.0, 1.5, 0.2).painted_thickness, 0.0);
  EXPECT_DOUBLE_EQ(Holonight::separatorAlignment(-1.0, 1.0, 1.5, 0.2).painted_thickness, 0.0);
  EXPECT_NEAR(Holonight::separatorAlignment(2.0, 1.0, 1.75, 0.2).painted_thickness * 1.75, 2.0, 0.000001);
}

TEST_F(QmlSmoke, Controls_ColorPickerDefaultColorsMatchPalette) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight.Controls
    HnColorPicker {}
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults());
  ASSERT_EQ(tok.warning, tok.accentYellow) << "warning is expected to alias accentYellow in every scheme; "
                                              "if this changes, restore warning as a distinct default swatch";
  const QVariantList colors = root->property("colors").toList();
  ASSERT_EQ(colors.size(), 6);
  EXPECT_EQ(colors.at(0).value<QColor>(), tok.accentCyan);
  EXPECT_EQ(colors.at(1).value<QColor>(), tok.accentBlue);
  EXPECT_EQ(colors.at(2).value<QColor>(), tok.accentViolet);
  EXPECT_EQ(colors.at(3).value<QColor>(), tok.accentYellow);
  EXPECT_EQ(colors.at(4).value<QColor>(), tok.success);
  EXPECT_EQ(colors.at(5).value<QColor>(), tok.error);

  EXPECT_EQ(colorPickerSwatches(root.get()).size(), 6);
}

TEST_F(QmlSmoke, Controls_ColorPickerEmptyColorsRendersNoSwatches) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight.Controls
    HnColorPicker { colors: [] }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  EXPECT_EQ(root->property("colors").toList().size(), 0);
  EXPECT_TRUE(colorPickerSwatches(root.get()).isEmpty());
}

TEST_F(QmlSmoke, Controls_ColorPickerSelectionMatchesByColorEqualityAcrossDuplicates) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight.Controls
    HnColorPicker {
      colors: ["#FF0000", "#00FF00", "#FF0000"]
      selectedColor: "#FF0000"
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  const auto swatches = colorPickerSwatches(root.get());
  ASSERT_EQ(swatches.size(), 3);
  QQuickItem* first = colorPickerSwatchAt(swatches, 0);
  QQuickItem* second = colorPickerSwatchAt(swatches, 1);
  QQuickItem* third = colorPickerSwatchAt(swatches, 2);
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  ASSERT_NE(third, nullptr);
  EXPECT_TRUE(first->property("selected").toBool());
  EXPECT_FALSE(second->property("selected").toBool());
  EXPECT_TRUE(third->property("selected").toBool());
}

TEST_F(QmlSmoke, Controls_ColorPickerClickUpdatesSelectionAndEmitsSignal) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnColorPicker {
      colors: ["#FF0000", "#00FF00", "#0000FF"]
      property int selectionCount: 0
      property color lastSelected
      onColorSelected: (color) => { selectionCount++; lastSelected = color }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  const auto swatches = colorPickerSwatches(root.get());
  QQuickItem* target = colorPickerSwatchAt(swatches, 1);
  ASSERT_NE(target, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(target, "clicked", Qt::DirectConnection));

  const QColor expected = QColor{QStringLiteral("#00FF00")};
  EXPECT_EQ(root->property("selectedColor").value<QColor>(), expected);
  EXPECT_EQ(root->property("selectionCount").toInt(), 1);
  EXPECT_EQ(root->property("lastSelected").value<QColor>(), expected);
}

TEST_F(QmlSmoke, Controls_ColorPickerControlledModePreservesExternalSelectionBinding) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property color externalColor: "#FF0000"
      property alias picker: picker
      property int selectionCount: 0
      property color lastSelected
      HnColorPicker {
        id: picker
        colors: ["#FF0000", "#00FF00", "#0000FF"]
        selectedColor: parent.externalColor
        autoUpdateSelectedColor: false
        onColorSelected: (color) => { parent.selectionCount++; parent.lastSelected = color }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* picker = root->property("picker").value<QObject*>();
  ASSERT_NE(picker, nullptr);

  const auto swatches = colorPickerSwatches(picker);
  QQuickItem* target = colorPickerSwatchAt(swatches, 1);
  ASSERT_NE(target, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(target, "clicked", Qt::DirectConnection));

  EXPECT_EQ(picker->property("selectedColor").value<QColor>(), QColor{QStringLiteral("#FF0000")});
  EXPECT_EQ(root->property("selectionCount").toInt(), 1);
  EXPECT_EQ(root->property("lastSelected").value<QColor>(), QColor{QStringLiteral("#00FF00")});

  ASSERT_TRUE(root->setProperty("externalColor", QColor{QStringLiteral("#0000FF")}));
  EXPECT_EQ(picker->property("selectedColor").value<QColor>(), QColor{QStringLiteral("#0000FF")});
}

TEST_F(QmlSmoke, Controls_ColorPickerSizeRolesAndInvalidFallback) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias hero: hero
      property alias invalid: invalid
      property int normalRole: HnControlSize.Normal
      HnColorPicker { id: compact; sizeRole: HnControlSize.Compact }
      HnColorPicker { id: normal; sizeRole: HnControlSize.Normal }
      HnColorPicker { id: large; sizeRole: HnControlSize.Large }
      HnColorPicker { id: hero; sizeRole: HnControlSize.Hero }
      HnColorPicker { id: invalid; sizeRole: 999 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  auto* compact = root->property("compact").value<QObject*>();
  auto* normal = root->property("normal").value<QObject*>();
  auto* large = root->property("large").value<QObject*>();
  auto* hero = root->property("hero").value<QObject*>();
  auto* invalid = root->property("invalid").value<QObject*>();
  ASSERT_NE(compact, nullptr);
  ASSERT_NE(normal, nullptr);
  ASSERT_NE(large, nullptr);
  ASSERT_NE(hero, nullptr);
  ASSERT_NE(invalid, nullptr);

  const qreal ringExtra = compact->property("ringExtra").toReal();
  EXPECT_DOUBLE_EQ(compact->property("swatchDiameter").toReal(), 28.0);
  EXPECT_DOUBLE_EQ(compact->property("implicitHeight").toReal(), 28.0 + ringExtra);
  EXPECT_DOUBLE_EQ(normal->property("swatchDiameter").toReal(), 32.0);
  EXPECT_DOUBLE_EQ(normal->property("implicitHeight").toReal(), 32.0 + ringExtra);
  EXPECT_DOUBLE_EQ(large->property("swatchDiameter").toReal(), 40.0);
  EXPECT_DOUBLE_EQ(large->property("implicitHeight").toReal(), 40.0 + ringExtra);
  EXPECT_DOUBLE_EQ(hero->property("swatchDiameter").toReal(), 52.0);
  EXPECT_DOUBLE_EQ(hero->property("implicitHeight").toReal(), 52.0 + ringExtra);

  EXPECT_EQ(invalid->property("resolvedSizeRole").toInt(), root->property("normalRole").toInt());
  EXPECT_DOUBLE_EQ(invalid->property("swatchDiameter").toReal(), 32.0);
}

TEST_F(QmlSmoke, Controls_ColorPickerDisabledStateBlocksSelection) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      visible: true
      width: 200
      height: 100
      property alias picker: picker
      property int selectionCount: 0
      HnColorPicker {
        id: picker
        anchors.centerIn: parent
        enabled: false
        colors: ["#FF0000", "#00FF00"]
        onColorSelected: selectionCount++
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  auto* picker = qobject_cast<QQuickItem*>(object->property("picker").value<QObject*>());
  ASSERT_NE(picker, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  const auto swatches = colorPickerSwatches(picker);
  ASSERT_EQ(swatches.size(), 2);
  expectAllSwatchesDisabled(swatches);

  const QPoint click_point = picker->mapToScene(QPointF{picker->width() / 2, picker->height() / 2}).toPoint();
  QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, click_point);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  EXPECT_FALSE(picker->property("selectedColor").value<QColor>().isValid());
  EXPECT_EQ(object->property("selectionCount").toInt(), 0);
}

TEST_F(QmlSmoke, Controls_ColorPickerKeyboardNavigationAndActivation) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      visible: true
      width: 240
      height: 100
      property alias picker: picker
      property int selectionCount: 0
      property var lastSelected
      HnColorPicker {
        id: picker
        anchors.centerIn: parent
        colors: ["#FF0000", "#00FF00", "#0000FF"]
        onColorSelected: (color) => { selectionCount++; lastSelected = color }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  auto* picker = qobject_cast<QQuickItem*>(object->property("picker").value<QObject*>());
  ASSERT_NE(picker, nullptr);

  picker->forceActiveFocus(Qt::TabFocusReason);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_EQ(picker->property("focusedIndex").toInt(), 0);

  QTest::keyClick(window, Qt::Key_Left);
  EXPECT_EQ(picker->property("focusedIndex").toInt(), 0);

  QTest::keyClick(window, Qt::Key_Right);
  EXPECT_EQ(picker->property("focusedIndex").toInt(), 1);
  const auto swatches = colorPickerSwatches(picker);
  QQuickItem* focused_swatch = colorPickerSwatchAt(swatches, 1);
  ASSERT_NE(focused_swatch, nullptr);
  EXPECT_TRUE(focused_swatch->property("keyboardFocused").toBool());

  QTest::keyClick(window, Qt::Key_Right);
  EXPECT_EQ(picker->property("focusedIndex").toInt(), 2);
  QTest::keyClick(window, Qt::Key_Right);
  EXPECT_EQ(picker->property("focusedIndex").toInt(), 2);
  EXPECT_FALSE(picker->property("selectedColor").value<QColor>().isValid());
  EXPECT_EQ(object->property("selectionCount").toInt(), 0);

  QTest::keyClick(window, Qt::Key_Space);
  EXPECT_EQ(picker->property("selectedColor").value<QColor>(), QColor{QStringLiteral("#0000FF")});
  EXPECT_EQ(object->property("selectionCount").toInt(), 1);
  EXPECT_EQ(object->property("lastSelected").value<QColor>(), QColor{QStringLiteral("#0000FF")});

  QTest::keyClick(window, Qt::Key_Left);
  EXPECT_EQ(picker->property("focusedIndex").toInt(), 1);
  QTest::keyClick(window, Qt::Key_Return);
  EXPECT_EQ(picker->property("selectedColor").value<QColor>(), QColor{QStringLiteral("#00FF00")});
  EXPECT_EQ(object->property("selectionCount").toInt(), 2);
}

TEST_F(QmlSmoke, Controls_ColorPickerIntegratesStandaloneAndInSettingsRow) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias standalone: standalone
      property alias row: row

      HnColorPicker { id: standalone }
      HnSettingsRow {
        id: row
        titleText: "Accent color"
        descriptionText: "Used for highlights and interactive elements"
        control: Component { HnColorPicker {} }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  auto* standalone = root->property("standalone").value<QObject*>();
  auto* row = root->property("row").value<QObject*>();
  ASSERT_NE(standalone, nullptr);
  ASSERT_NE(row, nullptr);

  EXPECT_EQ(colorPickerSwatches(standalone).size(), 6);

  auto* control_item = row->property("controlItem").value<QObject*>();
  ASSERT_NE(control_item, nullptr);
  EXPECT_EQ(colorPickerSwatches(control_item).size(), 6);
}
