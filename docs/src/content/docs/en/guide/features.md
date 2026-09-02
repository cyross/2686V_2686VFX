---
title: Highlights and what it's for
description: What these plugins can do, and when they suit
sidebar:
  order: 2
---

## Highlights

### Close to the hardware, with the gaps filled in

FM, SSG, wave memory, ADPCM — each of the era's sound chips is carried as a
**channel**. Where the hardware had a feature it behaves as the hardware did,
and on top of that there are original extensions for things the hardware could
never do.

You can tell which is which from the colour of the heading on screen and from
the wording in this manual. If you are after the real thing, simply leave the
original entries alone.

### Envelopes, in depth

Several envelopes can be stacked on both level and pitch.

- Amp envelope (ADSR)
- SSG hardware envelope (with original waveforms added)
- SSG software envelope (6-tap / 11-tap)
- Pitch envelope (3-tap / 11-tap)
- Pitch modulation from wave memory

These are available on **every channel, operator and rhythm pad**. The fine,
rapid movement that the sound drivers of the day produced in software can be
built straight from the screen.

### Waveforms you can see

Loaded audio files, wave memory and envelope graphs are all drawn on screen. You
are not assembling anything from numbers alone; you can check the shape as you
go.

### Every colour is yours

The COLORS tab lets you change each colour the interface uses. Besides RGB and
HSV you can pick from the **PC-8801 / PC-9801 / X68000 / PC Engine / Mega
Drive** palettes. One button puts the defaults back.

### Files are JSON or YAML

Presets and parameters can both be read and written as `JSON` or `YAML`; you
choose in the settings. The contents are readable, so you can edit them in a
text editor or diff them.

## When they suit

### You want the sound of the machines of the day

**86V** is the short path. It matches the PC-9801-86 layout, so choices that
never existed back then never appear. There is less to be distracted by.

**26V** matches the PC-9801-26(K) layout. With only OPN and SSG, it is the
one to reach for when you want the sound of the earliest PC-98 machines.

For a wider range of period sounds use **2686V**; if you don't need curve
editing and want it to run lighter, **2686VLight**.

### You want to dig into FM synthesis

**OPZX7S**. Up to 8 operators, free routing through the algorithm matrix, and
feedback set independently per operator. Curve editing is standard.

### You want to make audio you already have sound retro

**2686VFX**. It makes no sound of its own; it processes what you feed it. Drum
loops, guitars — drop it on the track and go.

What sets it apart is that the instruments' envelopes and LFO can be applied
**to the output itself**, driven from a MIDI keyboard. Touch no keys and the
audio passes straight through, so you can move only the parts you want.

### You want to make sound effects

Every channel has a **sound effect mode**, and there is the BEEP channel. SSG
noise and beeps *are* the sound effects of the era's games. Push a pitch
envelope hard and you get lasers and explosions.

### You want to bring in existing data

`.wt` and `.wt2` wave memory files can be loaded. There is also **MML input**
per operator, so if you are used to writing it that way you can start there.

## What they are not for

- **Playing several chips at once from one plugin** — use separate tracks
- **Exact hardware emulation** — this recreates the character of the sound, not
  register-level compatibility
- **Dense polyphonic material on a single track** — 10 voices at once, up to 80
  when combined with unison, but the load rises to match
