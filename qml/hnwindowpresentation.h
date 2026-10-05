#pragma once
#include "hnwindowdecoration.h"

#include <QPointer>
#include <QQuickWindow>
#include <QtQml/qqmlregistration.h>

#include <holonight_system/compositor/CompositorFactory.h>

class HnWindowPresentationTestPeer;
class HnWindowPresentation : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QQuickWindow* window READ window WRITE setWindow NOTIFY windowChanged FINAL)
  Q_PROPERTY(State externalTitleBarState READ externalTitleBarState NOTIFY externalTitleBarStateChanged FINAL)
 public:
  enum class State : quint8 { Unknown, Present, Absent };
  Q_ENUM(State)
  explicit HnWindowPresentation(QObject* parent = nullptr);
  ~HnWindowPresentation() override;
  Q_DISABLE_COPY_MOVE(HnWindowPresentation)
  [[nodiscard]] QQuickWindow* window() const { return window_; }
  void setWindow(QQuickWindow* window);
  [[nodiscard]] State externalTitleBarState() const { return state_; }
 signals:
  void windowChanged();
  void externalTitleBarStateChanged();

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  friend class HnWindowPresentationTestPeer;
  void refresh();
  void acceptSnapshot(const CompositorSnapshot& snapshot, quint64 generation);
  void resetObservation();
  void setState(State state);
  HnWindowDecoration decoration_;
  QPointer<QQuickWindow> window_;
  std::unique_ptr<CompositorBackend> backend_;
  ExternalTitleBarState observation_{ExternalTitleBarState::Unknown};
  State state_{State::Unknown};
  quint64 generation_{0};
  bool surface_destroying_{false};
};
