---
title: OPL3チャンネル
description: YMF262 にあたる 4 オペレータ FM チャンネル
sidebar:
  order: 4
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opl3.png" alt="OPL3チャンネル" style="width:600px;" />
	<figcaption>OPL3チャンネル</figcaption>
</figure>

OPL3 は **YMF262** の FM 音源部分にあたるチャンネルです。OPL の系譜を継ぎつつ、オペレータが
4 つに増え、波形の種類も倍になりました。DOS 時代のサウンドカードでおなじみの
音源です。

OPL の「波形を選べる」という持ち味に、OPN 系の 4 オペレータが加わった形です。

## 表記について

| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## オペレータとアルゴリズム

| 項目 | 内容 |
| --- | --- |
| オペレータ数 | **4** |
| アルゴリズム | **0〜7** |

実機の 4 種類に加えて、**2OPx2**（2 オペレータの組を 2 つ並べる）という独自
アルゴリズムを足してあります（**独自**）。1 つのチャンネルで 2 音色を重ねる
ような使い方ができます。

詳細は、[リファレンス](../reference/lists-algorithm.md#opl3) をご参照ください。

## OPL との違い

| 区分 | OPL | OPL3 |
| --- | :---: | :---: |
| オペレータ数 | 2 | **4** |
| アルゴリズム | 2 種類 | **8 種類**（独自を含む） |
| 波形 | 4 種類 | **8 種類** |
| SSG HW ENV | ✔ | — |

## 画面の区分

### ALGORITHM / FEEDBACK（実機・独自）

アルゴリズムとフィードバックを選びます。2OPx2 はここから選びます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ALG** | オペレータのつなぎ方 | [`OPL3_ALG`](/2686V_2686VFX/reference/automation/opl3/#opl3-alg) |
| **FB** | OP1 が自分自身へ戻す量 | [`OPL3_FB1`](/2686V_2686VFX/reference/automation/opl3/#opl3-fb1) |

### SHAPE（実機）

OPL の 4 種類から **8 種類**へ増えています。サイン波を切る・折り返す・
半分だけ使うといった変形が並びます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **EG** | オペレータの波形。正弦をどう削るかを選ぶ | [`OPL3_OP[0-3]_EG`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-eg) |

### AMP ENV（実機・独自）

オペレータごとの音量エンベロープです。**KOR / XOF / Bypass** の独自オプション
が付きます。


OPL 系は SR を持ちません。減衰の続き方は EG SHAPE の EGTP で決めます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | アタックレート | [`OPL3_OP[0-3]_R_AR`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-r-ar) |
| **DR** | ディケイレート | [`OPL3_OP[0-3]_R_DR`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-r-dr) |
| **SL** | サステインレベル | [`OPL3_OP[0-3]_R_SL`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-r-sl) |
| **RR** | リリースレート | [`OPL3_OP[0-3]_R_RR`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-r-rr) |
| **TL** | トータルレベル。**大きいほど音が小さくなる** | [`OPL3_OP[0-3]_R_TL`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-r-tl) |
| **XOF** | RR を無視して減衰させない（**独自**） | [`OPL3_OP[0-3]_XOF`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-xof) |
| **KOR** | 鍵を離しても、指定の時間まで減衰させない（**独自**） | [`OPL3_OP[0-3]_KOR`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-kor) |
| **Bypass** | このオペレータの音量エンベロープを通さない（**独自**） | [`OPL3_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-amp-bypass) |

### EG SHAPE（実機）

エンベロープの型を選びます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **EGTP** | 押しているあいだ音量を保つ（オン）か、減らし続ける（オフ）か | [`OPL3_OP[0-3]_EG_TYP`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-eg-typ) |

### LFO（実機）

オペレータごとに独立した LFO です。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **VIB** | 音程側の揺れを掛ける | [`OPL3_OP[0-3]_LFO_VIB`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-lfo-vib) |
| **PMS** | 音程の揺れの速さ（Hz） | [`OPL3_OP[0-3]_LFO_PMS`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-lfo-pms) |
| **PMD** | 音程の揺れの深さ（セント） | [`OPL3_OP[0-3]_LFO_PMD`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-lfo-pmd) |
| **AM** | 音量側の揺れを掛ける | [`OPL3_OP[0-3]_LFO_AM`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-lfo-am) |
| **AMS** | 音量の揺れの速さ（Hz） | [`OPL3_OP[0-3]_LFO_AMS`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-lfo-ams) |
| **AMD** | 音量の揺れの深さ（dB） | [`OPL3_OP[0-3]_LFO_AMD`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-lfo-amd) |

### MUL / DET（実機）

逓倍とデチューンです。


OPL 系にはデチューンがありません。ずらしたいときは [MUL/DET](/2686V_2686VFX/chips/common/#muldet) の DT3 を使います。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MUL** | 逓倍 | [`OPL3_OP[0-3]_MUL`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-mul) |

### KEY SCALE（実機）

**KSL（レベル）と KSR（レート）**を別々に設定します。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KSR** | 音が高いほどエンベロープを速める | [`OPL3_OP[0-3]_KSR`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-ksr) |
| **KSL** | 音が高いほど音量を下げる度合い。OFF / 1.5 / 3.0 / 6.0 dB/oct | [`OPL3_OP[0-3]_KSL`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-ksl) |

### MASK（実機）

オペレータごとに音を止めます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MASK** | このオペレータを鳴らさない | [`OPL3_OP[0-3]_MASK`](/2686V_2686VFX/reference/automation/opl3/#opl3-op-0-3-mask) |

### OPTIONAL（実機）

実機が持っていた追加の動作です。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/chips/common/#quality) | ビット数とサンプリング周波数を落として質感を作る |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**WT AMP MOD**](/2686V_2686VFX/chips/common/#wt-amp-mod) | 波形メモリによる音量の変調 |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

:::tip[OPL との行き来]
UTILITY から、**OPL と OPL3 のあいだでパラメータをやりとりできます。** OPL で
作った 2 オペレータの音色を持ち込んで、残り 2 つを足す、といった組み方ができ
ます。
:::

## MML 入力

OPL と同じ記号を受け付けます。`AR` `DR` `RR` `SL` `TL` `MUL` `MASK`
`KSL` `KSR` `AM` `PM`（`VIB`）`EGTYPE`。

書き方は [MML 入力](/2686V_2686VFX/reference/mml/) を参照してください。

## オートメーション

OPL3 が DAW へ出すパラメータの一覧は
[OPL3 のオートメーション](/2686V_2686VFX/reference/automation/opl3/) に
あります。ID・型・範囲・初期値が並びます。
