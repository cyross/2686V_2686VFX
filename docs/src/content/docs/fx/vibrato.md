---
title: ビブラート
description: ビブラート の設定
sidebar:
  order: 4
---

音程を周期的に揺らします。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **RATE** | 揺れの速さ（Hz） | 0.1 〜 10 | 2 | [`FX_VIB_RATE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-vib-rate) |
| **DPTH** | 揺れの深さ | 0 〜 1 | 0.5 | [`FX_VIB_DEPTH`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-vib-depth) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_VIB_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-vib-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

浅く掛けると、テープの揺れのような不安定さが出ます。深く速く掛けると金属的になります。

**MIX** を半分あたりにすると、揺れていない音と混ざって厚みが出ます。

## 使えるプラグイン

音源 5 つ（2686V / 2686VLight / 26V / 86V / OPZX7S）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
