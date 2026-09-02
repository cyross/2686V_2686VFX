---
title: SSG hardware level envelope
description: What is inside .ssgHwEnv.json
sidebar:
  order: 3
---

SSG HW The AMP ENV settings.

| | |
| --- | --- |
| **Extension** | `.ssgHwEnv.json` / `.ssgHwEnv.yaml` |
| **format** | `ssgHwEnv` |
| **version** | `1` |
| **Export / import** | SSG HW AMP ENV's [EX] / [IM] |

## Shape

```json
{
  "format": "ssgHwEnv",
  "version": 1,
  "values": {
    "ssgHwEnv": {
      "envEnable": 0,
      "shape": 0,
      "period": 0,
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

`ssgHwEnv` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `envEnable` | bool | — | — |
| `shape` | int | — | — |
| `period` | float | — | — |
| `min` | float | — | — |
| `max` | float | — | — |
| `smoothEnable` | bool | — | — |

:::note[Writing one by hand]
- Knobs that pick from a list are held as **numbers**. Those are the ones whose
  range reads “choice number” in the table below. For what each number means,
  see the defaults in the
  [automation reference](/2686V_2686VFX/en/reference/automation/) and the page
  for the chip in question.
- The saved form can be `JSON` or `YAML`, chosen in the settings. The contents
  are laid out the same either way.
- Files **can be read across chips**. An AMP ENV built on SSG can be loaded
  onto an OPNA operator.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
