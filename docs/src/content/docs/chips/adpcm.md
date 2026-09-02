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

## 画面の区分

### FORM — 音声ファイルの設定

**Load** でファイルを読み込みます。読み込んだ波形は画面で確認できます。

**LPST / LPED を波形の途中に置く**と、押している間そこだけを繰り返します。
持続する音を短い素材から作れます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **LOOP** | ファイル全体を繰り返す | [`ADPCM_LOOP`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-loop) |
| **P.OF** | 頭出し（ミリ秒） | [`ADPCM_PCM_OFFSET`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-pcm-offset) |
| **P.RT** | 再生の速さ | [`ADPCM_PCM_RATIO`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-pcm-ratio) |
| **Loop Point Enable** | 繰り返す範囲を指定する | [`ADPCM_LOOPPOINT_EN`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-looppoint-en) |
| **LPST / LPED** | その範囲。全体を 0.0〜1.0 とした位置 | [`ADPCM_LOOPPOINT_ST`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-looppoint-st) |

### OPTIONAL

読み込んだ音へ、ざらつきを混ぜられます（**独自**）。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **T.LV** | 読み込んだ音の側の量 | [`ADPCM_TONE`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-tone) |
| **N.LV** | ざらつきの側の量 | [`ADPCM_NOISE`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-noise) |
| **N.FQ** | ざらつきの高さ | [`ADPCM_NOISEFREQ`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-noisefreq) |
| **MIX** | 2 つの混ぜ具合 | [`ADPCM_NOISEMIX`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-noisemix) |

### PAN

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **PAN** | 定位。0 が左、0.5 が中央、1 が右 | [`ADPCM_PAN`](/2686V_2686VFX/reference/automation/adpcm/#adpcm-pan) |

### FIX — 音程の固定

鍵盤の音程を無視して、決まった高さで鳴らします。打楽器のように、どの鍵を
押しても同じ高さで鳴ってほしいときに使います。

つまみは [FIX](/2686V_2686VFX/chips/common/#fix) を参照してください。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY(PCM)**](/2686V_2686VFX/chips/common/#qualitypcm) | ビット数・記録方式・サンプリング周波数と、読み戻すときの補間 |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 音量・音程を周期的に揺らす |
| [**MUL/DET**](/2686V_2686VFX/chips/common/#muldet) | 音程をずらす |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

## 付属のプリセット

`M-M-Pro` に、音声合成をリズム音源と ADPCM で組んだ例が入っています。

扱いは [ライセンス](/2686V_2686VFX/reference/license/) を参照してください。

## オートメーション

ADPCM が DAW へ出すパラメータの一覧は
[ADPCM のオートメーション](/2686V_2686VFX/reference/automation/adpcm/) に
あります。ID・型・範囲・初期値が並びます。
