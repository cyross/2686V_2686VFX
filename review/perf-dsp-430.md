# v4.3.0 DSP パフォーマンス敵対的検証

対象ブランチ: `feat/adversarial-review-430`（`aefdf5fd`）
観点: 音声スレッド（`processBlock` から下）の計算量・確保・ロック・非正規化数など
方法: コードを読んだだけ。ビルド・計測はしていない。効き具合の見積もりは、明記がなくてもすべて**推測**。

## 範囲について

`main` はすでに 4.3.0 を取り込んでいる（`da8a16c9 Merge pull request #182 from cyross/4.3.0`）。
そのため `git diff main...HEAD` は検証ブランチの 11 コミットしか出さない（DSP は SSG の More Real FC / SSG、
LivePosition、PARAM ENV の値の組など）。4.3.0 全体の DSP を見るため、**4.2.0 のマージ `5789b9b9` から HEAD まで**
を「差分」として扱った。

## 凡例

- 効き: 高 = 普通の使い方で CPU 使用率に目に見えて効きそう / 中 = 条件がそろうと効く / 低 = 気になる程度
- 確からしさ: 確実 = コードの流れとして確かにそうなる（重さの量は推測） / 推測 = 流れも条件次第

---

# 第 1 段階: 4.3.0 の差分のホットパス

## P1-1. 鳴り終えたボイスも、毎ブロック音源の値を丸ごと写して読み直す（PARAM ENV / TAP PARAM）

- 場所:
  - `2686V/Source/Core/Processor/PluginProcessor.cpp:437`（全ボイスへ毎ブロック `setParameters`）
  - `Shared/Core/Synth/SynthCore.h:69-86`（`paramModResolve`）
  - `Shared/Core/Synth/ParamMod.h:119,131,162`（`started` を false に戻すのは指し先が変わったときだけ）
  - `Shared/Core/Synth/ParamMod.h:208-224`（`collect`）
- 何が重いか:
  プロセッサは `Global::totalVoices`（10 × ユニゾン 8 = **80 ボイス**）すべてへ毎ブロック `setParameters` を配る。
  各コアの `setParameters` は `paramModResolve` を通り、`collect` が 1 つでも置き換えを返すと
  `scratch = chip`（OpnaParams など。WtModParams がチャンネル 1 + オペレーター 4 × 2 本あり、それぞれ 256 バイト超の表を
  持つので数 KB）を写し、`applier->apply` で `applyChannel` / `applyOp` を走らせてパラメータを読み直す。
  `collect` が返すのは「TAP の組（`tap.count > 0`）」と「`enable && started` の PARAM ENV」だが、
  - `started[i]` は `noteOn` で true にしたあと、鳴り終えても false に戻らない（戻すのは `configure` で enable / scope / index
    が変わったときだけ）。
  - `tap` も `setTap` で入れたまま残る。
  そのため **一度でも PARAM ENV / TAP PARAM SEQUENCER 付きで鳴らしたボイスは、鳴っていなくても毎ブロック
  コピー + 読み直し** を続ける。ユニゾンを使えば 1 打鍵で 8 ボイス、しばらく弾けば 80 ボイスすべてがこの状態になる。
  加えて `m_paramModLive = envRunning()` も true のまま残る（下の P1-3）。
- 効き: **中〜高**（推測。1 ボイスあたり数 µs として 80 ボイスで数百 µs / ブロック。512 サンプル / 48kHz の
  ブロック 10.7ms に対し数 % 程度と見込む。2686V はチャンネルが 1 つなので掛け算は 80 で止まる）
- 確からしさ: 流れは**確実**、量は推測
- 直し方の案:
  1. `SynthVoice::setParameters` で、鳴っていないボイスは `paramModResolve` を通さない（あるいは `isVoiceActive()` の
     ボイスにだけ配り、`startNote` の頭で最新の値を配る）。`SynthVoice.cpp:41-55` の注記にあるとおり各コアの
     `setParameters` は値を溜め込まないので、鳴る直前に 1 回配れば足りるはず。
  2. または `Runtime` に「鳴り終えた」を知らせる口を作り、`started` と `tap.count` を落とす
     （コアの `isPlaying()` が false になったとき、または `clearCurrentNote` のとき）。

## P1-2. PARAM ENV が斜めに動くあいだ、最短 16 サンプルごとにコアの setParameters を丸ごとやり直す

- 場所:
  - `2686V/Source/Core/Synth/SynthVoice.cpp:100-106`（`advanceParamMod` → `reapplyParamMod`）
  - `2686V/Source/Core/Synth/SynthVoice.cpp:111-129`（`paramModChunk` = 16 で区切る）
  - `Shared/Core/Synth/ParamMod.h:178-206`（`continuousInterval` = 64、`bigJump` = 0.05）
- 何が重いか:
  値が変わるたびに `reapplyParamMod` → コアの `setParameters(*m_paramModSrc)` を呼び直す。これは
  (a) `paramModResolve` で `configure`（有効な StepEnv の `setParameters`）と数 KB のコピー、
  (b) 置き換えた区分の `applyChannel` / `applyOp`（数十〜百個のパラメータ読み出し。各読み出しは
  `PrOverride::load` で置き換えの並びを線形に探す）、
  (c) コアの `setParameters` 本体（全オペレーターの `setParameters`、エンベロープの増分の作り直し、ルーティングの
  キャッシュ作り直し、`m_paramModLive` による周波数の作り直し）
  をすべて含む。
  間引きは「斜めに動く指し先は 64 サンプルに 1 回」だが、1 区切り（16 サンプル）で 0.05 以上動くと待たずに読み直す。
  速いアタック・ディケイ、または段で値を保つ指し先（`stepped`）が細かい段で変わると、**16 サンプルごと
  （48kHz で 3,000 回 / 秒 / ボイス）** になる。
  しかも `applier->apply` は dirty の区分だけ読み直すのに、(c) はコア全体を作り直すので、TL 1 個を動かすだけでも
  全オペレーターぶんの仕事が走る。
- 効き: **中**（推測。PARAM ENV を使うパッチで、同時発音数 × ユニゾンに比例）
- 確からしさ: 流れは**確実**、量は推測
- 直し方の案:
  1. `reapplyParamMod` 用に、dirty の区分だけを作り直す軽い道を作る（たとえば TL・MUL・DT など指し先の種類ごとに
     オペレーター 1 つだけ `setParameters` する）。
  2. `bigJump` で待たずに読み直す回数に上限を付ける（たとえば最短 32 サンプル）。
  3. `scratch = chip` のコピーを、置き換える区分（チャンネル / オペレーター n）だけに絞る。

## P1-3. PARAM ENV が何を指していても、鳴っているボイスは毎ブロック全オペレーターの周波数を作り直す

- 場所:
  - `Shared/Core/Synth/SynthCore.h:73`（`m_paramModLive = m_paramMod.envRunning()`）
  - `Shared/Core/Synth/ParamMod.h:147-152`（`envRunning`）
  - 例: `Shared/Synth/Opn/SynthOpn.cpp:172-174`、`Shared/Synth/Ssg/SynthSsg.cpp:132`、ほか全コア
- 何が重いか:
  `envRunning` は「有効で走り始めた PARAM ENV があるか」だけを見て、指し先が MUL / DT / FIX のような周波数に効く
  ものかを見ない。TL や AR を動かすだけの PARAM ENV でも、鳴っているボイスは毎ブロック（さらに P1-2 の読み直しの
  たびに）全オペレーターの `refreshFrequency` → `updateFrequency` を走らせる。`refreshFrequency` は `isPlaying()` で守られて
  いるので鳴っていないボイスには効かない。
- 効き: **低〜中**（推測。`updateFrequency` の中身次第。P1-2 と重なると回数が増える）
- 確からしさ: **確実**
- 直し方の案: カタログに「周波数に効く指し先」の印を持たせ、`envRunning` をその指し先に限る。
  あるいは `apply` の dirty と同じく、周波数に効く指し先が置き換えられたときだけ作り直す。

## P1-4. More Real FC のノイズで、ブロックごと・ボイスごとに `log` を 16 回呼ぶ

- 場所:
  - `Shared/Generator/Pulse/GenTnd2A03.h:79-95`（`nearestNoiseRate`。表 16 段それぞれで `std::log`）
  - `Shared/Generator/Noise/Ssg/GenNoiseSsg.cpp:49`（`updateDelta` から呼ぶ）
  - `Shared/Synth/Ssg/SynthSsg.cpp:121-122`（`setParameters` で毎ブロック `updateDelta`）、`:140-141`（`updateFrequency`）
- 何が重いか:
  `setParameters` は鳴っていないボイスにも毎ブロック配られる（P1-1）ので、NOISE の More Real FC を入れると
  80 ボイス × 16 回 = **1,280 回の `log` / ブロック**。`log(r / rate)` は `log(r) - log(rate)` に分けられるので、
  本来は 1 回で済む。
- 効き: **低〜中**（推測。1 回 20ns として 1 ブロック 25µs 前後）
- 確からしさ: **確実**
- 直し方の案: 表の `log(cpuClock / period)` を `constexpr` か静的な表に持ち、呼ぶたびの `log` は `log(rate)` の 1 回だけに
  する。さらに `targetFreq` が前回と同じなら `delta` を作り直さない。

## P1-5. SSG の 2A03 三角波で、チップのサンプルごとに周波数を寄せ直す

- 場所: `Shared/Synth/Ssg/SynthSsg.cpp:415-425`（`quantizeTriangleFrequency`）、`GenTnd2A03.h:55-65`
- 何が重いか:
  `m_rateAccumulator` の 1 刻み（チップのサンプル）ごとに、double の掛け算・割り算 2 回と `std::lround` を回す。
  入力の `phaseInc` はピッチエンベロープや LFO で毎刻み変わりうるので完全には外へ出せないが、寄せた結果（タイマー値）は
  ほとんどの刻みで前回と同じ。矩形波の 2A03（`Pulse2A03::quantizeFrequency`、4.3.0 より前からある）も同じ形。
- 効き: **低**（推測）
- 確からしさ: **確実**
- 直し方の案: 前回の `effective` を覚えておき、変わったときだけ寄せ直す。

## P1-6. エンベロープの「鳴っている位置」を、GUI のグラフを開いていなくても毎サンプル書く

- 場所:
  - `Shared/Core/Synth/LivePosition.h:97-104,120-134`（`LiveWriter::write` / `LiveLevelWriter::write`）
  - `Shared/Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddr.h:117-124`（同じ形が FmRgAdssr / OplAdsr / Opzx7Adddr にもある）
  - `Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.cpp:99-105`、`Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.cpp:90-100`
  - `Shared/Effect/Envelope/Step/EnvStep.cpp:109-123,271`（`publish` を毎サンプル）
  - `Shared/Processor/*/Processor*.cpp`（`pXxx.live = &stepTables.position(...)` を無条件で配る）
- 何が重いか:
  いちばん新しく押したボイスだけが書く造り（持ち主でなくなったボイスは 1 回目の失敗で `id = 0` にして手を離す）なので、
  ボイス数には比例しない。持ち主のボイスでは、エンベロープ 1 本につき毎サンプル `owner` の原子的読み出し 1 回、
  `clamp`、原子的書き込み 2 回、`LiveLevelWriter` では `abs` 2 回と割り算 1 回、`StepEnv::publish` では割り算 1 回。
  FM の 4 オペレーターなら EG 4 本 + PITCH ENV 4 本 + AMP ENV + PARAM ENV の StepEnv 最大 9 本ぶん。
  SETTINGS の `envLiveGraph` / `paramEnvLiveGraph` / `tapLiveGraph` を切っても、プロセッサは `live` を配り続けるので書き続ける。
  x86 では relaxed の store は普通の mov なので 1 回は安い。GUI が同じキャッシュラインを読むのは数十 fps なので、
  取り合いもほぼ起きない。
- 効き: **低**（推測）
- 確からしさ: **確実**
- 直し方の案:
  1. 書く間隔を間引く（たとえば 32〜64 サンプルに 1 回。グラフは数十 fps なので十分）。
  2. SETTINGS でグラフを切っているとき、または画面を閉じているときは `live` に nullptr を配る。

## P1-7. CHIP DAC の浮動小数点 DAC は、サンプルごとに double の丸めと 6 回のループ

- 場所: `Shared/Core/Synth/ChipDac.h:58-91`、呼び出しは `2686V/Source/Core/Processor/PluginProcessor.cpp`（`ChipDac::process`）
- 何が重いか:
  全ボイスを混ぜたあとに 1 回だけ通すので、ボイス数には比例しない。1 サンプルあたり double の掛け算、
  `std::round`（MSVC の既定の `/arch` では CRT の呼び出しになりうる）、`roundtripFloat` の 6 回のループと分岐、
  `spec.format` の分岐（ループの外へ出せる）。
- 効き: **低**（推測。2 ch × ブロック長だけ）
- 確からしさ: **確実**
- 直し方の案: `std::lrint` か `_mm_cvtsd_si32` で丸める。指数は `std::bit_width`（C++20）や `_BitScanReverse` で求める。
  `format` の分岐はループの外で振り分ける。必要なら気にしなくてよい程度。

## P1-8. 新しいフィルタの `prepare` をチップのサンプルごとに呼ぶ

- 場所: `Shared/Synth/Ssg/SynthSsg.cpp:418`（`m_triFcFilter.prepare`）、`:546`（`m_noiseFcFilter.prepare`）、
  `:595`（`m_ssgDcCut.prepare`）
- 何が重いか: どれも「レートとモデルが同じなら戻る」造り（`GenPulse2A03.h:100-102`、`GenTnd2A03.h:127-129`）なので、
  毎回は比較と分岐だけ。重くはない。
- 効き: **低**
- 確からしさ: **確実**
- 直し方の案: `setSampleRate` / `setParameters` で 1 回呼ぶ形へ移す。

## 確かめて問題なしとしたもの（第 1 段階）

- `ParamModCatalog::defaultsOf` / `channelTargetIds` / `opTargetIds`（`std::vector` と `juce::String` を作る）や
  `StepTableStore::position` / `getWithLevelSets`（`std::map` へ入れる）は、各プロセッサの `init` からだけ呼ばれる。
  音声スレッドでの確保ではない。
- `StepEnv::setParameters` の 256 段の写し直しは、表の `version` が変わったときだけ。`version` を進めるのは GUI からの
  書き換えで、TAP の `clearCursor` は進めない。
- More Real SSG の `DcBlocker`（`GenTnd2A03.h`）は入力が 0 になると出力が指数的に 0 へ近づくが、
  `processBlock` の頭の `juce::ScopedNoDenormals`（`2686V/Source/Core/Processor/PluginProcessor.cpp:269`）が効いている。
- `LivePosition::nextId` の `fetch_add` は押したときに 1 回だけ。

---

# 第 2 段階: 差分の外の、毎サンプル（毎ブロック）通る共有 DSP

## P2-1. 鳴っていないボイスへも、毎ブロック音源の値を丸ごと配る（P1-1 の土台）

- 場所:
  - `2686V/Source/Core/Processor/PluginProcessor.cpp:433-440`（`m_synth.getNumVoices()` の全ボイスへ `setParameters`）
  - `2686V/Source/Core/Synth/SynthVoice.cpp:41-55`（作動中のコアにだけ配る。ボイスが鳴っているかは見ない）
  - 例: `Shared/Synth/Opna/Operator/SynthOpnaOp.cpp:48-67`（`m_params = params` で OpnaOpParams を丸ごと写し、
    11 個の部品の `setParameters` を呼ぶ）、`Shared/Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssr.cpp:74`
    （`std::pow` と `updateIncrementsWithKeyScale`）
- 何が重いか:
  80 ボイスすべてが毎ブロック、作動中のコアの `setParameters` を通る。FM なら 4 オペレーターぶんの値の写し
  （WtModParams / WtAmpModParams の 32 要素の表を含む）、エンベロープの増分の作り直し、`pow` などが、
  鳴っているボイスが 1 つでも 80 回走る。ブロックが短い（64 サンプルなど）ほど比率が上がる。
  4.3.0 の PARAM ENV / TAP PARAM（P1-1）で、ここにさらに値の丸ごとのコピーと読み直しが乗った。
- 効き: **中**（推測。ブロック長が短い設定や、軽い音源（SSG・BEEP）で目立ちやすい）
- 確からしさ: 流れは**確実**、量は推測
- 直し方の案: `isVoiceActive()` のボイスにだけ配り、`startNote` の頭（`SynthVoice.cpp:57` 付近）で 1 回配る。
  `SynthVoice.cpp:44-50` の注記のとおり各コアの `setParameters` は値を溜め込まないので、鳴る前に配れば音は変わらない
  はず（要確認: リリース中のボイスは `isVoiceActive()` が true なので配られ続ける）。
  加えて、プロセッサ側で「パラメータが変わったか」の版を持ち、変わらないブロックは配らない手もある。

## P2-2. LFO を切っていても、チップのサンプルごとに `pow(2, 0)` を呼ぶ（8 つのコア）

- 場所:
  - `Shared/Synth/Ssg/SynthSsg.cpp:375`
  - `Shared/Synth/Wavetable/SynthWt.cpp:343`、`Shared/Synth/Wt2/SynthWt2.cpp:345`、`Shared/Synth/WtPlus/SynthWtPlus.cpp:314`
  - `Shared/Synth/Adpcm/SynthAdpcm.cpp:725`、`Shared/Synth/AdpcmPlus/SynthAdpcmPlus.cpp:728`、`Shared/Synth/Beep/SynthBeep.cpp:270`
  - `Shared/Synth/Rhythm/SynthRhythm.cpp:713`（**パッドごと**）
- 何が重いか:
  `float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);` が `if (m_lfo.pm.enable)` の外にあり、
  PM を切っていて `pitchModCents == 0` でも毎回 `pow` を呼ぶ。AM のほうは `if (m_lfo.am.enable)` の中にあるので
  形がそろっていない。SSG・WT 系はチップのレート（`m_rateAccumulator` の刻み。既定の 96kHz なら 48kHz のホストで
  1 サンプルに 2 回）で回る。`powf` は 1 回 20〜40 サイクル前後なので、80 ボイスが鳴れば無視できない。
- 効き: **中**（推測。PM を使わない普通のパッチで、鳴っているボイス数 × チップのレートぶん）
- 確からしさ: **確実**
- 直し方の案: `if (m_lfo.pm.enable)` の中へ入れ、切っているときは 1.0f にする。
  使うときも `std::exp2(x * (1.0f / 1200.0f))` にすれば `pow` より軽い。

## P2-3. FM オペレーターの波形が、仮想呼び出し + double の `floor` + `std::sin`

- 場所:
  - `Shared/Core/Fm/FmOperator.h:79`（`float virtual calcWaveform(double phase, int wave)`）
  - `Shared/Core/Fm/FmOperator.cpp:21-30`（別の翻訳単位にあり、LTCG が無いとインライン化されない）
  - 呼び出し: `Shared/Synth/Opna/Operator/SynthOpnaOp.cpp:428` ほか、全 FM オペレーターの `getSample`
- 何が重いか:
  オペレーターごと・チップのサンプルごとに、仮想呼び出し、double の `floor` と掛け算、float の `sin`。
  4 オペレーター × 96kHz（既定）× 鳴っているボイス数。OPZX7 は 8 オペレーター。FM 音源の中ではいちばん数の多い計算。
  OPNA・OPN・OPM は `calcWaveform` を上書きしていないので、仮想にする必要がない。
- 効き: **中**（推測）
- 確からしさ: 流れは**確実**、量は推測
- 直し方の案:
  1. 各オペレーターのクラスを `final` にするか、`calcWaveform` を非仮想のインライン関数にして、`getSample` から
     直接呼べるようにする。
  2. サインは表引き（たとえば 4096 点 + 線形補間）にする。実機の FM 音源も 1/4 周期の log-sin 表を引いているので、
     実機に寄せる方向でもある（ただし音が変わるので「使う」トグルの扱いを検討すること）。

## P2-4. OPNA の N88 LFO・オペレーターの LFO で、サンプルごとに `pow`

- 場所:
  - `Shared/Synth/Opna/Operator/SynthOpnaOp.cpp:358`（AM。`n88Lfo.amEnable` のとき）
  - `Shared/Synth/Opna/Operator/SynthOpnaOp.cpp:379`（PM。`n88Lfo.pmEnable` のとき）
  - `Shared/Effect/Lfo/Opna/LfoOpna.cpp:91,194`（オペレーターの LFO の AM）
  - 同じ形がほかの FM オペレーターにもある見込み（OPN・OPM・OPL・OPZX7 は未確認）
- 何が重いか:
  LFO を入れたとき、オペレーターごと・チップのサンプルごとに `pow(10, x)` と `pow(2, x)`。
  LFO の値は 1 サンプルで少ししか動かないうえ、N88 LFO の値はコアで 1 つ（全オペレーター共通）なのに、
  オペレーターの数だけ同じ `pow` を計算している（PM は `n88Lfo.value.pm * n88Lfo.depthNorm` で全オペレーター同じ値）。
- 効き: **中**（推測。LFO を使うパッチで）
- 確からしさ: **確実**
- 直し方の案:
  1. PM の倍率はコアで 1 回求め（`m_n88Lfo.getSample()` の直後）、オペレーターへ渡す。
  2. `pow(10, -dB/20)` は `exp2(dB * -0.16609640f)`、`pow(2, c/1200)` は `exp2(c * (1/1200.f))` に置き換える。

## P2-5. 切っている部品も、オペレーターごとに毎サンプル通す

- 場所: `Shared/Synth/Opna/Operator/SynthOpnaOp.cpp:392-406`（`m_ssgHwPEnv.process`、`m_wtMod.process`、
  `m_wtAmpMod.process`、`m_ssgHwEnv.process`）、`Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHw.cpp:270-301`
- 何が重いか:
  コメントにあるとおり「切ってあるときは 1.0 / MAX が返るので毎サンプル通しておく」造り。1 つずつは分岐と
  数回の演算だが、4 部品 × オペレーター数 × チップのレート × ボイス数。SSG HW PITCH ENV は切っていても
  スムージングの 1 次ローパスまで回る（`m_smooth` が切れていれば早めに抜ける）。
- 効き: **低**（推測）
- 確からしさ: **確実**
- 直し方の案: `setParameters` で「すべて切れている」を 1 つの bool にまとめ、そのときは 4 つを飛ばす。
  ただし位相を進め続ける意図（切り替えた瞬間に飛ばない）があるので、飛ばすなら切り替え時に位相を合わせ直すこと。

## P2-6. WT PITCH / AMP MOD のサイン形で、サンプルごとに double の `sin`

- 場所: `Shared/Generator/WtMod/GenWtModulator.h:139`、`Shared/Generator/WtMod/GenWtAmpModulator.h:126`、
  `Shared/Effect/Lfo/Opna/LfoOpna.cpp:137,160`
- 何が重いか: 有効なとき（またはモジュレーションホイールを上げたとき。`GenWtModulator.h:88`）、オペレーターごと・
  サンプルごとに `sin` と `pow`（`centsToRatio`）。FM ではオペレーター 4 本 + チャンネル 1 本ぶん。
- 効き: **低〜中**（推測。使ったときだけ）
- 確からしさ: **確実**
- 直し方の案: LFO のサインは回転（複素数の掛け算）か表引きにする。

## P2-7. TAP AMP を使うと、ブロックが長くなったときに音声スレッドで `vector::resize`

- 場所: `2686V/Source/Core/Synth/SynthVoice.cpp:227-230`（`m_tapKeepL` / `m_tapKeepR`）
- 何が重いか: `numSamples` がそれまでより大きいと、音声スレッドでヒープを確保する。ホストのブロック長が変わった
  直後に 1 回だけ（ボイスごと）起きる。4.2.0 以前（TAP SEQUENCER を足したとき）からある。
- 効き: **低**（1 回だけだが、リアルタイムの決まりとしては違反）
- 確からしさ: **確実**
- 直し方の案: `setCurrentPlaybackSampleRate` か `prepareToPlay` の経路で、最大ブロック長ぶん確保しておく。

## P2-8. EQ（3 BAND）をオートメーションすると、毎ブロック係数を new / delete

- 場所: `2686V/Source/Effect/Fx/Fx.cpp:427-470`（2686VFX は `2686VFX/Source/Effect/Fx/Fx.cpp:495-535`）
- 何が重いか: 値が同じなら作り直さない守りは入っている（3.x で直したもの）。ただしオートメーションで値が動く
  あいだは、毎ブロック `makeLowShelf` / `makePeakFilter` / `makeHighShelf` がヒープに確保し、古い係数の参照が外れて
  音声スレッドで解放される。
- 効き: **低**（オートメーション中だけ）
- 確からしさ: **確実**
- 直し方の案: 係数は `juce::dsp::IIR::ArrayCoefficients<float>::makeLowShelf`（確保しない）で求め、既にある
  `Coefficients` の `coefficients` 配列へ書き込む。

## P2-9. ユニゾンの音量補正で、サンプルごとに `sqrt`

- 場所: `Shared/Core/Synth/UnisonState.h:60-63`（`getGainComp`）。呼び出しは例として `Shared/Synth/Opna/SynthOpna.cpp:413`
- 何が重いか: ボイスごと・ホストのサンプルごとに `1 / sqrt(total)`。`total` はノートの間変わらない。
- 効き: **低**
- 確からしさ: **確実**
- 直し方の案: `total` を決めたときに 1 回求めて持つ。

## 確かめて問題なしとしたもの（第 2 段階）

- `processBlock` の頭に `juce::ScopedNoDenormals` がある（2686V・2686VFX とも）。FX のディレイ・フィルタ、
  2A03 の出力フィルタ、SSG の DcBlocker の非正規化数はこれで防がれている。
  波形プレビュー（画面のスレッドで DSP を回す）はこの外だが、音声スレッドではない。
- RHYTHM は鳴っているパッドだけを回す（`Shared/Synth/Rhythm/SynthRhythm.cpp:873-874`）。
- `SynthVoice::renderNextBlock` はアルペジオを使わないとき、ブロックごとに 1 回だけ仮想呼び出しする（`renderRange`）。
- AdvancedCurve の `processAudio` はスロットを読むだけで、ロックも確保もない。
- 2686VFX の `ModPitchShifter` のバッファ確保（`assign`）は準備のときだけ。

---

## まとめ（優先順の案）

| # | 指摘 | 効き | 確からしさ |
|---|---|---|---|
| P1-1 / P2-1 | 鳴っていないボイスへの毎ブロックの setParameters と、PARAM ENV / TAP PARAM の丸ごとのコピー + 読み直し | 中〜高 | 流れは確実・量は推測 |
| P2-2 | LFO を切っていても `pow(2, 0)`（8 コア、RHYTHM はパッドごと） | 中 | 確実 |
| P1-2 | PARAM ENV の読み直しが最短 16 サンプルごとにコア全体をやり直す | 中 | 流れは確実・量は推測 |
| P2-3 | FM の `calcWaveform`（仮想 + `floor` + `sin`） | 中 | 流れは確実・量は推測 |
| P2-4 | FM の LFO の `pow` をオペレーターごとに | 中 | 確実 |
| P1-3 | PARAM ENV が何を指していても周波数を作り直す | 低〜中 | 確実 |
| P1-4 | More Real FC のノイズで `log` を 16 回 | 低〜中 | 確実 |
| その他 | P1-5〜P1-8、P2-5〜P2-9 | 低 | 確実 |

手元で裏を取るときは、P1-1 / P2-1 は「PARAM ENV を入れた FM のパッチでユニゾン 8 を何度か弾いてから止め、
無音のまま CPU 使用率を比べる（PARAM ENV を切ったときと）」、P2-2 は「SSG で PM を切ったまま 10 音鳴らし、
`pow` を外す前後で比べる」と差が見えやすいはず。
