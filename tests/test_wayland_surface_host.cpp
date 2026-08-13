#include "layersurfacebackend_p.h"

#include <QGuiApplication>
#include <QQuickItem>
#include <QQuickView>
#include <QScreen>
#include <QSignalSpy>

#include <gtest/gtest.h>
#include <holonight/wayland/layershellcontext.h>
#include <holonight/wayland/layersurfacehost.h>

namespace {

using Holonight::Wayland::LayerShellContext;
using Holonight::Wayland::LayerSurfaceHost;
using Holonight::Wayland::LayerSurfaceSpec;

class FakeBackend final : public Holonight::Wayland::LayerSurfaceBackend {
 public:
  bool isAvailable() const override { return available; }
  quint32 version() const override { return protocol_version; }
  QString diagnostic() const override { return QStringLiteral("fake protocol unavailable"); }
  bool create(const LayerSurfaceSpec&, ConfigureHandler configure_handler, CloseHandler close_handler,
              FailureHandler failure_handler, QString* reason) override {
    if (!create_succeeds) {
      *reason = QStringLiteral("fake creation failure");
      return false;
    }
    configure = std::move(configure_handler);
    close = std::move(close_handler);
    failure = std::move(failure_handler);
    quick_view = new QQuickView();
    role = true;
    events.append(QStringLiteral("create"));
    return true;
  }
  QQuickView* view() const override { return quick_view; }
  QObject* rootObject() const override { return quick_view == nullptr ? nullptr : quick_view->rootObject(); }
  QQmlEngine* engine() const override { return quick_view == nullptr ? nullptr : quick_view->engine(); }
  bool hasRole() const override { return role; }
  void ackConfigure(quint32 serial) override {
    acknowledgements.append(serial);
    events.append(QStringLiteral("ack"));
  }
  void applyConfigureSize(quint32 width, quint32 height) override { sizes.append(QSize(width, height)); }
  void show() override {
    ++show_count;
    events.append(QStringLiteral("show"));
  }
  void setSize(int width, int height) override { sizes.append(QSize(width, height)); }
  void setMargins(int top, int right, int bottom, int left) override { margins = {top, right, bottom, left}; }
  void setAnchors(Holonight::Wayland::Anchors value) override { anchors = value; }
  void setExclusiveZone(int value) override { exclusive_zone = value; }
  void setKeyboardInteractivity(Holonight::Wayland::KeyboardInteractivity value) override { keyboard = value; }
  void setInputRegion(Holonight::Wayland::InputRegionPolicy policy, const QRegion& region) override {
    input_policy = policy;
    input_region = region;
  }
  void commit() override {
    ++commit_count;
    events.append(QStringLiteral("commit"));
  }
  void destroyRole() override {
    if (role) {
      role = false;
      events.append(QStringLiteral("destroy-role"));
    }
  }
  void queueWindowDestruction() override {
    if (quick_view != nullptr) {
      events.append(QStringLiteral("queue-window"));
      quick_view->deleteLater();
      quick_view = nullptr;
    }
  }

  bool available{true};
  bool create_succeeds{true};
  quint32 protocol_version{4};
  bool role{false};
  QQuickView* quick_view{nullptr};
  int show_count{0};
  int commit_count{0};
  int exclusive_zone{0};
  QList<int> margins;
  QList<quint32> acknowledgements;
  QList<QSize> sizes;
  QStringList events;
  Holonight::Wayland::Anchors anchors;
  Holonight::Wayland::KeyboardInteractivity keyboard{};
  Holonight::Wayland::InputRegionPolicy input_policy{};
  QRegion input_region;
  ConfigureHandler configure;
  CloseHandler close;
  FailureHandler failure;
};

LayerSurfaceSpec validSpec() {
  LayerSurfaceSpec spec;
  spec.output = QGuiApplication::primaryScreen();
  spec.name_space = QStringLiteral("test-surface");
  spec.width = 100;
  spec.height = 40;
  spec.qml_url = QUrl(QStringLiteral("qrc:/test.qml"));
  return spec;
}

TEST(WaylandLayerShellContext, ReportsActionableDiagnosticOutsideWayland) {
  const auto* context = LayerShellContext::instance();
  EXPECT_FALSE(context->isAvailable());
  EXPECT_EQ(context->version(), 0U);
  EXPECT_TRUE(context->diagnostic().contains(QStringLiteral("Wayland")));
  EXPECT_TRUE(context->diagnostic().contains(QStringLiteral("zwlr_layer_shell_v1")));
}

TEST(WaylandLayerSurfaceHost, UnavailableProtocolFailsWithoutResources) {
  LayerSurfaceHost host;
  QSignalSpy failed_spy(&host, &LayerSurfaceHost::failed);

  EXPECT_FALSE(host.open(LayerSurfaceSpec{}));
  EXPECT_EQ(host.state(), LayerSurfaceHost::State::Failed);
  EXPECT_EQ(failed_spy.count(), 1);
  EXPECT_EQ(host.view(), nullptr);
  EXPECT_EQ(host.rootObject(), nullptr);
  EXPECT_EQ(host.engine(), nullptr);
  EXPECT_FALSE(host.diagnostic().isEmpty());
}

TEST(WaylandLayerSurfaceHost, RepeatedCloseAfterFailureIsIdempotent) {
  LayerSurfaceHost host;
  QSignalSpy closed_spy(&host, &LayerSurfaceHost::closed);
  EXPECT_FALSE(host.open(LayerSurfaceSpec{}));

  host.close();
  host.close();

  EXPECT_EQ(host.state(), LayerSurfaceHost::State::Failed);
  EXPECT_EQ(closed_spy.count(), 0);
  EXPECT_EQ(host.view(), nullptr);
}

TEST(WaylandLayerSurfaceHost, MutationsAreRejectedWithoutALiveRole) {
  LayerSurfaceHost host;
  EXPECT_FALSE(host.setSize(100, 20));
  EXPECT_FALSE(host.setMargins(1, 2, 3, 4));
  EXPECT_FALSE(host.setAnchors(Holonight::Wayland::Anchor::Top));
  EXPECT_FALSE(host.setExclusiveZone(20));
  EXPECT_FALSE(host.setKeyboardInteractivity(Holonight::Wayland::KeyboardInteractivity::Exclusive));
  EXPECT_FALSE(host.setInputRegion(Holonight::Wayland::InputRegionPolicy::Empty));
}

}  // namespace

namespace Holonight::Wayland {
class LayerSurfaceHostTestAccess {
 public:
  static std::unique_ptr<LayerSurfaceHost> create(std::unique_ptr<LayerSurfaceBackend> backend) {
    return std::unique_ptr<LayerSurfaceHost>(new LayerSurfaceHost(std::move(backend)));
  }
};
}  // namespace Holonight::Wayland

namespace {

TEST(WaylandLayerSurfaceHostLifecycle, ConfiguresAndOnlyShowsOnce) {
  auto fake = std::make_unique<FakeBackend>();
  auto* backend = fake.get();
  auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
  QSignalSpy configured_spy(host.get(), &LayerSurfaceHost::configured);
  ASSERT_TRUE(host->open(validSpec()));
  EXPECT_EQ(host->state(), LayerSurfaceHost::State::WaitingForConfigure);
  backend->configure(10, 120, 50);
  backend->configure(11, 0, 60);
  EXPECT_EQ(host->state(), LayerSurfaceHost::State::Configured);
  EXPECT_EQ(backend->acknowledgements, (QList<quint32>{10, 11}));
  EXPECT_EQ(backend->show_count, 1);
  EXPECT_EQ(configured_spy.count(), 2);
}

TEST(WaylandLayerSurfaceHostLifecycle, CloseBeforeConfigureIsOrderedAndIdempotent) {
  auto fake = std::make_unique<FakeBackend>();
  auto* backend = fake.get();
  auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
  QSignalSpy closed_spy(host.get(), &LayerSurfaceHost::closed);
  ASSERT_TRUE(host->open(validSpec()));
  EXPECT_NE(host->view(), nullptr);
  host->close();
  host->close();
  EXPECT_EQ(host->view(), nullptr);
  EXPECT_EQ(closed_spy.count(), 1);
  EXPECT_LT(backend->events.indexOf(QStringLiteral("destroy-role")),
            backend->events.indexOf(QStringLiteral("queue-window")));
}

TEST(WaylandLayerSurfaceHostLifecycle, ProtocolAndOutputClosuresUseCommonTeardown) {
  for (int event = 0; event < 2; ++event) {
    auto fake = std::make_unique<FakeBackend>();
    auto* backend = fake.get();
    auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
    ASSERT_TRUE(host->open(validSpec()));
    backend->configure(1, 0, 0);
    if (event == 0)
      backend->close();
    else
      backend->close();
    EXPECT_EQ(host->state(), LayerSurfaceHost::State::Closed);
    EXPECT_FALSE(backend->role);
    EXPECT_EQ(host->view(), nullptr);
  }
}

TEST(WaylandLayerSurfaceHostLifecycle, QmlFailureInvalidatesAccessAndDestroysRole) {
  auto fake = std::make_unique<FakeBackend>();
  auto* backend = fake.get();
  auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
  QSignalSpy failed_spy(host.get(), &LayerSurfaceHost::failed);
  ASSERT_TRUE(host->open(validSpec()));
  backend->failure(QStringLiteral("fake QML load failure"));
  EXPECT_EQ(host->state(), LayerSurfaceHost::State::Failed);
  EXPECT_EQ(host->view(), nullptr);
  EXPECT_EQ(failed_spy.count(), 1);
}

TEST(WaylandLayerSurfaceHostLifecycle, MutationsCommitObservableState) {
  auto fake = std::make_unique<FakeBackend>();
  auto* backend = fake.get();
  auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
  ASSERT_TRUE(host->open(validSpec()));
  const int initial_commits = backend->commit_count;
  EXPECT_TRUE(host->setSize(200, 80));
  EXPECT_TRUE(host->setMargins(1, 2, 3, 4));
  EXPECT_TRUE(host->setAnchors(Holonight::Wayland::Anchor::Top | Holonight::Wayland::Anchor::Left));
  EXPECT_TRUE(host->setExclusiveZone(20));
  EXPECT_TRUE(host->setKeyboardInteractivity(Holonight::Wayland::KeyboardInteractivity::OnDemand));
  const QRegion region(QRect(2, 3, 10, 11));
  EXPECT_TRUE(host->setInputRegion(Holonight::Wayland::InputRegionPolicy::Region, region));
  EXPECT_EQ(backend->commit_count, initial_commits + 6);
  EXPECT_EQ(backend->margins, (QList<int>{1, 2, 3, 4}));
  EXPECT_EQ(backend->input_region, region);
}

TEST(WaylandLayerSurfaceHostLifecycle, CloseFromQtCallbackIsSafe) {
  auto fake = std::make_unique<FakeBackend>();
  auto* backend = fake.get();
  auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
  ASSERT_TRUE(host->open(validSpec()));
  QObject callback_owner;
  QObject::connect(&callback_owner, &QObject::objectNameChanged, host.get(), &LayerSurfaceHost::close);
  callback_owner.setObjectName(QStringLiteral("close-now"));
  EXPECT_EQ(host->state(), LayerSurfaceHost::State::Closed);
  EXPECT_FALSE(backend->role);
}

TEST(WaylandLayerSurfaceHostLifecycle, RepeatedOpenCloseCyclesAreSupported) {
  auto fake = std::make_unique<FakeBackend>();
  auto* backend = fake.get();
  auto host = Holonight::Wayland::LayerSurfaceHostTestAccess::create(std::move(fake));
  ASSERT_TRUE(host->open(validSpec()));
  host->close();
  ASSERT_TRUE(host->open(validSpec()));
  backend->configure(4, 100, 40);
  EXPECT_EQ(host->state(), LayerSurfaceHost::State::Configured);
  host->close();
  EXPECT_EQ(host->state(), LayerSurfaceHost::State::Closed);
}

}  // namespace
