---
title: リバーブ
description: リバーブ の設定
sidebar:
  order: 8
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_reverb.png" alt="リバーブ" style="width:400px;" />
	<figcaption>リバーブ</figcaption>
</figure>

広さの響きを足します。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **SIZE** | 広さ。大きいほど長く残る | 0 〜 1 | 0.5 | [`FX_RVB_SIZE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-rvb-size) |
| **DAMP** | 高いところの減り方。大きいほど早く落ちる | 0 〜 1 | 0.5 | [`FX_RVB_DAMP`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-rvb-damp) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_RVB_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-rvb-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

**DAMP** を上げると、響きの高いところが早く消えて、柔らかい残り方になります。下げると硬い響きになります。

チップの音は素のままだと平べったく聞こえます。**MIX** を 0.2 あたりまで足すだけでも、まとまりが出ます。

## 使えるプラグイン

音源 5 つ（2686V / 2686VLight / 26V / 86V / OPZX7S）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
