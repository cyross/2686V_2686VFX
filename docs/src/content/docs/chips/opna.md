---
title: OPNAチャンネル
description: YM2608 にあたる 4 オペレータ FM チャンネル
sidebar:
  order: 1
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opna.png" alt="OPNAチャンネル" style="width:600px;" />
	<figcaption>OPNAチャンネル</figcaption>
</figure>

OPNA は **YM2608B** の FM 音源部分にあたるチャンネルです。PC-9801-86 に載っていた音源で、
このシリーズの名前の由来にもなっています。

4 オペレータの FM に加えて、実機が持っていた **SSG のハードウェアエンベロープ**
と **ハードウェア LFO**、そして当時 N88-BASIC が使っていた**ソフトウェア LFO**
まで揃っています。FM チャンネルの中では最も機能が多い部類です。

## 表記について

| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

実機の音を狙うときは、**独自**の項目を初期値のままにしておくと近づきます。

## オペレータとアルゴリズム

| 項目 | 内容 |
| --- | --- |
| オペレータ数 | **4** |
| アルゴリズム | **0〜7**（実機と同じ 8 種類） |
| フィードバック | OP1 のみ（実機と同じ） |

アルゴリズムの図は画面にリアルタイムで描かれます。どのオペレータがどこへ
変調を掛けているか、選んだ瞬間に確認できます。

詳細は、[リファレンス](../reference/lists-algorithm.md#opna) をご参照ください。

## 画面の区分

### ALGORITHM / FEEDBACK（実機）

アルゴリズムとフィードバックを選びます。図はここに出ます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ALG** | オペレータのつなぎ方。実機と同じ 8 種類 | [`OPNA_ALG`](/2686V_2686VFX/reference/automation/opna/#opna-alg) |
| **FB** | OP1 が自分自身へ戻す量。0 で無し、7 で最大 | [`OPNA_FB1`](/2686V_2686VFX/reference/automation/opna/#opna-fb1) |

### AMP ENV（実機・独自）

オペレータごとの音量エンベロープです。実機の **AR / DR / SR / SL / RR / TL**
に加えて、次の独自オプションが付きます。

| 項目 | 内容 |
| --- | --- |
| **KOR** | リリース後、指定時間まで音量が減衰しない |
| **XOF** | RR を無視して減衰しない |
| **Bypass** | 音量エンベロープを通さない |

どれも **SSG ソフトウェアエンベロープを主役にしたいとき**に使います。FM の
エンベロープを止めて、SSG 側の刻みだけで音量を作る、という組み方ができます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | アタックレート。大きいほど速く立ち上がる | [`OPNA_OP[0-3]_R_AR`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-r-ar) |
| **DR** | ディケイレート。大きいほど速く減る | [`OPNA_OP[0-3]_R_DR`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-r-dr) |
| **SL** | サステインレベル。DR で下がりきる高さ | [`OPNA_OP[0-3]_R_SL`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-r-sl) |
| **SR** | サステインレート。押しているあいだ減り続ける速さ | [`OPNA_OP[0-3]_R_SR`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-r-sr) |
| **RR** | リリースレート。鍵を離してから消えるまでの速さ | [`OPNA_OP[0-3]_R_RR`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-r-rr) |
| **TL** | トータルレベル。**大きいほど音が小さくなる** | [`OPNA_OP[0-3]_R_TL`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-r-tl) |
| **XOF** | RR を無視して減衰させない（**独自**） | [`OPNA_OP[0-3]_XOF`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-xof) |
| **KOR** | 鍵を離しても、指定の時間まで減衰させない（**独自**） | [`OPNA_OP[0-3]_KOR`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-kor) |
| **Bypass** | このオペレータの音量エンベロープを通さない（**独自**） | [`OPNA_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-amp-bypass) |

### MUL / DET（実機）

逓倍とデチューンです。オペレータごとに音程をずらして、うなりや倍音を作ります。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MUL** | 逓倍。0 が 0.5 倍、1〜15 がそのままの倍数 | [`OPNA_OP[0-3]_MUL`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-mul) |
| **DT** | デチューン。実機と同じ 8 段（0 と ±1〜3） | [`OPNA_OP[0-3]_DT`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-dt) |

### HW LFO（実機）

YM2608 が持っていたハードウェア LFO です。チャンネル全体に掛かります。


YM2608 が持っていたハードウェア LFO です。実機は速さがチップ全体で 1 つでしたが、ここでは**オペレータごとに**速さと効きを決められます（**独自**）。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **FQ** | 揺れの速さ。実機と同じ 8 段（3.98Hz〜72.2Hz） | [`OPNA_OP[0-3]_LFO_FREQS`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-lfo-freqs) |
| **SDLY** | 押してから揺れ始めるまでの待ち（**独自**） | [`OPNA_OP[0-3]_LFO_SYNC_DELAY`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-lfo-sync-delay) |
| **PM** | 音程側の揺れを掛ける | [`OPNA_OP[0-3]_LFO_VIB`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-lfo-vib) |
| **PMS** | 音程の揺れ幅。0cent〜±80cent の 8 段 | [`OPNA_OP[0-3]_LFO_PMS`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-lfo-pms) |
| **AM** | 音量側の揺れを掛ける | [`OPNA_OP[0-3]_LFO_AM`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-lfo-am) |
| **AMS** | 音量の揺れ幅。0dB / 1.4dB / 5.9dB / 11.8dB | [`OPNA_OP[0-3]_LFO_AMS`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-lfo-ams) |

### N88 LFO（実機由来）

**N88-BASIC が使っていたソフトウェア LFO** を再現したものです。ハードウェア
LFO とは別に動くので、両方を重ねられます。

当時のプログラムが自前で音程を揺らしていた動きを、そのまま画面から組めます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SP** | 揺れの速さ | [`OPNA_LFO_FREQ`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-freq) |
| **SH** | 揺れる形。Saw Up / Square / Triangle / S&H と、1 回だけ動く 2 種 | [`OPNA_LFO_SHAPE`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-shape) |
| **SDLY** | 押してから揺れ始めるまでの待ち | [`OPNA_LFO_SYNC_DELAY`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-sync-delay) |
| **PM Enable** | 音程側の揺れを掛ける | [`OPNA_LFO_PM`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-pm) |
| **PMS** | 音程の効き | [`OPNA_LFO_PMS`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-pms) |
| **PMD** | 音程の深さ。負の値で揺れる向きが反転する | [`OPNA_LFO_PMD`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-pmd) |
| **AM Enable** | 音量側の揺れを掛ける | [`OPNA_LFO_AM`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-am) |
| **AMD** | 音量の深さ | [`OPNA_LFO_AMD`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-amd) |
| **AMSR** | 音量側の角の丸め | [`OPNA_LFO_ASMRT`](/2686V_2686VFX/reference/automation/opna/#opna-lfo-asmrt) |
| **AMS** | オペレータごとの音量の効き | [`OPNA_OP[0-3]_N88AMS`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-n88ams) |

### SSG HW ENV（実機・独自）

SSG のハードウェアエンベロープを FM 側でも使えます。実機の 8 種類の形に加えて
**独自波形を 36 種**足し、範囲の MIN / MAX も指定できます（**独自**）。


オペレータごとに掛ける SSG のエンベロープです。チャンネル全体へ掛けるものは [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) にあります。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SHPE** | 形。Normal（掛けない）＋実機の 8 種類 | [`OPNA_OP[0-3]_SE`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-se) |
| **FQ** | 繰り返しの速さ | [`OPNA_OP[0-3]_SEFREQ`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-sefreq) |

### KEY SCALE（実機）

音の高さによって音量やレートを変える設定です。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KS** | 音が高いほどエンベロープを速める度合い。OFF / 弱 / 中 / 強 | [`OPNA_OP[0-3]_KS`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-ks) |

### MASK（実機）

オペレータごとに音を止めます。どのオペレータがどう効いているか確かめるときに
使います。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MASK** | このオペレータを鳴らさない | [`OPNA_OP[0-3]_MASK`](/2686V_2686VFX/reference/automation/opna/#opna-op-0-3-mask) |

### PAN（実機）

左右の定位です。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **PAN** | 定位。−1 が左、0 が中央、+1 が右 | [`OPNA_PAN`](/2686V_2686VFX/reference/automation/opna/#opna-pan) |

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

## MML 入力

オペレータごとに **MML** ボタンがあります。OPNA では `AR` `DR` `SR` `SL` `RR`
`TL` `MUL` `DT` `KS` `MASK` を受け付けます。

書き方は [MML 入力](/2686V_2686VFX/reference/mml/) を参照してください。

## オートメーション

OPNA が DAW へ出すパラメータの一覧は
[OPNA のオートメーション](/2686V_2686VFX/reference/automation/opna/) に
あります。ID・型・範囲・初期値が並びます。
