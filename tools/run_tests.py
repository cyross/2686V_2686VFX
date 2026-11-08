#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""組んだテストを、小分けにして並べて走らせる。

    python tools/run_tests.py                 # out/b の Debug を全部
    python tools/run_tests.py GuiTests_2686V  # 名前に含む exe だけ
    python tools/run_tests.py --jobs 8        # 同時に走らせる数

テストの exe を 1 本ずつ頭から走らせると、GuiTests_2686V だけで 14 分ほど
掛かる (重いテストが 1 つあるのではなく、エディタを組み立てるテストが
91 件並んでいて、どれもそれなりに重い)。

doctest の --first / --last で 1 本の exe を何切れかに分け、全部の切れを
同時に走らせる。テストの中身は変えないので、確かめることは同じ。

切れが 1 つ終わるたびに 1 行出す (どこまで進んだか分かるように)。
落ちたものがあれば、最後にその切れの出力をまとめて出し、終了コードを 1 にする。
"""
import argparse
import concurrent.futures
import glob
import os
import re
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# 1 切れに入れるテストの数。小さいほど散らばるが、exe を立ち上げる手間が増える
CHUNK = 6

COUNT_RE = re.compile(rb"passing the current filters:\s*(\d+)")
RESULT_RE = re.compile(rb"test cases:\s*(\d+)\s*\|\s*(\d+) passed\s*\|\s*(\d+) failed")


def find_exes(build_dir, config, names):
    patterns = [
        os.path.join(build_dir, "tests", config, "*.exe"),
        os.path.join(build_dir, "tests", "*_artefacts", config, "*.exe"),
    ]
    exes = sorted({p for pat in patterns for p in glob.glob(pat)})

    if names:
        exes = [e for e in exes if any(n.lower() in os.path.basename(e).lower() for n in names)]

    return exes


def count_cases(exe):
    out = subprocess.run([exe, "--count"], capture_output=True).stdout
    m = COUNT_RE.search(out)

    return int(m.group(1)) if m else 0


def run_chunk(exe, first, last):
    started = time.time()
    proc = subprocess.run([exe, f"--first={first}", f"--last={last}"], capture_output=True)
    out = proc.stdout + proc.stderr
    m = RESULT_RE.search(out)

    # 締めの「test cases」は --first / --last で外したものまで通過に数えるので、
    # 通過の数は切れの範囲から出す。失敗の数は走らせたものだけが数えられる。
    failed = int(m.group(3)) if m else -1
    ok = proc.returncode == 0 and failed == 0
    passed = (last - first + 1) if ok else 0

    return exe, first, last, passed, failed, ok, time.time() - started, out


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("names", nargs="*", help="exe の名前に含む文字 (省けば全部)")
    parser.add_argument("--build-dir", default=os.path.join(ROOT, "out", "b"))
    parser.add_argument("--config", default="Debug")
    parser.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 4) - 4))
    args = parser.parse_args()

    exes = find_exes(args.build_dir, args.config, args.names)

    if not exes:
        print("テストの exe が見つかりません。先に組んでください。")
        return 1

    chunks = []

    for exe in exes:
        n = count_cases(exe)

        for first in range(1, n + 1, CHUNK):
            chunks.append((exe, first, min(n, first + CHUNK - 1)))

    # 重い exe (切れの多いもの) を先に回すと、最後に 1 本だけ残って待つことが減る
    weight = {}

    for exe, _, _ in chunks:
        weight[exe] = weight.get(exe, 0) + 1

    chunks.sort(key=lambda c: -weight[c[0]])

    print(f"{len(exes)} 本の exe を {len(chunks)} 切れに分け、{args.jobs} 並びで走らせます", flush=True)

    started = time.time()
    done = 0
    passed_total = 0
    failures = []
    per_exe = {}

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(run_chunk, *c) for c in chunks]

        for f in concurrent.futures.as_completed(futures):
            exe, first, last, passed, failed, ok, secs, out = f.result()
            done += 1
            passed_total += passed

            name = os.path.basename(exe)
            per_exe.setdefault(name, [0, 0])
            per_exe[name][0] += passed
            per_exe[name][1] += max(0, failed) if failed >= 0 else 1

            mark = "OK" if ok else "NG"
            print(f"[{done}/{len(chunks)} {time.time() - started:5.0f}s] {mark} {name} #{first}-{last} ({secs:.0f}s)", flush=True)

            if not ok:
                failures.append((name, first, last, out))

    print("", flush=True)

    for name in sorted(per_exe):
        p, fl = per_exe[name]
        print(f"  {name}: {p} 件通過" + (f" / {fl} 件失敗" if fl else ""))

    print(f"合計 {passed_total} 件通過、失敗 {len(failures)} 切れ、{time.time() - started:.0f} 秒", flush=True)

    for name, first, last, out in failures:
        print(f"\n===== {name} #{first}-{last} =====")
        print(out.decode("utf-8", errors="replace"))

    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
