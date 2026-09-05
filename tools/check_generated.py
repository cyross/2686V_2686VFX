#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""生成したプラグインで、さわってはいけないファイルが変わっていないか調べる。

    python tools/check_generated.py

gen_plugin.py は「消す音源への言及を落とす」という削りをするので、
行きすぎると関係のないところまで消える。次の 2 つは 2686V と
同じ中身でなければならないので、それを突き合わせる。

  ・残した音源のディレクトリ (Gui/<chip>, Synth/<chip>, Processor/<chip>)
  ・共有の実装 (Effect, Generator, Advanced, Gui/Components)

出てくる差は、ひとつずつ意図したものか確かめること。いまのところ
意図した差は次の 1 件だけ。

  Gui/Components/WavePreview/WavePreviewSource.cpp
      OPZX7 のオペレータ波形プレビュー (opzx7Ws) は OPZX7 タブからしか
      呼ばれないので、OPZX7 を外したプラグインでは消える。
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import gen_plugin as g  # noqa: E402

SHARED_ROOTS = ("Effect/", "Generator/", "Advanced/", "Gui/Components/")


def main():
    total = 0

    for name, spec in g.PLUGINS.items():
        keep_dirs = tuple(d + "/" for k in spec["chips"] for d in g.CHIPS[k]["dirs"])
        src = os.path.join(ROOT, g.SOURCE_PLUGIN, "Source")
        dst = os.path.join(ROOT, name, "Source")

        if not os.path.isdir(dst):
            continue

        bad = []

        for dirpath, _, files in os.walk(dst):
            for f in files:
                if not g.is_source(f):
                    continue

                path = os.path.join(dirpath, f)
                rel = os.path.relpath(path, dst).replace(os.sep, "/")

                if not (rel.startswith(keep_dirs) or rel.startswith(SHARED_ROOTS)):
                    continue

                orig = os.path.join(src, rel.replace("/", os.sep))

                if not os.path.exists(orig):
                    bad.append((rel, "元に無い"))
                elif open(orig, "rb").read() != open(path, "rb").read():
                    bad.append((rel, "中身が違う"))

        print("%-8s … %s" % (name, "問題なし" if not bad else "%d 件" % len(bad)))

        for rel, why in bad:
            print("    %s  %s" % (why, rel))

        total += len(bad)

    print()
    print("合計 %d 件" % total)

    return 0


if __name__ == "__main__":
    sys.exit(main())
