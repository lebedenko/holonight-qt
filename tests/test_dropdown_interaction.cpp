// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QFontDatabase>
#include <QPointer>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QStringListModel>
#include <QTest>

#include <gtest/gtest.h>
#include <memory>

namespace {
class PopulatedFontModel : public QStringListModel {
  Q_OBJECT
  Q_PROPERTY(QString retainedFamily READ retainedFamily WRITE setRetainedFamily NOTIFY retainedFamilyChanged)
 public:
  explicit PopulatedFontModel(QObject* parent = nullptr) : QStringListModel(parent) { rebuild(); }
  QString retainedFamily() const { return retained_; }
  void setRetainedFamily(const QString& value) {
    if (retained_ == value) return;
    retained_ = value;
    emit retainedFamilyChanged();
    rebuild();
  }
  Q_INVOKABLE int indexOf(const QString& value) const { return stringList().indexOf(value); }
 signals:
  void retainedFamilyChanged();

 private:
  QString retained_;
  void rebuild() {
    auto names = QFontDatabase::families();
    if (!retained_.isEmpty() && !names.contains(retained_)) names.prepend(retained_);
    setStringList(names);
  }
};
class DropdownInteraction : public testing::TestWithParam<const char*> {
 protected:
  QQmlEngine engine;
  QStringListModel fonts;
  std::unique_ptr<QObject> root;
  QQuickWindow* window = nullptr;
  QQuickItem* combo = nullptr;
  QObject* popup = nullptr;
  QQuickItem* list = nullptr;

  void SetUp() override {
    engine.addImportPath(qEnvironmentVariable("UQC_IMPORT_PATH", QStringLiteral(HOLONIGHT_QML_IMPORT_PATH)));
    engine.rootContext()->setContextProperty("fontRows", &fonts);
    QQmlComponent component(&engine);
    component.setData(QString(R"(
import QtQuick
import QtQuick.Controls as C
import Holonight.Controls
Window {
    width: 500; height: 400; visible: true
    property int rowCount: 30
    property bool useFonts: false
    ListModel { id: rows }
    Component.onCompleted: {
        for (let i = 0; i < rowCount; ++i)
            rows.append({name: "Session " + i, icon: ""})
    }
    %1 {
        id: combo; objectName: "combo"
        x: 30; y: 40; width: 200
        model: useFonts ? fontRows : rows
        textRole: useFonts ? "display" : "name"; currentIndex: 15
    }
})")
                          .arg(GetParam())
                          .toUtf8(),
                      QUrl());
    root.reset(component.create());
    ASSERT_NE(root, nullptr) << component.errorString().toStdString();
    window = qobject_cast<QQuickWindow*>(root.get());
    ASSERT_NE(window, nullptr);
    combo = root->findChild<QQuickItem*>("combo");
    ASSERT_NE(combo, nullptr);
    popup = combo->property("popup").value<QObject*>();
    ASSERT_NE(popup, nullptr);
    list = popup->property("contentItem").value<QQuickItem*>();
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(QTest::qWaitForWindowExposed(window));
    combo->forceActiveFocus(Qt::TabFocusReason);
  }

  void open() {
    QTest::mouseMove(window, QPoint(450, 350));
    QTest::keyClick(window, Qt::Key_Space);
    ASSERT_TRUE(QTest::qWaitFor([&] { return popup->property("opened").toBool(); }));
    QSignalSpy rendered(window, &QQuickWindow::frameSwapped);
    window->update();
    ASSERT_TRUE(rendered.wait(1000));
  }

  QQuickItem* currentItem() { return list->property("currentItem").value<QQuickItem*>(); }
};

INSTANTIATE_TEST_SUITE_P(Controls, DropdownInteraction, testing::Values("C.ComboBox", "HnIconComboBox"));

TEST_P(DropdownInteraction, PopulatedFontModelInSettingsLoaderConstructsWithoutHeightLoop) {
  qmlRegisterType<PopulatedFontModel>("DropdownTests", 1, 0, "FontListModel");
  QQmlComponent component(&engine);
  component.setData(QString(R"(
import DropdownTests
import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Controls
Item {
    width: 500; height: 400
    ColumnLayout {
        anchors.fill: parent
        HnSettingsRow {
            Layout.fillWidth: true
            titleText: "Interface font"
            control: Component {
                %1 {
                    objectName: "loadedCombo"
                    implicitWidth: 200
                    model: FontListModel { id: fontModel; retainedFamily: "Missing test font" }
                    textRole: "display"
                    currentIndex: fontModel.indexOf("Missing test font")
                }
            }
        }
    }
})")
                        .arg(GetParam())
                        .toUtf8(),
                    QUrl());
  std::unique_ptr<QObject> page(
      component.createWithInitialProperties({{"parent", QVariant::fromValue(window->contentItem())}}));
  ASSERT_NE(page, nullptr) << component.errorString().toStdString();
  auto* item = qobject_cast<QQuickItem*>(page.get());
  ASSERT_NE(item, nullptr);
  item->setParentItem(window->contentItem());
  QCoreApplication::processEvents();
  auto* loaded = page->findChild<QQuickItem*>("loadedCombo");
  ASSERT_NE(loaded, nullptr);
  loaded->forceActiveFocus(Qt::TabFocusReason);
  QTest::keyClick(window, Qt::Key_Space);
  auto* loaded_popup = loaded->property("popup").value<QObject*>();
  ASSERT_NE(loaded_popup, nullptr);
  ASSERT_TRUE(QTest::qWaitFor([&] { return loaded_popup->property("opened").toBool(); }));
}

TEST_P(DropdownInteraction, WeatherModelInSettingsLoaderConstructsWithoutHeightLoop) {
  QQmlComponent component(&engine);
  component.setData(QString(R"(
import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Controls
Item {
    width: 500; height: 400
    ColumnLayout {
        anchors.fill: parent
        HnSettingsRow {
            Layout.fillWidth: true
            titleText: "Temperature unit"
            control: Component {
                %1 {
                    objectName: "loadedCombo"
                    implicitWidth: 200
                    model: [{text: "Celsius", value: "celsius"}, {text: "Fahrenheit", value: "fahrenheit"}]
                    textRole: "text"
                    valueRole: "value"
                    currentIndex: {
                        for (let i = 0; i < model.length; ++i) {
                            if (model[i].value === "celsius") return i;
                        }
                        return 0;
                    }
                }
            }
        }
    }
})")
                        .arg(GetParam())
                        .toUtf8(),
                    QUrl());
  std::unique_ptr<QObject> page(
      component.createWithInitialProperties({{"parent", QVariant::fromValue(window->contentItem())}}));
  ASSERT_NE(page, nullptr) << component.errorString().toStdString();
  auto* item = qobject_cast<QQuickItem*>(page.get());
  ASSERT_NE(item, nullptr);
  item->setParentItem(window->contentItem());
  QCoreApplication::processEvents();
  auto* loaded = page->findChild<QQuickItem*>("loadedCombo");
  ASSERT_NE(loaded, nullptr);
  loaded->forceActiveFocus(Qt::TabFocusReason);
  QTest::keyClick(window, Qt::Key_Space);
  auto* loaded_popup = loaded->property("popup").value<QObject*>();
  ASSERT_NE(loaded_popup, nullptr);
  ASSERT_TRUE(QTest::qWaitFor([&] { return loaded_popup->property("opened").toBool(); }));
}

TEST_P(DropdownInteraction, OutsidePressDismissesWithoutSelection) {
  QSignalSpy activated(combo, SIGNAL(activated(int)));
  ASSERT_NO_FATAL_FAILURE(open());
  ASSERT_TRUE(popup->property("visible").toBool());
  QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, QPoint(450, 350));
  EXPECT_TRUE(QTest::qWaitFor([&] { return !popup->property("visible").toBool(); }, 1000));
  EXPECT_EQ(combo->property("currentIndex").toInt(), 15);
  EXPECT_EQ(activated.count(), 0);
}

TEST_P(DropdownInteraction, CollapsedControlClosesPopupWithoutReopening) {
  ASSERT_NO_FATAL_FAILURE(open());
  const QPoint center = combo->mapToScene(QPointF(combo->width() / 2, combo->height() / 2)).toPoint();
  // Fusion may cover the collapsed control with selectable popup rows.
  const bool covered_by_list = list->contains(list->mapFromScene(center));
  QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);
  EXPECT_TRUE(QTest::qWaitFor([&] { return !popup->property("visible").toBool(); }, 1000));
  if (!covered_by_list) EXPECT_EQ(combo->property("currentIndex").toInt(), 15);
}

TEST_P(DropdownInteraction, ReopenShowsCurrentRowAndKeyboardStartsThere) {
  for (int index : {15, 29, 0}) {
    ASSERT_TRUE(combo->setProperty("currentIndex", index));
    ASSERT_NO_FATAL_FAILURE(open());
    ASSERT_TRUE(QTest::qWaitFor([&] { return currentItem() != nullptr; }));
    auto* item = currentItem();
    EXPECT_TRUE(item->isVisible());
    const QPointF scene_center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
    EXPECT_TRUE(list->contains(list->mapFromScene(scene_center)));
    EXPECT_EQ(combo->property("highlightedIndex").toInt(), index);
    EXPECT_TRUE(item->property("highlighted").toBool());
    EXPECT_GE(item->y(), list->property("contentY").toReal() - 0.5);
    EXPECT_LE(item->y() + item->height(), list->property("contentY").toReal() + list->height() + 0.5);
    QTest::keyClick(window, index == 29 ? Qt::Key_Up : Qt::Key_Down);
    EXPECT_EQ(combo->property("highlightedIndex").toInt(), index == 29 ? 28 : index + 1);
    QTest::keyClick(window, Qt::Key_Escape);
    ASSERT_TRUE(QTest::qWaitFor([&] { return !popup->property("visible").toBool(); }));
    EXPECT_EQ(combo->property("currentIndex").toInt(), index);
  }
}

TEST_P(DropdownInteraction, MouseReopenShowsSelectedRowAndKeyboardStartsThere) {
  for (int index : {15, 29, 0}) {
    ASSERT_TRUE(combo->setProperty("currentIndex", index));
    const QPoint center = combo->mapToScene(QPointF(combo->width() / 2, combo->height() / 2)).toPoint();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);
    ASSERT_TRUE(QTest::qWaitFor([&] { return popup->property("opened").toBool(); }));
    QSignalSpy rendered(window, &QQuickWindow::frameSwapped);
    window->update();
    ASSERT_TRUE(rendered.wait(1000));
    ASSERT_TRUE(QTest::qWaitFor([&] { return currentItem() != nullptr; }));
    auto* item = currentItem();
    EXPECT_TRUE(item->isVisible());
    const QPointF scene_center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
    EXPECT_TRUE(list->contains(list->mapFromScene(scene_center)));
    EXPECT_EQ(combo->property("highlightedIndex").toInt(), index);
    EXPECT_TRUE(item->property("highlighted").toBool());
    EXPECT_GE(item->y(), list->property("contentY").toReal() - 0.5);
    EXPECT_LE(item->y() + item->height(), list->property("contentY").toReal() + list->height() + 0.5);
    QTest::keyClick(window, index == 29 ? Qt::Key_Up : Qt::Key_Down);
    EXPECT_EQ(combo->property("highlightedIndex").toInt(), index == 29 ? 28 : index + 1);
    QTest::keyClick(window, Qt::Key_Escape);
    ASSERT_TRUE(QTest::qWaitFor([&] { return !popup->property("visible").toBool(); }));
    EXPECT_EQ(combo->property("currentIndex").toInt(), index);
  }
}

TEST_P(DropdownInteraction, HoveredRowsRemainVisibleAndFirstLastCanBeSelected) {
  for (int index : {0, 29}) {
    ASSERT_TRUE(combo->setProperty("currentIndex", index));
    ASSERT_NO_FATAL_FAILURE(open());
    ASSERT_TRUE(QTest::qWaitFor([&] { return currentItem() != nullptr; }));
    QPointer<QQuickItem> item = currentItem();
    const QPoint center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2)).toPoint();
    QTest::mouseMove(window, center);
    QCoreApplication::processEvents();
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(item->isVisible());
    EXPECT_GT(item->width(), 0);
    EXPECT_GT(item->height(), 0);
    EXPECT_EQ(item->property("text").toString(), QString("Session %1").arg(index));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);
    EXPECT_TRUE(QTest::qWaitFor([&] { return !popup->property("visible").toBool(); }, 1000));
    EXPECT_EQ(combo->property("currentIndex").toInt(), index);
  }
}

TEST_P(DropdownInteraction, FontModelResetsKeepPopupBoundedAtBothWindowEdges) {
  ASSERT_TRUE(root->setProperty("useFonts", true));
  for (int y : {10, 340}) {
    combo->setY(y);
    for (int count : {3, 60, 1, 30}) {
      QStringList names;
      for (int i = 0; i < count; ++i) names.append(QString("Font %1").arg(i));
      fonts.setStringList(names);
      ASSERT_TRUE(combo->setProperty("currentIndex", count - 1));
      ASSERT_NO_FATAL_FAILURE(open());
      ASSERT_TRUE(QTest::qWaitFor([&] { return currentItem() && currentItem()->isVisible(); }));
      auto* item = currentItem();
      EXPECT_EQ(item->property("text").toString(), names.last());
      const QPointF top = list->mapToScene(QPointF());
      const QPointF bottom = list->mapToScene(QPointF(list->width(), list->height()));
      EXPECT_GE(top.y(), 0);
      EXPECT_LE(bottom.y(), window->height());
      EXPECT_GT(list->height(), 0);
      EXPECT_GE(item->y(), list->property("contentY").toReal() - 0.5);
      EXPECT_LE(item->y() + item->height(), list->property("contentY").toReal() + list->height() + 0.5);
      QTest::keyClick(window, Qt::Key_Escape);
      ASSERT_TRUE(QTest::qWaitFor([&] { return !popup->property("visible").toBool(); }));
    }
  }
}
}  // namespace

#include "test_dropdown_interaction.moc"
