---
title: エフェクトの順番
description: .fxo.json の中身
sidebar:
  order: 51
---

FX を掛ける順番だけを持つファイルです。

| | |
| --- | --- |
| **拡張子** | `.fxo.json` / `.fxo.yaml` |
| **表記（format）** | `fxOrder` |
| **版（version）** | `1` |
| **書き出し・読み込み** | FX の Order の [EX] / [IM] |

## かたち

```json
{
  "format": "fxOrder",
  "version": 1,
  "values": {
    "order": [
      "filter", "eq3b", "tremolo", "vibrato",
      "modernBitCrusher", "delay", "reverb", "sfcEcho"
    ]
  }
}
```

## 名前

| 名前 | 効果 |
| --- | --- |
| `filter` | フィルター |
| `eq3b` | 3 バンドイコライザー |
| `tremolo` | トレモロ |
| `vibrato` | ビブラート |
| `modernBitCrusher` | ビットクラッシャー |
| `delay` | ディレイ |
| `reverb` | リバーブ |
| `sfcEcho` | SFC エコー |
| `pcmBitCrusher` | PCM ビットクラッシャー（**2686VFX だけ**） |

**番号ではなく名前で持ちます。** 効果の数が違うプラグイン同士でも、位置が
ずれて別の効果として読まれることがありません。

知らない名前は読み飛ばされ、書かれていない効果は後ろへ回されます。
2686VFX で書いたファイルを音源側で読んでも壊れません。

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
