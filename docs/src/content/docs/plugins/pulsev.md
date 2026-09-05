---
title: PULSEV
description: PULSEV の構成と使いどころ
sidebar:
  order: 12
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/PULSEV.png" alt="PULSEV ロゴ" width="128" height="128" />
	<figcaption>ロゴ</figcaption>
</figure>

**矩形波の 2 チャンネル**だけに絞ったものです。SSG と BEEP が
入っています。細い矩形波の音を 1 パートぶん差したいときに向きます。

## 収録しているチャンネル

| チャンネル | |
| --- | :---: |
| [SSG](/2686V_2686VFX/chips/ssg/) | ✔ |
| [BEEP](/2686V_2686VFX/chips/beep/) | ✔ |

持っていないのは OPNA / OPN / OPL / OPL3 / OPM / OPZX7 / WAVETABLE / WT2 / WTPLUS / RHYTHM / ADPCM です。

各チャンネルの中身は、それぞれのページを参照してください。どのチャンネルにも
同じ形で出てくる区分は [共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

## カーブ編集（ADV）

**使えます。**

詳しくは [カーブ編集](/2686V_2686VFX/chips/adv/) を参照してください。

## こんなときに

- SSG のベースやアルペジオを足したいとき
- ビープらしい素の矩形波がほしいとき

## ファイル

| 種類 | 拡張子 |
| --- | --- |
| プリセット | `.pulsev.json` |
| パラメータファイル | ほかのプラグインと共通 |

プリセットはプラグインごとに拡張子が違うので、`Presets` フォルダへまとめて
置いても、そのプラグインのものだけが一覧に出ます。パラメータファイルは
**プラグインをまたいで読めます**。

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。

:::note
チャンネルを絞ったぶん、**同時に何本も挿したときの負荷が軽くなります**。
全部入りが要るときは [2686V](/2686V_2686VFX/plugins/2686v/) を使ってください。
:::
