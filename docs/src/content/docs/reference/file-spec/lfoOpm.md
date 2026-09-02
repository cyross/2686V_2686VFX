---
title: OPM LFO
description: .lfoOpm.json の中身
sidebar:
  order: 17
---

OPM のハードウェア LFO。

| | |
| --- | --- |
| **拡張子** | `.lfoOpm.json` / `.lfoOpm.yaml` |
| **表記（format）** | `opmLfo` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]LFO / [IM]LFO |

## かたち

```json
{
  "format": "opmLfo",
  "version": 1,
  "values": {
    "lfoFreq": 0,
    "lfoSyncDelay": 0,
    "lfoPm": false,
    "lfoPgShape": 0,
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
| `lfoFreq` | 整数 | 0 〜 255 | 0 |
| `lfoSyncDelay` | 整数 | 0 〜 127 | 0 |
| `lfoPm` | 真偽 | false / true | false |
| `lfoPgShape` | 整数 | 0 〜 3（選択肢の番号） | 0 |
| `lfoPms` | 整数 | 0 〜 7（選択肢の番号） | 0 |
| `lfoPmd` | 整数 | 0 〜 127 | 0 |
| `lfoAm` | 真偽 | false / true | false |
| `lfoAmSmRt` | 小数 | 0.005 〜 0.5 | 0.005 |
| `lfoEgShape` | 整数 | 0 〜 3（選択肢の番号） | 0 |
| `lfoAms` | 整数 | 0 〜 3（選択肢の番号） | 0 |
| `lfoAmd` | 整数 | 0 〜 127 | 0 |

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
