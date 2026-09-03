---
title: What changed in v3.1.0
description: What 3.1.0 adds and what moved
sidebar:
  order: 4
---

The changes between 3.0.0 and 3.1.0. **Nothing stops loading.** Presets and
parameter files made with 3.0.0 still read as they did.

## New modulation

### SSG HW PITCH ENV

It drives pitch with the **same shapes** as
[SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env). The hardware
envelope on the real chip only touched level, but pointing the same shape at
pitch gives repeating arpeggios and trills.

- MIN and MAX are in **cents**, from **-4800 to 4800** (four octaves either way)
- Defaults are MIN = 0 and MAX = 1200 (one octave)
- The same **±100 / ±1200** buttons as the other pitch controls sit beside them
- The same 44 shapes as SSG HW AMP ENV
- Available on every channel (FM included), every operator and every RHYTHM pad
- Reads and writes its own file (`.ssgHwPEnv`)

See [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) for the
details.

### WT AMP MOD

It uses the **same modulation shapes** as
[WT PITCH MOD](/2686V_2686VFX/en/chips/common/#wt-pitch-mod), but drives level
instead of pitch. The output moves between MIN and MAX.

- MIN and MAX default to 0.0 and 1.0, and are held **once for the whole
  modulator**, not per wave slot
- DPTH is how far the level is pulled down from MAX; at 0.0 nothing changes
- Nudge buttons sit beside the values
- The HuC6280 wave memory and the FDS user table are supported too
- Available on every channel (FM included), every operator and every RHYTHM pad
- Reads and writes its own file (`.wtampmod`)

See [WT AMP MOD](/2686V_2686VFX/en/chips/common/#wt-amp-mod) for the details.

### FDS TABLE is now two tables

The WT PITCH MOD table is now **FDS PITCH TABLE** and the WT AMP MOD table is
**FDS AMP TABLE**. The values mean the same thing as before, and **each is kept
separately.**

## Now available per operator

Two things that used to sit only on the whole chip are now on every FM operator
as well.

- SSG HW AMP ENV
- WT PITCH MOD

They are **separate parameters** from the chip-wide ones, so on OPNA and OPZX7S
they coexist with the existing SSG HW ENV and both can apply at once. The
WT PITCH MOD wave slots are per operator too.

They are stored in the operator parameter files, so [IM] / [EX] reads and writes
them one operator at a time.

## 2686VFX

**WT AMP MOD** (level) and **SSG HW PITCH ENV** (pitch) were added to the
modulation applied to the output. They work exactly as they do on the
instruments.

The WT AMP MOD rate is a ratio against the carrier too, so it shares the **same
base frequency** as WT PITCH MOD.

## On screen

### Section colours are split more finely

The software-side sections were all one shade of cyan, so they are now split by
role. Hardware-derived (yellow), quality (red) and other (lime) are unchanged.

| Colour | Sections |
| --- | --- |
| Blue | AMP ENV / SSG SW AMP ENV / SSG SW AMP ENV[11] / WT AMP MOD / FDS AMP TABLE |
| Turquoise | PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD / FDS PITCH TABLE |
| Purple | LFO |
| Cyan | OPTIONAL / UNISON・HARMONY / FIX / MUL・DET (unchanged) |

SSG HW AMP ENV and SSG HW PITCH ENV, and WT PITCH MOD and WT AMP MOD, stay
**yellow** on their own channels (SSG / WT / WT2 / WT+). They only take the
colours above when borrowed onto another channel.

### 2686VFX panels are colour-coded too

| Section | Colour |
| --- | --- |
| Effects (including the PCM bit crusher) | blue (unchanged) |
| Modulation of the output | red |
| LFO | green |
| MUL・DET / UNISON・HARMONY | cyan |

### Sections are in a tidier order

There were enough sections that the order had become ad hoc, so they are grouped
by role. **Where hardware-derived and software-derived sections both exist, the
hardware-derived one comes first.**

| Where | Order |
| --- | --- |
| FM, whole chip | ALGORITHM/FEEDBACK → AMP ENV → level → pitch → LFO → PAN → UNISON/HARMONY → QUALITY → MIDI → UTILITY |
| FM operator | AMP ENV → OPTIONAL → WAVE SHAPE → level → pitch → KEY SCALE → MUL/DET → LFO → FIX → MASK |
| SSG / ADPCM / BEEP / RHYTHM pad | FORM → OPTIONAL → level → pitch → LFO → MUL/DET → FIX → UNISON/HARMONY → QUALITY → MIDI → UTILITY |
| WT / WT2 / WT+ | The same, except modulation is a hardware feature there, so WT AMP MOD and WT PITCH MOD lead the level and pitch groups |

Within the level group the order is SSG HW AMP ENV → SSG SW AMP ENV →
SSG SW AMP ENV[11] → WT AMP MOD; within the pitch group it is PITCH ENV →
SSG HW PITCH ENV → SSG SW PITCH ENV[11] → WT PITCH MOD.

## Fixed

- On RHYTHM pads, a change of sample rate was not passed on to SSG HW AMP ENV
- WT PITCH MOD was missing from the RHYTHM pad parameter files
