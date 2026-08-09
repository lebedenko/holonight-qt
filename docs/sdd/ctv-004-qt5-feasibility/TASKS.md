# CTV-004 tasks

| Task | State | Evidence |
|---|---|---|
| Define Qt-major source and package boundaries | Done | `SPEC.md`; `BUILD_QT5_PROBES=OFF` by default |
| Compile palette/style against Qt 5 | Done | `holonight_qt5_style`; build-tree style plugin |
| Prototype private-QPA platform theme | Done | `qholonight_qt5`; exact-minor rebuild policy documented |
| Add palette, resolver, style, hint, and linkage tests | Done | `holonight_qt5_probe_tests`; `holonight_qt5_linkage_test` |
| Exercise KeePassXC and native fallback | Done | Isolated Wayland/XWayland commands and observations in `SPEC.md` |
| Record separate recommendations | Done | Reject CTV-201 and CTV-202; recommend native fallback |
