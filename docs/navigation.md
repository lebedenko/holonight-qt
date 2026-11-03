# Shared collection navigation

`Holonight::NavigationInterpreter` maps normalized keys to intentions and owns counts
(saturating at INT_MAX) and the 600 ms `gg` sequence. Standard is the default;
Vim navigation is opt-in via the shared preferences document. The shared
Qt reader uses DocumentWatcher and retains the last valid profile on invalid updates.

An interpreter belongs to one active scope. Consumers reset it on focus/profile changes,
modal entry and cancellation. Text editors and modal scopes never route into the
collection interpreter. `keyName` enforces exact modifiers. Tab/Backtab are left to Qt.
Operations and acceptance are consumer policy, never effects of shared movement.

`NavigationAdapter` provides target count, grid columns, viewport height and geometry
from layout/model metadata. It does not require delegates. `navigationTarget` clamps
movement and pages by vertical distance across section gaps. `NavigationCursor` owns
one index and stable identity, preserving the current identity across reorder and choosing
the nearest surviving old neighbor after removal. Selection is separate.

QML scopes use `HnNavigation` for interpretation and target calculation, delegating the
cursor to their existing application controller. `HnInputPreferences` exposes live
preferences to standalone QML consumers. Future consumers may interpret Parent/Open
as leave/enter row controls and panel intentions as spatial focus routing.

## Bindings

| Intention | Standard | Vim navigation (in addition to standard movement) |
| --- | --- | --- |
| Previous/next | Up/Down | k/j |
| First/last | Home/End | gg/G |
| Full page | PgUp/PgDown | PgUp/PgDown |
| Half page | — | Ctrl+U/Ctrl+D |
| Panel focus | Tab/Backtab | Ctrl+H/J/K/L |
| Hidden files | Ctrl+H | . |
| Edit location | Ctrl+L, Alt+D | Alt+D |

In file lists h/l mean parent/open; grids retain spatial h/j/k/l movement.
Popup row h/l leave/enter controls. Enter/Space keep consumer policy. Text editing
is conventional in both profiles. Escape, focus changes and modal entry cancel sequences.


## Navigation style

Choose **Settings → Keyboard → Navigation style** to use Standard (the default) or
Vim navigation in Files, the standalone file chooser, and the Bluetooth popup. The preference
applies live, independently of Shell; text editing remains conventional. Standard
keeps arrow, Home/End, Page Up/Down and Tab navigation. Vim adds h/j/k/l, gg/G,
counts, Ctrl+U/D half pages and Ctrl+H/J/K/L panel focus where supported. In file
lists h/l go to parent/open; in grids they move horizontally. `.` toggles hidden
files; Alt+D edits the location. Text fields and modal dialogs isolate navigation.

The shared preference is in `$XDG_CONFIG_HOME/holonight/config.toml`, or
`~/.config/holonight/config.toml` when XDG_CONFIG_HOME is unset:

```toml
[input]
navigation = "vim-navigation"
```

Use `navigation = "standard"` or remove the key to restore Standard. Missing files
are never created by readers. Valid replacement/deletion applies live; invalid
updates retain the last valid setting and expose reader diagnostics. Settings Save,
Discard, reset and conflict resolution apply to this preference. Edits preserve
Shell sections, comments and unknown keys; appearance stays in its own document.

For manual migration from the unreleased `input.toml` contract, copy its `profile`
value into `[input].navigation` in `config.toml`. The old `version` and `profile`
keys are no longer read. Old files are never deleted automatically. There are no
application-specific navigation overrides.
