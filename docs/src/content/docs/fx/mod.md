---
title: 出力への変調
description: 2686VFX だけにある、出てきた音そのものへ掛ける変調
sidebar:
  order: 11
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

[キーアサイン](#キーアサイン--変調を動かす鍵盤) をカスタマイズにすると、対象
ごとに動かす鍵盤を決められます。そのときは LFO と音程ずらしも、割り当てた
鍵盤を押している間だけ掛かります。
:::

## 変調の入り切り

エフェクターの枠の中に、4 つの札が並んでいます。**どれも初めはバイパスが
入っています**（＝掛かりません）。使うものだけ外してください。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **エンベロープをバイパス** | AMP ENV・SSG HW AMP ENV・SSG SW AMP ENV[256]・WT AMP MOD・AMP SEQUENCER をまとめて通さない | オン / オフ | オン | [`MOD_ENV_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-env-bypass) |
| **LFO をバイパス** | LFO を通さない | オン / オフ | オン | [`MOD_LFO_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-lfo-bypass) |
| **ピッチ変調をバイパス** | PITCH ENV・SSG HW PITCH ENV・SSG SW PITCH ENV[256]・WT PITCH MOD・PITCH SEQUENCER をまとめて通さない | オン / オフ | オン | [`MOD_PITCH_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-pitch-bypass) |
| **音程ずらしをバイパス** | MUL・DET と UNISON・HARMONY をまとめて通さない | オン / オフ | オン | [`MOD_SHIFT_BYPASS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-shift-bypass) |

## キーアサイン — 変調を動かす鍵盤

エフェクターの枠の、ファイルの読み書きの上にあります。**どの鍵盤で、どの変調を
動かすか**を決めます。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **キーアサイン** | シングルキーアサイン / キーアサインのカスタマイズ | 0 〜 1 | 0（シングル） | [`MOD_KEYASSIGN_MODE`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-keyassign-mode) |

- **シングルキーアサイン**（初期）: これまでどおり、どの鍵盤でも全部が動きます
- **キーアサインのカスタマイズ**: 対象ごとに決めた鍵盤でだけ動きます。選ぶと、
  下に対象ごとの鍵盤の一覧が出ます

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_single_key_assign.png" alt="シングルキーアサイン" style="width:400px;" />
	<figcaption>シングルキーアサイン</figcaption>
</figure>

### 対象ごとの鍵盤

カスタマイズのときだけ出ます。鍵盤は C-2 (0) 〜 G8 (127) から選べます。
FIX の音名と同じく、60 が C3 です。

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_key_assign.png" alt="キーアサインのカスタマイズ" style="width:400px;" />
	<figcaption>キーアサインのカスタマイズ</figcaption>
</figure>

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **AMP ENV** | AMP ENV を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_AMPENV`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-ampenv) |
| **SSG HW AMP ENV** | SSG HW AMP ENV を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_SSGHWENV`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-ssghwenv) |
| **WT AMP MOD** | WT AMP MOD を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_WTAMPMOD`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-wtampmod) |
| **SSG SW AMP ENV\[256\]** | SSG SW AMP ENV\[256\] を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_SSGSWENV11`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-ssgswenv11) |
| **AMP SEQUENCER** | AMP SEQUENCER を動かす鍵盤（4.0.0 から） | 0 〜 127 | 60 (C3) | [`MOD_KEY_TAPAMP`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-tapamp) |
| **PITCH ENV** | PITCH ENV を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_PITCHENV`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-pitchenv) |
| **SSG HW PITCH ENV** | SSG HW PITCH ENV を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_SSGHWPENV`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-ssghwpenv) |
| **SSG SW PITCH ENV\[256\]** | SSG SW PITCH ENV\[256\] を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_SSGSWPENV11`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-ssgswpenv11) |
| **PITCH SEQUENCER** | PITCH SEQUENCER を動かす鍵盤（4.0.0 から） | 0 〜 127 | 60 (C3) | [`MOD_KEY_TAPPITCH`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-tappitch) |
| **WT PITCH MOD** | WT PITCH MOD を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_WTMOD`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-wtmod) |
| **LFO AM** | LFO AM を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_LFOAM`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-lfoam) |
| **LFO PM** | LFO PM を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_LFOPM`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-lfopm) |
| **MUL/DET** | MUL/DET を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_MULDET`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-muldet) |
| **UNISON/HARMONY** | UNISON/HARMONY を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_UNISON`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-unison) |
| **ARPEGGIO** | ARPEGGIO を動かす鍵盤 | 0 〜 127 | 60 (C3) | [`MOD_KEY_ARP`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-key-arp) |

割り当てた鍵盤でのふるまいは、対象によって 2 通りです。

| 対象 | ふるまい |
| --- | --- |
| エンベロープ（AMP ENV・PITCH ENV・SSG HW・SSG SW[256]・WT MOD の各枠） | 押すと始まり、離すと戻る |
| LFO AM / LFO PM | **押している間だけ**掛かる。押したときに頭から回る |
| MUL/DET / UNISON/HARMONY | **押している間だけ**掛かる |
| ARPEGGIO | **押している間だけ**アルペジオになる。押したときに 1 声目から始まる |
| AMP SEQUENCER | 押すたびに段を 1 つ進め、**押している間だけ**その段の割合で鳴らす。離すと音を出さない |
| PITCH SEQUENCER | 押すたびに段を 1 つ進め、**押している間だけ**その段のセントずらす。離すと元の高さに戻る |

- 同じ鍵盤を複数の対象へ割り当てられます。その鍵盤を押すと、まとめて動きます
- カスタマイズのときは、変調の枠の見出しの色で、いま動いているかが分かります。
  割り当てた鍵盤が押されていない枠は灰、押されている間はオフホワイトです。
  LFO の枠は AM / PM のどちらか、UNISON/HARMONY の枠は UNISON / ARPEGGIO の
  どちらかの鍵盤が押されていればオフホワイトになります。シングルキーアサインでは、
  これまでどおりすべてオフホワイトです
- 複数の鍵盤を押したときは、それぞれに割り当てた処理がすべて動きます
- 処理する順番は、シングルキーアサインのときと同じです
- 押し離しで掛けたり外したりする LFO は、音が飛ばないよう数ミリ秒かけて
  なめらかに切り替えます
- それぞれの区分の入り切り（札）は、キーアサインとは別に効きます。札で切って
  ある区分は、割り当てた鍵盤を押しても動きません

割り当ては FX のパラメータファイル（[`.2fx`](/2686V_2686VFX/reference/file-spec/2fx/)）へ
保存されます。

## 音量 — AMP ENV

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_amp_env.png" alt="AMP ENV" style="width:400px;" />
	<figcaption>AMP ENV</figcaption>
</figure>

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
[SSG HW AMP ENV](/2686V_2686VFX/chips/common/#ssg-hw-amp-env)、
[SSG SW AMP ENV\[256\]](/2686V_2686VFX/chips/common/#ssg-sw-amp-env256)、
[WT AMP MOD](/2686V_2686VFX/chips/common/#wt-amp-mod) の枠もあります。中身は
音源のものと同じです。

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_hw_amp_env.png" alt="SSG HW AMP ENV" style="width:400px;" />
	<figcaption>SSG HW AMP ENV</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_sw_amp_env_256.png" alt="SSG SW AMP ENV[256]" style="width:400px;" />
	<figcaption>SSG SW AMP ENV[256]</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_wt_amp_mod.png" alt="WT AMP MOD" style="width:400px;" />
	<figcaption>WT AMP MOD</figcaption>
</figure>

### AMP SEQUENCER

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_tap_amp_sequencer.png" alt="AMP SEQUENCER" style="width:400px;" />
	<figcaption>AMP SEQUENCER</figcaption>
</figure>

4.0.0 で足しました。音源の [TAP AMP SEQUENCER](/2686V_2686VFX/chips/common/#tap-sequencer) と
同じ中身で、**鍵盤を押すたびに段を 1 つ進め**、押しているあいだだけ、その段の割合を
入ってきた音の大きさへ掛けます。ドラムのループを、押すごとに違う音量で刻むといった
使い方ができます。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Enable** | 使う | 入 / 切 | 切 | [`MOD_TAPAMP_ENABLE`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tapamp-enable) |
| **STEPS** | 段の数。1 のときは毎回同じ割合で鳴らす | 1 〜 256 | 1 | [`MOD_TAPAMP_STEPS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tapamp-steps) |
| **LOOP** | ループの組（LP.TARGET / START / END / COUNT / ADD / DELETE）に沿って段を戻す（4.1.0 から） | 入 / 切 | 切 | [`MOD_TAPAMP_LOOP`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tapamp-loop) |
| **S.TARGET** | 値のつまみで直す段 | 1 〜 STEPS | 1 | — |
| **RATIO** | その段の割合。下のボタンで少しずつ動かせる | 0.0 〜 4.0 | 1.0 | — |
| **INCREMENT** | 次に使う段を 1 つ進める（最後の段の次は 1） | — | — | [`MOD_TAPAMP_INC`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tapamp-inc) |
| **DECREMENT** | 次に使う段を 1 つ戻す（1 の前は最後の段） | — | — | [`MOD_TAPAMP_DEC`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tapamp-dec) |
| **RESET** | 次に使う段を 1 に戻す | — | — | [`MOD_TAPAMP_RESET`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tapamp-reset) |

- 段の値は横向きの棒で並び、その下に「S1: 値 / S2: 値 / …」の一覧が出ます。地の色が
  変わっている段が、次に押したときに使う段です
- 鍵盤を離すと音を出しません。鍵盤の押し方で、音を区切るゲートとしても働きます
- 掛かるのは「エンベロープをバイパス」が外れているときだけです。札が入っていると、
  Enable を入れても掛かりません
- INCREMENT / DECREMENT / RESET はオートメーションでも押せます（値が 0.5 を下から越えたときに 1 回動く）
- 段の値は表として FX のパラメータファイルと DAW の状態へ保存します（オートメーションでは
  動かせません）。区分のパラメータファイルは `.tapAmp` で、見出しの右クリックから読み書きします
- LOOP とループの組は、音源の [TAP SEQUENCER](/2686V_2686VFX/chips/common/#tap-sequencer) と同じです。
  INCREMENT は組に沿って進み、RESET はループも数え直します

:::caution[Enable を入れたら MIDI を送る]
Enable が入っていると、**鍵盤を押していないあいだは音が出ません**。AMP ENV などと違い、
MIDI を送らないあいだも素通しにはなりません。
:::

## LFO

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_lfo.png" alt="LFO" style="width:400px;" />
	<figcaption>LFO</figcaption>
</figure>

音量と音程を、周期的に揺らします。中身は音源の [LFO](/2686V_2686VFX/chips/common/#lfo) と同じで、
AM 側が音量、PM 側が音程へ掛かります。

押さなくても回り続けるので、入り切りは **LFO をバイパス** で別に持っています。
[キーアサイン](#キーアサイン--変調を動かす鍵盤) をカスタマイズにしたときは、AM と
PM をそれぞれ割り当てた鍵盤を押している間だけ掛かります。

## 音程 — PITCH ENV

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_pitch_env.png" alt="PITCH ENV" style="width:400px;" />
	<figcaption>PITCH ENV</figcaption>
</figure>

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
[SSG HW PITCH ENV](/2686V_2686VFX/chips/common/#ssg-hw-pitch-env)、
[SSG SW PITCH ENV\[256\]](/2686V_2686VFX/chips/common/#ssg-sw-pitch-env256)、
[WT PITCH MOD](/2686V_2686VFX/chips/common/#wt-pitch-mod) の枠もあります。

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_sw_pitch_env_256.png" alt="SSG SW PITCH ENV[256]" style="width:400px;" />
	<figcaption>SSG SW PITCH ENV[256]</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_wt_pitch_mod.png" alt="WT PITCH MOD" style="width:400px;" />
	<figcaption>WT PITCH MOD</figcaption>
</figure>

### PITCH SEQUENCER

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_tap_pitch_sequencer.png" alt="PITCH SEQUENCER" style="width:400px;" />
	<figcaption>PITCH SEQUENCER</figcaption>
</figure>

4.0.0 で足しました。音源の [TAP PITCH SEQUENCER](/2686V_2686VFX/chips/common/#tap-sequencer) と
同じ中身で、**鍵盤を押すたびに段を 1 つ進め**、押しているあいだだけ、その段のセントだけ
入ってきた音の高さをずらします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **Enable** | 使う | 入 / 切 | 切 | [`MOD_TAPPITCH_ENABLE`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tappitch-enable) |
| **STEPS** | 段の数。1 のときは毎回同じだけずらす | 1 〜 256 | 1 | [`MOD_TAPPITCH_STEPS`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tappitch-steps) |
| **LOOP** | ループの組に沿って段を戻す（4.1.0 から。AMP SEQUENCER と同じ） | 入 / 切 | 切 | [`MOD_TAPPITCH_LOOP`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tappitch-loop) |
| **S.TARGET** | 値のつまみで直す段 | 1 〜 STEPS | 1 | — |
| **CENT** | その段のずらし幅（セント）。下のボタンで少しずつ動かせる | −2400 〜 2400 | 0 | — |
| **INCREMENT** | 次に使う段を 1 つ進める（最後の段の次は 1） | — | — | [`MOD_TAPPITCH_INC`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tappitch-inc) |
| **DECREMENT** | 次に使う段を 1 つ戻す（1 の前は最後の段） | — | — | [`MOD_TAPPITCH_DEC`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tappitch-dec) |
| **RESET** | 次に使う段を 1 に戻す | — | — | [`MOD_TAPPITCH_RESET`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-tappitch-reset) |

- 鍵盤を離すと元の高さに戻ります。押していないあいだは素通しです
- 掛かるのは「ピッチ変調をバイパス」が外れているときだけです
- 段の並び・INCREMENT などのボタン・保存のしかたは AMP SEQUENCER と同じです。
  区分のパラメータファイルは `.tapPitch` です
- ほかの音程の変調と同じく、入ってきた音を読み出す速さを変えて作っているので、
  大きくずらすほどざらつきます

### WT PITCH MOD / WT AMP MOD の基準周波数

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **基準周波数** | 揺らす元になる高さ（Hz） | 1 〜 2000 | 440 | [`MOD_WTMOD_BASEFREQ`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-wtmod-basefreq) |

音源では、鳴っている音の高さがそのまま基準になります。2686VFX には決まった
高さがないので、**基準周波数**で決めます。実機には無い、こちらで足したものです。

この基準周波数は WT PITCH MOD と WT AMP MOD で共通です。**枠の地色**でも
系統が分かるようにしてあり、効果は青、変調は赤、LFO は緑、MUL・DET と
UNISON・HARMONY はシアンで塗り分けています。

## 音程ずらし — MUL・DET

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_mul_det.png" alt="MUL/DET" style="width:400px;" />
	<figcaption>MUL/DET</figcaption>
</figure>

音の高さを、一定の量だけずらします。

| つまみ | 内容 | 範囲 | 初期値 | オートメーション |
| --- | --- | --- | ---: | --- |
| **MUL** | 元の高さの何倍にするか | 0 〜 21 | 2 | [`MOD_MUL`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-mul) |
| **MUL.RATE** | 倍率を数値で細かく決める | 0.01 〜 100 | 1 | [`MOD_MUL_RATIO`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-mul-ratio) |
| **DT1** | わずかにずらす | 0 〜 15 | 0 | [`MOD_DT`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dt) |
| **DT2** | 粗くずらす | 0 〜 3 | 0 | [`MOD_DT2`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dt2) |
| **DT3** | セントで指定してずらす | -4800 〜 4800 | 0 | [`MOD_DT3`](/2686V_2686VFX/reference/automation/fx-plugin/#mod-dt3) |

同じ「音程ずらしをバイパス」の下に
[UNISON・HARMONY](/2686V_2686VFX/chips/common/#unisonharmony) の枠もあります。入ってきた音を
複数に分け、それぞれをずらして重ねます。

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_unison_harmony.png" alt="UNISON/HARMONY" style="width:400px;" />
	<figcaption>UNISON/HARMONY</figcaption>
</figure>

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
