---
title: OPLチャンネル
description: YM3812 / YM2413 にあたる 2 オペレータ FM チャンネル
sidebar:
  order: 3
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opl.png" alt="OPLチャンネル" style="width:600px;" />
	<figcaption>OPLチャンネル</figcaption>
</figure>

OPL は **YM3812（OPL2）** と **YM2413（OPLL）** の FM 音源部分にあたるチャンネルです。
MSX-MUSIC やゲーム基板でよく使われた系統で、OPN 系とは音の作りがはっきり
違います。

いちばんの違いは **オペレータが 2 つ**しかないことです。そのぶん組み合わせは
限られますが、**波形を選べる**という別の武器があります。

## 表記について

| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## オペレータとアルゴリズム

| 項目 | 内容 |
| --- | --- |
| オペレータ数 | **2** |
| アルゴリズム | **0〜1**（FM と加算の 2 種類） |

アルゴリズムが 2 つしかないのは実機どおりです。OP2 が OP1 を変調する（FM）か、
2 つを足す（加算）かの選択になります。

詳細は、[リファレンス](../reference/lists-algorithm.md#opl) をご参照ください。

## 画面の区分

### ALGORITHM / FEEDBACK（実機）

FM か加算かを選びます。図はここに出ます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ALG** | オペレータのつなぎ方 | [`OPL_ALG`](/2686V_2686VFX/reference/automation/opl/#opl-alg) |
| **FB** | OP1 が自分自身へ戻す量 | [`OPL_FB1`](/2686V_2686VFX/reference/automation/opl/#opl-fb1) |

### SHAPE（実機）

**この系統の要です。** サイン波を切ったり折り返したりした波形を選べます。
OPN 系がサイン波しか出せないのに対し、OPL は波形そのものを変えて音色を作り
ます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **EG** | オペレータの波形。正弦をどう削るかを選ぶ | [`OPL_OP[0-1]_EG`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-eg) |

### AMP ENV（実機・独自）

オペレータごとの音量エンベロープです。**KOR / XOF / Bypass** の独自オプション
が付きます。


OPL 系は SR を持ちません。減衰の続き方は EG SHAPE の EGTP で決めます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | アタックレート | [`OPL_OP[0-1]_R_AR`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-r-ar) |
| **DR** | ディケイレート | [`OPL_OP[0-1]_R_DR`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-r-dr) |
| **SL** | サステインレベル | [`OPL_OP[0-1]_R_SL`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-r-sl) |
| **RR** | リリースレート | [`OPL_OP[0-1]_R_RR`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-r-rr) |
| **TL** | トータルレベル。**大きいほど音が小さくなる** | [`OPL_OP[0-1]_R_TL`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-r-tl) |
| **SUS** | キーオフのあとも音を保つ | [`OPL_OP[0-1]_SUS`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-sus) |
| **XOF** | RR を無視して減衰させない（**独自**） | [`OPL_OP[0-1]_XOF`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-xof) |
| **KOR** | 鍵を離しても、指定の時間まで減衰させない（**独自**） | [`OPL_OP[0-1]_KOR`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-kor) |
| **Bypass** | このオペレータの音量エンベロープを通さない（**独自**） | [`OPL_OP[0-1]_AMP_BYPASS`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-amp-bypass) |

### EG SHAPE（実機）

エンベロープの型を選びます。減衰し切る型と、押している間持続する型があります。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **EGTP** | 押しているあいだ音量を保つ（オン）か、減らし続ける（オフ）か | [`OPL_OP[0-1]_EG_TYP`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-eg-typ) |

### LFO（実機）

**オペレータごとに独立した**ハードウェア LFO です。OPN 系のチャンネル全体に
掛かる LFO とは違い、片方のオペレータだけを揺らすといった使い方ができます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **VIB** | 音程側の揺れを掛ける | [`OPL_OP[0-1]_LFO_VIB`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-lfo-vib) |
| **PMS** | 音程の揺れの速さ（Hz） | [`OPL_OP[0-1]_LFO_PMS`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-lfo-pms) |
| **PMD** | 音程の揺れの深さ（セント） | [`OPL_OP[0-1]_LFO_PMD`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-lfo-pmd) |
| **AM** | 音量側の揺れを掛ける | [`OPL_OP[0-1]_LFO_AM`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-lfo-am) |
| **AMS** | 音量の揺れの速さ（Hz） | [`OPL_OP[0-1]_LFO_AMS`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-lfo-ams) |
| **AMD** | 音量の揺れの深さ（dB） | [`OPL_OP[0-1]_LFO_AMD`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-lfo-amd) |

### MUL / DET（実機）

逓倍とデチューンです。


OPL 系にはデチューンがありません。ずらしたいときは [MUL/DET](/2686V_2686VFX/chips/common/#muldet) の DT3 を使います。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MUL** | 逓倍 | [`OPL_OP[0-1]_MUL`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-mul) |

### KEY SCALE（実機）

**KSL（レベル）と KSR（レート）**を別々に設定します。OPL 系はこの 2 つが
分かれているのが特徴です。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KSR** | 音が高いほどエンベロープを速める | [`OPL_OP[0-1]_KSR`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-ksr) |
| **KSL** | 音が高いほど音量を下げる度合い。OFF / 1.5 / 3.0 / 6.0 dB/oct | [`OPL_OP[0-1]_KSL`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-ksl) |

### MASK（実機）

オペレータごとに音を止めます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MASK** | このオペレータを鳴らさない | [`OPL_OP[0-1]_MASK`](/2686V_2686VFX/reference/automation/opl/#opl-op-0-1-mask) |

### INITIALIZE / OPTIONAL

初期化と、実機が持っていた追加の動作です。

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

:::tip[OPLL の音色を試す]
付属のプリセットに `OPLL Roms` フォルダがあります。OPLL の ROM 音色をなぞった
もので、当時の音を手早く確かめられます。扱いは
[ライセンス](/2686V_2686VFX/reference/license/) を参照してください。
:::

## MML 入力

OPL では `AR` `DR` `RR` `SL` `TL` `MUL` `MASK` に加えて、この系統特有の
`KSL` `KSR` `AM` `PM`（`VIB`）`EGTYPE` を受け付けます。

書き方は [MML 入力](/2686V_2686VFX/reference/mml/) を参照してください。

## オートメーション

OPL が DAW へ出すパラメータの一覧は
[OPL のオートメーション](/2686V_2686VFX/reference/automation/opl/) に
あります。ID・型・範囲・初期値が並びます。
