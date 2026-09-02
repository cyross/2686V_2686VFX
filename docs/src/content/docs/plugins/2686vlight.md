---
title: 2686VLight
description: 2686VLight の構成と使いどころ
sidebar:
  order: 3
---

<img src="/2686V_2686VFX/logos/2686VLight.png" alt="" width="72" height="72" style="float:right;margin:0 0 1rem 1rem;" />

2686V から**カーブ編集（ADV）を外した**ものです。音源タブの構成は 2686V と
まったく同じで、そのぶん軽く動きます。

## プラグイン表示

<figure class="shot">
	<img src="/2686V_2686VFX/ui/plugin/2686VLight.png" alt="2686VLight プラグイン全景" width="1426" height="737" />
	<figcaption>プラグイン全景</figcaption>
</figure>

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/ui/about/2686VLight.png" alt="2686VLight ABOUTタブ" width="386" height="500" />
	<figcaption>ABOUTタブ</figcaption>
</figure>

## 収録しているチャンネル

| チャンネル | |
| --- | :---: |
| [OPNA](/2686V_2686VFX/chips/opna/) | ✔ |
| [OPN](/2686V_2686VFX/chips/opn/) | ✔ |
| [OPL](/2686V_2686VFX/chips/opl/) | ✔ |
| [OPL3](/2686V_2686VFX/chips/opl3/) | ✔ |
| [OPM](/2686V_2686VFX/chips/opm/) | ✔ |
| [OPZX7](/2686V_2686VFX/chips/opzx7/) | ✔ |
| [SSG](/2686V_2686VFX/chips/ssg/) | ✔ |
| [WAVETABLE](/2686V_2686VFX/chips/wt/) | ✔ |
| [WT2](/2686V_2686VFX/chips/wt2/) | ✔ |
| [WTPLUS](/2686V_2686VFX/chips/wtplus/) | ✔ |
| [RHYTHM](/2686V_2686VFX/chips/rhythm/) | ✔ |
| [ADPCM](/2686V_2686VFX/chips/adpcm/) | ✔ |
| [BEEP](/2686V_2686VFX/chips/beep/) | ✔ |


各チャンネルの中身は、それぞれのページを参照してください。どのチャンネルにも
同じ形で出てくる区分は [共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

## カーブ編集（ADV）

**ありません。** 負荷を下げるために外してあります。

カーブ編集を使いたいときは 2686V か OPZX7S を選んでください。

## こんなときに

- カーブ編集を使わないとき
- トラック数が多く、負荷を抑えたいとき

## ファイル

| 種類 | 拡張子 |
| --- | --- |
| プリセット | `.2686vl.json` |
| パラメータファイル | ほかのプラグインと共通 |

プリセットはプラグインごとに拡張子が違うので、`Presets` フォルダへまとめて
置いても、そのプラグインのものだけが一覧に出ます。パラメータファイルは
**プラグインをまたいで読めます**。

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。

:::note
パラメータファイルとプリセットは 2686V と同じ形です。カーブの設定だけは読み飛ばされます。
:::
