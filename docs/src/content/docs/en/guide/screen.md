---
title: Reading the screen
description: Where things are, and how the shared controls behave
sidebar:
  order: 3
---

Every plugin is laid out the same way. This page covers the parts they share.

## The main division

In the instruments, the screen splits into **tabs on the left and the FX pane on
the right**.

| Where | What's there |
| --- | --- |
| Left (tabs) | the sound channels, plus ADV / PRESET / SETTINGS / COLORS / ABOUT |
| Right (pane) | FX. **It stays put as you switch tabs** |

FX is always visible so that you can adjust the effects no matter which chip you
happen to be working on.

:::note
**In 2686VFX, FX is a tab.** There are no sound-chip tabs, so there is nothing
for it to sit beside.
:::

## The tabs

They run left to right. Which ones you get depends on the plugin (see
[Choosing a plugin](/2686V_2686VFX/en/plugins/compare/)).

### Sound chip tabs

OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / SSG / WT / WT2 / WT+ / RHYTHM /
ADPCM / BEEP

**Only the chip on the open tab sounds.** To layer them, use separate DAW
tracks.

Tab colours vary by family, so the FM, SSG, wave-memory and PCM groups are
distinguishable at a glance.

### The other tabs

| Tab | What's there |
| --- | --- |
| **ADV** | curve editing — how a value changes over time (2686V / OPZX7S only) |
| **PRESET** | saving and loading presets, favourites, history |
| **SETTINGS** | behaviour, folder locations, file format |
| **COLORS** | changing the interface colours |
| **ABOUT** | version and licence |

## Shared controls

### Category headings

Related controls sit under a heading. **The lamp at its left shows whether it is
open.** Press the heading to open or close it.

While it is open, a backdrop sits behind its contents, so you can see where the
category ends.

**The heading's colour tells you what kind of feature it is.**

| Colour | Meaning |
| --- | --- |
| Yellow | a feature the real chip had |
| Blue | something that moves level (AMP ENV, SSG SW AMP ENV, WT AMP MOD, …) |
| Turquoise | something that moves pitch (PITCH ENV, SSG SW PITCH ENV, WT PITCH MOD, …) |
| Purple | the LFO |
| Cyan | anything else (OPTIONAL, UNISON・HARMONY, FIX, MUL・DET) |
| Red | sound quality (QUALITY) |
| Lime | other things this plugin adds |

Blue, turquoise, purple and cyan are all either **things the drivers of the
day did in software** or additions of ours. There were too many of them for a
single cyan to stay readable, so 3.1.0 splits them by role.

The order follows the same grouping: **hardware-derived sections come first**,
then level, then pitch, then the LFO.

### Sliders

Drag the control, or type a number in directly.

- **Hover over the button** to see the value's range (can be turned off in
  SETTINGS)
- Double-click to return to the default

### Nudge

The small buttons beside a slider move the value a step at a time. **The "N"
button shows and hides them** — fold them away when you don't need them and the
screen gets roomier.

LEVEL also takes stepped input through **Steps**.

### Waveform previews

Loaded audio files, wave memory and envelope movement are all drawn out, so you
are never assembling something from numbers alone.

## Top right of the screen

| Button | What it does |
| --- | --- |
| Undo / redo | walks the edit history |
| Initialise | resets the open tab's parameters to their defaults |
| Panic | stops any note that got stuck on |
| Preview | shows and hides the waveform preview strip |
| View mode | cycles the three views |

## View modes

| Mode | When to use it |
| --- | --- |
| **Full** | normal editing |
| **Mini player** | checking the sound while keeping the window small |
| **Minimum** | the smallest view, for when tracks are stacked up |

## Wallpaper

SETTINGS lets you put an image behind everything, with a choice of how it is
arranged.

## The keyboard

A MIDI keyboard can be shown along the bottom, so you can try a sound without
sending MIDI from the DAW. Show or hide it in SETTINGS.

For what the computer keyboard does, see
[Keyboard shortcuts](/2686V_2686VFX/en/reference/shortcuts/).
