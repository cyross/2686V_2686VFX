---
title: SSG
description: 矩形波・三角波とノイズを鳴らす SSG チャンネルの使い方
sidebar:
  order: 7
---

SSG は、YM-2203/YM-2608B の SSG 部分(ルーツは、 AY-3-8910 系のいわゆる PSG )にあたるチャンネルです。矩形波とノイズを鳴らし、ハードウェアエンベロープで音量を動かします。実機に無い拡張として、三角波・鋸波と、より細かい音作りのための項目を足してあります。

## 印について

このマニュアルでは、画面の区分ごとに次の印を付けています。**画面上の見出しの
色と対応しています。**

| 印 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |
| **共通** | 音源に依らない、プラグイン全体の機能 |

実機の音を狙うときは、**独自**の項目を初期値のままにしておくと近づきます。

## FORM — 波形の選択（実機）

鳴らす波形の種類を選びます。

| 値 | 波形 |
| --- | --- |
| `0: Pulse(Rect)` | 矩形波。実機と同じ |
| `1: Triangle / Saw` | 三角波・鋸波（**独自**） |

選んだ波形によって、下に出る設定が切り替わります。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **WAVEFORM** | 矩形波か三角波か | [`SSG_WAVEFORM`](/2686V_2686VFX/reference/automation/ssg/#ssg-waveform) |

### 矩形波（デューティ比）設定

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **MODE** | デューティ比の決め方。`0: Preset Ratio` は実機にある 8 種類から選ぶ、`1: Variable (Slider)` は連続で変える（**独自**） | — | |
| **PRST** | 実機にある 8 種類のデューティ比 | 下表 | |
| **RAT** | MODE が Variable のときの比率 | 0.0〜1.0 | |
| **FC Mode** | ファミコン風の挙動にする（**独自**） | 入 / 切（初期は切） | |
| **FCFL** | FC Mode のときの揺らぎの量（**独自**） | 0.0〜1.0（初期 0.2） | |

PRST で選べるデューティ比は次の 8 種類です。

| 値 | 比 | デューティ比 |
| --- | --- | --- |
| 0 | 1:1 | 50% |
| 1 | 3:5 | 37.5% |
| 2 | 5:11 | 31.25% |
| 3 | 1:3 | 25% |
| 4 | 1:4 | 20% |
| 5 | 3:13 | 18.75% |
| 6 | 1:7 | 12.5% |
| 7 | 1:15 | 6.25% |

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **DUTY MODE** | 決まった比から選ぶか、自由に決めるか | [`SSG_DUTY_MODE`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-mode) |
| **DUTY PRESET** | 実機で使われていた比。9 種 | [`SSG_DUTY_PRESET`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-preset) |
| **DUTY VAR** | 自由に決めるときの比 | [`SSG_DUTY_VAR`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-var) |
| **INVERT** | 上下を入れ替える | [`SSG_DUTY_INV`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-inv) |
| **FC MODE** | ファミコンのように、比が細かく揺れる動きにする（**独自**） | [`SSG_DUTY_FC_MODE`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-fc-mode) |
| **FC FLUC** | その揺れの大きさ | [`SSG_DUTY_FC_FLUC`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-fc-fluc) |

### 三角波設定（独自）

FORM で `1: Triangle / Saw` を選んだときに出ます。**PEAK** で波形の頂点の位置を
動かし、三角波から鋸波まで連続で変えられます。

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **PEAK** | 頂点の位置。`0.0` で下降鋸波、`0.5` で三角波、`1.0` で上昇鋸波 | 0.0〜1.0（初期 0.5） | |
| **Key Track (Pitch)** | 音程に追従させる。切ると FQ の周波数で固定される | 入 / 切（初期は入） | |
| **FQ** | Key Track を切ったときの周波数 | 0〜8000 Hz（初期 440） | |

PEAK の下には `0.0 (Down)` `0.5 (Tri)` `1.0 (Up)` のボタンがあり、押すと
その値へ一度に移ります。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KEY TRACK** | 鍵盤の音程で鳴らす。切ると TRI FREQ の高さで固定される | [`SSG_TRI_KEYTRK`](/2686V_2686VFX/reference/automation/ssg/#ssg-tri-keytrk) |
| **TRI FREQ** | 固定するときの周波数 | [`SSG_TRI_FREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-tri-freq) |
| **TRI PEAK** | 三角形の頂点の位置。0.5 で左右対称 | [`SSG_TRI_PEAK`](/2686V_2686VFX/reference/automation/ssg/#ssg-tri-peak) |

## TONE / NOISE / MIX — 音の混ぜ方（実機）

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **T.LV** | 波形（トーン）の音量 | 0.0〜1.0（初期 1.0） | |
| **N.LV** | ノイズの音量 | 0.0〜1.0（初期 1.0） | |
| **N.FQ** | ノイズの周波数 | 0〜30000 Hz（初期 12000） | |
| **MIX** | トーンとノイズの混ぜ具合 | 0.0〜1.0（初期 0.0） | |
| **Noise On Note** | 発音のたびにノイズを鳴らし直す | 入 / 切（初期は切） | |

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **TONE** | 音の側の量 | [`SSG_TONE`](/2686V_2686VFX/reference/automation/ssg/#ssg-tone) |
| **NOISE** | ざらつきの側の量 | [`SSG_NOISE`](/2686V_2686VFX/reference/automation/ssg/#ssg-noise) |
| **NOISE FREQ** | ざらつきの高さ | [`SSG_NOISEFREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-noisefreq) |
| **NOISE ON NOTE** | ざらつきの高さを鍵盤に合わせる（**独自**） | [`SSG_NOISE_ON_NOTE`](/2686V_2686VFX/reference/automation/ssg/#ssg-noise-on-note) |
| **MIX** | 2 つの混ぜ具合。0 で音だけ、1 でざらつきだけ | [`SSG_NOISEMIX`](/2686V_2686VFX/reference/automation/ssg/#ssg-noisemix) |

## HW ENV — ハードウェアエンベロープ（実機）

実機の SSG が持っている、音量を自動で動かす仕組みです。**Enable** を入れると
働きます。

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **Enable** | ハードウェアエンベロープを使う | 入 / 切（初期は切） | |
| **SHPE** | 形の種類 | 0〜43 | |
| **PROD** | 一周にかかる時間 | 0.1〜200.0（初期 1.0） | |

:::note
実機の形は 8 種類ですが、ここでは **43 種類**まで選べます。8 番目より後ろは
独自の追加です。実機の音を狙うときは 8 種類の中から選んでください。
:::

## そのほかの区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 印 | 内容 |
| --- | --- | --- |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope)（AMP / PITCH / SSG SW） | 独自 | 音量・音程を時間で動かす |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 独自 | 音量・音程を周期的に揺らす |
| [**MUL / DETUNE**](/2686V_2686VFX/chips/common/#mul--det) | 実機・独自 | 音程をずらす |
| [**UNISON**](/2686V_2686VFX/chips/common/#unison--harmony) | 独自 | 同じ音を重ねて厚くする |
| [**QUALITY**](/2686V_2686VFX/chips/common/#quality) | — | ビット数とサンプリング周波数を落として質感を作る |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | 共通 | レベルの一括設定と、ファイルの読み書き |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

## UTILITY — ファイルの読み書き（共通）

`[IM]` が読み込み、`[EX]` が書き出しです。読み書きできるのは、LFO・AMP ENV・
PITCH ENV・SSG SW ENV・DETUNE・UNISON・QUALITY・TONE/NOISE と、チャンネル
全体です。

**Level -> All Ch** は、このチャンネルの音量を他のすべてのチャンネルへ
一度に反映します。

ファイルの形式と置き場所は
[ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。

## オートメーション

SSG が DAW へ出すパラメータの一覧は
[SSG のオートメーション](/2686V_2686VFX/reference/automation/ssg/) に
あります。ID・型・範囲・初期値が並びます。
