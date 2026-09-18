#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
"""UQC-223 opt-in collection and independent stability assertion (never CTest)."""

import argparse
import itertools
import json
from pathlib import Path
import subprocess
import sys


def collect(args):
    args.output.mkdir(parents=True, exist_ok=False)
    experiments = []
    for repeat, scale, history, read in itertools.product(
        range(2), ["1", "1.25"], ["dark", "light"], ["passive", "read"]
    ):
        experiments.append(
            (
                f"activation-{repeat}-{scale}-{history}-{read}",
                "activation",
                scale,
                history,
                read,
                [],
            )
        )
    for trigger, history, read in itertools.product(
        ["hover", "resolve", "application-event", "enabled"],
        ["dark", "light"],
        ["passive", "read"],
    ):
        experiments.append(
            (f"{trigger}-{history}-{read}", trigger, "1.25", history, read, [])
        )
    for read in ["passive", "read"]:
        for boundary in ["plain", "native"]:
            experiments.append(
                (
                    f"{boundary}-{read}",
                    "activation",
                    "1.25",
                    "dark",
                    read,
                    ["--boundary", boundary],
                )
            )
        experiments.append(
            (
                f"no-theme-{read}",
                "activation",
                "1.25",
                "dark",
                read,
                ["--no-platform-theme"],
            )
        )
    if args.observer:
        # Keep the original allocating Qt-boundary matrix available unchanged.
        experiments = [e for e in experiments if e[4] == "passive"]
        for scope in ["application", "window", "control"]:
            for history in ["dark", "light"]:
                experiments.append(
                    (
                        f"override-{scope}-{history}",
                        "activation",
                        "1.25",
                        history,
                        "passive",
                        ["--override-scope", scope],
                    )
                )
        experiments = [
            (
                f"{name}-{mode}",
                trigger,
                scale,
                history,
                read,
                extra + (["--observer", str(args.observer)] if mode == "on" else []),
            )
            for name, trigger, scale, history, read, extra in experiments
            for mode in ["off", "on"]
        ]
    (args.output / "collection.json").write_text(
        json.dumps(
            {
                "profile": "observer-repair" if args.observer else "qt-boundary",
                "experiments": [e[0] for e in experiments],
                "expected_cases": 98 if args.observer else 74,
            },
            indent=2,
        )
        + "\n"
    )
    for name, trigger, scale, history, read, extra in experiments:
        command = [
            sys.executable,
            str(Path(__file__).with_name("palette-transition.py")),
            "collect",
            "--prefix",
            str(args.prefix),
            "--executable",
            str(args.executable),
            "--output",
            str(args.output / name),
            "--stability",
            trigger,
            "--scale",
            scale,
            "--history",
            history,
            "--content-palette",
            read,
            *extra,
        ]
        with (args.output / f"{name}.log").open("w") as log:
            result = subprocess.run(
                command, stdout=log, stderr=subprocess.STDOUT, timeout=90
            )
        if result.returncode:
            raise RuntimeError(f"Collection failed: {name}; see log")
        print(name, "collected", flush=True)


def controls(state):
    return {o["name"]: o for o in state["objects"] if o["name"]}


def check(args):
    cases = []
    paths = sorted(args.output.rglob("measurements.json"))
    if not paths:
        raise RuntimeError("No measurements; collection is not a behavioral pass")
    # Reject a partial suite instead of reporting its passing subset as complete.
    nested = any(p.parent.parent != args.output for p in paths)
    manifest = args.output / "collection.json"
    expected = (
        json.loads(manifest.read_text())["expected_cases"] if manifest.exists() else 74
    )
    if nested and len(paths) != expected:
        raise RuntimeError(f"Expected {expected} independent cases, found {len(paths)}")
    for path in paths:
        experiment = json.loads(path.with_name("experiment.json").read_text())
        records = json.loads(path.read_text())["records"]
        assert [r["phase"] for r in records] == [
            f"{s}-{p}"
            for s in ["light", "dark-return"]
            for p in ["before", "enter", "leave"]
        ]
        failures = []
        rendered = []
        for start in [0, 3]:
            before = records[start]
            for after in records[start + 1 : start + 3]:
                assert before["requestedAppearance"] == after["requestedAppearance"]
                for window in ["primary", "secondary"]:
                    assert (
                        before[window]["appearanceRevision"]
                        == after[window]["appearanceRevision"]
                    )
                    old = controls(before[window])
                    for timing, key in [
                        ("immediate", "immediate" + window.title()),
                        ("settled", window),
                    ]:
                        new = controls(after[key])
                        for name in [
                            "nativeButton",
                            "nativeText",
                            "nativeSpin",
                            "nativeCombo",
                            "disabledText",
                            "localOverride",
                            "sharedSearch",
                            "sharedCombo",
                        ]:
                            if name not in old:
                                assert experiment[
                                    "boundary"
                                ] == "native" and name.startswith("shared")
                                continue
                            assert old[name]["identity"] == new[name]["identity"]
                            for group in ["0", "1", "2"]:
                                for role in [
                                    "Base",
                                    "Button",
                                    "Text",
                                    "ButtonText",
                                    "Window",
                                    "Highlight",
                                ]:
                                    a, b = (
                                        old[name]["palette"][group][role],
                                        new[name]["palette"][group][role],
                                    )
                                    if a != b:
                                        failures.append(
                                            {
                                                "phase": after["phase"],
                                                "window": window,
                                                "timing": timing,
                                                "control": name,
                                                "group": group,
                                                "role": role,
                                                "before": a,
                                                "after": b,
                                            }
                                        )
                            # Text-field center-right is fill, with no hover animation.
                            if (
                                timing == "settled"
                                and name == "nativeText"
                                and experiment["trigger"] == "activation"
                            ):
                                if old[name]["pixel"] != new[name]["pixel"]:
                                    rendered.append(
                                        {
                                            "phase": after["phase"],
                                            "window": window,
                                            "before": old[name]["pixel"],
                                            "after": new[name]["pixel"],
                                        }
                                    )
        reset_path = path.with_name("override-reset.json")
        if reset_path.exists():
            reset = json.loads(reset_path.read_text())
            for window in ["primary", "secondary"]:
                field = controls(reset[window])["nativeText"]
                for group in ["0", "1", "2"]:
                    for role in ["Base", "Text"]:
                        expected = reset[window]["palette"][group][role]
                        actual = field["palette"][group][role]
                        if actual != expected:
                            failures.append(
                                {
                                    "phase": "override-reset",
                                    "window": window,
                                    "control": "nativeText",
                                    "group": group,
                                    "role": role,
                                    "before": expected,
                                    "after": actual,
                                }
                            )
        cases.append(
            {
                "case": str(path.parent.relative_to(args.output)),
                "experiment": experiment,
                "failures": failures,
                "rendered_reversals": rendered,
            }
        )
        print(
            cases[-1]["case"],
            len(failures),
            "role failures;",
            len(rendered),
            "rendered reversals",
        )
    equivalence = []
    if (
        manifest.exists()
        and json.loads(manifest.read_text())["profile"] == "observer-repair"
    ):
        for path in paths:
            if not path.parent.parent.name.endswith("-on"):
                continue
            off = (
                path.parent.parent.with_name(path.parent.parent.name[:-3] + "-off")
                / path.parent.name
                / path.name
            )
            observed = json.loads(path.read_text())["records"]
            passive = json.loads(off.read_text())["records"]
            experiment = json.loads(path.with_name("experiment.json").read_text())
            for left, right in zip(observed, passive, strict=True):
                for window in [
                    "primary",
                    "secondary",
                    "initialPrimary",
                    "beforeSecondaryPrimary",
                ]:
                    if window not in left:
                        continue
                    a, b = left[window], right[window]
                    assert a["paletteAllocated"] == b["paletteAllocated"]
                    assert a["palette"] == b["palette"]
                    assert a["contentItem"]["paletteAllocated"] is False
                    assert b["contentItem"]["paletteAllocated"] is False
                    ac, bc = controls(a), controls(b)
                    assert ac.keys() == bc.keys()
                    for name in ac:
                        for key in ["paletteAllocated", "palette"]:
                            assert ac[name].get(key) == bc[name].get(key), (
                                path,
                                window,
                                name,
                                key,
                            )
                        if name == "nativeText":
                            assert ac[name]["pixel"] == bc[name]["pixel"], (
                                path,
                                window,
                                name,
                            )
            if (
                experiment["boundary"] == "shared"
                and experiment["override_scope"] == "none"
            ):
                first = observed[0]
                initial = controls(first["initialPrimary"])
                light = controls(first["beforeSecondaryPrimary"])
                dark = controls(observed[3]["primary"])
                for name, role in [
                    ("nativeButton", "Button"),
                    ("nativeText", "Base"),
                    ("nativeSpin", "Base"),
                    ("nativeCombo", "Button"),
                    ("disabledText", "Base"),
                    ("sharedSearch", "Base"),
                    ("sharedCombo", "Base"),
                ]:
                    for group in ["0", "1", "2"]:
                        assert (
                            initial[name]["palette"][group][role]
                            != light[name]["palette"][group][role]
                        )
                        assert (
                            initial[name]["palette"][group][role]
                            == dark[name]["palette"][group][role]
                        )
                assert initial["nativeText"]["pixel"] != light["nativeText"]["pixel"]
                assert initial["nativeText"]["pixel"] == dark["nativeText"]["pixel"]
            log = path.with_name("launch.log").read_text()
            observer_records = [
                json.loads(line.split("HN_PALETTE ", 1)[1])
                for line in log.splitlines()
                if "HN_PALETTE " in line
            ]
            assert any(r.get("paletteAllocated") is False for r in observer_records)
            assert any(
                "event" in r and r.get("beforeDelivery") for r in observer_records
            )
            equivalence.append(str(path.parent.relative_to(args.output)))
        assert len(equivalence) == 49
    report = {
        "observer_equivalence": equivalence,
        "cases": cases,
        "failed_cases": sum(
            bool(c["failures"] or c["rendered_reversals"]) for c in cases
        ),
    }
    (args.output / "stability-assertions.json").write_text(
        json.dumps(report, indent=2) + "\n"
    )
    print(report["failed_cases"], "failing cases; collection success is independent")
    return bool(report["failed_cases"])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=["collect", "assert"])
    parser.add_argument("--output", required=True, type=lambda p: Path(p).resolve())
    parser.add_argument("--observer", type=lambda p: Path(p).resolve())
    parser.add_argument("--prefix", type=lambda p: Path(p).resolve())
    parser.add_argument("--executable", type=lambda p: Path(p).resolve())
    args = parser.parse_args()
    if args.action == "collect":
        if not args.prefix or not args.executable:
            parser.error("collect needs --prefix and --executable")
        collect(args)
        return 0
    return check(args)


if __name__ == "__main__":
    raise SystemExit(main())
