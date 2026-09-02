---
title: PCMビットクラッシャー
description: PCMビットクラッシャー の設定
sidebar:
  order: 6
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_pcmbc.png" alt="PCMビットクラッシャー" style="width:400px;" />
	<figcaption>PCMビットクラッシャー</figcaption>
</figure>

音源の [QUALITY(PCM)](/2686V_2686VFX/chips/common/#qualitypcm) と同じ刻みで粗くします。**2686VFX だけ**の効果です。

## つまみ

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **BIT** | ビット数と圧縮方式。12 段 | 1 〜 12 | 12 | [`FX_PCMBC_BITS`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-bits) |
| **RATE** | サンプリング周波数。15 段 | 1 〜 15 | 9 | [`FX_PCMBC_RATE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-rate) |
| **INTP** | 読み戻すときの補間のしかた。7 種 | 0 〜 6 | 1 | [`FX_PCMBC_INTP`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-intp) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_PCMBC_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-mix) |

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

ビットクラッシャーが連続した値で決めるのに対し、こちらは**実機と同じ段**で決めます。当時の機種の質感を狙うときは、その機種が使っていた方式を選びます。

**RATE** を下げて **INTP** を Nearest にすると、いちばん当時らしい粗さになります。逆に B-Spline はこもるので、遠くで鳴っている感じになります。

選べるものの一覧は [QUALITY 一覧](/2686V_2686VFX/reference/lists-quality/) にあります。

## 使えるプラグイン

**2686VFX だけ**です。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
