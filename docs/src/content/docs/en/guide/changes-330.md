---
title: What changed in v3.3.0
description: What was added and what changed in 3.3.0
sidebar:
  order: 5
---

A summary of what changed between 3.2.0 and 3.3.0. **No file has become unreadable.** Presets and parameter files made up to 3.2.0 still load as they are.

Every knob added in this release is arranged so that **a file without it still sounds the way it used to**. Guessing "in use or not" from the value itself would make the default impossible to ask for, and would hide the switching condition from the screen. Each addition carries its own on/off switch, and while it is off the old path is taken.

## The ADPCM+ channel

A channel for **loading up to 32 audio files and switching between them while you play**. It is what [WT+](/2686V_2686VFX/en/chips/wtplus/) does for wavetable memory, moved over to PCM.

The big difference from ADPCM is that **the sound shaping is split per file**. FORM, OPTIONAL and PAN hold their own values for each of the 32 slots, so switching material switches how it is played as well.

- **SLOT** is the PCM that sounds. It can be switched from automation
- **TGT** is the PCM shown on screen. It does not affect the sound, so you can edit one PCM while a different one is sounding
- All 32 are **drawn in a grid**. The one with a border is TGT; the one with a coloured background is SLOT

It ships in **2686V, 2686VLight and PCMV**. See [the ADPCM+ channel](/2686V_2686VFX/en/chips/adpcmplus/) for the detail.

:::note[Tab order and chip numbering]
ADPCM+ sits to the right of ADPCM on the tab bar, but **its chip number was added at the end.** The numbers are what saved files and automation point at, so they could not be moved. Anything made up to 3.2.0 still opens on the chip it always did.
:::

## Stopping a wave, or playing part of it

**HOLD** and **KEEP** are new.

- **HOLD** stops the wave once it has run the set number of cycles. After that it holds the MIN or MAX value
- **KEEP** plays only the stretch from START to END within one cycle. Outside that stretch you can either hold the edge value or drop to 0

They appear in OPTIONAL on WT, WT2, WT+ and SSG, and in WT PITCH MOD, WT AMP MOD, SSG HW AMP ENV, SSG HW PITCH ENV and the LFO. When a shape that **already finishes in one cycle** is chosen — SSG HW ENV 1, 3, 5 and 7, or LFO 6 and 7 — HOLD is greyed out, because stopping something twice does nothing.

Which side is held is **a switch rather than a reading of the wave**: stopping on whatever value happened to be passing would give a different sound on every note.

## SPEED — playback rate

A multiplier on how far one sample advances. **0.0001 to 100.0**, default 1.0. The change in rate that the note gives is worked out as before, and this multiplies the result.

RHYTHM does not change rate with the note, so SPEED is simply the rate of the material there.

It covers RHYTHM, ADPCM (PCM), ADPCM+, the WS on OPZX7S, WT, WT2, WT+ and SSG. Along with it, **an OPTIONAL section was created for WT, WT2, WT+ and SSG** — those four did not have one.

## CNT — the loop counter

Counts each time the loop point turns back, and once the set count is reached it carries on instead of turning. **0 to 3000**, where **0 means forever** (as before). It starts again on every note.

It covers RHYTHM, ADPCM (PCM), ADPCM+ and the WS on OPZX7S.

The nudge buttons come in two rows: **-10 / -1 / 0 / +1 / +10** on top, **-1000 / -100 / +100 / +1000** below. 0 to 3000 is a wide span, where one at a time is too slow and a hundred at a time too coarse, so the rows are split by digit. 0 means "go round forever", so it sits in the middle of the top row.

## DELAY — a delayed start

How long after the key before the sound begins. **0.0 to 60.0 seconds**, default 0.0. While it waits, **neither the phase nor the envelope advances**. Running and throwing the output away would mean the sound started partway through once the wait was over.

Three levels are counted separately.

| Where | How many |
| --- | --- |
| The whole channel | 13 channels |
| Operators | 6 kinds of FM, 2 to 8 operators |
| Rhythm pads | 8 |

So you can wait one second for the whole channel and then half a second more on OP1 alone. It is a way to bring the modulator in first and let the carrier arrive later.

The order within OPTIONAL follows time itself — **start, then rate, then repeat** — so it reads DELAY, SPEED, CNT.

## KEEP and ENDL on the envelopes

**KEEP** holds each stage's starting value for the length of that stage instead of sloping between them, then jumps at the end of the stage. Stage lengths do not change. It covers PITCH ENV, SSG SW AMP ENV[11] and SSG SW PITCH ENV[11].

**ENDL** is the level held once the release has run out. It covers AMP ENV, PITCH ENV, SSG SW AMP ENV[11] and SSG SW PITCH ENV[11], and only takes effect with **USE ENDL** switched on. While it is off (the default) the old path is taken, so the tails of existing patches are unchanged.

## WT+ holds OPTIONAL per slot

On WT+, **SPEED and HOLD / KEEP now hold separate values for each of the 32 wave memory slots.** Moving TGT switches to that slot's values.

Since each wave can stop differently, moving SLOT alone changes how the note behaves as well as what it sounds like.

## Screen work

- **Labels are now eight characters wide.** The row total is unchanged, so nothing moves. Text that does not fit is made smaller rather than squashed sideways
- **Toggle buttons can be laid out two ways.** The setting is under "simple view" in SETTINGS: centred (as before) or left-aligned, and it takes effect across the whole window as soon as it is switched
- **Dividers were added inside OPTIONAL**, because the position and rate above and the looping below are different matters
- **The browser gained shortcuts.** **ESC** closes it and **Ctrl+M** builds a preview
- **Clicking the same row again clears the selection.** Double-click works as before

## About the older file format

The knobs added in this release are not written into the **line-ordered format** from before 3.0.0. Adding lines there would make existing files in that format unreadable. Reading still works as it always did; writing produces the newer name-keyed format, and the new knobs do go into that.
