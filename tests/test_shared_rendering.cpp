// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QFile>
#include <QIcon>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTemporaryDir>
#include <QTest>
#include <QtQuick/private/qquickimage_p.h>

#include <gtest/gtest.h>
#include <memory>

namespace {
class SharedRendering : public testing::Test {
 protected:
  QQmlEngine engine;
  QTemporaryDir icons;
  QString old_theme = QIcon::themeName();
  QStringList old_paths = QIcon::themeSearchPaths();
  std::unique_ptr<QObject> root;
  QQuickWindow* window = nullptr;

  void SetUp() override {
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
    ASSERT_TRUE(icons.isValid());
    ASSERT_TRUE(QDir(icons.path()).mkpath("uqc207/16x16/actions"));
    QFile index(icons.filePath("uqc207/index.theme"));
    ASSERT_TRUE(index.open(QIODevice::WriteOnly));
    index.write(
        "[Icon Theme]\nName=uqc207\nDirectories=16x16/actions\n"
        "[16x16/actions]\nSize=16\nType=Fixed\nContext=Actions\n");
    index.close();
    QFile svg(icons.filePath("uqc207/16x16/actions/uqc207-configure.svg"));
    ASSERT_TRUE(svg.open(QIODevice::WriteOnly));
    svg.write(
        "<svg xmlns='http://www.w3.org/2000/svg' width='16' height='16'>"
        "<path fill='#ffffff' d='M2 2h12v12H2z'/></svg>");
    svg.close();
    QIcon::setThemeSearchPaths({icons.path()});
    QIcon::setThemeName("uqc207");
  }
  void TearDown() override {
    root.reset();
    QIcon::setThemeName(old_theme);
    QIcon::setThemeSearchPaths(old_paths);
  }
  void create(const QByteArray& body) {
    QQmlComponent component(&engine);
    component.setData(
        "import QtQuick\nimport QtQuick.Controls as C\nimport QtQuick.Layouts\n"
        "Window { width: 480; height: 400; visible: true; " +
            body + "}",
        QUrl());
    root.reset(component.create());
    ASSERT_TRUE(root) << component.errorString().toStdString();
    window = qobject_cast<QQuickWindow*>(root.get());
    ASSERT_TRUE(window);
    ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
  }
  QQuickItem* visualItem(QQuickItem* parent, const char* name) {
    if (parent->objectName() == QLatin1String(name)) return parent;
    for (auto* child : parent->childItems())
      if (auto* found = visualItem(child, name)) return found;
    return nullptr;
  }
  QQuickItem* item(const char* name) { return visualItem(window->contentItem(), name); }
  bool hasRenderedIcon(QQuickItem* parent) {
    if (auto* image = qobject_cast<QQuickImage*>(parent))
      if (image->isVisible() && image->status() == QQuickImage::Ready) return true;
    for (auto* child : parent->childItems())
      if (hasRenderedIcon(child)) return true;
    return false;
  }
  QColor backgroundPixel(QQuickItem* item) {
    const auto point = item->mapToScene(QPointF(4, item->height() / 2));
    const auto image = window->grabWindow();
    return image.pixelColor(qRound(point.x() * window->devicePixelRatio()),
                            qRound(point.y() * window->devicePixelRatio()));
  }
};

TEST_F(SharedRendering, ActionNamedAndUrlIconsRenderInMenuAndNavigation) {
  create(R"(
    property string iconName: "uqc207-configure"
    property url iconUrl: ""
    C.Action { id: action; text: "Configure"; icon.name: iconName; icon.source: iconUrl }
    C.ItemDelegate { objectName: "navigation"; action: action; width: 220 }
    C.MenuItem { objectName: "entry"; y: 50; action: action; width: 220 }
  )");
  ASSERT_TRUE(root);
  for (const char* name : {"navigation", "entry"}) {
    ASSERT_TRUE(item(name));
    EXPECT_TRUE(QTest::qWaitFor([&] { return hasRenderedIcon(item(name)); })) << name;
  }
  root->setProperty("iconName", "");
  root->setProperty("iconUrl", QUrl::fromLocalFile(icons.filePath("uqc207/16x16/actions/uqc207-configure.svg")));
  for (const char* name : {"navigation", "entry"})
    EXPECT_TRUE(QTest::qWaitFor([&] { return hasRenderedIcon(item(name)); })) << name;
}

TEST_F(SharedRendering, ListCurrentRowDoesNotImplySelectionWithReplacedContent) {
  create(R"(
    ListView {
      id: list; objectName: "list"; width: 300; height: 250; model: 3; reuseItems: true
      delegate: C.ItemDelegate {
        required property int index
        objectName: "row" + index; width: list.width; height: 45
        contentItem: RowLayout {
          C.Label { text: "Shortcut"; Layout.fillWidth: true }
          C.Button { text: "Assign"; activeFocusOnTab: true }
        }
      }
    }
  )");
  ASSERT_TRUE(root);
  ASSERT_TRUE(QTest::qWaitFor([&] { return item("row0") && item("row1"); }));
  window->contentItem()->forceActiveFocus(Qt::OtherFocusReason);
  QTest::mouseMove(window, QPoint(450, 350));
  EXPECT_EQ(backgroundPixel(item("row0")), backgroundPixel(item("row1")));
  item("list")->setProperty("currentIndex", 1);
  EXPECT_EQ(backgroundPixel(item("row0")), backgroundPixel(item("row1")));
  item("row1")->setProperty("highlighted", true);
  EXPECT_TRUE(QTest::qWaitFor([&] { return backgroundPixel(item("row0")) != backgroundPixel(item("row1")); }));
  item("row1")->setProperty("highlighted", false);
  EXPECT_TRUE(QTest::qWaitFor([&] { return backgroundPixel(item("row0")) == backgroundPixel(item("row1")); }));
  item("row1")->setProperty("checkable", true);
  item("row1")->setProperty("checked", true);
  if (qEnvironmentVariable("QT_QUICK_CONTROLS_STYLE") == "Holonight")
    EXPECT_TRUE(QTest::qWaitFor([&] { return backgroundPixel(item("row0")) != backgroundPixel(item("row1")); }));
  item("row1")->forceActiveFocus(Qt::TabFocusReason);
  EXPECT_TRUE(item("row1")->property("visualFocus").toBool());
  QTest::keyClick(window, Qt::Key_Space);
  EXPECT_FALSE(item("row1")->property("checked").toBool());
}

TEST_F(SharedRendering, MenuIconColumnTracksVisibleItemsAndMirroring) {
  if (qEnvironmentVariable("QT_QUICK_CONTROLS_STYLE") != "Holonight")
    GTEST_SKIP() << "Conditional menu spacing is the HoloNight convention.";
  create(R"(
    property bool mirror: false
    property string iconName: ""
    property bool showIcon: true
    C.Menu {
      id: menu; objectName: "menu"; width: 260
      C.MenuItem { objectName: "plain"; text: "Plain"; LayoutMirroring.enabled: mirror }
      C.MenuItem { objectName: "icon"; text: "Icon"; icon.name: iconName; visible: showIcon;
                   LayoutMirroring.enabled: mirror }
    }
    Component { id: entry; C.MenuItem { text: "Inserted"; icon.name: "uqc207-configure" } }
    property var inserted: null
    function insertIcon() { inserted = entry.createObject(menu.contentItem); menu.insertItem(0, inserted) }
    function removeIcon() { menu.removeItem(inserted); inserted.destroy(); inserted = null }
    Component.onCompleted: menu.open()
  )");
  ASSERT_TRUE(root);
  auto* label = root->findChild<QQuickItem*>("hnMenuItemLabel");
  ASSERT_TRUE(label);
  auto leading = [&] { return label->mapToItem(item("plain"), QPointF()).x(); };
  QCoreApplication::processEvents();
  const auto without_icon = leading();
  root->setProperty("iconName", "uqc207-configure");
  EXPECT_TRUE(QTest::qWaitFor([&] { return leading() > without_icon; }));
  const auto with_icon = leading();
  root->setProperty("showIcon", false);
  EXPECT_TRUE(QTest::qWaitFor([&] { return leading() == without_icon; }));
  root->setProperty("showIcon", true);
  EXPECT_TRUE(QTest::qWaitFor([&] { return leading() == with_icon; }));
  root->setProperty("iconName", "");
  EXPECT_TRUE(QTest::qWaitFor([&] { return leading() == without_icon; }));
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "insertIcon"));
  EXPECT_TRUE(QTest::qWaitFor([&] { return leading() == with_icon; }));
  ASSERT_TRUE(QMetaObject::invokeMethod(root.get(), "removeIcon"));
  EXPECT_TRUE(QTest::qWaitFor([&] { return leading() == without_icon; }));
  root->setProperty("mirror", true);
  EXPECT_TRUE(
      QTest::qWaitFor([&] { return qAbs(item("plain")->width() - leading() - label->width() - without_icon) < 0.1; }));
  root->setProperty("iconName", "uqc207-configure");
  EXPECT_TRUE(
      QTest::qWaitFor([&] { return qAbs(item("plain")->width() - leading() - label->width() - with_icon) < 0.1; }));
}

TEST_F(SharedRendering, MenuKeepsCheckActivationAndMirroredSubmenuArrow) {
  create(R"(
    property bool mirror: false
    C.Menu {
      id: menu; width: 260
      delegate: C.MenuItem { LayoutMirroring.enabled: mirror }
      C.MenuItem { objectName: "check"; text: "Check"; checkable: true; LayoutMirroring.enabled: mirror }
      C.Menu { title: "Submenu"; C.MenuItem { text: "Child" } }
    }
    property var submenuEntry: menu.itemAt(1)
    Component.onCompleted: menu.open()
  )");
  ASSERT_TRUE(root);
  auto* check = item("check");
  ASSERT_TRUE(check);
  check->forceActiveFocus(Qt::TabFocusReason);
  QTest::keyClick(window, Qt::Key_Space);
  EXPECT_TRUE(check->property("checked").toBool());
  auto* entry = root->property("submenuEntry").value<QQuickItem*>();
  ASSERT_TRUE(entry);
  auto* arrow = entry->property("arrow").value<QQuickItem*>();
  ASSERT_TRUE(arrow);
  EXPECT_GT(arrow->x(), entry->width() / 2);
  root->setProperty("mirror", true);
  EXPECT_TRUE(QTest::qWaitFor([&] { return arrow->x() < entry->width() / 2; }));
}

TEST_F(SharedRendering, KeyboardButtonActivationRetainsOwnerReasonAndFeedback) {
  create(R"(
    property int activations: 0
    Column {
      C.Button { objectName: "before"; text: "Before" }
      C.Button { objectName: "button"; text: "Test connection"; onClicked: ++activations }
      C.Button { objectName: "after"; text: "After" }
    }
  )");
  ASSERT_TRUE(root);
  item("before")->forceActiveFocus(Qt::TabFocusReason);
  QTest::keyClick(window, Qt::Key_Tab);
  ASSERT_EQ(window->activeFocusItem(), item("button"));
  EXPECT_EQ(item("button")->property("focusReason").toInt(), Qt::TabFocusReason);
  ASSERT_TRUE(item("button")->property("visualFocus").toBool());
  QTest::keyClick(window, Qt::Key_Space);
  EXPECT_EQ(root->property("activations").toInt(), 1);
  EXPECT_EQ(window->activeFocusItem(), item("button"));
  EXPECT_EQ(item("button")->property("focusReason").toInt(), Qt::TabFocusReason);
  EXPECT_TRUE(item("button")->property("visualFocus").toBool());
  QTest::keyClick(window, Qt::Key_Tab);
  EXPECT_EQ(window->activeFocusItem(), item("after"));
  QTest::keyClick(window, Qt::Key_Backtab);
  EXPECT_EQ(window->activeFocusItem(), item("button"));
  EXPECT_EQ(item("button")->property("focusReason").toInt(), Qt::BacktabFocusReason);
  EXPECT_TRUE(item("button")->property("visualFocus").toBool());
}

TEST_F(SharedRendering, SwitchFocusOutlineSurroundsWholeControlInBothStates) {
  if (qEnvironmentVariable("QT_QUICK_CONTROLS_STYLE") != "Holonight")
    GTEST_SKIP() << "Whole-control outline is the requested HoloNight convention.";
  create(R"(
    C.Switch { objectName: "switch"; x: 30; y: 30; width: 120; height: 36 }
    C.Button { objectName: "other"; y: 120; text: "Other" }
  )");
  ASSERT_TRUE(root);
  auto* control = item("switch");
  for (bool checked : {false, true}) {
    SCOPED_TRACE(checked);
    control->setProperty("checked", checked);
    item("other")->forceActiveFocus(Qt::TabFocusReason);
    // Settle the track/thumb animation before comparing focus alone.
    QTest::qWait(180);
    const auto before = window->grabWindow();
    control->forceActiveFocus(Qt::BacktabFocusReason);
    ASSERT_EQ(window->activeFocusItem(), control);
    ASSERT_TRUE(control->property("visualFocus").toBool());
    const auto after = window->grabWindow();
    QRect changed;
    const auto dpr = window->devicePixelRatio();
    const QRect region(qRound(30 * dpr), qRound(30 * dpr), qRound(120 * dpr), qRound(36 * dpr));
    for (int y = region.top(); y <= region.bottom(); ++y)
      for (int x = region.left(); x <= region.right(); ++x)
        if (before.pixel(x, y) != after.pixel(x, y)) changed |= QRect(x, y, 1, 1);
    EXPECT_GE(changed.width(), region.width() - 2);
    EXPECT_GE(changed.height(), region.height() - 2);
    QTest::keyClick(window, Qt::Key_Space);
    EXPECT_EQ(control->property("checked").toBool(), !checked);
    EXPECT_EQ(window->activeFocusItem(), control);
    EXPECT_TRUE(control->property("visualFocus").toBool());
  }
}
}  // namespace
