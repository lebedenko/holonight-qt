// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/appearance_reader.h"
#include "themeresolver.h"

#include <QGuiApplication>
#include <QPointer>
#include <QQuickWindow>
#include <QtQuickControls2/private/qquickstyleplugin_p.h>
#include <QtQuickTemplates2/private/qquicktheme_p_p.h>

#include <holonight/config/config.h>

void qml_register_types_Holonight();

class HolonightStylePlugin : public QQuickStylePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
 public:
  HolonightStylePlugin() {
    volatile auto registration = &qml_register_types_Holonight;
    Q_UNUSED(registration);
  }
  ~HolonightStylePlugin() override {
    if (reader_) {
      reader_->disconnect();
      reader_->deleteLater();
    }
  }
  QString name() const override { return QStringLiteral("Holonight"); }
  void initializeTheme(QQuickTheme* theme) override {
    active_ = true;
    // The loader blocks the GUI thread here. Read synchronously without constructing watchers.
    auto appearance = HoloNight::Config::defaults();
    const auto path = HoloNight::Config::resolveAppearancePath();
    if (path) {
      const auto loaded = HoloNight::Config::load(*path.value);
      if (loaded) appearance = loaded.value->appearance;
    }
    auto resolved = Holonight::resolveAppearance(appearance);
    if (!resolved) resolved = Holonight::resolveAppearance(HoloNight::Config::defaults());
    theme->setUsePlatformPalette(false);
    tokens_ = Holonight::ThemeResolver::resolve(*resolved.value);
    applyPalette(theme);
    QMetaObject::invokeMethod(qGuiApp, [guard = QPointer<HolonightStylePlugin>(this)] {
      if (!guard || !guard->active_) return;
      auto* self = guard.data();
      self->startWatcher();
    });
  }
  void unregisterTypes() override {
    active_ = false;
    if (reader_) {
      reader_->disconnect();
      reader_->deleteLater();
      reader_ = nullptr;
    }
    QQuickStylePlugin::unregisterTypes();
  }
  void updateTheme() override {
    if (auto* theme = QQuickTheme::instance()) {
      applyPalette(theme);
      notifyWindows();
    }
  }

 private:
  void applyPalette(QQuickTheme* theme) {
    theme->setPalette(QQuickTheme::System, Holonight::buildPalette(tokens_));
    // Configuration supplies defaults; explicit application roles take precedence.
    auto& system = QQuickThemePrivate::get(theme)->palettes[QQuickTheme::System];
    *system = QGuiApplication::palette().resolve(*system);
  }
  static void notifyWindows() {
    for (auto* window : QGuiApplication::allWindows()) {
      QEvent event(QEvent::ApplicationPaletteChange);
      QCoreApplication::sendEvent(window, &event);
    }
  }
  void startWatcher() {
    if (reader_) return;
    reader_ = new Holonight::AppearanceReader(qGuiApp);
    connect(reader_, &Holonight::AppearanceReader::paletteChanged, reader_, [this] {
      if (!QQuickTheme::instance()) return;
      tokens_ = Holonight::ThemeResolver::resolve(reader_->appearance());
      applyPalette(QQuickTheme::instance());
      notifyWindows();
    });
    const auto current = Holonight::ThemeResolver::resolve(reader_->appearance());
    if (current != tokens_) {
      tokens_ = current;
      updateTheme();
    }
  }
  bool active_ = false;
  QPointer<Holonight::AppearanceReader> reader_;
  Holonight::ColorTokens tokens_;
};

#include "quickstyleplugin.moc"
