---
title: SFCエコー
description: SFCエコー の設定
sidebar:
  order: 9
---

スーパーファミコンの音源が持っていたエコーを再現したものです。ディレイに**8 つの係数のフィルター**が付いた作りで、繰り返すたびに音色が変わります。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **TIME** | 遅らせる時間（ミリ秒） | 0 〜 240 | 0 | [`FX_SFCECHO_TIME`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-time) |
| **FB** | 戻す量。**負の値**にすると位相が反転する | -1 〜 1 | 0 | [`FX_SFCECHO_FB`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fb) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_SFCECHO_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

### FIR 係数

繰り返す音に掛かるフィルターです。8 つの係数で形を決めます。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **FC0** | 0 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC1** | 1 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC2** | 2 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC3** | 3 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC4** | 4 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC5** | 5 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC6** | 6 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC7** | 7 番目の係数 | -128 〜 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |

## 使いどころ

**FB** に負の値を入れられるのが実機ゆずりです。繰り返しごとに上下が入れ替わり、独特のうねりが出ます。

**FC0〜FC7** は、繰り返す音に掛かるフィルターの係数です。FC0 だけを大きくすると素直な繰り返しに、交互に正負を置くと高いところが強調されます。

## 使えるプラグイン

音源 5 つ（2686V / 2686VLight / 26V / 86V / OPZX7S）と 2686VFX のすべてにあります。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
