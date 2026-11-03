// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#include <QCoreApplication>
#include <QFile>
#include <QSaveFile>
#include <QTemporaryDir>
#include <QTest>

#include <gtest/gtest.h>
#include <holonight/input.h>
#include <holonight/navigation.h>
using namespace Holonight;
using C = NavigationCommand;
TEST(Navigation, ProfilesModifiersAndCounts) {
  NavigationInterpreter keys;
  EXPECT_FALSE(keys.route("j", false).consumed());
  EXPECT_FALSE(keys.route("g", false).consumed());
  EXPECT_EQ(keys.route("ArrowDown", false).command, C::Next);
  EXPECT_EQ(keys.route("Ctrl+H", false, false, true).command, C::ToggleHidden);
  EXPECT_EQ(keys.route("Ctrl+H", true, false, true).command, C::PanelLeft);
  EXPECT_EQ(keys.route("2", true).command, C::Pending);
  EXPECT_EQ(keys.route("3", true).command, C::Pending);
  EXPECT_EQ(keys.route("j", true).count, 23);
  EXPECT_EQ(keys.route("j", true).count, 1);
  EXPECT_EQ(keys.route("0", true).command, C::Pending);
  EXPECT_EQ(keys.route("j", true).count, 0);
  for (int i = 0; i < 20; ++i) keys.route("9", true);
  EXPECT_EQ(keys.route("k", true).count, INT_MAX);
  EXPECT_TRUE(NavigationInterpreter::keyName(Qt::Key_J, "j", Qt::AltModifier).isEmpty());
  EXPECT_TRUE(NavigationInterpreter::keyName(Qt::Key_J, "j", Qt::ControlModifier | Qt::ShiftModifier).isEmpty());
  EXPECT_EQ(NavigationInterpreter::keyName(Qt::Key_J, "j", Qt::ControlModifier), "Ctrl+J");
}
TEST(Navigation, SequencesAndCancellation) {
  NavigationInterpreter keys;
  EXPECT_EQ(keys.route("g", true).command, C::Pending);
  EXPECT_EQ(keys.route("g", true).command, C::First);
  keys.route("g", true);
  keys.reset();
  EXPECT_EQ(keys.route("g", true).command, C::Pending);
  keys.route("Escape", true);
  keys.route("5", true);
  keys.route(" ", true);
  EXPECT_EQ(keys.route("j", true).count, 1);
  EXPECT_EQ(keys.route("g", true).command, C::Pending);
  QTest::qWait(610);
  EXPECT_EQ(keys.route("g", true).command, C::Pending);
  keys.route("g", false);
  EXPECT_EQ(keys.route("g", true).command, C::Pending);
}
TEST(Navigation, BoundariesGridAndVariablePages) {
  NavigationAdapter a{8, 1, 100, [](int i) {
                        const int tops[] = {0, 20, 40, 120, 140, 160, 250, 270};
                        return QRectF(0, tops[i], 1, 20);
                      }};
  EXPECT_EQ(navigationTarget(1, {C::HalfPageDown}, a), 3);
  EXPECT_EQ(navigationTarget(3, {C::HalfPageUp}, a), 2);
  EXPECT_EQ(navigationTarget(7, {C::Next, INT_MAX}, a), 7);
  EXPECT_EQ(navigationTarget(0, {C::Previous, INT_MAX}, a), 0);
  a.columns = 3;
  EXPECT_EQ(navigationTarget(2, {C::Next}, a), 5);
  EXPECT_EQ(navigationTarget(5, {C::Next}, a), 7);
  a.count = 0;
  EXPECT_EQ(navigationTarget(0, {C::Last}, a), -1);
}
TEST(Navigation, IdentityReorderRemovalAndEmpty) {
  NavigationCursor cursor;
  cursor.reconcile({"a", "b", "c", "d"});
  cursor.setIndex(1);
  cursor.reconcile({"d", "c", "b", "a"});
  EXPECT_EQ(cursor.index(), 2);
  EXPECT_EQ(cursor.identity(), "b");
  cursor.reconcile({"d", "c", "a"});
  EXPECT_EQ(cursor.identity(), "a");
  cursor.reconcile({});
  EXPECT_EQ(cursor.index(), -1);
  cursor.reconcile({"new"});
  EXPECT_EQ(cursor.index(), 0);
}
TEST(Input, InvalidUpdatesRetainLastValidProfile) {
  int argc = 1;
  char name[] = "navigation-test";
  char* argv[] = {name, nullptr};
  QCoreApplication app(argc, argv);
  QTemporaryDir directory;
  const QString path = directory.filePath("config.toml");
  InputPreferences preferences(path);
  EXPECT_FALSE(preferences.vimNavigation());
  EXPECT_FALSE(QFile::exists(path));
  auto write = [&](const QByteArray& bytes) {
    QFile file(path);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.write(bytes);
  };
  write("[input]\nnavigation = 'vim-navigation'\n");
  EXPECT_TRUE(preferences.reload());
  EXPECT_TRUE(preferences.vimNavigation());
  write("[input]\nnavigation = 'invalid'\n");
  EXPECT_FALSE(preferences.reload());
  EXPECT_FALSE(preferences.diagnostics().isEmpty());
  EXPECT_TRUE(preferences.vimNavigation());
  write("[input]\nnavigation = [\n");
  EXPECT_FALSE(preferences.reload());
  EXPECT_FALSE(preferences.diagnostics().isEmpty());
  EXPECT_TRUE(preferences.vimNavigation());
  write("# Shell-only preferences\n[bar.workspaces]\ncount = 7\n");
  EXPECT_TRUE(preferences.reload());
  EXPECT_FALSE(preferences.vimNavigation());
  EXPECT_TRUE(preferences.diagnostics().isEmpty());
  QFile::remove(path);
  EXPECT_TRUE(preferences.reload());
  EXPECT_TRUE(preferences.diagnostics().isEmpty());
  EXPECT_FALSE(preferences.vimNavigation());
  QSaveFile replacement(path);
  ASSERT_TRUE(replacement.open(QIODevice::WriteOnly));
  replacement.write("[input]\nnavigation = 'vim-navigation'\n");
  ASSERT_TRUE(replacement.commit());
  EXPECT_TRUE(QTest::qWaitFor([&] { return preferences.vimNavigation(); }));
  QFile::remove(path);
  EXPECT_TRUE(QTest::qWaitFor([&] { return !preferences.vimNavigation(); }));
}

TEST(Navigation, EveryVimBindingIsProfileGated) {
  const std::pair<const char*, C> bindings[] = {{"h", C::Parent},
                                                {"j", C::Next},
                                                {"k", C::Previous},
                                                {"l", C::Open},
                                                {"G", C::Last},
                                                {"Ctrl+U", C::HalfPageUp},
                                                {"Ctrl+D", C::HalfPageDown},
                                                {"Ctrl+J", C::PanelDown},
                                                {"Ctrl+K", C::PanelUp},
                                                {".", C::ToggleHidden},
                                                {"g", C::Pending},
                                                {"2", C::Pending}};
  for (const auto& [key, command] : bindings) {
    SCOPED_TRACE(key);
    NavigationInterpreter interpreter;
    EXPECT_FALSE(interpreter.route(key, false, false, true).consumed());
    EXPECT_EQ(interpreter.route(key, true, false, true).command, command);
  }
  NavigationInterpreter interpreter;
  EXPECT_EQ(interpreter.route("Ctrl+H", false, false, true).command, C::ToggleHidden);
  EXPECT_EQ(interpreter.route("Ctrl+L", false, false, true).command, C::EditLocation);
  EXPECT_EQ(interpreter.route("Ctrl+H", true, false, true).command, C::PanelLeft);
  EXPECT_EQ(interpreter.route("Ctrl+L", true, false, true).command, C::PanelRight);
}
