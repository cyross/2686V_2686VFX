---
title: PCM quality
description: What is inside .pcmQuality.json
sidebar:
  order: 13
---

The QUALITY(PCM) settings.

| | |
| --- | --- |
| **Extension** | `.pcmQuality.json` / `.pcmQuality.yaml` |
| **format** | `pcmQuality` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]Quality / [IM]Quality |

## Shape

```json
{
  "format": "pcmQuality",
  "version": 1,
  "values": {
    "mode": 13,
    "rate": 9,
    "interp": 1,
    "nrResample": false,
    "nrGate": false,
    "nrGateLevel": -60.0,
    "nrLpf": 0
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `mode` | integer | 1 – 21 | 13 |
| `rate` | integer | 1 – 15 | 9 |
| `interp` | integer | 0 – 6 | 1 |
| `nrResample` | bool | — | `false` |
| `nrGate` | bool | — | `false` |
| `nrGateLevel` | decimal | -96 – -24 | -60 |
| `nrLpf` | integer | 0 – 3 (0: Off / 1: Light / 2: Medium / 3: Strong) | 0 |

The `nr` keys are noise reduction, added in 3.6.0. Files written by 3.5.0 or
earlier do not have them, so loading such a file keeps the current values.

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
