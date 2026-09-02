---
title: SSG ソフトウェア音量エンベロープ（11 タップ）
description: .ssgSwEnv11.json の中身
sidebar:
  order: 5
---

SSG SW AMP ENV[11] の設定。

| | |
| --- | --- |
| **拡張子** | `.ssgSwEnv11.json` / `.ssgSwEnv11.yaml` |
| **表記（format）** | `ssgSwEnv11` |
| **版（version）** | `1` |
| **書き出し・読み込み** | SSG SW AMP ENV[11] の [EX] / [IM] |

## かたち

```json
{
  "format": "ssgSwEnv11",
  "version": 1,
  "values": {
    "ssgSwEnv11": {
      "flag": 0,
      "steps": 0,
      "loop": 0,
      "loopTo": 0,
      "…": "…"
    }
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

`values` の下に `ssgSwEnv11` があり、その中に並びます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `flag` | bool | — | — |
| `steps` | float | — | — |
| `loop` | bool | — | — |
| `loopTo` | float | — | — |
| `loopCount` | float | — | — |
| `startLevel` | float | — | — |
| `r1` | float | — | — |
| `l1` | float | — | — |
| `r2` | float | — | — |
| `l2` | float | — | — |
| `r3` | float | — | — |
| `l3` | float | — | — |
| `r4` | float | — | — |
| `l4` | float | — | — |
| `r5` | float | — | — |
| `l5` | float | — | — |
| `r6` | float | — | — |
| `l6` | float | — | — |
| `r7` | float | — | — |
| `l7` | float | — | — |
| `r8` | float | — | — |
| `l8` | float | — | — |
| `r9` | float | — | — |
| `l9` | float | — | — |
| `r10` | float | — | — |
| `l10` | float | — | — |
| `r11` | float | — | — |
| `l11` | float | — | — |

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
