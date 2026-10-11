---
title: ノイズリダクション
description: ノイズリダクション の設定
sidebar:
  order: 7
---

音源の [QUALITY(PCM)](/2686V_2686VFX/chips/common/#qualitypcm) にあるノイズリダクションのうち、鳴らすときに掛ける **ゲート** と **高域カット** を、単独の効果にしたものです。**2686VFX だけ**の効果です（3.6.3 から）。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **RATE** | LPF がどこで切るかの基準にするサンプリング周波数。15 段 | 1 〜 15 | 9（16kHz） | [`FX_NR_RATE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-nr-rate) |
| **RATE: Bypass** | RATE を通さない。入れると、LPF はホストのサンプリング周波数を基準にする | オン / オフ | オフ | [`FX_NR_RATE_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-nr-rate-bypass) |
| **GATE** | 音が **GATE.LV** より小さくなったところで閉じ、かすかな雑音を消す | オン / オフ | オフ | [`FX_NR_GATE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-nr-gate) |
| **GATE.LV** | ゲートが閉じる音量（dB） | -96 〜 -24 | -60 | [`FX_NR_GATE_LV`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-nr-gate-lv) |
| **LPF** | 高い音を切って、ざらつきを抑える。切 / 弱 / 中 / 強 | 1 〜 4 | 1（切） | [`FX_NR_LPF`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-nr-lpf) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_NR_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-nr-mix) |

中身は音源の QUALITY(PCM) と同じです。

- **GATE** は、開くのに 1 ミリ秒、閉じるのに 30 ミリ秒かけるので、音の頭や余韻は切れません。直流（中心のずれ）もあわせて取り除きます。**GATE.LV** は GATE が入っているときだけ押せます
- **LPF** は、**RATE** の帯域の上端（その半分の周波数）の 9 割・7 割・5 割から上を、弱・中・強の順に削ります。たとえば 16kHz で「中」なら、5.6kHz から上を削ります
- **RATE: Bypass** を入れると、RATE の代わりにホスト（DAW）のサンプリング周波数を基準にします。48kHz で「中」なら、16.8kHz から上を削ります。そのあいだ RATE は押せません

GATE も LPF も切れているときは、何もせずに通します。

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

[PCMビットクラッシャー](/2686V_2686VFX/fx/pcm-bitcrusher/) の**後ろ**に並べ、**RATE** をビットクラッシャーと同じにすると、音源の QUALITY(PCM) と同じ掛かり方になります。補間の折り返しや、粗い量子化のシャリシャリした感じが取れます。

間引く前に高域を切る **NR: Resample** は、間引きそのものと一緒でないと意味がないので、PCMビットクラッシャーの側にあります。

1-bit DPCM のように、音が止んでも出力が 0 へ戻らない素材には **GATE** が効きます。

## 使えるプラグイン

**2686VFX だけ**です。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
