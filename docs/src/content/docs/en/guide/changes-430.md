---
title: What changed in v4.3.0
description: What's added and changed in 4.3.0
sidebar:
  order: 0
---

This page sums up the changes from 4.2.0 to 4.3.0. 4.3.0 adds a way to follow the **output quirks** of the real hardware,
and **widens the set of parameters** that TAP SEQUENCER and the 256-step envelopes can drive.

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

## New column order for FM operators

The KEY SCALE / MUL/DET column (the one with FIX and LFO) now sits **right next to the AMP ENV / OPTIONAL column**,
since the two are often edited together. On OPZX7 it comes before the WAVEFORM column.

This applies to the OPNA / OPN / OPL / OPL3 / OPM / OPZX7 tabs in every plugin with FM channels.

## Alt key for slider dragging

While dragging a label, or the bar of a graph's zoom slider, holding **Alt** aligns the value to **steps of 100**.
Sliders whose range is narrower than 100 are left alone. All plugins (including 2686VFX).

| Key | Aligns to |
| --- | --- |
| **Shift** | 0.1 (real-valued sliders only) |
| **Ctrl** | 1 |
| **Ctrl + Shift** | 10 |
| **Alt** | 100 (from 4.3.0) |

## TAP PARAM SEQUENCER

[[TAP SEQUENCER]] has a third section, **TAP PARAM SEQUENCER**. Each key press moves on one step, and the
**(parameter, value) pairs** stored in that step replace parameters **for that note**. As an extension of our own,
one step holds **up to 16 pairs**. On FM channels the pairs can point at operator parameters; on RHYTHM, at pad parameters.

You can choose the following. On/off toggles are not offered.

- The numeric parameters of FM operators (TL / AR / DR / SR / SL / RR / KS / MUL / DT and so on) and FIX frequency
- The FM channel's ALG / FB
- The WT PITCH MOD / WT AMP MOD waveform and speed
- The WT+ waveform and the ADPCM+ voice (slot)
- SSG duty and triangle peak
- FIX frequency and the various frequencies and speeds (LFO, SSG HW ENV period, noise, SPEED, TIMER CLOCK, ARP FREQ and so on)

Steps advance exactly as in TAP AMP / PITCH SEQUENCER (LOOP, SKIP, INCREMENT / DECREMENT / CLEAR STEP, one step per key
press). See [TAP PARAM SEQUENCER](/2686V_2686VFX/en/chips/common/#tap-param-sequencer) for details.

## PARAM ENV[256] (TL ENV is now part of it)

**PARAM ENV[256]** is the same 256-step envelope as SSG SW AMP ENV[256], driving the parameter chosen with **TARGET**.
It offers the same parameters as TAP PARAM SEQUENCER.

- One per channel (the new **[[PARAM ENV]]** major section, above [[TAP SEQUENCER]])
- FM channels also have one per operator (just below the AMP ENV section, where TL ENV used to be)
- Values are kept as a fraction of the target parameter's range and shown the way the target shows them
- Integer and choice parameters hold their value for each step

**TL ENV on FM operators became the PARAM ENV target TL.** Presets, DAW state, channel parameters and section parameter
files (`.tlEnv`) from 4.2.x and earlier are converted to PARAM ENV (TARGET set to TL) when loaded, so they sound the same.
The parameter IDs changed from `_TLENV_*` to `_PRMENV_*`; if you automated TL ENV in your DAW, assign the automation again.

See [PARAM ENV\[256\]](/2686V_2686VFX/en/chips/common/#param-env256) for details.

## More sections hidden in simple view mode

PARAM ENV[256] and TAP PARAM SEQUENCER are hidden in simple view mode. "PARAM ENV[256]" and "TAP PARAM SEQUENCER" were
added to the **simple view customization** in SETTINGS, so you can keep them shown. **[Bypass hidden sections]** turns
them off too.

## ADPCM+ keeps every slot encoded

Changing the ADPCM+ VOICE with TAP PARAM SEQUENCER / PARAM ENV can play slots other than the selected one. So every
loaded slot is now encoded again when QUALITY changes.
