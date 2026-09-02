---
title: N88 LFO
description: What is inside .lfoN88.json
sidebar:
  order: 16
---

The software LFO that came from N88-BASIC.

| | |
| --- | --- |
| **Extension** | `.lfoN88.json` / `.lfoN88.yaml` |
| **format** | `n88Lfo` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]LFO / [IM]LFO |

## Shape

```json
{
  "format": "n88Lfo",
  "version": 1,
  "values": {
    "lfoFreq": 0,
    "lfoShape": 0,
    "lfoSyncDelay": 0,
    "lfoPm": false,
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
| `lfoFreq` | integer | 0 – 16383 | 0 |
| `lfoShape` | integer | 0 – 5 (choice number) | 0 |
| `lfoSyncDelay` | integer | 0 – 255 | 0 |
| `lfoPm` | boolean | false / true | false |
| `lfoPms` | integer | 0 – 15 | 0 |
| `lfoPmd` | integer | -127 – 127 | 0 |
| `lfoAm` | boolean | false / true | false |
| `lfoAmSmRt` | decimal | 0.005 – 0.5 | 0.005 |
| `lfoAmd` | integer | -127 – 127 | 0 |

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
