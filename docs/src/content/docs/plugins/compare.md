---
title: プラグインの選び方
description: 5 つのプラグインの違い
sidebar:
  order: 1
---

このシリーズには 5 つのプラグインがあります。**4 つは音を作る音源**で、
残る 1 つ **2686VFX は音を加工するエフェクト**です。

## 音源の 4 つ

音源どうしは、**収録するタブの数だけが違います。** 共通の画面と操作は
すべて同じです。 FX は音源側ではタブではなく、右端に出しっぱなしの
ペインです（[画面の見かた](/2686V_2686VFX/guide/screen/)）。

| タブ | <img src="/2686V_2686VFX/logos/2686V.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />2686V | <img src="/2686V_2686VFX/logos/2686VLight.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />2686VLight | <img src="/2686V_2686VFX/logos/86V.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />86V | <img src="/2686V_2686VFX/logos/OPZX7S.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />OPZX7S |
| --- | :---: | :---: | :---: | :---: |
| OPNA | ✔ | ✔ | ✔ | — |
| OPN | ✔ | ✔ | — | — |
| OPL | ✔ | ✔ | — | — |
| OPL3 | ✔ | ✔ | — | — |
| OPM | ✔ | ✔ | — | — |
| OPZX7 | ✔ | ✔ | — | ✔ |
| SSG | ✔ | ✔ | ✔ | — |
| WAVETABLE | ✔ | ✔ | — | — |
| WT2 | ✔ | ✔ | — | — |
| WTPLUS | ✔ | ✔ | — | — |
| RHYTHM | ✔ | ✔ | ✔ (6 パッド) | — |
| ADPCM | ✔ | ✔ | ✔ (PCM) | — |
| BEEP | ✔ | ✔ | — | — |
| FX (ペイン) | ✔ | ✔ | ✔ | ✔ |
| CURVE | ✔ | — | — | ✔ |
| PRESET / SETTINGS / COLORS / ABOUT | ✔ | ✔ | ✔ | ✔ |

### 選び方の目安

- **迷ったら 2686V** — すべて入っています
- **2686VLight** — 2686V からカーブ編集を外したものです。カーブを使わないなら
  こちらのほうが軽く動きます
- **86V** — PC-9801-86 に構成を合わせてあります。ADPCM は **PCM** という名前
  になり、リズムのパッドは 8 つではなく **6 つ**です。当時の構成で作りたいとき
  に、余計な選択肢が出ません
- **OPZX7S** — OPZX7 に集中したいとき。**カーブ編集は標準で付きます**

## エフェクトの 1 つ

**2686VFX** だけは毛色が違います。音を作らず、**入ってきた音を加工します。**
音源に付いている FX タブを取り出して、単体で使えるようにしたものです。

| | 音源の 4 つ | <img src="/2686V_2686VFX/logos/2686VFX.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />2686VFX |
| --- | :---: | :---: |
| 種類 | 音源 (シンセ) | エフェクト |
| 音声入力 | なし | ステレオ |
| 音源タブ | あり | なし |
| FX | 8 種類 | **9 種類** |
| 出力への変調 | — | ✔ |
| PRESET タブ | ✔ | — |

詳しくは [2686VFX (エフェクト)](/2686V_2686VFX/plugins/fx-plugin/) を
参照してください。

:::note
プリセットの拡張子はプラグインごとに違うので、別のプラグインのプリセットは
一覧に出ません。ただし **FX の順番とパラメータのファイルは共通**で、音源と
2686VFX のあいだで行き来できます。詳しくは
[ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
:::
