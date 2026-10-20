---
title: SSG software level envelope (256 steps)
description: What is inside .ssgSwEnv256.json
sidebar:
  order: 6
---

The SSG SW AMP ENV[256] settings (since 4.0.0).

| | |
| --- | --- |
| **Extension** | `.ssgSwEnv256.json` / `.ssgSwEnv256.yaml` |
| **format** | `ssgSwEnv256` |
| **version** | `1` |
| **Export / import** | The section's [EX] / [IM], or EXPORT / IMPORT on the heading's right-click |

## Shape

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

## Contents

| Key | Type | Meaning |
| --- | --- | --- |
| `flag` | bool | Bypass / Enable |
| `steps` | float | STEP (1–256) |
| `loop` / `loopTo` / `loopCount` | bool / float / float | LOOP / LOOP.TO / LOOP.CNT |
| `loopMode` | float | LOOP MODE (0 = Original Mode, 1 = Rich Mode). From 4.1.0; Original Mode if missing |
| `loops` | array of float | The Rich Mode loop sets: START, END and COUNT, three per set, in order. From 4.1.0; no sets if missing |
| `keep` | bool | KEEP |
| `startLevel` | float | STL (0–1) |
| `releaseRate` / `releaseLevel` | float | RR (seconds) / LR (0–1) |
| `endLevel` / `endLevelEnable` | float / bool | ENDL / Use Endl |
| `rates` | array of float | R (seconds) from step 1. Trailing steps at their defaults are left out |
| `levels` | array of float | L (0–1) from step 1. Same as above |

Steps not in the arrays go back to their defaults (R 0.001, L 1).

`.ssgSwEnv11.json` files from 3.6.3 and earlier (`r1`–`r11` / `l1`–`l11`) load too:
entries 1–10 become steps 1–10 and entry 11 becomes RR / LR.
