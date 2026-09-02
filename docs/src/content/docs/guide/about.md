---
title: 2686V ファミリーについて
description: 2686V ファミリーがどういうものかの説明
sidebar:
  order: 1
---

2686V ファミリーは、1980〜90 年代のパソコンやゲーム機に載っていた音源チップの鳴り方を
再現するソフトウェア音源とエフェクトの集合体です。VST3 とスタンドアロンの両方で動きます。

## 様々な方向性を鑑みた 6 つのプラグイン

ファミリーのプラグインのうち、 **2686V/2686VLight/26V/86V/OPZX7S の 5 つは音を作る音源**で、残る 1 つ **2686VFX は音を加工するエフェクト**です。
音源どうしは収録する音源の数だけが違い、共通の画面と操作は同じなので、この
マニュアルの大半はどれにも当てはまります。違いは
[プラグインの選び方](/2686V_2686VFX/plugins/compare/) にまとめてあります。

| | プラグイン | 種別 | 位置づけ |
| :---: | --- | --- | --- |
| <img src="/2686V_2686VFX/logos/2686V.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | [**2686V**](/2686V_2686VFX/plugins/2686v/) | 音源 | すべての音源を収録した本体 |
| <img src="/2686V_2686VFX/logos/2686VLight.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | [**2686VLight**](/2686V_2686VFX/plugins/2686vlight/) | 音源 | 2686V からカーブ編集を外した軽い版 |
| <img src="/2686V_2686VFX/logos/26V.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | [**26V**](/2686V_2686VFX/plugins/26v/) | 音源 | PC-9801-26(K) に構成を合わせた版 |
| <img src="/2686V_2686VFX/logos/86V.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | [**86V**](/2686V_2686VFX/plugins/86v/) | 音源 | PC-9801-86 に構成を合わせた版 |
| <img src="/2686V_2686VFX/logos/OPZX7S.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | [**OPZX7S**](/2686V_2686VFX/plugins/opzx7s/) | 音源 | OPZX7 音源に特化した版。カーブ編集は標準で付く |
| <img src="/2686V_2686VFX/logos/2686VFX.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | [**2686VFX**](/2686V_2686VFX/plugins/fx-plugin/) | エフェクト | 音を作らず、入ってきた音を加工するエフェクト |

それぞれのプラグインの構成と使いどころは、上の名前から個別のページへ
進めます。横に並べて見比べたいときは
[プラグインの選び方](/2686V_2686VFX/plugins/compare/) が向いています。

## 動作環境

- Windows 11 (x64 / ARM64)
- VST3 に対応した DAW、またはスタンドアロン

Linux と macOS はビルド設定を用意していますが、配布はしていません。

## 音の作り方の考え方

実機にある機能は、実機の仕様に合わせています。実機に無い拡張を足している
ところは、マニュアルの中で「独自」と書いて区別しています。実機の音を狙う
ときは独自の項目を触らない、という使い分けができます。

## 1 つのプラグインで鳴るのは 1 音源

音源を切り替えるタブはたくさんありますが、**同時に鳴るのはそのうち 1 つだけ**
です。複数の音源を重ねたいときは、DAW のトラックを分けてそれぞれにプラグイン
を挿します。

そのほうが音量や定位、エフェクトをトラックごとに扱えて融通が利く、という
考えでこうしています。

## 参考文献

- 「マシン語サウンドプログラミング」
  - 青山学／日高徹 著
  - アスキー出版局
  - 1994年

- 「PC-9801-86サウンドボード ユーザーズマニュアル」
  - 日本電気 編
  - 日本電気
  - 1993年

- 「Inside X68000」
  - 桒野雅彦 著
  - ソフトバンク
  - 1992年

- 「MA-5 オーサリングツール ユーザーマニュアル(ATS-SMAFPhraseL2版)」
  - ヤマハ株式会社 編
  - ヤマハ株式会社
  - 2006年

- 「OPL3 YMF262アプリケーションマニュアル」
  - ヤマハ株式会社 編
  - ヤマハ株式会社
  - 1994年

- 「PC-9801シリーズ N88-日本語BASIC(86)(Ver6.1) リファレンスマニュアル」
  - (株)アスキー テクライト編集部 編
  - 日本電気株式会社
  - 1989年

- 「PC-8801FE/MA2 N88-BASIC/N88-日本語BASIC リファレンスマニュアル」
  - 日本電気株式会社 編
  - 日本電気株式会社
  - 1988年

- 「別冊POPCOM・プログラムマガジン PCシリーズ FM音源サウンドコレクション」
  - 強矢邦生 著
  - 小学館
  - 1987年
  - ※国立国会図書館デジタルアーカイブスより参照

- YMF262のデータシート

- [Wikipedia: YAMAHA YM2414](https://en.wikipedia.org/wiki/Yamaha_YM2414)
- [YAMAHAのFM音源チップ、YMF825がMAKE市場向けに登場。Maker Faireで先行発売が決定だ！](https://www.dtmstation.com/archives/52000321.html)
- [MAMEのFM271部分](https://git.libretro.com/libretro/mame/-/blob/f3218cc336813f24adf720e7c48b544e7bfc2d0e/src/devices/sound/ymf271.cpp)
- [OPL3解析ドキュメント](https://moddingwiki.shikadi.net/wiki/OPL_chip)
- [YAMAHA V50の取扱説明書](https://jp.yamaha.com/support/manuals/index.html?l=ja&k=V50)
- [YAMAHA TX81Zの取扱説明書](https://jp.yamaha.com/products/music_production/synthesizers/tx81z/downloads.html#product-tabs)
- [Programmer's Guide to OPL3 FM Music Synthesizer](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/programmers-guide-to-opl3-fm-music-synthesizer-r447/)
- [DOC@FMP7](http://fmpdoc.fmp.jp/)
