---
title: PCM playback position
description: What is inside .pcmPlay.json
sidebar:
  order: 12
---

How a loaded audio file is played.

| | |
| --- | --- |
| **Extension** | `.pcmPlay.json` / `.pcmPlay.yaml` |
| **format** | `pcmPlay` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]PCM Play / [IM]PCM Play |

## Shape

```json
{
  "format": "pcmPlay",
  "version": 1,
  "values": {
    "pcmOffset": 0.0,
    "pcmRatio": 1.0,
    "loopPointEnable": false,
    "loopPointStart": 0.0,
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
| `pcmOffset` | decimal | 0 – 2000 | 0 |
| `pcmRatio` | decimal | 0.05 – 1 | 1 |
| `loopPointEnable` | boolean | false / true | false |
| `loopPointStart` | decimal | 0 – 0.999999 | 0 |
| `loopPointEnd` | decimal | 1e-06 – 1 | 1 |

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
