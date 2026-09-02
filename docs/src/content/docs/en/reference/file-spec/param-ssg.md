---
title: SSG channel
description: What is inside .param.ssg.json
sidebar:
  order: 36
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.ssg.json` / `.param.ssg.yaml` |
| **format** | `ssg` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [ssg](/2686V_2686VFX/en/chips/ssg/) |

## Shape

```json
{
  "format": "ssg",
  "version": 1,
  "values": { ... }
}
```

The reader checks `format` at the root and will not open a file that does not
match. **Anything you leave out keeps its current value** — it is not reset to
the default.

## The channel's own knobs

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `wave` | integer | 0 – 1 (choice number) | 0 |
| `tone` | decimal | 0 – 1 | 1 |
| `noise` | decimal | 0 – 1 | 1 |
| `noiseFreq` | decimal | 0 – 30000 | 12000 |
| `noiseOnNote` | boolean | false / true | false |
| `mix` | decimal | 0 – 1 | 0 |
| `dutyMode` | integer | 0 – 1 (choice number) | 0 |
| `dutyPreset` | integer | 0 – 8 (choice number) | 0 |
| `dutyVar` | decimal | 0 – 0.5 | 0.5 |
| `dutyInvert` | boolean | false / true | false |
| `dutyFc` | boolean | false / true | false |
| `dutyFcFluc` | decimal | 0 – 1 | 0.2 |
| `triKeyTrack` | boolean | false / true | true |
| `triFreq` | decimal | 0 – 8000 | 440 |
| `triPeak` | decimal | 0 – 1 | 0.5 |

## The nested blocks in a channel

| Key | What it holds |
| --- | --- |
| `level` | The channel level. It holds `level` and nothing else |
| `ssgHwEnv` | The same as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `fix` | Holding the pitch. It holds `enable` and `freq` |
| `ampEnv` | The same as [ampEnv](/2686V_2686VFX/en/reference/file-spec/ampenv/) |
| `pitchEnv` | The same as [pitchEnv](/2686V_2686VFX/en/reference/file-spec/pitchenv/) |
| `ssgSwEnv` | The same as [ssgSwEnv](/2686V_2686VFX/en/reference/file-spec/ssgswenv/) |
| `ssgSwEnv11` | The same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `ssgSwPEnv11` | The same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |
| `mulDetune` | The same as [mulDetune](/2686V_2686VFX/en/reference/file-spec/detune/) |
| `lfo` | The same as [lfo](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) |
| `quality` | The same as [quality](/2686V_2686VFX/en/reference/file-spec/quality/) |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |
| `wtMod` | The same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
