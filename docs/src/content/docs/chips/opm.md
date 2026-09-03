---
title: OPMチャンネル
description: YM2151 にあたる 4 オペレータ FM チャンネル
sidebar:
  order: 5
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opm.png" alt="OPMチャンネル" style="width:600px;" />
	<figcaption>OPMチャンネル</figcaption>
</figure>

OPM は **YM2151** の FM 音源部分にあたるチャンネルです。X68000 やアーケード基板で広く使われ、
FM 音源の音といえばこれを思い浮かべる人も多い音源です。

OPN 系と同じ 4 オペレータですが、**エンベロープの持ち方とデチューンの細かさ**
が違います。とくに **DT2** は OPM 系だけのもので、大きく音程をずらして金属的な
響きを作れます。

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

詳細は、[リファレンス](../reference/lists-algorithm.md#opm) をご参照ください。

## OPN 系との違い

| 項目 | OPN / OPNA | OPM |
| --- | --- | --- |
| エンベロープ | AR / DR / SR / SL / RR | **AR / D1R / D1L / D2R / RR** |
| デチューン | DT のみ | **DT1 と DT2** |
| キースケール | KS | KS（**OPP の設定も選べる**） |

エンベロープの呼び方が違うだけでなく、**減衰が 2 段**あるのが OPM の特徴です。
D1R で最初の落ち方、D1L でどこまで落ちるか、D2R でそこから先の落ち方を決めます。

## 画面の区分

### ALGORITHM / FEEDBACK（実機）

アルゴリズムとフィードバックを選びます。図はここに出ます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ALG** | オペレータのつなぎ方。実機と同じ 8 種類 | [`OPM_ALG`](/2686V_2686VFX/reference/automation/opm/#opm-alg) |
| **FB** | OP1 が自分自身へ戻す量 | [`OPM_FB1`](/2686V_2686VFX/reference/automation/opm/#opm-fb1) |

### AMP ENV（実機・独自）

**AR / D1R / D1L / D2R / RR / TL** です。**KOR / XOF / Bypass** の独自
オプションが付きます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | アタックレート | [`OPM_OP[0-3]_R_AR`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-r-ar) |
| **D1R** | 1 段目の減衰の速さ（他機種の DR） | [`OPM_OP[0-3]_R_D1R`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-r-d1r) |
| **D1L** | 1 段目が下がりきる高さ（他機種の SL） | [`OPM_OP[0-3]_R_D1L`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-r-d1l) |
| **D2R** | 2 段目の減衰の速さ（他機種の SR） | [`OPM_OP[0-3]_R_D2R`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-r-d2r) |
| **RR** | リリースレート | [`OPM_OP[0-3]_R_RR`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-r-rr) |
| **TL** | トータルレベル。**大きいほど音が小さくなる** | [`OPM_OP[0-3]_R_TL`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-r-tl) |
| **XOF** | RR を無視して減衰させない（**独自**） | [`OPM_OP[0-3]_XOF`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-xof) |
| **KOR** | 鍵を離しても、指定の時間まで減衰させない（**独自**） | [`OPM_OP[0-3]_KOR`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-kor) |
| **Bypass** | このオペレータの音量エンベロープを通さない（**独自**） | [`OPM_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-amp-bypass) |

### MUL / DET（実機・独自）

逓倍と、**DT1 / DT2** です。DT2 は音程を段階的に大きくずらすもので、鐘や
金属的な音を作るときの要になります。

DT3 という独自の項目もあります。DT1 / DT2 の枠にとらわれず、自由に値を
決められます（**独自**）。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MUL** | 逓倍 | [`OPM_OP[0-3]_MUL`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-mul) |
| **MURT** | 倍率を整数から外して自由に決める（**独自**） | [`OPM_OP[0-3]_MUL_RATIO`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-mul-ratio) |
| **DT1** | 細かいデチューン | [`OPM_OP[0-3]_DT`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-dt) |
| **DT2** | 粗いデチューン。実機と同じ 4 段 | [`OPM_OP[0-3]_DT2`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-dt2) |

### HW LFO（実機）

YM2151 のハードウェア LFO です。波形・速度・掛かり具合を細かく決められます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **FQ** | 揺れの速さ | [`OPM_LFO_FREQ`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-freq) |
| **SDLY** | 押してから揺れ始めるまでの待ち | [`OPM_LFO_SYNC_DELAY`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-sync-delay) |
| **PM Enable** | 音程側の揺れを掛ける | [`OPM_LFO_PM`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-pm) |
| **P.SH** | 音程側の形。実機と同じ 4 種 | [`OPM_LFO_PG_SHAPE`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-pg-shape) |
| **PMS** | 音程の効き | [`OPM_LFO_PMS`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-pms) |
| **PMD** | 音程の深さ | [`OPM_LFO_PMD`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-pmd) |
| **AM Enable** | 音量側の揺れを掛ける | [`OPM_LFO_AM`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-am) |
| **A.SH** | 音量側の形 | [`OPM_LFO_EG_SHAPE`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-eg-shape) |
| **AMS** | 音量の効き | [`OPM_LFO_AMS`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-ams) |
| **AMD** | 音量の深さ | [`OPM_LFO_AMD`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-amd) |
| **AMSR** | 音量側の角の丸め（**独自**） | [`OPM_LFO_ASMRT`](/2686V_2686VFX/reference/automation/opm/#opm-lfo-asmrt) |
| **AMS Enable** | オペレータごとに音量の揺れを受けるかどうか | [`OPM_OP[0-3]_AMS_EN`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-ams-en) |

### KEY SCALE（実機・独自）

音の高さによる変化です。実機の設定に加えて、**OPP のキースケール**も選べます
（**独自**）。


#### Mode の選択

オペレータごとに Mode を選びます。**Mode で出るつまみが変わります。**

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **Mode** | どのやり方で効かせるか。OPM / OPP | [`OPM_OP[0-3]_KS_MODE`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-ks-mode) |

#### Mode = OPM

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KS** | 実機のキースケール。OFF / 弱 / 中 / 強 | [`OPM_OP[0-3]_KS`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-ks) |

#### Mode = OPP（**独自**）

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KSR** | 音が高いほどエンベロープを速める | [`OPM_OP[0-3]_KSR_OPP`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-ksr-opp) |
| **KSL** | 音が高いほど音量を下げる | [`OPM_OP[0-3]_KSL_OPP`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-ksl-opp) |

### MASK（実機）

オペレータごとに音を止めます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MASK** | このオペレータを鳴らさない | [`OPM_OP[0-3]_MASK`](/2686V_2686VFX/reference/automation/opm/#opm-op-0-3-mask) |

### PAN（実機）

左右の定位です。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **PAN** | 定位。−1 が左、0 が中央、+1 が右 | [`OPM_PAN`](/2686V_2686VFX/reference/automation/opm/#opm-pan) |

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

## MML 入力

OPM では `AR` `D1R` `D1L` `D2R` `RR` `TL` `MUL` `MASK` に加えて、
`DT1`（`DT` / `DTO`）と `DT2`（`DTT`）、`KS` を受け付けます。

書き方は [MML 入力](/2686V_2686VFX/reference/mml/) を参照してください。

## オートメーション

OPM が DAW へ出すパラメータの一覧は
[OPM のオートメーション](/2686V_2686VFX/reference/automation/opm/) に
あります。ID・型・範囲・初期値が並びます。
