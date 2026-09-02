---
title: ユニゾンとハーモニー
description: .unison.json の中身
sidebar:
  order: 9
---

UNISON/HARMONY の設定。

| | |
| --- | --- |
| **拡張子** | `.unison.json` / `.unison.yaml` |
| **印（format）** | `unison` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]Unison / [IM]Unison |

## かたち

```json
{
  "format": "unison",
  "version": 1,
  "values": {
    "unison": {
      "voices": 0,
      "detune": 0,
      "spread": 0,
      "arpEnable": 0,
      "…": "…"
    }
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

`values` の下に `unison` があり、その中に並びます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `voices` | int | — | — |
| `detune` | int | — | — |
| `spread` | float | — | — |
| `arpEnable` | bool | — | — |
| `arpFreq` | 小数 | 0 〜 8000 | 440 |
| `arpSmooth` | bool | — | — |

さらに `paraVoices` という並びを持ちます。2 声目以降の 7 声ぶんで、それぞれ `distance`（左右の位置）と `detune`（ずらし幅・セント）を持ちます。


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
