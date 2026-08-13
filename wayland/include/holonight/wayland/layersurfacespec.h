#pragma once

#include <QColor>
#include <QFlags>
#include <QRegion>
#include <QString>
#include <QUrl>
#include <QVariantMap>
#include <Qt>

#include <functional>

class QQmlEngine;
class QScreen;

namespace Holonight::Wayland {

enum class Layer : quint32 { Background = 0, Bottom = 1, Top = 2, Overlay = 3 };

enum class Anchor : quint32 { Top = 1, Bottom = 2, Left = 4, Right = 8 };
Q_DECLARE_FLAGS(Anchors, Anchor)

enum class KeyboardInteractivity : quint32 { None = 0, Exclusive = 1, OnDemand = 2 };
enum class InputRegionPolicy { Default, Empty, Region };

struct LayerSurfaceSpec {
  QScreen* output{nullptr};
  QString name_space;
  Layer layer{Layer::Top};
  Anchors anchors{};
  int width{0};
  int height{0};
  int margin_top{0};
  int margin_right{0};
  int margin_bottom{0};
  int margin_left{0};
  int exclusive_zone{0};
  KeyboardInteractivity keyboard_interactivity{KeyboardInteractivity::None};
  InputRegionPolicy input_region_policy{InputRegionPolicy::Default};
  QRegion input_region;
  QUrl qml_url;
  QVariantMap initial_properties;
  Qt::WindowFlags window_flags{Qt::FramelessWindowHint | Qt::BypassWindowManagerHint};
  QColor color{Qt::transparent};
  QQmlEngine* engine{nullptr};
  std::function<void(QQmlEngine*)> before_load;
};

}  // namespace Holonight::Wayland

Q_DECLARE_OPERATORS_FOR_FLAGS(Holonight::Wayland::Anchors)
