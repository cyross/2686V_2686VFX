---
title: RHYTHM
description: 音声ファイルをパッドへ割り当てて鳴らすドラムマシン
sidebar:
  order: 11
---

RHYTHM は **YM2608B のリズム音源**にあたるチャンネルです。実機は 6 つの
ドラム音を ROM で持っていましたが、こちらは**好きな音声ファイルをパッドへ割り当てて**鳴らします。

音質を落とす仕組みが付いているので、読み込んだ音を当時のドラム音源らしい
質感に寄せられます。

## 印について

| 印 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |

## パッド

| プラグイン | パッドの数 |
| --- | :---: |
| 2686V / 2686VLight | **8** |
| 86V | **6** |

実機（YM2608B）は 6 つでしたが、**8 つに拡張**してあります（**独自**）。
86V は 86 音源の構成に合わせて 6 つのままです。

パッドの名前は実機のドラム構成に沿っています。

| # | 名前 | |
| :---: | --- | --- |
| 1 | **BD** | バスドラム |
| 2 | **SD** | スネアドラム |
| 3 | **RIM** | リムショット |
| 4 | **TOM** | タム |
| 5 | **CYMBAL** | シンバル |
| 6 | **HI-HAT** | ハイハット |
| 7 | **PERC1** | パーカッション（**独自**） |
| 8 | **PERC2** | パーカッション（**独自**） |

:::tip[8 パッドの使いどころ]
2686V と 2686VLight の RHYTHM では 8 パッドとなっており、**PPZ8 などが持っていた 8 音の SSGPCM（PSGPCM）の代わり**としても使えます。
ドラム以外の音を並べてることによる切り替え再生の再現にも便利です。
:::

## 画面の区分

パッド 1 つずつに、次の区分があります。

### FORM — 音声ファイルの設定

**Load** でファイルを読み込みます。読み込んだ波形は画面で確認できます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **One Shot** | 鍵を離しても最後まで鳴らす | [`RHYTHM_PAD[0-7]_ONESHOT`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-oneshot) |
| **POFF** | 頭出し（ミリ秒） | [`RHYTHM_PAD[0-7]_PCM_OFFSET`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-pcm-offset) |
| **PRT** | 再生の速さ | [`RHYTHM_PAD[0-7]_PCM_RATIO`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-pcm-ratio) |
| **Loop Point Enable** | 繰り返す範囲を指定する | [`RHYTHM_PAD[0-7]_LOOPPOINT_EN`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-looppoint-en) |
| **LPST / LPED** | その範囲。全体を 0.0〜1.0 とした位置 | [`RHYTHM_PAD[0-7]_LOOPPOINT_ST`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-looppoint-st) |

### NOTE — MIDI キーの割り当て

そのパッドを鳴らす MIDI ノート番号です。**割り当てを自由に変えられる**ので、
手持ちのドラムマップに合わせられます。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **NOTE** | そのパッドを鳴らすノート番号 | [`RHYTHM_PAD0_NOTE`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad0-note) |

### OPTIONAL

読み込んだ音へ、ざらつきを混ぜられます（**独自**）。

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **T.LV** | 読み込んだ音の側の量 | [`RHYTHM_PAD[0-7]_TONE`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-tone) |
| **N.LV** | ざらつきの側の量 | [`RHYTHM_PAD[0-7]_NOISE`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-noise) |
| **N.FQ** | ざらつきの高さ | [`RHYTHM_PAD[0-7]_NOISEFREQ`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-noisefreq) |
| **MIX** | 2 つの混ぜ具合 | [`RHYTHM_PAD[0-7]_NOISEMIX`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-noisemix) |

### LEVEL / PAN

| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **LV** | パッドごとの音量 | [`RHYTHM_PAD[0-7]_VOL`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-vol) |
| **PAN** | パッドごとの定位 | [`RHYTHM_PAD[0-7]_PAN`](/2686V_2686VFX/reference/automation/rhythm/#rhythm-pad-0-7-pan) |

### エンベロープ

パッド 1 つずつに、[共通の区分](/2686V_2686VFX/chips/common/#envelope) のエンベロープを持てます。
ドラムの 1 打ごとに音程を動かす、といった作り込みができます。

音程をずらす [MUL/DET](/2686V_2686VFX/chips/common/#muldet) と、周期的に揺らす [LFO](/2686V_2686VFX/chips/common/#lfo) も
パッドごとに置かれています。

## 共通の区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 内容 |
| --- | --- |
| [**QUALITY(PCM)**](/2686V_2686VFX/chips/common/#qualitypcm) | ビット数・記録方式・サンプリング周波数と、読み戻すときの補間 |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 同じ音を重ねて厚くする |
| [**OPTIONAL**](/2686V_2686VFX/chips/common/#optional) | 追加の動作 |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | パラメータの読み書き |

## 付属のサンプル

`Assets/2686V/Samples` に、SSG で作ったドラム音のサンプルが入っています。
`SSG RHYTHM Sample` プリセットがそれを使う組み方の例です。

扱いは [ライセンス](/2686V_2686VFX/reference/license/) を参照してください。

## オートメーション

RHYTHM が DAW へ出すパラメータの一覧は
[RHYTHM のオートメーション](/2686V_2686VFX/reference/automation/rhythm/) に
あります。ID・型・範囲・初期値が並びます。
