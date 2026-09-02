---
title: OPM channel
description: What is inside .param.opm.json
sidebar:
  order: 34
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.opm.json` / `.param.opm.yaml` |
| **format** | `opm` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [opm](/2686V_2686VFX/en/chips/opm/) |

## Shape

```json
{
  "format": "opm",
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
| `alg` | integer | 1 – 8 (choice number + 1) | 1 |
| `feedback` | integer | 0 – 7 | 0 |
| `pan` | integer | -1 – 1 | 0 |
| `lfoFreq` | integer | 0 – 255 | 0 |
| `lfoSyncDelay` | integer | 0 – 127 | 0 |
| `lfoPm` | boolean | false / true | false |
| `lfoPgShape` | integer | 1 – 4 (choice number + 1) | 1 |
| `lfoPms` | integer | 1 – 8 (choice number + 1) | 1 |
| `lfoPmd` | integer | 0 – 127 | 0 |
| `lfoAm` | boolean | false / true | false |
| `lfoAmSmRt` | decimal | 0.005 – 0.5 | 0.005 |
| `lfoEgShape` | integer | 1 – 4 (choice number + 1) | 1 |
| `lfoAms` | integer | 1 – 4 (choice number + 1) | 1 |
| `lfoAmd` | integer | 0 – 127 | 0 |

## The nested blocks in a channel

| Key | What it holds |
| --- | --- |
| `level` | The channel level. It holds `level` and nothing else |
| `ssgHwEnv` | The same as [ssgHwEnv](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) |
| `ssgSwEnv11` | The same as [ssgSwEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) |
| `quality` | The same as [quality](/2686V_2686VFX/en/reference/file-spec/quality/) |
| `unison` | The same as [unison](/2686V_2686VFX/en/reference/file-spec/unison/) |
| `ampEnv` | The same as [ampEnv](/2686V_2686VFX/en/reference/file-spec/ampenv/) |
| `ssgSwPEnv11` | The same as [ssgSwPEnv11](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) |
| `wtMod` | The same as [wtMod](/2686V_2686VFX/en/reference/file-spec/wtmod/) |

## The `ops` list

Four operators are listed. Each one carries the following.

| Key | Type | Range | Default |
| --- | --- | --- | ---: |
| `mul` | integer | 1 – 17 (choice number + 1) | 2 |
| `mulRatio` | decimal | 0.01 – 100 | 1 |
| `dt1` | integer | 0 – 7 | 0 |
| `dt2` | integer | 0 – 3 | 0 |
| `ar` | integer | 0 – 31 | 31 |
| `d1r` | integer | 0 – 31 | 31 |
| `d1l` | integer | 0 – 15 | 0 |
| `d2r` | integer | 0 – 31 | 0 |
| `rr` | integer | 0 – 15 | 15 |
| `tl` | integer | 0 – 127 | 0 |
| `ksMode` | integer | 1 – 2 (choice number + 1) | 1 |
| `ks` | integer | 1 – 4 (choice number + 1) | 1 |
| `ksrOPP` | integer | 0 – 3 | 0 |
| `kslOPP` | integer | 0 – 99 | 0 |
| `amsEnable` | boolean | false / true | true |
| `bypass` | boolean | false / true | false |
| `xof` | boolean | false / true | false |
| `kor` | boolean | false / true | false |
| `mask` | boolean | false / true | false |

It also carries the following nested blocks.

| Key | What it holds |
| --- | --- |
| `fix` | Holding the pitch. It holds `enable` and `freq` |
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
