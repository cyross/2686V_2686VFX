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

## New column order for RHYTHM pads

The pad settings are now laid out like those of FM operators.

- **MUL/DET and FIX** moved below OPTIONAL, in the same column. The column is now called "OPTIONAL / MUL/DET / FIX"
- To its right come the pad's **PARAM ENV** column (see PARAM ENV[256] below) and its **TAP PARAM SEQUENCER** column
- The pad's **EDIT** left the OPTIONAL column and now has its own "EDIT" column at the far right

The order is FORM / PAN / QUALITY → OPTIONAL / MUL/DET / FIX → PARAM ENV → TAP PARAM SEQUENCER → AMP ENV → … → LFO → EDIT.
This applies to every plugin with RHYTHM (2686V, 2686VLight, 86V, PCMV).

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
one step holds **up to 16 pairs**.

There is one per channel (inside [[TAP SEQUENCER]]) and one per FM operator and per RHYTHM pad (a column of the
operator / pad settings). Each lists only its owner's parameters, and each advances on its own with every key press.

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
- FM channels also have one per operator (in its own "PARAM ENV" column of the settings, right of KEY SCALE / MUL/DET / FIX)
- RHYTHM also has one per pad (the "PARAM ENV" column of the pad settings, right of OPTIONAL / MUL/DET / FIX), listing that pad's parameters
- Values are kept as a fraction of the target parameter's range and shown the way the target shows them
- The values (STL, L, LR, ENDL) are kept separately for each TARGET. Changing TARGET switches to that target's values
- Integer and choice parameters hold their value for each step
- Parameters set at key-on, such as MUL, DT and FIX FREQ, also change with each step while the note plays
- With STEP at 1, R.TARGET is disabled (likewise in the other step envelopes, S.TARGET in TAP SEQUENCER and so on)

**TL ENV on FM operators became the PARAM ENV target TL.** Presets, DAW state, channel parameters and section parameter
files (`.tlEnv`) from 4.2.x and earlier are converted to PARAM ENV (TARGET set to TL) when loaded, so they sound the same.
The parameter IDs changed from `_TLENV_*` to `_PRMENV_*`; if you automated TL ENV in your DAW, assign the automation again.

See [PARAM ENV\[256\]](/2686V_2686VFX/en/chips/common/#param-env256) for details.

## Graphs can show where the sound is playing

As you play, the current-step mark of TAP SEQUENCER and the playing position of the envelopes (a yellow vertical line on
the graph) move along. The line appears for PARAM ENV[256] (PRM) and for AMP ENV, PITCH ENV, SSG SW AMP / PITCH ENV[256] and
the FM operator EG (AMP / PIT / S256 / P256). SETTINGS switches "TAP SEQUENCER", "PARAM ENV" and "the other envelopes"
separately and lets you choose the rate from 15 / 30 / 60 / 120 FPS (all start on, at 30 FPS).

The channel graph switch, the FM operator graph switch and the RHYTHM pad graph switch gain **PRM** (PARAM ENV[256]). See
[Where the sound is playing on the graphs](/2686V_2686VFX/en/guide/screen/#where-the-sound-is-playing-on-the-graphs).

## Step envelopes hold a repeated value for R seconds

A step used to end when it reached its target value, so a step with the same value as the one before ended without waiting
R seconds and the later steps came early (KEEP behaved the same way). Steps now end once R seconds have passed. This applies to
SSG SW AMP / PITCH ENV[256], PARAM ENV[256], ALG ENV, DUTY RATIO ENVELOPE and PEAK RATIO ENVELOPE.

Patches with repeated values in a row step differently. Steps with different values keep their length.

## The sustain part of the graphs stands out

On the AMP ENV / PITCH ENV / SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] / PARAM ENV[256] graphs, the part held while the key
is down (the sustain) is drawn with a fixed width in its own colour (yellow-green).

- The step envelopes (SSG SW AMP / PITCH ENV[256] and PARAM ENV[256]) now show the part where the last step is held. Before,
  the release was drawn right after the last step
- With KEEP (always on for integer and choice PARAM ENV targets), the graph now draws the move to LR (ENDL with Use Endl) at the
  end of the release

## New section heading colours

- TAP AMP SEQUENCER is now cyan and TAP PITCH SEQUENCER whitish purple (the other way round from before)
- PARAM ENV[256] and TAP PARAM SEQUENCER are whitish dark blue

## A "Tutorials" chapter in the manual

Pages that teach the controls while you build sounds will be added over time. They can embed videos and animated pictures.

## More chip-accurate modes for the SSG

- The pulse settings gain **More Real SSG**, which mimics the SSG (YM2203 / YM2608B): a fixed 50% duty cycle, a unipolar
  output, the DC cut, the logarithmic volume and the AND of tone and noise. It is exclusive with FC Mode / More Real FC / More Real NES
- The triangle settings gain **More Real FC**, the FC's 4-bit × 32-step staircase triangle
- The lower half of the pulse / triangle group is now the **NOISE** group, with **More Real FC** (the FC's 15-bit LFSR noise)

All of them start off, so existing sounds don't change. See [SSG](/2686V_2686VFX/en/chips/ssg/).

## STEPS for DUTY / PEAK RATIO ENVELOPE

**STEPS** under the value knob snaps the values to 1/10, 1/4, 1/5, 1/8 or 1/16 steps: 10% / 25% / 20% / 12.5% / 6.25%
for DUTY RATIO ENVELOPE, and 0.1 / 0.25 / 0.2 / 0.125 / 0.0625 for PEAK RATIO ENVELOPE.

## More sections hidden in simple view mode

PARAM ENV[256] and TAP PARAM SEQUENCER are hidden in simple view mode. "PARAM ENV[256]" and "TAP PARAM SEQUENCER" were
added to the **simple view customization** in SETTINGS, so you can keep them shown. **[Bypass hidden sections]** turns
them off too.

## ADPCM+ keeps every slot encoded

Changing the ADPCM+ VOICE with TAP PARAM SEQUENCER / PARAM ENV can play slots other than the selected one. So every
loaded slot is now encoded again when QUALITY changes.

## Machine and part names are written as short forms

The screens and the manual now use short forms such as FC, PCE, MD, 98, 88, X68K, 26 and 86 for machine and part names.
This changes the real-machine palettes of the COLORS tab, the machine choice of QUALITY (MSX is split into 2413 and TR),
"7: PCE Wave" of WT PITCH / AMP MOD, and "15: SFC BRR", "19: Z280", "20: K53" and "21: K54" of the QUALITY(PCM) BIT, among others.

Only the names change. The sound, the parameter IDs and the saved format stay the same, so existing presets and DAW states load as before.
