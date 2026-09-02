---
title: 出力への変調
description: 2686VFX だけにある、出てきた音そのものへ掛ける変調
sidebar:
  order: 10
---

**2686VFX だけ**の区分です。音源が 1 音ごとに持っているエンベロープや LFO を、
**入ってきた音そのもの**へ掛けます。ドラムのループを音量エンベロープで刻む、
ギターを音程エンベロープで落とす、といった使い方ができます。

:::note[鍵盤で動くもの、動かないもの]
エンベロープは **MIDI のノートオン・ノートオフ**で動きます。2686VFX には
鍵盤がないので、DAW のトラックへ MIDI を送ってください。送らないあいだ、
エンベロープの区分は素通しになります。

LFO と音程ずらしは、押さなくても掛かります。だから入り切りの札が
分けてあります。
:::

## 変調の入り切り

エフェクターの枠の中に、4 つの札が並んでいます。**どれも初めはバイパスが
入っています**（＝掛かりません）。使うものだけ外してください。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **エンベロープをバイパス** | AMP ENV・SSG HW AMP ENV・SSG SW AMP ENV[11] をまとめて通さない | オン / オフ | オン | [`MOD_ENV_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-env-bypass) |
| **LFO をバイパス** | LFO を通さない | オン / オフ | オン | [`MOD_LFO_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-lfo-bypass) |
| **ピッチ変調をバイパス** | PITCH ENV・SSG SW PITCH ENV[11]・WT PITCH MOD をまとめて通さない | オン / オフ | オン | [`MOD_PITCH_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-bypass) |
| **音程ずらしをバイパス** | MUL・DET と UNISON・HARMONY をまとめて通さない | オン / オフ | オン | [`MOD_SHIFT_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-shift-bypass) |

## 音量 — AMP ENV

音の大きさを、押してから離すまでの流れで動かします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **バイパス** | この枠を通さない | オン / オフ | オフ | [`MOD_ADSR_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-adsr-bypass) |
| **STL** | 鳴り始めの音量 | 0 〜 1 | 0 | [`MOD_STL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-stl) |
| **AR** | 最大まで上がる時間（秒） | 0.001 〜 10 | 0.001 | [`MOD_AR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-ar) |
| **DR** | SL まで下がる時間（秒） | 0.001 〜 10 | 0.001 | [`MOD_DR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dr) |
| **SL** | 押しているあいだ保つ音量 | 0 〜 1 | 1 | [`MOD_SL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-sl) |
| **RR** | 離してから消えるまでの時間（秒） | 0.001 〜 10 | 0.001 | [`MOD_RR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-rr) |
| **KOR** | 離しても、最後まで鳴らし切る | オン / オフ | オフ | [`MOD_KOR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-kor) |

同じ「エンベロープをバイパス」の下に、
[SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) と
[SSG SW AMP ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env11) の枠もあります。中身は
音源のものと同じです。

## LFO

音量と音程を、周期的に揺らします。中身は音源の [LFO](/2686V_2686VFX/chips/common/#lfo) と同じで、
AM 側が音量、PM 側が音程へ掛かります。

押さなくても回り続けるので、入り切りは **LFO をバイパス** で別に持っています。

## 音程 — PITCH ENV

音の高さを、押してから離すまでの流れで動かします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **バイパス** | この枠を通さない | オン / オフ | オフ | [`MOD_PITCHENV_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitchenv-bypass) |
| **STL** | 鳴り始めの音程（セント） | -4800 〜 4800 | 0 | [`MOD_PITCH_STL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-stl) |
| **AR** | ATL へ移る時間（秒） | 0.001 〜 10 | 0.001 | [`MOD_PITCH_AR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-ar) |
| **ATL** | 頂点の音程（セント） | -4800 〜 4800 | 0 | [`MOD_PITCH_ATL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-atl) |
| **DR** | SSL へ移る時間（秒） | 0.001 〜 10 | 0.001 | [`MOD_PITCH_DR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-dr) |
| **SSL** | 押しているあいだ保つ音程（セント） | -4800 〜 4800 | 0 | [`MOD_PITCH_SSL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-ssl) |
| **RR** | RLL へ移る時間（秒） | 0.001 〜 10 | 0.001 | [`MOD_PITCH_RR`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-rr) |
| **RLL** | 離したあと落ち着く音程（セント） | -4800 〜 4800 | 0 | [`MOD_PITCH_RLL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-rll) |

同じ「ピッチ変調をバイパス」の下に、
[SSG SW PITCH ENV\[11\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env11) と
[WT PITCH MOD](/2686V_2686VFX/chips/common/#wt-pitch-mod) の枠もあります。

### WT PITCH MOD の基準周波数

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **基準周波数** | 揺らす元になる高さ（Hz） | 1 〜 2000 | 440 | [`MOD_WTMOD_BASEFREQ`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-wtmod-basefreq) |

音源では、鳴っている音の高さがそのまま基準になります。2686VFX には決まった
高さがないので、**基準周波数**で決めます。実機には無い、こちらで足したものです。

## 音程ずらし — MUL・DET

音の高さを、一定の量だけずらします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **MUL** | 元の高さの何倍にするか | 0 〜 21 | 2 | [`MOD_MUL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-mul) |
| **MURT** | 倍率を数値で細かく決める | 0.01 〜 100 | 1 | [`MOD_MUL_RATIO`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-mul-ratio) |
| **DT1** | わずかにずらす | 0 〜 15 | 0 | [`MOD_DT`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dt) |
| **DT2** | 粗くずらす | 0 〜 3 | 0 | [`MOD_DT2`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dt2) |
| **DT3** | セントで指定してずらす | -4800 〜 4800 | 0 | [`MOD_DT3`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dt3) |

同じ「音程ずらしをバイパス」の下に
[UNISON・HARMONY](/2686V_2686VFX/chips/common/#unisonharmony) の枠もあります。入ってきた音を
複数に分け、それぞれをずらして重ねます。

:::caution[音程を動かすということ]
エフェクトには発振器がありません。音程側の変調は、入ってきた音を溜めてから
読み出す速さを変えて作っています。**簡易的な作り**なので、大きくずらすほど
ざらつきます。仕組みは
[音程を動かす仕組み](/2686V_2686VFX/plugins/fx-plugin/#%E9%9F%B3%E7%A8%8B%E3%82%92%E5%8B%95%E3%81%8B%E3%81%99%E4%BB%95%E7%B5%84%E3%81%BF)
を参照してください。
:::

## オートメーション

一覧は [2686VFX のオートメーション](/2686V_2686VFX/reference/automation/fx-plugin/)
にあります。`MOD_` で始まるものがこの区分です。
