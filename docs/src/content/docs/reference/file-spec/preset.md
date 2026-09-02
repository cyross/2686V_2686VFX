---
title: プリセット
description: .2686v.json などの中身
sidebar:
  order: 54
---

プラグイン全体の状態を保存したものです。チャンネルの設定だけでなく、
選んでいるチャンネル・エフェクト・カーブ・音声ファイルの場所まで含みます。

| プラグイン | 拡張子 |
| --- | --- |
| 2686V | `.2686v.json` |
| 2686VLight | `.2686vl.json` |
| 26V | `.26v.json` |
| 86V | `.86v.json` |
| OPZX7S | `.opzx7s.json` |

表記（format）はどれも `preset`、版は `1` です。拡張子が違うので、`Presets`
フォルダへまとめて置いても、そのプラグインのものだけが一覧に出ます。

## かたち

```json
{
  "format": "preset",
  "version": 1,
  "values": {
    "meta": {
      "presetName": "My Sound",
      "presetAuthor": "",
      "presetVersion": "1.0.0",
      "presetComment": "",
      "presetGenre": "",
      "activeModeName": "OPNA",
      "pluginVersion": "3.0.0",
      "adpcmPath": "",
      "rhythmPath0": ""
    },
    "params": {
      "MODE": "0",
      "OPNA_ALG": "4"
    }
  }
}
```

## meta

見出しと、音声ファイルの場所が入ります。

| 鍵 | 内容 |
| --- | --- |
| `presetName` / `presetAuthor` / `presetVersion` | 名前・作者・版 |
| `presetComment` / `presetGenre` | コメントとジャンル |
| `activeModeName` | 保存したときのチャンネル名 |
| `pluginVersion` | 保存したプラグインの版 |
| `adpcmPath` | ADPCM の音声ファイル |
| `rhythmPath0` 〜 `rhythmPath7` | リズムのパッドごとの音声ファイル |
| `opzx7PcmPath0` 〜 / `opzx7WtPath0` 〜 / `opzx7Wt2Path0` 〜 | OPZX7 のオペレータごとの波形 |
| `wtPlusWavePath0` 〜 | WT+ の枠ごとの波形 |

**場所は `Samples` フォルダからの相対で書かれます。** 絶対パスで書かれて
いればそのまま使うので、配布されたプリセットでも、素材をその下へ置けば
そのまま鳴ります。

## params

**オートメーションの名前をそのまま鍵にして**、値を文字で持ちます。
名前・型・範囲は [オートメーション一覧](/2686V_2686VFX/reference/automation/) に
あります。

**書かれていないパラメータは、いまの値のまま残ります。** 使うチャンネルの
ぶんだけを書いたプリセットも作れます。持っていないチャンネルの名前が
書かれていても読み飛ばされるので、2686V で作ったプリセットを 26V で
読むこともできます。

:::note[3.0.0 より前のプリセット]
以前は XML でした。読み込みだけは残してあるので、作り溜めたものはそのまま
使えます。書き出しは新しい形だけです。
[旧形式からの変換](/2686V_2686VFX/files/convert/) を参照してください。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
