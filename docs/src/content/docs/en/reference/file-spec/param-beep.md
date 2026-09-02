---
title: BEEP channel
description: What is inside .param.beep.json
sidebar:
  order: 40
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.beep.json` / `.param.beep.yaml` |
| **format** | `beep` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [beep](/2686V_2686VFX/en/chips/beep/) |

## Shape

```json
{
  "format": "beep",
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
| `antiAlias` | boolean | false / true | false |
| `timerClock` | integer | 0 – 3 (choice number) | 0 |

## The nested blocks in a channel

| Key | What it holds |
| --- | --- |
| `level` | The channel level. It holds `level` and nothing else |
| `fix` | Holding the pitch. It holds `enable` and `freq` |
| `ampEnv` | The same as [ampEnv](/2686V_2686VFX/en/reference/file-spec/ampenv/) |
| `pitchEnv` | The same as [pitchEnv](/2686V_2686VFX/en/reference/file-spec/pitchenv/) |
| `ssgHwEnv` | The same as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `ssgSwEnv` | The same as [ssgSwEnv](/2686V_2686VFX/en/reference/file-spec/ssgswenv/) |
| `ssgSwEnv11` | The same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `ssgSwPEnv11` | The same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |
| `mulDetune` | The same as [mulDetune](/2686V_2686VFX/en/reference/file-spec/detune/) |
| `lfo` | The same as [lfo](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |
| `wtMod` | The same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
