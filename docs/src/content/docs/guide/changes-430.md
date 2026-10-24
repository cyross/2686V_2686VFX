---
title: v4.3.0 の変更点
description: 4.3.0 で足したものと変わったところ
sidebar:
  order: 0
---

4.2.0 から 4.3.0 までの変更をまとめます。4.3.0 は、TAP SEQUENCER と 256 段のエンベロープで
**動かせるパラメータを広げた**版です。

## FM 音源のオペレーターの列の並びを変えました

KEY SCALE / MUL/DET の列（FIX や LFO を含む列）を、**AMP ENV / OPTIONAL の列のすぐ右**へ移しました。
音色を作るときに一緒に触ることが多いためです。OPZX7 は WAVEFORM の列より左に来ます。

OPNA / OPN / OPL / OPL3 / OPM / OPZX7 のタブで、FM 音源を持つ全プラグインが対象です。

## スライダーのドラッグに Alt を足しました

ラベルのドラッグと、グラフの拡大率のつまみのドラッグで、**Alt** を押しているあいだ値が **100 刻み**へ
そろいます。範囲が 100 より狭いスライダーでは何もしません。全プラグイン（2686VFX を含む）が対象です。

| キー | そろえる刻み |
| --- | --- |
| **Shift** | 0.1（実数のスライダーだけ） |
| **Ctrl** | 1 |
| **Ctrl + Shift** | 10 |
| **Alt** | 100（4.3.0 から） |

## TAP PARAM SEQUENCER を足しました

[[TAP SEQUENCER]] に 3 つ目の区分 **TAP PARAM SEQUENCER** を足しました。鍵を押すたびに段を 1 つ進め、
その段に入れた **(パラメータ, 値) の組**で、パラメータを**その音のあいだ**置き換えます。独自拡張として、
1 つの段に組を**最大 16 個**入れられます。FM 音源はオペレーターごとの、RHYTHM はパッドごとのパラメータも選べます。

選べるのは次のものです。オン / オフのトグルは選べません。

- FM 音源のオペレーターの数値のパラメータ（TL / AR / DR / SR / SL / RR / KS / MUL / DT など）と FIX の周波数
- FM 音源のチャンネルの ALG / FB
- WT PITCH MOD / WT AMP MOD の波形と速さ
- WT+ の波形、ADPCM+ の音声（スロット）
- SSG のデューティ比と三角波の頂点
- FIX の周波数、各種の周波数・速さ（LFO・SSG HW ENV の周期・ノイズ・SPEED・TIMER CLOCK・ARP FREQ など）

段の進め方（LOOP・SKIP・INCREMENT / DECREMENT / CLEAR STEP・打鍵 1 回につき 1 段）は TAP AMP / PITCH SEQUENCER と
同じです。くわしくは [TAP PARAM SEQUENCER](/2686V_2686VFX/chips/common/#tap-param-sequencer) を参照してください。

## PARAM ENV[256] を足しました（TL ENV はこの機能にまとめました）

SSG SW AMP ENV[256] と同じ 256 段のエンベロープで、**TARGET** で選んだパラメータを動かす **PARAM ENV[256]** を
足しました。選べるパラメータは TAP PARAM SEQUENCER と同じです。

- チャンネルに 1 本（新しい大区分 **[[PARAM ENV]]**。[[TAP SEQUENCER]] の上）
- FM 音源はオペレーターごとにも 1 本（AMP ENV 区分のすぐ下。これまでの TL ENV の場所）
- 値は指し先のパラメータの範囲の割合で持ち、画面には指し先の書き方で出します
- 整数や選択肢のパラメータは、段のあいだ値を保ちます

**FM 音源のオペレーターの TL ENV は、PARAM ENV の指し先 TL になりました。** 4.2.x までのプリセット・DAW の状態・
チャンネルパラメータ・区分のパラメータファイル（`.tlEnv`）は、読み込むときに PARAM ENV（TARGET が TL）へ写すので、
同じように鳴ります。パラメータの ID は `_TLENV_*` から `_PRMENV_*` に変わったため、DAW で TL ENV を
オートメーションしていたときは割り当て直してください。

くわしくは [PARAM ENV\[256\]](/2686V_2686VFX/chips/common/#param-env256) を参照してください。

## 簡易表示モードで隠すものを足しました

PARAM ENV[256] と TAP PARAM SEQUENCER は、簡易表示モードで隠れます。SETTINGS の**簡易表示モードカスタマイズ**に
「PARAM ENV[256]」と「TAP PARAM SEQUENCER」を足したので、出したままにもできます。**[非表示中の区分をバイパス]** でも
切れます。

## ADPCM+ のスロットをすべて符号化しておくようにしました

TAP PARAM SEQUENCER / PARAM ENV で ADPCM+ の VOICE を変えると、選んでいないスロットも鳴ります。そのため、
読み込んであるスロットは QUALITY を変えたときにすべて符号化し直すようにしました。
