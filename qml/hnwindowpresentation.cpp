#include "hnwindowpresentation.h"

#include "hnwindowpresentation_p.h"

#include <QCoreApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QPlatformSurfaceEvent>

HnWindowPresentation::HnWindowPresentation(QObject* parent) : QObject(parent) {
  connect(&decoration_, &HnWindowDecoration::modeChanged, this, [this] {
    resetObservation();
    refresh();
  });
}
HnWindowPresentation::~HnWindowPresentation() {
  if (window_) {
    window_->removeEventFilter(this);
  }
}
void HnWindowPresentation::setState(State state) {
  if (state_ == state) {
    return;
  }
  state_ = state;
  emit externalTitleBarStateChanged();
}
void HnWindowPresentation::resetObservation() {
  ++generation_;
  backend_.reset();
  observation_ = ExternalTitleBarState::Unknown;
  setState(State::Unknown);
}
void HnWindowPresentation::setWindow(QQuickWindow* window) {
  if (window_ == window) {
    return;
  }
  if (window_) {
    window_->removeEventFilter(this);
    disconnect(window_, nullptr, this, nullptr);
  }
  resetObservation();
  window_ = window;
  surface_destroying_ = false;
  decoration_.setWindow(window);
  if (window_) {
    window_->installEventFilter(this);
    connect(window_, &QWindow::visibilityChanged, this, [this] {
      resetObservation();
      refresh();
    });
    connect(window_, &QWindow::windowStateChanged, this, [this] {
      resetObservation();
      refresh();
    });
    connect(window_, &QObject::destroyed, this, [this] {
      window_ = nullptr;
      resetObservation();
      emit windowChanged();
    });
  }
  refresh();
  emit windowChanged();
}
void HnWindowPresentation::refresh() {
  if (!window_ || surface_destroying_) {
    setState(State::Unknown);
    return;
  }
  if (window_->flags().testFlag(Qt::FramelessWindowHint) || window_->windowStates().testFlag(Qt::WindowFullScreen)) {
    setState(State::Absent);
    return;
  }
#ifdef HOLONIGHT_WINDOW_DECORATION_WAYLAND
  if (!QGuiApplication::platformName().startsWith(QStringLiteral("wayland")) || (window_->handle() == nullptr) ||
      !window_->isVisible() || !window_->isExposed()) {
    setState(State::Unknown);
    return;
  }
  if (decoration_.mode() == HnWindowDecoration::Mode::ToolkitClientSide) {
    setState(Holonight::Private::titleBarState(decoration_.mode(), observation_, true));
    return;
  }
  if (!backend_) {
    backend_ = createCompositorBackend();
    const auto generation = generation_;
    connect(backend_.get(), &CompositorBackend::snapshotReady, this,
            [this, generation](const CompositorSnapshot& snapshot) { acceptSnapshot(snapshot, generation); });
    backend_->start();
  }
  if (decoration_.mode() == HnWindowDecoration::Mode::ServerSide) {
    setState(Holonight::Private::titleBarState(decoration_.mode(), observation_, true));
    return;
  }
#endif
  setState(State::Unknown);
}
void HnWindowPresentation::acceptSnapshot(const CompositorSnapshot& snapshot, quint64 generation) {
  if (generation != generation_ || !window_ || surface_destroying_) {
    return;
  }
  auto app_id = QGuiApplication::desktopFileName();
  if (app_id.endsWith(QStringLiteral(".desktop"))) {
    app_id.chop(8);
  }
  observation_ =
      externalTitleBarForApplication(snapshot, static_cast<quint32>(QCoreApplication::applicationPid()), app_id);
  refresh();
}
bool HnWindowPresentation::eventFilter(QObject* watched, QEvent* event) {
  if (watched == window_) {
    if (event->type() == QEvent::PlatformSurface) {
      const auto* surface = dynamic_cast<QPlatformSurfaceEvent*>(event);
      if (surface == nullptr) {
        return QObject::eventFilter(watched, event);
      }
      surface_destroying_ = surface->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed;
      resetObservation();
    }
    if (event->type() == QEvent::PlatformSurface || event->type() == QEvent::Expose || event->type() == QEvent::Show ||
        event->type() == QEvent::Hide || event->type() == QEvent::WindowStateChange) {
      QMetaObject::invokeMethod(
          this,
          [this] {
            refresh();
            if (backend_) {
              backend_->requestSnapshotRefresh();
            }
          },
          Qt::QueuedConnection);
    }
  }
  return QObject::eventFilter(watched, event);
}
