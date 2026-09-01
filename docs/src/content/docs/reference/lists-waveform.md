---
title: 波形一覧
description: 各所で選べる波形の一覧
sidebar:
  order: 7
---

画面のあちこちで波形を選べます。どこで何が選べるかをまとめます。

:::tip[形は画面で見られます]
選んだ波形は、その場でプレビューに描かれます。名前だけで分からないときは、
実際に選んで形を確かめてください。
:::

## SSG HW ENV — ハードウェアエンベロープ

先頭の 8 種類が実機のもので、それ以降は**独自に足した波形**です。すべてのチャンネルで使えます。

**0〜43 の 44 種類**です。名前は付いておらず、画面では結線図として出ます。

## OPZX7S — WAVE SHAPE

オペレータが出す波形です。`[MA-7:nn]` や `[OPZ:nn]` は元になった音源での番号、それが無いものは独自波形です。

**73 種類**あります。

| 値 | 名前 |
| ---: | --- |
| 0 | 00 [MA-7:00]Sine/OPZ(TX81Z) W1 |
| 1 | 01 [MA-7:01]Half Sine |
| 2 | 02 [MA-7:02]Abs Sine |
| 3 | 03 [MA-7:03]Quadra Abs Half Sin |
| 4 | 04 [MA-7:04]Alt Sine |
| 5 | 05 [MA-7:05]Alt Abs Sine |
| 6 | 06 [MA-7:06]Square |
| 7 | 07 [MA-7:07]Log Saw |
| 8 | 08 [MA-7:08]Pudding Sine |
| 9 | 09 [MA-7:09]Half Pudding Sine |
| 10 | 10 [MA-7:10]Abs Pudding Sine |
| 11 | 11 [MA-7:11]Quad Abs Pudding Sine |
| 12 | 12 [MA-7:12]Mini Alt Sine |
| 13 | 13 [MA-7:13]Mini Alt Abs Sine |
| 14 | 14 [MA-7:14]Half Square |
| 15 | 15 ★★ WT File |
| 16 | 16 [MA-7:16]Triangle |
| 17 | 17 [MA-7:17]Half Triangle |
| 18 | 18 [MA-7:18]Abs Triangle |
| 19 | 19 [MA-7:19]Quad Abs Triangle |
| 20 | 20 [MA-7:20]Alt Triangle |
| 21 | 21 [MA-7:21]Alt Abs Triangle |
| 22 | 22 [MA-7:22]Quad Half Square |
| 23 | 23 ★★ WT2 File |
| 24 | 24 [MA-7:24]Diagram |
| 25 | 25 [MA-7:25]Half Diagram |
| 26 | 26 [MA-7:26]Abs Half Saw Up |
| 27 | 27 [MA-7:27]Quad Abs Half Saw Up |
| 28 | 28 [MA-7:28]Alt Diagram |
| 29 | 29 [MA-7:29]Alt Quad Abs Half Saw Up |
| 30 | 30 [MA-7:30]Quad Square |
| 31 | 31 ★★ PCM(Audio) File |
| 32 | 32 [EX000]Alternating Abs Sine |
| 33 | 33 [EX001]Derived Square |
| 34 | 34 [EX002]Saw Down |
| 35 | 35 [EX003]Saw Up |
| 36 | 36 [EX004]Saw + Sine |
| 37 | 37 [EX005]Pulse 25% |
| 38 | 38 [EX006]Pulse 12.5% |
| 39 | 39 [EX007]Pulse 6.25% |
| 40 | 40 [EX008]Round Square |
| 41 | 41 [EX009]Impulse Train |
| 42 | 42 [EX010]Comb / Multi-pulse |
| 43 | 43 [EX011]Resonant Saw (Low) |
| 44 | 44 [EX012]Resonant Saw (High) |
| 45 | 45 [EX013]Resonant Triangle |
| 46 | 46 [EX014]Bulb Sine |
| 47 | 47 [EX015]Double Hump |
| 48 | 48 [EX016]Pseudo Voice Formant 1 |
| 49 | 49 [EX017]Pseudo Voice Formant 2 |
| 50 | 50 [EX018]Metallic 1 |
| 51 | 51 [EX019]Metallic 2 |
| 52 | 52 [EX020]Noise-Like |
| 53 | 53 [EX021]PD Resonance |
| 54 | 54 [EX022]PD Resonance High |
| 55 | 55 [EX023]4-Step Sine |
| 56 | 56 [EX024]8-Step Sine |
| 57 | 57 [EX025]Wavefolded Sine (Soft) |
| 58 | 58 [EX026]Wavefolded Sine (Hard) |
| 59 | 59 [EX027]Bitwise XOR Fractal |
| 60 | 60 [EX028]Bitwise AND Texture |
| 61 | 61 [EX029]Self-Modulated Sine(FB=1) |
| 62 | 62 [EX030]Self-Modulated Sine(FB=2) |
| 63 | 63 [EX031]OPZ(TX81Z) W2 |
| 64 | 64 [EX032]OPZ(TX81Z) W3 |
| 65 | 65 [EX033]OPZ(TX81Z) W4 |
| 66 | 66 [EX034]OPZ(TX81Z) W5 |
| 67 | 67 [EX035]OPZ(TX81Z) W6 |
| 68 | 68 [EX036]OPZ(TX81Z) W7 |
| 69 | 69 [EX037]OPZ(TX81Z) W8 |
| 70 | 70 [EX038]Cubic Triangle |
| 71 | 71 [EX039]Inverse Circle |
| 72 | 72 [EX040]Exponential Spike |

## OPZX7S — LFO

AM と PM で共通の波形です。

**0〜12 の 13 種類**です。名前は付いておらず、画面では結線図として出ます。

## OPNA — HW LFO

YM2608 のハードウェア LFO です。

**0〜5 の 6 種類**です。名前は付いておらず、画面では結線図として出ます。

## OPNA — N88 LFO

N88-BASIC が使っていたソフトウェア LFO です。

**0〜3 の 4 種類**です。名前は付いておらず、画面では結線図として出ます。

## OPM — HW LFO

YM2151 のハードウェア LFO です。

**0〜4 の 5 種類**です。名前は付いておらず、画面では結線図として出ます。

## WT / WT2 — プリセット波形

描く前の出発点として用意してある波形です。

**0〜8 の 9 種類**です。名前は付いておらず、画面では結線図として出ます。

## WT PITCH MOD — 変調のしかた

波形メモリによる音程の変調です。すべてのチャンネルで使えます。

**0〜8 の 9 種類**です。名前は付いておらず、画面では結線図として出ます。

## SSG — 波形

矩形波と三角波・鋸波の切り替えです。

**2 種類**あります。

| 値 | 名前 |
| ---: | --- |
| 0 | Pulse(Rect) |
| 1 | Triangle / Saw |
