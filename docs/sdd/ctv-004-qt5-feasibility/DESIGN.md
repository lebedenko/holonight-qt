# CTV-004 design

The probe uses source-level reuse with separately compiled Qt-major target graphs. Stable public Qt APIs are shared;
configuration adaptation and private QPA integration remain Qt-5-local. This is the smallest boundary that proves
feasibility without creating an installed support promise or coupling Qt 5 to Qt 6 binaries.

The palette/style and platform-theme candidates have independent targets, tests, output directories, and decisions.
The style can operate without the platform theme, and native Qt startup remains the fallback when either is absent.
