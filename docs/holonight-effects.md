# Optional effects

## Good places

Focus ring
Active tab underline
Default/primary button hover
Progress bar fill
Switch checked state
Slider handle/fill
Selected item accent
Tooltip/popup subtle border

## Avoid

Every button border
Every frame border
List/table backgrounds
Text fields at rest
Menu items at rest
Whole window backgrounds
Large constant glows

## Recommendation

Default state: no glow, no gradient
Hover: tiny gradient or brighter border
Focus: clear cyan glow/ring
Active/selected: cyan→violet gradient
Urgent/destructive: red/orange, not neon everywhere

## Config

enableGlow = false by default, or very subtle
enableGradients = true only for accent fills
