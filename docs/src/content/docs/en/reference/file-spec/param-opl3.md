---
title: OPL3 channel
description: What is inside .param.opl3.json
sidebar:
  order: 33
---

Holds the settings for one whole channel.

| | |
| --- | --- |
| **Extension** | `.param.opl3.json` / `.param.opl3.yaml` |
| **format** | `opl3` |
| **version** | `1` |
| **Export / import** | UTILITY's [EX] / [IM] |
| **The chip's page** | [opl3](/2686V_2686VFX/en/chips/opl3/) |

## Shape

```json
{
  "format": "opl3",
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
| `mul` | integer | 1 – 16 (choice number + 1) | 2 |
| `ar` | integer | 0 – 15 | 15 |
| `dr` | integer | 0 – 15 | 15 |
| `sl` | integer | 0 – 15 | 0 |
| `rr` | integer | 0 – 15 | 15 |
| `tl` | integer | 0 – 63 | 0 |
| `ksr` | boolean | false / true | false |
| `ksl` | integer | 1 – 4 (choice number + 1) | 1 |
| `egType` | boolean | false / true | true |
| `bypass` | boolean | false / true | false |
| `kor` | boolean | false / true | false |
| `xof` | boolean | false / true | false |
| `mask` | boolean | false / true | false |
| `eg` | integer | 1 – 10 (choice number + 1) | 1 |
| `am` | boolean | false / true | false |
| `amd` | decimal | 0.1 – 10 | 1 |
| `ams` | decimal | 1 – 20 | 3.7 |
| `vib` | boolean | false / true | false |
| `pmd` | decimal | 1 – 100 | 7 |
| `pms` | decimal | 1 – 20 | 6.4 |

It also carries the following nested blocks.

| Key | What it holds |
| --- | --- |
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
