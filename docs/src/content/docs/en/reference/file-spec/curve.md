---
title: Curves
description: What is inside curve_*.json
sidebar:
  order: 52
---

The [curve editing (ADV)](/2686V_2686VFX/en/chips/adv/) settings. There is one
file for each combination of Position and Target.

| | |
| --- | --- |
| **Extension** | `curve_<common or op>_<Target>.json` |
| **format** | `curve` |
| **version** | `1` |
| **Export / import** | ADV's [EX] / [IM] |

`<Target>` is one of `RegValue`, `AmpEnv`, `PitchEnv`, `SsgSwEnv256`,
`SsgSwPEnv256` — for example
`MyCurve.curve_op_AmpEnv.json`.

## Shape

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

## Contents

| Key | Type | Range | What it holds |
| --- | --- | --- | --- |
| `logic` | integer | 0 – 30 | How it joins up. Linear, arc, exponential, logarithmic, spline, and combinations |
| `k` | decimal | −50 – 50 | How hard it bends. 1.0 is neutral |
| `values` | A list of decimals | −3.0 to 3.0 each | Waypoints. 16 of them |

How many `params` there are depends on the Target: **6 for RegValue, 4 for
AmpEnv, 3 for PitchEnv, and 12 for SsgSwEnv256 and
SsgSwPEnv256 (named SsgSwEnv11 / SsgSwPEnv11 up to 3.6.3).** On load they go into whichever Position and Target are selected
on screen.

With SsgSwEnv256 selected, the `SsgSwEnv` (6-step, 7 `params`) and `SsgSwEnv11` files of
3.6.3 and earlier are listed too. A 6-step file puts R1–R5 into R1–R5, R6 (the release)
into R11, and LoopTo into LoopTo.

:::caution
Curve settings are **not exposed to automation** — there are far too many slots.
They are saved in presets, so use a preset when you want to swap a whole set.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
