---
title: ビットクラッシャー
description: ビットクラッシャー の設定
sidebar:
  order: 5
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_mbc.png" alt="ビットクラッシャー" style="width:400px;" />
	<figcaption>ビットクラッシャー</figcaption>
</figure>

ビット数とサンプリング周波数を落として、粗さを作ります。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **BIT** | ビット数。小さいほど粗い | 2 〜 24 | 24 | [`FX_MBC_BITS`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-mbc-bits) |
| **RATE** | 間引く割合。大きいほど粗い | 1 〜 50 | 1 | [`FX_MBC_RATE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-mbc-rate) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_MBC_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-mbc-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

**BIT** を下げると、小さい音のところがざらつきます。**RATE** を上げると、高いところが折り返して金属的な倍音が出ます。

どちらも連続した値で決められます。実機の刻みに合わせたいときは [PCMビットクラッシャー](/2686V_2686VFX/fx/pcm-bitcrusher/) のほうを使います。

## 使えるプラグイン

音源 11 本（2686V / 2686VLight / 26V / 86V / OPZX7S / OPNV / OPLV /
OPMV / WTV / PCMV / PULSEV）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
