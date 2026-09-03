---
title: 波形メモリによる音量変調
description: .wtampmod.json の中身
sidebar:
  order: 9
---

WT AMP MOD の設定。変調波形は WT PITCH MOD と同じものを使い、当て先が音程では
なく音量になります。出力は `min`〜`max` の間を動きます。

| | |
| --- | --- |
| **拡張子** | `.wtampmod.json` / `.wtampmod.yaml` |
| **表記（format）** | `wtampmod` |
| **版（version）** | `1` |
| **書き出し・読み込み** | WT AMP MOD の [EX] / [IM] |

## かたち

```json
{
  "format": "wtampmod",
  "version": 1,
  "values": {
    "wtAmpMod": {
      "enable": 0,
      "depth": 0.5,
      "speed": 1,
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

`values` の下に `wtAmpMod` があり、その中に並びます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `enable` | bool | — | — |
| `depth` | float | 0.0〜1.0 | 0.5 |
| `speed` | float | — | — |
| `shape` | int | 選択肢の番号 | — |
| `min` | float | 0.0〜1.0 | 0.0 |
| `max` | float | 0.0〜1.0 | 1.0 |
| `waveSmooth` | bool | — | — |
| `table` | int の配列（32 個） | 0〜7 | — |

:::note[書き方について]
- `depth` は「`max` からどれだけ下げるか」の割合です。0.0 なら `max` のまま
  動かず、1.0 で `min`〜`max` の全域を使います。WT PITCH MOD の `depth` が
  0.0 のときに音程が動かないのと同じ考え方です。
- `min` / `max` は波形スロットをまたいで 1 組だけ持ちます。スロットごとの
  設定はありません。
- `table` は FDS AMP TABLE（`shape` が FDS Table のときに使う 32 段の
  レジスタ値）です。中身の意味は
  [波形メモリによる音程変調](/2686V_2686VFX/reference/file-spec/wtmod/) と同じです。
- HuC6280 モードで使う 32 サンプルの波形は、このファイルには入りません。
  波形は `.wt` / `.wt2` から読み込み、場所だけがプラグインの状態に残ります。
- 保存する形は設定で `JSON` と `YAML` を選べます。中身の並びは同じです。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
