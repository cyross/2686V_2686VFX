---
title: Tone and noise
description: What is inside .toneNoise.json
sidebar:
  order: 13
---

The TONE / NOISE / MIX settings.

| | |
| --- | --- |
| **Extension** | `.toneNoise.json` / `.toneNoise.yaml` |
| **format** | `toneNoise` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]Tone/Noise / [IM]Tone/Noise |

## Shape

```json
{
  "format": "toneNoise",
  "version": 1,
  "values": {
    "tone": 1.0,
    "noise": 1.0,
    "noiseFreq": 12000.0,
    "mix": 0.0
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `tone` | decimal | 0 – 1 | 1 |
| `noise` | decimal | 0 – 1 | 1 |
| `noiseFreq` | decimal | 0 – 30000 | 12000 |
| `mix` | decimal | 0 – 1 | 0 |

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
