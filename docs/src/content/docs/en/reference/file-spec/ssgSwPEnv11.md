---
title: SSG software pitch envelope (11 taps)
description: What is inside .ssgSwPEnv11.json
sidebar:
  order: 7
---

The SSG SW PITCH ENV[11] settings.

| | |
| --- | --- |
| **Extension** | `.ssgSwPEnv11.json` / `.ssgSwPEnv11.yaml` |
| **format** | `ssgSwPEnv11` |
| **version** | `1` |
| **Export / import** | SSG SW PITCH ENV[11]'s [EX] / [IM] |

## Shape

```json
{
  "format": "ssgSwPEnv11",
  "version": 1,
  "values": {
    "ssgSwPEnv11": {
      "flag": 0,
      "steps": 0,
      "loop": 0,
      "loopTo": 0,
      "…": "…"
    }
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

`ssgSwPEnv11` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `flag` | bool | — | — |
| `steps` | float | — | — |
| `loop` | bool | — | — |
| `loopTo` | float | — | — |
| `loopCount` | float | — | — |
| `startLevel` | float | — | — |
| `r1` | float | — | — |
| `l1` | float | — | — |
| `r2` | float | — | — |
| `l2` | float | — | — |
| `r3` | float | — | — |
| `l3` | float | — | — |
| `r4` | float | — | — |
| `l4` | float | — | — |
| `r5` | float | — | — |
| `l5` | float | — | — |
| `r6` | float | — | — |
| `l6` | float | — | — |
| `r7` | float | — | — |
| `l7` | float | — | — |
| `r8` | float | — | — |
| `l8` | float | — | — |
| `r9` | float | — | — |
| `l9` | float | — | — |
| `r10` | float | — | — |
| `l10` | float | — | — |
| `r11` | float | — | — |
| `l11` | float | — | — |

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
