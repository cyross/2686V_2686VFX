---
title: ADPCM+ channel
description: What is inside .param.adpcmplus.json
sidebar:
  order: 42
---

Holds the settings for one channel. **All 32 sets of PCM settings go in as they are.**

| | |
| --- | --- |
| **Extension** | `.param.adpcmplus.json` / `.param.adpcmplus.yaml` |
| **format** | `adpcmPlus` |
| **version** | `1` |
| **Write / read** | [EX] / [IM] under UTILITY |
| **Channel page** | [adpcmplus](/2686V_2686VFX/en/chips/adpcmplus/) |

## Shape

```json
{
  "format": "adpcmPlus",
  "version": 1,
  "values": {
    "slot": 0,
    "slot0": { "filePath": "...", "pan": 0.5, ... },
    "slot1": { ... },
    "slot31": { ... }
  }
}
```

The reader checks `format` at the root and will not open a file that does not match. **Anything not written keeps its current value** (it is not reset to the default).

## Channel knobs

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `slot` | integer | 0 – 31 | 0 |

Which PCM sounds. The slot shown on screen (TGT) does not affect the sound, so it is not stored.

## One PCM — `slot0` through `slot31`

Thirty-two keys named `slot` plus a number. Each has the same shape.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `filePath` | string | — | — |
| `tone` | float | 0 – 1 | 1 |
| `noise` | float | 0 – 1 | 1 |
| `noiseFreq` | float | 0 – 30000 | 12000 |
| `mix` | float | 0 – 1 | 0 |
| `pan` | float | 0 – 1 | 0.5 |
| `speed` | float | 0.0001 – 100 | 1 |
| `pcmOffset` | float | 0 – 2000 | 0 |
| `pcmRatio` | float | 0.05 – 1 | 1 |
| `loop` | boolean | false / true | true |
| `loopPointEnable` | boolean | false / true | false |
| `loopPointStart` | float | 0 – 0.999999 | 0 |
| `loopPointEnd` | float | 1e-06 – 1 | 1 |
| `loopCount` | integer | 0 – 3000 | 0 |

`filePath` is **where** the material is, not the material itself. It is read back from there. An empty value leaves that slot empty.

## Nested sections

| Key | What it is |
| --- | --- |
| `level` | Channel level and start delay. Holds `level` and `delay` |
| `fix` | Fixed pitch. Holds `enable` and `freq` |
| `ampEnv` | Same as [ampEnv](/2686V_2686VFX/en/reference/file-spec/ampenv/) |
| `pitchEnv` | Same as [pitchEnv](/2686V_2686VFX/en/reference/file-spec/pitchenv/) |
| `ssgHwEnv` | Same as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `ssgHwPEnv` | Same shape as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `ssgSwEnv` | Same as [ssgSwEnv](/2686V_2686VFX/en/reference/file-spec/ssgswenv/) |
| `ssgSwEnv11` | Same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `ssgSwPEnv11` | Same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |
| `mulDetune` | Same as [mulDetune](/2686V_2686VFX/en/reference/file-spec/detune/) |
| `lfo` | Same as [lfo](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) |
| `qualityPcm` | Same as [qualityPcm](/2686V_2686VFX/en/reference/file-spec/pcmquality/) |
| `unison` | Same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |
| `wtMod` | Same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |
| `wtAmpMod` | Same shape as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |

For where files live, see [file locations](/2686V_2686VFX/en/files/format/).
