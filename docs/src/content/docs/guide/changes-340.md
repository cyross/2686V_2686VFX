---
title: v3.4.0 の変更点
description: 3.4.0 で足したものと変わったところ
sidebar:
  order: 5
---

3.3.0 から 3.4.0 までの変更をまとめます。**読み込めなくなるファイルはありません。**
3.3.0 までに作ったプリセットやパラメータファイルは、そのまま読めます。

## FM 音源と OPZX7S のあいだで音色を受け渡す

**FM 音源で作った音色を、OPZX7S の音色へ直して持っていけるように
しました。** ボタンは 2 つあり、どちらも同じ直し方をします。

| ボタン | 置き場所 | 入っているプラグイン |
| --- | --- | --- |
| **[EX]OPZX7S Params** | OPNA / OPN / OPL / OPL3 / OPM の UTILITY | 2686V・2686VLight・26V・86V・OPNV・OPLV・OPMV |
| **[IM]FM Params** | OPZX7S の UTILITY | 2686V・2686VLight・OPZX7S |

- **[EX]OPZX7S Params** は、いまのチャンネルを OPZX7S の CH Params ファイル
  として書き出します
- **[IM]FM Params** は、OPNA / OPN / OPL / OPL3 / OPM の CH Params ファイルを
  選び、OPZX7S の値へ直して画面へ読み込みます。**OPZX7S プラグインは FM 音源を
  持っていませんが、ほかのプラグインで保存したファイルを読めます**
- 読み込んだあとは、そのまま **OPZX7S の CH Params として保存できます。**
  名前は元のファイルから付けます

直し方の要点は次のとおりです。

- アルゴリズムは、同じつなぎ方の**アルゴリズムマトリックス**にします
- 使うオペレーターの AMP ENV は**実数モード**にし、実機のレート表で秒と
  レベルへ直します
- LFO は OPZX7S の LFO へ、同じ速さ・深さで入れます
- 元の音源に無いオペレーターは、どこにもつながず初期値を入れます

同じ動きにできない項目（DT1、キースケールの効き方、10 秒を超える段など）は
近い値へ寄せ、**寄せたものを最後にダイアログで知らせます。**

何をどう直すかの一覧は
[FM 音源と OPZX7S の受け渡し](/2686V_2686VFX/chips/common/#fm-音源と-opzx7s-の受け渡し)
を参照してください。

## 大区分 [[AMP ENV]] / [[PITCH ENV]] を足しました

チャンネル全体に掛かる区分のうち、音量にかかわるものを **[[AMP ENV]]**、
音程にかかわるものを **[[PITCH ENV]]** の下へまとめました。区分が縦に
長く並び、目で追いにくくなっていたためです。

<figure class="shot">
	<img src="/2686V_2686VFX/ui/parts/major_category.png" alt="大区分" style="width:212px;" />
	<figcaption>SSG の [[AMP ENV]] を開いたところ</figcaption>
</figure>

| 大区分 | 中に入る区分 |
| --- | --- |
| **[[AMP ENV]]** | AMP ENV / SSG HW AMP ENV / SSG SW AMP ENV / SSG SW AMP ENV[11] / WT AMP MOD |
| **[[PITCH ENV]]** | PITCH ENV / SSG HW PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD |

- 対象は OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / SSG / WT / WT2 / WT+ /
  ADPCM / ADPCM+ / BEEP のタブです。区分の並び順は変えていません
- 見出しの下地はオフホワイトです。ランプの色は変わりません
- **最初は閉じています。** OP / CL は、中の区分に合わせて大区分も開閉します
- 閉じても**音は変わりません**

詳しくは [大区分](/2686V_2686VFX/guide/screen/#大区分--amp-env--pitch-env)
を参照してください。

## 直したもの

3.3.0 までにあった不具合です。

- **TARGET が指していないオペレーターやパッドへ値を入れると、スライダーと
  選ぶ部品の値が入っていませんでした。** 入り切りの札だけは入っていました。
  チャンネル全体の CH Params の読み込みや、オペレーター・パッドの貼り付けが
  当たります。3.3.0 でつまみを TARGET で差し替える作りにしたときからです
- **OPN / OPL / OPL3 / OPM / OPZX7S の [EX]CH Params で、すべてのオペレーターへ
  TARGET のオペレーターの値が書かれていました。** 3.0.0 より前の形式の読み込みと、
  OPL3 の [IM]OPL CH Params では、読んだ値が TARGET のオペレーターへ重ねて
  入っていました。これも 3.3.0 からです
- **OPL3 のタブで、左の枠（LEVEL から UTILITY まで）が表示されていません
  でした。** 3.3.0 でタブを作り直したときからです
- **ファイルを読み込んだあとに書き出しのブラウザを開くと、名前の欄と保存ボタンが
  出ていませんでした**
