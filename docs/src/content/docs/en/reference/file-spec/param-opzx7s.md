---
title: OPZX7 channel
description: What is inside .param.opzx7s.json
sidebar:
  order: 35
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.opzx7s.json` / `.param.opzx7s.yaml` |
| **format** | `opzx7` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [opzx7](/2686V_2686VFX/en/chips/opzx7/) |

## Shape

```json
{
  "format": "opzx7",
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
| `algMode` | int | — | — |
| `alg` | integer | 1 – 130 (choice number + 1) | 16 |
| `feedback1` | decimal | -8 – 8 | 0 |
| `feedback2` | decimal | -8 – 8 | 0 |
| `feedback3` | decimal | -8 – 8 | 0 |
| `feedback4` | decimal | -8 – 8 | 0 |
| `feedback5` | decimal | -8 – 8 | 0 |
| `feedback6` | decimal | -8 – 8 | 0 |
| `feedback7` | decimal | -8 – 8 | 0 |
| `feedback8` | decimal | -8 – 8 | 0 |
| `panpotEnable` | boolean | false / true | false |
| `panpot` | integer | 0 – 31 | 15 |

## The nested blocks in a channel

| Key | What it holds |
| --- | --- |
| `level` | The channel level. It holds `level` and nothing else |
| `algMatrixComp` | The algorithm matrix. Its `ops` list holds `isCarrier` and the `mod` / `fbMod` lists |
| `ssgHwEnv` | The same as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `ssgSwEnv11` | The same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `glLfo` | The same as [glLfo](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) |
| `quality` | The same as [quality](/2686V_2686VFX/en/reference/file-spec/quality/) |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |
| `ampEnv` | The same as [ampEnv](/2686V_2686VFX/en/reference/file-spec/ampenv/) |
| `ssgSwPEnv11` | The same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |
| `wtMod` | The same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |

## The `ops` list

Eight operators are listed. Each one carries the following.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `rgEn` | boolean | false / true | true |
| `rgAr` | integer | 0 – 31 | 31 |
| `rgD1r` | integer | 0 – 31 | 31 |
| `rgD1l` | integer | 0 – 15 | 0 |
| `rgD2r` | integer | 0 – 31 | 0 |
| `rgRr` | integer | 0 – 31 | 31 |
| `rgTl` | integer | 0 – 63 | 0 |
| `ar` | decimal | 0.001 – 10 | 0.001 |
| `d1r` | decimal | 0.001 – 10 | 0.001 |
| `d1l` | decimal | 0 – 1 | 1 |
| `d2r` | decimal | 0.001 – 10 | 0.001 |
| `rr` | decimal | 0.001 – 10 | 0.001 |
| `tl` | decimal | 0 – 1 | 1 |
| `ksEn` | boolean | false / true | false |
| `ksMode` | integer | 1 – 3 (choice number + 1) | 1 |
| `ksrMA7` | boolean | false / true | false |
| `kslMA7` | integer | 1 – 4 (choice number + 1) | 1 |
| `ksrOPZ` | integer | 0 – 3 | 0 |
| `kslOPZ` | integer | 0 – 99 | 0 |
| `ksBp` | integer | 0 – 127 | 60 |
| `ksLc` | integer | 1 – 4 (choice number + 1) | 1 |
| `ksRc` | integer | 1 – 4 (choice number + 1) | 1 |
| `ksLd` | decimal | 0 – 1 | 0 |
| `ksRd` | decimal | 0 – 1 | 0 |
| `ksRs` | integer | 0 – 7 | 0 |
| `bypass` | boolean | false / true | false |
| `sus` | boolean | false / true | false |
| `xof` | boolean | false / true | false |
| `kor` | boolean | false / true | false |
| `mask` | boolean | false / true | false |
| `ws` | integer | 1 – 73 (choice number + 1) | 1 |
| `pcmFile` | ? | — | — |
| `wtFile` | ? | — | — |
| `wt2File` | ? | — | — |
| `pcmOffset` | decimal | 0 – 2000 | 0 |
| `pcmRatio` | decimal | 0.05 – 1 | 1 |
| `loopPointEnable` | boolean | false / true | false |
| `loopPointStart` | decimal | 0 – 0.999999 | 0 |
| `loopPointEnd` | decimal | 1e-06 – 1 | 1 |
| `se` | integer | 1 – 16 (choice number + 1) | 1 |
| `seFreq` | decimal | 0.1 – 30 | 1 |

It also carries the following nested blocks.

| Key | What it holds |
| --- | --- |
| `mulDetune` | The same as [mulDetune](/2686V_2686VFX/en/reference/file-spec/detune/) |
| `fix` | Holding the pitch. It holds `enable` and `freq` |
| `lfo` | The same as [lfo](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) |
| `pitchEnv` | The same as [pitchEnv](/2686V_2686VFX/en/reference/file-spec/pitchenv/) |
| `ssgSwEnv` | The same as [ssgSwEnv](/2686V_2686VFX/en/reference/file-spec/ssgswenv/) |
| `ssgSwEnv11` | The same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `ssgSwPEnv11` | The same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |

:::note
The **file locations** inside `ops` (`filePath` / `pcmFile` /
`wtFile` / `wt2File`) may be written relative to the `Samples` folder.
An absolute path is used as it stands.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
