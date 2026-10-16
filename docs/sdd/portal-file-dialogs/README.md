# Portal file dialogs through the Qt platform theme

## Intent and boundaries

Standard Qt 6 Widgets and Qt Quick file/folder dialogs should reach the desktop's
configured chooser while retaining HoloNight appearance. Only holonight-qt changes:
consumers keep Qt APIs, the portal broker owns backend routing, and neither Qt 5
nor the backend's X11 support changes. There is no new public HoloNight API.

## Design

The platform theme advertises a private QPlatformFileDialogHelper for file dialogs.
Its constructor does not access desktop services. QtDBus is a private plugin
dependency; the helper does not depend on the optional Wayland layer component.

At show time, an activating Properties.Get queries FileChooser.version with a
three-second timeout. Directories require version 3. Discovery is repeated on
each open, allowing recovery when the broker starts later. For a supplied parent,
the active Qt integration's QDesktopUnixServices exports the identifier; export
failure returns false. Intentionally unparented requests are allowed.

The helper subscribes on a token-derived request path before asynchronously
calling OpenFile or SaveFile. The method reply timeout is 25 seconds; there is no
user interaction timeout. A distinct receiver per open binds callbacks to the
broker's unique bus owner and prevents replies from previous opens changing new
state. Returned handles are validated and subscriptions adjusted if necessary.

Hide, parent hide/close/destruction, service loss and helper destruction cancel
locally, disconnect responses and close the request. A pending receiver outlives
the helper until the method reply, so cancellation also closes a late handle.
Every terminal path quits a synchronous exec loop. Selected URLs and filter state
are updated before acceptance. Qt remains responsible for default suffix behavior.

Only pre-submission failures return false for Qt fallback. After submission,
errors reject the dialog; they never open a second chooser. Unsupported visual
customization remains under backend control.

## Protocol and verification

The implementation follows the public
[FileChooser contract](https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.FileChooser.html)
and [Request lifecycle](https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.Request.html).
Name filters become glob patterns and MIME filters become MIME patterns.
Local directory/save paths use nul-terminated filesystem bytes; returned URLs
are retained without conversion to local paths.

Tests use a separate fake broker process on a private D-Bus session with no
activation directories. They cover option translation, Unicode/space URLs,
filter round trips, repeated opens, cancellation, malformed results, early
responses, changed request handles, late replies after cancellation/destruction,
service restart, simultaneous dialogs, synchronous completion, native disabling,
unavailable parent export, missing broker, discovery timeout and old capabilities.

Qt-only Widgets and QML FileDialog/FolderDialog probes load the real plugin.
The package install test repeats them against the staged plugin. A build with
BUILD_WAYLAND=OFF verifies independence from the layer-surface library.

Native Hyprland acceptance remains a separate manual check: run Markdown Viewer
with QT_QPA_PLATFORMTHEME=holonight and the staged host-built plugin, then exercise
first open, accept, cancel, reopen, filters, modality, parent association and
closing the parent. Headless tests cannot establish compositor parenting fidelity.
No global environment or system installation changes are needed.

## Native verification record (2026-10-16)

Using the host Qt build with BUILD_WAYLAND=OFF, staged under the package-test
DESTDIR, a separate Markdown Viewer process loaded the HoloNight platform plugin
under Hyprland. Ctrl+O opened the installed HoloNight portal backend with the
“Open Markdown file” title and “Markdown files” filter. Selecting a temporary
Markdown document changed the viewer title to native.md. A second open cancelled
with Escape, then reopened successfully. Closing the parent viewer while the
third chooser was open removed both windows. The test process exited cleanly.

This verifies the native user flow and parent-close cleanup, separately from
the fake-broker suite. Strict compositor modality and other desktop backends
have not been independently verified. No system plugin or global configuration
was changed.

## Automated verification record (2026-10-16)

The host build used Qt 6.12 with BUILD_WAYLAND=OFF. All six focused CTest
entries passed: theme tests, fatal-warning bootstrap, nine fake-broker helper
tests, and the three Qt-only consumer probes. Focused clang-tidy checks passed
for the affected public headers and portal test utilities.

After lint cleanup, the pinned task check:integration workflow passed in full:
all 101 tests, formatting, repository-wide clang-tidy, QML lint/import/metadata
checks, licensing, Release staging, and all 90 tests in the minimal configuration
with Wayland and syntax themes disabled. Both configurations include staged
Qt-only portal consumer probes.

The lint cleanup preserves published palette token names and public enum
representations through documented, narrowly scoped compatibility annotations.
Test-only lint policy permits protected fixture state for generated GTest
subclasses. The fake broker and consumer probes use smaller functions, explicit
captures, and bounded command-line argument access.

Logs are retained in the ignored build/portal/verification directory and the
container acceptance directories.
