---
title: WT+チャンネル
description: 読み込んだ波形を切り替えながら鳴らすチャンネル
sidebar:
  order: 10
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_wtp.png" alt="WT+チャンネル" style="width:600px;" />
	<figcaption>WT+チャンネル</figcaption>
</figure>

WT+ は、**すでにある波形ファイルを並べておいて、演奏しながら切り替える**ための
チャンネルです。3.0.0 で新しく足しました。

WT や WT2 が「ひと山を描く」ためのものなのに対し、WT+ は「描いたものを**使う**」
ためのものです。波形を切り替えると音色がその場で変わるので、1 つの音の中で
音色が移り変わっていく、といった鳴り方を作れます。

:::note[このチャンネルは独自です]
実機にそのまま対応するものはありません。当時の音源ドライバが、演奏中に波形
メモリを書き換えて音色を変えていた動きを、扱いやすい形にまとめたものです。
:::

## SLOT — 波形の並び

**最大 32 枚**の波形を読み込んでおけます。`.wt` と `.wt2` のどちらも使えます。

| 項目 | 内容 | 範囲 |
| --- | --- | --- |
| **SLOT** | いま鳴らしている波形の番号 | 0〜31 |
| **WT** / **W2** | その枠へ `.wt` / `.wt2` を読み込む | — |
| **Clear** | その枠を空にする | — |

読み込んだ波形は画面で確認できます。どの枠に何が入っているか、目で見て分かり
ます。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **SLOT** | 鳴らす枠。0〜31。オートメーションから切り替えられる | [`WTPLUS_SLOT`](/2686V_2686VFX/reference/automation/wtplus/#wtplus-slot) |
| **Interpolate** | 枠の切り替わりをつなぐ | [`WTPLUS_INTERPOLATE`](/2686V_2686VFX/reference/automation/wtplus/#wtplus-interpolate) |
| **STEP** | 描くときの値の刻み | [`WTPLUS_STEPS`](/2686V_2686VFX/reference/automation/wtplus/#wtplus-steps) |

## 切り替え方

**SLOT のスライダーを動かすと、その場で音色が変わります。**

いちばんの使いどころは **DAW のオートメーション**です。SLOT もオートメーション
の対象なので、時間に沿って波形を切り替える動きを DAW 側で描けます。

:::tip[使いかたの例]

1. 明るい波形から暗い波形まで、順に並べて読み込む
2. SLOT をオートメーションでゆっくり動かす
3. 音を伸ばしたまま音色だけが移り変わる

順番を工夫すると、フィルターとは違う独特の変化になります。
:::

## WAVE MEMORY — 鳴らし方の設定

| 項目 | 内容 | 範囲 |
| --- | --- | --- |
| **STEP** | 段階の数を指定して丸める | 0〜10 |
| **Interpolate** | 点と点のあいだをなめらかにつなぐ | 入 / 切（初期は入） |

読み込んだ波形の細かさに関わらず、ここで粗さをそろえられます。**Interpolate を
切ると階段のまま鳴ります。**

## MOD — 波形メモリのピッチ変調（実機・独自）

WT / WT2 と同じ変調が使えます。FDS / WonderSwan / HuC6280 の 3 系統です。

| 項目 | 内容 | 範囲 |
| --- | --- | --- |
| **Enable** | 変調を使うか | 入 / 切 |
| **DEPTH** | 掛かり具合 | 0.1〜1.0 |
| **SPEED** | 速さ。搬送波の周波数に対する比 | 0.1〜10.0 |
| **SHAPE** | 変調のしかた | 9 種類 |

詳しくは [WT の MOD](/2686V_2686VFX/chips/wt/#mod--波形メモリのピッチ変調実機独自)
を参照してください。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/chips/common/#quality) | ビット数とサンプリング周波数を落として質感を作る |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 音量・音程を周期的に揺らす（OPZX7S のものを使用） |
| [**MUL/DET**](/2686V_2686VFX/chips/common/#muldet) | 音程をずらす |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き。**Broadcast Level** で他チャンネルへ音量を配れます |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

## 関連するページ

- [WT](/2686V_2686VFX/chips/wt/) — 波形を実数で描く
- [WT2](/2686V_2686VFX/chips/wt2/) — 波形を段階で描く

## オートメーション

WT+ が DAW へ出すパラメータの一覧は
[WT+ のオートメーション](/2686V_2686VFX/reference/automation/wtplus/) に
あります。ID・型・範囲・初期値が並びます。
