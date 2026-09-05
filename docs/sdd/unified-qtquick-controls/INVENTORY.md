# Source inventory — 2026-09-05

Generated from tracked files in the published discovery baselines; paths are repository-relative.
This is a static dependency inventory, not runtime or visual acceptance.

## holonight-qt

Baseline: `dd8eef45f92baf77efe6680e4ccaaa40b3c20030`.

### QML imports and control-sensitive usage

| File | Imports | Attached properties / enum owners / custom-property assignments |
|---|---|---|
| `demo/Main.qml` | `QtQuick.Controls`, `Holonight as H`, `Holonight.Core as Core`, `Holonight.Controls as Controls` | `ButtonGroup.group`, `ScrollBar.AlwaysOn`, `ScrollBar.vertical`, `TextArea.Wrap`, `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible`, `hasError:` |
| `examples/controls-gallery/Main.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` |  |
| `examples/controls-gallery/SurfaceHierarchyExample.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` |  |
| `examples/controls-gallery/pages/ButtonsPage.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` |  |
| `examples/controls-gallery/pages/DelegatesPage.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` | `ButtonGroup.group` |
| `examples/controls-gallery/pages/InputsPage.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` | `hasError:` |
| `examples/controls-gallery/pages/SettingsPage.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` |  |
| `examples/controls-gallery/pages/SurfacesPage.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` |  |
| `examples/controls-gallery/pages/TypographyPage.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/Button.qml` | `QtQuick.Controls.impl`, `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/CheckBox.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/ComboBox.qml` | `QtQuick.Controls as C`, `QtQuick.Templates as T`, `Holonight`, `Holonight.Core` | `C.Overlay.overlay`, `ScrollBar.AlwaysOff`, `ScrollBar.AlwaysOn`, `ScrollBar.vertical`, `T.Popup.Item`, `bottomLeftRadius:`, `bottomRightRadius:`, `delegateHeight:`, `maximumVisibleItems:`, `topLeftRadius:`, `topRightRadius:` |
| `qml/HnApplicationWindow.qml` | `Holonight.Core` |  |
| `qml/HnIcon.qml` | `Holonight.Core` | `hasError:` |
| `qml/HnSurfaceFrame.qml` | `Holonight.Core` |  |
| `qml/ItemDelegate.qml` | `QtQuick.Templates as T`, `Holonight.Core` | `bottomLeftRadius :`, `bottomLeftRadius:`, `bottomRightRadius :`, `bottomRightRadius:`, `topLeftRadius :`, `topLeftRadius:`, `topRightRadius :`, `topRightRadius:` |
| `qml/Menu.qml` | `QtQuick.Templates as T`, `Holonight`, `Holonight.Core` |  |
| `qml/MenuItem.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/ProgressBar.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/RadioButton.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/ScrollBar.qml` | `QtQuick.Templates as T`, `Holonight.Core` | `T.ScrollBar.AlwaysOn` |
| `qml/ScrollView.qml` | `QtQuick.Templates as T`, `Holonight` | `ScrollBar.horizontal`, `ScrollBar.vertical` |
| `qml/Slider.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/SpinBox.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/Switch.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/TabBar.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/TabButton.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/TextArea.qml` | `QtQuick.Templates as T`, `Holonight.Core` | `hasError:` |
| `qml/TextField.qml` | `QtQuick.Templates as T`, `Holonight.Core` | `hasError:` |
| `qml/ToolTip.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/controls/HnActionBar.qml` | `Holonight.Core` |  |
| `qml/controls/HnActionDelegate.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnAppTitle.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnAvatar.qml` | `Holonight.Core` |  |
| `qml/controls/HnCardDelegate.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnChoiceCard.qml` | `QtQuick.Templates as T`, `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnColorPicker.qml` | `QtQuick.Controls.Basic as C`, `QtQuick.Templates as T`, `Holonight as H`, `Holonight.Core` | `C.ScrollBar.AlwaysOff`, `C.ScrollBar.AsNeeded`, `C.ScrollBar.horizontal`, `C.ScrollBar.vertical` |
| `qml/controls/HnEmptyState.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnFormField.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` | `hasError:` |
| `qml/controls/HnHeaderBar.qml` | `Holonight.Core` |  |
| `qml/controls/HnIconButton.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/controls/HnIconComboBox.qml` | `QtQuick.Templates as T`, `Holonight as H`, `Holonight.Core` | `bottomLeftRadius:`, `bottomRightRadius:`, `delegateHeight:`, `topLeftRadius:`, `topRightRadius:` |
| `qml/controls/HnKeyHint.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnListDelegate.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnLoadingState.qml` | `QtQuick.Controls.Basic`, `Holonight as H`, `Holonight.Core` |  |
| `qml/controls/HnNavigationDelegate.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/controls/HnPanelHeader.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnSearchField.qml` | `QtQuick.Templates as T`, `Holonight as H`, `Holonight.Core` |  |
| `qml/controls/HnSectionHeader.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnSegmentedControl.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/controls/HnSelectableDelegate.qml` | `QtQuick.Templates as T`, `Holonight.Core` |  |
| `qml/controls/HnSeparator.qml` | `Holonight.Core` |  |
| `qml/controls/HnSettingsRow.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnStatusIndicator.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/controls/HnTextArea.qml` | `QtQuick.Controls.Basic as C`, `Holonight as H`, `Holonight.Core` | `C.ScrollBar.AlwaysOff`, `C.ScrollBar.AsNeeded`, `C.ScrollBar.horizontal`, `C.ScrollBar.vertical`, `hasError:` |
| `qml/core/HnLabel.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |

## holonight-shell

Baseline: `723763e09ff815d344a6cb01529dd8345a43b316`.

### QML imports and control-sensitive usage

| File | Imports | Attached properties / enum owners / custom-property assignments |
|---|---|---|
| `apps/shell/qml/Background/Background.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Controls/AudioIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/BarFrame.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/BarIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/BatteryIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/HudFrame.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `apps/shell/qml/Controls/NetworkIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/SessionIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/SignalStrengthIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Controls/UtilityIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Launcher/Launcher.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Launcher/LauncherActionRow.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `apps/shell/qml/Launcher/LauncherCategoryIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Launcher/LauncherResultRow.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Launcher/LauncherRightPanelBrowse.qml` | `QtQuick.Controls`, `Holonight`, `Holonight.Components`, `Holonight.Core`, `Holonight.Controls`, `HolonightShell` | `ScrollBar.AlwaysOff`, `ScrollBar.horizontal` |
| `apps/shell/qml/Launcher/LauncherRightPanelSearch.qml` | `Holonight.Core`, `Holonight.Components`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Launcher/LauncherSearchField.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Notifications/ToastActionButton.qml` | `HolonightShell` |  |
| `apps/shell/qml/Notifications/ToastItem.qml` | `HolonightShell`, `Holonight.Core`, `Holonight.Components` |  |
| `apps/shell/qml/Notifications/ToastStack.qml` | `HolonightShell` |  |
| `apps/shell/qml/Osd/OsdLevelRenderer.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Osd/OsdSelectionRenderer.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Osd/OsdView.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioApplicationsSection.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioCurrentDeviceRow.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioDeviceDelegate.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioDeviceList.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioDeviceSection.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioMasterBar.qml` | `Holonight.Core`, `Holonight.Components`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioMasterPanel.qml` | `Holonight.Core`, `Holonight.Controls`, `Holonight.Components`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioPopupContent.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioPopupHeader.qml` | `Holonight.Core`, `Holonight.Controls`, `Holonight.Components`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioStreamDelegate.qml` | `Holonight.Core`, `Holonight.Components`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioStreamList.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/AudioTintedIcon.qml` | `Holonight.Core`, `Holonight.Components` |  |
| `apps/shell/qml/Popups/Audio/AudioVolumeSlider.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Popups/Audio/InputLevelMeter.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Audio/KeyboardHintFooter.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Battery/BatteryPopupContent.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Popups/Battery/ProfileButton.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Popups/Network/NetworkActionRow.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Network/NetworkCurrentCard.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Network/NetworkPopupContent.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Network/NetworkPopupIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Popups/Network/NetworkToggleRow.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Network/WifiNetworkDelegate.qml` | `Holonight.Core`, `Holonight.Components`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Network/WifiNetworkList.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Network/WifiPasswordDialog.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `Holonight.Controls` | `Controls.Popup.CloseOnEscape`, `Controls.Popup.CloseOnPressOutside`, `hasError:` |
| `apps/shell/qml/Popups/Status/StatusPopup.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Popups/Status/StatusPopupDismissOverlay.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Tooltip/TooltipPopup.qml` | `QtQuick.Controls as Controls`, `HolonightShell`, `Holonight.Core`, `Holonight.Controls` |  |
| `apps/shell/qml/Popups/Tray/TrayMenuPopup.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Popups/Weather/WeatherAqiGauge.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Popups/Weather/WeatherCurrentSection.qml` | `HolonightShell`, `Holonight.Core`, `Holonight.Controls` |  |
| `apps/shell/qml/Popups/Weather/WeatherDailyCards.qml` | `HolonightShell`, `Holonight.Core`, `Holonight.Controls` |  |
| `apps/shell/qml/Popups/Weather/WeatherDetailsGrid.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Popups/Weather/WeatherHourlyStrip.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Popups/Weather/WeatherPopupContent.qml` | `HolonightShell`, `Holonight.Core`, `Holonight.Controls` |  |
| `apps/shell/qml/Popups/Weather/WeatherWindWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/RightSidebar/RightSidebar.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/SidebarContent.qml` | `QtQuick.Controls as Controls`, `HolonightShell` | `Controls.ScrollBar.AlwaysOff`, `Controls.ScrollBar.AsNeeded`, `Controls.ScrollBar.horizontal`, `Controls.ScrollBar.vertical` |
| `apps/shell/qml/RightSidebar/SidebarTabBar.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/SidebarTabButton.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/Calendar/SidebarCalendar.qml` | `QtQuick.Controls as Controls`, `Holonight.Core` |  |
| `apps/shell/qml/RightSidebar/Tabs/Media/SidebarMedia.qml` | `QtQuick.Controls as Controls`, `Holonight.Core` |  |
| `apps/shell/qml/RightSidebar/Tabs/Notifications/SidebarNotifications.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `Holonight.Components`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/Overview/SidebarOverview.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/Overview/SidebarOverviewCalendar.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/Overview/SidebarOverviewNotifications.qml` | `Holonight.Core`, `Holonight.Components`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/Overview/SidebarOverviewUpcoming.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/QuickSettings/BrightnessSlider.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/QuickSettings/ChargeLimitRow.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/QuickSettings/InhibitorSection.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/QuickSettings/KeepAwakeAction.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/QuickSettings/SidebarQuickSettings.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/System/DefaultAppRow.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/RightSidebar/Tabs/System/SidebarSystem.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `apps/shell/qml/Topbar/ActiveWindowSection.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Topbar/AppWindowIcon.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Topbar/AudioWidget.qml` | `QtQuick.Controls as Controls`, `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/BarBackground.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Topbar/BarSeparator.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Topbar/BarTooltipArea.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/BatteryIndicator.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Topbar/BatteryWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/ClockSection.qml` | `QtQuick.Controls as Controls`, `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Topbar/KeyboardLayoutWidget.qml` | `QtQuick.Controls as Controls`, `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/LogoSection.qml` | `QtQuick.Controls as Controls`, `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/MprisControlButton.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Topbar/MprisSection.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/MprisWidget.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Topbar/NamedWorkspaceSection.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/NetworkWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/NotificationsWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/NumericWorkspaceSection.qml` | `HolonightShell`, `Holonight.Controls` |  |
| `apps/shell/qml/Topbar/PrecipitationGraph.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/SpecialWorkspaceDot.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/StatusPopupTriggerArea.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/StatusesSection.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/TemperatureGraph.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/TopBar.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/WeatherSection.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/WeatherWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/WorkspaceEdgeArrow.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Topbar/WorkspacePill.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Topbar/WorkspacePillStrip.qml` | `HolonightShell` |  |
| `apps/shell/qml/Topbar/WorkspaceSection.qml` | `HolonightShell` |  |
| `apps/shell/qml/Tray/TrayItem.qml` | `HolonightShell`, `Holonight.Core`, `Holonight.Components` |  |
| `apps/shell/qml/Tray/TrayMenuDismissOverlay.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/Tray/TrayMenuItem.qml` | `Holonight.Core`, `Holonight.Components` |  |
| `apps/shell/qml/Tray/TraySection.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Utility/AppearanceReloadBridge.qml` | `Holonight.Core`, `HolonightShell` |  |
| `apps/shell/qml/WeatherIcon/WeatherIconCompositor.qml` | `HolonightShell` |  |
| `apps/shell/qml/Widgets/ClockWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Widgets/MprisArtwork.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Widgets/MprisProgressBar.qml` | `Holonight.Core` |  |
| `apps/shell/qml/Widgets/MprisWidgetSurface.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Widgets/TimeToEventWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `apps/shell/qml/Widgets/WidgetSurface.qml` | `HolonightShell` |  |
| `qml/Authentication/AuthenticationDialog.qml` | `QtQuick.Controls.Basic`, `Holonight`, `Holonight.Core`, `Holonight.Controls`, `Holonight.Authentication` | `ScrollBar.AlwaysOff`, `ScrollBar.horizontal` |
| `qml/Authentication/AuthenticationIcon.qml` | `Holonight.Core` |  |
| `qml/Authentication/AuthenticationPrompt.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` |  |
| `qml/Authentication/IdentitySelector.qml` | `QtQuick.Controls.Basic`, `Holonight as H`, `Holonight.Core`, `Holonight.Controls` | `delegateHeight:` |
| `qml/Authentication/MessageList.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` | `ScrollBar.vertical` |
| `qml/HoloNight/Components/ExternalIcon.qml` | `Holonight.Core` |  |
| `tests/qml/tst_AudioPopupRedesign.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_AudioSharedControls.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_AuthenticationDialog.qml` | `QtQuick.Controls.Basic`, `Holonight`, `Holonight.Core`, `Holonight.Authentication` |  |
| `tests/qml/tst_BarFrame.qml` | `HolonightShell` |  |
| `tests/qml/tst_ExternalIcon.qml` | `Holonight.Core`, `Holonight.Components` |  |
| `tests/qml/tst_HnIcon.qml` | `Holonight.Core` |  |
| `tests/qml/tst_HudFrame.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_LauncherActionRow.qml` | `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_LauncherSharedControls.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_LogoSection.qml` | `HolonightShell` |  |
| `tests/qml/tst_Mpris.qml` | `HolonightShell` |  |
| `tests/qml/tst_MprisWidget.qml` | `HolonightShell`, `Holonight.Core` |  |
| `tests/qml/tst_NetworkCurrentCard.qml` | `HolonightShell` |  |
| `tests/qml/tst_NetworkPopupIcon.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_NetworkPopupPolish.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_OsdLevelRenderer.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_OsdSelectionRenderer.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_OsdView.qml` | `HolonightShell` |  |
| `tests/qml/tst_Phase12PopupResilience.qml` | `HolonightShell` |  |
| `tests/qml/tst_RightSidebarSharedControls.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_TooltipPopup.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_TopbarSectionTransitions.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_TrayItem.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_TrayMenuPopup.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_WifiNetworkDelegate.qml` | `Holonight.Core`, `Holonight.Components`, `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_WifiPasswordDialog.qml` | `Holonight.Controls`, `HolonightShell` |  |
| `tests/qml/tst_WorkspaceEdgeArrow.qml` | `HolonightShell`, `Holonight.Core` |  |
| `tests/qml/tst_WorkspacePill.qml` | `HolonightShell` |  |
| `tests/qml/tst_WorkspaceSection.qml` | `HolonightShell` |  |
| `tests/qml/tst_basic.qml` | `HolonightShell` |  |
| `tests/qml/tst_component_instantiation.qml` | `Holonight.Core`, `HolonightShell` |  |
| `tests/qml/tst_holonight_theme_singleton.qml` | `Holonight.Core` |  |
| `tests/qml/tst_power_extensions.qml` | `HolonightShell` |  |

## holonight-settings

Baseline: `579515ffb456c59cd1299e5852c392c3064c8262`.

### QML imports and control-sensitive usage

| File | Imports | Attached properties / enum owners / custom-property assignments |
|---|---|---|
| `apps/settings/qml/AppearancePage.qml` | `QtQuick.Controls.Basic as QQC2`, `Holonight`, `Holonight.Core`, `Holonight.Controls`, `HolonightSettings` | `QQC2.ButtonGroup.group` |
| `apps/settings/qml/AudioPage.qml` | `Holonight.Core`, `HolonightSettings`, `QtQuick.Controls as Controls` |  |
| `apps/settings/qml/BarPage.qml` | `Holonight`, `Holonight.Core`, `Holonight.Controls`, `HolonightSettings` |  |
| `apps/settings/qml/ColorSchemeSwatchCard.qml` | `QtQuick.Templates as T`, `Holonight.Controls`, `Holonight.Core`, `HolonightSettings` |  |
| `apps/settings/qml/ContentStack.qml` | `HolonightSettings`, `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` |  |
| `apps/settings/qml/FooterBar.qml` | `Holonight as HnStyle`, `Holonight.Controls`, `Holonight.Core`, `HolonightSettings`, `QtQuick.Controls.Basic` | `Overlay.overlay` |
| `apps/settings/qml/IntegrationsPage.qml` | `Holonight as HnStyle`, `Holonight.Controls`, `Holonight.Core`, `HolonightSettings`, `QtQuick.Controls.Basic` |  |
| `apps/settings/qml/NavPanel.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `apps/settings/qml/PlaceholderPage.qml` | `Holonight.Core` |  |
| `apps/settings/qml/PreviewPanel/PreviewPanel.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `apps/settings/qml/SettingsWindow.qml` | `Holonight.Controls`, `Holonight.Core`, `HolonightSettings` |  |
| `apps/settings/qml/WeatherPage.qml` | `Holonight`, `Holonight.Core`, `Holonight.Controls`, `HolonightSettings` |  |

## holonight-ai

Baseline: `b600674ce4c86d883d98a27ae783e056a6a2f0e6`.

### QML imports and control-sensitive usage

| File | Imports | Attached properties / enum owners / custom-property assignments |
|---|---|---|
| `qml/quickpanel/QuickPanel.qml` | `QtQuick.Controls.Basic`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/quickpanel/QuickPanelHeader.qml` | `QtQuick.Controls.Basic`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls` | `Popup.CloseOnEscape`, `Popup.CloseOnPressOutside` |
| `qml/shared/AssistantResponseContent.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/shared/BottomAnchoredListView.qml` | `Holonight` | `ScrollBar.AlwaysOn`, `ScrollBar.vertical` |
| `qml/shared/ChatCodeBlock.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/shared/ChatComposer.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` | `ScrollBar.AlwaysOff`, `ScrollBar.AsNeeded`, `ScrollBar.horizontal`, `ScrollBar.vertical` |
| `qml/shared/ChatComposerActions.qml` | `QtQuick.Controls.Basic as QQC2`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/shared/ChatHeader.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/shared/ChatNoticeStack.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/shared/ChatPanel.qml` | `Holonight.Core` |  |
| `qml/shared/GenericToolContent.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` | `TextArea.PlainText`, `TextArea.Wrap` |
| `qml/shared/ListFilesToolContent.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls`, `Holonight as H` | `hasError:` |
| `qml/shared/MarkdownBlock.qml` | `Holonight.Core` |  |
| `qml/shared/MessageBubble.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/shared/MessageList.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls as HnControls` |  |
| `qml/shared/ProviderIcon.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/shared/ResponseStatsFooter.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/shared/ResponseStatsPopup.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` | `Overlay.overlay`, `Popup.CloseOnEscape`, `Popup.CloseOnPressOutside` |
| `qml/shared/StreamingStatusBar.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/shared/ToolActivityCard.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls`, `Holonight as H` | `TextArea.PlainText`, `TextArea.Wrap` |
| `qml/shared/ToolCallCard.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/shared/UserMessageCard.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/AnthropicSettingsPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` | `hasError:` |
| `qml/workspace/BackgroundAiSettingsPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/workspace/ComingSoonPage.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/ConversationListDelegate.qml` | `QtQuick.Controls.Basic as QQC2`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/workspace/ConversationListPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/workspace/CredentialTextField.qml` | `Holonight as H`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/GoogleSettingsPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` | `hasError:` |
| `qml/workspace/OllamaSettingsPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` | `hasError:` |
| `qml/workspace/OpenAISettingsPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight` | `hasError:` |
| `qml/workspace/ProviderActionButton.qml` | `Holonight as H` |  |
| `qml/workspace/ProviderFormActionRow.qml` | `Holonight.Core` |  |
| `qml/workspace/ProviderListDelegate.qml` | `QtQuick.Controls.Basic`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/ProviderListPanel.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls`, `Holonight as H` |  |
| `qml/workspace/ProviderModelPicker.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/ProviderSettingsScaffold.qml` | `QtQuick.Controls.Basic as QQC2`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls`, `Holonight`, `Holonight as H` | `QQC2.Dialog.NoButton`, `ScrollBar.AlwaysOff`, `ScrollBar.horizontal`, `hasError:` |
| `qml/workspace/ProvidersPage.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/SettingsSidebar.qml` | `QtQuick.Controls.Basic`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/SettingsWindow.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls`, `Holonight as H` | `Dialog.NoButton` |
| `qml/workspace/UnsupportedProviderPanel.qml` | `QtQuick.Controls.Basic as QQC2`, `Holonight.Core`, `Holonight.Controls`, `Holonight` |  |
| `qml/workspace/WorkspaceInspectorPanel.qml` | `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/WorkspaceWindow.qml` | `QtQuick.Controls.Basic`, `HolonightChat`, `Holonight.Core`, `Holonight.Controls` |  |

## holonight-pkg-manager

Baseline: `518bb60232086e9537fb402f91b4b703d260ffcf`.

### QML imports and control-sensitive usage

| File | Imports | Attached properties / enum owners / custom-property assignments |
|---|---|---|
| `qml/packages/InstalledFilterTabs.qml` | `Holonight.Core`, `Holonight.Controls`, `HolonightPackages` |  |
| `qml/packages/InstalledPackagesView.qml` | `QtQuick.Controls.Basic`, `Holonight as H`, `Holonight.Controls`, `HolonightPackages` | `ScrollBar.AlwaysOff`, `ScrollBar.horizontal`, `ScrollBar.vertical` |
| `qml/packages/InstalledToolbar.qml` | `QtQuick.Controls.Basic`, `Holonight as H`, `Holonight.Core`, `Holonight.Controls`, `HolonightPackages` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/packages/OrphanFooterBar.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls`, `HolonightPackages` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/packages/PackageDetailDependencySections.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` |  |
| `qml/packages/PackageDetailFooterLinks.qml` | `Holonight.Controls` |  |
| `qml/packages/PackageDetailHeader.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/packages/PackageDetailMetadataRows.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/packages/PackageDetailPanel.qml` | `QtQuick.Controls.Basic`, `Holonight as H`, `Holonight.Core`, `Holonight.Controls`, `HolonightPackages` | `ScrollBar.AlwaysOff`, `ScrollBar.horizontal`, `ScrollBar.vertical` |
| `qml/packages/PackageOriginBadge.qml` | `QtQuick.Controls.Basic`, `Holonight.Core` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/packages/PackageTable.qml` | `QtQuick.Controls.Basic`, `Holonight as H`, `Holonight.Controls`, `HolonightPackages` | `ScrollBar.AlwaysOff`, `ScrollBar.horizontal`, `ScrollBar.vertical` |
| `qml/packages/PackageTableHeader.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/packages/PackageTableRow.qml` | `QtQuick.Controls.Basic`, `Holonight.Core`, `Holonight.Controls` | `ToolTip.delay`, `ToolTip.text`, `ToolTip.visible` |
| `qml/workspace/Sidebar.qml` | `Holonight.Core`, `Holonight.Controls` |  |
| `qml/workspace/WorkspaceWindow.qml` | `Holonight.Core`, `HolonightPackages` |  |

## holonight-greeter

Baseline: `9130c9ccbf05986ab1843ae322831e7fe9efdac9`.

### QML imports and control-sensitive usage

| File | Imports | Attached properties / enum owners / custom-property assignments |
|---|---|---|
| `qml/FooterSelector.qml` | `Holonight as Hn`, `Holonight.Core` | `delegateHeight:` |
| `qml/LoginPanel.qml` | `QtQuick.Controls`, `Holonight.Controls` |  |
| `qml/Main.qml` | `QtQuick.Controls`, `Holonight.Core`, `Holonight.Controls` | `ToolTip.text`, `ToolTip.visible` |
