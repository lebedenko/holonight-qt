// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnseparatoralignment.h"
#include "holonight/metrics.h"
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
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QString>
#include <QTemporaryDir>
#include <QTest>
#include <QUrlQuery>
#include <QVariant>

#include <array>
#include <cmath>
#include <filesystem>
#include <gtest/gtest.h>
#include <holonight/config/config.h>
#include <limits>
#include <memory>
#include <vector>

namespace {

void writeAppearance(const QString& path, const char* scheme = "holonight-dark", const char* accent = "blue",
                     const char* ui_family = "Inter", int ui_size = 12,
                     HoloNight::Config::ShapeStyle shape_style = HoloNight::Config::ShapeStyle::Inherit,
                     double shape_scale = 1.0) {
  HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  appearance.theme.scheme = scheme;
  appearance.theme.accent = accent;
  appearance.typography.ui_family = ui_family;
  appearance.typography.ui_size = ui_size;
  appearance.shape.style = shape_style;
  appearance.shape.scale = shape_scale;
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, std::filesystem::path{path.toStdString()}));
}

Holonight::ColorTokens canonicalDefaultTokens() {
  return Holonight::ThemeResolver::resolve(*Holonight::resolveAppearance(HoloNight::Config::defaults()).value);
}

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
  void SetUp() override {
    engine_.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
  }

  // GTest-derived tests intentionally share protected fixture state.
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
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
        font.pixelSize: 12
        text: "Text"
      }
      Button {
        id: textAndIconButton
        font.pixelSize: 12
        text: "Text and icon"
        icon.source: "missing-test-icon.svg"
      }
      Button {
        id: iconOnlyButton
        font.pixelSize: 12
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

  auto* text_button = root->property("textButton").value<QObject*>();
  auto* text_and_icon_button = root->property("textAndIconButton").value<QObject*>();
  auto* icon_only_button = root->property("iconOnlyButton").value<QObject*>();
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

  auto* defaultBtn = root->property("defaultBtn").value<QObject*>();
  auto* defaultPressedBtn = root->property("defaultPressedBtn").value<QObject*>();

  auto* primaryBtn = root->property("primaryBtn").value<QObject*>();
  auto* primaryPressedBtn = root->property("primaryPressedBtn").value<QObject*>();

  ASSERT_NE(defaultBtn, nullptr);
  ASSERT_NE(primaryBtn, nullptr);

  EXPECT_TRUE(defaultBtn->property("hoverEnabled").toBool());
  EXPECT_TRUE(primaryBtn->property("hoverEnabled").toBool());

  const Holonight::ColorTokens tok = canonicalDefaultTokens();

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
      property int xs: HnControlSize.Xs
      property var heights: [HnMetrics.controlHeight(compact),
                             HnMetrics.controlHeight(normal),
                             HnMetrics.controlHeight(large),
                             HnMetrics.controlHeight(hero), HnMetrics.controlHeight(xs)]
      property var paddings: [HnMetrics.horizontalPadding(compact),
                              HnMetrics.horizontalPadding(normal),
                              HnMetrics.horizontalPadding(large),
                              HnMetrics.horizontalPadding(hero), HnMetrics.horizontalPadding(xs)]
      property var icons: [HnMetrics.iconSize(compact),
                           HnMetrics.iconSize(normal),
                           HnMetrics.iconSize(large),
                           HnMetrics.iconSize(hero), HnMetrics.iconSize(xs)]
      property var spacings: [HnMetrics.internalSpacing(compact),
                              HnMetrics.internalSpacing(normal),
                              HnMetrics.internalSpacing(large),
                              HnMetrics.internalSpacing(hero), HnMetrics.internalSpacing(xs)]
      property var headerHeights: [HnMetrics.headerHeightForSize(xs), HnMetrics.headerHeightForSize(compact),
                                   HnMetrics.headerHeightForSize(normal), HnMetrics.headerHeightForSize(large),
                                   HnMetrics.headerHeightForSize(hero)]
      property int invalidRole: HnMetrics.normalizedSizeRole(999)
      property int invalidHeight: HnMetrics.controlHeight(-1)
      property int headerHeight: HnMetrics.headerHeight
      property int appTitleIconSize: HnMetrics.appTitleIconSize
      property int appTitleIconSpacing: HnMetrics.appTitleIconSpacing
      property int appTitleTextSpacing: HnMetrics.appTitleTextSpacing
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
  EXPECT_EQ(object->property("xs").toInt(), 4);
  EXPECT_EQ(object->property("heights").toList(), QVariantList({28, 32, 40, 52, 24}));
  EXPECT_EQ(object->property("paddings").toList(), QVariantList({8, 8, 12, 16, 6}));
  EXPECT_EQ(object->property("icons").toList(), QVariantList({16, 16, 20, 24, 16}));
  EXPECT_EQ(object->property("spacings").toList(), QVariantList({4, 6, 8, 10, 4}));
  EXPECT_EQ(object->property("headerHeights").toList(), QVariantList({42, 48, 56, 64, 72}));
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
    import QtQuick.Layouts
    import Holonight.Controls
    Item {
      property alias bar: bar
      property real headerMinimum: bar.Layout.minimumHeight
      property real headerPreferred: bar.Layout.preferredHeight
      property real headerMaximum: bar.Layout.maximumHeight
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
  auto* bar = object->property("bar").value<QObject*>();
  ASSERT_NE(bar, nullptr);
  QQuickItem* content_item = qobject_cast<QQuickItem*>(bar->property("contentItem").value<QObject*>());
  auto* content_loader = bar->findChild<QQuickItem*>(QStringLiteral("headerContentLoader"));
  auto* divider = bar->findChild<QQuickItem*>(QStringLiteral("headerDivider"));
  ASSERT_NE(content_item, nullptr);
  ASSERT_NE(content_loader, nullptr);
  ASSERT_NE(divider, nullptr);
  EXPECT_EQ(bar->property("implicitHeight").toInt(), 56);
  EXPECT_EQ(bar->property("height").toInt(), 56);
  for (const auto [role, expected] : {std::pair{4, 42}, {0, 48}, {1, 56}, {2, 64}, {3, 72}}) {
    bar->setProperty("sizeRole", role);
    QCoreApplication::processEvents();
    EXPECT_EQ(bar->property("implicitHeight").toInt(), expected);
    EXPECT_EQ(bar->property("height").toInt(), expected);
    EXPECT_EQ(object->property("headerMinimum").toInt(), expected);
    EXPECT_EQ(object->property("headerPreferred").toInt(), expected);
    EXPECT_EQ(object->property("headerMaximum").toInt(), expected);
  }
  bar->setProperty("sizeRole", 1);
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
      property bool themeValid: HolonightTheme.uiFontSize > 0
      property bool appearanceValid: HnAppearance.resolve(HnSurfaceRole.Card, HnCornerStyle.Inherit,
                                                          80, 40, NaN, NaN, HnCornerMask.Inherit,
                                                          HnAppearance.revision).radius >= 0
      property bool profileValid: HnShapeProfile.resolve(HnSurfaceRole.Card, HnCornerStyle.Rounded,
                                                        80, 40, NaN, NaN).kind === HnShapeKind.Rounded
      property bool providerValid: HnIconProvider.sourceUrl("image://unknown/icon", 24, "white", "white", "white", "white", "white") === ""
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
  auto* icon = object->property("icon").value<QObject*>();
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

  auto* body = root->property("bodyLabel").value<QObject*>();
  auto* micro = root->property("microLabel").value<QObject*>();
  auto* display = root->property("displayLabel").value<QObject*>();
  auto* code = root->property("codeLabel").value<QObject*>();
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
  auto* frame = object->property("frame").value<QObject*>();
  auto* window = object->property("windowObject").value<QObject*>();
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
  auto* bar = object->property("bar").value<QObject*>();
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
    if ((mask & 1) != 0) {
      EXPECT_NEAR(coordinate("leadingX"), 0.0, 0.01);
    }
    if ((mask & 2) != 0) {
      EXPECT_NEAR(coordinate("centerMidpoint"), bar->property("width").toDouble() / 2, 0.01);
    }
    if ((mask & 4) != 0) {
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
  auto* defaultIcon =
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
  auto* listCombo = root->property("listCombo").value<QObject*>();
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
  auto* footer = area->findChild<QObject*>(QStringLiteral("hnTextAreaFooter"));
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

  auto* control = field->property("controlItem").value<QObject*>();
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

  auto* leadingLoader = field->findChild<QObject*>(QStringLiteral("hnSearchCustomLeading"));
  auto* trailingLoader = field->findChild<QObject*>(QStringLiteral("hnSearchCustomTrailing"));
  auto* defaultIcon = field->findChild<QObject*>(QStringLiteral("hnSearchDefaultIcon"));
  auto* clearButton = field->findChild<QObject*>(QStringLiteral("hnSearchClearButton"));
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
  auto* field = window->property("field").value<QObject*>();
  auto* clearButton = field->findChild<QObject*>(QStringLiteral("hnSearchClearButton"));
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
  auto* field = object->property("field").value<QObject*>();
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

  auto* vBar = root->property("vBar").value<QObject*>();
  auto* hBar = root->property("hBar").value<QObject*>();
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

  auto* normalDelegate = root->property("normalDelegate").value<QObject*>();
  auto* highlightedDelegate = root->property("highlightedDelegate").value<QObject*>();
  auto* pressedDelegate = root->property("pressedDelegate").value<QObject*>();

  ASSERT_NE(normalDelegate, nullptr);
  ASSERT_NE(highlightedDelegate, nullptr);
  ASSERT_NE(pressedDelegate, nullptr);

  const Holonight::ColorTokens tok = canonicalDefaultTokens();

  // Highlighted delegates use a quiet surface while retaining primary text.
  auto* highlightedBase = highlightedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateBase"));
  auto* highlightedSelection =
      highlightedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateSelectedOverlay"));
  ASSERT_NE(highlightedBase, nullptr);
  ASSERT_NE(highlightedSelection, nullptr);
  EXPECT_EQ(highlightedBase->property("color").value<QColor>(), QColor{Qt::transparent});
  EXPECT_EQ(highlightedSelection->property("color").value<QColor>(), tok.surfaceSelected);
  EXPECT_TRUE(highlightedSelection->property("visible").toBool());
  auto* label = highlightedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateLabel"));
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->property("color").value<QColor>(), tok.textPrimary);

  // Pressed item delegates use the same opaque resolved surface as segmented controls.
  auto* pressedBase = pressedDelegate->findChild<QObject*>(QStringLiteral("hnItemDelegateBase"));
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

TEST_F(QmlSmoke, CheckBoxKeyboardFocusFollowsIndicatorRadius) {
  QQmlComponent component{&engine_};
  component.setData(R"(
    import QtQuick
    import Holonight as H
    Window {
      visible: true
      width: 200; height: 80
      H.CheckBox { objectName: "checkbox"; x: 20; y: 20; text: "Focused" }
    }
  )",
                    QUrl{});
  ASSERT_EQ(component.status(), QQmlComponent::Ready) << component.errorString().toStdString();
  std::unique_ptr<QObject> root{component.create()};
  ASSERT_NE(root, nullptr);
  auto* checkbox = root->findChild<QQuickItem*>("checkbox");
  ASSERT_NE(checkbox, nullptr);
  checkbox->forceActiveFocus(Qt::TabFocusReason);
  QCoreApplication::processEvents();
  auto* indicator = checkbox->property("indicator").value<QQuickItem*>();
  ASSERT_NE(indicator, nullptr);
  EXPECT_DOUBLE_EQ(indicator->property("radius").toDouble(), 3.0);
  QQuickItem* ring = nullptr;
  for (auto* child : indicator->childItems()) {
    if (child->width() == indicator->width() + 4) ring = child;
  }
  ASSERT_NE(ring, nullptr);
  EXPECT_DOUBLE_EQ(ring->property("radius").toDouble(), 5.0);
  EXPECT_TRUE(ring->isVisible());
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

class ComboBoxGeometry : public QmlSmoke, public testing::WithParamInterface<QString> {};
INSTANTIATE_TEST_SUITE_P(Variants, ComboBoxGeometry,
                         testing::Values(QStringLiteral("H.ComboBox"), QStringLiteral("HnIconComboBox")));

TEST_P(ComboBoxGeometry, ScaledPopupMatchesControlSceneGeometry) {
  for (const qreal scale : {0.78, 1.0, 1.25}) {
    for (const bool rightEdge : {false, true}) {
      for (const bool opensAbove : {false, true}) {
        const qreal wrapper_x = rightEdge ? 480 - ((19 + 160) * scale) - 11 : 11 - (19 * scale);
        const qreal wrapper_y = opensAbove ? 335 - (23 * scale) : 41;
        QQmlComponent comp = QQmlComponent{&engine_};
        comp.setData(QString{R"(
          import QtQuick
          import QtQuick.Controls
          import Holonight as H
          import Holonight.Controls
          Window {
            width: 480
            height: 400
            visible: true

            property var combo: combo
            property bool popupUsesOverlay: combo.popup.parent === Overlay.overlay

            function moveAndReopen() {
                combo.popup.close()
                combo.parent.x -= 2
                combo.parent.y += 3
                combo.popup.open()
            }
            function controlTopLeft() { return combo.mapToItem(contentItem, 0, 0) }
            function controlBottomRight() { return combo.mapToItem(contentItem, combo.width, combo.height) }
            function popupTopLeft() { return combo.popup.background.mapToItem(contentItem, 0, 0) }
            function popupBottomRight() {
              return combo.popup.background.mapToItem(contentItem,
                                                       combo.popup.background.width,
                                                       combo.popup.background.height)
            }

            Item {
              x: %WRAPPER_X%
              y: %WRAPPER_Y%
              scale: %SCALE%
              transformOrigin: Item.TopLeft

              %TYPE% {
                id: combo
                x: 19
                y: 23
                width: 160
                model: ["one", "two", "three"]
                Component.onCompleted: popup.open()
              }
            }
          }
        )"}
                         .replace(QStringLiteral("%TYPE%"), GetParam())
                         .replace(QStringLiteral("%SCALE%"), QString::number(scale, 'f', 2))
                         .replace(QStringLiteral("%WRAPPER_X%"), QString::number(wrapper_x, 'f', 3))
                         .replace(QStringLiteral("%WRAPPER_Y%"), QString::number(wrapper_y, 'f', 3))
                         .toUtf8(),
                     QUrl{});
        ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
        std::unique_ptr<QObject> window{comp.create()};
        ASSERT_NE(window, nullptr);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

        auto* combo = window->property("combo").value<QObject*>();
        ASSERT_NE(combo, nullptr);
        ASSERT_TRUE(combo->property("popup").value<QObject*>()->property("visible").toBool());
        EXPECT_TRUE(combo->property("popupTransformSupported").toBool());
        EXPECT_NEAR(combo->property("effectiveScale").toReal(), scale, 0.001);
        EXPECT_TRUE(window->property("popupUsesOverlay").toBool());

        const auto mappedPoint = [&](const char* method) {
          QVariant result;
          EXPECT_TRUE(
              QMetaObject::invokeMethod(window.get(), method, Qt::DirectConnection, Q_RETURN_ARG(QVariant, result)));
          return result.toPointF();
        };
        const QPointF control_top_left = mappedPoint("controlTopLeft");
        const QPointF control_bottom_right = mappedPoint("controlBottomRight");
        const QPointF popup_top_left = mappedPoint("popupTopLeft");
        const QPointF popup_bottom_right = mappedPoint("popupBottomRight");
        EXPECT_NEAR(popup_top_left.x(), control_top_left.x(), 0.5) << "scale " << scale;
        EXPECT_NEAR(popup_bottom_right.x(), control_bottom_right.x(), 0.5) << "scale " << scale;
        EXPECT_GT(popup_top_left.x(), 5.0) << "scale " << scale;
        EXPECT_LT(popup_bottom_right.x(), 475.0) << "scale " << scale;

        if (opensAbove) {
          EXPECT_TRUE(combo->property("popup").value<QObject*>()->property("opensAbove").toBool());
          EXPECT_NEAR(popup_bottom_right.y(), control_top_left.y() - (2 * scale), 0.5) << "scale " << scale;
        } else {
          EXPECT_FALSE(combo->property("popup").value<QObject*>()->property("opensAbove").toBool());
          EXPECT_NEAR(popup_top_left.y(), control_bottom_right.y() + (2 * scale), 0.5) << "scale " << scale;
        }
        ASSERT_TRUE(QMetaObject::invokeMethod(window.get(), "moveAndReopen"));
        QCoreApplication::processEvents();
        EXPECT_NEAR(mappedPoint("popupTopLeft").x(), mappedPoint("controlTopLeft").x(), 0.5);
        EXPECT_NEAR(mappedPoint("popupBottomRight").x(), mappedPoint("controlBottomRight").x(), 0.5);
        if (opensAbove) {
          EXPECT_NEAR(mappedPoint("popupBottomRight").y(), mappedPoint("controlTopLeft").y() - (2 * scale), 0.5);
        } else {
          EXPECT_NEAR(mappedPoint("popupTopLeft").y(), mappedPoint("controlBottomRight").y() + (2 * scale), 0.5);
        }
      }
    }
  }
}

TEST_P(ComboBoxGeometry, EmptyAndLongModelsKeepSelectedItemVisible) {
  for (const qreal scale : {0.78, 1.0, 1.25}) {
    QQmlComponent component(&engine_);
    component.setData(QString{R"(
import QtQuick
import Holonight as H
import Holonight.Controls
Window {
    width: 400; height: 500; visible: true
    property alias combo: combo
    Item {
        scale: %SCALE%; transformOrigin: Item.TopLeft
        %TYPE% { id: combo; width: 200; model: 0; maximumVisibleItems: 4 }
    }
})"}
                          .replace("%TYPE%", GetParam())
                          .replace("%SCALE%", QString::number(scale))
                          .toUtf8(),
                      QUrl());
    std::unique_ptr<QObject> root(component.create());
    ASSERT_NE(root, nullptr) << component.errorString().toStdString();
    auto* combo = root->property("combo").value<QObject*>();
    auto* popup = combo->property("popup").value<QObject*>();
    ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
    QCoreApplication::processEvents();
    auto* list = popup->property("contentItem").value<QObject*>();
    EXPECT_EQ(list->property("count").toInt(), 0);
    EXPECT_EQ(list->property("height").toReal(), 0);
    EXPECT_FALSE(list->property("interactive").toBool());
    ASSERT_TRUE(QMetaObject::invokeMethod(popup, "close"));
    ASSERT_TRUE(combo->setProperty("model", 100));
    ASSERT_TRUE(combo->setProperty("currentIndex", 99));
    ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
    ASSERT_TRUE(QTest::qWaitFor([&] { return list->property("currentIndex").toInt() == 99; }));
    QCoreApplication::processEvents();
    EXPECT_TRUE(list->property("interactive").toBool());
    EXPECT_NEAR(list->property("height").toReal(), 4 * combo->property("delegateHeight").toReal(), 0.5);
    auto* selected = list->property("currentItem").value<QQuickItem*>();
    ASSERT_NE(selected, nullptr);
    EXPECT_GE(selected->y(), list->property("contentY").toReal() - 0.5);
    EXPECT_LE(selected->y() + selected->height(),
              list->property("contentY").toReal() + list->property("height").toReal() + 0.5);
  }
}

TEST_P(ComboBoxGeometry, RefreshesPopupGeometryAfterAncestorLayout) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(QString{R"(
    import QtQuick
    import Holonight as H
    import Holonight.Controls
    Window {
      width: 480
      height: 400
      visible: true

      property var combo: combo
      function layoutAndOpen() {
        form.x = 245
        form.y = 306.25
        form.scale = 1.25
        combo.popup.open()
      }
      function controlTopLeft() { return combo.mapToItem(contentItem, 0, 0) }
      function popupTopLeft() { return combo.popup.background.mapToItem(contentItem, 0, 0) }

      Item {
        id: form
        transformOrigin: Item.TopLeft

        %TYPE% {
          id: combo
          x: 19
          y: 23
          width: 160
          model: ["one", "two", "three"]
        }
      }
    }
  )"}
                   .replace(QStringLiteral("%TYPE%"), GetParam())
                   .toUtf8(),
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  ASSERT_TRUE(QMetaObject::invokeMethod(window.get(), "layoutAndOpen", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  const auto mappedPoint = [&](const char* method) {
    QVariant result;
    EXPECT_TRUE(QMetaObject::invokeMethod(window.get(), method, Qt::DirectConnection, Q_RETURN_ARG(QVariant, result)));
    return result.toPointF();
  };
  auto* combo = window->property("combo").value<QObject*>();
  ASSERT_NE(combo, nullptr);
  auto* popup = combo->property("popup").value<QObject*>();
  ASSERT_NE(popup, nullptr);
  EXPECT_TRUE(popup->property("opensAbove").toBool());
  EXPECT_NEAR(combo->property("effectiveScale").toReal(), 1.25, 0.001);
  EXPECT_NEAR(mappedPoint("popupTopLeft").x(), mappedPoint("controlTopLeft").x(), 0.5);
  EXPECT_GT(mappedPoint("popupTopLeft").x(), 250.0);
  EXPECT_GT(mappedPoint("popupTopLeft").y(), 200.0);
}

TEST_P(ComboBoxGeometry, ReportsUnsupportedPopupTransforms) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(QString{R"(
    import QtQuick
    import Holonight as H
    import Holonight.Controls
    Item {
      property var nonUniform: nonUniform
      property var rotated: rotated
      Item {
        scale: 1.25
        transform: Scale { xScale: 1; yScale: 0.8 }
        %TYPE% { id: nonUniform }
      }
      Item {
        rotation: 5
        %TYPE% { id: rotated }
      }
    }
  )"}
                   .replace(QStringLiteral("%TYPE%"), GetParam())
                   .toUtf8(),
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_FALSE(object->property("nonUniform").value<QObject*>()->property("popupTransformSupported").toBool());
  EXPECT_FALSE(object->property("rotated").value<QObject*>()->property("popupTransformSupported").toBool());
}

TEST_F(QmlSmoke, ComboBox_DelegateCornersFollowPopupInnerRadius) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Window {
      width: 320
      height: 240
      visible: true
      property var combo: combo
      function openPopup() { combo.popup.open() }
      function delegateRadius(index, propertyName) {
        const delegate = combo.popup.contentItem.itemAtIndex(index)
        return delegate ? delegate[propertyName] : -1
      }
      ComboBox { id: combo; model: ["first", "middle", "last"] }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "openPopup", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  const auto radius = [&](int index, const char* property_name) {
    QVariant result;
    EXPECT_TRUE(QMetaObject::invokeMethod(object.get(), "delegateRadius", Qt::DirectConnection,
                                          Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, index),
                                          Q_ARG(QVariant, QString::fromLatin1(property_name))));
    return result.toReal();
  };
  auto* combo = object->property("combo").value<QObject*>();
  ASSERT_NE(combo, nullptr);
  auto* popup = combo->property("popup").value<QObject*>();
  ASSERT_NE(popup, nullptr);
  auto* background = popup->property("background").value<QObject*>();
  ASSERT_NE(background, nullptr);
  const qreal inner_radius =
      (std::max)(0.0, background->property("semanticRadius").toReal() - popup->property("padding").toReal());

  EXPECT_DOUBLE_EQ(radius(0, "topLeftRadius"), inner_radius);
  EXPECT_DOUBLE_EQ(radius(0, "bottomLeftRadius"), 0.0);
  EXPECT_DOUBLE_EQ(radius(1, "topLeftRadius"), 0.0);
  EXPECT_DOUBLE_EQ(radius(1, "bottomRightRadius"), 0.0);
  EXPECT_DOUBLE_EQ(radius(2, "topRightRadius"), 0.0);
  EXPECT_DOUBLE_EQ(radius(2, "bottomRightRadius"), inner_radius);
}

TEST_P(ComboBoxGeometry, BoundsPopupByModelAndMaximumVisibleItems) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(QString{R"(
    import QtQuick
    import Holonight as H
    import Holonight.Controls
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

      %TYPE% { id: shortCombo; model: ["1", "2", "3"] }
      %TYPE% { id: defaultCombo; x: 130; model: 20 }
      %TYPE% { id: fourCombo; x: 260; model: 20; maximumVisibleItems: 4 }
      %TYPE% { id: invalidCombo; y: 320; model: 20; maximumVisibleItems: -5 }
    }
  )"}
                   .replace(QStringLiteral("%TYPE%"), GetParam())
                   .toUtf8(),
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);

  auto verifyPopup = [&](const char* comboProperty, qreal expectedRows) {
    auto* combo = window->property(comboProperty).value<QObject*>();
    ASSERT_NE(combo, nullptr);
    ASSERT_TRUE(QMetaObject::invokeMethod(window.get(), "openCombo", Qt::DirectConnection,
                                          Q_ARG(QVariant, QVariant::fromValue(combo))));
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    auto* popup = combo->property("popup").value<QObject*>();
    ASSERT_NE(popup, nullptr);
    auto* list = popup->property("contentItem").value<QObject*>();
    ASSERT_NE(list, nullptr);
    EXPECT_NEAR(list->property("height").toReal(), expectedRows * combo->property("delegateHeight").toReal(), 0.5);
    EXPECT_NEAR(popup->property("height").toReal(), (expectedRows * combo->property("delegateHeight").toReal()) + 8.0,
                0.5);
    popup->setProperty("visible", false);
  };

  verifyPopup("shortCombo", 3);
  verifyPopup("defaultCombo", 8);
  verifyPopup("fourCombo", 4);
  verifyPopup("invalidCombo", 1);
  EXPECT_EQ(window->property("invalidCombo").value<QObject*>()->property("resolvedMaximumVisibleItems").toInt(), 1);
}

TEST_P(ComboBoxGeometry, ConstrainsPopupToWindowAndKeepsOverflowInteractive) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(QString{R"(
    import QtQuick
    import Holonight as H
    import Holonight.Controls
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

      %TYPE% {
        id: combo
        y: 72
        model: 20
        Component.onCompleted: popup.open()
      }
    }
  )"}
                   .replace(QStringLiteral("%TYPE%"), GetParam())
                   .toUtf8(),
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> window{comp.create()};
  ASSERT_NE(window, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  auto* combo = window->property("combo").value<QObject*>();
  ASSERT_NE(combo, nullptr);
  auto* popup = combo->property("popup").value<QObject*>();
  ASSERT_NE(popup, nullptr);
  auto* list = popup->property("contentItem").value<QObject*>();
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
    auto* combo = window->property(comboProperty).value<QObject*>();
    EXPECT_NE(combo, nullptr);
    auto* popup = combo->property("popup").value<QObject*>();
    EXPECT_NE(popup, nullptr);
    QMetaObject::invokeMethod(popup, "open", Qt::DirectConnection);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    auto* list = popup->property("contentItem").value<QObject*>();
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

// Keep the exhaustive geometry matrix in a single test scenario.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_F(QmlSmoke, Indicators_ReserveLayoutSpaceAndMirror) {
  for (const auto* type : {"CheckBox", "RadioButton", "Switch"}) {
    for (bool mirrored : {false, true}) {
      for (bool labeled : {false, true}) {
        for (int size_role : {0, 1, 2, 3}) {
          SCOPED_TRACE(::testing::Message()
                       << type << " mirrored=" << mirrored << " labeled=" << labeled << " size=" << size_role);
          const auto qml =
              QStringLiteral(R"(
            import QtQuick
            import QtQuick.Layouts
            import Holonight as H
            Item {
              width: 400
              height: 100
              RowLayout {
                anchors.fill: parent
                layoutDirection: %1 ? Qt.RightToLeft : Qt.LeftToRight
                Text { objectName: "label"; text: "Adjacent label"; Layout.fillWidth: true }
                H.%2 {
                  objectName: "control"
                  LayoutMirroring.enabled: %1
                  %3
                  %4
                }
              }
            }
          )")
                  .arg(mirrored ? "true" : "false", type,
                       labeled ? "text: \"Labeled control\"" : "contentItem: null; padding: 0; spacing: 0",
                       QString::fromLatin1(type) == "Switch" ? QStringLiteral("sizeRole: %1").arg(size_role) : "");
          QQmlComponent comp{&engine_};
          comp.setData(qml.toUtf8(), QUrl{});
          ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
          std::unique_ptr<QObject> root{comp.create()};
          ASSERT_NE(root, nullptr);
          QCoreApplication::processEvents();
          auto* control = root->findChild<QQuickItem*>(QStringLiteral("control"));
          auto* label = root->findChild<QQuickItem*>(QStringLiteral("label"));
          ASSERT_NE(control, nullptr);
          ASSERT_NE(label, nullptr);
          auto* indicator = control->property("indicator").value<QQuickItem*>();
          ASSERT_NE(indicator, nullptr);
          for (bool enabled : {true, false}) {
            for (bool checked : {false, true}) {
              control->setProperty("enabled", enabled);
              control->setProperty("checked", checked);
              QCoreApplication::processEvents();
              EXPECT_GE(control->implicitWidth(), indicator->width());
              EXPECT_GE(control->implicitHeight(), indicator->height());
              EXPECT_GE(indicator->x(), 0);
              EXPECT_LE(indicator->x() + indicator->width(), control->width());
              EXPECT_GE(indicator->y(), 0);
              EXPECT_LE(indicator->y() + indicator->height(), control->height());
              if (mirrored) {
                EXPECT_LE(control->x() + control->width(), label->x());
                EXPECT_NEAR(indicator->x() + indicator->width(),
                            control->width() - control->property("rightPadding").toDouble(), 0.001);
              } else {
                EXPECT_GE(control->x(), label->x() + label->width());
                EXPECT_NEAR(indicator->x(), control->property("leftPadding").toDouble(), 0.001);
              }
              EXPECT_LE(control->x() + indicator->x() + indicator->width(), 400);
              if (labeled) {
                auto* content = control->property("contentItem").value<QQuickItem*>();
                ASSERT_NE(content, nullptr);
                EXPECT_GE(content->property(mirrored ? "rightPadding" : "leftPadding").toDouble(), indicator->width());
              }
            }
          }
        }
      }
    }
  }
}

TEST_F(QmlSmoke, Switch_KnobRemainsVisibleAcrossThemesAndHoverStates) {
  EnvGuard config_guard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

  for (const auto* scheme :
       {"holonight-dark", "holonight-light", "holonight-mocha", "holonight-latte", "holonight-storm", "holonight-day",
        "holonight-ember", "holonight-sol", "holonight-cyber-d", "holonight-cyber-l", "holonight-dracula",
        "holonight-alucard", "holonight-frost", "holonight-snow", "holonight-canopy", "holonight-glade"}) {
    SCOPED_TRACE(scheme);
    writeAppearance(path, scheme);
    QQmlEngine engine;
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
    QQmlComponent comp{&engine};
    comp.setData(R"(
      import QtQuick
      import Holonight
      Window {
        width: 200
        height: 100
        visible: true
        property alias toggle: toggle
        property color knobColor: toggle.checked ? toggle.controlColors.colors.onPrimary
                                                 : toggle.controlColors.colors.textPrimary
        property color trackColor: toggle.checked
          ? (toggle.hovered ? toggle.controlColors.colors.primaryHover : toggle.controlColors.colors.primary)
          : (toggle.hovered ? toggle.controlColors.colors.textMuted : toggle.controlColors.colors.borderPassive)
        property color disabledKnobColor: toggle.controlColors.colors.disabledThumb
        property color disabledTrackColor: toggle.controlColors.colors.surface
        Switch { id: toggle; x: 40; y: 30; hoverEnabled: true }
      }
    )",
                 QUrl{});
    ASSERT_TRUE(comp.isReady()) << comp.errorString().toStdString();
    std::unique_ptr<QObject> object{comp.create()};
    ASSERT_NE(object, nullptr);
    auto* window = qobject_cast<QQuickWindow*>(object.get());
    ASSERT_NE(window, nullptr);
    ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
    auto* toggle = object->property("toggle").value<QQuickItem*>();
    ASSERT_NE(toggle, nullptr);
    auto* indicator = toggle->property("indicator").value<QQuickItem*>();
    ASSERT_NE(indicator, nullptr);
    const auto children = indicator->childItems();
    ASSERT_EQ(children.size(), 3);
    auto* track = children.front();
    auto* knob = children.back();
    const QPoint center = toggle->mapToScene(QPointF{toggle->width() / 2, toggle->height() / 2}).toPoint();

    for (const bool checked : {false, true}) {
      ASSERT_TRUE(toggle->setProperty("checked", checked));
      for (const bool hovered : {false, true, false}) {
        SCOPED_TRACE(checked ? "checked" : "unchecked");
        SCOPED_TRACE(hovered ? "hovered" : "idle");
        QTest::mouseMove(window, hovered ? center : QPoint{1, 1});
        ASSERT_TRUE(QTest::qWaitFor([&] { return toggle->property("hovered").toBool() == hovered; }));
        ASSERT_TRUE(waitForPropertyColor(track, "color", object->property("trackColor").value<QColor>()));
        ASSERT_TRUE(waitForPropertyColor(knob, "color", object->property("knobColor").value<QColor>()));
        EXPECT_NE(knob->property("color").value<QColor>(), track->property("color").value<QColor>());
      }
    }
    ASSERT_TRUE(toggle->setProperty("enabled", false));
    for (const bool checked : {false, true}) {
      ASSERT_TRUE(toggle->setProperty("checked", checked));
      ASSERT_TRUE(waitForPropertyColor(track, "color", object->property("disabledTrackColor").value<QColor>()));
      ASSERT_TRUE(waitForPropertyColor(knob, "color", object->property("disabledKnobColor").value<QColor>()));
    }
  }
}

TEST_F(QmlSmoke, Switch_SizeRoles) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    import Holonight.Core

    Item {
      property alias xs: xs
      property alias compact: compact
      property alias normal: normal
      property alias large: large
      property alias hero: hero
      property alias invalid: invalid

      Switch { id: xs; sizeRole: HnControlSize.Xs }
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

  auto* compact = root->property("compact").value<QObject*>();
  auto* extra_small = root->property("xs").value<QObject*>();
  auto* normal = root->property("normal").value<QObject*>();
  auto* large = root->property("large").value<QObject*>();
  auto* hero = root->property("hero").value<QObject*>();
  auto* invalid = root->property("invalid").value<QObject*>();

  ASSERT_NE(compact, nullptr);
  ASSERT_NE(extra_small, nullptr);
  ASSERT_NE(normal, nullptr);
  ASSERT_NE(large, nullptr);
  ASSERT_NE(hero, nullptr);
  ASSERT_NE(invalid, nullptr);

  EXPECT_EQ(compact->property("resolvedSizeRole").toInt(), 0);
  EXPECT_EQ(extra_small->property("resolvedSizeRole").toInt(), 4);
  EXPECT_EQ(extra_small->property("indicatorTrackWidth").toDouble(), 24.0);
  EXPECT_EQ(extra_small->property("indicatorTrackHeight").toDouble(), 12.0);
  EXPECT_EQ(extra_small->property("indicatorThumbSize").toDouble(), 8.0);
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

  auto* indicator = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndicator"));
  auto* enter = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndeterminateEnter"));
  auto* travel = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndeterminateTravel"));
  auto* exit = progress->findChild<QObject*>(QStringLiteral("hnProgressBarIndeterminateExit"));
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

TEST_F(QmlSmoke, MenuItemTracksHoverAndOpensSubmenuOnHover) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Window {
      width: 400
      height: 300
      visible: true
      property var menu: menu
      property var subMenu: subMenu
      function openMenu() { menu.popup(20, 20) }
      function submenuDelegateCenter() {
        const delegate = menu.contentItem.itemAtIndex(1)
        return delegate ? delegate.mapToItem(contentItem, delegate.width / 2, delegate.height / 2) : Qt.point(-1, -1)
      }
      function submenuDelegateObject() { return menu.contentItem.itemAtIndex(1) }
      Menu {
        id: menu
        MenuItem { text: "Plain" }
        Menu {
          id: subMenu
          title: "Submenu"
          MenuItem { text: "Nested" }
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
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "openMenu", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  QVariant center_value;
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "submenuDelegateCenter", Qt::DirectConnection,
                                        Q_RETURN_ARG(QVariant, center_value)));
  const QPointF center = center_value.toPointF();
  ASSERT_GE(center.x(), 0);
  ASSERT_GE(center.y(), 0);
  QTest::mouseMove(window, center.toPoint());

  QVariant delegate_value;
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "submenuDelegateObject", Qt::DirectConnection,
                                        Q_RETURN_ARG(QVariant, delegate_value)));
  auto* delegate = delegate_value.value<QObject*>();
  ASSERT_NE(delegate, nullptr);
  EXPECT_TRUE(delegate->property("hoverEnabled").toBool());
  QTRY_VERIFY_WITH_TIMEOUT(delegate->property("hovered").toBool(), 500);
  auto* submenu = object->property("subMenu").value<QObject*>();
  ASSERT_NE(submenu, nullptr);
  QTRY_VERIFY_WITH_TIMEOUT(submenu->property("visible").toBool(), 1000);
}

TEST_F(QmlSmoke, MenuItem_RendersStandardIconAndIndependentCheckmark) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    import Holonight.Core
    Item {
      property real expectedMenuRadius: HnAppearance.roundedRadius(HnSurfaceRole.Menu, highlighted.width,
                                                                   highlighted.height, HnAppearance.revision)
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

  auto* plain = root->property("plain").value<QObject*>();
  auto* checked_icon = root->property("checkedIcon").value<QObject*>();
  auto* highlighted = root->property("highlighted").value<QObject*>();
  auto* pressed_highlighted = root->property("pressedHighlighted").value<QObject*>();
  ASSERT_NE(plain, nullptr);
  ASSERT_NE(checked_icon, nullptr);
  ASSERT_NE(highlighted, nullptr);
  ASSERT_NE(pressed_highlighted, nullptr);

  auto* icon = checked_icon->findChild<QObject*>(QStringLiteral("hnMenuItemIcon"));
  ASSERT_NE(icon, nullptr);
  EXPECT_TRUE(icon->property("visible").toBool());
  EXPECT_DOUBLE_EQ(icon->property("size").toReal(), 16.0);
  EXPECT_FALSE(plain->findChild<QObject*>(QStringLiteral("hnMenuItemIcon"))->property("visible").toBool());

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_EQ(highlighted->property("background").value<QObject*>()->property("color").value<QColor>(), tok.primary);
  EXPECT_EQ(pressed_highlighted->property("background").value<QObject*>()->property("color").value<QColor>(),
            tok.primaryPressed);
  EXPECT_EQ(highlighted->findChild<QObject*>(QStringLiteral("hnMenuItemLabel"))->property("color").value<QColor>(),
            tok.onPrimary);
  EXPECT_DOUBLE_EQ(highlighted->property("background").value<QObject*>()->property("semanticRadius").toReal(),
                   root->property("expectedMenuRadius").toReal());
}

TEST_F(QmlSmoke, ToolTip_LoadsWithoutError) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import Holonight
    ToolTip { text: "hint" }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> tooltip{comp.create()};
  ASSERT_NE(tooltip, nullptr);
  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_EQ(tooltip->property("delay").toInt(), 500);
  EXPECT_EQ(tooltip->property("contentItem").value<QObject*>()->property("color").value<QColor>(), tok.textPrimary);
  EXPECT_EQ(tooltip->property("background").value<QObject*>()->property("color").value<QColor>(), tok.surfaceRaised);
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

  auto* compact = root->property("compact").value<QObject*>();
  auto* normal = root->property("normal").value<QObject*>();
  auto* large = root->property("large").value<QObject*>();
  auto* invalid = root->property("invalid").value<QObject*>();
  auto* disabled = root->property("disabled").value<QObject*>();
  ASSERT_NE(compact, nullptr);
  ASSERT_NE(normal, nullptr);
  ASSERT_NE(large, nullptr);
  ASSERT_NE(invalid, nullptr);
  ASSERT_NE(disabled, nullptr);

  EXPECT_DOUBLE_EQ(compact->property("implicitWidth").toReal(), 28.0);
  EXPECT_DOUBLE_EQ(compact->property("implicitHeight").toReal(), 28.0);
  auto* compact_icon = compact->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  auto* normal_icon = normal->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  auto* large_icon = large->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
  auto* disabled_icon = disabled->findChild<QObject*>(QStringLiteral("hnIconButtonIcon"));
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

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_EQ(disabled_icon->property("resolvedColor").value<QColor>(), tok.textDisabled);
  EXPECT_EQ(normal->property("background").value<QObject*>()->property("color").value<QColor>(),
            QColor{Qt::transparent});
  EXPECT_TRUE(normal->property("hoverEnabled").toBool());
}

TEST_F(QmlSmoke, Controls_IconButtonAppliesHoverColorWithoutTransition) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      width: 100
      height: 100
      visible: true
      property var button: button
      HnIconButton { id: button; anchors.centerIn: parent }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(object.get());
  ASSERT_NE(window, nullptr);
  auto* button = object->property("button").value<QObject*>();
  ASSERT_NE(button, nullptr);

  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  QTest::mouseMove(window, QPoint{1, 1});
  QTest::mouseMove(window, QPoint{50, 50});
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(button->property("hovered").toBool());
  EXPECT_EQ(button->property("background").value<QObject*>()->property("color").value<QColor>(),
            canonicalDefaultTokens().surfaceHover);
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

  auto* short_delegate = root->property("shortDelegate").value<QObject*>();
  auto* long_delegate = root->property("longDelegate").value<QObject*>();
  ASSERT_NE(short_delegate, nullptr);
  ASSERT_NE(long_delegate, nullptr);
  auto* short_label = short_delegate->findChild<QObject*>(QStringLiteral("hnNavigationDelegateTitle"));
  auto* long_label = long_delegate->findChild<QObject*>(QStringLiteral("hnNavigationDelegateTitle"));
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
      rendering: HnIcon.Original
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

TEST_F(QmlSmoke, HnIcon_ExplicitSourceAndRenderingContract) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property HnIcon themeIcon: HnIcon { name: "folder"; rendering: HnIcon.Original }
      property HnIcon assetIcon: HnIcon {
        source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
        rendering: HnIcon.Semantic
      }
      property HnIcon originalProvider: HnIcon {
        source: "image://icon/example"
        rendering: HnIcon.Original
      }
      property HnIcon unsupportedProvider: HnIcon {
        source: "image://icon/example"
        rendering: HnIcon.Semantic
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  const auto child = [&](const char* property) { return object->property(property).value<QObject*>(); };
  ASSERT_NE(child("themeIcon"), nullptr);
  EXPECT_TRUE(child("themeIcon")->property("_renderSource").toString().startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_TRUE(child("assetIcon")->property("_renderSource").toString().startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_EQ(child("originalProvider")->property("_renderSource").toUrl().toString(),
            QStringLiteral("image://icon/example"));
  EXPECT_TRUE(child("unsupportedProvider")->property("hasError").toBool());
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
  auto* title_label = object->findChild<QObject*>(QStringLiteral("hnApplicationWindowHeaderTitle"));
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
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path);
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

  writeAppearance(path, "holonight-dark", "blue", "Inter", 12, HoloNight::Config::ShapeStyle::Chamfered, 2.0);
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
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path);
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

  writeAppearance(path, "holonight-dark", "blue", "Inter", 12, HoloNight::Config::ShapeStyle::Inherit, 2.0);
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
  auto* content = object->findChild<QObject*>(QStringLiteral("consumerContent"));
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
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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
  writeAppearance(path, "holonight-light");
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadPalette", Qt::DirectConnection));
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  const QString updated = object->property("_renderSource").toString();
  EXPECT_GT(object->property("currentRevision").toInt(), object->property("initialRevision").toInt());
  EXPECT_TRUE(initial.startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_TRUE(updated.startsWith(QStringLiteral("image://hnicons/")));
  EXPECT_NE(initial, updated);

  writeAppearance(path);
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
      property int focusBorderWidth: HnMetrics.focusBorderWidth
      property color ansiBlue: HoloniightPalette.ansiBlue
      property color ansiBrightWhite: HoloniightPalette.ansiBrightWhite
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HoloniightPalette_ReloadEmitsNotificationOnChange) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

  // Force singleton creation in dark mode first.
  {
    QQmlComponent init = QQmlComponent{&engine_};
    init.setData(R"(import QtQuick; import Holonight.Core; Item { property color c: HoloniightPalette.primary })",
                 QUrl{});
    std::unique_ptr<QObject> tmp{init.create()};
    ASSERT_NE(tmp, nullptr);
  }

  writeAppearance(path, "holonight-light");

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

TEST_F(QmlSmoke, HoloniightPalette_ReloadUsesCanonicalLightScheme) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path, "holonight-light");
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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
  EnvGuard configGuard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path, "holonight-light", "blue");
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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

TEST_F(QmlSmoke, HoloniightPalette_WatchesCanonicalAppearanceChanges) {
  EnvGuard configGuard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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

  writeAppearance(path, "holonight-light");
  EXPECT_TRUE(waitForPropertyColor(object.get(), "background", QColor(QStringLiteral("#E7EEF5"))));
}

TEST_F(QmlSmoke, AppearanceSingletonsPublishOneEngineLocalRevision) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property int appearanceRevision: HnAppearance.revision
      property int paletteRevision: HoloniightPalette.revision
      property color background: HoloniightPalette.background
      property string uiFont: HolonightTheme.uiFont
      function reloadAppearance() { HnAppearance.reload() }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);

  writeAppearance(path, "holonight-light", "blue", "Shared Sans", 14);
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadAppearance"));
  QCoreApplication::processEvents();

  EXPECT_EQ(object->property("appearanceRevision").toInt(), 1);
  EXPECT_EQ(object->property("paletteRevision").toInt(), 1);
  EXPECT_EQ(object->property("background").value<QColor>(), QColor(QStringLiteral("#E7EEF5")));
  EXPECT_EQ(object->property("uiFont").toString(), QStringLiteral("Shared Sans"));
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
  EXPECT_EQ(object->property("appTitleSize").toInt(), 16);
  EXPECT_EQ(object->property("brandForeground").value<QColor>(), QColor(QStringLiteral("#9A8CFF")));
}

TEST_F(QmlSmoke, HolonightTheme_ReloadUpdatesExistingLabelTypography) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path, "holonight-dark", "blue", "Initial Sans", 10);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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

  writeAppearance(path, "holonight-dark", "blue", "Reloaded Sans", 14);
  ASSERT_TRUE(QMetaObject::invokeMethod(object.get(), "reloadTheme"));
  QCoreApplication::processEvents();

  EXPECT_EQ(object->property("labelFamily").toString(), QStringLiteral("Reloaded Sans"));
  EXPECT_EQ(object->property("labelSize").toInt(), 14);
}

TEST_F(QmlSmoke, HolonightTheme_ReloadUpdatesSharedControlsTypography) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path, "holonight-dark", "blue", "Initial Sans", 10);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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

  auto* btn = object->findChild<QObject*>(QStringLiteral("btn"));
  auto* nav = object->findChild<QObject*>(QStringLiteral("nav"));
  auto* row = object->findChild<QObject*>(QStringLiteral("row"));
  auto* status = object->findChild<QObject*>(QStringLiteral("status"));
  auto* check = object->findChild<QObject*>(QStringLiteral("check"));

  ASSERT_NE(btn, nullptr);
  ASSERT_NE(nav, nullptr);
  ASSERT_NE(row, nullptr);
  ASSERT_NE(status, nullptr);
  ASSERT_NE(check, nullptr);

  auto* nav_title = nav->findChild<QObject*>(QStringLiteral("hnNavigationDelegateTitle"));
  auto* row_title = row->findChild<QObject*>(QStringLiteral("hnSettingsRowTitle"));
  auto* row_desc = row->findChild<QObject*>(QStringLiteral("hnSettingsRowDescription"));
  auto* status_text = status->findChild<QObject*>(QStringLiteral("hnStatusIndicatorText"));

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
  writeAppearance(path, "holonight-dark", "blue", "Reloaded Sans", 14);
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
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  writeAppearance(path, "holonight-dark", "blue", "Initial Sans", 10);
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());

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

  auto* custom_btn = object->findChild<QObject*>(QStringLiteral("customBtn"));
  auto* custom_nav = object->findChild<QObject*>(QStringLiteral("customNav"));
  auto* custom_card = object->findChild<QObject*>(QStringLiteral("customCard"));

  ASSERT_NE(custom_btn, nullptr);
  ASSERT_NE(custom_nav, nullptr);
  ASSERT_NE(custom_card, nullptr);

  EXPECT_EQ(custom_btn->property("font").value<QFont>().pointSize(), 22);
  EXPECT_EQ(custom_btn->property("font").value<QFont>().family(), QStringLiteral("Initial Sans"));
  EXPECT_EQ(custom_nav->property("font").value<QFont>().family(), QStringLiteral("Custom Nav Font"));
  EXPECT_EQ(custom_card->property("font").value<QFont>().pointSize(), 18);

  // Reload theme
  writeAppearance(path, "holonight-dark", "blue", "Reloaded Sans", 14);
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

  auto* brand = title->findChild<QObject*>(QStringLiteral("brandLabel"));
  auto* application = title->findChild<QObject*>(QStringLiteral("applicationLabel"));
  auto* icon = title->findChild<QObject*>(QStringLiteral("appTitleIcon"));
  ASSERT_NE(brand, nullptr);
  ASSERT_NE(application, nullptr);
  ASSERT_NE(icon, nullptr);

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_EQ(brand->property("text").toString(), QStringLiteral("HoloNight"));
  EXPECT_EQ(brand->property("color").value<QColor>(), tok.brandForeground);
  EXPECT_EQ(application->property("color").value<QColor>(), tok.textPrimary);
  EXPECT_EQ(brand->property("font").value<QFont>().family(), QStringLiteral("Inter"));
  EXPECT_EQ(brand->property("font").value<QFont>().pointSize(), 16);
  EXPECT_EQ(brand->property("font").value<QFont>().weight(), QFont::DemiBold);
  EXPECT_EQ(application->property("font").value<QFont>(), brand->property("font").value<QFont>());
  EXPECT_EQ(icon->property("size").toInt(), 32);
  EXPECT_EQ(icon->property("rendering").toInt(), 1);
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

  auto* empty = root->property("emptyTitle").value<QObject*>();
  auto* failed = root->property("failedTitle").value<QObject*>();
  auto* constrained = root->property("constrainedTitle").value<QObject*>();
  ASSERT_NE(empty, nullptr);
  ASSERT_NE(failed, nullptr);
  ASSERT_NE(constrained, nullptr);
  EXPECT_DOUBLE_EQ(failed->property("implicitWidth").toDouble(), empty->property("implicitWidth").toDouble());
  EXPECT_GT(empty->property("implicitWidth").toDouble(), 0.0);
  EXPECT_GT(empty->property("implicitHeight").toDouble(), 0.0);
  auto* application = constrained->findChild<QObject*>(QStringLiteral("applicationLabel"));
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

  auto* defaultTitle = root->property("defaultTitle").value<QObject*>();
  auto* skippedTitle = root->property("skippedTitle").value<QObject*>();
  ASSERT_NE(defaultTitle, nullptr);
  ASSERT_NE(skippedTitle, nullptr);

  EXPECT_FALSE(defaultTitle->property("skipBranding").toBool());
  EXPECT_TRUE(skippedTitle->property("skipBranding").toBool());

  auto* defaultBrand = defaultTitle->findChild<QObject*>(QStringLiteral("brandLabel"));
  auto* skippedBrand = skippedTitle->findChild<QObject*>(QStringLiteral("brandLabel"));
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
  EXPECT_EQ(object->property("familyCount").toInt(), 8);
  EXPECT_EQ(object->property("variantCount").toInt(), 16);
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
  auto* leading = row->property("leadingItem").value<QObject*>();
  auto* control = row->property("controlItem").value<QObject*>();
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
  EXPECT_NEAR(default_control->mapToItem(default_row, QPointF{}).y() + (default_control->height() / 2),
              default_row->height() / 2, 0.51);
  EXPECT_NEAR(padded_control->mapToItem(padded_row, QPointF{}).y() + (padded_control->height() / 2),
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
  auto* single_row = object->property("singleRow").value<QObject*>();
  auto* compound_row = object->property("compoundRow").value<QObject*>();
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
  auto* empty = root->property("empty").value<QObject*>();
  auto* composed = root->property("composed").value<QObject*>();
  auto* compactHeader = root->property("compactHeader").value<QObject*>();
  auto* categoryHeader = root->property("categoryHeader").value<QObject*>();
  ASSERT_NE(empty, nullptr);
  ASSERT_NE(composed, nullptr);
  ASSERT_NE(compactHeader, nullptr);
  ASSERT_NE(categoryHeader, nullptr);
  EXPECT_EQ(empty->property("implicitHeight").toReal(), 0.0);
  ASSERT_NE(composed->property("leadingItem").value<QObject*>(), nullptr);
  ASSERT_NE(composed->property("trailingItem").value<QObject*>(), nullptr);
  EXPECT_FALSE(composed->property("activeFocusOnTab").toBool());
  EXPECT_EQ(compactHeader->property("resolvedSizeRole").toInt(), 0);
  auto* categoryTitle = categoryHeader->findChild<QObject*>(QStringLiteral("sectionTitleLabel"));
  ASSERT_NE(categoryTitle, nullptr);
  EXPECT_EQ(categoryTitle->property("rawText").toString(), QStringLiteral("// Theme"));
  EXPECT_EQ(categoryTitle->property("color").value<QColor>(), root->property("expectedCategoryColor").value<QColor>());
  EXPECT_EQ(categoryTitle->property("font").value<QFont>().pointSizeF(), 11.0);
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
  auto* progress = state->findChild<QObject*>(QStringLiteral("hnLoadingProgress"));
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
  auto* navigation = root->property("navigation").value<QObject*>();
  auto* list = root->property("list").value<QObject*>();
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
  auto* list = root->property("list").value<QObject*>();
  auto* centered_list = root->property("centeredList").value<QObject*>();
  ASSERT_NE(list, nullptr);
  ASSERT_NE(centered_list, nullptr);
  auto* leading = list->property("leadingItem").value<QObject*>();
  auto* leading_loader = list->findChild<QObject*>(QStringLiteral("hnListDelegateLeadingLoader"));
  auto* centered_loader = centered_list->findChild<QObject*>(QStringLiteral("hnListDelegateLeadingLoader"));
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

  auto* navigation = root->property("navigation").value<QObject*>();
  auto* list = root->property("list").value<QObject*>();
  ASSERT_NE(navigation, nullptr);
  ASSERT_NE(list, nullptr);
  auto* overlay = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSelectedOverlay"));
  auto* gradient = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSelectionGradient"));
  auto* accent_start = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateAccentStartStop"));
  auto* accent_end = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateAccentEndStop"));
  auto* surface_start = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSurfaceStartStop"));
  auto* surface_end = navigation->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSurfaceEndStop"));
  ASSERT_NE(overlay, nullptr);
  ASSERT_NE(gradient, nullptr);
  ASSERT_NE(accent_start, nullptr);
  ASSERT_NE(accent_end, nullptr);
  ASSERT_NE(surface_start, nullptr);
  ASSERT_NE(surface_end, nullptr);

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_EQ(gradient->property("orientation").toInt(), Qt::Horizontal);
  EXPECT_EQ(accent_start->property("color").value<QColor>(), tok.selectionIndicator);
  EXPECT_EQ(accent_end->property("color").value<QColor>(), tok.selectionIndicator);
  const qreal expected_stop = (std::max)(2.0, static_cast<qreal>(Holonight::metricTokens().border_width * 2)) / 160.0;
  EXPECT_DOUBLE_EQ(accent_end->property("position").toReal(), expected_stop);
  EXPECT_DOUBLE_EQ(surface_start->property("position").toReal(), expected_stop + (0.25 / 160.0));
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

  auto* list_overlay = list->findChild<QObject*>(QStringLiteral("hnSelectableDelegateSelectedOverlay"));
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
  auto* wrapped = root->property("wrapped").value<QObject*>();
  auto* single_line = root->property("singleLine").value<QObject*>();
  ASSERT_NE(wrapped, nullptr);
  ASSERT_NE(single_line, nullptr);
  auto* wrapped_label = wrapped->findChild<QObject*>(QStringLiteral("hnListDelegateSubtitle"));
  auto* single_line_label = single_line->findChild<QObject*>(QStringLiteral("hnListDelegateSubtitle"));
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
    import Holonight.impl as Impl
    Item {
      property Impl.ControlPalette comparisonPalette: Impl.ControlPalette {}
      property alias choice: choice
      property alias card: card
      property alias navigation: navigation
      property alias itemDelegate: itemDelegate

      HnChoiceCard { id: choice; width: 160; height: 80 }
      HnCardDelegate { id: card; width: 160; height: 80 }
      HnNavigationDelegate { id: navigation; width: 160; height: 40 }
      Hn.ItemDelegate { palette: comparisonPalette.appearancePalette; id: itemDelegate; width: 160; height: 40 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  const auto verify_state = [&tok](QObject* control, const QString& base_name, const QString& selection_name,
                                   const QColor& idle_base_color) {
    ASSERT_NE(control, nullptr);
    auto* base = control->findChild<QObject*>(base_name);
    auto* selection = control->findChild<QObject*>(selection_name);
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
  auto* choice = object->property("choice").value<QObject*>();
  ASSERT_NE(choice, nullptr);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

  QTest::mouseMove(window, QPoint{60, 60});
  QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
  EXPECT_TRUE(choice->property("hovered").toBool());
  QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, QPoint{60, 60});
  EXPECT_TRUE(choice->property("down").toBool());

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
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
  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_GE(control->property("implicitHeight").toReal(),
            Holonight::metricTokens().control(Holonight::ControlSize::Normal).height);
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
  auto* first = root->property("first").value<QObject*>();
  auto* second = root->property("second").value<QObject*>();
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

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
  EXPECT_EQ(separator->property("orientation").toInt(), Qt::Horizontal);
  EXPECT_EQ(separator->property("fadeMode").toInt(), root->property("solid").toInt());
  EXPECT_EQ(root->property("fadeEnd").toInt(), 3);
  EXPECT_EQ(separator->property("color").value<QColor>(), tok.borderPassive);
  EXPECT_DOUBLE_EQ(separator->property("thickness").toDouble(), 1.0);
  EXPECT_DOUBLE_EQ(separator->property("implicitWidth").toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(separator->property("implicitHeight").toDouble(), 1.0);
  EXPECT_DOUBLE_EQ(separator->property("height").toDouble(), 1.0);
  EXPECT_TRUE(root->property("ignored").toBool());

  ASSERT_TRUE(separator->setProperty("orientation", Qt::Vertical));
  EXPECT_DOUBLE_EQ(separator->property("implicitWidth").toDouble(), 1.0);
  EXPECT_DOUBLE_EQ(separator->property("implicitHeight").toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(separator->property("width").toDouble(), 120.0);
  ASSERT_TRUE(separator->setProperty("orientation", 999));
  EXPECT_EQ(separator->property("effectiveOrientation").toInt(), Qt::Horizontal);
  ASSERT_TRUE(separator->setProperty("thickness", -4.0));
  EXPECT_DOUBLE_EQ(separator->property("implicitWidth").toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(separator->property("implicitHeight").toDouble(), 0.0);
}

TEST_F(QmlSmoke, HnSeparator_AnchorOnlyLayoutsOccupyTheirMinorAxis) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      width: 180; height: 120
      property alias horizontal: horizontal
      property alias vertical: vertical
      HnSeparator {
        id: horizontal
        anchors { left: parent.left; right: parent.right; top: parent.top }
      }
      HnSeparator {
        id: vertical
        orientation: Qt.Vertical
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left }
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  const auto* horizontal = qobject_cast<QQuickItem*>(root->property("horizontal").value<QObject*>());
  const auto* vertical = qobject_cast<QQuickItem*>(root->property("vertical").value<QObject*>());
  ASSERT_NE(horizontal, nullptr);
  ASSERT_NE(vertical, nullptr);

  const qreal thickness = 1.0;
  EXPECT_DOUBLE_EQ(horizontal->width(), 180.0);
  EXPECT_DOUBLE_EQ(horizontal->height(), thickness);
  EXPECT_DOUBLE_EQ(vertical->width(), thickness);
  EXPECT_DOUBLE_EQ(vertical->height(), 120.0);
  auto* horizontal_line = horizontal->findChild<QQuickItem*>("separatorLine");
  auto* vertical_line = vertical->findChild<QQuickItem*>("separatorLine");
  ASSERT_NE(horizontal_line, nullptr);
  ASSERT_NE(vertical_line, nullptr);
  EXPECT_TRUE(horizontal_line->isVisible());
  EXPECT_TRUE(vertical_line->isVisible());
}

TEST_F(QmlSmoke, HnSeparator_ResolvesFadeProfilesAndUsesInheritedOpacity) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnSeparator {
      width: 120
      color: Qt.rgba(0.2, 0.4, 0.6, 0.5)
      opacity: 0.4
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

  EXPECT_DOUBLE_EQ(separator->property("opacity").toDouble(), 0.4);
  EXPECT_EQ(separator->metaObject()->indexOfProperty("centerOpacity"), -1);
  EXPECT_EQ(separator->metaObject()->indexOfProperty("edgeOpacity"), -1);
  EXPECT_NEAR(start->property("color").value<QColor>().alphaF(), 0.5, 0.0001);
  EXPECT_NEAR(end->property("color").value<QColor>().alphaF(), 0.5, 0.0001);

  ASSERT_TRUE(separator->setProperty("fadeMode", 1));
  EXPECT_EQ(start->property("color").value<QColor>().alphaF(), 0.0);
  EXPECT_NEAR(middle->property("color").value<QColor>().alphaF(), 0.5, 0.0001);
  EXPECT_EQ(end->property("color").value<QColor>().alphaF(), 0.0);

  ASSERT_TRUE(separator->setProperty("fadeMode", 2));
  EXPECT_EQ(start->property("color").value<QColor>().alphaF(), 0.0);
  EXPECT_NEAR(middle->property("color").value<QColor>().alphaF(), 0.5, 0.0001);
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
  EXPECT_NEAR(line->mapRectToScene(line->boundingRect()).height() * dpr, 1.0, 0.000001);

  ASSERT_TRUE(root->setProperty("y", 0.6));
  const qreal moved_scene_y = separator->mapToScene(QPointF{}).y();
  EXPECT_NEAR(std::round((moved_scene_y + line->y()) * dpr), (moved_scene_y + line->y()) * dpr, 0.000001);

  ASSERT_TRUE(separator->setProperty("orientation", Qt::Vertical));
  EXPECT_FALSE(line->isVisible());
  ASSERT_TRUE(separator->setProperty("height", 80.0));
  EXPECT_TRUE(line->isVisible());
  EXPECT_NEAR(line->mapRectToScene(line->boundingRect()).width() * dpr, 1.0, 0.000001);
  EXPECT_NEAR(line->mapRectToScene(line->boundingRect()).height(), 80.0, 1e-6);
  ASSERT_TRUE(separator->setProperty("thickness", 0.0));
  EXPECT_FALSE(line->isVisible());
}

TEST_F(QmlSmoke, HnSeparator_CenterOriginResizeUpdatesAlignment) {
  QQmlComponent component(&engine_);
  component.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      width: 80; height: 80; scale: 0.5
      Item {
        scale: 2; width: 40; height: 40
        HnSeparator { objectName: "separator"; width: 30; height: 30 }
      }
    }
  )",
                    QUrl());
  ASSERT_TRUE(component.isReady()) << component.errorString().toStdString();
  std::unique_ptr<QObject> object(component.create());
  auto* root = qobject_cast<QQuickItem*>(object.get());
  ASSERT_NE(root, nullptr);
  auto* separator = root->findChild<QQuickItem*>("separator");
  ASSERT_NE(separator, nullptr);
  auto* line = separator->findChild<QQuickItem*>("separatorLine");
  ASSERT_NE(line, nullptr);
  for (bool vertical : {false, true}) {
    separator->setProperty("orientation", vertical ? Qt::Vertical : Qt::Horizontal);
    for (qreal size : {81.3, 83.7, 80.0}) {
      root->setWidth(size);
      root->setHeight(size);
      const QPointF origin = separator->mapToScene(QPointF{});
      const QRectF bounds = line->mapRectToScene(line->boundingRect());
      EXPECT_NEAR(vertical ? bounds.left() : bounds.top(), std::round(vertical ? origin.x() : origin.y()), 1e-6);
    }
  }
}

TEST_F(QmlSmoke, HnSeparator_ZeroScaleSuppressesTinyPaintedGeometryAndRecovers) {
  QQmlComponent component(&engine_);
  component.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      scale: 10000000; transformOrigin: Item.TopLeft
      HnSeparator { width: 40 }
    }
  )",
                    QUrl());
  ASSERT_TRUE(component.isReady()) << component.errorString().toStdString();
  std::unique_ptr<QObject> object(component.create());
  auto* root = qobject_cast<QQuickItem*>(object.get());
  ASSERT_NE(root, nullptr);
  auto* geometry = root->findChild<QObject*>("separatorGeometry");
  auto* line = root->findChild<QQuickItem*>("separatorLine");
  ASSERT_TRUE(geometry && line);
  EXPECT_TRUE(line->isVisible());
  EXPECT_GT(geometry->property("paintedRect").toRectF().height(), 0);
  EXPECT_LT(geometry->property("paintedRect").toRectF().height(), 1e-6);
  root->setScale(0);
  EXPECT_FALSE(line->isVisible());
  EXPECT_DOUBLE_EQ(geometry->property("paintedRect").toRectF().height(), 0);
  EXPECT_DOUBLE_EQ(geometry->property("paintedRect").toRectF().y(), 0);
  root->setScale(10000000);
  EXPECT_TRUE(line->isVisible());
  EXPECT_GT(geometry->property("paintedRect").toRectF().height(), 0);
}

TEST_F(QmlSmoke, HnSeparator_FractionalScrollingKeepsLayoutStable) {
  QQmlComponent component(&engine_);
  component.setData(R"(
    import QtQuick
    import QtQuick.Layouts
    import Holonight.Controls
    Item {
      width: 400; height: 300
      Flickable {
        id: flick; objectName: "scroll"; anchors.fill: parent
        contentWidth: width; contentHeight: column.implicitHeight
        ColumnLayout {
          id: column; width: flick.width
          Repeater {
            model: 24
            HnSettingsRow {
              titleText: "Scrolling row"; dividerVisible: true
              Layout.fillWidth: true
            }
          }
          RowLayout {
            Layout.fillWidth: true
            HnSeparator { orientation: Qt.Vertical; Layout.preferredHeight: 40 }
            ColumnLayout {
              Layout.fillWidth: true
              HnSeparator { Layout.fillWidth: true }
              Item { Layout.preferredHeight: 40 }
            }
          }
        }
      }
    }
  )",
                    QUrl());
  ASSERT_TRUE(component.isReady()) << component.errorString().toStdString();
  std::unique_ptr<QObject> object(component.create());
  auto* root = qobject_cast<QQuickItem*>(object.get());
  ASSERT_NE(root, nullptr);
  auto* flick = root->findChild<QQuickItem*>("scroll");
  ASSERT_NE(flick, nullptr);
  QList<QObject*> geometries;
  QList<QQuickItem*> pending{root};
  while (!pending.isEmpty()) {
    auto* item = pending.takeLast();
    if (item->objectName() == QStringLiteral("separatorGeometry")) {
      geometries.append(item);
    }
    pending.append(item->childItems());
  }
  ASSERT_EQ(geometries.size(), 26);
  for (qreal scale : {1.3, -1.3, 0.7, -0.7}) {
    root->setScale(scale);
    for (int pass = 0; pass < 5; ++pass) {
      QCoreApplication::processEvents();
    }
    std::vector<std::unique_ptr<QSignalSpy>> spies;
    QList<QSizeF> sizes;
    for (auto* geometry : geometries) {
      spies.emplace_back(std::make_unique<QSignalSpy>(geometry, SIGNAL(logicalThicknessChanged())));
      auto* slot = qobject_cast<QQuickItem*>(geometry)->parentItem();
      sizes.append(QSizeF(slot->implicitWidth(), slot->implicitHeight()));
    }
    const qreal content_height = flick->property("contentHeight").toReal();
    for (int step = 0; step < 4096; ++step) {
      // Sweep both ways through integer and binary exponent boundaries.
      const int offset = step < 2048 ? step : 4095 - step;
      flick->setProperty("contentY", (offset * 0.731) + 0.13);
      for (qsizetype index = 0; index < geometries.size(); ++index) {
        auto* geometry = geometries[index];
        auto* slot = qobject_cast<QQuickItem*>(geometry)->parentItem();
        EXPECT_EQ(QSizeF(slot->implicitWidth(), slot->implicitHeight()), sizes[index]);
        auto* line = slot->findChild<QQuickItem*>("separatorLine");
        ASSERT_NE(line, nullptr);
        const QRectF bounds = line->mapRectToScene(line->boundingRect());
        const qreal dpr = geometry->property("effectiveDevicePixelRatio").toReal();
        const bool vertical = geometry->property("orientation").toInt() == Qt::Vertical;
        const qreal edge = (vertical ? bounds.left() : bounds.top()) * dpr;
        EXPECT_NEAR(edge, std::round(edge), 1e-6);
        EXPECT_NEAR((vertical ? bounds.width() : bounds.height()) * dpr, 1, 1e-6);
      }
      if (step % 128 == 0) {
        bool heartbeat = false;
        QMetaObject::invokeMethod(flick, [&] { heartbeat = true; }, Qt::QueuedConnection);
        QCoreApplication::processEvents();
        EXPECT_TRUE(heartbeat);
        EXPECT_DOUBLE_EQ(flick->property("contentHeight").toReal(), content_height);
      }
    }
    EXPECT_DOUBLE_EQ(flick->property("contentHeight").toReal(), content_height);
    for (const auto& spy : spies) {
      EXPECT_EQ(spy->count(), 0);
    }
  }
}

TEST_F(QmlSmoke, HnSeparator_ReentrantUpdatesCoalesceAndSurviveReparenting) {
  class QueuedCounter : public QObject {
   public:
    int delivered = 0;
    bool eventFilter(QObject* watched, QEvent* event) override {
      Q_UNUSED(watched);
      if (event->type() == QEvent::MetaCall) {
        ++delivered;
      }
      return false;
    }
  } counter;
  QQmlComponent component(&engine_);
  component.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      id: root; width: 200; height: 200
      property bool mutate: false
      Item { id: other; objectName: "other"; width: 80; height: 80 }
      HnSeparator {
        id: separator; objectName: "separator"; width: 100
        onImplicitHeightChanged: {
          if (root.mutate) {
            root.mutate = false
            for (let i = 0; i < 32; ++i) { root.x = i * 0.13; root.y = i * 0.17 }
            separator.parent = other
          }
        }
      }
    }
  )",
                    QUrl());
  ASSERT_TRUE(component.isReady()) << component.errorString().toStdString();
  std::unique_ptr<QObject> object(component.create());
  auto* separator = object->findChild<QQuickItem*>("separator");
  ASSERT_NE(separator, nullptr);
  auto* geometry = separator->findChild<QObject*>("separatorGeometry");
  ASSERT_NE(geometry, nullptr);
  QCoreApplication::processEvents();
  geometry->installEventFilter(&counter);
  object->setProperty("mutate", true);
  separator->setProperty("thickness", 2);
  QCoreApplication::sendPostedEvents(geometry, QEvent::MetaCall);
  EXPECT_EQ(counter.delivered, 1);
  EXPECT_DOUBLE_EQ(separator->implicitHeight(), 2);
  auto* line = separator->findChild<QQuickItem*>("separatorLine");
  ASSERT_NE(line, nullptr);
  const QRectF bounds = line->mapRectToScene(line->boundingRect());
  EXPECT_NEAR(bounds.height(), 2, 1e-6);
  EXPECT_NEAR(bounds.top(), std::round(separator->mapToScene(QPointF{}).y()), 1e-6);
  for (int pass = 0; pass < 5; ++pass) {
    QCoreApplication::processEvents();
  }
  EXPECT_EQ(counter.delivered, 1);
  separator->setProperty("orientation", Qt::Vertical);
  EXPECT_DOUBLE_EQ(separator->implicitWidth(), 2);
  object->findChild<QQuickItem*>("other")->setScale(2);
  EXPECT_DOUBLE_EQ(separator->implicitWidth(), 1);
  separator->setProperty("orientation", Qt::Horizontal);
  object->setProperty("mutate", true);
  separator->setProperty("thickness", 3);
  const int delivered_before_destruction = counter.delivered;
  object.reset();
  QCoreApplication::processEvents();
  EXPECT_EQ(counter.delivered, delivered_before_destruction);
}

// Keep the exhaustive geometry matrix in a single test scenario.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_F(QmlSmoke, HnSeparator_CompleteRectangleAlignmentAndInvalidGeometry) {
  using Holonight::SeparatorCrossAlignment;
  for (qreal dpr : {1.0, 1.25, 1.5, 1.75, 2.0}) {
    for (qreal scale : {0.5, -0.5, 1.0, -1.0, 1.25, -1.25, 2.0, -2.0}) {
      for (qreal origin : {-12.501, -12.5, -12.499, -0.5, 0.0, 0.499, 0.5, 0.501, 12.5}) {
        for (int stroke : {1, 2, 3}) {
          for (bool vertical : {false, true}) {
            for (auto align : {SeparatorCrossAlignment::Leading, SeparatorCrossAlignment::Center,
                               SeparatorCrossAlignment::Trailing}) {
              SCOPED_TRACE(::testing::Message()
                           << "dpr=" << dpr << " scale=" << scale << " origin=" << origin << " stroke=" << stroke
                           << " vertical=" << vertical << " align=" << static_cast<int>(align));
              const auto rectangle =
                  Holonight::separatorRectangle(QRectF(0, 0, 30.3, 30.3), QPointF(origin / dpr, origin / dpr),
                                                QPointF(scale, scale), dpr, stroke, vertical, align);
              ASSERT_FALSE(rectangle.isEmpty());
              const qreal minor = vertical ? rectangle.width() : rectangle.height();
              EXPECT_NEAR(minor * dpr * std::abs(scale), stroke, 1e-9);
              for (qreal boundary : {rectangle.left(), rectangle.right(), rectangle.top(), rectangle.bottom()}) {
                const qreal physical = origin + (boundary * scale * dpr);
                EXPECT_NEAR(physical, std::floor(physical + 0.5), 1e-9);
              }
              const qreal leading = origin + ((vertical ? rectangle.left() : rectangle.top()) * scale * dpr);
              const qreal trailing = leading + std::copysign(stroke, scale);
              if (align == SeparatorCrossAlignment::Leading) {
                EXPECT_NEAR(leading, std::floor(origin + 0.5), 1e-9);
              }
              if (align == SeparatorCrossAlignment::Trailing) {
                EXPECT_NEAR(trailing, std::floor(origin + (30.3 * scale * dpr) + 0.5), 1e-9);
              }
              if (align == SeparatorCrossAlignment::Center) {
                EXPECT_NEAR(leading,
                            std::floor(origin + (((30.3 * scale * dpr) - std::copysign(stroke, scale)) / 2) + 0.5),
                            1e-9);
              }
            }
          }
        }
      }
    }
  }
  const qreal infinity = std::numeric_limits<qreal>::infinity();
  const qreal nan = std::numeric_limits<qreal>::quiet_NaN();
  for (qreal invalid : {0.0, infinity, -infinity, nan}) {
    EXPECT_TRUE(Holonight::separatorRectangle(QRectF(0, 0, 30, 30), {}, QPointF(invalid, 1), 1.25, 1, false,
                                              SeparatorCrossAlignment::Leading)
                    .isEmpty());
    EXPECT_TRUE(Holonight::separatorRectangle(QRectF(0, 0, 30, 30), {}, QPointF(1, 1), invalid, 1, false,
                                              SeparatorCrossAlignment::Leading)
                    .isEmpty());
  }
  for (int stroke : {0, -1}) {
    EXPECT_TRUE(Holonight::separatorRectangle(QRectF(0, 0, 30, 30), {}, QPointF(1, 1), 1.25, stroke, false,
                                              SeparatorCrossAlignment::Leading)
                    .isEmpty());
  }
  EXPECT_TRUE(Holonight::separatorRectangle(QRectF(0, 0, 30, 30), QPointF(std::numeric_limits<qreal>::max(), 0),
                                            QPointF(1, 1), 2, 1, false, SeparatorCrossAlignment::Leading)
                  .isEmpty());
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

  const Holonight::ColorTokens tok = canonicalDefaultTokens();
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

TEST_F(QmlSmoke, Controls_KeyHintSemanticNamesAndLiteralCompatibility) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnKeyHint {
      text: "literal Ctrl+/ <b>"
      property string spokenName: Accessible.name
      property int accessibleRole: Accessible.role
      property bool decorationIgnored: contentItem.Accessible.ignored
      function semantic() { keyGroups = [[Qt.Key_Control, Qt.Key_Plus], [Qt.Key_Slash]] }
      function symbols() {
        keyGroups = [[Qt.Key_Shift, Qt.Key_Tab, Qt.Key_Backtab, Qt.Key_Return, Qt.Key_Enter,
          Qt.Key_Backspace, Qt.Key_Space, Qt.Key_Delete, Qt.Key_Up, Qt.Key_Down, Qt.Key_Left, Qt.Key_Right]]
      }
      function legacy() { keyGroups = [] }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> hint{comp.create()};
  ASSERT_NE(hint, nullptr);
  EXPECT_EQ(hint->property("spokenName").toString(), QStringLiteral("literal Ctrl+/ <b>"));
  EXPECT_TRUE(hint->property("decorationIgnored").toBool());
  EXPECT_FALSE(hint->property("activeFocusOnTab").toBool());
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "semantic"));
  EXPECT_EQ(hint->property("spokenName").toString(), QStringLiteral("Ctrl plus + or /"));
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "symbols"));
  EXPECT_EQ(hint->property("spokenName").toString(),
            QStringLiteral("Shift plus Tab plus Backtab plus Return plus Enter plus Backspace plus Space plus Delete "
                           "plus Up plus Down plus Left plus Right"));
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "legacy"));
  EXPECT_EQ(hint->property("spokenName").toString(), QStringLiteral("literal Ctrl+/ <b>"));
}

TEST_F(QmlSmoke, Controls_KeyHintWrapsAlternativesThenKeysAndTracksResolvedFont) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnKeyHint {
      id: hint
      keyGroups: [[Qt.Key_Control, Qt.Key_Return], [Qt.Key_Control, Qt.Key_Tab]]
      font.family: "DejaVu Sans Mono"
      font.pointSize: 12
      wrap: true
      property real lineHeight: fm.height
      FontMetrics { id: fm; font: hint.font }
      function textOnly() { keyGroups = [[Qt.Key_Control, Qt.Key_M]] }
      function largeFont() { font.pointSize = 18 }
      function pixelFont() { font = Qt.font({pixelSize: 32, family: "DejaVu Sans"}) }
      function legacy() { keyGroups = []; text = "Ctrl plus Return or Ctrl plus Tab" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> hint{comp.create()};
  ASSERT_NE(hint, nullptr);
  auto* content = hint->property("contentItem").value<QQuickItem*>();
  ASSERT_NE(content, nullptr);
  const double natural_width = hint->property("implicitWidth").toDouble();
  const double line_height = hint->property("lineHeight").toDouble();
  EXPECT_DOUBLE_EQ(content->implicitHeight(), line_height);
  // A full alternative fits, but both alternatives do not.
  hint->setProperty("width", natural_width * 0.65);
  QCoreApplication::processEvents();
  EXPECT_DOUBLE_EQ(content->implicitHeight(), 2 * line_height);
  hint->setProperty("width", natural_width * 0.3);
  QCoreApplication::processEvents();
  EXPECT_GT(content->implicitHeight(), 2 * line_height);
  hint->setProperty("wrap", false);
  EXPECT_DOUBLE_EQ(content->implicitHeight(), line_height);
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "textOnly"));
  const double text_width = content->implicitWidth();
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "largeFont"));
  EXPECT_GT(content->implicitWidth(), text_width);
  EXPECT_GT(content->implicitHeight(), line_height);
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "pixelFont"));
  EXPECT_GT(hint->property("lineHeight").toDouble(), line_height);
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "legacy"));
  hint->setProperty("wrap", true);
  QCoreApplication::processEvents();
  EXPECT_GT(content->implicitHeight(), hint->property("lineHeight").toDouble());
}

// GTest assertion macro expansion dominates the score of this linear state check.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_F(QmlSmoke, Controls_KeyHintDisabledAppearance) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnKeyHint { keyGroups: [[Qt.Key_Control, Qt.Key_Return]] }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> hint{comp.create()};
  ASSERT_NE(hint, nullptr);
  const auto colors = canonicalDefaultTokens();
  auto* content = hint->property("contentItem").value<QQuickItem*>();
  ASSERT_NE(content, nullptr);
  // Every text decoration uses the same state-dependent foreground.
  QList<QObject*> labels;
  for (QObject* child : content->findChildren<QObject*>()) {
    if (child->property("text").isValid() && child->property("color").isValid()) {
      labels.append(child);
    }
  }
  ASSERT_FALSE(labels.empty());
  for (QObject* label : labels) {
    EXPECT_EQ(label->property("color").value<QColor>(), colors.textSecondary);
  }
  hint->setProperty("enabled", false);
  for (QObject* label : labels) {
    EXPECT_EQ(label->property("color").value<QColor>(), colors.textDisabled);
  }
  EXPECT_EQ(hint->property("background").value<QObject*>()->property("color").value<QColor>().alpha(), 0);
  EXPECT_EQ(hint->property("borderColor").value<QColor>(), colors.borderPassive);
}

// GTest assertions dominate the score of this small state matrix.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_F(QmlSmoke, Controls_KeyHintColorsFollowSurfaceAcrossContentAndEnabledChanges) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Rectangle {
      id: surface
      border.color: "#4c90e0"
      property bool literal: false
      property alias hint: hint
      HnKeyHint {
        id: hint
        keyGroups: surface.literal ? [] : [[Qt.Key_Control, Qt.Key_Return], [Qt.Key_Escape]]
        text: "Esc"
        foregroundColor: surface.border.color
        borderColor: surface.border.color
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> surface{comp.create()};
  ASSERT_NE(surface, nullptr);
  auto* hint = surface->property("hint").value<QObject*>();
  ASSERT_NE(hint, nullptr);
  auto* content = hint->property("contentItem").value<QObject*>();
  auto* background = hint->property("background").value<QObject*>();
  ASSERT_NE(content, nullptr);
  ASSERT_NE(background, nullptr);
  for (const auto& color : {QColor("#4c90e0"), QColor("#50d5ed")}) {
    ASSERT_TRUE(QQmlProperty::write(surface.get(), "border.color", color));
    for (bool enabled : {true, false}) {
      hint->setProperty("enabled", enabled);
      for (bool literal : {false, true}) {
        surface->setProperty("literal", literal);
        QCoreApplication::processEvents();
        EXPECT_EQ(hint->property("foregroundColor").value<QColor>(), color);
        EXPECT_EQ(QQmlProperty::read(background, "border.color").value<QColor>(), color);
        EXPECT_EQ(background->property("color").value<QColor>().alpha(), 0);
        int colored_items = 0;
        for (auto* child : content->findChildren<QObject*>()) {
          if (child->property("text").isValid() && child->property("color").isValid()) {
            EXPECT_EQ(child->property("color").value<QColor>(), color);
            ++colored_items;
          }
          if (child->property("strokeColor").isValid()) {
            EXPECT_EQ(child->property("strokeColor").value<QColor>(), color);
            ++colored_items;
          }
        }
        EXPECT_GT(colored_items, 0);
      }
    }
  }
}

TEST_F(QmlSmoke, Controls_KeySequenceLabelMatchesBadgeWithoutFrameOrPadding) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      HnKeyHint {
        id: badge; objectName: "badge"
        keyGroups: [[Qt.Key_Control, Qt.Key_Shift, Qt.Key_Return], [Qt.Key_Tab]]
        font.family: "DejaVu Sans"; font.pointSize: 12
      }
      HnKeySequenceLabel {
        id: label; objectName: "label"
        keyGroups: badge.keyGroups; font: badge.font; color: "#ff123456"
        property string spokenName: Accessible.name
      }
      function pixelFont() { badge.font = Qt.font({pixelSize: 32, family: "DejaVu Sans Mono", bold: true}) }
      function literal() { badge.keyGroups = []; badge.text = "<b>Ctrl+/</b>"; label.text = badge.text }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* badge = root->findChild<QQuickItem*>("badge");
  auto* label = root->findChild<QQuickItem*>("label");
  ASSERT_NE(badge, nullptr);
  ASSERT_NE(label, nullptr);
  EXPECT_FALSE(label->property("background").isValid());
  EXPECT_FALSE(label->property("padding").isValid());
  EXPECT_FALSE(label->activeFocusOnTab());
  EXPECT_EQ(label->property("spokenName"), badge->property("accessibleText"));
  EXPECT_EQ(label->property("spokenName").toString(), QStringLiteral("Ctrl plus Shift plus Return or Tab"));
  const double initial_width = label->implicitWidth();
  EXPECT_DOUBLE_EQ(initial_width, badge->property("implicitContentWidth").toDouble());
  EXPECT_DOUBLE_EQ(label->implicitHeight(), badge->property("implicitContentHeight").toDouble());
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "pixelFont"));
  EXPECT_GT(label->implicitWidth(), initial_width);
  EXPECT_EQ(label->property("font"), badge->property("font"));
  label->setProperty("enabled", false);
  EXPECT_EQ(label->property("color").value<QColor>(), QColor("#123456"));
  label->setProperty("wrap", true);
  label->setWidth(label->implicitWidth() * 0.65);
  EXPECT_GT(label->implicitHeight(), badge->property("implicitContentHeight").toDouble());
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "literal"));
  EXPECT_EQ(label->property("spokenName").toString(), QStringLiteral("<b>Ctrl+/</b>"));
  label->setWidth(label->implicitWidth());
  EXPECT_DOUBLE_EQ(label->implicitHeight(), badge->property("implicitContentHeight").toDouble());
}

// GTest assertion macro expansion dominates the score of this small geometry matrix.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_F(QmlSmoke, Controls_KeyHintCompactGeometryCentersShortKeys) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnKeyHint {
      id: hint
      property int size: 12
      property int key: Qt.Key_R
      keyGroups: [[key]]
      font.family: "DejaVu Sans Mono"; font.pointSize: size
      property real lineHeight: fm.height
      FontMetrics { id: fm; font: hint.font }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> hint{comp.create()};
  ASSERT_NE(hint, nullptr);
  auto* content = hint->property("contentItem").value<QQuickItem*>();
  ASSERT_NE(content, nullptr);
  ASSERT_EQ(content->childItems().size(), 1);
  auto* sequence = content->childItems().front();
  for (int size : {8, 12, 18}) {
    hint->setProperty("size", size);
    for (int key : {Qt::Key_R, Qt::Key_1, Qt::Key_Return}) {
      hint->setProperty("key", key);
      QCoreApplication::processEvents();
      const double line_height = hint->property("lineHeight").toDouble();
      const double height = hint->property("implicitHeight").toDouble();
      EXPECT_NEAR(height, line_height * 26 / 22, 0.001);
      EXPECT_GE(hint->property("implicitWidth").toDouble(), height * 1.2);
      EXPECT_NEAR(sequence->x() + (sequence->width() / 2), content->width() / 2, 0.001);
      EXPECT_NEAR(sequence->y() + (sequence->height() / 2), content->height() / 2, 0.001);
      EXPECT_DOUBLE_EQ(hint->property("background").value<QObject*>()->property("radius").toDouble(), 2);
    }
  }
}

TEST_F(QmlSmoke, Controls_KeyHintPreservesSquareAppearance) {
  EnvGuard appearance_guard{"HOLONIGHT_APPEARANCE_FILE"};
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  auto appearance = HoloNight::Config::defaults();
  appearance.shape.base_radius = 0.0;
  ASSERT_TRUE(HoloNight::Config::writeAtomically(appearance, std::filesystem::path{path.toStdString()}));
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnKeyHint { keyGroups: [[Qt.Key_R]] }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> hint{comp.create()};
  ASSERT_NE(hint, nullptr);
  EXPECT_DOUBLE_EQ(hint->property("background").value<QObject*>()->property("radius").toDouble(), 0);
}

TEST_F(QmlSmoke, IconProviderOptionsKeepLegacyApiAndResolveSelectionOnce) {
  QQmlComponent component{&engine_};
  component.setData(R"(
    import QtQuick
    import Holonight.Core
    Item {
      property string legacy: HnIconProvider.sourceUrl("folder", 24, "red", "blue", "green", "yellow", "magenta", 7, true)
      property string selected: HnIconProvider.sourceUrlWithOptions("folder", 24, {
        color: "red", highlight: "blue", positive: "green", neutral: "yellow", negative: "magenta",
        accent: "#80123456", background: "black", highlightedText: "white", state: 4, dpr: 2
      })
      property string normal: HnIconProvider.sourceUrlWithOptions("folder", 24, {
        color: "red", highlight: "blue", positive: "green", neutral: "yellow", negative: "magenta",
        accent: "#80123456", background: "black", highlightedText: "white", state: 0, dpr: 2
      })
    })",
                    QUrl{});
  ASSERT_EQ(component.status(), QQmlComponent::Ready) << component.errorString().toStdString();
  const std::unique_ptr<QObject> object{component.create()};
  ASSERT_TRUE(object);
  const QUrlQuery legacy{QUrl(object->property("legacy").toString())};
  EXPECT_FALSE(legacy.hasQueryItem("accent"));
  EXPECT_EQ(legacy.queryItemValue("revision"), "7");
  EXPECT_EQ(legacy.queryItemValue("color"), "#ffff0000");
  const QUrlQuery selected{QUrl(object->property("selected").toString())};
  EXPECT_EQ(selected.queryItemValue("color"), "#ffffffff");
  EXPECT_EQ(selected.queryItemValue("highlight"), "#ffffffff");
  EXPECT_EQ(selected.queryItemValue("background"), "#ff0000ff");
  EXPECT_EQ(QColor(selected.queryItemValue("accent")).alpha(), 128);
  EXPECT_NE(object->property("selected"), object->property("normal"));
}

TEST_F(QmlSmoke, Controls_KeyHintLowercasesOnlyLettersWhenOptedIn) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnKeyHint {
      keyGroups: [[Qt.Key_Control, Qt.Key_R], [Qt.Key_Shift, Qt.Key_G], [Qt.Key_Left], [Qt.Key_Plus]]
      property string spokenName: Accessible.name
      function literal() { keyGroups = []; text = "Literal GG" }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> hint{comp.create()};
  ASSERT_NE(hint, nullptr);
  EXPECT_FALSE(hint->property("lowercaseLetters").toBool());
  EXPECT_EQ(hint->property("spokenName").toString(), "Ctrl plus R or Shift plus G or Left or +");
  ASSERT_TRUE(hint->setProperty("lowercaseLetters", true));
  EXPECT_EQ(hint->property("spokenName").toString(), "Ctrl plus r or Shift plus g or Left or +");
  ASSERT_TRUE(QMetaObject::invokeMethod(hint.get(), "literal"));
  EXPECT_EQ(hint->property("spokenName").toString(), "Literal GG");
}

TEST_F(QmlSmoke, Controls_FilePresentationStableGeometryAndSemanticHint) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias frame: frame
      property alias hint: hint
      HnPreviewFrame {
        id: frame
        previewContent: Rectangle { objectName: "content"; implicitWidth: 900; implicitHeight: 700 }
        loadingContent: Rectangle { objectName: "loading"; implicitWidth: 10; implicitHeight: 10 }
        errorContent: Item { objectName: "error" }
      }
      HnLabeledKeyHint {
        id: hint
        keyGroups: [[Qt.Key_Control, Qt.Key_L]]
        description: "Edit location"
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* frame = root->property("frame").value<QObject*>();
  EXPECT_EQ(frame->property("padding").toDouble(), 0);
  EXPECT_EQ(frame->property("background").value<QObject*>(), nullptr);
  const auto width = frame->property("implicitWidth");
  const auto height = frame->property("implicitHeight");
  for (int state = 0; state < 4; ++state) {
    ASSERT_TRUE(frame->setProperty("previewState", state));
    EXPECT_EQ(frame->property("implicitWidth"), width);
    EXPECT_EQ(frame->property("implicitHeight"), height);
    auto* presentation = frame->property("previewItem").value<QObject*>();
    if (state == 2) {
      EXPECT_EQ(presentation, nullptr);
    } else {
      ASSERT_NE(presentation, nullptr);
      EXPECT_EQ(presentation->objectName(), state == 0 ? "content" : state == 1 ? "loading" : "error");
    }
  }
  auto* hint = root->property("hint").value<QObject*>();
  EXPECT_EQ(hint->property("accessibleText").toString(), QStringLiteral("Ctrl plus L Edit location"));
  EXPECT_GT(hint->property("implicitWidth").toDouble(), 0);
}

TEST_F(QmlSmoke, Controls_ThumbnailDoesNotSelectCurrentListItem) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    ListView {
      width: 200; height: 200
      model: 1
      delegate: HnThumbnailDelegate { caption: "Image"; width: 100; height: 100 }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  QCoreApplication::processEvents();
  auto* item = root->property("currentItem").value<QObject*>();
  ASSERT_NE(item, nullptr);
  EXPECT_FALSE(item->property("selected").toBool());
  ASSERT_TRUE(item->setProperty("checked", true));
  EXPECT_TRUE(item->property("selected").toBool());
}

TEST_F(QmlSmoke, Controls_BreadcrumbCollapsesAndMetadataLoads) {
  checkComponent(engine_, R"(
    import QtQuick
    import Holonight.Controls
    HnMetadataList {
      width: 200
      rows: [{heading: "Image"}, {label: "Name", value: "A long name that wraps"}]
    }
  )");
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnBreadcrumbBar {
      width: 200; segments: ["/", "same", "same", "current"]
      function longer() { segments = ["root", "a very long ancestor", "a very long ancestor", "a very long current"] }
      function shorter() { segments = ["/", "a", "b", "c"] }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  EXPECT_TRUE(root->property("collapsed").toBool());
  root->setProperty("width", 1000);
  EXPECT_FALSE(root->property("collapsed").toBool());
  // Fit the measured short labels, even below the sum of maximum segment widths.
  const auto natural_width = root->property("expandedWidth").toDouble();
  ASSERT_GT(natural_width, 0);
  EXPECT_LT(natural_width, 600);
  root->setProperty("width", natural_width + 1);
  EXPECT_FALSE(root->property("collapsed").toBool());
  root->setProperty("width", 350);
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "longer"));
  QCoreApplication::processEvents();
  EXPECT_TRUE(root->property("collapsed").toBool());
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "shorter"));
  QCoreApplication::processEvents();
  EXPECT_FALSE(root->property("collapsed").toBool());
}

namespace {

int expectBreadcrumbGeometry(QQuickItem* bar, QQuickItem* item, qreal buttonHeight) {
  if (!item->isVisible()) {
    return 0;
  }
  const auto name = item->objectName();
  const bool isButton = name.startsWith("hnBreadcrumbSegment") || name == "hnBreadcrumbRoot" ||
                        name == "hnBreadcrumbOverflow" || name == "hnBreadcrumbEdit";
  if (isButton) {
    EXPECT_EQ(item->height(), buttonHeight);
    EXPECT_GE(item->mapToItem(bar, QPointF()).x(), -0.5);
    EXPECT_LE(item->mapToItem(bar, QPointF(item->width(), 0)).x(), bar->width() + 0.5);
    EXPECT_GE(item->mapToItem(bar, QPointF()).y(), -0.5);
    EXPECT_LE(item->mapToItem(bar, QPointF(0, item->height())).y(), bar->height() + 0.5);
    auto* content = item->property("contentItem").value<QQuickItem*>();
    EXPECT_NE(content, nullptr);
    if (content != nullptr) {
      EXPECT_NEAR(content->mapToItem(bar, QPointF(0, content->height() / 2)).y(), bar->height() / 2, 0.5);
    }
  }
  if (item->property("rawText").toString() == QString::fromUtf8("›")) {
    EXPECT_NEAR(item->mapToItem(bar, QPointF(0, item->height() / 2)).y(), bar->height() / 2, 0.5);
  }
  int buttons = isButton ? 1 : 0;
  for (auto* child : item->childItems()) {
    buttons += expectBreadcrumbGeometry(bar, child, buttonHeight);
  }
  return buttons;
}

}  // namespace

TEST_F(QmlSmoke, Controls_BreadcrumbButtonsStayInsideField) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      width: 800; height: 80; visible: true
      property alias bar: bar
      HnBreadcrumbBar {
        id: bar; width: 800
        segments: ["/", "home", "andrii", "Pictures"]
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* bar = root->property("bar").value<QQuickItem*>();
  ASSERT_NE(bar, nullptr);
  EXPECT_EQ(bar->property("sizeRole").toInt(), 1);
  EXPECT_EQ(bar->implicitHeight(), 32);
  for (int sizing = 0; sizing < 3; ++sizing) {
    SCOPED_TRACE(sizing);
    bar->setProperty("sizeRole", sizing == 1 ? 4 : 1);
    if (sizing == 2) {
      bar->setHeight(24);
    }
    for (bool editable : {true, false}) {
      bar->setProperty("editable", editable);
      for (int width : {800, 200}) {
        SCOPED_TRACE(width);
        SCOPED_TRACE(editable);
        bar->setWidth(width);
        QTest::qWait(20);
        EXPECT_EQ(bar->height(), sizing == 0 ? 32 : 24);
        EXPECT_EQ(bar->property("collapsed").toBool(), width == 200);
        EXPECT_GE(expectBreadcrumbGeometry(bar, bar, sizing == 0 ? 28 : 24), 2);
      }
    }
  }
}

TEST_F(QmlSmoke, Controls_BreadcrumbProgressivelyRestoresAncestors) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      width: 1000; height: 80; visible: true
      property alias bar: bar
      property int activated: -1
      HnBreadcrumbBar {
        id: bar; width: 1000
        segments: ["/", "same", "same", "Projects", "holonight", "references"]
        onSegmentActivated: index => activated = index
      }
      function triggerAncestor(index) { bar.overflowMenu.itemAt(index).triggered() }
      function ancestorLabel(index) { return bar.overflowMenu.itemAt(index).text }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* bar = root->property("bar").value<QQuickItem*>();
  ASSERT_NE(bar, nullptr);
  auto* menu = bar->property("overflowMenu").value<QObject*>();
  ASSERT_NE(menu, nullptr);
  // Walk every pixel in both directions, observing menu membership and containment.
  for (bool editable : {false, true}) {
    bar->setProperty("editable", editable);
    int previousHidden = 4;
    bool sawPartial = false;
    for (int width = 40; width <= 800; ++width) {
      bar->setWidth(width);
      QCoreApplication::processEvents();
      const int hidden = menu->property("count").toInt();
      EXPECT_LE(hidden, previousHidden);
      if (hidden > 0 && hidden < 4) sawPartial = true;
      EXPECT_EQ(bar->property("collapsed").toBool(), hidden > 0);
      if (hidden != previousHidden && hidden > 0) {
        for (int index = 0; index < hidden; ++index) {
          ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "triggerAncestor", Q_ARG(QVariant, index)));
          EXPECT_EQ(root->property("activated").toInt(), index + 1);
          QVariant label;
          ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "ancestorLabel", Q_RETURN_ARG(QVariant, label),
                                                Q_ARG(QVariant, index)));
          EXPECT_EQ(label.toString(), QStringList({"same", "same", "Projects", "holonight"}).at(index));
        }
      }
      previousHidden = hidden;
    }
    EXPECT_TRUE(sawPartial);
    EXPECT_EQ(previousHidden, 0);
    for (int width = 800; width >= 0; width -= 10) {
      bar->setWidth(width);
      QCoreApplication::processEvents();
      const int hidden = menu->property("count").toInt();
      EXPECT_GE(hidden, previousHidden);
      previousHidden = hidden;
      QTest::qWait(20);
      expectBreadcrumbGeometry(bar, bar, 28);
    }
    // The complete path must fit at its measured boundary, and collapse just below it.
    bar->setWidth(bar->property("expandedWidth").toReal());
    QCoreApplication::processEvents();
    EXPECT_FALSE(bar->property("collapsed").toBool());
    bar->setWidth(bar->width() - 0.01);
    QCoreApplication::processEvents();
    EXPECT_TRUE(bar->property("collapsed").toBool());
  }
  bar->setWidth(800);
  QCoreApplication::processEvents();
  const qreal originalWidth = bar->property("expandedWidth").toReal();
  auto font = bar->property("font").value<QFont>();
  font.setPointSizeF(font.pointSizeF() * 1.5);
  bar->setProperty("font", font);
  QCoreApplication::processEvents();
  EXPECT_GT(bar->property("expandedWidth").toReal(), originalWidth);
  bar->setProperty("segmentWidth", 40);
  QCoreApplication::processEvents();
  EXPECT_LT(bar->property("expandedWidth").toReal(), originalWidth);
  for (const auto& labels :
       {QStringList{}, QStringList{QString(300, 'a')}, QStringList{QString(300, 'a'), QString(300, 'b')}}) {
    bar->setProperty("segments", labels);
    bar->setWidth(80);
    QTest::qWait(20);
    EXPECT_FALSE(bar->property("collapsed").toBool());
    expectBreadcrumbGeometry(bar, bar, 28);
  }
}

TEST_F(QmlSmoke, Controls_BreadcrumbNaturalLayoutAndEditing) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      id: window
      width: 800; height: 100; visible: true
      property int edits: 0
      property int ancestor: -1
      property alias bar: bar
      HnBreadcrumbBar {
        id: bar; width: 800; segments: ["/", "same", "same", "Pictures"]
        onLocationEditRequested: window.edits++
        onSegmentActivated: index => window.ancestor = index
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(root.get());
  auto* bar = root->property("bar").value<QQuickItem*>();
  ASSERT_NE(bar, nullptr);
  window->show();
  QTest::qWait(50);
  const auto findItem = [](auto&& self, QQuickItem* item, const QString& name) -> QQuickItem* {
    if (item->objectName() == name) return item;
    for (auto* child : item->childItems()) {
      if (auto* found = self(self, child, name)) return found;
    }
    return nullptr;
  };
  auto* first = findItem(findItem, bar, "hnBreadcrumbSegment0");
  auto* second = findItem(findItem, bar, "hnBreadcrumbSegment1");
  auto* current = findItem(findItem, bar, "hnBreadcrumbSegment3");
  auto* edit = findItem(findItem, bar, "hnBreadcrumbEdit");
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  ASSERT_NE(current, nullptr);
  ASSERT_NE(edit, nullptr);
  for (int index = 1; index < 4; ++index) {
    auto* previous = findItem(findItem, bar, QString("hnBreadcrumbSegment%1").arg(index - 1));
    auto* next = findItem(findItem, bar, QString("hnBreadcrumbSegment%1").arg(index));
    ASSERT_NE(previous, nullptr);
    ASSERT_NE(next, nullptr);
    EXPECT_GT(next->mapToItem(bar, QPointF{}).x(), previous->mapToItem(bar, QPointF{}).x() + previous->width());
  }
  const auto expectCentered = [bar](QQuickItem* button) {
    auto* content = button->property("contentItem").value<QQuickItem*>();
    ASSERT_NE(content, nullptr);
    EXPECT_NEAR(content->mapToItem(bar, QPointF(0, content->height() / 2)).y(), bar->height() / 2, 0.5);
  };
  expectCentered(first);
  expectCentered(current);
  expectCentered(edit);
  bar->setWidth(200);
  QTest::qWait(20);
  EXPECT_TRUE(bar->property("collapsed").toBool());
  expectCentered(current);
  expectCentered(edit);
  bar->setWidth(800);
  QTest::qWait(20);
  EXPECT_LT(current->width(), 150);
  EXPECT_GT(second->mapToItem(bar, QPointF{}).x(), first->mapToItem(bar, QPointF{}).x() + first->width());
  EXPECT_GT(edit->mapToItem(bar, QPointF{}).x(), current->mapToItem(bar, QPointF{}).x() + current->width() + 100);
  EXPECT_FALSE(current->property("font").value<QFont>().bold());
  ASSERT_TRUE(QMetaObject::invokeMethod(second, "clicked"));
  EXPECT_EQ(root->property("ancestor").toInt(), 1);
  ASSERT_TRUE(QMetaObject::invokeMethod(edit, "clicked"));
  EXPECT_EQ(root->property("edits").toInt(), 1);
  current->forceActiveFocus();
  QTest::keyClick(window, Qt::Key_L, Qt::ControlModifier);
  EXPECT_EQ(root->property("edits").toInt(), 2);
  bar->setProperty("editable", false);
  EXPECT_FALSE(edit->isVisible());
  QTest::keyClick(window, Qt::Key_L, Qt::ControlModifier);
  EXPECT_EQ(root->property("edits").toInt(), 2);
  bar->setProperty("segments", QStringList{});
  QCoreApplication::processEvents();
  EXPECT_FALSE(bar->property("collapsed").toBool());
  bar->setProperty("segments", QStringList{QString(300, 'a')});
  bar->setWidth(180);
  QTest::qWait(20);
  EXPECT_FALSE(bar->property("collapsed").toBool());
  auto* single = findItem(findItem, bar, "hnBreadcrumbSegment0");
  ASSERT_NE(single, nullptr);
  EXPECT_LE(single->width(), 150);
}

TEST_F(QmlSmoke, Controls_ThumbnailAndBreadcrumbActivationRetainCallerIndices) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Item {
      property alias thumbnail: thumbnail
      property alias breadcrumbs: breadcrumbs
      property int activations: 0
      property int ancestor: -1
      function activateAncestor() { breadcrumbs.overflowMenu.itemAt(1).triggered() }
      HnThumbnailDelegate { id: thumbnail; caption: "Image"; onActivated: parent.activations++ }
      HnBreadcrumbBar {
        id: breadcrumbs; width: 200; segments: ["/", "same", "same", "current"]
        onSegmentActivated: index => parent.ancestor = index
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* thumbnail = root->property("thumbnail").value<QObject*>();
  ASSERT_TRUE(QMetaObject::invokeMethod(thumbnail, "clicked"));
  EXPECT_EQ(root->property("activations").toInt(), 1);
  QCoreApplication::processEvents();
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "activateAncestor"));
  EXPECT_EQ(root->property("ancestor").toInt(), 2);
}

TEST_F(QmlSmoke, Controls_IconButtonThemeNameYieldsToExplicitSource) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    HnIconButton {
      icon.name: "window-close"
      property bool explicitSource: false
      icon.source: explicitSource ? "qrc:/qt/qml/Holonight/Controls/assets/edit.svg" : ""
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* icon = root->findChild<QObject*>("hnIconButtonIcon");
  ASSERT_NE(icon, nullptr);
  EXPECT_EQ(icon->property("name").toString(), "window-close");
  root->setProperty("explicitSource", true);
  EXPECT_TRUE(icon->property("name").toString().isEmpty());
  EXPECT_FALSE(icon->property("source").toUrl().isEmpty());
  EXPECT_FALSE(icon->property("hasError").toBool());
}

TEST_F(QmlSmoke, Controls_HeaderCloseButtonPreservesCompactActionAndKeyboardContract) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import QtQuick.Controls as C
    import Holonight.Core
    import Holonight.Controls
    HnHeaderCloseButton {
      property int activations: 0
      property string accessibleName: Accessible.name
      property bool allowed: true
      property bool compactIcon: icon.width === 16 && icon.height === 16
      property bool compact: width === HnMetrics.controlHeight(HnControlSize.Compact)
          && height === HnMetrics.controlHeight(HnControlSize.Compact)
      property color expectedForeground: enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
      property color foreground: icon.color
      action: C.Action { enabled: allowed; onTriggered: activations++ }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* item = qobject_cast<QQuickItem*>(root.get());
  ASSERT_NE(item, nullptr);
  QQuickWindow window;
  item->setParentItem(window.contentItem());
  window.show();
  item->forceActiveFocus(Qt::TabFocusReason);
  EXPECT_TRUE(root->property("compact").toBool());
  EXPECT_TRUE(root->property("compactIcon").toBool());
  EXPECT_EQ(root->property("accessibleName").toString(), "Close");
  EXPECT_EQ(root->property("foreground").value<QColor>(), root->property("expectedForeground").value<QColor>());
  for (auto key : {Qt::Key_Return, Qt::Key_Enter, Qt::Key_Space}) {
    QTest::keyClick(&window, key);
  }
  EXPECT_EQ(root->property("activations").toInt(), 3);
  root->setProperty("allowed", false);
  QTest::keyClick(&window, Qt::Key_Return);
  QTest::keyClick(&window, Qt::Key_Space);
  EXPECT_EQ(root->property("activations").toInt(), 3);
  EXPECT_FALSE(item->isEnabled());
  EXPECT_EQ(root->property("foreground").value<QColor>(), root->property("expectedForeground").value<QColor>());
  item->setParentItem(nullptr);
}

TEST_F(QmlSmoke, Controls_BreadcrumbFocusCurrentSegment) {
  QQmlComponent comp{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight.Controls
    Window {
      id: window
      width: 800; height: 100; visible: true
      property alias bar: bar
      property alias other: other
      property int activations: 0
      Item { id: other }
      HnBreadcrumbBar {
        id: bar; width: 800; editable: false
        segments: ["/", "same", "same", "Pictures"]
        onSegmentActivated: window.activations++
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> root{comp.create()};
  ASSERT_NE(root, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(root.get());
  auto* bar = root->property("bar").value<QQuickItem*>();
  auto* other = root->property("other").value<QQuickItem*>();
  ASSERT_NE(window, nullptr);
  ASSERT_NE(bar, nullptr);
  ASSERT_NE(other, nullptr);
  window->requestActivate();
  ASSERT_TRUE(QTest::qWaitForWindowActive(window));
  const auto focusCurrent = [&] {
    bool result = false;
    EXPECT_TRUE(QMetaObject::invokeMethod(bar, "focusCurrentSegment", Q_RETURN_ARG(bool, result)));
    return result;
  };
  ASSERT_TRUE(focusCurrent());
  auto* initial = window->activeFocusItem();
  QTest::keyClick(window, Qt::Key_H);
  EXPECT_EQ(window->activeFocusItem(), initial);  // opt-in only
  bar->setProperty("vimNavigation", true);
  for (const int width : {800, 160}) {
    bar->setWidth(width);
    QTest::qWait(20);
    other->forceActiveFocus();
    ASSERT_TRUE(focusCurrent());
    EXPECT_EQ(window->activeFocusItem()->objectName(), "hnBreadcrumbSegment3");
    EXPECT_TRUE(window->activeFocusItem()->property("visualFocus").toBool());
    EXPECT_EQ(root->property("activations").toInt(), 0);
    const auto first = bar->property("collapsed").toBool() ? bar->property("firstVisibleAncestor").toInt() : 0;
    QStringList reverseRoute;
    for (int index = 3; index >= first; --index) {
      reverseRoute.append(QString("hnBreadcrumbSegment%1").arg(index));
    }
    if (bar->property("collapsed").toBool()) {
      reverseRoute.append("hnBreadcrumbOverflow");
      reverseRoute.append("hnBreadcrumbRoot");
    }
    for (const auto& name : reverseRoute.mid(1)) {
      QTest::keyClick(window, Qt::Key_H);
      EXPECT_EQ(window->activeFocusItem()->objectName(), name);
      EXPECT_TRUE(window->activeFocusItem()->property("visualFocus").toBool());
    }
    auto* leftmost = window->activeFocusItem();
    QTest::keyClick(window, Qt::Key_H);
    EXPECT_EQ(window->activeFocusItem(), leftmost);
    for (auto index = reverseRoute.size() - 2; index >= 0; --index) {
      QTest::keyClick(window, Qt::Key_L);
      EXPECT_EQ(window->activeFocusItem()->objectName(), reverseRoute[index]);
    }
    auto* rightmost = window->activeFocusItem();
    QTest::keyClick(window, Qt::Key_L);
    EXPECT_EQ(window->activeFocusItem(), rightmost);
    QTest::keyClick(window, Qt::Key_H, Qt::ControlModifier);
    QTest::keyClick(window, Qt::Key_H, Qt::ShiftModifier);
    EXPECT_EQ(window->activeFocusItem(), rightmost);
    EXPECT_EQ(root->property("activations").toInt(), 0);
  }
  EXPECT_TRUE(bar->property("collapsed").toBool());
  bar->setProperty("segments", QStringList{"/"});
  QTest::qWait(20);
  ASSERT_TRUE(focusCurrent());
  EXPECT_EQ(window->activeFocusItem()->objectName(), "hnBreadcrumbSegment0");
  EXPECT_TRUE(window->activeFocusItem()->property("visualFocus").toBool());
  auto* onlySegment = window->activeFocusItem();
  QTest::keyClick(window, Qt::Key_H);
  QTest::keyClick(window, Qt::Key_L);
  EXPECT_EQ(window->activeFocusItem(), onlySegment);
  for (const auto* property : {"enabled", "visible"}) {
    other->forceActiveFocus();
    bar->setProperty(property, false);
    EXPECT_FALSE(focusCurrent());
    EXPECT_TRUE(other->hasActiveFocus());
    bar->setProperty(property, true);
  }
  other->forceActiveFocus();
  bar->setWidth(0);
  QTest::qWait(20);
  EXPECT_FALSE(focusCurrent());
  EXPECT_TRUE(other->hasActiveFocus());
  bar->setWidth(800);
  bar->setProperty("segments", QStringList{});
  EXPECT_FALSE(focusCurrent());
  EXPECT_TRUE(other->hasActiveFocus());
  EXPECT_EQ(root->property("activations").toInt(), 0);
}
