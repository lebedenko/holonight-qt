// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QApplication>
#include <QFile>
#include <QProcess>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QSaveFile>
#include <QSignalSpy>
#include <QStyleHints>
#include <QTemporaryDir>
#include <QTest>

#include <memory>
#include <span>

namespace {
QByteArray document(const QByteArray& scheme, const QByteArray& accent) {
  return "version = 1\n[theme]\nscheme = \"" + scheme + "\"\naccent = \"" + accent +
         "\"\n"
         "[typography]\nui_family = \"Inter\"\nui_size = 12\nmonospace_family = \"JetBrains Mono\"\n"
         "monospace_size = 12\ntitle_family = \"Audiowide\"\ntitle_size = 10\ndisplay_family = "
         "\"Rajdhani\"\ndisplay_size = 24\n"
         "[icons]\ntheme = \"HoloNight\"\nfallback = \"Papirus\"\ncursor = \"default\"\n"
         "[layout]\nscale = 1.0\n[shape]\nstyle = \"inherit\"\nscale = 1.0\n";
}
bool replace(const QString& path, const QByteArray& bytes) {
  QSaveFile file(path);
  return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size() && file.commit();
}
QByteArray colors(const QPalette& palette) {
  QByteArray result;
  for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    for (auto role : {QPalette::Window, QPalette::Base, QPalette::Text, QPalette::Highlight, QPalette::Link}) {
      result += palette.color(group, role).name(QColor::HexArgb).toUtf8() + '\n';
    }
  }
  return result;
}
class Events : public QObject {
 public:
  int changes = 0;
  bool eventFilter(QObject* watched, QEvent* event) override {
    if (watched == qGuiApp && event->type() == QEvent::ApplicationPaletteChange) {
      ++changes;
    }
    return false;
  }
};
}  // namespace

// Keep the process-level scenarios in execution order for failure diagnosis.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
int main(int argc, char** argv) {
  // root, mode, optional snapshot scheme/accent
  if (argc < 3) {
    return 1;
  }
  const std::span<char*> arguments(argv, static_cast<std::size_t>(argc));
  const QByteArray root(arguments[1]);
  const QByteArray mode(arguments[2]);
  const bool snapshot = argc == 5;
  QTemporaryDir directory;
  const QString path = directory.filePath(QStringLiteral("appearance.toml"));
  if (!replace(path, document(snapshot ? QByteArray(arguments[3]) : "holonight-dark",
                              snapshot ? QByteArray(arguments[4]) : "blue"))) {
    return 2;
  }
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toUtf8());
  qputenv("QT_QPA_PLATFORMTHEME", "holonight");
  qputenv("QT_PLUGIN_PATH", root);
  qputenv("QT_STYLE_OVERRIDE", mode == "holonight" ? "Holonight" : "Fusion");
  qputenv("QT_QUICK_CONTROLS_STYLE", "Holonight");
  std::unique_ptr<QGuiApplication> app;
  if (mode == "gui" || mode == "quick") {
    app = std::make_unique<QGuiApplication>(argc, argv);
  } else {
    app = std::make_unique<QApplication>(argc, argv);
  }
  QFile maps(QStringLiteral("/proc/self/maps"));
  if (!maps.open(QIODevice::ReadOnly) || !maps.readAll().contains(root + "/platformthemes/libqholonight.so")) {
    qCritical("Requested platform plugin was not mapped");
    return 3;
  }
  if (snapshot) {
    QFile output;
    if (!output.open(stdout, QIODevice::WriteOnly)) {
      return 11;
    }
    output.write(colors(QGuiApplication::palette()));
    return 0;
  }
  QQmlEngine engine;
  QQmlComponent component(&engine);
  component.setData(mode == "quick"
                        ? "import QtQuick\nimport QtQuick.Controls as Controls\n"
                          "Controls.ApplicationWindow { property color observed: button.palette.window; property color "
                          "observedAccent: button.palette.highlight; Controls.Button { id: button } }"
                        : "import QtQuick\nSystemPalette { property color observedAccent: highlight }",
                    QUrl());
  std::unique_ptr<QObject> object(component.create());
  if (!object) {
    qCritical() << component.errors();
    return 4;
  }
  Events events;
  app->installEventFilter(&events);
  QSignalSpy schemes(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged);
  QPalette explicit_palette;
  if (mode == "override") {
    explicit_palette.setColor(QPalette::Active, QPalette::Text, QColor("#123456"));
    QGuiApplication::setPalette(explicit_palette);
  }
  const auto mask = QGuiApplication::palette().resolveMask();
  QTest::qWait(200);  // Allow the deferred watcher to attach before the first replacement.
  // These accents differ semantically but produce the same effective QPalette.
  const int unchanged_events = events.changes;
  for (const QByteArray& accent : {QByteArray("default"), QByteArray("blue")}) {
    if (!replace(path, document("holonight-dark", accent))) {
      return 12;
    }
    QTest::qWait(300);
    if (events.changes != unchanged_events || !schemes.isEmpty()) {
      qCritical("Effective palette did not change but the platform notified Qt");
      return 13;
    }
  }
  for (const auto& selection : {
           qMakePair(QByteArray("holonight-light"), QByteArray("blue")),
           qMakePair(QByteArray("holonight-dark"), QByteArray("blue")),
           qMakePair(QByteArray("holonight-mocha"), QByteArray("blue")),
           qMakePair(QByteArray("holonight-mocha"), QByteArray("violet")),
       }) {
    QProcess reference;
    reference.start(QCoreApplication::applicationFilePath(), {
                                                                 QString::fromUtf8(root),
                                                                 QString::fromUtf8(mode),
                                                                 QString::fromUtf8(selection.first),
                                                                 QString::fromUtf8(selection.second),
                                                             });
    if (!reference.waitForFinished(10000) || reference.exitCode() != 0) {
      qCritical() << reference.readAllStandardError();
      return 5;
    }
    QByteArray expected = reference.readAllStandardOutput();
    if (mode == "override") {
      auto lines = expected.split('\n');
      lines[2] = "#ff123456";
      expected = lines.join('\n');
    }
    const int before = events.changes;
    const auto scheme_signals = schemes.count();
    if (!replace(path, document(selection.first, selection.second)) ||
        !QTest::qWaitFor([&] { return colors(QGuiApplication::palette()) == expected; }, 5000)) {
      qCritical() << "Palette did not converge" << mode << selection << colors(QGuiApplication::palette()) << expected;
      return 6;
    }
    QTest::qWait(200);
    if (events.changes <= before ||
        schemes.count() - scheme_signals != (selection.first == "holonight-mocha" ? 0 : 1) ||
        object->property(mode == "quick" ? "observed" : "window").value<QColor>() !=
            QGuiApplication::palette().color(QPalette::Window) ||
        object->property("observedAccent").value<QColor>() != QGuiApplication::palette().color(QPalette::Highlight) ||
        (mode == "override" && QGuiApplication::palette().resolveMask() != mask)) {
      qCritical() << "Notification, QML observation, or resolve mask mismatch" << selection << events.changes << before
                  << schemes.count() << scheme_signals << object->property(mode == "quick" ? "observed" : "window")
                  << expected;
      return 7;
    }
  }
  const auto stable = colors(QGuiApplication::palette());
  const int count = events.changes;
  for (const auto& bytes : {
           QByteArray("version = ["),
           document("unsupported", "blue"),
           document("holonight-mocha", "violet") + "\n# equivalent replacement\n",
       }) {
    if (!replace(path, bytes)) {
      return 8;
    }
    QTest::qWait(400);
    if (colors(QGuiApplication::palette()) != stable || events.changes != count) {
      return 9;
    }
  }
  // Recover after rejected documents, then leave a replacement pending during teardown.
  replace(path, document("holonight-light", "blue"));
  if (!QTest::qWaitFor([&] { return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light; }, 5000)) {
    return 10;
  }
  replace(path, document("holonight-dark", "blue"));
  return 0;
}
