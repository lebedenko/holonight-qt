---
name: verify
description: Run the full holonight pre-commit checklist — cmake build, clang-tidy, and ctest — and report any failures. Use before committing or after a significant change.
---

Run the following steps in sequence. Stop and report clearly if any step fails; do not proceed to the next step after a failure.

1. **Build**
   ```bash
   cmake --build build -j$(nproc)
   ```
   Report any compiler errors or warnings.

2. **Clang-tidy**
   Try `cmake --build build --target clang-tidy` first. If that target doesn't exist, fall back to:
   ```bash
   run-clang-tidy -p build
   ```
   Report any clang-tidy diagnostics. Zero diagnostics is the goal.

3. **Tests**
   ```bash
   QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
   ```
   Report the pass/fail count and any failing test names with their output.

At the end, print a single summary line:
- "✓ All checks passed." if everything succeeded.
- "✗ Failed at: [build|clang-tidy|tests] — [brief reason]." if anything failed.
