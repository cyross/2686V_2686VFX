---
title: Wavetable level modulation
description: What is inside .wtampmod.json
sidebar:
  order: 9
---

The WT AMP MOD settings. It uses the same modulation shapes as WT PITCH MOD,
but drives level instead of pitch. The output moves between `min` and `max`.

| | |
| --- | --- |
| **Extension** | `.wtampmod.json` / `.wtampmod.yaml` |
| **format** | `wtampmod` |
| **version** | `1` |
| **Export / import** | WT AMP MOD's [EX] / [IM] |

## Shape

```json
{
  "format": "wtampmod",
  "version": 1,
  "values": {
    "wtAmpMod": {
      "enable": 0,
      "depth": 0.5,
      "speed": 1,
      "shape": 0,
      "…": "…"
    }
  }
}
```

The reader checks `format` at the root and **will not open a file that does
not match**. The contents sit under `values`. **Anything you leave out keeps
its current value** — it is not reset to the default.

## Contents

`wtAmpMod` sits under `values`, and the keys are inside it.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `enable` | bool | — | — |
| `depth` | float | 0.0 to 1.0 | 0.5 |
| `speed` | float | — | — |
| `shape` | int | choice number | — |
| `min` | float | 0.0 to 1.0 | 0.0 |
| `max` | float | 0.0 to 1.0 | 1.0 |
| `waveSmooth` | bool | — | — |
| `table` | array of 32 ints | 0 to 7 | — |

:::note[Writing one by hand]
- `depth` is how far the level is pulled down from `max`. At 0.0 nothing moves
  and the level stays at `max`; at 1.0 the whole `min` to `max` range is used.
  This mirrors WT PITCH MOD, where a `depth` of 0.0 leaves the pitch alone.
- `min` and `max` are held once for the whole modulator, not per wave slot.
- `table` is the FDS AMP TABLE — the 32 register values used when `shape` is
  FDS Table. The values mean the same thing as in
  [Wavetable pitch modulation](/2686V_2686VFX/en/reference/file-spec/wtmod/).
- The 32-sample wave used by the HuC6280 mode is not part of this file. Waves
  are loaded from `.wt` / `.wt2`, and only their location is kept in the
  plugin state.
- The saved form can be `JSON` or `YAML`, chosen in the settings. The contents
  are laid out the same either way.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
