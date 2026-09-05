---
title: ディレイ
description: ディレイ の設定
sidebar:
  order: 7
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_delay.png" alt="ディレイ" style="width:400px;" />
	<figcaption>ディレイ</figcaption>
</figure>

音を遅らせて繰り返します。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **TIME** | 遅らせる時間（ミリ秒） | 10 〜 1000 | 375 | [`FX_DLY_TIME`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-dly-time) |
| **FB** | 戻す量。大きいほど長く繰り返す | 0 〜 0.95 | 0.4 | [`FX_DLY_FB`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-dly-fb) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_DLY_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-dly-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

**TIME** を曲の速さに合わせると、リズムとして噛み合います。四分音符なら `60000 ÷ BPM` ミリ秒です。BPM 120 なら 500、八分音符なら 250 になります。

**FB** は 0.95 までです。それ以上にすると音が発散して止まらなくなるため、上限を設けてあります。

## 使えるプラグイン

音源 11 本（2686V / 2686VLight / 26V / 86V / OPZX7S / OPNV / OPLV /
OPMV / WTV / PCMV / PULSEV）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
