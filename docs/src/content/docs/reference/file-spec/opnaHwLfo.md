---
title: OPNA ハードウェア LFO
description: .opnaHwLfo.json の中身
sidebar:
  order: 15
---

OPNA のオペレータごとのハードウェア LFO。

| | |
| --- | --- |
| **拡張子** | `.opnaHwLfo.json` / `.opnaHwLfo.yaml` |
| **印（format）** | `opnaHwLfo` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]HW LFO / [IM]HW LFO |

## かたち

```json
{
  "format": "opnaHwLfo",
  "version": 1,
  "values": {
    "freq": 0,
    "syncDelay": 0,
    "pm": false,
    "pms": 0,
    "…": "…"
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `freq` | 整数 | 0 〜 7（選択肢の番号） | 0 |
| `syncDelay` | 整数 | 0 〜 255 | 0 |
| `pm` | 真偽 | false / true | false |
| `pms` | 整数 | 0 〜 7（選択肢の番号） | 0 |
| `am` | 真偽 | false / true | false |
| `ams` | 整数 | 0 〜 3（選択肢の番号） | 0 |

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
