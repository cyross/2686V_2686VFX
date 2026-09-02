---
title: MML input
description: Feeding values into an FM operator all at once, MML style
sidebar:
  order: 2
---

Every FM channel has an **MML** button on each operator. It lets you pour in a
whole set of parameters using the notation the sound drivers of the day used.

Instead of moving sliders one at a time, you can paste values straight from your
notes or from existing song data.

## How to write it

Put **a token and a number** together, and separate the pairs with spaces. The
colon between them is optional.

```text
AR:31 DR:5 SL:2 RR:7 TL:20 MUL:1 DT:-1
```

```text
AR31 DR5 SL2 RR7 TL20 MUL1 DT-1
```

Both mean the same thing. Negative values are written as `-1`.

:::note
Tokens it doesn't recognise are skipped, so notation from another driver can be
mixed in and whatever is understood still gets applied.
:::

## Where it's available

MML input exists **only on the FM channels**. SSG, wave memory and rhythm do not
have it.

## The tokens

Which tokens are accepted varies by channel. Some meanings have several
spellings, because the drivers of the day did not agree on them.

### On every FM channel

| Token | Meaning |
| --- | --- |
| `AR` | attack rate |
| `DR` | decay rate |
| `RR` | release rate |
| `SL` | sustain level |
| `TL` / `O` / `OL` | total level (volume) |
| `MUL` / `ML` | multiple |
| `MASK` | operator mask |

### OPNA / OPN

| Token | Meaning |
| --- | --- |
| `SR` | sustain rate |
| `DT` | detune |
| `KS` | key scale |

### OPL / OPL3

| Token | Meaning |
| --- | --- |
| `KSL` / `KL` | key scale level |
| `KSR` / `KR` | key scale rate |
| `AM` | amplitude modulation |
| `PM` / `VIB` / `VB` | phase modulation (vibrato) |
| `EGTYPE` / `ET` | envelope type |

### OPM / OPZX7

| Token | Meaning |
| --- | --- |
| `D1R` | first decay rate |
| `D1L` | first decay level |
| `D2R` | second decay rate |
| `DT1` / `DT` / `DTO` | detune 1 |
| `DT2` / `DTT` | detune 2 |
| `KS` | key scale (OPM only) |

## Value ranges

Values are written as **the register numbers of the original hardware**. Even
where the slider on screen shows a real number, MML takes the integer register
value.

| Token | Range |
| --- | --- |
| `AR` / `DR` / `SR` / `D1R` / `D2R` | 0–31 |
| `SL` / `RR` / `D1L` | 0–15 |
| `TL` | 0–127 |
| `MUL` | 0–15 |
| `DT` | -3–3 |

Anything out of range is rounded to the nearest valid value.

## Using it

1. Open an FM channel's tab
2. Press the **MML** button on the operator you want to fill in
3. Type into the field and confirm

:::tip[From the keyboard]
Pressing an operator's number key (**1**–**8**, numeric keypad included) opens
that operator's MML input. Holding **Alt** toggles its mask instead.
:::
