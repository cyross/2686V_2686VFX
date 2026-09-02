---
title: Interface colours
description: What is inside .ccolor.json
sidebar:
  order: 53
---

The colours of the interface. Unlike the other files, **it has nothing to do
with automation**.

| | |
| --- | --- |
| **Extension** | `.ccolor.json` / `.ccolor.yaml` |
| **format** | `2686V color settings` |
| **version** | `1` |
| **Export / import** | COLORS' Export and Import |

## Shape

```json
{
  "format": "2686V color settings",
  "version": 1,
  "colors": {
    "Editor.defaultBg": "ff1a1206",
    "Group.TitleText": "ffffb000"
  }
}
```

Unlike the other parameter files, it is not wrapped in `values` — it carries
`colors` directly.

## How a colour is written

**Eight hexadecimal digits, `aarrggbb`.** The first two are the opacity; `ff`
is fully opaque.

The colour names are listed under
[colour names](/2686V_2686VFX/en/reference/color-names/).

## What gets written out

**Only the colours you changed** are written out. Writing all of them would mean
that, once new colours were added later, an old file would overwrite them with
“still at the default” and the new defaults would never show.

On load, **any colour the file does not mention goes back to its default**, so
what you see after loading matches what you saw when you wrote the file. Names
it does not know are skipped, so a file written by a version with fewer colours
still loads.

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
