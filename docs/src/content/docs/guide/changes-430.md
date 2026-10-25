---
title: v4.3.0 の変更点
description: 4.3.0 で足したものと変わったところ
sidebar:
  order: 0
---

4.2.0 から 4.3.0 までの変更をまとめます。4.3.0 は、実機の**出力の癖**をなぞる機能を足し、
TAP SEQUENCER と 256 段のエンベロープで**動かせるパラメータを広げた**版です。

## QUALITY に「CHIP DAC」を足しました

FM 音源（OPNA・OPN・OPL・OPL3・OPM）と SSG の QUALITY に、**CHIP DAC** のトグルを足しました。
INTERP の下にあります。入れると、その音源の実機が音を出すときに通る DAC の癖をなぞります。

**初期値は切**です。切っているあいだはこれまでと同じ音で、これまでのプリセットや DAW の
プロジェクトの音は変わりません。

### FM 音源

鳴っている音をすべて足してから、実機の DAC へ通します。

| 音源 | DAC | すること |
| --- | --- | --- |
| OPN (YM2203) | YM3014B | 浮動小数点（仮数 10bit + 指数 3bit）。大きい音ほど下の桁が落ちる |
| OPM (YM2151) | YM3012 | 同上 |
| OPL (YM3526) | YM3014B | 同上 |
| OPNA (YM2608) | — | 16bit の刻みへ寄せる |
| OPL3 (YMF262) | YAC512 | 16bit の刻みへ寄せる |

- どれも 16bit を超えたぶんは頭打ちになります。実機と同じく、音を重ねすぎると割れます
- 1 キャリアをベロシティ 127・LEVEL 1.0 で鳴らした大きさを、実機の 1 チャンネルの最大に
  合わせています（OPN・OPM・OPL3 は 14bit、OPNA・OPL は 13bit）

### SSG

音量を、実機（YM2149 互換）の対数の表で鳴らします。エンベロープの値を 16 段の音量
レジスタと見なし、SSG HW AMP ENV は 32 段で表を引きます。値が下がると、これまで（線形）
より早く小さくなります。

DUTY の More Real FC / More Real NES を入れているときは、2A03 の DAC を通すので効きません。

### 保存

チャンネルパラメータ・区分のパラメータファイル（QUALITY）・プリセット・DAW の状態に
保存します（`chipDac`）。持たない古いファイルを読んでも、いまの値のままです。

詳しくは [チャンネル共通の CHIP DAC](/2686V_2686VFX/chips/common/#chip-dac--実機の-dac-を通す) を
見てください。

対象: 2686V・2686VLight・26V・86V・OPNV・OPLV・OPMV・PULSEV

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
1 つの段に組を**最大 16 個**入れられます。

チャンネルに 1 つ（[[TAP SEQUENCER]] の中）と、FM 音源のオペレーターごと・RHYTHM のパッドごとに 1 つずつ
（オペレーター・パッドの設定の列）あります。並ぶのは持ち主のパラメータだけで、どれも打鍵ごとに別々に段を進めます。

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
- FM 音源はオペレーターごとにも 1 本（設定の列「PARAM ENV」。KEY SCALE / MUL/DET / FIX の右に、独立した列で置きます）
- 値は指し先のパラメータの範囲の割合で持ち、画面には指し先の書き方で出します
- 値（STL・L・LR・ENDL）は TARGET ごとに別に持ちます。TARGET を変えると、その指し先の値に切り替わります
- 整数や選択肢のパラメータは、段のあいだ値を保ちます
- MUL・DT・FIX の FREQ など押したときに決まるパラメータも、鳴っているあいだ段ごとに変わります
- STEP が 1 のときは R.TARGET を動かせません（ほかの段のエンベロープ・TAP SEQUENCER の S.TARGET なども同じ）

**FM 音源のオペレーターの TL ENV は、PARAM ENV の指し先 TL になりました。** 4.2.x までのプリセット・DAW の状態・
チャンネルパラメータ・区分のパラメータファイル（`.tlEnv`）は、読み込むときに PARAM ENV（TARGET が TL）へ写すので、
同じように鳴ります。パラメータの ID は `_TLENV_*` から `_PRMENV_*` に変わったため、DAW で TL ENV を
オートメーションしていたときは割り当て直してください。

くわしくは [PARAM ENV\[256\]](/2686V_2686VFX/chips/common/#param-env256) を参照してください。

## グラフに、鳴っている位置を動かして出せるようにしました

鍵を弾くのに合わせて、TAP SEQUENCER のいまの段の印と、PARAM ENV[256] の鳴っている位置（グラフの上の黄色の縦線）を
動かします。SETTINGS で別々に入り切りでき、速さを 15 / 30 / 60 / 120 FPS から選べます（初期値は入・30 FPS）。

チャンネルのグラフと FM 音源のオペレーターのグラフの切り替えに **PRM**（PARAM ENV[256]）を足しました。
くわしくは [グラフの鳴っている位置](/2686V_2686VFX/guide/screen/#グラフの鳴っている位置) を参照してください。

## 区分の見出しの色を変えました

- TAP AMP SEQUENCER はシアン、TAP PITCH SEQUENCER は白寄りの紫にしました（これまでと逆の組み合わせです）
- PARAM ENV[256] と TAP PARAM SEQUENCER は白寄りのダークブルーにしました

## マニュアルに「チュートリアル」の章を足しました

音を作りながら操作を覚えるページを、これから足していきます。動画や動く絵も貼れます。

## 簡易表示モードで隠すものを足しました

PARAM ENV[256] と TAP PARAM SEQUENCER は、簡易表示モードで隠れます。SETTINGS の**簡易表示モードカスタマイズ**に
「PARAM ENV[256]」と「TAP PARAM SEQUENCER」を足したので、出したままにもできます。**[非表示中の区分をバイパス]** でも
切れます。

## ADPCM+ のスロットをすべて符号化しておくようにしました

TAP PARAM SEQUENCER / PARAM ENV で ADPCM+ の VOICE を変えると、選んでいないスロットも鳴ります。そのため、
読み込んであるスロットは QUALITY を変えたときにすべて符号化し直すようにしました。
