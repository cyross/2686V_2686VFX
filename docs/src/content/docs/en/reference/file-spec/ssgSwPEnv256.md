---
title: SSG software pitch envelope (256 steps)
description: What is inside .ssgSwPEnv256.json
sidebar:
  order: 6
---

The SSG SW PITCH ENV[256] settings (since 4.0.0).

| | |
| --- | --- |
| **Extension** | `.ssgSwPEnv256.json` / `.ssgSwPEnv256.yaml` |
| **format** | `ssgSwPEnv256` |
| **version** | `1` |
| **Export / import** | The section's [EX] / [IM], or EXPORT / IMPORT on the heading's right-click |

## Shape

```json
{
  "format": "ssgSwPEnv256",
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
| `keep` | bool | KEEP |
| `startLevel` | float | STL (cents) |
| `releaseRate` / `releaseLevel` | float | RR (seconds) / LR (cents) |
| `endLevel` / `endLevelEnable` | float / bool | ENDL / Use Endl |
| `rates` | array of float | R (seconds) from step 1. Trailing steps at their defaults are left out |
| `levels` | array of float | L (cents) from step 1. Same as above |

Steps not in the arrays go back to their defaults (R 0.001, L 0).

`.ssgSwPEnv11.json` files from 3.6.3 and earlier (`r1`–`r11` / `l1`–`l11`) load too:
entries 1–10 become steps 1–10 and entry 11 becomes RR / LR.
