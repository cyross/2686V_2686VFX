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
| **INTERP** | 読み戻すときの補間のしかた。7 種 | 0 〜 6 | 1 | [`FX_PCMBC_INTP`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-intp) |
| **NR: Resample** | 間引く前に高い音を切って、折り返しを防ぐ（3.6.3 から） | オン / オフ | オフ | [`FX_PCMBC_NR_RESAMPLE`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-nr-resample) |
| **MIX** | 原音との混ぜ具合 | 0 〜 1 | 0 | [`FX_PCMBC_MIX`](/2686V_2686VFX/reference/automation/fx-plugin/#fx-pcmbc-mix) |

**DAC** の行で機種を選んで **適応** を押すと、BIT・RATE・INTERP をその機種に合わせてまとめて切り替えます（3.6.3 から）。選んだ機種は保存しません。

**NR: Resample** を入れると、間引く前に **RATE** の帯域の上端（その半分の周波数）の少し手前で切ります。切らないと、表せない高い音が低いところへ折り返して濁ります。音源の QUALITY(PCM) の NR: Resample と同じ考え方で、流れてくる音向けに遅れの無いフィルタで切ります。ゲートや高域カットは [ノイズリダクション](/2686V_2686VFX/fx/noise-reduction/) にあります。

**MIX** は原音と効果音の混ぜ具合です。0.0 で原音のまま、1.0 で効果だけになります。

**バイパス**を入れると、その効果を通しません。初めは入った状態で、MIX も 0.0 です。外して MIX を上げてから、つまみを触ってください。

## 使いどころ

ビットクラッシャーが連続した値で決めるのに対し、こちらは**実機と同じ段**で決めます。当時の機種の質感を狙うときは、その機種が使っていた方式を選びます。

**RATE** を下げて **INTERP** を Nearest にすると、いちばん当時らしい粗さになります。逆に B-Spline はこもるので、遠くで鳴っている感じになります。

選べるものの一覧は [QUALITY 一覧](/2686V_2686VFX/reference/lists-quality/) にあります。

## 使えるプラグイン

**2686VFX だけ**です。

順番の変え方は [FX について](/2686V_2686VFX/fx/) を参照してください。
