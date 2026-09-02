---
title: 波形メモリによる音程変調
description: .wtmod.json の中身
sidebar:
  order: 7
---

WT PITCH MOD の設定。

| | |
| --- | --- |
| **拡張子** | `.wtmod.json` / `.wtmod.yaml` |
| **印（format）** | `wtmod` |
| **版（version）** | `1` |
| **書き出し・読み込み** | WT PITCH MOD の [EX] / [IM] |

## かたち

```json
{
  "format": "wtmod",
  "version": 1,
  "values": {
    "wtMod": {
      "enable": 0,
      "depth": 0,
      "speed": 0,
      "shape": 0,
      "…": "…"
    }
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

`values` の下に `wtMod` があり、その中に並びます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `enable` | bool | — | — |
| `depth` | float | — | — |
| `speed` | float | — | — |
| `shape` | int | — | — |
| `waveSmooth` | bool | — | — |

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
