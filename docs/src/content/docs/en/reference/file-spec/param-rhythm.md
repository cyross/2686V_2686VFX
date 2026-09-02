---
title: Rhythm channel
description: What is inside .param.rhythm.json
sidebar:
  order: 42
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.rhythm.json` / `.param.rhythm.yaml` |
| **format** | `rhythm` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [rhythm](/2686V_2686VFX/en/chips/rhythm/) |

## Shape

```json
{
  "format": "rhythm",
  "version": 1,
  "values": { ... }
}
```

The reader checks `format` at the root and will not open a file that does not
match. **Anything you leave out keeps its current value** — it is not reset to
the default.

## The nested blocks in a channel

| Key | What it holds |
| --- | --- |
| `level` | The channel level. It holds `level` and nothing else |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |

## The `pads` list

Eight pads are listed. Each one carries the following.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `filePath` | ? | — | — |
| `vol` | decimal | 0 – 10 | 1 |
| `pan` | decimal | 0 – 1 | 0.5 |
| `oneShot` | boolean | false / true | true |
| `note` | integer | 0 – 127 | 61 |
| `tone` | decimal | 0 – 1 | 1 |
| `noise` | decimal | 0 – 1 | 1 |
| `noiseFreq` | decimal | 0 – 30000 | 12000 |
| `mix` | decimal | 0 – 1 | 0 |
| `pcmOffset` | decimal | 0 – 2000 | 0 |
| `pcmRatio` | decimal | 0.05 – 1 | 1 |
| `loopPointEnable` | boolean | false / true | false |
| `loopPointStart` | decimal | 0 – 0.999999 | 0 |
| `loopPointEnd` | decimal | 1e-06 – 1 | 1 |

It also carries the following nested blocks.

| Key | What it holds |
| --- | --- |
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

:::note
The **file locations** inside `pads` (`filePath` / `pcmFile` /
`wtFile` / `wt2File`) may be written relative to the `Samples` folder.
An absolute path is used as it stands.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
