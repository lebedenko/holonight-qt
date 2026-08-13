#pragma once

#include <QObject>
#include <QPointer>
#include <QString>

#include <holonight/wayland/layersurfacespec.h>
#include <memory>

class QQmlEngine;
class QQuickView;

namespace Holonight::Wayland {

class LayerSurfaceBackend;
class LayerSurfaceHostTestAccess;

class LayerSurfaceHost final : public QObject {
  Q_OBJECT
  Q_PROPERTY(State state READ state NOTIFY stateChanged)
  Q_PROPERTY(QString diagnostic READ diagnostic NOTIFY diagnosticChanged)

 public:
  enum class State { Idle, WaitingForConfigure, Configured, Closing, Closed, Failed };
  Q_ENUM(State)

  explicit LayerSurfaceHost(QObject* parent = nullptr);
  ~LayerSurfaceHost() override;

  bool open(const LayerSurfaceSpec& spec);
  Q_INVOKABLE void close();

  [[nodiscard]] State state() const;
  [[nodiscard]] QString diagnostic() const;
  [[nodiscard]] QQuickView* view() const;
  [[nodiscard]] QObject* rootObject() const;
  [[nodiscard]] QQmlEngine* engine() const;

  bool setSize(int width, int height);
  bool setMargins(int top, int right, int bottom, int left);
  bool setAnchors(Anchors anchors);
  bool setExclusiveZone(int exclusive_zone);
  bool setKeyboardInteractivity(KeyboardInteractivity interactivity);
  bool setInputRegion(InputRegionPolicy policy, const QRegion& region = {});

 Q_SIGNALS:
  void configured();
  void closed();
  void failed(const QString& diagnostic);
  void stateChanged();
  void diagnosticChanged();

 private:
  friend class LayerSurfaceHostTestAccess;
  explicit LayerSurfaceHost(std::unique_ptr<LayerSurfaceBackend> backend, QObject* parent = nullptr);
  bool validate(const LayerSurfaceSpec& spec, QString* reason) const;
  void handleConfigure(quint32 serial, quint32 width, quint32 height);
  void terminate(State terminal_state, const QString& reason = {});

  State state_{State::Idle};
  QString diagnostic_;
  LayerSurfaceSpec spec_;
  std::unique_ptr<LayerSurfaceBackend> backend_;
};

}  // namespace Holonight::Wayland
