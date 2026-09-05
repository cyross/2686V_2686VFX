---
title: PCMV
description: PCMV の構成と使いどころ
sidebar:
  order: 11
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/PCMV.png" alt="PCMV ロゴ" width="128" height="128" />
	<figcaption>ロゴ</figcaption>
</figure>

**録った音を鳴らす 2 チャンネル**だけに絞ったものです。RHYTHM と
ADPCM が入っています。リズムとサンプル再生をまとめて 1 本で
受け持てます。

## プラグイン表示

<figure class="shot">
	<img src="/2686V_2686VFX/ui/plugin/PCMV.png" alt="PCMV プラグイン全景" width="1426" height="736" />
	<figcaption>プラグイン全景</figcaption>
</figure>

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/ui/about/PCMV.png" alt="PCMV ABOUTタブ" width="384" height="497" />
	<figcaption>ABOUTタブ</figcaption>
</figure>

## 収録しているチャンネル

| チャンネル | |
| --- | :---: |
| [RHYTHM](/2686V_2686VFX/chips/rhythm/) | ✔ |
| [ADPCM](/2686V_2686VFX/chips/adpcm/) | ✔ |

持っていないのは OPNA / OPN / OPL / OPL3 / OPM / OPZX7 / SSG / WAVETABLE / WT2 / WTPLUS / BEEP です。

各チャンネルの中身は、それぞれのページを参照してください。どのチャンネルにも
同じ形で出てくる区分は [共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

## カーブ編集（ADV）

**使えます。**

詳しくは [カーブ編集](/2686V_2686VFX/chips/adv/) を参照してください。

## こんなときに

- リズムパートを 1 本で用意したいとき
- 読み込んだ音をそのまま鳴らしたいとき

## ファイル

| 種類 | 拡張子 |
| --- | --- |
| プリセット | `.pcmv.json` |
| パラメータファイル | ほかのプラグインと共通 |

プリセットはプラグインごとに拡張子が違うので、`Presets` フォルダへまとめて
置いても、そのプラグインのものだけが一覧に出ます。パラメータファイルは
**プラグインをまたいで読めます**。

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。

:::note
チャンネルを絞ったぶん、**同時に何本も挿したときの負荷が軽くなります**。
全部入りが要るときは [2686V](/2686V_2686VFX/plugins/2686v/) を使ってください。
:::
