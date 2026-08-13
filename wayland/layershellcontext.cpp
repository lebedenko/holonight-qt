#include "wayland-wlr-layer-shell-unstable-v1-client-protocol.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QtGui/qguiapplication_platform.h>

#include <algorithm>
#include <cstring>
#include <holonight/wayland/layershellcontext.h>
#include <wayland-client.h>

namespace Holonight::Wayland {

namespace {
constexpr quint32 kSupportedVersion = 4;
const QString kUnavailableDiagnostic = QStringLiteral(
    "wlr-layer-shell is unavailable; run with Qt's Wayland platform on a compositor that advertises "
    "zwlr_layer_shell_v1");
}  // namespace

LayerShellContext* LayerShellContext::instance() {
  static LayerShellContext context;
  return &context;
}

LayerShellContext::LayerShellContext(QObject* parent) : QObject(parent), diagnostic_(kUnavailableDiagnostic) {
  initialize();
}

LayerShellContext::~LayerShellContext() {
  if (layer_shell_ != nullptr) {
    auto* shell = static_cast<zwlr_layer_shell_v1*>(layer_shell_);
    if (version_ >= ZWLR_LAYER_SHELL_V1_DESTROY_SINCE_VERSION) {
      zwlr_layer_shell_v1_destroy(shell);
    } else {
      wl_proxy_destroy(reinterpret_cast<wl_proxy*>(shell));  // NOLINT
    }
  }
  if (registry_ != nullptr) {
    wl_registry_destroy(static_cast<wl_registry*>(registry_));
  }
}

bool LayerShellContext::isAvailable() const { return layer_shell_ != nullptr; }
quint32 LayerShellContext::version() const { return version_; }
QString LayerShellContext::diagnostic() const { return diagnostic_; }
void* LayerShellContext::nativeLayerShell() const { return layer_shell_; }

void LayerShellContext::initialize() {
  if (qGuiApp == nullptr) {
    setUnavailable(QStringLiteral("wlr-layer-shell initialization requires a QGuiApplication"));
    return;
  }
  auto* native = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (native == nullptr || native->display() == nullptr) {
    setUnavailable(kUnavailableDiagnostic);
    return;
  }
  auto* registry = wl_display_get_registry(native->display());
  if (registry == nullptr) {
    setUnavailable(QStringLiteral("could not obtain the Wayland registry for wlr-layer-shell"));
    return;
  }
  registry_ = registry;
  static const wl_registry_listener listener{
      .global =
          [](void* data, wl_registry* registry_handle, quint32 name, const char* interface, quint32 version) {
            auto* self = static_cast<LayerShellContext*>(data);
            if (self->layer_shell_ != nullptr || std::strcmp(interface, zwlr_layer_shell_v1_interface.name) != 0) {
              return;
            }
            self->version_ = std::min(version, kSupportedVersion);
            self->global_name_ = name;
            self->layer_shell_ =
                wl_registry_bind(registry_handle, name, &zwlr_layer_shell_v1_interface, self->version_);
            self->diagnostic_.clear();
            Q_EMIT self->availabilityChanged();
          },
      .global_remove =
          [](void* data, wl_registry*, quint32 name) {
            auto* self = static_cast<LayerShellContext*>(data);
            if (name != self->global_name_) {
              return;
            }
            auto* shell = static_cast<zwlr_layer_shell_v1*>(self->layer_shell_);
            if (shell != nullptr) {
              if (self->version_ >= ZWLR_LAYER_SHELL_V1_DESTROY_SINCE_VERSION) {
                zwlr_layer_shell_v1_destroy(shell);
              } else {
                wl_proxy_destroy(reinterpret_cast<wl_proxy*>(shell));  // NOLINT
              }
            }
            self->layer_shell_ = nullptr;
            self->global_name_ = 0;
            self->version_ = 0;
            self->diagnostic_ = kUnavailableDiagnostic;
            Q_EMIT self->availabilityChanged();
          },
  };
  wl_registry_add_listener(registry, &listener, this);
  if (wl_display_roundtrip(native->display()) < 0 || layer_shell_ == nullptr) {
    setUnavailable(kUnavailableDiagnostic);
  }
}

void LayerShellContext::setUnavailable(const QString& diagnostic) {
  const bool changed = layer_shell_ != nullptr || version_ != 0 || diagnostic_ != diagnostic;
  layer_shell_ = nullptr;
  global_name_ = 0;
  version_ = 0;
  diagnostic_ = diagnostic;
  if (changed) {
    Q_EMIT availabilityChanged();
  }
}

}  // namespace Holonight::Wayland
