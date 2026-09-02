---
title: 3バンドイコライザー
description: 3バンドイコライザー の設定
sidebar:
  order: 2
---

低い・真ん中・高いの 3 つに分けて、それぞれの量を上げ下げします。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **LGn** | 低いところの量（dB） | -15 〜 15 | 0 | [`FX_EQ3B_LGDB`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-eq3b-lgdb) |
| **MFq** | 真ん中とみなす高さ（Hz） | 500 〜 3000 | 2000 | [`FX_EQ3B_MFREQ`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-eq3b-mfreq) |
| **MGn** | 真ん中の量（dB） | -15 〜 15 | 0 | [`FX_EQ3B_MGDB`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-eq3b-mgdb) |
| **HGn** | 高いところの量（dB） | -15 〜 15 | 0 | [`FX_EQ3B_HGDB`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-eq3b-hgdb) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_EQ3B_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-eq3b-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

フィルターが「削る」のに対し、こちらは「増やす・減らす」です。音の芯を残したまま印象を変えたいときは、こちらのほうが向きます。

**MFq** を動かすと、同じ量でも当たるところが変わります。声の帯を持ち上げる、太鼓の胴を出す、といった使い分けができます。

## 使えるプラグイン

音源 5 つ（2686V / 2686VLight / 26V / 86V / OPZX7S）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
