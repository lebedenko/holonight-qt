// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QString>
#include <QTemporaryDir>
#include <QVariant>

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

TEST_F(QmlSmoke, LowercaseCompatibilityModule_LoadsWithoutError) {
  checkComponent(engine_, R"(import holonight; Button { text: "test" })");
}

TEST_F(QmlSmoke, ScrollBar_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; ScrollBar {})"); }

TEST_F(QmlSmoke, TextField_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; TextField { placeholderText: "hint" })");
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
    ComboBox {
      model: ["item1", "item2", "item3"]
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  object->setProperty("opened", true);
}

TEST_F(QmlSmoke, Slider_LoadsWithoutError) { checkComponent(engine_, R"(import Holonight; Slider {})"); }

TEST_F(QmlSmoke, Switch_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; Switch { text: "toggle" })");
}

TEST_F(QmlSmoke, ProgressBar_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ProgressBar { value: 0.5 })");
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

TEST_F(QmlSmoke, ToolTip_LoadsWithoutError) {
  checkComponent(engine_, R"(import Holonight; ToolTip { text: "hint" })");
}

TEST_F(QmlSmoke, HoloniightPalette_PrimaryIsValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item { property color c: HoloniightPalette.primary }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HoloniightPalette_DesignSystemAccentsAreValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
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
    import Holonight
    Item {
      property color background: HoloniightPalette.background
      property color surfaceElevated: HoloniightPalette.surfaceElevated
      property color surfaceRaised: HoloniightPalette.surfaceRaised
      property color textPrimary: HoloniightPalette.textPrimary
      property color textSecondary: HoloniightPalette.textSecondary
      property color textMuted: HoloniightPalette.textMuted
      property color textDisabled: HoloniightPalette.textDisabled
      property color borderSubtle: HoloniightPalette.borderSubtle
      property color borderStrong: HoloniightPalette.borderStrong
      property color hoverOverlay: HoloniightPalette.hoverOverlay
      property int radiusControl: HoloniightPalette.radiusControl
      property int focusBorderWidth: HoloniightPalette.focusBorderWidth
      property color ansiBlue: HoloniightPalette.ansiBlue
      property color ansiBrightWhite: HoloniightPalette.ansiBrightWhite
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HoloniightPalette_DeprecatedAliasesRemainAvailable) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property bool surfaceAlias: HoloniightPalette.surfaceVariant === HoloniightPalette.surface
      property bool textAlias: HoloniightPalette.onSurface === HoloniightPalette.textPrimary
      property bool disabledAlias: HoloniightPalette.onSurfaceDisabled === HoloniightPalette.textDisabled
      property bool outlineAlias: HoloniightPalette.outlineVariant === HoloniightPalette.borderPassive
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_TRUE(object->property("surfaceAlias").toBool());
  EXPECT_TRUE(object->property("textAlias").toBool());
  EXPECT_TRUE(object->property("disabledAlias").toBool());
  EXPECT_TRUE(object->property("outlineAlias").toBool());
}

TEST_F(QmlSmoke, HoloniightPalette_ReloadEmitsNotificationOnChange) {
  // Force singleton creation in dark mode first.
  {
    QQmlComponent init = QQmlComponent{&engine_};
    init.setData(R"(import QtQuick; import Holonight; Item { property color c: HoloniightPalette.primary })", QUrl{});
    std::unique_ptr<QObject> tmp{init.create()};
    ASSERT_NE(tmp, nullptr);
  }

  // Switching to light forces a token change → signal must fire.
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qputenv("HOLONIGHT_APPEARANCE_MODE", "light");

  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
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
    import Holonight
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
  EXPECT_EQ(object->property("background").value<QColor>(), QColor(QStringLiteral("#F3F7FB")));
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
    import Holonight
    Item {
      property color background: "transparent"
      property color primary: "transparent"
      Component.onCompleted: {
        HoloniightPalette.reload()
        background = HoloniightPalette.background
        primary = HoloniightPalette.primary
      }
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
  std::unique_ptr<QObject> object{comp.create()};
  ASSERT_NE(object, nullptr);
  EXPECT_EQ(object->property("background").value<QColor>(), QColor(QStringLiteral("#F3F7FB")));
  EXPECT_EQ(object->property("primary").value<QColor>(), QColor(QStringLiteral("#7aa2f7")));
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
    import Holonight
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
  EXPECT_TRUE(waitForPropertyColor(object.get(), "background", QColor(QStringLiteral("#F3F7FB"))));
}

TEST_F(QmlSmoke, HolonightTheme_ConfigPropertiesAreValid) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
    Item {
      property string iconTheme: HolonightTheme.iconTheme
      property string uiFont: HolonightTheme.uiFont
      property int bodySize: HolonightTheme.bodySize
      property int captionSize: HolonightTheme.captionSize
    }
  )",
               QUrl{});
  ASSERT_EQ(comp.status(), QQmlComponent::Ready) << comp.errorString().toStdString();
}

TEST_F(QmlSmoke, HolonightTheme_CatalogPropertiesAreReadable) {
  QQmlComponent comp = QQmlComponent{&engine_};
  comp.setData(R"(
    import QtQuick
    import Holonight
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
  EXPECT_EQ(object->property("familyCount").toInt(), 3);
  EXPECT_EQ(object->property("variantCount").toInt(), 6);
  EXPECT_EQ(object->property("firstAccentId").toString(), QStringLiteral("default"));
  EXPECT_TRUE(object->property("firstAccentColor").value<QColor>().isValid());
}
