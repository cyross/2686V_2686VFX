---
title: フィルター
description: フィルター の設定
sidebar:
  order: 1
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_filter.png" alt="フィルター" style="width:400px;" />
	<figcaption>フィルター</figcaption>
</figure>

決めた高さから上、または下を削ります。音の明るさを決める、いちばん基本の道具です。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **TYPE** | 削り方。ローパス / ハイパス / バンドパス | 1 〜 3 | 1 | [`FX_FIL_TYPE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-fil-type) |
| **FQ** | 境目の高さ（Hz） | 0 〜 20000 | 20000 | [`FX_FIL_FREQ`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-fil-freq) |
| **Q** | 境目の尖り方。大きいほど境目が強調される | 0.1 〜 10 | 0.71 | [`FX_FIL_Q`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-fil-q) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_FIL_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-fil-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

**FQ** を下げると、こもった、遠くで鳴っているような音になります。当時の小さなスピーカーの鳴りに寄せたいときに使います。

**Q** を上げてから **FQ** を動かすと、境目が鳴きます。オートメーションで動かすと、いわゆるフィルターの上げ下げになります。

## 使えるプラグイン

音源 11 本（2686V / 2686VLight / 26V / 86V / OPZX7S / OPNV / OPLV /
OPMV / WTV / PCMV / PULSEV）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
