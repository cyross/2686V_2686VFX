---
title: SSG ソフトウェア音量エンベロープ（256 段）
description: .ssgSwEnv256.json の中身
sidebar:
  order: 6
---

SSG SW AMP ENV[256] の設定（4.0.0 から）。

| | |
| --- | --- |
| **拡張子** | `.ssgSwEnv256.json` / `.ssgSwEnv256.yaml` |
| **表記（format）** | `ssgSwEnv256` |
| **版（version）** | `1` |
| **書き出し・読み込み** | 区分の [EX] / [IM]、見出しの右クリックの EXPORT / IMPORT |

## かたち

```json
{
  "format": "ssgSwEnv256",
  "version": 1,
  "values": {
    "flag": true,
    "steps": 10,
    "rates": [0.001, 0.05, "…"],
    "levels": [1.0, 0.5, "…"],
    "…": "…"
  }
}
```

## 中身

| 鍵 | 型 | 内容 |
| --- | --- | --- |
| `flag` | bool | Bypass / Enable |
| `steps` | float | STEP（1〜256） |
| `loop` / `loopTo` / `loopCount` | bool / float / float | LOOP / LOOP.TO / LOOP.CNT |
| `keep` | bool | KEEP |
| `startLevel` | float | STL（0〜1） |
| `releaseRate` / `releaseLevel` | float | RR（秒） / LR（0〜1） |
| `endLevel` / `endLevelEnable` | float / bool | ENDL / Use Endl |
| `rates` | float の並び | 1 段目からの R（秒）。後ろの初期値のままの段は書かない |
| `levels` | float の並び | 1 段目からの L（0〜1）。同上 |

並びに書かれていない段は初期値（R は 0.001、L は 1）へ戻ります。

3.6.3 までの `.ssgSwEnv11.json`（`r1`〜`r11` / `l1`〜`l11`）も読めます。
1〜10 番目が表の 1〜10 段、11 番目が RR / LR になります。
