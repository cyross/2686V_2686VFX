---
title: N88 LFO
description: .lfoN88.json の中身
sidebar:
  order: 16
---

N88-BASIC 由来のソフトウェア LFO。

| | |
| --- | --- |
| **拡張子** | `.lfoN88.json` / `.lfoN88.yaml` |
| **印（format）** | `n88Lfo` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]LFO / [IM]LFO |

## かたち

```json
{
  "format": "n88Lfo",
  "version": 1,
  "values": {
    "lfoFreq": 0,
    "lfoShape": 0,
    "lfoSyncDelay": 0,
    "lfoPm": false,
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
| `lfoFreq` | 整数 | 0 〜 16383 | 0 |
| `lfoShape` | 整数 | 0 〜 5（選択肢の番号） | 0 |
| `lfoSyncDelay` | 整数 | 0 〜 255 | 0 |
| `lfoPm` | 真偽 | false / true | false |
| `lfoPms` | 整数 | 0 〜 15 | 0 |
| `lfoPmd` | 整数 | -127 〜 127 | 0 |
| `lfoAm` | 真偽 | false / true | false |
| `lfoAmSmRt` | 小数 | 0.005 〜 0.5 | 0.005 |
| `lfoAmd` | 整数 | -127 〜 127 | 0 |

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
