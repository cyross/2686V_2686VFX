---
title: ADPCM channel
description: What is inside .param.adpcm.json
sidebar:
  order: 41
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.adpcm.json` / `.param.adpcm.yaml` |
| **format** | `adpcm` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [adpcm](/2686V_2686VFX/en/chips/adpcm/) |

## Shape

```json
{
  "format": "adpcm",
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
| `filePath` | ? | — | — |
| `loop` | boolean | false / true | true |
| `tone` | decimal | 0 – 1 | 1 |
| `noise` | decimal | 0 – 1 | 1 |
| `noiseFreq` | decimal | 0 – 30000 | 12000 |
| `mix` | decimal | 0 – 1 | 0 |
| `pan` | decimal | 0 – 1 | 0.5 |
| `pcmOffset` | decimal | 0 – 2000 | 0 |
| `pcmRatio` | decimal | 0.05 – 1 | 1 |
| `loopPointEnable` | boolean | false / true | false |
| `loopPointStart` | decimal | 0 – 0.999999 | 0 |
| `loopPointEnd` | decimal | 1e-06 – 1 | 1 |

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
| `qualityPcm` | The same as [qualityPcm](/2686V_2686VFX/en/reference/file-spec/pcmquality/) |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |
| `wtMod` | The same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
