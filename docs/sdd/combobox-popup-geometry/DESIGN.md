# ComboBox Popup Geometry Design

The ComboBox already maps its control origin and axes into scene coordinates and sizes the popup for a supported
uniform transform. A `Popup.Item` interprets `x` and `y` relative to its parent, so leaving the implicit parent in a
translated consumer mixes coordinate systems and lets popup repositioning clamp the result to an unrelated window
margin. Explicitly parenting the popup to `Overlay.overlay` makes its coordinate space match those calculations.

Regression coverage maps the rendered popup background back to the window content item. This observes the actual
result after parenting, scaling, transform origin, and popup repositioning instead of restating the logical `x/y`
formula.

QML cannot infer change dependencies from `mapToItem()` across ancestor transforms. A small popup geometry revision
is incremented by `aboutToShow` and participates in the scene-point bindings, forcing those mappings to be sampled
after the consumer's latest layout and before popup positioning.
