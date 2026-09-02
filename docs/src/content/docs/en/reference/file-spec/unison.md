---
title: Unison and harmony
description: What is inside .unison.json
sidebar:
  order: 9
---

The UNISON/HARMONY settings.

| | |
| --- | --- |
| **Extension** | `.unison.json` / `.unison.yaml` |
| **format** | `unison` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX]Unison / [IM]Unison |

## Shape

```json
{
  "format": "unison",
  "version": 1,
  "values": {
    "unison": {
      "voices": 0,
      "detune": 0,
      "spread": 0,
      "arpEnable": 0,
      "…": "…"
    }
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

`unison` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `voices` | int | — | — |
| `detune` | int | — | — |
| `spread` | float | — | — |
| `arpEnable` | bool | — | — |
| `arpFreq` | decimal | 0 – 8000 | 440 |
| `arpSmooth` | bool | — | — |

It also carries a list called `paraVoices` — the seven voices from the second on, each with a `distance` (position across the stereo field) and a `detune` (how far it is shifted, in cents).


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
