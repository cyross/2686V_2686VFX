---
title: SSG ハードウェア音程エンベロープ
description: .ssgHwPEnv.json の中身
sidebar:
  order: 4
---

SSG HW PITCH ENV の設定。波形スロットは SSG HW AMP ENV と同じものを使い、
当て先が音量ではなく音程になります。`min` / `max` はセントです。

| | |
| --- | --- |
| **拡張子** | `.ssgHwPEnv.json` / `.ssgHwPEnv.yaml` |
| **表記（format）** | `ssgHwPEnv` |
| **版（version）** | `1` |
| **書き出し・読み込み** | SSG HW PITCH ENV の [EX] / [IM] |

## かたち

```json
{
  "format": "ssgHwPEnv",
  "version": 1,
  "values": {
    "ssgHwPEnv": {
      "envEnable": 0,
      "shape": 0,
      "period": 1,
      "min": 0,
      "…": "…"
    }
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。`values` の下に
中身が入ります。**書かれていない項目はそのままの値が残ります**（初期値へは
戻りません）。

## 中身

`values` の下に `ssgHwPEnv` があり、その中に並びます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `envEnable` | bool | — | — |
| `shape` | int | 選択肢の番号 | — |
| `period` | float | — | — |
| `min` | int | -4800〜4800（セント） | 0 |
| `max` | int | -4800〜4800（セント） | 1200 |
| `smoothEnable` | bool | — | — |

:::note[書き方について]
- `min` と `max` はセントです。1200 セントが 1 オクターブになります。
  大小が逆でも壊れませんが、画面では小さい方が `min` に寄せられます。
- 選択肢のつまみは**番号**で持ちます。何番が何かは
  [オートメーション一覧](/2686V_2686VFX/reference/automation/) の初期値と、
  各音源のページを参照してください。
- 保存する形は設定で `JSON` と `YAML` を選べます。中身の並びは同じです。
- ファイルは**音源をまたいで読めます**。SSG で作ったものを OPNA の
  オペレータへ読む、といった使い方ができます。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
