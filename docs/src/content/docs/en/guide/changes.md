---
title: What changed since v2.5.1
description: What is different in 3.0.0, and what to watch for when you move over
sidebar:
  order: 4
---

A summary of everything between 2.5.1 and 3.0.0. **The part worth reading first
is [What to watch for when you move over](#what-to-watch-for-when-you-move-over).**

## The biggest changes

### One plugin became five

2.5.1 shipped only **2686V**. In 3.0.0 the work is split by role.

| Plugin | What it is |
| --- | --- |
| 2686V | the full one |
| 2686VLight | curve editing removed, runs lighter |
| 86V | matched to the PC-9801-86 layout |
| OPZX7S | focused on the OPZX7 chip; curve editing is standard |
| 2686VFX | an effect that processes audio |

See [Choosing a plugin](/2686V_2686VFX/en/plugins/compare/) to pick one.

### Files are now JSON and YAML

Presets and parameters are both read and written as `JSON` or `YAML`; you choose
in the settings. The contents are readable, so you can edit them in a text
editor or diff them.

## Making sound

### The OPZX7S chip, strengthened

- Extended to **up to 8 operators**
- **Algorithm matrix** — set the modulation and feedback routing freely with
  checkboxes, including connections the original specs could never express
- **Feedback is now set independently per operator** (a real number, -8.0 to 8.0)
- Algorithm diagrams are drawn in real time instead of from images, and the
  traditional algorithms are drawn too
- Original waveforms added to the LFO

### More envelopes

- **11-tap envelopes added** — SSG SW ENV11 (level) and SSG SW PENV11 (pitch)
- **The SSG hardware envelope is on every channel now**, with 36 original
  waveforms added and a settable MIN / MAX range
- **Envelopes that cover the whole chip added to the FM channels** — AMP ENV,
  SSG HW AMP ENV, SSG SW ENV11, PITCH ENV and SSG SW PENV11
- A graph was added for watching those whole-chip envelopes move

### Wave memory pitch modulation

- **FDS / WonderSwan / HuC6280 modulation added**
- The FDS modulation table can be edited on the spot across its 32 entries
- HuC6280 mode holds up to 8 modulation waveforms, switchable in real time
- **Available on FM, SSG, RHYTHM, ADPCM and BEEP alike**

### A new channel

- **WT+** — loads up to 32 waveforms from `.wt` / `.wt2` files and switches
  between them in real time, from a slider or from automation

### Also

- ADPCM qualities added to the bit list on the PCM-family channels
- BEEP's audio quality improved, with settings added
- **UNISON / HARMONY extended** — the pseudo fast arpeggio, and detune and
  spread per voice
- Rhythm pad names revised to follow the YM2608B
- Parameters can be imported between OPN and OPNA, and between OPL and OPL3

## The screen

### A new COLORS tab

Change each colour the interface uses. Besides RGB and HSV you can pick from
**colour names (JUCE plus original ones) and the palettes of real machines**
(PC-8801 / PC-9801 / X68000 / PC Engine / Mega Drive). One button puts the
defaults back.

The settings are saved to a file, and changes reach the screen immediately.

### A visual overhaul

- Category headings are now drawn rather than composed, and **the open/closed
  state is shown by a lamp instead of text**
- A backdrop sits behind an opened category so you can see where it ends
- Shadows fall from surfaced components and sliders
- Tab widths are measured with the font actually used, so names are no longer
  truncated
- The window and the group widths were made larger
- **Nudge buttons can be shown or hidden with the "N" button**
- Tab and category colours vary per channel
- Many more waveform previews

### The PRESET tab, extended

- **Favourites** and **load history** added (neither is capped)
- Switch the listing between all / favourites / history
- Search can be filtered down to favourites or history

## The 2686VFX effect plugin

The effect plugin that was dropped during v2.x.x is back. It carries 2686V's FX
pane as it stands, and on top of that the instruments' envelopes and LFO can be
applied **to the output itself**.

- **PCM bit crusher** — unique to 2686VFX; crushes on the real chips' rate and
  bit steps
- Nine kinds of output modulation added, driven from a MIDI keyboard
- FX order files move freely between it and the instruments

See [2686VFX (the effect)](/2686V_2686VFX/en/plugins/fx-plugin/) for details.

## Elsewhere

- **This manual site is new**, and it carries the automation reference
- Stepped input (Steps) added for level
- Parameters can be imported and exported per channel, operator and pad
- Build settings for Windows ARM64, Linux and macOS are provided (macOS also
  produces AU)

## Notable fixes

- AMP ENV's STL had no effect at all
- Curves now apply regardless of index, and OPZX7S's envelopes were brought into
  line
- A curve's attack ran one sample behind the linear path
- The OPZX7 LFO's Sample & Hold was counting samples instead of cycles
- Working the STEP control on the SSG SW envelopes froze the screen
- Parameter names shown in the DAW had gaps and duplicates
- Sliders with many digits truncated their numbers
- Rebuilding the preset list and copying parameters both used to stall

## What to watch for when you move over

:::caution
**Older files can still be read, but anything you save comes out in the new
format.**
:::

| Kind | How 3.0.0 treats it |
| --- | --- |
| Presets (`.xml`) | **read-only**; open and re-save and it becomes the new format |
| Parameter files (plain text) | **converted automatically** on load |
| Wave memory (`.wt` / `.wt2`) | **unchanged**; the format stays as it was |

- Preset extensions **differ per plugin**, so another plugin's presets don't
  appear in the list
- **FX order and parameter files are shared**, though, and move freely between
  all five plugins

See [File formats and locations](/2686V_2686VFX/en/files/format/) and
[Converting from the old formats](/2686V_2686VFX/en/files/convert/) for details.
