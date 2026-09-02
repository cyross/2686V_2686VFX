---
title: 共通の区分
description: どの音源タブにも同じ形で出てくる区分の説明
sidebar:
  order: 0
---

音源タブには、その音源だけの区分（アルゴリズム、波形の選択など）と、
**どの音源にも同じ形で出てくる区分**があります。ここでは後者をまとめて
説明します。

各音源のページの「共通の区分」からここへ来られます。持っている区分は音源に
よって違うので、そのページの表に出ているものだけを見てください。

:::note[表記について]
| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |
:::

:::tip[オートメーションの列について]
共通のつまみは、どの音源でも**同じ後ろ名前**で並びます。表のリンクは代表として
**SSG** のものを指しています。ほかの音源では、頭が `OPNA_` `OPM_` のように
変わるだけで、範囲も初期値も同じです。オペレータごとに置かれている場合は、
`OPNA_OP[0-3]_` のようにオペレータの番号が挟まります。
:::

## LEVEL

チャンネル全体の音量です。

UTILITY の **Level -> All Ch** を押すと、この値を他のすべてのチャンネルへ
一度に反映できます。音源を切り替えながら作るときに、音量差が出ないように
そろえる用途を想定しています。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **LV** | チャンネル全体の音量 | 0 〜 10 | 1 | [`SSG_LEVEL`](/2686V_2686VFX/reference/automation/ssg/#ssg-level) |

## QUALITY

**独自**の区分です。ビット数とサンプリング周波数をわざと落として、当時の
機材のざらつきを作ります。数字が小さいほど粗くなります。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **BIT** | ビット数。12 段階。4-bit（16 段）から Raw（落とさない）まで | 1 〜 12 | 9 | [`SSG_BIT`](/2686V_2686VFX/reference/automation/ssg/#ssg-bit) |
| **RATE** | サンプリング周波数。15 段階。96kHz から 2kHz まで | 1 〜 15 | 2 | [`SSG_RATE`](/2686V_2686VFX/reference/automation/ssg/#ssg-rate) |

80 年代の音に寄せたいときは BIT を下げ、RATE も一緒に下げると当時の雰囲気に
近づきます。

選べるものの一覧は
[QUALITY 一覧](/2686V_2686VFX/reference/lists-quality/) にあります。

## QUALITY(PCM)

音声ファイルを鳴らすチャンネル（**RHYTHM / ADPCM** と、OPZX7 の PCM）では、
QUALITY の代わりにこちらが出ます。単にビット数を落とすだけでなく、**実機の
記録方式そのもの**を選べます。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **BIT** | ビット数と圧縮方式。21 種 | 1 〜 21 | 13 | [`ADPCM_MODE`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-mode) |
| **RATE** | サンプリング周波数。15 段階 | 1 〜 15 | 9 | [`ADPCM_RATE`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-rate) |
| **INTP** | 読み戻すときの補間のしかた。7 種 | 0 〜 6 | 1 | [`ADPCM_INTERP`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-interp) |

### BIT で選べるもの

| 種類 | 内容 |
| --- | --- |
| 32bit 〜 4bit PCM | 単純にビット数を落とす |
| **4bit ADPCM** | YM2608 の ADPCM（**実機**） |
| **1bit DPCM** | ファミコンの DPCM |
| SNES BRR / PS1 VAG / IMA ADPCM / CD-ROM XA / YMZ280B / K053260 / K054539 | 各機の圧縮方式（**独自**） |

**圧縮方式は、単にビット数を落とすのとは歪み方が違います。** 実機の質感を
狙うなら、その機種が使っていた方式を選びます。

### INTP — 補間のしかた

サンプリング周波数を落としたあと、読み戻すときの埋め方です。

| 値 | 方式 | 傾向 |
| --- | --- | --- |
| 0 | Nearest | 補間なし。エイリアスノイズが出る |
| 1 | Linear | 標準 |
| 2 | Gaussian | SFC 風の丸み |
| 3 | Zero-Order Hold | 最も粗い |
| 4 | Cosine | Linear と Gaussian の中間 |
| 5 | B-Spline | 強いローパス。こもった感じ |
| 6 | Lagrange | Gaussian とは違う倍音 |

**RATE を下げて Nearest にすると、いちばん当時らしい粗さ**になります。逆に
B-Spline はこもるので、遠くで鳴っている感じを作れます。

## ENVELOPE

時間とともに音量や音程を動かす部分です。複数の種類が同時に使えます。

| 名前 | 対象 | 段数 | 表記 |
| --- | --- | --- | --- |
| [AMP ENV](#amp-env) | 音量 | ADSR | 実機・独自 |
| [SSG HW AMP ENV](#ssg-hw-amp-env) | 音量 | 波形 44 種 | 実機・独自 |
| [SSG SW AMP ENV](#ssg-sw-amp-env) | 音量 | 6 タップ | 独自 |
| [SSG SW AMP ENV\[11\]](#ssg-sw-amp-env11) | 音量 | 11 タップ | 独自 |
| [PITCH ENV](#pitch-env) | 音程 | 3 タップ | 独自 |
| [SSG SW PITCH ENV\[11\]](#ssg-sw-pitch-env11) | 音程 | 11 タップ | 独自 |

音量側は掛け算で重なります。AMP ENV で全体の形を作り、SSG HW AMP ENV や
SSG SW AMP ENV でその上に細かい動きを乗せる、という組み立てになります。

### AMP ENV

音量の基本の形です。**開始レベル → アタック → ディケイ → サステイン →
リリース**の順に動きます。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Bypass** | **独自**。このエンベロープを掛けない | False / True | False | [`SSG_ADSR_BYPASS`](/2686V_2686VFX/reference/automation/ssg/#ssg-adsr-bypass) |
| **STL** | 鍵を押した瞬間の音量 | 0 〜 1 | 0 | [`SSG_STL`](/2686V_2686VFX/reference/automation/ssg/#ssg-stl) |
| **AR** | 最大まで上がる時間（秒） | 0.001 〜 10 | 0.001 | [`SSG_AR`](/2686V_2686VFX/reference/automation/ssg/#ssg-ar) |
| **DR** | サステインまで下がる時間（秒） | 0.001 〜 10 | 0.001 | [`SSG_DR`](/2686V_2686VFX/reference/automation/ssg/#ssg-dr) |
| **SL** | 押しているあいだ保つ音量 | 0 〜 1 | 1 | [`SSG_SL`](/2686V_2686VFX/reference/automation/ssg/#ssg-sl) |
| **RR** | 鍵を離してから消えるまでの時間（秒） | 0.001 〜 10 | 0.001 | [`SSG_RR`](/2686V_2686VFX/reference/automation/ssg/#ssg-rr) |
| **KOR** | **独自**。鍵を離しても最後まで鳴らし切る | False / True | False | [`SSG_KOR`](/2686V_2686VFX/reference/automation/ssg/#ssg-kor) |

FM 音源のオペレータでは、これとは別にオペレータごとのレジスタ式の
エンベロープ（AR / DR / SR / SL / RR / TL）を持ちます。そちらは各音源の
ページを参照してください。

### SSG HW AMP ENV

SSG のハードウェアエンベロープです。**決まった形の波を繰り返す**やり方で、
実機では音量を階段状に動かすのに使われていました。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Enable** | 使う・使わない | False / True | False | [`SSG_SSGHWENV_ENABLE`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssghwenv-enable) |
| **SHPE** | 波の形。**0〜15 が実機、16 以降が独自** | 0 〜 43 | 0 | [`SSG_SSGHWENV_SHAPE`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssghwenv-shape) |
| **PERD** | 繰り返しの速さ | 0.1 〜 200 | 1 | [`SSG_SSGHWENV_PERIOD`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssghwenv-period) |
| **MIN** | **独自**。動く範囲の下 | 0 〜 1 | 0 | [`SSG_SSGHWENV_MIN`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssghwenv-min) |
| **MAX** | **独自**。動く範囲の上 | 0 〜 1 | 1 | [`SSG_SSGHWENV_MAX`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssghwenv-max) |
| **Smooth** | **独自**。段の角を丸める | False / True | False | [`SSG_SSGHWENV_SMOOTH`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssghwenv-smooth) |

実機は形が 16 種類・範囲は固定でしたが、ここでは段の数を変えたもの、
サンプル&ホールド、正弦、ランダムなどを足してあります。

形の一覧は [波形一覧](/2686V_2686VFX/reference/lists-waveform/) にあります。

### SSG SW AMP ENV

**独自**の区分です。ドライバがソフトウェアで作っていた音量変化を、
**折れ線**として直接描けるようにしたものです。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | 使う・使わない。チャンネルでは「バイパス」、オペレータでは「有効」の意味になる | False / True | True | [`SSG_SSGSW_BYPASS`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-bypass) |
| **STEP** | 使う段の数 | 1 〜 5 | 5 | [`SSG_SSGSW_STEPS`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-steps) |
| **STL** | 始まりの高さ | 0 〜 1 | 0 | [`SSG_SSGSW_STL`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-stl) |
| **R1** | 1 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L1** | 1 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R2** | 2 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L2** | 2 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R3** | 3 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L3** | 3 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R4** | 4 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L4** | 4 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R5** | 5 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L5** | 5 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R6** | 6 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L6** | 6 段目の音量 | 0 〜 1 | 0 | [`SSG_SSGSW_L6`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-l6) |
| **LOOP** | 繰り返す | False / True | False | [`SSG_SSGSW_LOOP`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-loop) |
| **LOOP TO** | 戻り先の段。0〜3 | 0 〜 3 | 0 | [`SSG_SSGSW_LOOPTO`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-loopto) |
| **LOOP COUNT** | 繰り返す回数。**0 で無限** | 0 〜 200 | 0 | [`SSG_SSGSW_LOOPCNT`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw-loopcnt) |

### SSG SW AMP ENV[11]

上と同じ作りで、段が **11 タップ**に増えたものです。こまかい動きを作りたい
ときは、6 タップではなくこちらを使います。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | 使う・使わない。チャンネルでは「バイパス」、オペレータでは「有効」の意味になる | False / True | True | [`SSG_SSGSW11_BYPASS`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-bypass) |
| **STEP** | 使う段の数 | 1 〜 10 | 10 | [`SSG_SSGSW11_STEPS`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-steps) |
| **STL** | 始まりの高さ | 0 〜 1 | 0 | [`SSG_SSGSW11_STL`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-stl) |
| **R1** | 1 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L1** | 1 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R2** | 2 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L2** | 2 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R3** | 3 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L3** | 3 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R4** | 4 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L4** | 4 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R5** | 5 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L5** | 5 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R6** | 6 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L6** | 6 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R7** | 7 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L7** | 7 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R8** | 8 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L8** | 8 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R9** | 9 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L9** | 9 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R10** | 10 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L10** | 10 段目の音量 | 0 〜 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R11** | 11 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L11** | 11 段目の音量 | 0 〜 1 | 0 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **LOOP** | 繰り返す | False / True | False | [`SSG_SSGSW11_LOOP`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-loop) |
| **LOOP TO** | 戻り先の段。0〜8 | 0 〜 8 | 0 | [`SSG_SSGSW11_LOOPTO`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-loopto) |
| **LOOP COUNT** | 繰り返す回数。**0 で無限** | 0 〜 200 | 0 | [`SSG_SSGSW11_LOOPCNT`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgsw11-loopcnt) |

### PITCH ENV

音程を時間で動かします。値は**セント**で、**−4800〜+4800**（上下 4 オクターブ）
まで指定できます。100 で半音、1200 で 1 オクターブです。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Bypass** | 使う・使わない | False / True | False | [`SSG_PITCHENV_BYPASS`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitchenv-bypass) |
| **STL** | 鍵を押した瞬間の音程 | -4800 〜 4800 | 0 | [`SSG_PITCH_STL`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-stl) |
| **AR** | ATL へ移る時間（秒） | 0.001 〜 10 | 0.001 | [`SSG_PITCH_AR`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-ar) |
| **ATL** | 頂点の音程 | -4800 〜 4800 | 0 | [`SSG_PITCH_ATL`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-atl) |
| **DR** | SSL へ移る時間（秒） | 0.001 〜 10 | 0.001 | [`SSG_PITCH_DR`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-dr) |
| **SSL** | 押しているあいだ保つ音程 | -4800 〜 4800 | 0 | [`SSG_PITCH_SSL`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-ssl) |
| **RR** | RLL へ移る時間（秒） | 0.001 〜 10 | 0.001 | [`SSG_PITCH_RR`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-rr) |
| **RLL** | 終わりの音程 | -4800 〜 4800 | 0 | [`SSG_PITCH_RLL`](/2686V_2686VFX/reference/automation/ssg/#ssg-pitch-rll) |

打鍵の頭だけ音程を上げる、レーザーのように急降下させる、といった使い方を
します。

### SSG SW PITCH ENV[11]

音程側の **11 タップ**版です。作りは
[SSG SW AMP ENV\[11\]](#ssg-sw-amp-env11) と同じで、L1〜L11 が音量ではなく
**セント**（±4800）になります。

段ごとに音程を置けるので、**アルペジオ**やサイレンのような動きを 1 音の中で
作れます。LOOP を入れれば、鳴っているあいだ繰り返します。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | 使う・使わない。チャンネルでは「バイパス」、オペレータでは「有効」の意味になる | False / True | True | [`SSG_SSGSWP11_BYPASS`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-bypass) |
| **STEP** | 使う段の数 | 1 〜 10 | 10 | [`SSG_SSGSWP11_STEPS`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-steps) |
| **STL** | 始まりの高さ | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_STL`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-stl) |
| **R1** | 1 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L1** | 1 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R2** | 2 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L2** | 2 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R3** | 3 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L3** | 3 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R4** | 4 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L4** | 4 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R5** | 5 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L5** | 5 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R6** | 6 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L6** | 6 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R7** | 7 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L7** | 7 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R8** | 8 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L8** | 8 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R9** | 9 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L9** | 9 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R10** | 10 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L10** | 10 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R11** | 11 段目へ移るまでの時間 | 0.001 〜 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L11** | 11 段目の音程（セント） | -4800 〜 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **LOOP** | 繰り返す | False / True | False | [`SSG_SSGSWP11_LOOP`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-loop) |
| **LOOP TO** | 戻り先の段。0〜8 | 0 〜 8 | 0 | [`SSG_SSGSWP11_LOOPTO`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-loopto) |
| **LOOP COUNT** | 繰り返す回数。**0 で無限** | 0 〜 200 | 0 | [`SSG_SSGSWP11_LOOPCNT`](/2686V_2686VFX/reference/automation/ssg/#ssg-ssgswp11-loopcnt) |

## WT PITCH MOD

**独自**の区分です。波形メモリの形をそのまま使って音程を揺らします。LFO が
決まった形で揺らすのに対し、こちらは**波形の形がそのまま揺れ方になります。**

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Enable** | 使う・使わない | False / True | False | [`SSG_MOD_ENABLE`](/2686V_2686VFX/reference/automation/ssg/#ssg-mod-enable) |
| **DPTH** | 揺れの深さ | 0.1 〜 1 | 0.2 | [`SSG_MOD_DEPTH`](/2686V_2686VFX/reference/automation/ssg/#ssg-mod-depth) |
| **SPED** | 揺れの速さ | 0.1 〜 10 | 1 | [`SSG_MOD_SPEED`](/2686V_2686VFX/reference/automation/ssg/#ssg-mod-speed) |
| **SHPE** | 揺らす形。9 種 | 0 〜 8 | 0 | [`SSG_MOD_SHAPE`](/2686V_2686VFX/reference/automation/ssg/#ssg-mod-shape) |
| **Smooth** | 段の角を丸める | False / True | True | [`SSG_MOD_WAVE_SMOOTH`](/2686V_2686VFX/reference/automation/ssg/#ssg-mod-wave-smooth) |

形は **Sine / FDS Triangle / FDS Saw / FDS Reset / FDS Pulse / WS Sweep Up /
WS Sweep Down / HuC6280 Wave / FDS Table** の 9 種類です。ファミコンの
ディスクシステム、ワンダースワン、PC エンジンの音源が持っていた変調の
かかり方を持ってきたものです。

`.wt` `.wt2` ファイルを読み込んで、自分で描いた形で揺らすこともできます。

## LFO

音量（AM）と音程（PM）を周期的に揺らします。**AM と PM は別々の設定を
持ちます。** 片方だけ使うこともできます。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **PM Enable** | 音程側の揺れを掛ける | False / True | False | [`SSG_LFO_PM`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-pm) |
| **P.SP** | 音程側の揺れの速さ（Hz） | 0.1 〜 50 | 5 | [`SSG_LFO_PM_FREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-pm-freq) |
| **P.SH** | 音程側の形。13 種 | 0 〜 12 | 0 | [`SSG_LFO_PG_SHAPE`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-pg-shape) |
| **SDLY** | 音程側。押してから揺れ始めるまでの待ち | 0 〜 255 | 0 | [`SSG_LFO_PM_SYNC_DELAY`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-pm-sync-delay) |
| **PMS** | 音程の効き | 0 〜 1 | 0 | [`SSG_LFO_PMS`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-pms) |
| **PMD** | 音程の深さ | 0 〜 1 | 0 | [`SSG_LFO_PMD`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-pmd) |
| **AM Enable** | 音量側の揺れを掛ける | False / True | False | [`SSG_LFO_AM`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-am) |
| **A.SP** | 音量側の揺れの速さ（Hz） | 0.1 〜 50 | 5 | [`SSG_LFO_AM_FREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-am-freq) |
| **A.SH** | 音量側の形。13 種 | 0 〜 12 | 0 | [`SSG_LFO_EG_SHAPE`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-eg-shape) |
| **SDLY** | 音量側。押してから揺れ始めるまでの待ち | 0 〜 255 | 0 | [`SSG_LFO_AM_SYNC_DELAY`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-am-sync-delay) |
| **AMS** | 音量の効き | 0 〜 1 | 0 | [`SSG_LFO_AMS`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-ams) |
| **AMD** | 音量の深さ | 0 〜 1 | 0 | [`SSG_LFO_AMD`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-amd) |
| **AMSR** | 音量側の角の丸め | 0.005 〜 0.5 | 0.005 | [`SSG_LFO_ASMRT`](/2686V_2686VFX/reference/automation/ssg/#ssg-lfo-asmrt) |

形は Sine / Saw Up / Saw Down / Square / Triangle / Sample & Hold のほか、
**1 回だけ動いて止まる**もの（Saw Down & One Shot / Triangle & One Shot）と、
段の粗さを変えた Sample & Hold が 4 種類あります。

SDLY を入れると、押した直後は揺れず、少し経ってから揺れ始めます。歌の
ビブラートのような掛かり方になります。

FM 音源には、これとは別に**実機の LFO**（OPNA のハードウェア LFO、
N88-BASIC のソフトウェア LFO、OPM の LFO、OPL のオペレータごとの LFO）が
あります。そちらは各音源のページを参照してください。

## MUL/DET

音程をずらす部分です。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **MUL** | 逓倍。x0.5 から x15 まで | 0 〜 21 | 2 | [`SSG_MUL`](/2686V_2686VFX/reference/automation/ssg/#ssg-mul) |
| **MURT** | **独自**。倍率を自由に決める | 0.01 〜 100 | 1 | [`SSG_MUL_RATIO`](/2686V_2686VFX/reference/automation/ssg/#ssg-mul-ratio) |
| **DT1** | デチューン（実機の DT） | 0 〜 15 | 0 | [`SSG_DT`](/2686V_2686VFX/reference/automation/ssg/#ssg-dt) |
| **DT2** | 粗いデチューン（実機の DT2） | 0 〜 3 | 0 | [`SSG_DT3`](/2686V_2686VFX/reference/automation/ssg/#ssg-dt3) |
| **DT3** | **独自**。セントで自由に指定する | -4800 〜 4800 | 0 | [`SSG_DT3`](/2686V_2686VFX/reference/automation/ssg/#ssg-dt3) |

実機の逓倍は整数倍が中心でしたが、**MURT** で整数から外れた倍率を指定
できます。鐘や金属のような、倍音が整数比から外れた音を作れます。

## FIX

**独自**の区分です。鍵盤の音程を無視して、**決まった周波数**で鳴らします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Enable** | 使う・使わない | False / True | False | [`SSG_FIX`](/2686V_2686VFX/reference/automation/ssg/#ssg-fix) |
| **FQ** | 鳴らす周波数（Hz） | 0 〜 8000 | 440 | [`SSG_FREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-freq) |

打楽器のように、どの鍵を押しても同じ高さで鳴ってほしいときに使います。
FM 音源ではオペレータごとに指定できるので、片方だけ固定して金属的な響きを
作る、といった使い方もできます。

## UNISON/HARMONY

**独自**の区分です。同じ音を重ねて厚くします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **VOIC** | 重ねる数 | 1 〜 8 | 1 | [`SSG_UNI_VOICES`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-voices) |
| **DT** | 全体のずらし幅（セント） | -4800 〜 4800 | 0 | [`SSG_UNI_DETUNE`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-detune) |
| **SPR** | 左右への広がり | 0 〜 1 | 0.5 | [`SSG_UNI_SPREAD`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-spread) |
| **Arpeggio** | 重ねずに順番に鳴らす | False / True | False | [`SSG_UNI_ARP`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-arp) |
| **ARFQ** | アルペジオの速さ | 1 〜 4000 | 60 | [`SSG_UNI_ARPFREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-arpfreq) |
| **Arp Smooth** | 切り替わりの角を丸める | False / True | True | [`SSG_UNI_ARPSMTH`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-arpsmth) |
| **P-SP** | 声ごとの距離（7 声ぶん） | 0 〜 1 | 0 | [`SSG_UNI_PDIST[1-7]`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-pdist-1-7) |
| **P-DT** | 声ごとのずらし幅（7 声ぶん、セント） | -4800 〜 4800 | 0 | [`SSG_UNI_PDET[1-7]`](/2686V_2686VFX/reference/automation/ssg/#ssg-uni-pdet-1-7) |

2 声目以降は、**声ごとに距離とずらし幅を別々に決められます**（7 声ぶん）。
均等に散らすだけでなく、片側に寄せる、オクターブで積む、といった組み方が
できます。

**Arpeggio** を入れると、重ねる代わりに 1 音ずつ順番に鳴らします。同時発音数
の少ない実機で和音に聞かせていた、あの鳴り方になります。声ごとのずらし幅を
半音単位で置けば、そのまま分散和音として使えます。

## OPTIONAL

音源が持っていた追加の動作です。中身は音源によって違います。効果音モード
（オペレータごとに別の音程を持たせる）、ノイズの扱い、キーオフの挙動などが
ここに入ります。

何が出るかは各音源のページを参照してください。

## UTILITY

パラメータの読み書きです。`[IM]` が読み込み、`[EX]` が書き出しです。

区分ごとに別々のファイルとして出し入れできます。AMP ENV だけを他のチャンネル
へ持っていく、LFO の設定だけを使い回す、といったことができます。チャンネル
全体をまとめて 1 ファイルにすることもできます。

**Level -> All Ch** は、このチャンネルの音量を他のすべてのチャンネルへ一度に
反映します。

ファイルの形式と置き場所は
[ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
パラメータファイルは**音源をまたいで読めます**。OPN で作った AMP ENV を SSG で
読む、といった使い方ができます。

## オートメーション

ここに出てくるつまみは、ほぼすべて DAW のオートメーションから動かせます。
名前・型・範囲・初期値は
[オートメーション一覧](/2686V_2686VFX/reference/automation/) にあります。

カーブ編集（ADV）だけは枠の数が多すぎるため、オートメーションには出しません。
[カーブ編集](/2686V_2686VFX/chips/adv/) を参照してください。

