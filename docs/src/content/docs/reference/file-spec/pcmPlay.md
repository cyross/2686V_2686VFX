---
title: PCM の再生位置
description: .pcmPlay.json の中身
sidebar:
  order: 14
---

読み込んだ音声ファイルの鳴らし方。

| | |
| --- | --- |
| **拡張子** | `.pcmPlay.json` / `.pcmPlay.yaml` |
| **表記（format）** | `pcmPlay` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX]PCM Play / [IM]PCM Play |

## かたち

```json
{
  "format": "pcmPlay",
  "version": 1,
  "values": {
    "pcmOffset": 0.0,
    "pcmRatio": 1.0,
    "loopPointEnable": false,
    "loopPointStart": 0.0,
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
| `pcmOffset` | 小数 | 0 〜 2000 | 0 |
| `pcmRatio` | 小数 | 0.05 〜 1 | 1 |
| `loopPointEnable` | 真偽 | false / true | false |
| `loopPointStart` | 小数 | 0 〜 0.999999 | 0 |
| `loopPointEnd` | 小数 | 1e-06 〜 1 | 1 |

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
