---
title: トレモロ
description: トレモロ の設定
sidebar:
  order: 3
---

音量を周期的に揺らします。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **RATE** | 揺れの速さ（Hz） | 0.1 〜 20 | 5 | [`FX_TRM_RATE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-trm-rate) |
| **DPTH** | 揺れの深さ | 0 〜 1 | 1 | [`FX_TRM_DEPTH`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-trm-depth) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_TRM_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-trm-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

速く深く掛けると、音が切れているように聞こえます。**RATE** を曲の速さに合わせると、刻みとして使えます。

音源側にも音量を揺らす仕組み（[LFO](/2686V_2686VFX/chips/common/#lfo)）がありますが、そちらは 1 音ごと、こちらは出てきた音全体に掛かります。

## 使えるプラグイン

音源 5 つ（2686V / 2686VLight / 26V / 86V / OPZX7S）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
