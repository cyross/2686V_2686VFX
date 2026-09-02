---
title: このプラグインについて
description: 2686V シリーズがどういうものかの説明
sidebar:
  order: 1
---

2686V は、1980〜90 年代のパソコンやゲーム機に載っていた音源チップの鳴り方を
再現するソフトウェア音源です。VST3 とスタンドアロンの両方で動きます。

## 6 つのプラグイン

**5 つは音を作る音源**で、残る 1 つ **2686VFX は音を加工するエフェクト**です。
音源どうしは収録する音源の数だけが違い、共通の画面と操作は同じなので、この
マニュアルの大半はどれにも当てはまります。違いは
[プラグインの選び方](/2686V_2686VFX/plugins/compare/) にまとめてあります。

| | プラグイン | 位置づけ |
| :---: | --- | --- |
| <img src="/2686V_2686VFX/logos/2686V.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | **2686V** | すべての音源を収録した本体 |
| <img src="/2686V_2686VFX/logos/2686VLight.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | **2686VLight** | 2686V からカーブ編集を外した軽い版 |
| <img src="/2686V_2686VFX/logos/26V.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | **26V** | PC-9801-26(K) に構成を合わせた版 |
| <img src="/2686V_2686VFX/logos/86V.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | **86V** | PC-9801-86 に構成を合わせた版 |
| <img src="/2686V_2686VFX/logos/OPZX7S.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | **OPZX7S** | OPZX7 音源に特化した版。カーブ編集は標準で付く |
| <img src="/2686V_2686VFX/logos/2686VFX.png" alt="" width="28" height="28" style="vertical-align:middle;" /> | **2686VFX** | 音を作らず、入ってきた音を加工するエフェクト |

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
