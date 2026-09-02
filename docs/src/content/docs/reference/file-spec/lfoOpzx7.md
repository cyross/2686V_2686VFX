---
title: LFO（共通）
description: .lfoOpzx7.json の中身
sidebar:
  order: 14
---

共通の LFO の設定。

| | |
| --- | --- |
| **拡張子** | `.lfoOpzx7.json` / `.lfoOpzx7.yaml` |
| **印（format）** | `opzx7Lfo` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]LFO / [IM]LFO |

## かたち

```json
{
  "format": "opzx7Lfo",
  "version": 1,
  "values": {
    "lfo": {
      "pmEnable": 0,
      "pmFreq": 440.0,
      "pmSyncDelay": 0,
      "pgShape": 0,
      "…": "…"
    }
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

`values` の下に `lfo` があり、その中に並びます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `pmEnable` | bool | — | — |
| `pmFreq` | 小数 | 0 〜 8000 | 440 |
| `pmSyncDelay` | float | — | — |
| `pgShape` | int | — | — |
| `pms` | 小数 | 0 〜 1 | 0 |
| `pmd` | 小数 | 0 〜 1 | 0 |
| `amEnable` | bool | — | — |
| `amFreq` | 小数 | 0 〜 8000 | 440 |
| `egShape` | int | — | — |
| `amSyncDelay` | float | — | — |
| `amSmRt` | float | — | — |
| `ams` | 小数 | 0 〜 1 | 0 |
| `amd` | 小数 | 0 〜 1 | 0 |

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
