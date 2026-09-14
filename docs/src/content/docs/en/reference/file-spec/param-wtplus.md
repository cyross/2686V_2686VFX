---
title: Wavetable+ channel
description: What is inside .param.wtplus.json
sidebar:
  order: 39
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.wtplus.json` / `.param.wtplus.yaml` |
| **format** | `wtPlus` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [wtplus](/2686V_2686VFX/en/chips/wtplus/) |

## Shape

```json
{
  "format": "wtPlus",
  "version": 1,
  "values": {
    "slot": 0,
    "slot0": { "speed": 1.0, "holdEnable": false, ... },
    "slot31": { ... }
  }
}
```

The reader checks `format` at the root and will not open a file that does not
match. **Anything you leave out keeps its current value** — it is not reset to
the default.

## The channel's own knobs

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `slot` | integer | 0 – 31 | 0 |
| `interpolate` | boolean | false / true | true |
| `steps` | integer | 0 – 10 (choice number) | 0 |

## One wave slot — `slot0` through `slot31`

Thirty-two keys named `slot` plus a number. From 3.3.0, playback speed and the hold / partial playback settings are held **per slot**.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `speed` | float | 0.0001 – 100 | 1 |
| `holdEnable` | boolean | false / true | false |
| `holdCount` | integer | 1 – 3000 | 8 |
| `holdTarget` | integer | 0 = MIN / 1 = MAX | 1 |
| `holdMin` | float | 0 – 1 | 0 |
| `holdMax` | float | 0 – 1 | 1 |
| `keepEnable` | boolean | false / true | false |
| `waveStart` | float | 0 – 1 | 0 |
| `keepStart` | boolean | false / true | false |
| `waveEnd` | float | 0 – 1 | 1 |
| `keepEnd` | boolean | false / true | false |

The wave itself is not stored here. Where it was loaded from is remembered by the preset.

## The nested blocks in a channel

| Key | What it holds |
| --- | --- |
| `level` | The channel level and start delay. It holds `level` and `delay` |
| `wtMod` | The same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |
| `fix` | Holding the pitch. It holds `enable` and `freq` |
| `ampEnv` | The same as [ampEnv](/2686V_2686VFX/en/reference/file-spec/ampenv/) |
| `pitchEnv` | The same as [pitchEnv](/2686V_2686VFX/en/reference/file-spec/pitchenv/) |
| `ssgHwEnv` | The same as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `ssgSwEnv` | The same as [ssgSwEnv](/2686V_2686VFX/en/reference/file-spec/ssgswenv/) |
| `ssgSwEnv11` | The same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `ssgSwPEnv11` | The same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |
| `mulDetune` | The same as [mulDetune](/2686V_2686VFX/en/reference/file-spec/detune/) |
| `lfo` | The same as [lfo](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) |
| `quality` | The same as [quality](/2686V_2686VFX/en/reference/file-spec/quality/) |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
