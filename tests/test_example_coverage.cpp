// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTest>

#include <algorithm>
#include <gtest/gtest.h>
#include <memory>

namespace {
QList<QQuickItem*> namedItems(QQuickItem* root, const QString& name) {
  QList<QQuickItem*> result;
  if (root->objectName() == name) result << root;
  for (auto* child : root->childItems()) result.append(namedItems(child, name));
  return result;
}
class ExampleCoverage : public ::testing::Test {
 protected:
  QQmlEngine engine;
  void SetUp() override { engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)); }
  std::unique_ptr<QObject> load(const char* path) {
    QQmlComponent component(&engine, QUrl::fromLocalFile(QStringLiteral(HOLONIGHT_SOURCE_DIR) + path));
    if (component.isError()) {
      ADD_FAILURE() << component.errorString().toStdString();
      return {};
    }
    auto result = std::unique_ptr<QObject>(component.create());
    if (!result) ADD_FAILURE() << component.errorString().toStdString();
    QCoreApplication::processEvents();
    return result;
  }
};

TEST_F(ExampleCoverage, GalleryHeadingsCoverPublicInventory) {
  auto gallery = load("/examples/controls-gallery/Main.qml");
  ASSERT_NE(gallery, nullptr);
  QStringList headings;
  for (auto* item : gallery->findChildren<QObject*>()) {
    if (item->property("rawText").isValid()) headings << item->property("rawText").toString();
  }
  QStringList types = QStringLiteral(HOLONIGHT_COMPOSITE_TYPES).split(',');
  types << "HnLabel" << "HnIcon" << "HnButtonSurface";
  for (const auto& type : types) {
    bool found = false;
    for (const auto& text : headings) found |= text.split(" / ").contains(type);
    EXPECT_TRUE(found) << type.toStdString();
  }
}

TEST_F(ExampleCoverage, GalleryInteractionsAndTypographyBounds) {
  auto gallery = load("/examples/controls-gallery/Main.qml");
  ASSERT_NE(gallery, nullptr);
  auto* window = qobject_cast<QQuickWindow*>(gallery.get());
  ASSERT_NE(window, nullptr);
  gallery->setProperty("currentPageIndex", 4);
  QTest::qWait(30);
  const auto breadcrumbs = namedItems(window->contentItem(), "galleryBreadcrumbBar");
  ASSERT_EQ(breadcrumbs.size(), 2);
  for (auto* bar : breadcrumbs) {
    auto* editor = bar->parentItem()->findChild<QQuickItem*>("galleryLocationEditor");
    ASSERT_NE(editor, nullptr);
    ASSERT_TRUE(QMetaObject::invokeMethod(bar, "locationEditRequested"));
    EXPECT_TRUE(editor->isVisible());
    EXPECT_TRUE(editor->hasActiveFocus());
    EXPECT_EQ(editor->property("text").toString(), "/home/andrii/Pictures");
    editor->setProperty("text", "/tmp/Images");
    ASSERT_TRUE(QMetaObject::invokeMethod(editor, "accepted"));
    EXPECT_FALSE(editor->isVisible());
    EXPECT_EQ(bar->property("segments").toStringList(), (QStringList{"/", "tmp", "Images"}));
    QTest::keyClick(window, Qt::Key_L, Qt::ControlModifier);
    EXPECT_TRUE(editor->isVisible());
    editor->setProperty("text", "/cancelled");
    QTest::keyClick(window, Qt::Key_Escape);
    EXPECT_FALSE(editor->isVisible());
    EXPECT_EQ(bar->property("segments").toStringList(), (QStringList{"/", "tmp", "Images"}));
    bar->setProperty("segments", QStringList{"/", "home", "andrii", "Pictures"});
  }
  auto* first_editor = breadcrumbs[0]->parentItem()->findChild<QQuickItem*>("galleryLocationEditor");
  auto* second_editor = breadcrumbs[1]->parentItem()->findChild<QQuickItem*>("galleryLocationEditor");
  ASSERT_NE(first_editor, nullptr);
  ASSERT_NE(second_editor, nullptr);
  ASSERT_TRUE(QMetaObject::invokeMethod(breadcrumbs[0], "locationEditRequested"));
  first_editor->setProperty("text", "/discarded");
  ASSERT_TRUE(QMetaObject::invokeMethod(breadcrumbs[1], "locationEditRequested"));
  EXPECT_FALSE(first_editor->isVisible());
  EXPECT_TRUE(breadcrumbs[0]->isVisible());
  EXPECT_TRUE(second_editor->isVisible());
  EXPECT_TRUE(second_editor->hasActiveFocus());
  EXPECT_EQ(breadcrumbs[0]->property("segments").toStringList(), (QStringList{"/", "home", "andrii", "Pictures"}));
  // Losing focus to a different control also cancels, without stealing focus back.
  breadcrumbs[0]->forceActiveFocus();
  EXPECT_FALSE(second_editor->isVisible());
  EXPECT_TRUE(breadcrumbs[0]->hasActiveFocus());
  ASSERT_TRUE(QMetaObject::invokeMethod(breadcrumbs[0], "locationEditRequested"));
  EXPECT_EQ(first_editor->property("text").toString(), "/home/andrii/Pictures");
  QTest::keyClick(window, Qt::Key_Escape);
  const auto icons = namedItems(window->contentItem(), "sidebarIcon");
  ASSERT_EQ(icons.size(), 6);
  for (auto* icon : icons) {
    EXPECT_FALSE(icon->property("source").toUrl().isEmpty());
    EXPECT_FALSE(icon->property("hasError").toBool());
    const auto images = icon->findChildren<QObject*>();
    bool ready = false;
    for (auto* image : images) {
      if (image->property("status").isValid()) ready |= image->property("status").toInt() == 1;
    }
    EXPECT_TRUE(ready);
  }
  auto* selector = gallery->findChild<QObject*>("previewSelector");
  auto* preview = gallery->findChild<QObject*>("previewFrame");
  ASSERT_NE(selector, nullptr);
  ASSERT_NE(preview, nullptr);
  for (int state : {0, 1, 2, 3, 0, 3, 2, 1}) {
    selector->setProperty("currentIndex", state);
    QCoreApplication::processEvents();
    EXPECT_EQ(preview->property("previewState").toInt(), state);
    auto* content = preview->property("previewItem").value<QObject*>();
    if (state == 2) {
      EXPECT_EQ(content, nullptr);
      continue;
    }
    ASSERT_NE(content, nullptr);
    if (state == 0) {
      auto* icon = content->findChild<QObject*>("previewContentIcon");
      ASSERT_NE(icon, nullptr);
      EXPECT_FALSE(icon->property("source").toUrl().isEmpty());
      EXPECT_FALSE(icon->property("hasError").toBool());
    } else if (state == 1) {
      auto* busy = content->findChild<QObject*>("previewBusy");
      ASSERT_NE(busy, nullptr);
      EXPECT_TRUE(busy->property("running").toBool());
    } else {
      auto* caption = content->findChild<QObject*>("previewFailureCaption");
      ASSERT_NE(caption, nullptr);
      EXPECT_EQ(caption->property("rawText").toString(), QStringLiteral("No preview available"));
    }
  }
  for (const auto* name : {"selectableCard", "selectableThumbnail"}) {
    auto* item = gallery->findChild<QObject*>(name);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(item->property("checkable").toBool());
    for (bool selected : {false, true, false}) {
      ASSERT_TRUE(QMetaObject::invokeMethod(item, "toggle"));
      EXPECT_EQ(item->property("checked").toBool(), selected);
    }
  }
  gallery->setProperty("currentPageIndex", 1);
  for (const QSize size : {QSize(1000, 760), QSize(840, 700)}) {
    window->resize(size);
    QTest::qWait(50);
    auto* flow = gallery->findChild<QQuickItem*>("typographyPresets");
    ASSERT_NE(flow, nullptr);
    EXPECT_GT(flow->width(), 0);
    ASSERT_NE(flow->parentItem(), nullptr);
    EXPECT_LE(flow->y() + flow->height(), flow->parentItem()->height() + 1);
    for (auto* button : flow->childItems()) {
      EXPECT_GE(button->x(), 0);
      EXPECT_LE(button->x() + button->width(), flow->width() + 1);
      EXPECT_LE(button->y() + button->height(), flow->height() + 1);
    }
  }
  const auto artifactDir = qEnvironmentVariable("HOLONIGHT_EXAMPLE_SCREENSHOTS");
  if (!artifactDir.isEmpty()) {
    QDir().mkpath(artifactDir);
    window->resize(1000, 760);
    for (int page = 0; page < 6; ++page) {
      gallery->setProperty("currentPageIndex", page);
      QTest::qWait(50);
      const QStringList pages = {"SurfacesPage", "TypographyPage", "InputsPage",
                                 "ButtonsPage",  "DelegatesPage",  "SettingsPage"};
      auto* scroll = gallery->findChild<QObject*>(pages.at(page));
      ASSERT_NE(scroll, nullptr);
      auto* flickable = scroll->property("contentItem").value<QObject*>();
      ASSERT_NE(flickable, nullptr);
      const auto height = flickable->property("height").toDouble();
      const auto contentHeight = flickable->property("contentHeight").toDouble();
      int section = 0;
      for (double y = 0; y < contentHeight; y += height * 0.8) {
        flickable->setProperty("contentY", std::min(y, std::max(0.0, contentHeight - height)));
        QTest::qWait(50);
        EXPECT_TRUE(window->grabWindow().save(artifactDir + QString("/gallery-%1-%2.png").arg(page).arg(section++)));
        if (y + height >= contentHeight) break;
      }
    }
  }
}

TEST_F(ExampleCoverage, WidgetsDemoButtonEmitsLaunchRequestAndDisplaysError) {
  auto demo = load("/demo/Main.qml");
  ASSERT_NE(demo, nullptr);
  auto* button = demo->findChild<QObject*>("openWidgetsDemo");
  ASSERT_NE(button, nullptr);
  QSignalSpy request(demo.get(), SIGNAL(openWidgetsDemoRequested()));
  ASSERT_TRUE(QMetaObject::invokeMethod(button, "clicked"));
  EXPECT_EQ(request.count(), 1);
  demo->setProperty("widgetsLaunchError", QStringLiteral("Launch failed"));
  auto* error = demo->findChild<QObject*>("widgetsLaunchError");
  ASSERT_NE(error, nullptr);
  EXPECT_EQ(error->property("text").toString(), QStringLiteral("Launch failed"));
  EXPECT_TRUE(error->property("visible").toBool());
}

TEST_F(ExampleCoverage, SecondaryWindowAndPopupReopen) {
  auto demo = load("/demo/Main.qml");
  ASSERT_NE(demo, nullptr);
  auto* button = demo->findChild<QObject*>("openSecondaryWindow");
  auto* loader = demo->findChild<QObject*>("secondaryWindowLoader");
  ASSERT_NE(button, nullptr);
  ASSERT_NE(loader, nullptr);
  for (int cycle = 0; cycle < 2; ++cycle) {
    ASSERT_TRUE(QMetaObject::invokeMethod(button, "clicked"));
    QCoreApplication::processEvents();
    auto* secondary = loader->property("item").value<QObject*>();
    ASSERT_NE(secondary, nullptr);
    EXPECT_TRUE(secondary->property("visible").toBool());
    auto* popup = secondary->findChild<QObject*>("standardPopup");
    ASSERT_NE(popup, nullptr);
    for (int opening = 0; opening < 2; ++opening) {
      ASSERT_TRUE(QMetaObject::invokeMethod(popup, "open"));
      EXPECT_TRUE(popup->property("visible").toBool());
      ASSERT_TRUE(QMetaObject::invokeMethod(popup, "close"));
      QTest::qWait(250);
      EXPECT_FALSE(popup->property("visible").toBool());
    }
    ASSERT_TRUE(QMetaObject::invokeMethod(secondary, "close"));
    QCoreApplication::processEvents();
    EXPECT_FALSE(loader->property("active").toBool());
  }
}
}  // namespace
