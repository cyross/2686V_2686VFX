---
title: PCM の音質
description: .pcmQuality.json の中身
sidebar:
  order: 13
---

QUALITY(PCM) の設定。

| | |
| --- | --- |
| **拡張子** | `.pcmQuality.json` / `.pcmQuality.yaml` |
| **表記（format）** | `pcmQuality` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]Quality / [IM]Quality |

## かたち

```json
{
  "format": "pcmQuality",
  "version": 1,
  "values": {
    "mode": 13,
    "rate": 9,
    "interp": 1,
    "nrResample": false,
    "nrGate": false,
    "nrGateLevel": -60.0,
    "nrLpf": 0
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `mode` | 整数 | 1 〜 21 | 13 |
| `rate` | 整数 | 1 〜 15 | 9 |
| `interp` | 整数 | 0 〜 6 | 1 |
| `nrResample` | bool | — | `false` |
| `nrGate` | bool | — | `false` |
| `nrGateLevel` | 小数 | -96 〜 -24 | -60 |
| `nrLpf` | 整数 | 0 〜 3（0: 切 / 1: 弱 / 2: 中 / 3: 強） | 0 |

`nr` で始まるものはノイズリダクションで、3.6.0 で足しました。3.5.0 までに
書いたファイルには無いので、読んだときはいまの値のまま変わりません。

:::note[書き方について]
- 選択肢のつまみは**番号**で持ちます。表の範囲に「選択肢の番号」と書いてある
  ものがそれです。何番が何かは
  [オートメーション一覧](/2686V_2686VFX/reference/automation/) の初期値と、
  各音源のページを参照してください。
- 保存する形は設定で `JSON` と `YAML` を選べます。中身の並びは同じです。
- ファイルは**音源をまたいで読めます**。SSG で作った AMP ENV を OPNA の
  オペレータへ読む、といった使い方ができます。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
