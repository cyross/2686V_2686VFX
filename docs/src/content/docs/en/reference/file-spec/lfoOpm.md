---
title: OPM LFO
description: What is inside .lfoOpm.json
sidebar:
  order: 19
---

The OPM's hardware LFO.

| | |
| --- | --- |
| **Extension** | `.lfoOpm.json` / `.lfoOpm.yaml` |
| **format** | `opmLfo` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]LFO / [IM]LFO |

## Shape

```json
{
  "format": "opmLfo",
  "version": 1,
  "values": {
    "lfoFreq": 0,
    "lfoSyncDelay": 0,
    "lfoPm": false,
    "lfoPgShape": 0,
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
| `lfoFreq` | integer | 0 – 255 | 0 |
| `lfoSyncDelay` | integer | 0 – 127 | 0 |
| `lfoPm` | boolean | false / true | false |
| `lfoPgShape` | integer | 0 – 3 (choice number) | 0 |
| `lfoPms` | integer | 0 – 7 (choice number) | 0 |
| `lfoPmd` | integer | 0 – 127 | 0 |
| `lfoAm` | boolean | false / true | false |
| `lfoAmSmRt` | decimal | 0.005 – 0.5 | 0.005 |
| `lfoEgShape` | integer | 0 – 3 (choice number) | 0 |
| `lfoAms` | integer | 0 – 3 (choice number) | 0 |
| `lfoAmd` | integer | 0 – 127 | 0 |

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
