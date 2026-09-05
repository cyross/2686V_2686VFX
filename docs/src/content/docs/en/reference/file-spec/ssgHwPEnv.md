---
title: SSG hardware pitch envelope
description: What is inside .ssgHwPEnv.json
sidebar:
  order: 4
---

The SSG HW PITCH ENV settings. It uses the same shape slots as SSG HW AMP ENV,
but drives pitch instead of level. `min` and `max` are in cents.

| | |
| --- | --- |
| **Extension** | `.ssgHwPEnv.json` / `.ssgHwPEnv.yaml` |
| **format** | `ssgHwPEnv` |
| **version** | `1` |
| **Export / import** | SSG HW PITCH ENV's [EX] / [IM] |

## Shape

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

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

`ssgHwPEnv` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `envEnable` | bool | — | — |
| `shape` | int | choice number | — |
| `period` | float | — | — |
| `min` | int | -4800 to 4800 (cents) | 0 |
| `max` | int | -4800 to 4800 (cents) | 1200 |
| `smoothEnable` | bool | — | — |

:::note[Writing one by hand]
- `min` and `max` are in cents; 1200 cents is one octave. Swapping them does
  no harm, but the editor keeps the smaller one in `min`.
- Knobs that pick from a list are held as **numbers**. For what each number
  means, see the defaults in the
  [automation reference](/2686V_2686VFX/en/reference/automation/) and the page
  for the chip in question.
- The saved form can be `JSON` or `YAML`, chosen in the settings. The contents
  are laid out the same either way.
- Files **can be read across chips**. One built on SSG can be loaded onto an
  OPNA operator.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
