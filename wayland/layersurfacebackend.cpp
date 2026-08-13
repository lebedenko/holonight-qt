#include "layersurfacebackend_p.h"
#include "wayland-wlr-layer-shell-unstable-v1-client-protocol.h"

#include <QGuiApplication>
#include <QPointer>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickItem>
#include <QQuickView>
#include <QScreen>
#include <QtGui/qguiapplication_platform.h>
#include <qpa/qplatformwindow_p.h>
#include <qscreen_platform.h>

#include <holonight/wayland/layershellcontext.h>
#include <wayland-client.h>

namespace Holonight::Wayland {

static QString qmlErrors(const QQuickView* view) {
  QStringList messages;
  for (const QQmlError& error : view->errors()) messages.append(error.toString());
  return messages.join(QStringLiteral("; "));
}

class WaylandLayerSurfaceBackend final : public LayerSurfaceBackend {
 public:
  ~WaylandLayerSurfaceBackend() override {
    destroyRole();
    if (view_ != nullptr) delete view_.data();
  }

  bool isAvailable() const override { return LayerShellContext::instance()->isAvailable(); }
  quint32 version() const override { return LayerShellContext::instance()->version(); }
  QString diagnostic() const override { return LayerShellContext::instance()->diagnostic(); }

  bool create(const LayerSurfaceSpec& spec, ConfigureHandler configure, CloseHandler close, FailureHandler failure,
              QString* reason) override {
    configure_ = std::move(configure);
    close_ = std::move(close);
    failure_ = std::move(failure);
    spec_ = spec;
    auto* candidate = spec.engine != nullptr ? new QQuickView(spec.engine, nullptr) : new QQuickView();
    candidate->setScreen(spec.output);
    candidate->setResizeMode(QQuickView::SizeRootObjectToView);
    candidate->setFlags(spec.window_flags);
    candidate->setColor(spec.color);
    candidate->resize(spec.width, spec.height);
    candidate->create();
    auto* native_window = candidate->nativeInterface<QNativeInterface::Private::QWaylandWindow>();
    auto* native_screen = spec.output->nativeInterface<QNativeInterface::QWaylandScreen>();
    if (native_window == nullptr || native_window->surface() == nullptr || native_screen == nullptr ||
        native_screen->output() == nullptr) {
      delete candidate;
      *reason = QStringLiteral("could not obtain native Wayland surface and output handles");
      return false;
    }
    view_ = candidate;
    surface_ = native_window->surface();
    const QByteArray name_space = spec.name_space.toUtf8();
    role_ = zwlr_layer_shell_v1_get_layer_surface(
        static_cast<zwlr_layer_shell_v1*>(LayerShellContext::instance()->nativeLayerShell()), surface_,
        native_screen->output(), static_cast<quint32>(spec.layer), name_space.constData());
    if (role_ == nullptr) {
      *reason = QStringLiteral("the compositor refused to create a layer-surface role");
      queueWindowDestruction();
      return false;
    }
    static const zwlr_layer_surface_v1_listener listener{
        .configure =
            [](void* data, zwlr_layer_surface_v1*, quint32 serial, quint32 width, quint32 height) {
              auto* self = static_cast<WaylandLayerSurfaceBackend*>(data);
              self->configure_(serial, width, height);
            },
        .closed = [](void* data, zwlr_layer_surface_v1*) { static_cast<WaylandLayerSurfaceBackend*>(data)->close_(); },
    };
    zwlr_layer_surface_v1_add_listener(role_, &listener, this);
    QObject::connect(spec.output, &QObject::destroyed, candidate, [this]() { close_(); });
    QObject::connect(LayerShellContext::instance(), &LayerShellContext::availabilityChanged, candidate, [this]() {
      if (!isAvailable()) close_();
    });
    setSize(spec.width, spec.height);
    setAnchors(spec.anchors);
    setExclusiveZone(spec.exclusive_zone);
    setMargins(spec.margin_top, spec.margin_right, spec.margin_bottom, spec.margin_left);
    setKeyboardInteractivity(spec.keyboard_interactivity);
    setInputRegion(spec.input_region_policy, spec.input_region);
    if (spec.before_load) spec.before_load(candidate->engine());
    candidate->setInitialProperties(spec.initial_properties);
    candidate->setSource(spec.qml_url);
    if (candidate->status() == QQuickView::Error) {
      *reason = QStringLiteral("could not load layer-surface QML: %1").arg(qmlErrors(candidate));
      return false;
    }
    QObject::connect(candidate, &QQuickView::statusChanged, candidate, [this](QQuickView::Status status) {
      if (status == QQuickView::Error && failure_) {
        failure_(QStringLiteral("could not load layer-surface QML: %1").arg(qmlErrors(view_)));
      }
    });
    return true;
  }

  QQuickView* view() const override { return view_; }
  QObject* rootObject() const override { return view_ != nullptr ? view_->rootObject() : nullptr; }
  QQmlEngine* engine() const override { return view_ != nullptr ? view_->engine() : nullptr; }
  bool hasRole() const override { return role_ != nullptr; }
  void ackConfigure(quint32 serial) override { zwlr_layer_surface_v1_ack_configure(role_, serial); }
  void applyConfigureSize(quint32 width, quint32 height) override {
    if (width != 0) view_->setWidth(static_cast<int>(width));
    if (height != 0) view_->setHeight(static_cast<int>(height));
  }
  void show() override { view_->show(); }
  void setSize(int width, int height) override { zwlr_layer_surface_v1_set_size(role_, width, height); }
  void setMargins(int top, int right, int bottom, int left) override {
    zwlr_layer_surface_v1_set_margin(role_, top, right, bottom, left);
  }
  void setAnchors(Anchors anchors) override { zwlr_layer_surface_v1_set_anchor(role_, anchors.toInt()); }
  void setExclusiveZone(int zone) override { zwlr_layer_surface_v1_set_exclusive_zone(role_, zone); }
  void setKeyboardInteractivity(KeyboardInteractivity value) override {
    zwlr_layer_surface_v1_set_keyboard_interactivity(role_, static_cast<quint32>(value));
  }
  void setInputRegion(InputRegionPolicy policy, const QRegion& requested_region) override {
    if (policy == InputRegionPolicy::Default) {
      wl_surface_set_input_region(surface_, nullptr);
      return;
    }
    auto* native = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (native == nullptr || native->compositor() == nullptr) return;
    wl_region* region = wl_compositor_create_region(native->compositor());
    if (policy == InputRegionPolicy::Region) {
      for (const QRect& rectangle : requested_region)
        wl_region_add(region, rectangle.x(), rectangle.y(), rectangle.width(), rectangle.height());
    }
    wl_surface_set_input_region(surface_, region);
    wl_region_destroy(region);
  }
  void commit() override { wl_surface_commit(surface_); }
  void destroyRole() override {
    if (role_ == nullptr) return;
    zwlr_layer_surface_v1_destroy(role_);
    role_ = nullptr;
    surface_ = nullptr;
  }
  void queueWindowDestruction() override {
    QPointer<QQuickView> doomed = view_;
    view_.clear();
    if (doomed != nullptr) doomed->deleteLater();
  }

 private:
  LayerSurfaceSpec spec_;
  QPointer<QQuickView> view_;
  wl_surface* surface_{nullptr};
  zwlr_layer_surface_v1* role_{nullptr};
  ConfigureHandler configure_;
  CloseHandler close_;
  FailureHandler failure_;
};

std::unique_ptr<LayerSurfaceBackend> createLayerSurfaceBackend() {
  return std::make_unique<WaylandLayerSurfaceBackend>();
}

}  // namespace Holonight::Wayland
