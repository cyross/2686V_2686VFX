---
title: What changed in v4.1.0
description: What's added and changed in 4.1.0
sidebar:
  order: 0
---

This page covers changes from 4.0.1 to 4.1.0. 4.1.0 widens the **loops** of the step envelopes
and the TAP SEQUENCER.

## LOOP MODE for the [256] loops

**LOOP MODE** now sits under LOOP in SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] and in the
sections built the same way: **TL ENV and ALG ENV** (FM chips) and **DUTY RATIO ENV and PEAK RATIO
ENV** (SSG). It applies to every channel, operator and pad, and to 2686VFX. The LOOP toggle is
unchanged.

| LOOP MODE | How it repeats |
| --- | --- |
| **Original Mode** (default) | The existing LOOP.TO / LOOP.CNT: the last step goes back to LOOP.TO, LOOP.CNT times |
| **Rich Mode** | Up to 256 sets of **START, END and COUNT**. After the END step it goes back to the START step, COUNT times (0 is endless) |

Original Mode is the default, so existing presets and parameter files sound the same.

In Rich Mode, pick a set with **LP.TARGET**, edit it with START / END / COUNT, and add or remove
sets with **ADD** / **DELETE**. The sets are sorted by START, then END, and listed below as
No / START / END / COUNT. The section grows taller as sets are added.

- Sets take effect from the one whose START comes first (with the same START, the one whose END comes first)
- Sets can be **nested**. The inner loop finishes first, then the outer one goes back, and the inner
  one starts counting again each time
- A set that **crosses another set's END**: the loop that started first finishes, then playback
  moves on to the START of the next loop
- An endless (COUNT 0) inner loop, or an endless loop that is crossed, means playback never gets past
  it. It just plays that way, but the list shows an orange **! INF** in its COUNT
- The sets are saved with the step table in presets, the DAW state, channel parameters and the
  section's parameter file (`loopMode` / `loops`)

See [LOOP MODE – Rich Mode](/2686V_2686VFX/en/chips/common/#loop-mode--rich-mode) for details.

## LOOP for the TAP SEQUENCER

**LOOP** now sits under STEPS in TAP AMP SEQUENCER / TAP PITCH SEQUENCER (AMP SEQUENCER / PITCH
SEQUENCER on 2686VFX). With the same START, END and COUNT sets as above (up to 256), the step that
advances on each key press can go back.

- After the last step is used it returns to step 1, and the loops start counting again
- INCREMENT follows the sets just like a key press, and RESET also restarts the loop counts.
  DECREMENT still steps back one at a time
- Off by default, so existing sounds do not change. It is also saved in the section's parameter
  file (`loop` / `loops`)

See [TAP SEQUENCER](/2686V_2686VFX/en/chips/common/#tap-sequencer) for details.

## Fixes

### With KEEP on, the last step's value was never heard

In SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] and the sections built the same way (TL ENV, ALG ENV,
DUTY RATIO ENV, PEAK RATIO ENV), finishing the last step with KEEP on left the value of the step
before it. L(STEP) was never heard, and the release started from that value too (TL ENV and ALG ENV
are always KEEP, so they always behaved this way).

Once the last step is reached, L(STEP) is now held until release (sustain). With STEPS = 4 it stays
at L4 after reaching it, and on release goes from L4 to LR over RR seconds.

### The TAP SEQUENCER step did not advance, or did not match the screen

Since 4.0.0, two things went wrong:

- In **mono** mode, while a note was still sounding (legato, or pressing again right after release), the step did not advance
- With **unison**, the step advanced once per voice, and each voice played a different step's value. It also disagreed with the "next step" on screen

It now advances by exactly one step per key press and gives every unison voice the same value.
Releasing a key in mono mode and going back to a key still held does not advance it.
2686VFX's AMP SEQUENCER / PITCH SEQUENCER already worked correctly.
