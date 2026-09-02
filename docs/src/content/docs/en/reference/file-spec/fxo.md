---
title: Effect order
description: What is inside .fxo.json
sidebar:
  order: 51
---

Holds nothing but the order the effects run in.

| | |
| --- | --- |
| **Extension** | `.fxo.json` / `.fxo.yaml` |
| **format** | `fxOrder` |
| **version** | `1` |
| **Export / import** | the FX Order's [EX] / [IM] |

## Shape

```json
{
  "format": "fxOrder",
  "version": 1,
  "values": {
    "order": [
      "filter", "eq3b", "tremolo", "vibrato",
      "modernBitCrusher", "delay", "reverb", "sfcEcho"
    ]
  }
}
```

## Names

| Name | Effect |
| --- | --- |
| `filter` | Filter |
| `eq3b` | 3-band EQ |
| `tremolo` | Tremolo |
| `vibrato` | Vibrato |
| `modernBitCrusher` | Bit crusher |
| `delay` | Delay |
| `reverb` | Reverb |
| `sfcEcho` | SFC echo |
| `pcmBitCrusher` | PCM bit crusher (**2686VFX only**) |

**They are held by name, not by position.** Move a file between plugins that
carry different numbers of effects and nothing shifts into the wrong slot.

Names it does not know are skipped, and effects the file does not mention go to
the end. A file written by 2686VFX loads on the instruments without breaking.

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
