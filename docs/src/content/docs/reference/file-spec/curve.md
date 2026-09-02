---
title: カーブ
description: curve_*.json の中身
sidebar:
  order: 52
---

[カーブ編集（ADV）](/2686V_2686VFX/chips/adv/) の設定です。Position と Target の
組み合わせごとに 1 ファイルになります。

| | |
| --- | --- |
| **拡張子** | `curve_<common または op>_<Target>.json` |
| **表記（format）** | `curve` |
| **版（version）** | `1` |
| **書き出し・読み込み** | ADV の [EX] / [IM] |

`<Target>` は `RegValue` `AmpEnv` `PitchEnv` `SsgSwEnv` `SsgSwEnv11`
`SsgSwPEnv11` のどれかです。たとえば
`MyCurve.curve_op_AmpEnv.json` のようになります。

## かたち

```json
{
  "format": "curve",
  "version": 1,
  "values": {
    "params": [
      { "logic": 0, "k": 1.0, "values": [0.0, 0.0, "…"] }
    ]
  }
}
```

## 中身

| 鍵 | 型 | 範囲 | 内容 |
| --- | --- | --- | --- |
| `logic` | 整数 | 0 〜 30 | つなぎ方。線形・円弧・指数・対数・スプラインとその組み合わせ |
| `k` | 小数 | −50 〜 50 | 曲がり具合。1.0 でくせがない |
| `values` | 小数の並び | 各 −3.0 〜 3.0 | 通過ポイント。16 個 |

`params` の数は Target で決まります。**RegValue は 6、AmpEnv は 4、
PitchEnv は 3、SsgSwEnv は 7、SsgSwEnv11 と SsgSwPEnv11 は 12** です。
読み込むときは、画面で選んでいる Position と Target へ入ります。

:::caution
カーブの設定は**オートメーションには出ません**。枠の数が多すぎるためです。
プリセットには保存されるので、まとめて差し替えるにはプリセットを使います。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
