# HoloNight Design Deviations

This file records color values that deliberately differ from the [HoloNight Design System](../holonight-shell/assets/dont-commit/HoloNight-Design-System.md) due to technical or accessibility constraints.

## Format

Each entry: token name, design system target, actual value, reason, and WCAG ratio.

---

## Active Deviations

### `onSurfaceVariant`

| Field | Value |
|---|---|
| Design system target | Text Muted `#a9b1d6` |
| Actual value | `#565f89` |
| Reason | `onSurfaceVariant` is retained as the disabled/placeholder text role and is WCAG 1.4.3 exempt. Use `textSubtle` for design-system secondary text (`#a9b1d6`). |
| WCAG ratio (worst case) | 2.35:1 on panel (`#24283b`) |

### `primary`

| Field | Value |
|---|---|
| Design system target | Cyan `#7dcfff` for the primary neon accent |
| Actual value | `#00a0dc` |
| Reason | `primary` is mapped to `QPalette::Highlight` and selection/link fill roles, not the general design-system accent. It is kept darker so `onPrimary` (`#1a1b26`) remains a readable selection text color and existing Qt highlight behavior stays stable. Use `accentCyan` for the design-system cyan accent. |
| WCAG ratio | 5.75:1 with `onPrimary` |

### `focusRing` / `borderFocus`

| Field | Value |
|---|---|
| Design system target | Cyan `#7dcfff` |
| Actual value | `#00e0ff` |
| Reason | Keyboard focus uses a brighter pure-cyan indicator than the decorative accent so focus remains unmistakable on every dark surface. `borderFocus == focusRing` is intentional and documented by the semantic border token design notes. Use `accentCyan` for non-focus cyan accent work. |
| WCAG ratio (worst case) | 9.09:1 on panel (`#24283b`) |

### `borderPassive`

| Field | Value |
|---|---|
| Design system target | ≥3:1 contrast on all surfaces (WCAG 1.4.11) |
| Actual value | `#565f89` — 2.35:1 on `secondary`, 2.99:1 on `surface` |
| Reason | Value mandated by design system "Border Muted" spec. Same hex as the deprecated `outlineVariant` token, which carries an existing WCAG 1.4.11 exemption: passive/inactive borders are not interactive components and are explicitly exempt from SC 1.4.11. |
| WCAG ratio (worst case) | 2.35:1 on panel (`#24283b`) |

### `borderHover`

| Field | Value |
|---|---|
| Design system target | ≥3:1 contrast on all surfaces when composited (WCAG 1.4.11) |
| Actual value | `#7dcfff` at ~30% alpha (0x4d) — composited ratio 2.03:1 on `surface` |
| Reason | Hover is a transient state. Visual discrimination is provided jointly by the `surfaceHover` background fill (which lightens the surface before the border is drawn) and the semi-transparent cyan border. The border alone does not achieve 3:1 due to the low alpha, but the combined hover effect is visually clear. |
| WCAG ratio (composited, worst case) | 2.03:1 on `surface` (`#10131f`) |

---

## Surface Token Alignment

All four surface tokens were aligned to design system targets. No deviations required — all pass WCAG AA (≥4.5:1) for text contrast with comfortable margin:

| Token | Design target | Actual value | Text contrast |
|---|---|---|---|
| `surface` | `#10131f` | `#10131f` | 11.46:1 ✓ |
| `surfaceVariant` | `#161925` | `#161925` | 10.84:1 ✓ |
| `surfaceContainer` | `#1a1b26` | `#1a1b26` | 10.59:1 ✓ |
| `secondary` | `#24283b` | `#24283b` (unchanged) | 9.02:1 ✓ |
