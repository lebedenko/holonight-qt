#include "layersurfacebackend_p.h"

#include <QQmlEngine>
#include <QQuickView>

#include <holonight/wayland/layersurfacehost.h>

namespace Holonight::Wayland {

namespace {
constexpr quint32 kAllAnchors = 0x0f;
}

LayerSurfaceHost::LayerSurfaceHost(QObject* parent) : LayerSurfaceHost(createLayerSurfaceBackend(), parent) {}
LayerSurfaceHost::LayerSurfaceHost(std::unique_ptr<LayerSurfaceBackend> backend, QObject* parent)
    : QObject(parent), backend_(std::move(backend)) {}
LayerSurfaceHost::~LayerSurfaceHost() {
  if (state_ == State::WaitingForConfigure || state_ == State::Configured) terminate(State::Closed);
}

bool LayerSurfaceHost::open(const LayerSurfaceSpec& spec) {
  if (state_ != State::Idle && state_ != State::Closed) return false;
  QString reason;
  if (!validate(spec, &reason)) {
    terminate(State::Failed, reason);
    return false;
  }
  spec_ = spec;
  const bool created = backend_->create(
      spec, [this](quint32 serial, quint32 width, quint32 height) { handleConfigure(serial, width, height); },
      [this]() { close(); }, [this](const QString& failure) { terminate(State::Failed, failure); }, &reason);
  if (!created) {
    terminate(State::Failed, reason);
    return false;
  }
  diagnostic_.clear();
  state_ = State::WaitingForConfigure;
  Q_EMIT diagnosticChanged();
  Q_EMIT stateChanged();
  backend_->commit();
  return true;
}

void LayerSurfaceHost::close() {
  if (state_ != State::WaitingForConfigure && state_ != State::Configured) return;
  terminate(State::Closed);
}
LayerSurfaceHost::State LayerSurfaceHost::state() const { return state_; }
QString LayerSurfaceHost::diagnostic() const { return diagnostic_; }
QQuickView* LayerSurfaceHost::view() const {
  return state_ == State::WaitingForConfigure || state_ == State::Configured ? backend_->view() : nullptr;
}
QObject* LayerSurfaceHost::rootObject() const {
  return state_ == State::WaitingForConfigure || state_ == State::Configured ? backend_->rootObject() : nullptr;
}
QQmlEngine* LayerSurfaceHost::engine() const {
  return state_ == State::WaitingForConfigure || state_ == State::Configured ? backend_->engine() : nullptr;
}

bool LayerSurfaceHost::setSize(int width, int height) {
  if (!backend_->hasRole() || width < 0 || height < 0 ||
      (width == 0 && !spec_.anchors.testFlags(Anchor::Left | Anchor::Right)) ||
      (height == 0 && !spec_.anchors.testFlags(Anchor::Top | Anchor::Bottom)))
    return false;
  spec_.width = width;
  spec_.height = height;
  backend_->setSize(width, height);
  backend_->commit();
  return true;
}
bool LayerSurfaceHost::setMargins(int top, int right, int bottom, int left) {
  if (!backend_->hasRole()) return false;
  spec_.margin_top = top;
  spec_.margin_right = right;
  spec_.margin_bottom = bottom;
  spec_.margin_left = left;
  backend_->setMargins(top, right, bottom, left);
  backend_->commit();
  return true;
}
bool LayerSurfaceHost::setAnchors(Anchors anchors) {
  if (!backend_->hasRole() || (anchors.toInt() & ~kAllAnchors) != 0 ||
      (spec_.width == 0 && !anchors.testFlags(Anchor::Left | Anchor::Right)) ||
      (spec_.height == 0 && !anchors.testFlags(Anchor::Top | Anchor::Bottom)))
    return false;
  spec_.anchors = anchors;
  backend_->setAnchors(anchors);
  backend_->commit();
  return true;
}
bool LayerSurfaceHost::setExclusiveZone(int zone) {
  if (!backend_->hasRole()) return false;
  spec_.exclusive_zone = zone;
  backend_->setExclusiveZone(zone);
  backend_->commit();
  return true;
}
bool LayerSurfaceHost::setKeyboardInteractivity(KeyboardInteractivity value) {
  const quint32 raw = static_cast<quint32>(value);
  if (!backend_->hasRole() || raw > static_cast<quint32>(KeyboardInteractivity::OnDemand) ||
      (value == KeyboardInteractivity::OnDemand && backend_->version() < 4))
    return false;
  spec_.keyboard_interactivity = value;
  backend_->setKeyboardInteractivity(value);
  backend_->commit();
  return true;
}
bool LayerSurfaceHost::setInputRegion(InputRegionPolicy policy, const QRegion& region) {
  if (!backend_->hasRole() || static_cast<int>(policy) < static_cast<int>(InputRegionPolicy::Default) ||
      static_cast<int>(policy) > static_cast<int>(InputRegionPolicy::Region))
    return false;
  spec_.input_region_policy = policy;
  spec_.input_region = region;
  backend_->setInputRegion(policy, region);
  backend_->commit();
  return true;
}

bool LayerSurfaceHost::validate(const LayerSurfaceSpec& spec, QString* reason) const {
  const auto reject = [reason](const QString& message) {
    *reason = message;
    return false;
  };
  const quint32 layer = static_cast<quint32>(spec.layer);
  const quint32 keyboard = static_cast<quint32>(spec.keyboard_interactivity);
  if (!backend_->isAvailable()) return reject(backend_->diagnostic());
  if (spec.output == nullptr) return reject(QStringLiteral("layer-surface output must not be null"));
  if (spec.name_space.isEmpty() || spec.name_space.contains(QChar::Null))
    return reject(QStringLiteral("layer-surface namespace must be a non-empty protocol string"));
  if (!spec.qml_url.isValid() || spec.qml_url.isEmpty())
    return reject(QStringLiteral("layer-surface QML URL is invalid"));
  if (spec.width < 0 || spec.height < 0) return reject(QStringLiteral("layer-surface dimensions must be non-negative"));
  if (layer > static_cast<quint32>(Layer::Overlay)) return reject(QStringLiteral("layer-surface layer is invalid"));
  if ((spec.anchors.toInt() & ~kAllAnchors) != 0) return reject(QStringLiteral("layer-surface anchors are invalid"));
  if (keyboard > static_cast<quint32>(KeyboardInteractivity::OnDemand))
    return reject(QStringLiteral("keyboard interactivity is invalid"));
  if (static_cast<int>(spec.input_region_policy) < static_cast<int>(InputRegionPolicy::Default) ||
      static_cast<int>(spec.input_region_policy) > static_cast<int>(InputRegionPolicy::Region))
    return reject(QStringLiteral("input-region policy is invalid"));
  if (spec.keyboard_interactivity == KeyboardInteractivity::OnDemand && backend_->version() < 4)
    return reject(QStringLiteral("on-demand keyboard interactivity requires layer-shell version 4"));
  if (spec.width == 0 && !spec.anchors.testFlags(Anchor::Left | Anchor::Right))
    return reject(QStringLiteral("zero width requires left and right anchors"));
  if (spec.height == 0 && !spec.anchors.testFlags(Anchor::Top | Anchor::Bottom))
    return reject(QStringLiteral("zero height requires top and bottom anchors"));
  return true;
}

void LayerSurfaceHost::handleConfigure(quint32 serial, quint32 width, quint32 height) {
  if (!backend_->hasRole() || (state_ != State::WaitingForConfigure && state_ != State::Configured)) return;
  backend_->ackConfigure(serial);
  backend_->applyConfigureSize(width, height);
  if (state_ == State::WaitingForConfigure) {
    state_ = State::Configured;
    Q_EMIT stateChanged();
    backend_->show();
  }
  Q_EMIT configured();
}

void LayerSurfaceHost::terminate(State terminal_state, const QString& reason) {
  const bool has_resources = backend_->hasRole() || backend_->view() != nullptr;
  if (has_resources) {
    state_ = State::Closing;
    Q_EMIT stateChanged();
  }
  backend_->destroyRole();
  backend_->queueWindowDestruction();
  state_ = terminal_state;
  diagnostic_ = reason;
  Q_EMIT diagnosticChanged();
  Q_EMIT stateChanged();
  if (terminal_state == State::Failed)
    Q_EMIT failed(reason);
  else
    Q_EMIT closed();
}

}  // namespace Holonight::Wayland
