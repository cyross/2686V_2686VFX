---
title: What changed in v4.3.0
description: What's added and changed in 4.3.0
sidebar:
  order: 0
---

This page sums up the changes from 4.2.0 to 4.3.0. 4.3.0 adds a way to follow the **output quirks** of the real hardware.

## QUALITY gains "CHIP DAC"

The QUALITY section of the FM chips (OPNA, OPN, OPL, OPL3, OPM) and SSG gains a
**CHIP DAC** toggle, below INTERP. Turn it on to follow the quirks of the DAC the
real chip's sound passes through.

**It is off by default.** While it is off the sound is the same as before, so existing
presets and DAW projects sound unchanged.

### FM chips

Every sounding voice is added up first, and the sum goes through the real chip's DAC.

| Chip | DAC | What it does |
| --- | --- | --- |
| OPN (YM2203) | YM3014B | Floating point (10-bit mantissa + 3-bit exponent). The louder the sound, the more low bits drop |
| OPM (YM2151) | YM3012 | Same as above |
| OPL (YM3526) | YM3014B | Same as above |
| OPNA (YM2608) | — | Snaps to 16-bit steps |
| OPL3 (YMF262) | YAC512 | Snaps to 16-bit steps |

- On every chip, anything beyond 16 bits is clipped. As on the real chip, stacking too
  many notes distorts
- One carrier at velocity 127 and LEVEL 1.0 is matched to one channel of the real chip at
  full scale (14 bits on OPN, OPM and OPL3; 13 bits on OPNA and OPL)

### SSG

The volume plays through the real chip's logarithmic table (YM2149 compatible). The
envelope value is treated as the 16-step volume register, and SSG HW AMP ENV looks the
table up in 32 steps. As the value falls, the level drops faster than before (linear).

With More Real FC / More Real NES on in DUTY, the sound goes through the 2A03 DAC, so this
has no effect.

### Saving

It is saved in channel parameters, the QUALITY section parameter file, presets and the
DAW state (`chipDac`). Reading an older file that lacks it leaves the current value.

For details, see [CHIP DAC in the shared channel controls](/2686V_2686VFX/en/chips/common/#chip-dac--run-the-sound-through-the-real-dac).

Applies to: 2686V, 2686VLight, 26V, 86V, OPNV, OPLV, OPMV, PULSEV
