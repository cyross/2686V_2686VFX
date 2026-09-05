---
title: OPNA hardware LFO
description: What is inside .opnaHwLfo.json
sidebar:
  order: 17
---

The OPNA's per-operator hardware LFO.

| | |
| --- | --- |
| **Extension** | `.opnaHwLfo.json` / `.opnaHwLfo.yaml` |
| **format** | `opnaHwLfo` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]HW LFO / [IM]HW LFO |

## Shape

```json
{
  "format": "opnaHwLfo",
  "version": 1,
  "values": {
    "freq": 0,
    "syncDelay": 0,
    "pm": false,
    "pms": 0,
    "…": "…"
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `freq` | integer | 0 – 7 (choice number) | 0 |
| `syncDelay` | integer | 0 – 255 | 0 |
| `pm` | boolean | false / true | false |
| `pms` | integer | 0 – 7 (choice number) | 0 |
| `am` | boolean | false / true | false |
| `ams` | integer | 0 – 3 (choice number) | 0 |

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
