---
title: ADPCM
description: 音声ファイル 1 つをサンプラーのように鳴らすチャンネル
sidebar:
  order: 12
---

ADPCM は、**音声ファイルを 1 つ読み込んで、サンプラーのように鳴らす**
チャンネルです。YM2608B が持っていた ADPCM 再生にあたります。

RHYTHM が複数のパッドに短い音を並べるのに対し、こちらは**1 つの音を鍵盤で
音階として弾きます。** 声や環境音を持ち込んで、当時の音源らしく粗くする、
といった使い方に向きます。

:::note[86V では PCM]
86V では、 PC-9801-86 の仕様に合わせて、このチャンネルの名前が **PCM** になり、音質の初期値が **4bit PCM**
となっています。中身と操作は同じです。
:::

## 印について

| 印 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## FORM — 音声ファイルの設定

**Load** でファイルを読み込みます。読み込んだ波形は画面で確認できます。

| 項目 | 内容 |
| --- | --- |
| **P.RT** / **P.OF** | 再生の割合と開始位置 |
| **LOOP** | ループを使うか |
| **LPST** / **LPED** | ループの始まりと終わり |

**LPST / LPED を波形の途中に置く**と、押している間そこだけを繰り返します。
持続する音を短い素材から作れます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **LOOP** | ファイル全体を繰り返す | [`ADPCM_LOOP`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-loop) |
| **PAN** | 定位。0 が左、0.5 が中央、1 が右 | [`ADPCM_PAN`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-pan) |
| **PCM OFFSET** | 頭出し（ミリ秒） | [`ADPCM_PCM_OFFSET`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-pcm-offset) |
| **PCM RATIO** | 再生の速さ | [`ADPCM_PCM_RATIO`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-pcm-ratio) |
| **LOOP POINT** | 繰り返す範囲を指定する | [`ADPCM_LOOPPOINT_EN`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-looppoint-en) |
| **LOOP START / END** | その範囲。全体を 0.0〜1.0 とした位置 | [`ADPCM_LOOPPOINT_ST`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-looppoint-st) |

## QUALITY — 音質（実機・独自）

読み込んだ音のビット数とサンプリング周波数を落とします。**このチャンネルの
性格を決めるところ**です。

| 項目 | 内容 | 範囲 |
| --- | --- | --- |
| **BIT** | ビット数と圧縮方式 | 下表 |
| **RATE** | サンプリング周波数。96kHz から 2kHz まで | 1〜15（初期 9 ＝ 16kHz） |
| **INTP** | 読み戻すときの補間のしかた | 0〜6（初期 1 ＝ Linear） |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

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

## そのほかの区分

| 項目 | 内容 |
| --- | --- |
| **Tone / Noise** | ノイズを混ぜる（`T.LV` / `N.LV` / `N.FQ` / `MIX`） |
| **PAN** | 左右の定位 |
| [**FIX**](/2686V_2686VFX/chips/common/#fix) | 音程を固定する |

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **TONE** | 読み込んだ音の側の量 | [`ADPCM_TONE`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-tone) |
| **NOISE** | ざらつきの側の量 | [`ADPCM_NOISE`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-noise) |
| **NOISE FREQ** | ざらつきの高さ | [`ADPCM_NOISEFREQ`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-noisefreq) |
| **MIX** | 2 つの混ぜ具合 | [`ADPCM_NOISEMIX`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-noisemix) |

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | AMP / SSG HW / SSG SW（6・11 タップ）/ PITCH（3・11 タップ） |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 音量・音程を周期的に揺らす |
| [**MUL / DET**](/2686V_2686VFX/chips/common/#mul--det) | 音程をずらす |
| [**UNISON / HARMONY**](/2686V_2686VFX/chips/common/#unison--harmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

## 付属のプリセット

`M-M-Pro` に、音声合成をリズム音源と ADPCM で組んだ例が入っています。

扱いは [ライセンス](/2686V_2686VFX/reference/license/) を参照してください。

## オートメーション

ADPCM が DAW へ出すパラメータの一覧は
[ADPCM のオートメーション](/2686V_2686VFX/reference/automation/adpcm/) に
あります。ID・型・範囲・初期値が並びます。
