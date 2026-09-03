---
title: Multiple and detune
description: What is inside .detune.json
sidebar:
  order: 10
---

The MUL/DET settings.

| | |
| --- | --- |
| **Extension** | `.detune.json` / `.detune.yaml` |
| **format** | `detune` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]Detune / [IM]Detune |

## Shape

```json
{
  "format": "detune",
  "version": 1,
  "values": {
    "mulDetune": {
      "mul": 2,
      "mulRatio": 1.0,
      "dt1": 0,
      "dt2": 0,
      "…": "…"
    }
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

`mulDetune` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `mul` | integer | 0 – 21 (choice number) | 2 |
| `mulRatio` | decimal | 0.01 – 100 | 1 |
| `dt1` | int | — | — |
| `dt2` | integer | 0 – 3 | 0 |
| `dt3` | integer | -4800 – 4800 | 0 |

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
