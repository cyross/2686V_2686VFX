---
title: ADPCM+チャンネル
description: 音声ファイルを 32 本並べて、切り替えながら鳴らすチャンネル
sidebar:
  order: 13
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_adpcmp.png" alt="ADPCM+チャンネル" style="width:600px;" />
	<figcaption>ADPCM+チャンネル</figcaption>
</figure>

ADPCM+ は、**音声ファイルを 32 本読み込んでおいて、演奏しながら切り替える**
ためのチャンネルです。3.3.0 で新しく足しました。

ADPCM が 1 本の素材を鍵盤で弾くものなのに対し、ADPCM+ は**並べた素材から
1 本を選んで鳴らします。** [WT+](/2686V_2686VFX/chips/wtplus/) が波形メモリ
に対してしていることを、そのまま PCM へ移したものだと考えてください。

ADPCM と大きく違うのは、**音の作り方が素材ごとに分かれている**ところです。
FORM・OPTIONAL・PAN の値は PCM 1 本ごとに持つので、素材を切り替えると
鳴らし方も一緒に切り替わります。

:::note[このチャンネルは独自です]
実機にそのまま対応するものはありません。当時の音源ドライバが、演奏中に
ADPCM の読み出し位置を差し替えて音色を変えていた動きを、扱いやすい形に
まとめたものです。
:::

## 表記について

| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## 画面の分かれ方

画面は 2 つのグループに分かれています。

| グループ | 中身 |
| --- | --- |
| **ADPCM+チャンネル設定** | チャンネル全体に効くもの。音量・エンベロープ・LFO・QUALITY など |
| **PCM設定** | PCM 1 本ごとに持つもの。FORM・OPTIONAL・PAN |

右の **PCM設定**が、このチャンネルの中心です。

## PCM設定 — 素材ごとの設定

### SLOT と TARGET

グループの先頭にある 2 つのつまみは、役目がはっきり違います。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SLOT** | **鳴らす** PCM の番号。0〜31 | [`ADPCMP_SLOT`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot) |
| **TARGET** | **いま画面へ出している** PCM の番号。0〜31 | — |

**SLOT は音に効きます。** オートメーションの対象なので、時間に沿って素材を
切り替える動きを DAW 側で描けます。

**TARGET は音に効きません。** 下に並ぶ FORM・OPTIONAL・PAN が、どの PCM の値を
映すかを決めるだけです。鳴らしているものとは別の PCM を、鳴らしたまま
編集できます。

:::tip[畳めない場所に置いてあります]
SLOT と TARGET は区分の外にあります。下の区分をすべて束ねるつまみなので、
畳んで隠れてしまうと選び直せなくなるためです。
:::

### FORM — 音声ファイルの設定

**Load** で、いま TARGET が指している枠へファイルを読み込みます。**X** でその枠を
空にします。

読み込んだ音は 2 通りの見え方をします。

- **上の 1 枚** … TARGET の PCM を、P.OFFSET / P.RATIO で切り出した範囲で描きます。
  Loop Point を使っているときは、その位置に縦線が入ります。
- **下の 32 枚** … 素材そのものを並べたものです。どの枠に何が入っているかが
  一目で分かります。**枠が付いているもの**が TARGET、**下地に色が敷かれて
  いるもの**が SLOT です。

そのあとに続く 4 つで、読み込んだ音へざらつきを混ぜられます（**独自**）。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **T.LEVEL** | 読み込んだ音の側の量 | [`ADPCMP_SLOT[0-31]_TONE`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-tone) |
| **N.LEVEL** | ざらつきの側の量 | [`ADPCMP_SLOT[0-31]_NOISE`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-noise) |
| **N.FREQ** | ざらつきの高さ | [`ADPCMP_SLOT[0-31]_NOISEFREQ`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-noisefreq) |
| **MIX** | 2 つの混ぜ具合 | [`ADPCMP_SLOT[0-31]_NOISEMIX`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-noisemix) |

### OPTIONAL — 鳴らし方

**LPST / LPED を波形の途中に置く**と、押している間そこだけを繰り返します。
持続する音を短い素材から作れます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SPEED** | 再生の速さに掛ける倍率 | [`ADPCMP_SLOT[0-31]_SPEED`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-speed) |
| **CNT** | LPST〜LPED を何周したら先へ進むか。0 は無限 | [`ADPCMP_SLOT[0-31]_LOOPPOINT_CNT`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-looppoint-cnt) |
| **P.OFFSET** | 頭出し（ミリ秒） | [`ADPCMP_SLOT[0-31]_PCM_OFFSET`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-pcm-offset) |
| **P.RATIO** | 鳴らす長さの割合 | [`ADPCMP_SLOT[0-31]_PCM_RATIO`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-pcm-ratio) |
| **LOOP** | ファイル全体を繰り返す | [`ADPCMP_SLOT[0-31]_LOOP`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-loop) |
| **Loop Point Enable** | 繰り返す範囲を指定する | [`ADPCMP_SLOT[0-31]_LOOPPOINT_EN`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-looppoint-en) |
| **LPST / LPED** | その範囲。全体を 0.0〜1.0 とした位置 | [`ADPCMP_SLOT[0-31]_LOOPPOINT_ST`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-looppoint-st) |

### PAN

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **PAN** | 定位。0 が左、0.5 が中央、1 が右 | [`ADPCMP_SLOT[0-31]_PAN`](/2686V_2686VFX/reference/automation/adpcmplus/#adpcmp-slot-0-31-pan) |

素材ごとに持つので、**並べた PCM をそのまま左右へ散らせます。**

## チャンネル全体の設定

### FIX — 音程の固定

鍵盤の音程を無視して、決まった高さで鳴らします。打楽器のように、どの鍵を
押しても同じ高さで鳴ってほしいときに使います。

つまみは [FIX](/2686V_2686VFX/chips/common/#fix) を参照してください。

### QUALITY(PCM)

ビット数・記録方式・サンプリング周波数は、**チャンネル全体で 1 つ**です。
実機でいうチップ側の設定にあたるもので、素材ごとには分かれません。

## 使いかたの例

:::tip[並べた素材を切り替える]

1. 同じ楽器の強さ違いを、弱い順に読み込む
2. SLOT をオートメーションで動かす
3. 1 つの音の中で、ぶつかり方だけが変わる

素材ごとに P.OFFSET や PAN も変えておくと、切り替えの印象がさらに変わります。
:::

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY(PCM)**](/2686V_2686VFX/chips/common/#qualitypcm) | ビット数・記録方式・サンプリング周波数と、読み戻すときの補間 |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**WT AMP MOD**](/2686V_2686VFX/chips/common/#wt-amp-mod) | 波形メモリによる音量の変調 |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 音量・音程を周期的に揺らす |
| [**MUL/DET**](/2686V_2686VFX/chips/common/#muldet) | 音程をずらす |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

QUALITYの詳細は、[リファレンス](/2686V_2686VFX/reference/lists-quality/) をご参照ください。

## ファイルの扱い

チャンネルのパラメータは `.param.adpcmplus` です。**32 本ぶんの設定と、
それぞれが読んでいるファイルの場所**が入ります。ADPCM の `.param.adpcm` とは
別のものなので、取り違えることはありません。

素材そのものはファイルへ入りません。場所だけを覚えて、読み直すときに
そこから読みます。詳しくは [ファイル構成](/2686V_2686VFX/files/format/) を
参照してください。

## 関連するページ

- [ADPCM](/2686V_2686VFX/chips/adpcm/) — 素材を 1 本だけ鳴らす
- [WT+](/2686V_2686VFX/chips/wtplus/) — 同じ造りの波形メモリ版
- [RHYTHM](/2686V_2686VFX/chips/rhythm/) — 素材を鍵盤へ割り当てて同時に鳴らす

## オートメーション

ADPCM+ が DAW へ出すパラメータの一覧は
[ADPCM+ のオートメーション](/2686V_2686VFX/reference/automation/adpcmplus/) に
あります。ID・型・範囲・初期値が並びます。
