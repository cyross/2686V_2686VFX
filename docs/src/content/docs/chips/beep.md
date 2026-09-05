---
title: BEEPチャンネル
description: パソコン本体のビープ音を鳴らすチャンネル
sidebar:
  order: 13
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_beep.png" alt="BEEPチャンネル" style="width:600px;" />
	<figcaption>BEEPチャンネル</figcaption>
</figure>

BEEP は、**パソコン本体のビープ音**を鳴らすチャンネルです。音源ボードを積んで
いない機械が唯一出せた音で、デューティ比の固定された矩形波が 1 つ鳴るだけの
素朴なものです。

音色そのものは変えられませんが、**エンベロープと変調はほかのチャンネルと同じ
だけ揃っています。** 単純な矩形波を細かく動かすことで、当時のプログラムが
やっていた表現をなぞれます。

## 表記について

| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## 音の設定

| 項目 | 内容 | 範囲 |
| --- | --- | --- |
| **FREQ** | 周波数 | 0〜8000Hz（初期 440Hz） |
| **TIMERCLOCK** | もとになるタイマーの刻み | 1〜4（初期 1） |
| **ANTIALIAS** | 折り返しノイズを抑える（**独自**） | 入 / 切（初期は切） |

**TIMERCLOCK** は、実機がビープの周波数をどう作っていたかに対応します。刻みが
粗いほど、狙った周波数からずれます。**そのずれ方が当時らしさになる**ので、
実機に寄せたいときは触ってみてください。

**ANTIALIAS を入れると角が丸まり、今風の聞きやすい矩形波**になります。実機の
音を狙うなら切ったままにします。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **CLK** | もとにするクロック。実機の 4 種 | [`BEEP_TIMERCLOCK`](/2686V_2686VFX/reference/automation/beep/#beep-timerclock) |
| **Anti Alias** | 高い音の折り返しを抑える（**独自**） | [`BEEP_ANTIALIAS`](/2686V_2686VFX/reference/automation/beep/#beep-antialias) |

## OPTIONAL — 効果音モード（実機）

効果音モードには **2000Hz へ切り替えるボタン**があります。当時のプログラムが
警告音によく使っていた高さです。

短い音を出したいときは、ピッチエンベロープを深く掛けると、レーザーや爆発の
ような音になります。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

音色は単純ですが、**動かす仕組みはひととおり揃っています。**

| 区分 | 内容 |
| --- | --- |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/chips/common/#wt-pitch-mod) | 波形メモリによる音程の変調 |
| [**WT AMP MOD**](/2686V_2686VFX/chips/common/#wt-amp-mod) | 波形メモリによる音量の変調 |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 音量・音程を周期的に揺らす |
| [**MUL/DET**](/2686V_2686VFX/chips/common/#muldet) | 音程をずらす |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

:::tip[素朴な音ほど動かしがい]
BEEP は元の音が単純なぶん、**エンベロープの効果がそのまま聞こえます。**
SSG ソフトウェアエンベロープで細かく刻む、ピッチエンベロープで落とす、と
いった動きを確かめるのに向いています。

エンベロープの仕組みを覚えるための練習台としても使えます。
:::

## オートメーション

BEEP が DAW へ出すパラメータの一覧は
[BEEP のオートメーション](/2686V_2686VFX/reference/automation/beep/) に
あります。ID・型・範囲・初期値が並びます。
