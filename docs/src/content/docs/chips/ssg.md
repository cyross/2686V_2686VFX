---
title: SSGチャンネル
description: 矩形波・三角波とノイズを鳴らす SSG チャンネルの使い方
sidebar:
  order: 7
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_ssg.png" alt="SSGチャンネル" style="width:600px;" />
	<figcaption>SSGチャンネル</figcaption>
</figure>

SSG は、YM-2203/YM-2608B の SSG 部分(ルーツは、 AY-3-8910 系のいわゆる PSG )にあたるチャンネルです。矩形波とノイズを鳴らし、ハードウェアエンベロープで音量を動かします。実機に無い拡張として、三角波・鋸波と、より細かい音作りのための項目を足してあります。

## 表記について

このマニュアルでは、画面の区分ごとに次の表記を付けています。**画面上の見出しの
色と対応しています。**

| 表記 | 意味 |
| --- | --- |
| **実機** | 実機にある機能。実機の仕様どおりに動きます |
| **独自** | 実機に無い、このプラグイン独自の拡張 |
| **共通** | 音源に依らない、プラグイン全体の機能 |

実機の音を狙うときは、**独自**の項目を初期値のままにしておくと近づきます。

## FORM — 波形の選択（実機）

鳴らす波形の種類を選びます。

| 値 | 波形 |
| --- | --- |
| `0: Pulse(Rect)` | 矩形波。実機と同じ |
| `1: Triangle / Saw` | 三角波・鋸波（**独自**） |

選んだ波形によって、下に出る設定が切り替わります。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **FORM** | 矩形波か三角波か | [`SSG_WAVEFORM`](/2686V_2686VFX/reference/automation/ssg/#ssg-waveform) |

### 矩形波（デューティ比）設定

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **MODE** | デューティ比の決め方。`0: Preset Ratio` は実機にある 8 種類から選ぶ、`1: Variable (Slider)` は連続で変える（**独自**） | — | |
| **PRESET** | 実機にある 8 種類のデューティ比 | 下表 | |
| **RATIO** | MODE が Variable のときの比率 | 0.0〜1.0 | |
| **FC Mode** | ファミコン風の挙動にする（**独自**） | 入 / 切（初期は切） | |
| **FC FLUC** | FC Mode のときの揺らぎの量（**独自**） | 0.0〜1.0（初期 0.2） | |
| **More Real FC** | ファミコン（2A03）の矩形波の鳴り方を真似る（**独自**） | 入 / 切（初期は切） | 4.0.0 |
| **More Real NES** | NES（2A03）の矩形波の鳴り方を真似る（**独自**） | 入 / 切（初期は切） | 4.0.0 |

**FC Mode・More Real FC・More Real NES は、どれか 1 つだけ入れられます**（すべて切は可）。
1 つを入れると、ほかの 2 つは切れます。

More Real FC / More Real NES は FC FLUC とは別物で、2A03 の作りを次のように真似ます。

- デューティ比は実機の **12.5% / 25% / 50% / 75%** の 4 種類だけ。MODE・PRESET・RATIO・Invert Phrase で決まる比に最も近いものを使い、実機と同じ 8 段の並びで出します
- 音程は 11bit のタイマーの刻みへ寄せます（fpulse = 1789772.7 ÷ (16 × (t + 1))）。t が 8 未満になる高い音は鳴りません
- 音量は 16 段へ丸め、実機の DAC の曲がり（95.88 ÷ (8128 ÷ v + 100)）を通します
- 出力は 0 と音量の間を振れる単極で、後ろのフィルタが直流を抜きます。**ここが FC と NES の違いです**
  - More Real FC: 37Hz のハイパス
  - More Real NES: 90Hz と 440Hz のハイパス、14kHz のローパス（低音が細く、高い音のなまりが出る）

PRESET で選べるデューティ比は次の 8 種類です。

| 値 | 比 | デューティ比 |
| --- | --- | --- |
| 0 | 1:1 | 50% |
| 1 | 3:5 | 37.5% |
| 2 | 5:11 | 31.25% |
| 3 | 1:3 | 25% |
| 4 | 1:4 | 20% |
| 5 | 3:13 | 18.75% |
| 6 | 1:7 | 12.5% |
| 7 | 1:15 | 6.25% |


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **MODE** | 決まった比から選ぶか、自由に決めるか | [`SSG_DUTY_MODE`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-mode) |
| **PRESET** | 実機で使われていた比。9 種 | [`SSG_DUTY_PRESET`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-preset) |
| **RATIO** | 自由に決めるときの比 | [`SSG_DUTY_VAR`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-var) |
| **Invert Phrase** | 上下を入れ替える | [`SSG_DUTY_INV`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-inv) |
| **FC Mode** | ファミコンのように、比が細かく揺れる動きにする（**独自**） | [`SSG_DUTY_FC_MODE`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-fc-mode) |
| **FC FLUC** | その揺れの大きさ | [`SSG_DUTY_FC_FLUC`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-fc-fluc) |
| **More Real FC** | ファミコンの矩形波の鳴り方にする（**独自**） | [`SSG_DUTY_MORE_FC`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-more-fc) |
| **More Real NES** | NES の矩形波の鳴り方にする（**独自**） | [`SSG_DUTY_MORE_NES`](/2686V_2686VFX/reference/automation/ssg/#ssg-duty-more-nes) |

#### DUTY RATIO ENVELOPE（独自・4.0.0）

グループの一番下にある、デューティ比を時間で動かす 256 段のエンベロープです。
作りは [SSG SW AMP ENV\[256\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env256) と同じで、
値が音量ではなく**デューティ比（%）**になります。

- 一番上の **Enable** で入れます。入っているあいだは MODE・PRESET・RATIO・
  Invert Phrase の代わりに、このエンベロープの値を使います（Invert Phrase は掛けません）
- 値は **0.0〜100.0%**。名前は STL が **STDR**、L が **DR1〜DR256**、対象のつまみが
  **DR.TARGET**、値のつまみが **DUTY** です
- 値のつまみの下のボタンで、PRESET で選べるデューティ比
  （50 / 43.75 / 37.5 / 31.25 / 25 / 20 / 18.75 / 12.5 / 6.25%）を直に置けます
- RATE・STEP・KEEP・LOOP・RR / LR・Use Endl / ENDL は SSG SW AMP ENV[256] と同じです
- More Real FC / NES では、この値も 12.5 / 25 / 50 / 75% のうち最も近いものへ寄せます
- 段の R / L は表としてプリセット・チャンネルパラメータへ保存します（オートメーションでは動かせません）。
  区分のパラメータファイルは `.dutyEnv` で、見出しの右クリックから読み書きします

グループは段の数で縦に伸びるので、中を縦に送れるようにしてあります。

### 三角波設定（独自）

FORM で `1: Triangle / Saw` を選んだときに出ます。**PEAK** で波形の頂点の位置を
動かし、三角波から鋸波まで連続で変えられます。

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **PEAK** | 頂点の位置。`0.0` で下降鋸波、`0.5` で三角波、`1.0` で上昇鋸波 | 0.0〜1.0（初期 0.5） | |
| **Key Track (Pitch)** | 音程に追従させる。切ると FQ の周波数で固定される | 入 / 切（初期は入） | |
| **FREQ** | Key Track を切ったときの周波数 | 0〜8000 Hz（初期 440） | |

PEAK の下には `0.0 (Down)` `0.5 (Tri)` `1.0 (Up)` のボタンがあり、押すと
その値へ一度に移ります。


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **Key Track (Pitch)** | 鍵盤の音程で鳴らす。切ると FQ の高さで固定される | [`SSG_TRI_KEYTRK`](/2686V_2686VFX/reference/automation/ssg/#ssg-tri-keytrk) |
| **FREQ** | 固定するときの周波数 | [`SSG_TRI_FREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-tri-freq) |
| **PEAK** | 三角形の頂点の位置。0.5 で左右対称 | [`SSG_TRI_PEAK`](/2686V_2686VFX/reference/automation/ssg/#ssg-tri-peak) |

#### PEAK RATIO ENVELOPE（独自・4.0.0）

グループの一番下にある、三角波の頂点を時間で動かす 256 段のエンベロープです。
作りは DUTY RATIO ENVELOPE と同じです。

- 一番上の **Enable** で入れます。入っているあいだは PEAK の代わりにこの値を使います
- 値は **0.0〜1.0**。名前は STL が **STPK**、L が **PK1〜PK256**、対象のつまみが
  **PK.TARGET**、値のつまみが **PEAK** です
- 値のつまみの下のボタン（−0.01 / −0.1 / 0.0 / 1.0 / +0.1 / +0.01）で値を動かせます
- 区分のパラメータファイルは `.peakEnv` です

## TONE / NOISE / MIX — 音の混ぜ方（実機）

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **T.LEVEL** | 波形（トーン）の音量 | 0.0〜1.0（初期 1.0） | |
| **N.LEVEL** | ノイズの音量 | 0.0〜1.0（初期 1.0） | |
| **N.FREQ** | ノイズの周波数 | 0〜30000 Hz（初期 12000） | |
| **TONE: Use VALUE** | 鍵盤の音へ T.VALUE から決まる倍率を掛ける | 入 / 切（初期は切） | |
| **T.VALUE** | トーンの設定値 To | 0〜4095（初期 47） | 倍率 47 ÷ To |
| **NOISE: Use VALUE** | N.FREQ の代わりに N.VALUE を使う | 入 / 切（初期は切） | |
| **N.VALUE** | ノイズの設定値 Tn | 0〜31（初期 2） | 667000 ÷ (32 × Tn) Hz |
| **MIX** | トーンとノイズの混ぜ具合 | 0.0〜1.0（初期 0.0） | |
| **Noise On Note** | 発音のたびにノイズを鳴らし直す | 入 / 切（初期は切） | |


| つまみ | 内容 | オートメーション |
| --- | --- | --- |
| **T.LEVEL** | 音の側の量 | [`SSG_TONE`](/2686V_2686VFX/reference/automation/ssg/#ssg-tone) |
| **N.LEVEL** | ざらつきの側の量 | [`SSG_NOISE`](/2686V_2686VFX/reference/automation/ssg/#ssg-noise) |
| **N.FREQ** | ざらつきの高さ | [`SSG_NOISEFREQ`](/2686V_2686VFX/reference/automation/ssg/#ssg-noisefreq) |
| **TONE: Use VALUE** | 鍵盤の音へ T.VALUE から決まる倍率を掛ける | [`SSG_TONE_VALUE_ON`](/2686V_2686VFX/reference/automation/ssg/#ssg-tone-value-on) |
| **T.VALUE** | 実機のトーンの設定値 To（0〜4095）。倍率は 47 ÷ To（初期値 47 で 1 倍） | [`SSG_TONE_VALUE`](/2686V_2686VFX/reference/automation/ssg/#ssg-tone-value) |
| **NOISE: Use VALUE** | N.FREQ の代わりに N.VALUE でざらつきの高さを決める | [`SSG_NOISE_VALUE_ON`](/2686V_2686VFX/reference/automation/ssg/#ssg-noise-value-on) |
| **N.VALUE** | 実機のノイズの設定値 Tn（0〜31）。667000 ÷ (32 × Tn) Hz | [`SSG_NOISE_VALUE`](/2686V_2686VFX/reference/automation/ssg/#ssg-noise-value) |
| **Noise On Note** | ざらつきの高さを鍵盤に合わせる（**独自**） | [`SSG_NOISE_ON_NOTE`](/2686V_2686VFX/reference/automation/ssg/#ssg-noise-on-note) |
| **MIX** | 2 つの混ぜ具合。0 で音だけ、1 でざらつきだけ | [`SSG_NOISEMIX`](/2686V_2686VFX/reference/automation/ssg/#ssg-noisemix) |

### 実機の設定値で決める — VALUE

3.6.3 から、TONE / NOISE も実機（YM2203）の設定値で決められます。どれも
初めは切で、切っているあいだはこれまでどおりです。

- **TONE: Use VALUE**: 鍵盤の音の周波数へ、**T.VALUE**（トーンの設定値 To、
  0〜4095）から決まる倍率を掛けます。倍率は Fnote(To) ÷ Fnote(47) = 47 ÷ To
  で、Fnote = 667 × 1000 ÷ (32 × To) です。初期値の 47 で 1 倍、94 で 1 オク
  ターブ下、To を大きくするほど低くなります。切っているあいだは 1 倍です
- **NOISE: Use VALUE**: N.FREQ の代わりに **N.VALUE**（ノイズの設定値 Tn、
  0〜31）でざらつきの高さを決めます。Fnois = 667 × 1000 ÷ (32 × Tn) です。
  入れているあいだ N.FREQ は止まります

T.VALUE / N.VALUE の右の **N** ボタンで、値のボタンが出ます（LEVEL と同じ
様式）。T.VALUE は 1〜2 オクターブ上（12 / 24）・5 度上（31）・4 度上（35）・
5 度下（70）・1〜2 オクターブ下（94 / 188）、N.VALUE は 1 / 4 / 8 / 12 / 16 / 31
へ直に置けます。

0 は実機と同じく 1 として扱います。RHYTHM・ADPCM・ADPCM+ の FORM も同じです。

## HW ENV — ハードウェアエンベロープ（実機）

実機の SSG が持っている、音量を自動で動かす仕組みです。**Enable** を入れると
働きます。

| 項目 | 説明 | 範囲 | 備考 |
| --- | --- | --- | --- |
| **Enable** | ハードウェアエンベロープを使う | 入 / 切（初期は切） | |
| **SHAPE** | 形の種類 | 0〜43 | |
| **PERIOD** | 一周にかかる時間 | 0.1〜200.0（初期 1.0） | |
| **Use VALUE** | PERIOD の代わりに VALUE を使う | 入 / 切（初期は切） | |
| **VALUE** | エンベロープの設定値 Te | 0〜65535（初期 1303） | 667000 ÷ (512 × Te) Hz |

:::note
実機の形は 8 種類ですが、ここでは **43 種類**まで選べます。8 番目より後ろは
独自の追加です。実機の音を狙うときは 8 種類の中から選んでください。
:::

## そのほかの区分

以下は音源に依らない部分です。それぞれの詳しい説明は
[共通の区分](/2686V_2686VFX/chips/common/) にまとめてあります。

| 区分 | 表記 | 内容 |
| --- | --- | --- |
| [**ENVELOPE**](/2686V_2686VFX/chips/common/#envelope) | 独自 | [AMP ENV](/2686V_2686VFX/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV\[256\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env256) / [PITCH ENV](/2686V_2686VFX/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[256\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env256) |
| [**LFO**](/2686V_2686VFX/chips/common/#lfo) | 独自 | 音量・音程を周期的に揺らす |
| [**MUL/DET**](/2686V_2686VFX/chips/common/#muldet) | 実機・独自 | 音程をずらす |
| [**UNISON/HARMONY**](/2686V_2686VFX/chips/common/#unisonharmony) | 独自 | 同じ音を重ねて厚くする |
| [**QUALITY**](/2686V_2686VFX/chips/common/#quality) | — | ビット数とサンプリング周波数を落として質感を作る |
| [**EDIT**](/2686V_2686VFX/chips/common/#edit) | 共通 | 切り取り・コピー・貼り付け・初期化と、レベルの一括設定 |
| [**UTILITY**](/2686V_2686VFX/chips/common/#utility) | 共通 | ファイルの読み書き |

QUALITYの詳細は、[リファレンス](../reference/lists-quality.md) をご参照ください。

## UTILITY — ファイルの読み書き（共通）

`[IM]` が読み込み、`[EX]` が書き出しです。読み書きできるのは、LFO・AMP ENV・
PITCH ENV・SSG SW ENV・DETUNE・UNISON・QUALITY・TONE/NOISE と、チャンネル
全体です。

**Level -> All Ch**（このチャンネルの音量を他のすべてのチャンネルへ一度に
反映する）は、3.6.3 で [EDIT](/2686V_2686VFX/chips/common/#edit) へ移しました。

ファイルの形式と置き場所は
[ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。

## オートメーション

SSG が DAW へ出すパラメータの一覧は
[SSG のオートメーション](/2686V_2686VFX/reference/automation/ssg/) に
あります。ID・型・範囲・初期値が並びます。
