---
title: OPL LFO
description: What is inside .lfoOpl.json
sidebar:
  order: 20
---

OPL's per-operator LFO.

| | |
| --- | --- |
| **Extension** | `.lfoOpl.json` / `.lfoOpl.yaml` |
| **format** | `oplLfo` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]LFO / [IM]LFO |

## Shape

```json
{
  "format": "oplLfo",
  "version": 1,
  "values": {
    "vib": false,
    "pms": 6.4,
    "pmd": 7.0,
    "am": false,
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
| `vib` | boolean | false / true | false |
| `pms` | decimal | 1 – 20 | 6.4 |
| `pmd` | decimal | 1 – 100 | 7 |
| `am` | boolean | false / true | false |
| `ams` | decimal | 1 – 20 | 3.7 |
| `amd` | decimal | 0.1 – 10 | 1 |

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
