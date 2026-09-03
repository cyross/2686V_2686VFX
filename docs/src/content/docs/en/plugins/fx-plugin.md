---
title: 2686VFX (the effect)
description: The plugin that processes incoming audio instead of making sound
sidebar:
  order: 2
---

**2686VFX** is the only effect in the series. It is the FX tab from the
instruments, lifted out so you can use it on its own — and on top of that, the
envelopes and LFO that used to live inside the sound chips can now be applied
**to the output itself**.

Put it on an instrument track, a drum loop, or a guitar.

## How the screen is arranged

There are four tabs: **FX / SETTINGS / COLORS / ABOUT**. There are no sound-chip
tabs because nothing generates sound, and no PRESET tab because there is no
patch to store.

The FX tab lays everything out in a single horizontal row. Leftmost is
**Effector** (the global bypass, the reset, and the processing order); the
effect and modulation panels follow to its right. Scroll right to reach them
all.

## The effects (9 of them)

| Effect | What it does |
| --- | --- |
| Filter | LPF / HPF / BPF |
| 3-band EQ | low / mid / high |
| Tremolo | amplitude wobble |
| Vibrato | pitch wobble |
| Bit crusher | the modern kind |
| Delay | echo |
| Reverb | room tail |
| SFC echo | the Super Famicom echo, with its 8-tap FIR |
| **PCM bit crusher** | crushes on the real chips' rate and bit steps |

The last one, the **PCM bit crusher**, is new in 2686VFX. It uses the same steps
as the instruments' QUALITY section: it decimates down to the chosen rate,
rounds to the chosen bit depth, then fills the gaps on read-back with the
interpolation you pick. Seven interpolations are available (Nearest, Linear,
Gaussian, Zero-Order Hold, Cosine, B-Spline, Lagrange).

:::note
The bit list runs from **32-bit down to 4-bit PCM**. The compressed codecs from
the instruments' QUALITY section (ADPCM and friends) are not included: those
encode from the start of a sample in order, so cutting a live stream into blocks
would break the sound at every seam.
:::

## Modulating the output

In the instruments, envelopes and LFOs belong to a channel and move an
operator's level or pitch. An effect has no channels, so 2686VFX carries
**exactly one set**, applied to the output.

**MIDI drives it.** Press a key and the envelopes start; release and they
return. Touch no keys and nothing is applied — the audio passes straight
through.

### Moving the level

| Panel | Taken from |
| --- | --- |
| AMP ENV | the amplitude ADSR |
| SSG HW AMP ENV | the SSG hardware envelope |
| SSG SW AMP ENV[11] | the SSG software envelope (11 steps) |
| WT AMP MOD | wavetable level modulation (FDS / WonderSwan / HuC6280) |
| LFO | the AM side of the OPZX7 LFO |

### Moving the pitch

| Panel | Taken from |
| --- | --- |
| PITCH ENV | the pitch ADSR |
| SSG HW PITCH ENV | the SSG hardware envelope shapes pointed at pitch |
| SSG SW PITCH ENV[11] | the pitch software envelope (11 steps) |
| WT PITCH MOD | wavetable pitch modulation (FDS / WonderSwan / HuC6280) |
| LFO | the PM side of the OPZX7 LFO |

### Shifting the pitch by a fixed amount

| Panel | Taken from |
| --- | --- |
| MUL・DET | the OPZX7 multiple and detune |
| UNISON・HARMONY | unison (up to 8 voices, with the pseudo fast arpeggio) |

### Switching them on and off

The leftmost **Effector** panel carries four switches. Like every other effect
here they are **bypasses** — tick one to take it out of the path.

- Bypass envelopes
- Bypass LFO
- Bypass pitch modulation
- Bypass pitch shifting

Each individual feature has its own switch inside its own panel.

## How the pitch is moved

:::caution[Not a hardware emulation]
In the instruments, pitch modulation only had to change how fast an oscillator's
phase advanced, because the sound originated there. **An effect has no
oscillator.** To change the pitch of incoming audio you have to buffer it and
then read it back at a different speed.

Reading runs faster or slower than writing, so a single read point would either
overtake the write point or fall away from it. Instead there are two read
points: before one reaches the end, the other takes over, and the two are
crossfaded so the sound doesn't break.

**This is a simple design.** Large shifts sound grainy. When you set MUL to 2 or
higher, or open the unison spread wide, listen as you go.
:::

### The WT PITCH MOD / WT AMP MOD base frequency

On real hardware, the WT PITCH MOD and WT AMP MOD rates are a **ratio against
the carrier frequency**. An effect has no carrier, so the panel lets you set a
**base frequency** to stand in for it (1–2000 Hz, 440 Hz by default). The two
modulators share it.

**This has no counterpart on the real chips; it is our own addition.** The lower
you set it, the slower the modulation.

### Panel colours

The panels sit side by side, so the **background colour tells the sections
apart**.

| Section | Colour |
| --- | --- |
| Effects (including the PCM bit crusher) | blue |
| Modulation of the output | red |
| LFO | green |
| MUL・DET / UNISON・HARMONY | cyan |

The colours can be swapped under COLORS in SETTINGS.

## Files

FX order and FX parameter files **move freely between 2686VFX and the
instruments.**

| Kind | Extension |
| --- | --- |
| FX order | `.fxo.json` |
| FX parameters | `.2fx.json` |

The order file stores effects by **name**, so a differing number of effects
never causes a mix-up.

- An order file written by an instrument, read in 2686VFX — the PCM bit crusher
  isn't listed, so it is appended at the end
- An order file written by 2686VFX, read in an instrument — the PCM bit crusher
  doesn't exist there, so it is skipped

:::tip
2686VFX has no PRESET tab. To keep a set of settings, use your DAW's own plugin
preset feature.
:::
