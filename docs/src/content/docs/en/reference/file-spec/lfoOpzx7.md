---
title: LFO (shared)
description: What is inside .lfoOpzx7.json
sidebar:
  order: 14
---

The settings for the shared LFO.

| | |
| --- | --- |
| **Extension** | `.lfoOpzx7.json` / `.lfoOpzx7.yaml` |
| **format** | `opzx7Lfo` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]LFO / [IM]LFO |

## Shape

```json
{
  "format": "opzx7Lfo",
  "version": 1,
  "values": {
    "lfo": {
      "pmEnable": 0,
      "pmFreq": 440.0,
      "pmSyncDelay": 0,
      "pgShape": 0,
      "…": "…"
    }
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

`lfo` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `pmEnable` | bool | — | — |
| `pmFreq` | decimal | 0 – 8000 | 440 |
| `pmSyncDelay` | float | — | — |
| `pgShape` | int | — | — |
| `pms` | decimal | 0 – 1 | 0 |
| `pmd` | decimal | 0 – 1 | 0 |
| `amEnable` | bool | — | — |
| `amFreq` | decimal | 0 – 8000 | 440 |
| `egShape` | int | — | — |
| `amSyncDelay` | float | — | — |
| `amSmRt` | float | — | — |
| `ams` | decimal | 0 – 1 | 0 |
| `amd` | decimal | 0 – 1 | 0 |

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
