# -*- coding: utf-8 -*-
"""配布用の波形メモリファイルを作る。

.wt  … 1 行目にサンプル数、2 行目から値 (-1.0〜1.0)
.wt2 … 1 行目にサンプル数、2 行目に解像度、3 行目から値 (0〜解像度-1)

同じ形を両方の形式で出す。.wt2 は段階で持つ形式なので、実機に寄せた粗さの
ものを中心に、解像度を変えて用意する。
"""
import io
import math
import os
import random
import zipfile

OUT = r"C:\Users\takehiro\AppData\Local\Temp\claude\C--Users-takehiro-source-repos-2686V-2686VFX\550d07f6-0323-41aa-8cce-d5eb12a99095\waves"

TAU = math.pi * 2.0


# ---------------------------------------------------------------- 波の作り方
def norm(v):
    """いちばん大きいところが 1.0 になるようにそろえる。"""
    peak = max(abs(x) for x in v) or 1.0

    return [x / peak for x in v]


def sine(n, harmonics):
    """倍音の組み合わせ。(次数, 大きさ, 位相) の並びで指定する。"""
    out = []

    for i in range(n):
        t = i / n
        s = 0.0

        for h, amp, ph in harmonics:
            s += amp * math.sin(TAU * (h * t + ph))

        out.append(s)

    return norm(out)


def pulse(n, duty):
    return [1.0 if (i / n) < duty else -1.0 for i in range(n)]


def saw(n, down=False):
    out = [2.0 * (i / n) - 1.0 for i in range(n)]

    return [-x for x in out] if down else out


def triangle(n, peak=0.5):
    out = []

    for i in range(n):
        t = i / n
        out.append(t / peak if t < peak else (1.0 - t) / (1.0 - peak))

    return norm([x * 2.0 - 1.0 for x in out])


def fold(v, gain):
    """振り切れたぶんを折り返す。倍音が派手に増える。"""
    out = []

    for x in v:
        y = x * gain

        while y > 1.0 or y < -1.0:
            if y > 1.0:
                y = 2.0 - y
            if y < -1.0:
                y = -2.0 - y

        out.append(y)

    return out


def clip(v, gain):
    return [max(-1.0, min(1.0, x * gain)) for x in v]


def stepped(v, steps):
    """わざと段を粗くする。実機の粗さに寄せるとき。"""
    return [round(x * steps) / steps for x in v]


def formant(n, peaks):
    """帯ごとに倍音を盛る。声のような響きになる。"""
    hs = []

    for centre, width, amp in peaks:
        for h in range(1, 33):
            g = amp * math.exp(-((h - centre) ** 2) / (2.0 * width * width))

            if g > 0.001:
                hs.append((h, g, 0.0))

    return sine(n, hs)


def metallic(n, partials, seed):
    """整数倍でない倍音。鐘や金属の響きになる。"""
    rnd = random.Random(seed)
    hs = []

    for k in range(partials):
        h = 1.0 + k * (1.0 + rnd.uniform(0.15, 0.75))
        hs.append((h, 1.0 / (k + 1), rnd.random()))

    return sine(n, hs)


def periodic_noise(n, seed, smooth):
    """周期のあるざらつき。同じ形が繰り返されるのでノイズにはならない。"""
    rnd = random.Random(seed)
    out = [rnd.uniform(-1.0, 1.0) for _ in range(n)]

    for _ in range(smooth):
        out = [(out[i - 1] + out[i] + out[(i + 1) % n]) / 3.0 for i in range(n)]

    return norm(out)


# ---------------------------------------------------------------- 品ぞろえ
def catalogue():
    """(分類, 名前, サンプル数, 値) を並べる。"""
    items = []

    def add(group, name, n, values):
        items.append((group, name, n, values))

    # --- 基本の形 ---
    add("basic", "sine", 32, sine(32, [(1, 1.0, 0.0)]))
    add("basic", "saw-up", 32, saw(32))
    add("basic", "saw-down", 32, saw(32, down=True))
    add("basic", "square", 32, pulse(32, 0.5))
    add("basic", "triangle", 32, triangle(32, 0.5))

    # --- パルス幅 ---
    for duty in [0.0625, 0.125, 0.1875, 0.25, 0.3125, 0.375, 0.4375]:
        add("pulse", "pulse-%02d" % round(duty * 100), 32, pulse(32, duty))

    # --- のこぎりの傾き ---
    for peak in [0.1, 0.2, 0.3, 0.7, 0.8, 0.9]:
        add("ramp", "ramp-%02d" % round(peak * 100), 32, triangle(32, peak))

    # --- 倍音を積む ---
    add("additive", "odd-3", 64, sine(64, [(h, 1.0 / h, 0.0) for h in (1, 3, 5)]))
    add("additive", "odd-5", 64, sine(64, [(h, 1.0 / h, 0.0) for h in (1, 3, 5, 7, 9)]))
    add("additive", "odd-8", 64, sine(64, [(h, 1.0 / h, 0.0) for h in range(1, 17, 2)]))
    add("additive", "even-4", 64, sine(64, [(h, 1.0 / h, 0.0) for h in (1, 2, 4, 6)]))
    add("additive", "all-8", 64, sine(64, [(h, 1.0 / h, 0.0) for h in range(1, 9)]))
    add("additive", "all-16", 128, sine(128, [(h, 1.0 / h, 0.0) for h in range(1, 17)]))
    add("additive", "bright-8", 64, sine(64, [(h, 1.0 / math.sqrt(h), 0.0) for h in range(1, 9)]))
    add("additive", "hollow", 64, sine(64, [(1, 1.0, 0.0), (3, 0.7, 0.0), (9, 0.3, 0.0)]))
    add("additive", "nasal", 64, sine(64, [(2, 1.0, 0.0), (3, 0.8, 0.0), (5, 0.6, 0.0)]))
    add("additive", "organ-2", 64, sine(64, [(1, 1.0, 0.0), (2, 0.8, 0.0)]))
    add("additive", "organ-3", 64, sine(64, [(1, 1.0, 0.0), (2, 0.6, 0.0), (4, 0.4, 0.0)]))
    add("additive", "organ-full", 128, sine(128, [(h, 0.9 ** k, 0.0) for k, h in enumerate((1, 2, 3, 4, 6, 8))]))

    # --- 位相をずらす ---
    for k, ph in enumerate([0.125, 0.25, 0.375]):
        add("phase", "phase-%d" % (k + 1), 64,
            sine(64, [(1, 1.0, 0.0), (2, 0.6, ph), (3, 0.4, ph * 2)]))

    # --- 声のような響き ---
    add("formant", "vowel-a", 128, formant(128, [(2, 1.2, 1.0), (6, 1.6, 0.7), (11, 2.0, 0.35)]))
    add("formant", "vowel-i", 128, formant(128, [(1, 1.0, 1.0), (9, 2.0, 0.55), (16, 2.4, 0.4)]))
    add("formant", "vowel-u", 128, formant(128, [(1, 1.0, 1.0), (3, 1.4, 0.6), (7, 1.6, 0.2)]))
    add("formant", "vowel-e", 128, formant(128, [(2, 1.2, 1.0), (7, 1.8, 0.65), (13, 2.2, 0.3)]))
    add("formant", "vowel-o", 128, formant(128, [(1, 1.0, 1.0), (4, 1.5, 0.7), (8, 1.8, 0.25)]))

    # --- 鐘・金属 ---
    for k in range(4):
        add("metallic", "bell-%d" % (k + 1), 128, metallic(128, 6 + k * 2, 1000 + k))

    # --- 歪ませる ---
    base = sine(64, [(1, 1.0, 0.0)])
    for k, g in enumerate([1.6, 2.4, 3.6]):
        add("fold", "fold-%d" % (k + 1), 64, fold(base, g))

    for k, g in enumerate([2.0, 4.0, 8.0]):
        add("clip", "clip-%d" % (k + 1), 64, clip(base, g))

    # --- 実機に寄せた粗さ ---
    for steps in [3, 7, 15]:
        add("lofi", "lofi-sine-%d" % steps, 32, stepped(sine(32, [(1, 1.0, 0.0)]), steps))
        add("lofi", "lofi-saw-%d" % steps, 32, stepped(saw(32), steps))

    # --- 周期のあるざらつき ---
    for k in range(4):
        add("noise", "grain-%d" % (k + 1), 64, periodic_noise(64, 2000 + k, 2 - (k % 3)))

    # --- 組み合わせ ---
    sq = pulse(64, 0.5)
    sw = saw(64)
    tr = triangle(64, 0.5)
    si = sine(64, [(1, 1.0, 0.0)])

    add("mix", "saw-square", 64, norm([(a + b) / 2 for a, b in zip(sw, sq)]))
    add("mix", "sine-square", 64, norm([(a + b) / 2 for a, b in zip(si, sq)]))
    add("mix", "sine-saw", 64, norm([(a + b) / 2 for a, b in zip(si, sw)]))
    add("mix", "tri-square", 64, norm([(a + b) / 2 for a, b in zip(tr, sq)]))
    add("mix", "half-saw", 64, norm([a if i < 32 else 0.0 for i, a in enumerate(sw)]))
    add("mix", "half-sine", 64, norm([a if a > 0 else 0.0 for a in si]))
    add("mix", "abs-sine", 64, norm([abs(a) * 2 - 1 for a in si]))
    add("mix", "quarter-sine", 64, norm([a if (i % 32) < 16 else 0.0 for i, a in enumerate(si)]))

    # --- 段のある形 ---
    for k, n in enumerate([4, 8, 16]):
        add("stair", "stair-%d" % n, 32, [(2.0 * (i * n // 32) / (n - 1)) - 1.0 for i in range(32)])

    # --- 細かい形 ---
    add("detail", "sine-256", 256, sine(256, [(1, 1.0, 0.0)]))
    add("detail", "additive-256", 256, sine(256, [(h, 1.0 / h, 0.0) for h in range(1, 25)]))
    add("detail", "formant-256", 256, formant(256, [(2, 1.2, 1.0), (8, 2.0, 0.6), (15, 2.4, 0.3)]))
    add("detail", "bell-256", 256, metallic(256, 12, 7777))

    return items


# ---------------------------------------------------------------- 書き出し
def write_wt(path, values):
    lines = [str(len(values))]

    for v in values:
        lines.append("%.6f" % max(-1.0, min(1.0, v)))

    io.open(path, "w", encoding="utf-8", newline="\n").write("\n".join(lines) + "\n")


def write_wt2(path, values, resolution):
    centre = resolution >> 1
    lines = [str(len(values)), str(resolution)]

    for v in values:
        # -1.0〜1.0 を 0〜解像度-1 へ移す
        q = int(round((v + 1.0) * 0.5 * (resolution - 1)))
        lines.append(str(max(0, min(resolution - 1, q))))

    io.open(path, "w", encoding="utf-8", newline="\n").write("\n".join(lines) + "\n")


def main():
    items = catalogue()

    wt_dir = os.path.join(OUT, "wt")
    wt2_dir = os.path.join(OUT, "wt2")

    for d in (wt_dir, wt2_dir):
        os.makedirs(d, exist_ok=True)

    # .wt はそのまま
    for group, name, n, values in items:
        write_wt(os.path.join(wt_dir, "%s_%s.wt" % (group, name)), values)

    # .wt2 は解像度を変えて出す。段の粗さそのものが持ち味なので、
    # 同じ形でも解像度違いに意味がある。
    count2 = 0

    for group, name, n, values in items:
        res = 16 if group in ("basic", "pulse", "lofi", "stair") else 32

        write_wt2(os.path.join(wt2_dir, "%s_%s_r%d.wt2" % (group, name, res)), values, res)
        count2 += 1

    # 粗さの違いを見せるため、基本の形は解像度違いも足す
    for group, name, n, values in items:
        if group not in ("basic", "additive", "formant", "metallic"):
            continue

        for res in (64,):
            write_wt2(os.path.join(wt2_dir, "%s_%s_r%d.wt2" % (group, name, res)), values, res)
            count2 += 1

    print("wt  : %d 個" % len(items))
    print("wt2 : %d 個" % count2)

    return wt_dir, wt2_dir, len(items), count2


if __name__ == "__main__":
    main()
