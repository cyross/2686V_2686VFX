---
title: v4.1.0 の変更点
description: 4.1.0 で足したものと変わったところ
sidebar:
  order: 0
---

4.0.1 から 4.1.0 までの変更をまとめます。4.1.0 は、段のエンベロープと TAP SEQUENCER の
**ループ**を広げた版です。

## [256] 系のループに LOOP MODE を足しました

SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] と、同じ造りの **TL ENV・ALG ENV**（FM 音源）、
**DUTY RATIO ENV・PEAK RATIO ENV**（SSG）の LOOP の下に、**LOOP MODE** を置きました。
全チャンネル・オペレーター・パッドと 2686VFX が対象です。LOOP のトグルはこれまでどおりです。

| LOOP MODE | 繰り返し方 |
| --- | --- |
| **Original Mode**（初期値） | これまでの LOOP.TO / LOOP.CNT。最後の段から LOOP.TO へ LOOP.CNT 回戻る |
| **Rich Mode** | **START・END・COUNT** の組を最大 256 個かける。END の段を終えたら START の段へ、COUNT 回戻る（0 は無限） |

初期値は Original Mode なので、これまでのプリセットやパラメータファイルの鳴りは変わりません。

Rich Mode では、組を **LP.TARGET** で選んで START / END / COUNT で直し、**ADD** / **DELETE** で
足し引きします。組は START・END の昇順に並べ、下に No / START / END / COUNT の一覧で出します。
組が増えるたびに区分が縦に伸びます。

- 組は START が先のものから順に効きます（START が同じなら END が先のものから）
- **入れ子**にできます。内側を回し終えてから外側が戻り、外側が戻るたびに内側は数え直します
- **END を跨いだ**組は、先に始まったループを回し終えてから、次のループの START へ進みます
- 入れ子の内側や、跨がれている側を無限（COUNT 0）にすると、先へ進めなくなります。そのとおりに
  鳴るだけですが、一覧の COUNT に橙色の **! INF** が出ます
- 組は段の表と一緒に、プリセット・DAW の状態・チャンネルパラメータ・区分のパラメータファイル
  （`loopMode` / `loops`）へ保存します

くわしくは [LOOP MODE — Rich Mode](/2686V_2686VFX/chips/common/#loop-mode--rich-mode) を参照してください。

## TAP SEQUENCER に LOOP を足しました

TAP AMP SEQUENCER / TAP PITCH SEQUENCER（2686VFX では AMP SEQUENCER / PITCH SEQUENCER）の
STEPS の下に **LOOP** を置きました。上と同じ START・END・COUNT の組（最大 256 個）で、
押すたびに進む段を戻せます。

- 最後の段を使ったら 1 段目へ戻り、ループも数え直します
- INCREMENT は押したときと同じく組に沿って進み、CLEAR STEP（もとの RESET）はループも数え直します。
  DECREMENT はこれまでどおり 1 段ずつ戻ります
- 初期値は切で、これまでの鳴りは変わりません。区分のパラメータファイルにも保存します
  （`loop` / `loops`）

くわしくは [TAP SEQUENCER](/2686V_2686VFX/chips/common/#tap-sequencer) を参照してください。

## TAP SEQUENCER の段の出し方を改め、SKIP を足しました

### いまの段と、開始前

画面で地の色が変わっている段を、「次に使う段」から**いまの段**（最後に鍵で使った段）に改めました。
これまでは押す前から 1 段目を指し、押すと鳴らした段の次を指していたので、鳴っている段とずれて見えました。

- 押す前と、**DAW の再生を止めたあと**は**開始前**で、どの段の地も変えません。次に押すと 1 段目を使います。
  曲の頭から再生し直すたびに、同じ並びで鳴ります
- **RESET** を **CLEAR STEP** に改めました。開始前へ戻し、ループも数え直します。鳴りは RESET と同じで、
  オートメーションの ID（`_RESET`）もそのままです
- INCREMENT / DECREMENT は、いまの段を動かします。開始前からは、INCREMENT は 1 段目、DECREMENT は最後の段へ動きます

### SKIP

RATIO（CENT）の下に **SKIP** を置きました。押すと S.TARGET の段が SKIP になり、その段では何も変えません
（音量 1 倍・音程 0 セント）。段そのものは進みます。

- SKIP の段は、棒が左から右までいっぱいの赤になります。S.TARGET が指しているときは橙です。一覧には「SKIP」と出ます
- もう一度押すか、SKIP のまま RATIO（CENT）を動かすと外れ、棒は元の色に戻ります
- 段の表と一緒に、プリセット・DAW の状態・チャンネルパラメータ・区分のパラメータファイルへ保存します（`skips`）

## 直したもの

### KEEP を入れると、最後の段の値が鳴らなかった

SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] と、同じ造りの TL ENV・ALG ENV・DUTY RATIO ENV・
PEAK RATIO ENV で、KEEP のあいだは最後の段を走り終えても一つ前の段の値のままでした。
L(STEP) が鳴らず、リリースもその値から始まっていました（TL ENV と ALG ENV は常に KEEP なので、
いつもこうなっていました）。

最後の段まで来たら、離すまで L(STEP) を保つ（サステイン）ようにしました。STEPS が 4 なら、
L4 に届いたあとは L4 のまま鳴り、離すと L4 から RR 秒かけて LR へ向かいます。

### TAP SEQUENCER の段が進まない・画面の段とずれる

4.0.0 から、次の 2 つが起きていました。

- **モノ**で音が鳴り残っているとき（レガートや、離してすぐ押し直したとき）は、押しても段が進まなかった
- **ユニゾン**では声の数だけ段が進み、声ごとに違う段の値で鳴っていた。画面の「次に使う段」ともずれた

打鍵 1 回につき 1 段だけ進め、ユニゾンの声すべてへ同じ値を配るようにしました。モノで鍵を離して、
押したままの前の鍵へ戻るときは進めません。2686VFX の
AMP SEQUENCER / PITCH SEQUENCER は元から正しく動いていました。
