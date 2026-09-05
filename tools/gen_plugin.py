#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""2686V から音源を絞ったプラグインを作り出す。

このリポジトリはプラグインごとに Source ツリーを丸ごと持つ作りになっている。
手で複製すると、共有コードを直すたびに全部へ同じ手を入れることになるので、
「2686V を元に、残す音源を指定して作り直す」形にしてある。

    python tools/gen_plugin.py OPMV

直したいことがあるときは 2686V を直し、このスクリプトを流し直す。
生成されたツリーを直接触っても、次の生成で消えるので触らないこと。

ファイルはすべて BOM 付き UTF-8・CRLF で書き出す (リポジトリの決まり)。
"""

import argparse
import os
import re
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_PLUGIN = "2686V"

# ---------------------------------------------------------------------------
# 音源の一覧
#
#   dirs   … その音源だけが持つディレクトリ (Source からの相対)
#   camel  … 識別子で使う頭大文字の綴り (GuiOpm, OpmCore など)
#   lower  … 変数名で使う綴り (opmGui, params.opm など)
#   mode   … OscMode の名前
#   extra  … 規則で拾えない、その音源だけの名前 (共有ファイルに書かれた関数など)
#   label  … OscMode の並びに書くコメント
# ---------------------------------------------------------------------------
CHIPS = {
    "OPNA":  dict(dirs=["Gui/Opna", "Synth/Opna", "Processor/Opna"],
                  camel="Opna", lower="opna", mode="OPNA", color="Fm", label="YM2608"),
    "OPN":   dict(dirs=["Gui/Opn", "Synth/Opn", "Processor/Opn"],
                  camel="Opn", lower="opn", mode="OPN", color="Fm", label="YM2203"),
    "OPL":   dict(dirs=["Gui/Opl", "Synth/Opl", "Processor/Opl"],
                  camel="Opl", lower="opl", mode="OPL", color="Fm", label="YM3526 (2op + WaveSelect)"),
    "OPL3":  dict(dirs=["Gui/Opl3", "Synth/Opl3", "Processor/Opl3"],
                  camel="Opl3", lower="opl3", mode="OPL3", color="Fm", label="YMF262"),
    "OPM":   dict(dirs=["Gui/Opm", "Synth/Opm", "Processor/Opm"],
                  camel="Opm", lower="opm", mode="OPM", color="Fm", label="YM2151 (X68000/Arcade)"),
    "OPZX7": dict(dirs=["Gui/Opzx7", "Synth/Opzx7", "Processor/Opzx7"],
                  camel="Opzx7", lower="opzx7", mode="OPZX7", color="Fm", label="OPZ 系の拡張 8 オペレータ"),
    "SSG":   dict(dirs=["Gui/Ssg", "Synth/Ssg", "Processor/Ssg"],
                  camel="Ssg", lower="ssg", mode="SSG", color="Ssg", label="AY-3-8910 系"),
    "WT":    dict(dirs=["Gui/Wavetable", "Synth/Wavetable", "Processor/Wavetable"],
                  camel="Wt", lower="wt", mode="WAVETABLE", color="Wt", label="波形メモリ"),
    "WT2":   dict(dirs=["Gui/Wt2", "Synth/Wt2", "Processor/Wt2"],
                  camel="Wt2", lower="wt2", mode="WT2", color="Wt", label="波形メモリ (2 系統)"),
    "WTPLUS": dict(dirs=["Gui/WtPlus", "Synth/WtPlus", "Processor/WtPlus"],
                   camel="WtPlus", lower="wtPlus", mode="WTPLUS", color="Wt",
                   extra=["isWtPlusWaveLoaded"],
                   label="波形メモリを 32 スロット持つ拡張ウェーブテーブル"),
    "RHYTHM": dict(dirs=["Gui/Rhythm", "Synth/Rhythm", "Processor/Rhythm"],
                   camel="Rhythm", lower="rhythm", mode="RHYTHM", color="Pcm", label="リズム音源"),
    "ADPCM": dict(dirs=["Gui/Adpcm", "Synth/Adpcm", "Processor/Adpcm"],
                  camel="Adpcm", lower="adpcm", mode="ADPCM", color="Pcm", label="ADPCM"),
    "BEEP":  dict(dirs=["Gui/Beep", "Synth/Beep", "Processor/Beep"],
                  camel="Beep", lower="beep", mode="BEEP", color="Beep", label="ビープ"),
}

# ---------------------------------------------------------------------------
# 作るプラグインの一覧
#
#   code    … VST3 のプラグインコード (4 文字。他と重ならないこと)
#   product … 出力名。省略するとターゲット名がそのまま使われる
#   ext     … プリセットの拡張子 (先頭の . は付けない)
#   chips   … 残す音源。並びがそのままタブと OscMode の並びになる
# ---------------------------------------------------------------------------
PLUGINS = {
    "FMsV":   dict(code="FMSV", ext="fmsv",
                   chips=["OPNA", "OPN", "OPL", "OPL3", "OPM", "OPZX7"]),
    "OPNV":   dict(code="OPNV", ext="opnv", chips=["OPNA", "OPN"]),
    "OPLV":   dict(code="OPLV", ext="oplv", chips=["OPL", "OPL3"]),
    "OPMV":   dict(code="OPMV", ext="opmv", chips=["OPM"]),
    "WTV":    dict(code="WTVO", ext="wtv", chips=["WT", "WT2", "WTPLUS"]),
    "PCMV":   dict(code="PCMV", ext="pcmv", chips=["RHYTHM", "ADPCM"]),
    "PULSEV": dict(code="PLSV", ext="pulsev", chips=["SSG", "BEEP"]),
}


# ---------------------------------------------------------------------------
# 入出力 (BOM 付き UTF-8・CRLF を守る)
# ---------------------------------------------------------------------------
def read_text(path):
    data = open(path, "rb").read()

    return data.decode("utf-8-sig")


def write_text(path, text):
    text = text.replace("\r\n", "\n").replace("\n", "\r\n")

    with open(path, "wb") as f:
        f.write(b"\xef\xbb\xbf" + text.encode("utf-8"))


def is_source(path):
    return path.endswith(".h") or path.endswith(".cpp")


# ---------------------------------------------------------------------------
# その音源のディレクトリで宣言されている型の名前を集める。
# ここに出てくる名前は、その音源が消えれば一緒に消えるもの。
# Effect や Generator に置いてある LfoOpm や DetuneOpn は共有部品なので、
# ディレクトリで見ている限り巻き込まれない。
# ---------------------------------------------------------------------------
DECL = re.compile(r"^\s*(?:class|struct|namespace|enum\s+class|using)\s+([A-Za-z_][A-Za-z0-9_]*)", re.M)

# 前方宣言でどこにでも出てくるので、音源の持ち物とはみなさない
NOT_OWNED = {"AudioPlugin2686V", "AudioPlugin2686VEditor"}


def declared_names(paths):
    names = set()

    for path in paths:
        for m in DECL.finditer(read_text(path)):
            if m.group(1) not in NOT_OWNED:
                names.add(m.group(1))

    return names


def all_sources(base):
    out = []

    for dirpath, _, files in os.walk(base):
        for name in files:
            if is_source(name):
                out.append(os.path.join(dirpath, name))

    return out


def owned_symbols(src_root, chips, keep_dirs=()):
    """消す音源だけが持っている名前を集める。

    その音源のディレクトリで宣言されていても、外にも同じ名前があるものは
    数えない。namespace File や enum class Target のような、ありふれた名前が
    まぎれ込むと、関係のない行まで巻き込んでしまう。
    """
    inside = []
    chip_dirs = []

    for chip in chips:
        for rel in chip["dirs"]:
            base = os.path.join(src_root, rel.replace("/", os.sep))

            if os.path.isdir(base):
                chip_dirs.append(os.path.normpath(base))
                inside += all_sources(base)

    outside = [p for p in all_sources(src_root)
               if not any(os.path.normpath(p).startswith(d + os.sep) for d in chip_dirs)]

    core = os.path.normpath(os.path.join(src_root, "Core"))

    # 共有部品に出てくる名前。Effect や Gui/Components に一度でも出てくる
    # ものは、音源の名前を含んでいても皆で使うもの (ssgSwStepsSlider など)。
    shared = set()

    for path in outside:
        if os.path.normpath(path).startswith(core + os.sep):
            continue

        shared |= set(IDENT.findall(read_text(path)))

    # 共有ファイルで宣言されている型は、名前に音源の綴りを含んでいても
    # 皆のもの (WtModWaveSlot など)。
    shared |= declared_names(outside)

    # 残す音源が使っている名前も守る。WtModWaveStore のように、
    # 名前に音源の綴りを含んでいても、残る側が使うものがある。
    for rel in keep_dirs:
        base = os.path.join(src_root, rel.replace("/", os.sep))

        if os.path.isdir(base):
            for path in all_sources(base):
                shared |= set(IDENT.findall(read_text(path)))

    return declared_names(inside) - declared_names(outside), shared


def drop_patterns(chip):
    """その音源に触っている行を見つけるための当たり判定 (OscMode 以外)。"""
    camel = chip["camel"]
    lower = chip["lower"]

    pats = [
        re.compile(r"\b%sGui\b" % re.escape(lower)),
        re.compile(r"\btab%s\b" % re.escape(camel)),
        re.compile(r"\bTab::%s\b" % re.escape(lower)),
        re.compile(r"(?:params|currentParams)\s*(?:->|\.)\s*%s\b" % re.escape(lower)),
        re.compile(r"\bm_%sCore\b" % re.escape(lower)),
        re.compile(r"\bpr%s\b" % re.escape(camel)),
    ]

    # 規則では拾えない名前を、必要なぶんだけ足す
    for name in chip.get("extra", ()):
        pats.append(re.compile(r"\b%s\b" % re.escape(name)))

    return pats


IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")

# 音源の綴りを含んでいても、皆で使う機能の名前。
# SSG 風のエンベロープや WT 変調は、SSG や WT の音源が無くても
# どの音源からも使えるので、音源の持ち物と間違えないようにする。
# 共有ファイルに置かれた「音源ごとの関数」の書き出し。
CHIP_VERBS = (
    "set", "clear", "load", "unload", "update", "refresh",
    "init", "reset", "apply", "generate", "copy", "paste",
)

SHARED_FEATURES = (
    "SsgSw", "SsgHw", "SsgEg", "OpSsg", "EnvSsg", "PEnvSsg",
    "WtMod", "WtAmpMod", "WtPitchMod",
)


def chip_member_hit(code, camels, lowers, shared, verbs_only=False):
    """Core 配下で「音源の名前を含む識別子」に触っているか。

    setOpzx7PcmBuffer や updateRhythmFileNames のような、共有ファイルへ
    書かれている音源ごとの関数を見つけるためのもの。Effect や Generator に
    置いてある LfoOpm・KSOpn のような共有部品の名前は数えない。

    Core の外へ広げると、共有部品の名前まで巻き込むので当てない。
    """
    for word in IDENT.findall(code):

        # 大文字小文字は問わない (ssgSwStepsSlider も SsgSwEnv も同じ機能)
        low = word.lower()

        if any(f.lower() in low for f in SHARED_FEATURES):
            continue

        # setOpzx7PcmBuffer / updateRhythmFileNames のような、
        # 「動詞 + 音源名」で書かれた共有ファイル側の関数を拾う。
        # 音源名をただ含むだけでは当てない (resolveWtPath のように、
        # 音源が無くても要るものがある)。
        for camel in camels:
            for verb in CHIP_VERBS:
                if word.startswith(verb + camel):
                    return True

        if verbs_only or word in shared:
            continue

        # wtPlusWaves のように、変数名の頭が音源の綴りのものも拾う
        for lower in lowers:
            if (word.startswith(lower) and len(word) > len(lower)
                    and word[len(lower)].isupper()):
                return True

    return False


STRING = re.compile(r'"(?:[^"\\]|\\.)*"')
COMMENT = re.compile(r"//.*$")


def code_only(line):
    """当たり判定に使う部分だけ残す。

    文字列の中身と行末のコメントは外す。"Tri" のような字面や、説明文に
    出てくる型の名前で行が消えてしまうのを防ぐ。
    """
    return COMMENT.sub("", STRING.sub('""', line))


# 頭の 1 語がこれなら関数ではない。中の 1 行が当たっただけで、
# 型や制御構文をまるごと消してしまうのを防ぐ。
NOT_FUNC_WORDS = {
    "class", "struct", "namespace", "enum", "union",
    "if", "else", "for", "while", "do", "switch", "try", "catch", "return",
}

FIRST_WORD = re.compile(r"\s*([A-Za-z_][A-Za-z0-9_]*)")


def first_word(line):
    m = FIRST_WORD.match(line)

    return m.group(1) if m else ""


def head_span(lines, j):
    """j 行目で開いた囲いの、頭の始まり行を返す。

    「{」が次の行に置かれていたり、引数が何行かに分かれていたりするので、
    前の行が続きに見えるあいだはさかのぼる。
    """
    k = j

    while k - 1 >= 0:
        prev = code_only(lines[k - 1]).rstrip()

        if not prev or prev.endswith((";", "{", "}", ":")) or prev.lstrip().startswith(("//", "#")):
            break

        k -= 1

    return k


def enclosing_function(lines, i):
    """i 行目を含む関数の範囲 (start, end) を返す。見つからなければ None。

    関数の中の 1 行だけを消すと、その変数を使っている残りが宙に浮く。
    まとまりごと落とすために、囲っている関数を探す。
    """
    depth = 0
    j = i

    while j >= 0:
        depth += lines[j].count("}") - lines[j].count("{")

        if depth < 0:
            start = head_span(lines, j)
            head = "".join(lines[start:j + 1])

            if (first_word(lines[start]) in NOT_FUNC_WORDS
                    or "(" not in head or ")" not in head):
                return None

            return start, block_end(lines, j)

        j -= 1

    return None


def statement_span(lines, i):
    """i 行目を含む「1 つの文」の範囲 (start, end) を返す。

    引数が何行かに分かれている呼び出しの途中だけを消すと、括弧が閉じ
    なくなる。行ではなく文の単位で落とすためのもの。
    """
    # #include などの行は、それだけで 1 つのまとまり
    if lines[i].lstrip().startswith("#"):
        return i, i + 1

    start = i

    while start - 1 >= 0:
        prev = code_only(lines[start - 1]).rstrip()

        if (not prev or prev.endswith((";", "{", "}", ":"))
                or prev.lstrip().startswith(("//", "#"))):
            break

        start -= 1

    depth = 0
    j = start

    while j < len(lines):
        depth += lines[j].count("(") - lines[j].count(")")
        text = code_only(lines[j]).rstrip()

        if depth <= 0 and (text.endswith(";") or text.endswith("{")
                           or text.endswith("}")):
            return start, j + 1

        j += 1

    return start, i + 1


def block_end(lines, i):
    """i 行目から始まるまとまりの終わり (次に読む行) を返す。"""
    line = lines[i]

    # case ... : は break; か return まで
    if re.search(r"^\s*case\s+", line):
        if re.search(r"\breturn\b", line):
            return i + 1

        depth = 0
        j = i

        while j < len(lines):
            depth += lines[j].count("{") - lines[j].count("}")

            if depth <= 0 and re.search(r"\b(?:break;|return\b)", lines[j]):
                return j + 1

            j += 1

        return len(lines)

    depth = line.count("{") - line.count("}")
    j = i + 1

    # 「{」が次の行に置かれている書き方に合わせる。
    # 頭だけ消して中身を残すと、閉じ括弧が宙に浮く。
    if depth == 0 and not line.rstrip().endswith(";"):
        k = j

        while k < len(lines) and not lines[k].strip():
            k += 1

        if k < len(lines) and lines[k].strip() == "{":
            depth = 1
            j = k + 1

    # 括弧が開いたままなら、閉じるまで
    while depth > 0 and j < len(lines):
        depth += lines[j].count("{") - lines[j].count("}")
        j += 1

    return j


def strip_lines(text, pats, include_pat, mode_pat, first_mode, last_mode,
                extra=None, symbols=()):
    """消す音源に触っている行を落とす。

    まず消す範囲を洗い出してから書き出す。行を消しながら進むと、
    あとから「囲っている関数ごと消す」と分かったときに巻き戻せない。

    OscMode だけに触っている行は、意味で分ける。
      ・case / 等値の比較 … その音源のための分岐なので、まとまりごと落とす
      ・それ以外 (初期値や範囲の上限) … 残っている音源の名前へ書き換える
    """
    lines = text.splitlines(keepends=True)
    dead = set()
    rewrite = {}

    def kill(start, end):
        # 直前の連続したコメント行も一緒に落とす
        while start - 1 >= 0 and lines[start - 1].lstrip().startswith("//"):
            start -= 1

        dead.update(range(start, end))

    for i, line in enumerate(lines):
        if i in dead:
            continue

        # #include だけは行そのもの、それ以外は文字列とコメントを外して見る
        code = code_only(line)
        hit_other = (include_pat.search(line) is not None
                     or any(p.search(code) for p in pats)
                     or (extra is not None and extra(code)))
        hit_mode = mode_pat.search(code) is not None

        if not hit_other and not hit_mode:
            continue

        if not hit_other:
            is_branch = (re.search(r"^\s*case\s+OscMode::", code)
                         or re.search(r"[=!]=\s*OscMode::", code)
                         or re.search(r'if\s*\(name\s*==\s*"', line))

            if not is_branch:
                # 上限として使っているものは最後の音源、それ以外は先頭の音源へ
                repl = last_mode if "(int)OscMode::" in code else first_mode
                rewrite[i] = mode_pat.sub("OscMode::" + repl, line)
                continue

        # 関数の中の 1 行だけを消すと残りが宙に浮くことがある。
        # ただし、まるごと落としてよいのは「消える型で変数を作っている」
        # 行のときだけ。呼び出しが 1 つ入っているだけで関数ごと消すと、
        # processBlock のような共通の関数まで無くなってしまう。
        span = None

        if first_word(code) in symbols:
            span = enclosing_function(lines, i)

        if span is not None:
            kill(span[0], span[1])
            continue

        # 行だけを消すと括弧が閉じなくなることがあるので、
        # 文としてのまとまりと、括弧のまとまりの広いほうを落とす
        s_start, s_end = statement_span(lines, i)
        b_end = block_end(lines, i)

        kill(min(i, s_start), max(b_end, s_end))

    return "".join(rewrite.get(i, l) for i, l in enumerate(lines) if i not in dead)


# ---------------------------------------------------------------------------
# SynthMode.h は作り直す。並びも既定値もここで決まるので、削るより早い。
# ---------------------------------------------------------------------------
SYNTH_MODE_TEMPLATE = """#pragma once
#include <JuceHeader.h>

%(consts)s
enum class OscMode
{
%(entries)s    Count = %(count)d, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
%(names)s    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
%(lookup)s
    return OscMode::%(first)s; // Default
}
"""


# ---------------------------------------------------------------------------
# タブの色は並び順で決まるので、音源を減らしたら番号を振り直す。
# ---------------------------------------------------------------------------
TAB_COLOR_HEAD = """juce::Colour CustomTabLookAndFeel::getTabHeaderColor(int tabIndex)
{
    // 色そのものは GuiColor::Tab にある。タブの並びはプラグインごとに
    // 違うので、どの番号がどの系統かだけをここで決める。
    // ここは tools/gen_plugin.py が書き出している。
    switch (tabIndex)
    {
"""

TAB_COLOR_TAIL = """    default: return GuiColor::Tab::Other;    // OTHER
    }
}
"""


def build_tab_color(keep):
    rows = []
    idx = 0

    for key in keep:
        chip = CHIPS[key]
        rows.append("    case %2d: return GuiColor::Tab::%-9s // %s\n"
                    % (idx, chip["color"] + ";", chip["mode"]))
        idx += 1

    # 音源のあとは、どのプラグインでも同じ並び
    for label, color in (("ADVANCED", "Advanced"), ("PRESET", "Utility"),
                         ("SETTINGS", "Utility"), ("COLORS", "Utility"),
                         ("ABOUT", "Utility")):
        rows.append("    case %2d: return GuiColor::Tab::%-9s // %s\n"
                    % (idx, color + ";", label))
        idx += 1

    return TAB_COLOR_HEAD + "".join(rows) + TAB_COLOR_TAIL


# ---------------------------------------------------------------------------
# タブの番号も並び順で決まる。音源を減らしたら振り直す。
# ---------------------------------------------------------------------------
def build_tab_index(keep):
    names = ["tab" + CHIPS[k]["camel"] for k in keep]
    head = names[0] + " = 0"
    rest = names[1:] + ["tabCurve", "tabPreset", "tabSettings", "tabColors", "tabAbout"]

    lines = ["    enum TabIndex\r\n", "    {\r\n",
             "        " + ", ".join([head] + rest) + ",\r\n",
             "\r\n", "        tabCount\r\n", "    };\r\n"]

    return "".join(lines)


def build_synth_mode(keep):
    consts = []

    if "RHYTHM" in keep:
        consts.append("static constexpr int MaxRhythmPads = 8;")

    # OPZX7 だけ 8 オペレータ。ほかの FM は 4 本まで。
    consts.append("static constexpr int MaxFmOperators = %d;"
                  % (8 if "OPZX7" in keep else 4))

    entries = ""
    names = ""
    lookup = ""

    for idx, key in enumerate(keep):
        chip = CHIPS[key]
        entries += "    %s = %d, // %s\n" % (chip["mode"], idx, chip["label"])
        names += '    case OscMode::%s: return "%s";\n' % (chip["mode"], chip["mode"])
        lookup += '    if (name == "%s") return OscMode::%s;\n' % (chip["mode"], chip["mode"])

    return SYNTH_MODE_TEMPLATE % dict(
        consts="\n".join(consts) + "\n",
        entries=entries,
        count=len(keep),
        names=names,
        lookup=lookup,
        first=CHIPS[keep[0]]["mode"],
    )


# ---------------------------------------------------------------------------
# 生成の本体
# ---------------------------------------------------------------------------
def generate(name):
    spec = PLUGINS[name]
    keep = spec["chips"]
    drop = [k for k in CHIPS if k not in keep]

    src_root = os.path.join(ROOT, SOURCE_PLUGIN, "Source")
    dst_dir = os.path.join(ROOT, name)
    dst_root = os.path.join(dst_dir, "Source")

    print("[%s] 元: %s / 残す音源: %s" % (name, SOURCE_PLUGIN, ", ".join(keep)))

    # 1. まるごと写す
    if os.path.isdir(dst_dir):
        shutil.rmtree(dst_dir)

    shutil.copytree(os.path.join(ROOT, SOURCE_PLUGIN, "Source"), dst_root)
    shutil.copytree(os.path.join(ROOT, SOURCE_PLUGIN, "Resources"),
                    os.path.join(dst_dir, "Resources"))

    # 2. 消す音源の持ち物を調べてから、ディレクトリごと落とす
    keep_dirs = [d for k in keep for d in CHIPS[k]["dirs"]]
    symbols, shared = owned_symbols(dst_root, [CHIPS[k] for k in drop], keep_dirs)
    camels = [CHIPS[k]["camel"] for k in drop]
    lowers = [CHIPS[k]["lower"] for k in drop]

    for key in drop:
        for rel in CHIPS[key]["dirs"]:
            path = os.path.join(dst_root, rel.replace("/", os.sep))

            if os.path.isdir(path):
                shutil.rmtree(path)

    # 3. 並びで決まるものは、削るより先に作り直す。
    #    先に整えておけば、このあとの削りが引っかからない。
    write_text(os.path.join(dst_root, "Core", "Synth", "SynthMode.h"),
               build_synth_mode(keep))

    eh_path = os.path.join(dst_root, "Core", "Editor", "PluginEditor.h")
    eh = read_text(eh_path)
    a = eh.index("    enum TabIndex")
    b = eh.index("};", a) + len("};\r\n")
    write_text(eh_path, eh[:a] + build_tab_index(keep) + eh[b:])

    # mode つまみの上限はタブの数で決まる。音源を減らしたら詰める。
    # タブは「音源 + ADV + PRESET + SETTINGS + COLORS + ABOUT」。
    gv_path = os.path.join(dst_root, "Core", "Gui", "GuiValues.h")
    gv = read_text(gv_path)
    write_text(gv_path, re.sub(r"TabNumber = \d+;",
                               "TabNumber = %d;" % (len(keep) + 5 - 1), gv, count=1))

    lf_path = os.path.join(dst_root, "Core", "Gui", "GuiLF.cpp")
    lf = read_text(lf_path)
    head = lf.index("juce::Colour CustomTabLookAndFeel::getTabHeaderColor")
    write_text(lf_path, lf[:head] + build_tab_color(keep))

    # 4. 残ったファイルから、消えた音源への言及を落とす
    pats = []

    for key in drop:
        pats += drop_patterns(CHIPS[key])

    # include はディレクトリを丸ごと見る。末尾だけで見ると
    # Effect/KeyScale/Opn のような共有部品まで巻き込んでしまう。
    include_dirs = [d for key in drop for d in CHIPS[key]["dirs"]]
    include_pat = re.compile(r'#include\s+"[^"]*(?:%s)/'
                             % "|".join(re.escape(d) for d in include_dirs))

    if symbols:
        pats.append(re.compile(r"\b(?:%s)\b" % "|".join(re.escape(s) for s in sorted(symbols))))

    mode_pat = re.compile(r"\bOscMode::(?:%s)\b"
                          % "|".join(CHIPS[k]["mode"] for k in drop))
    first_mode = CHIPS[keep[0]]["mode"]
    last_mode = CHIPS[keep[-1]]["mode"]

    changed = 0

    for dirpath, _, files in os.walk(dst_root):
        for fname in files:
            if not is_source(fname):
                continue

            path = os.path.join(dirpath, fname)
            rel = os.path.relpath(path, dst_root).replace(os.sep, "/")

            # 音源ごとの関数を名前で見つけるのは Core の中だけにする。
            # 外まで広げると LfoOpm のような共有部品を巻き込む。
            # 「動詞 + 音源名」の関数はどこにあっても消す。呼び出し側が
            # 別のディレクトリに散っているため。
            in_core = rel.startswith("Core/")
            extra = (lambda code, c=in_core:
                     chip_member_hit(code, camels, lowers, shared,
                                     verbs_only=not c))

            text = read_text(path)
            new = strip_lines(text, pats, include_pat, mode_pat,
                              first_mode, last_mode, extra, symbols)

            if new != text:
                write_text(path, new)
                changed += 1

    print("  言及を落としたファイル: %d 個" % changed)

    # 5. このプラグインだけの名前を入れ替える
    apply_identity(dst_root, name, spec)

    # 6. CMakeLists を書き出して、ルートへ登録する
    write_text(os.path.join(dst_dir, "CMakeLists.txt"),
               CMAKELISTS_TEMPLATE % dict(name=name, code=spec["code"]))
    register_in_root(name)

    print("  生成しました: %s" % dst_dir)


# ---------------------------------------------------------------------------
# プラグインごとに違う名前を入れる
# ---------------------------------------------------------------------------
def apply_identity(dst_root, name, spec):
    ext = spec["ext"]
    path = os.path.join(dst_root, "Gui", "Preset", "PresetValues.h")
    text = read_text(path)

    text = text.replace('"*.2686v.json;*.2686v.yaml;*.xml"',
                        '"*.%s.json;*.%s.yaml;*.xml"' % (ext, ext))
    text = text.replace('".2686v." + Io::fileFormatExtension()',
                        '".%s." + Io::fileFormatExtension()' % ext)
    text = text.replace('"init_preset_vl.xml"', '"init_preset_%s.xml"' % ext)
    text = text.replace('"2686V.library"', '"%s.library"' % name)

    write_text(path, text)


CMAKELISTS_TEMPLATE = """cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# このファイルは tools/gen_plugin.py が書き出しています。手で直さないでください。
# 直したいときは 2686V か生成スクリプトを直して、流し直してください。
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS")
set(CY_COMPANY_NAME "CYROSS")

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_%(name)s
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_%(name)s
    HEADER_NAME "AppIconForAbout.h"
    NAMESPACE AppIconAboutForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# IDE で元の木の形のまま並べる
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "%(name)s" FILES ${ALL_SOURCES})

create_cyross_plugin("%(name)s" "%(code)s")
"""


def register_in_root(name):
    path = os.path.join(ROOT, "CMakeLists.txt")
    text = read_text(path)
    line = "add_subdirectory(%s)\n" % name

    if ("add_subdirectory(%s)" % name) in text:
        return

    anchor = "add_subdirectory(2686VFX)\n"
    assert anchor in text.replace("\r\n", "\n"), "ルートの CMakeLists に目印が見つかりません"

    text = text.replace("\r\n", "\n").replace(anchor, anchor + line, 1)
    write_text(path, text)


def main():
    ap = argparse.ArgumentParser(description="2686V から音源を絞ったプラグインを作る")
    ap.add_argument("names", nargs="*", help="作るプラグイン名 (省略すると一覧を出す)")
    args = ap.parse_args()

    if not args.names:
        print("作れるもの: " + ", ".join(PLUGINS))

        return 0

    for name in args.names:
        if name not in PLUGINS:
            print("知らないプラグイン: %s" % name, file=sys.stderr)

            return 1

        generate(name)

    return 0


if __name__ == "__main__":
    sys.exit(main())
