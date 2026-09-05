---
title: Effect settings
description: What is inside .2fx.json
sidebar:
  order: 50
---

Holds the FX settings together in one file.

| | |
| --- | --- |
| **Extension** | `.2fx.json` / `.2fx.yaml` |
| **format** | `fxParam` |
| **version** | `1` |
| **Export / import** | FX's [EX] / [IM] |

## Shape

```json
{
  "format": "fxParam",
  "version": 1,
  "values": {
    "bypass": false,
    "tremolo": { "bypass": true, "rate": 5.0, "depth": 1.0, "mix": 0.0 },
    "…": {}
  }
}
```

The `bypass` at the root switches the whole FX chain. Below it, each effect has
a nested block of its own.

## The keys for each effect

| `tremolo` | `bypass` / `rate` / `depth` / `mix` |
| `vibrato` | `bypass` / `rate` / `depth` / `mix` |
| `bitCrusher` | `bypass` / `rate` / `bits` / `mix` |
| `delay` | `bypass` / `time` / `fb` / `mix` |
| `reverb` | `bypass` / `size` / `damp` / `mix` |
| `filter` | `bypass` / `type` / `freq` / `q` / `mix` |
| `eq3band` | `bypass` / `lowGainDb` / `midFreq` / `midGainDb` / `highGainDb` / `mix` |
| `sfcEcho` | `bypass` / `time` / `fb` / `firCoef0` / `firCoef1` / `firCoef2` / `firCoef3` / `firCoef4` / `firCoef5` / `firCoef6` / `firCoef7` / `mix` |
| `pcmBitCrusher` | `bypass` / `bits` / `rate` / `interp` / `mix` |

Every effect has a `bypass`, and **true means it is taken out**. `mix` is 0.0
for the dry signal alone and 1.0 for the effect alone.

:::note
`pcmBitCrusher` is an effect only 2686VFX has. An instrument never looks for
that block, so it simply passes it by. A file written before 3.1.0 has no such
block either; the current values are then kept, so nothing stops loading.

`bits`, `rate` and `interp` hold the **position in the list** rather than the
value itself. The order of the effects is held on the
[effect order](/2686V_2686VFX/en/reference/file-spec/fxo/) side.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
