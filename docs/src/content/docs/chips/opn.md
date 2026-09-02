---
title: OPN
description: YM2203 にあたる 4 オペレータ FM チャンネル
sidebar:
  order: 2
---

OPN は **YM2203** の FM 音源部分にあたるチャンネルです。PC-9801-26 に載っていた音源で、
OPNA より前の世代にあたります。

作りは OPNA とほぼ同じですが、**ハードウェア LFO と定位（PAN）がありません。**
実機がそれらを持っていなかったためです。当時の 26 音源の構成に寄せたいときは、
OPNA ではなくこちらを選びます。

## 印について

| 印 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## オペレータとアルゴリズム

| 項目 | 内容 |
| --- | --- |
| オペレータ数 | **4** |
| アルゴリズム | **0〜7**（実機と同じ 8 種類） |
| フィードバック | OP1 のみ（実機と同じ） |

詳細は、[リファレンス](../reference/lists-algorithm.md#opn) をご参照ください。

## OPNA との違い

| 区分 | OPN | OPNA |
| --- | :---: | :---: |
| HW LFO | — | ✔ |
| N88 LFO | ✔ | ✔ |
| SSG HW ENV | — | ✔ |
| PAN | — | ✔ |

音程を揺らしたいときは **N88 LFO** を使います。当時の 26 音源でも、揺らしは
ドライバがソフトウェアで作っていました。その動きをそのまま再現したものです。

## 画面の区分

### ALGORITHM / FEEDBACK（実機）

アルゴリズムとフィードバックを選びます。図はここに出ます。

### AMP ENV（実機・独自）

オペレータごとの音量エンベロープです。実機の **AR / DR / SR / SL / RR / TL**
に、**KOR / XOF / Bypass** の独自オプションが付きます。SSG ソフトウェア
エンベロープを主役にしたいときに使います。

### MUL / DET（実機）

逓倍とデチューンです。

### N88 LFO（実機由来）

N88-BASIC が使っていたソフトウェア LFO を再現したものです。

### KEY SCALE（実機）

音の高さによって音量やレートを変えます。

### MASK（実機）

オペレータごとに音を止めます。

### OPTIONAL（実機）

効果音モードなど、実機が持っていた追加の動作です。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/chips/common/#quality) | ビット数とサンプリング周波数を落として質感を作る |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope)（チップ全体） | AMP / PITCH / SSG SW / SSG SW11 |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**UNISON / HARMONY**](/2686V_2686VFX/chips/common/#unison--harmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

:::tip[OPNA との行き来]
UTILITY から、**OPN と OPNA のあいだでパラメータをやりとりできます。** OPN で
作った音色を OPNA へ持っていって、ハードウェア LFO を足す、といった使い方が
できます。
:::

## MML 入力

OPNA と同じく `AR` `DR` `SR` `SL` `RR` `TL` `MUL` `DT` `KS` `MASK` を
受け付けます。

書き方は [MML 入力](/2686V_2686VFX/reference/mml/) を参照してください。

## オートメーション

OPN が DAW へ出すパラメータの一覧は
[OPN のオートメーション](/2686V_2686VFX/reference/automation/opn/) に
あります。ID・型・範囲・初期値が並びます。
