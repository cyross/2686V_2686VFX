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
- **TARGET** is the PCM shown on screen. It does not affect the sound, so you can edit one PCM while a different one is sounding
- All 32 are **drawn in a grid**. The one with a border is TARGET; the one with a coloured background is SLOT

It ships in **2686V, 2686VLight and PCMV**. See [the ADPCM+ channel](/2686V_2686VFX/en/chips/adpcmplus/) for the detail.

:::tip[Acknowledgement]
This channel was added after taking inspiration from [a post](https://x.com/yuzokoshiro/status/2098008081573855457) by Yuzo Koshiro. It was a tremendous help. Thank you for so generously sharing your knowledge.
:::

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

On WT+, **SPEED and HOLD / KEEP now hold separate values for each of the 32 wave memory slots.** Moving TARGET switches to that slot's values.

Since each wave can stop differently, moving SLOT alone changes how the note behaves as well as what it sounds like.

## The FM and RHYTHM tabs: pictures above, one set of settings below

The OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / RHYTHM tabs used to repeat a full set of sections for every operator or pad. In 3.3.0 they are gathered into **pictures at the top and a single set of settings below**.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opna.png" alt="OPNA channel" style="width:600px;" />
	<figcaption>The OPNA channel: pictures on top, TARGET and the settings below</figcaption>
</figure>

- **The frames at the top** show each operator's (or pad's) envelope, four to a row. OPZX7S also shows its WAVE SHAPE wave, and RHYTHM the waveform of the loaded sample. **Clicking anywhere in a frame moves TARGET there**
- **TARGET** says which operator (or pad) the settings below are pointing at, and that frame is outlined. The strip reads "TARGET → the name of what it points at → AMP / PIT / SSG / S11 / P11". The buttons on the right choose **which envelope the pictures show, for all frames at once**
- **The settings** are laid out one section per column, side by side; what does not fit is reached with the scrollbar underneath. Sections start open. A few short sections share a column
- **TARGET is remembered** when the window is closed
- **Ctrl + ← / →** steps TARGET one at a time and **Ctrl + a digit** jumps to that number. On WT+ and ADPCM+, **Ctrl + ← / →** steps the slot as well ([Keyboard shortcuts](/2686V_2686VFX/en/reference/shortcuts/))

Since operators are no longer shown four at a time, OPZX7S's **VIEW MODE** (switching between the upper and lower rows) is gone. None of this touches the sound or the files.

## Screen work

- **Labels are now eight characters wide, and the spellings are back in full.** Squeezed forms such as LV, SHPE and P.OF now read LEVEL, SHAPE and P.OFFSET; likewise FX's LGn → LOW.GAIN and FC0 → COEF0, QUALITY's INTP → INTERP, OPZX7S's POFF → P.OFFSET, B.P → BREAK.PT and F.OP → FB.TARGET, SSG's RAT → RATIO, and BEEP's CLK → CLOCK. The row total is unchanged, so nothing moves
- **The RHYTHM pad's LEVEL has value buttons too.** Pressing N brings up the same set as the channel LEVEL (±1.0 / ±0.1 / ±0.01, 1.0, 0.25 and so on)
- **WT+ and ADPCM+ OPTIONAL, and ADPCM+ PAN, now start open**
- **The first time the window opens, you are asked whether to use simple view.** Answering creates the default settings file, so the question is not asked again. See [the initial settings question](/2686V_2686VFX/en/guide/install/#the-initial-settings-question)
- **2686VFX stacks related effects in one column.** The nine effect columns became four, so the modulation panels to the right are a shorter scroll away. The processing order is unchanged
- **Toggle buttons can be laid out two ways.** The setting is under "simple view" in SETTINGS: centred (as before) or left-aligned, and it takes effect across the whole window as soon as it is switched
- **Dividers were added inside OPTIONAL**, because the position and rate above and the looping below are different matters
- **The browser gained shortcuts.** **ESC** closes it and **Ctrl+M** builds a preview
- **Clicking the same row again clears the selection.** Double-click works as before

## Fixes

Faults present up to 3.2.0.

- **On 2686VLight, 26V, 86V and OPZX7S, OP / CL and "Bypass hidden sections" did nothing.** The buttons were there but no tab responded, because when the feature was added in 3.1.0 these four plugins' tabs never received the implementation
- **The mask in the MML copied with Ctrl + C did not reflect OP3 and OP4.** The second operator's value was written twice. This affected OPNA / OPN / OPL3 / OPM
- **OPZX7S's TARGET stopped at 6**, so operators 7 and 8 could not be read or written one operator at a time
- **On 86V's RHYTHM, TARGET and the pad copy source/destination went up to 8.** 86V has six pads, so 7 and 8 pointed at pads that do not exist
- **"Load from settings file" on SETTINGS did not show everything it loaded.** The simple view, tooltip, headroom and virtual MIDI keyboard toggles kept their old state, and simple view, tooltips and the virtual keyboard did not follow the loaded values until switched again

## About the older file format

The knobs added in this release are not written into the **line-ordered format** from before 3.0.0. Adding lines there would make existing files in that format unreadable. Reading still works as it always did; writing produces the newer name-keyed format, and the new knobs do go into that.
