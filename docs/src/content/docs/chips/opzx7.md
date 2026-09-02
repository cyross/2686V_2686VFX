---
title: OPZX7S
description: OPZ / OPX / MA-7 / OPS のいいところを集めた、独自の 8 オペレータ FM
sidebar:
  order: 6
---

OPZX7S は、**実機には存在しない独自のチャンネル**です。OPZ・OPX・MA-7・OPS の FM 音源部分の
それぞれから良いところを取り、そこへ独自の拡張を足したものです。

このシリーズで**最も自由度が高い**チャンネルで、専用プラグインの
[OPZX7S](/2686V_2686VFX/plugins/compare/) もこれ 1 つに絞ったものです。

:::note[実機の再現ではありません]
元になった音源には分かっていない部分も多く（とくに OPX）、**100% の再現は
できません。** 再現できるところは再現しつつ、足りないところは近い形で組んだ
実装です。
:::

## オペレータとアルゴリズム

| 項目 | 内容 |
| --- | --- |
| オペレータ数 | **最大 8** |
| アルゴリズム | OPX / OPZ / MA-7 / OPS のものを選択 ＋ 独自の 1OP〜8OP |
| フィードバック | **オペレータごとに独立**（実数、-8.0〜8.0） |

フィードバックが**オペレータごとに実数で**設定できるのは、元になったどの音源
にもない点です。負の値も入れられます。

詳細は、[リファレンス](../reference/lists-algorithm.md#opzx7s) をご参照ください。

### アルゴリズムマトリックス

決まったアルゴリズムから選ぶ代わりに、**変調とフィードバックの経路を
チェックボックスで自分で組めます。**

- 1OP から 8OP まで、どのオペレータがどこへ掛かるかを直接指定
- 従来の仕様ではあり得ない結線もできます

アルゴリズムの図は画面にリアルタイムで描かれるので、組んだ結線がそのまま
確認できます。

## 画面の区分

### ALGORITHM / FEEDBACK

アルゴリズムの選択、マトリックス、オペレータごとのフィードバック。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ALG MODE** | 組み込みアルゴリズムを使うか、マトリックスで組むかを選ぶ（オートメーションには出ません） | — |
| **ALG** | 組み込みアルゴリズム。130 種 | [`OPZX7_ALG`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-alg) |
| **FB** | **オペレータごと**のフィードバック。負の値も指定できる（**独自**） | [`OPZX7_FB[1-8]`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-fb-1-8) |

### WAVE SHAPE

**このチャンネルの要です。** オペレータが出す波形を選びます。

| 種類 | 内容 |
| --- | --- |
| 内蔵波形 | MA-7 / OPZ の波形 ＋ 独自波形 |
| 音声ファイル | 読み込んだ音を波形として使う（OPX の再現） |
| 波形メモリ | `.wt` / `.wt2` ファイル（MA-7 の再現） |

音声ファイルは**再生開始位置と再生割合**も決められます。読み込んだ波形は画面
で確認できます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **WS** | オペレータの波形。73 種。31 番が PCM、15 番が `.wt`、23 番が `.wt2` | [`OPZX7_OP[0-7]_WS`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ws) |
| **POFF** | PCM の頭出し（ミリ秒） | [`OPZX7_OP[0-7]_PCM_OFFSET`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-pcm-offset) |
| **PRT** | PCM の再生の速さ | [`OPZX7_OP[0-7]_PCM_RATIO`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-pcm-ratio) |
| **Loop Point Enable** | PCM の繰り返しを使う | [`OPZX7_OP[0-7]_LOOPPOINT_EN`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-looppoint-en) |
| **LPST / LPED** | 繰り返す範囲。全体を 0.0〜1.0 とした位置 | [`OPZX7_OP[0-7]_LOOPPOINT_ST`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-looppoint-st) |

### AMP ENV

オペレータごとの音量エンベロープです。**MA-7 準拠**で、切り替えると実数でも
設定できます。**KOR / XOF / Bypass** の独自オプションが付きます。


#### Register Mode の切り替え

**Register Mode** で、実機と同じ段で決めるか、秒とレベルで直に決めるかを切り替えます。つまみの名前は MA-7 にならって、どちらも **AR / DR / SL / SR / RR / TL** です。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **Register Mode** | オンでレジスタの段、オフで秒とレベルで決める | [`OPZX7_OP[0-7]_R_EN`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-en) |

#### Register Mode = オン（レジスタの段）

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | アタックレート | [`OPZX7_OP[0-7]_R_AR`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-ar) |
| **DR** | ディケイレート | [`OPZX7_OP[0-7]_R_D1R`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-d1r) |
| **SL** | サステインレベル | [`OPZX7_OP[0-7]_R_D1L`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-d1l) |
| **SR** | サステインレート | [`OPZX7_OP[0-7]_R_D2R`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-d2r) |
| **RR** | リリースレート | [`OPZX7_OP[0-7]_R_RR`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-rr) |
| **TL** | トータルレベル。**大きいほど音が小さくなる** | [`OPZX7_OP[0-7]_R_TL`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-r-tl) |

#### Register Mode = オフ（秒とレベル）

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **AR** | 立ち上がりの時間（秒） | [`OPZX7_OP[0-7]_AR`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ar) |
| **DR** | 減衰の時間（秒） | [`OPZX7_OP[0-7]_D[1-2]R`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-d-1-2-r) |
| **SL** | 下がりきる高さ | [`OPZX7_OP[0-7]_D1L`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-d1l) |
| **SR** | 押しているあいだ減り続ける時間（秒） | [`OPZX7_OP[0-7]_D[1-2]R`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-d-1-2-r) |
| **RR** | 余韻の時間（秒） | [`OPZX7_OP[0-7]_RR`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-rr) |
| **TL** | 出力の大きさ。**大きいほど音が大きくなる** | [`OPZX7_OP[0-7]_TL`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-tl) |

#### どちらのモードでも使うもの

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SUS** | キーオフのあとも音を保つ | [`OPZX7_OP[0-7]_SUS`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-sus) |
| **XOF** | RR を無視して減衰させない（**独自**） | [`OPZX7_OP[0-7]_XOF`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-xof) |
| **KOR** | 鍵を離しても、指定の時間まで減衰させない（**独自**） | [`OPZX7_OP[0-7]_KOR`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-kor) |
| **Bypass** | このオペレータの音量エンベロープを通さない（**独自**） | [`OPZX7_OP[0-7]_AMP_BYPASS`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-amp-bypass) |

### MUL / DET

**MUL** は MA-7 準拠の選択に加えて、OPZ の MulRatio を拡張したものが使えます。
上限が大きく引き上げてあります。

**DT** は OPZ の DT / DT2 に、自由に値を決められる **DT3** を足してあります。


[MUL/DET](/2686V_2686VFX/chips/common/#muldet) をオペレータごとに置いたものです。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MUL** | 逓倍。22 段 | [`OPZX7_OP[0-7]_MUL`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-mul) |
| **MURT** | 倍率を自由に決める（**独自**） | [`OPZX7_OP[0-7]_MUL_RATIO`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-mul-ratio) |
| **DT1 / DT2 / DT3** | デチューン。DT3 はセントで指定できる（**独自**） | [`OPZX7_OP[0-7]_DT`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-dt) |

### LFO

AM と PM で**周波数と同期を別々に**設定できます。掛かり具合の幅も広く取って
あります。**チャンネル単位とオペレータ単位の両方**で設定でき、独自波形も
選べます。

### SSG HW ENV

OPNA の SSG ハードウェアエンベロープをここでも使えます。独自波形と範囲指定
付きです。


オペレータごとに掛ける SSG のエンベロープです。チャンネル全体へ掛けるものは [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) にあります。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **ENV** | 形。Normal（掛けない）＋ 8 種類 | [`OPZX7_OP[0-7]_SE`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-se) |
| **FQ** | 繰り返しの速さ | [`OPZX7_OP[0-7]_SEFREQ`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-sefreq) |

### KEY SCALE

**MA-7 / OPZ / OPS** の設定をそれぞれ切り替えられます。


#### Mode の選択

オペレータごとに MODE を選びます。**MODE で出るつまみが変わります。**

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **Enable** | キースケールを使う | [`OPZX7_OP[0-7]_KS_EN`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ks-en) |
| **MODE** | どのやり方で効かせるか。MA-7 / OPZ / OPS | [`OPZX7_OP[0-7]_KS_MODE`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ks-mode) |

#### MODE = MA-7

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KSR** | 音が高いほどエンベロープを速める | [`OPZX7_OP[0-7]_KSR`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ksr) |
| **KSL** | 音が高いほど音量を下げる度合い。OFF / 1.5 / 3.0 / 6.0 dB/oct | [`OPZX7_OP[0-7]_KSL`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ksl) |

#### MODE = OPZ

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KSR** | 音が高いほどエンベロープを速める | [`OPZX7_OP[0-7]_KSR_OPZ`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ksr-opz) |
| **KSL** | 音が高いほど音量を下げる度合い | [`OPZX7_OP[0-7]_KSL_OPZ`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ksl-opz) |

#### MODE = OPS

OPS のやり方だけ**折れ点を持ちます**。ある音の高さを境に、左右で別々の曲げ方を指定できます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **KSBP** | 折れ点にする音の高さ（ノート番号） | [`OPZX7_OP[0-7]_KS_BP`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ks-bp) |
| **KSLC / KSRC** | 折れ点の左右で、どう曲げるか | [`OPZX7_OP[0-7]_KS_LC`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ks-lc) |
| **KSLD / KSRD** | 折れ点の左右で、どれだけ下げるか | [`OPZX7_OP[0-7]_KS_LD`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ks-ld) |
| **KSRS** | エンベロープの速さへの効き | [`OPZX7_OP[0-7]_KS_RS`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-ks-rs) |

### MASK / PANPOT / OPTIONAL

オペレータごとの音の止め、左右の定位、追加の動作です。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MASK** | このオペレータを鳴らさない | [`OPZX7_OP[0-7]_MASK`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-op-0-7-mask) |
| **Enable** | 定位を使う | [`OPZX7_PANPOT_EN`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-panpot-en) |
| **PNPT** | 定位。0 が左、15 が中央、31 が右 | [`OPZX7_PANPOT`](/2686V_2686VFX/reference/automation/opzx7/#opzx7-panpot) |

## オペレータごとのエンベロープ

このチャンネルは、オペレータ 1 つずつに次のエンベロープを持てます。

- SSG ソフトウェア音量エンベロープ（6 タップ / 11 タップ）
- ピッチエンベロープ（3 タップ / 11 タップ）

チップ全体に掛かるエンベロープとは別に動くので、**オペレータごとに違う動きを
させたうえで、全体をもう一度動かす**という重ね方ができます。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/chips/common/#quality) | ビット数とサンプリング周波数を落として質感を作る |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

## カーブ編集

2686V と OPZX7S では、**値の変化のしかた**を 30 種類以上から選べます。
**OPZX7S プラグインではカーブ編集が標準**で、従来の線形モードには戻せません。

詳しくは ADV タブを参照してください。

## MML 入力

OPM と同じ系統の記号を受け付けます。`AR` `D1R` `D1L` `D2R` `RR` `TL`
`MUL` `MASK` `DT1`（`DT` / `DTO`）`DT2`（`DTT`）。

書き方は [MML 入力](/2686V_2686VFX/reference/mml/) を参照してください。

## オートメーション

OPZX7S が DAW へ出すパラメータの一覧は
[OPZX7S のオートメーション](/2686V_2686VFX/reference/automation/opzx7/) に
あります。ID・型・範囲・初期値が並びます。
