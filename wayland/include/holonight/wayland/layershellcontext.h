#pragma once

#include <QObject>
#include <QString>

namespace Holonight::Wayland {

class LayerSurfaceHost;
class WaylandLayerSurfaceBackend;

class LayerShellContext final : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool available READ isAvailable NOTIFY availabilityChanged)
  Q_PROPERTY(quint32 version READ version NOTIFY availabilityChanged)
  Q_PROPERTY(QString diagnostic READ diagnostic NOTIFY availabilityChanged)

 public:
  static LayerShellContext* instance();

  [[nodiscard]] bool isAvailable() const;
  [[nodiscard]] quint32 version() const;
  [[nodiscard]] QString diagnostic() const;

 Q_SIGNALS:
  void availabilityChanged();

 private:
  friend class LayerSurfaceHost;
  friend class WaylandLayerSurfaceBackend;
  explicit LayerShellContext(QObject* parent = nullptr);
  ~LayerShellContext() override;

  void initialize();
  void setUnavailable(const QString& diagnostic);
  [[nodiscard]] void* nativeLayerShell() const;

  void* registry_{nullptr};
  void* layer_shell_{nullptr};
  quint32 global_name_{0};
  quint32 version_{0};
  QString diagnostic_;
};

}  // namespace Holonight::Wayland
