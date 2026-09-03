---
title: OPNチャンネル
description: YM2203 にあたる 4 オペレータ FM チャンネル
sidebar:
  order: 2
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opn.png" alt="OPNチャンネル" style="width:600px;" />
	<figcaption>OPNチャンネル</figcaption>
</figure>

OPN は **YM2203** の FM 音源部分にあたるチャンネルです。PC-9801-26 に載っていた音源で、
OPNA より前の世代にあたります。

作りは OPNA とほぼ同じですが、**ハードウェア LFO と定位（PAN）がありません。**
実機がそれらを持っていなかったためです。当時の 26 音源の構成に寄せたいときは、
OPNA ではなくこちらを選びます。

## 表記について

| 表記 | 意味 |
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


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ALG** | オペレータのつなぎ方。実機と同じ 8 種類 | [`OPN_ALG`](/2686V_2686VFX/reference/automation/opn/#opn-alg) |
| **FB** | OP1 が自分自身へ戻す量。0 で無し、7 で最大 | [`OPN_FB1`](/2686V_2686VFX/reference/automation/opn/#opn-fb1) |

### AMP ENV（実機・独自）

オペレータごとの音量エンベロープです。実機の **AR / DR / SR / SL / RR / TL**
に、**KOR / XOF / Bypass** の独自オプションが付きます。SSG ソフトウェア
エンベロープを主役にしたいときに使います。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | アタックレート。大きいほど速く立ち上がる | [`OPN_OP[0-3]_R_AR`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-r-ar) |
| **DR** | ディケイレート。大きいほど速く減る | [`OPN_OP[0-3]_R_DR`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-r-dr) |
| **SL** | サステインレベル。DR で下がりきる高さ | [`OPN_OP[0-3]_R_SL`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-r-sl) |
| **SR** | サステインレート。押しているあいだ減り続ける速さ | [`OPN_OP[0-3]_R_SR`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-r-sr) |
| **RR** | リリースレート。鍵を離してから消えるまでの速さ | [`OPN_OP[0-3]_R_RR`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-r-rr) |
| **TL** | トータルレベル。**大きいほど音が小さくなる** | [`OPN_OP[0-3]_R_TL`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-r-tl) |
| **XOF** | RR を無視して減衰させない（**独自**） | [`OPN_OP[0-3]_XOF`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-xof) |
| **KOR** | 鍵を離しても、指定の時間まで減衰させない（**独自**） | [`OPN_OP[0-3]_KOR`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-kor) |
| **Bypass** | このオペレータの音量エンベロープを通さない（**独自**） | [`OPN_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-amp-bypass) |

### MUL / DET（実機）

逓倍とデチューンです。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MUL** | 逓倍。0 が 0.5 倍、1〜15 がそのままの倍数 | [`OPN_OP[0-3]_MUL`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-mul) |
| **DT** | デチューン。実機と同じ 8 段（0 と ±1〜3） | [`OPN_OP[0-3]_DT`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-dt) |

### N88 LFO（実機由来）

N88-BASIC が使っていたソフトウェア LFO を再現したものです。


OPN にはハードウェア LFO がないので、揺らしはここで作ります。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SP** | 揺れの速さ | [`OPN_LFO_FREQ`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-freq) |
| **SH** | 揺れる形。Saw Up / Square / Triangle / S&H と、1 回だけ動く 2 種 | [`OPN_LFO_SHAPE`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-shape) |
| **SDLY** | 押してから揺れ始めるまでの待ち | [`OPN_LFO_SYNC_DELAY`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-sync-delay) |
| **PM Enable** | 音程側の揺れを掛ける | [`OPN_LFO_PM`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-pm) |
| **PMS** | 音程の効き | [`OPN_LFO_PMS`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-pms) |
| **PMD** | 音程の深さ。負の値で揺れる向きが反転する | [`OPN_LFO_PMD`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-pmd) |
| **AM Enable** | 音量側の揺れを掛ける | [`OPN_LFO_AM`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-am) |
| **AMD** | 音量の深さ | [`OPN_LFO_AMD`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-amd) |
| **AMSR** | 音量側の角の丸め | [`OPN_LFO_ASMRT`](/2686V_2686VFX/reference/automation/opn/#opn-lfo-asmrt) |
| **AMS** | オペレータごとの音量の効き | [`OPN_OP[0-3]_N88AMS`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-n88ams) |

### KEY SCALE（実機）

音の高さによって音量やレートを変えます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KS** | 音が高いほどエンベロープを速める度合い。OFF / 弱 / 中 / 強 | [`OPN_OP[0-3]_KS`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-ks) |

### MASK（実機）

オペレータごとに音を止めます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MASK** | このオペレータを鳴らさない | [`OPN_OP[0-3]_MASK`](/2686V_2686VFX/reference/automation/opn/#opn-op-0-3-mask) |

### OPTIONAL（実機）

効果音モードなど、実機が持っていた追加の動作です。

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
