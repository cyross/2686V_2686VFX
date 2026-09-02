---
title: ファイル仕様
description: 各ファイルの中身の一覧
sidebar:
  order: 0
---

プラグインが読み書きするファイルの中身をまとめてあります。テキストなので、
エディタで直したり、スクリプトから作ったりできます。

## かたちの決まり

パラメータファイルは、どれも同じ包み方をしています。

```json
{
  "format": "<種類の名前>",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、**合わないファイルは開きません**。中身の鍵だけ
そろえても、この印が無いと何も起きずに終わります。

`values` の下は種類ごとに違います。**書かれていない項目は、いまの値のまま
残ります**（初期値へは戻りません）。必要なところだけ書いたファイルも作れます。

保存する形は設定で `JSON` と `YAML` を選べます。中身の並びは同じです。

## つまみごとのファイル

| ファイル | 中身 |
| --- | --- |
| [`.ampEnv.json`](/2686V_2686VFX/reference/file-spec/ampenv/) | 音量エンベロープ |
| [`.pitchEnv.json`](/2686V_2686VFX/reference/file-spec/pitchenv/) | 音程エンベロープ |
| [`.ssgHwEnv.json`](/2686V_2686VFX/reference/file-spec/ssghwenv/) | SSG ハードウェアエンベロープ |
| [`.ssgSwEnv.json`](/2686V_2686VFX/reference/file-spec/ssgswenv/) | SSG ソフトウェアエンベロープ（6 タップ） |
| [`.ssgSwEnv11.json`](/2686V_2686VFX/reference/file-spec/ssgswenv11/) | 同（11 タップ） |
| [`.ssgSwPEnv11.json`](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) | SSG ソフトウェア音程エンベロープ（11 タップ） |
| [`.wtmod.json`](/2686V_2686VFX/reference/file-spec/wtmod/) | 波形メモリによる音程変調 |
| [`.detune.json`](/2686V_2686VFX/reference/file-spec/detune/) | 逓倍とデチューン |
| [`.unison.json`](/2686V_2686VFX/reference/file-spec/unison/) | ユニゾンとハーモニー |
| [`.quality.json`](/2686V_2686VFX/reference/file-spec/quality/) | 音質 |
| [`.pcmQuality.json`](/2686V_2686VFX/reference/file-spec/pcmquality/) | PCM の音質 |
| [`.pcmPlay.json`](/2686V_2686VFX/reference/file-spec/pcmplay/) | PCM の再生位置 |
| [`.toneNoise.json`](/2686V_2686VFX/reference/file-spec/tonenoise/) | トーンとノイズ |
| [`.lfoOpzx7.json`](/2686V_2686VFX/reference/file-spec/lfoopzx7/) | LFO（共通） |
| [`.opnaHwLfo.json`](/2686V_2686VFX/reference/file-spec/opnahwlfo/) | OPNA ハードウェア LFO |
| [`.lfoN88.json`](/2686V_2686VFX/reference/file-spec/lfon88/) | N88 LFO |
| [`.lfoOpm.json`](/2686V_2686VFX/reference/file-spec/lfoopm/) | OPM LFO |
| [`.lfoOpl.json`](/2686V_2686VFX/reference/file-spec/lfoopl/) | OPL LFO |

## チャンネルのファイル

| ファイル | 中身 |
| --- | --- |
| [`.param.opna.json`](/2686V_2686VFX/reference/file-spec/param-opna/) | OPNA チャンネル |
| [`.param.opn.json`](/2686V_2686VFX/reference/file-spec/param-opn/) | OPN チャンネル |
| [`.param.opl.json`](/2686V_2686VFX/reference/file-spec/param-opl/) | OPL チャンネル |
| [`.param.opl3.json`](/2686V_2686VFX/reference/file-spec/param-opl3/) | OPL3 チャンネル |
| [`.param.opm.json`](/2686V_2686VFX/reference/file-spec/param-opm/) | OPM チャンネル |
| [`.param.opzx7s.json`](/2686V_2686VFX/reference/file-spec/param-opzx7s/) | OPZX7 チャンネル |
| [`.param.ssg.json`](/2686V_2686VFX/reference/file-spec/param-ssg/) | SSG チャンネル |
| [`.param.wt.json`](/2686V_2686VFX/reference/file-spec/param-wt/) | 波形メモリ チャンネル |
| [`.param.wt2.json`](/2686V_2686VFX/reference/file-spec/param-wt2/) | 波形メモリ 2 チャンネル |
| [`.param.wtplus.json`](/2686V_2686VFX/reference/file-spec/param-wtplus/) | 波形メモリ + チャンネル |
| [`.param.beep.json`](/2686V_2686VFX/reference/file-spec/param-beep/) | BEEP チャンネル |
| [`.param.adpcm.json`](/2686V_2686VFX/reference/file-spec/param-adpcm/) | ADPCM チャンネル |
| [`.param.rhythm.json`](/2686V_2686VFX/reference/file-spec/param-rhythm/) | リズム チャンネル |
| [`.param.<音源>.op.json`](/2686V_2686VFX/reference/file-spec/param-op/) | オペレータ 1 つぶん |

## そのほか

| ファイル | 中身 |
| --- | --- |
| [`.2fx.json`](/2686V_2686VFX/reference/file-spec/2fx/) | エフェクトの設定 |
| [`.fxo.json`](/2686V_2686VFX/reference/file-spec/fxo/) | エフェクトの順番 |
| [`curve_*.json`](/2686V_2686VFX/reference/file-spec/curve/) | カーブ |
| [`.ccolor.json`](/2686V_2686VFX/reference/file-spec/ccolor/) | 画面の色 |
| [プリセット](/2686V_2686VFX/reference/file-spec/preset/) | プラグイン全体の状態 |
| [`.wt` / `.wt2`](/2686V_2686VFX/reference/file-spec/wave/) | 波形メモリ（テキスト） |

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
