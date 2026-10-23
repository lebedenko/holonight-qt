// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "startup.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace {
QVBoxLayout* addPage(QTabWidget* tabs, const QString& title) {
  auto* scroll = new QScrollArea;
  scroll->setWidgetResizable(true);
  auto* page = new QWidget;
  auto* layout = new QVBoxLayout(page);
  layout->setContentsMargins(24, 24, 24, 24);
  layout->setSpacing(12);
  scroll->setWidget(page);
  tabs->addTab(scroll, title);
  return layout;
}

void addInput(QTabWidget* tabs) {
  auto* layout = addPage(tabs, QStringLiteral("Input"));
  auto* buttons = new QHBoxLayout;
  for (const auto& text : {QStringLiteral("Button"), QStringLiteral("Toggle"), QStringLiteral("Disabled")}) {
    auto* button = new QPushButton(text);
    button->setCheckable(text == QStringLiteral("Toggle"));
    button->setEnabled(text != QStringLiteral("Disabled"));
    buttons->addWidget(button);
  }
  layout->addLayout(buttons);
  layout->addWidget(new QLabel(QStringLiteral("Checkbox states")));
  for (const bool enabled : {true, false}) {
    auto* row = new QHBoxLayout;
    for (const auto state : {Qt::Unchecked, Qt::Checked, Qt::PartiallyChecked}) {
      auto* check = new QCheckBox(state == Qt::Unchecked ? QStringLiteral("Unchecked")
                                  : state == Qt::Checked ? QStringLiteral("Checked")
                                                         : QStringLiteral("Partial"));
      check->setTristate(true);
      check->setCheckState(state);
      check->setEnabled(enabled);
      row->addWidget(check);
    }
    layout->addLayout(row);
  }
  auto* radios = new QHBoxLayout;
  for (int i = 0; i < 4; ++i) {
    auto* radio = new QRadioButton(i < 2 ? QStringLiteral("Radio") : QStringLiteral("Disabled radio"));
    radio->setAutoExclusive(false);
    radio->setChecked(i % 2 == 1);
    radio->setEnabled(i < 2);
    radios->addWidget(radio);
  }
  layout->addLayout(radios);
  for (const bool enabled : {true, false}) {
    auto* combo = new QComboBox;
    combo->addItems({QStringLiteral("First option"), QStringLiteral("Second option"), QStringLiteral("Third option")});
    combo->setEnabled(enabled);
    layout->addWidget(combo);
    auto* spin = new QSpinBox;
    spin->setRange(-100, 100);
    spin->setValue(42);
    spin->setEnabled(enabled);
    layout->addWidget(spin);
    auto* real = new QDoubleSpinBox;
    real->setValue(3.14);
    real->setEnabled(enabled);
    layout->addWidget(real);
    auto* slider = new QSlider(Qt::Horizontal);
    slider->setValue(65);
    slider->setEnabled(enabled);
    layout->addWidget(slider);
  }
  layout->addStretch();
}

void addText(QTabWidget* tabs) {
  auto* layout = addPage(tabs, QStringLiteral("Text"));
  layout->addWidget(new QLabel(QStringLiteral("Labels and text editors")));
  auto* disabledLabel = new QLabel(QStringLiteral("Disabled label"));
  disabledLabel->setEnabled(false);
  layout->addWidget(disabledLabel);
  for (int i = 0; i < 3; ++i) {
    auto* edit = new QLineEdit(i == 0   ? QStringLiteral("Editable text")
                               : i == 1 ? QStringLiteral("Read-only text")
                                        : QStringLiteral("Disabled text"));
    edit->setReadOnly(i == 1);
    edit->setEnabled(i != 2);
    layout->addWidget(edit);
  }
  auto* plain = new QPlainTextEdit(QStringLiteral("Plain text editor\nSelect and edit this text."));
  plain->setMinimumHeight(140);
  layout->addWidget(plain);
  auto* rich = new QTextEdit(QStringLiteral("<b>Rich text editor</b><p>Multiline content and selection.</p>"));
  rich->setMinimumHeight(140);
  layout->addWidget(rich);
  layout->addStretch();
}

void addNavigation(QTabWidget* tabs) {
  auto* layout = addPage(tabs, QStringLiteral("Navigation"));
  auto* list = new QListWidget;
  for (int i = 0; i < 24; ++i) {
    auto* item = new QListWidgetItem(QApplication::style()->standardIcon(QStyle::SP_FileIcon),
                                     QStringLiteral("List item %1").arg(i + 1), list);
    item->setCheckState(static_cast<Qt::CheckState>(i % 3));
  }
  list->setCurrentRow(1);
  list->setMinimumHeight(170);
  layout->addWidget(list);
  auto* tree = new QTreeWidget;
  tree->setHeaderLabels({QStringLiteral("Name"), QStringLiteral("Details")});
  for (int i = 0; i < 12; ++i) {
    auto* item = new QTreeWidgetItem(tree, {QStringLiteral("Group %1").arg(i + 1), QStringLiteral("Checkable")});
    item->setCheckState(0, static_cast<Qt::CheckState>(i % 3));
    new QTreeWidgetItem(item, {QStringLiteral("Child item"), QStringLiteral("Tree content")});
    item->setExpanded(true);
  }
  tree->setCurrentItem(tree->topLevelItem(1));
  tree->setMinimumHeight(200);
  layout->addWidget(tree);
  auto* table = new QTableWidget(20, 3);
  table->setHorizontalHeaderLabels({QStringLiteral("Enabled"), QStringLiteral("Name"), QStringLiteral("Value")});
  for (int row = 0; row < table->rowCount(); ++row) {
    for (int column = 0; column < table->columnCount(); ++column) {
      auto* item = new QTableWidgetItem(QStringLiteral("Cell %1, %2").arg(row + 1).arg(column + 1));
      if (column == 0) item->setCheckState(static_cast<Qt::CheckState>(row % 3));
      table->setItem(row, column, item);
    }
  }
  table->setCurrentCell(1, 1);
  table->setMinimumHeight(200);
  layout->addWidget(table);
}

void addFeedback(QTabWidget* tabs, QMainWindow* window) {
  auto* layout = addPage(tabs, QStringLiteral("Feedback"));
  for (int i = 0; i < 3; ++i) {
    auto* progress = new QProgressBar;
    if (i == 1)
      progress->setRange(0, 0);
    else
      progress->setValue(i == 0 ? 65 : 40);
    progress->setEnabled(i != 2);
    layout->addWidget(progress);
  }
  auto* tooltip = new QPushButton(QStringLiteral("Hover for tooltip"));
  tooltip->setToolTip(QStringLiteral("A standard Qt Widgets tooltip"));
  layout->addWidget(tooltip);
  auto* message = new QPushButton(QStringLiteral("Open message box"));
  QObject::connect(message, &QPushButton::clicked, window, [window] {
    QMessageBox::information(window, QStringLiteral("HoloNight"), QStringLiteral("A standard Qt Widgets message box."));
  });
  layout->addWidget(message);
  layout->addStretch();
}

void addContainers(QTabWidget* tabs) {
  auto* layout = addPage(tabs, QStringLiteral("Containers"));
  for (int i = 0; i < 4; ++i) {
    auto* group = new QGroupBox(i == 0 ? QStringLiteral("Ordinary group") : QStringLiteral("Checkable group"));
    if (i > 0) {
      group->setCheckable(true);
      group->setChecked(i != 2);
      group->setEnabled(i != 3);
    }
    auto* contents = new QVBoxLayout(group);
    contents->addWidget(new QLabel(QStringLiteral("Group content")));
    contents->addWidget(new QCheckBox(QStringLiteral("Nested checkbox")));
    layout->addWidget(group);
  }
  auto* frame = new QFrame;
  frame->setFrameShape(QFrame::StyledPanel);
  auto* contents = new QVBoxLayout(frame);
  contents->addWidget(new QLabel(QStringLiteral("Standard frame")));
  layout->addWidget(frame);
  auto* splitter = new QSplitter;
  splitter->addWidget(new QTextEdit(QStringLiteral("Resizable left pane")));
  splitter->addWidget(new QTextEdit(QStringLiteral("Resizable right pane")));
  layout->addWidget(splitter);
}
}  // namespace

int main(int argc, char* argv[]) {
  Demo::addPluginPaths();
  QTemporaryDir themeConfigDir;
  const QString theme = Demo::findThemeArgument({argv, static_cast<std::size_t>(argc)});
  if (!theme.isEmpty() && !Demo::writeAppearance(themeConfigDir, theme)) return 1;
  if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORMTHEME")) qputenv("QT_QPA_PLATFORMTHEME", "holonight");
  QApplication app(argc, argv);
  if (!Demo::parseArguments(app)) return 1;

  QMainWindow window;
  window.setWindowTitle(QStringLiteral("HoloNight — Qt Widgets"));
  window.resize(900, 740);
  auto* tabs = new QTabWidget;
  window.setCentralWidget(tabs);
  addInput(tabs);
  addText(tabs);
  addNavigation(tabs);
  addFeedback(tabs, &window);
  addContainers(tabs);

  auto* dock = new QDockWidget(QStringLiteral("Dock panel"), &window);
  dock->setWidget(new QLabel(QStringLiteral("A standard dock widget")));
  window.addDockWidget(Qt::RightDockWidgetArea, dock);
  auto* file = window.menuBar()->addMenu(QStringLiteral("&File"));
  auto* message = file->addAction(QStringLiteral("Show &message"), &window, [&window] {
    QMessageBox::information(&window, QStringLiteral("HoloNight"),
                             QStringLiteral("Menus and toolbars are functional."));
  });
  file->addSeparator();
  file->addAction(QStringLiteral("&Quit"), QKeySequence::Quit, &app, &QApplication::quit);
  window.menuBar()->addMenu(QStringLiteral("&View"))->addAction(dock->toggleViewAction());
  auto* toolbar = window.addToolBar(QStringLiteral("Demo actions"));
  toolbar->addAction(message);
  toolbar->addAction(dock->toggleViewAction());
  auto* styleLabel = new QLabel(QStringLiteral("Active Widgets style: %1").arg(app.style()->objectName()));
  window.statusBar()->addPermanentWidget(styleLabel);
  window.statusBar()->showMessage(QStringLiteral("Ready"));
  qInfo().noquote() << styleLabel->text();
  window.show();
  return QApplication::exec();
}
