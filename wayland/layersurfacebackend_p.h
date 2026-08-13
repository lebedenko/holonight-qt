#pragma once

#include <QString>

#include <functional>
#include <holonight/wayland/layersurfacespec.h>
#include <memory>

class QObject;
class QQmlEngine;
class QQuickView;

namespace Holonight::Wayland {

class LayerSurfaceBackend {
 public:
  using ConfigureHandler = std::function<void(quint32, quint32, quint32)>;
  using CloseHandler = std::function<void()>;
  using FailureHandler = std::function<void(const QString&)>;

  virtual ~LayerSurfaceBackend() = default;
  [[nodiscard]] virtual bool isAvailable() const = 0;
  [[nodiscard]] virtual quint32 version() const = 0;
  [[nodiscard]] virtual QString diagnostic() const = 0;
  virtual bool create(const LayerSurfaceSpec& spec, ConfigureHandler configure, CloseHandler close,
                      FailureHandler failure, QString* reason) = 0;
  [[nodiscard]] virtual QQuickView* view() const = 0;
  [[nodiscard]] virtual QObject* rootObject() const = 0;
  [[nodiscard]] virtual QQmlEngine* engine() const = 0;
  [[nodiscard]] virtual bool hasRole() const = 0;
  virtual void ackConfigure(quint32 serial) = 0;
  virtual void applyConfigureSize(quint32 width, quint32 height) = 0;
  virtual void show() = 0;
  virtual void setSize(int width, int height) = 0;
  virtual void setMargins(int top, int right, int bottom, int left) = 0;
  virtual void setAnchors(Anchors anchors) = 0;
  virtual void setExclusiveZone(int exclusive_zone) = 0;
  virtual void setKeyboardInteractivity(KeyboardInteractivity interactivity) = 0;
  virtual void setInputRegion(InputRegionPolicy policy, const QRegion& region) = 0;
  virtual void commit() = 0;
  virtual void destroyRole() = 0;
  virtual void queueWindowDestruction() = 0;
};

std::unique_ptr<LayerSurfaceBackend> createLayerSurfaceBackend();

}  // namespace Holonight::Wayland
