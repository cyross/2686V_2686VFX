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

## Key assign (2686VFX)

The `keyAssign` block holds which keys drive the modulation. Added in 3.6.0.

```json
"keyAssign": { "mode": 1, "ampEnv": 60, "pitchEnv": 62, "lfoAm": 64, "…": 60 }
```

| Key | Type | Range | Default | Meaning |
| --- | --- | --- | ---: | --- |
| `mode` | integer | 0 to 1 | 0 | 0 is Single key, 1 is Customize |
| `ampEnv` | integer | 0 to 127 | 60 | The key that drives AMP ENV |
| `ssgHwEnv` | integer | 0 to 127 | 60 | The key that drives SSG HW AMP ENV |
| `wtAmpMod` | integer | 0 to 127 | 60 | The key that drives WT AMP MOD |
| `ssgSwEnv11` | integer | 0 to 127 | 60 | The key that drives SSG SW AMP ENV\[11\] |
| `pitchEnv` | integer | 0 to 127 | 60 | The key that drives PITCH ENV |
| `ssgHwPEnv` | integer | 0 to 127 | 60 | The key that drives SSG HW PITCH ENV |
| `ssgSwPEnv11` | integer | 0 to 127 | 60 | The key that drives SSG SW PITCH ENV\[11\] |
| `wtMod` | integer | 0 to 127 | 60 | The key that drives WT PITCH MOD |
| `lfoAm` | integer | 0 to 127 | 60 | The key that drives LFO AM |
| `lfoPm` | integer | 0 to 127 | 60 | The key that drives LFO PM |
| `mulDet` | integer | 0 to 127 | 60 | The key that drives MUL/DET |
| `unison` | integer | 0 to 127 | 60 | The key that drives UNISON/HARMONY |
| `arpeggio` | integer | 0 to 127 | 60 | The key that drives ARPEGGIO |

Keys are MIDI note numbers; 60 is C3.

:::note
Only 2686VFX has `keyAssign`; an instrument passes it by. Files written before
3.6.0 have no such block, and then the current assignment is kept as it is.
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
