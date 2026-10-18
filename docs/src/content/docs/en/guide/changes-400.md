---
title: What changed in v4.0.0
description: What's added and changed in 4.0.0
sidebar:
  order: 1
---

This page covers changes from 3.6.3 to 4.0.0. 4.0.0 is a major release that aims
to bring retro-game composing techniques into the plugins while leaning on
automation as little as possible.

## ALGORITHM/FEEDBACK can be folded

Press the **ALGORITHM/FEEDBACK** heading in the global part of OPNA / OPN / OPL /
OPL3 / OPM / OPZX7 (including OPZX7S) to open or close it, like the other
sections. It starts open. Folding it does not change the sound.

## LEVEL / STEPS / DELAY grouped under UNCATEGORIZED

The channel-wide **LEVEL, STEPS and DELAY**, which had no heading, now sit under
an **UNCATEGORIZED** heading on every channel. It can be folded and starts open.
Right-click the heading for the same copy menu as other sections, limited to
these three. 2686VFX's output LEVEL is unchanged.

## More Real FC / More Real NES for SSG

The SSG pulse (duty) settings gain **More Real FC** and **More Real NES** below
FC FLUC. They mimic the Famicom / NES (2A03) pulse channel, and are unrelated to
FC FLUC's wavering.

- The duty is the closest of the chip's 12.5% / 25% / 50% / 75%
- Pitch snaps to the 11-bit timer; notes needing t below 8 are silent
- Volume has 16 steps through the DAC curve
- Output filters: FC has a 37 Hz high-pass; NES has 90 Hz and 440 Hz high-passes and a 14 kHz low-pass

Only one of FC Mode, More Real FC and More Real NES can be on (all off is fine).
They are off by default, so existing patches sound the same. See
[SSG](/2686V_2686VFX/en/chips/ssg/) for details.

## Duplicate, Import and Export on the section right-click menu

The section heading's right-click menu gains **Duplicate** below CLEAR, and
**Import** and **Export** below a separator.

- **Duplicate** copies an operator, pad or TARGET section into the same section
  of other operators, pads or TARGETs on the channel, picked in a check-box dialog
- **Import / Export** do what that section's [IM]… / [EX]… in UTILITY do. The
  UTILITY buttons stay

See [Right-click a section](/2686V_2686VFX/en/chips/common/#right-click-a-section).

## SSG SW AMP / PITCH ENV[256]

SSG SW AMP ENV[11] / SSG SW PITCH ENV[11] (10 steps plus release) are replaced by
SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] with up to **256 steps**, on every
channel, operator and pad, plus 2686VFX.

- STEP is 1–256. The release uses **RR** (seconds) and **LR** (target)
- With 256 × 2 values, the per-step R / L are **saved as a table** in presets and
  channel parameters rather than as parameters, so they can't be automated
- The value strip lists only STEP steps
- Section files are `.ssgSwEnv256` / `.ssgSwPEnv256`

**[11] data is converted to [256] on load.** R1–R10 / L1–L10 become steps 1–10 and
R11 / L11 become RR / LR, so it sounds the same. See
[SSG SW AMP ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env256).

## The 6-step SSG SW AMP ENV is gone

The 6-step SSG SW AMP ENV of 3.6.3 and earlier (5 steps + release) is removed and folded into
[SSG SW AMP ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env256).
The section, the graph's "SSG" button, the [IM] / [EX] in UTILITY, the simple-view item and the
SsgSwEnv target of ADV curves are gone.

**Anything made with the 6-step is carried into [256] on load.** That covers presets, DAW state,
channel parameters, `.ssgSwEnv` files and curves.

- R1–R5 / L1–L5 become steps 1–5, and R6 / L6 (the release) become RR / LR
- KEEP and Use Endl are turned off (the 6-step had neither)
- A sound that used **both** the 6-step and [11] keeps [256] (the former [11]) and drops the
  6-step, because one [256] cannot express the two multiplied together

Along with this, RHYTHM pads now send [256] into its release on key-off too (the 6-step was
released, [11] was not).

## Wider groups

Knobs and value fields were cramped, so groups are now 1.2 times wider.

- Every channel's global group: 260 → 312
- Operator and pad columns: 280 → 336
- SSG's square-wave (duty) and triangle-wave groups: 250 → 300

## DUTY / PEAK RATIO ENVELOPE for SSG

The pulse wave's duty cycle and the triangle's peak can now move over time. Both are 256-step
envelopes like SSG SW AMP ENV[256], at the bottom of their groups.

- **DUTY RATIO ENVELOPE** (pulse): values are duty cycles, 0.0–100.0%. While on, it replaces MODE
  and Invert Phrase
- **PEAK RATIO ENVELOPE** (triangle): values are peak positions, 0.0–1.0. While on, it replaces PEAK

Switch them on with **Enable** at the top. They start off, so existing patches sound the same.
See [SSG](/2686V_2686VFX/en/chips/ssg/) for details.

## TL ENV for FM operators

The driver trick of rewriting TL on every key-on to move the level can now be drawn as a
**TL ENV**. It is a TL version of SSG SW AMP ENV[256], with TL register values, directly below each operator's AMP ENV section.

Switch it on with **Enable** at the top. It starts off, so existing patches sound the same.
See [TL ENV](/2686V_2686VFX/en/chips/common/#tl-env) for details.

## ALG ENV for FM channels

**ALG ENV** switches the ALG (algorithm) step by step. It is an ALG version of SSG SW AMP ENV[256],
below FEEDBACK in ALGORITHM/FEEDBACK. You can, for example, use a different routing just for the
start of a note.

Switch it on with **Enable** at the top. It starts off, so existing patches sound the same.
See [ALG ENV](/2686V_2686VFX/en/chips/common/#alg-env) for details.

## [[TAP SEQUENCER]]

**TAP AMP SEQUENCER / TAP PITCH SEQUENCER** move on one step with every key press and correct the
level (as a ratio) or the pitch (in cents). They sit in each channel as the [[TAP SEQUENCER]] major
section below [[PITCH ENV]]. 2686VFX has AMP / PITCH SEQUENCER too, with the key chosen in key assign.

Along with this, step value lists now show four per row, as "label: value" separated by " / ".

Switch them on with **Enable**. They start off, so existing patches sound the same.
See [TAP SEQUENCER](/2686V_2686VFX/en/chips/common/#tap-sequencer) for details.

## Larger operator / pad graphs

The graphs at the top of operator / pad tabs are now **a large TARGET view on the left and small
views of all of them on the right**. The large view follows TARGET at once and, on OPZX7S / RHYTHM,
shows the wave large too. The small views are two per row in two rows on OPNA / OPN / OPL3 / OPM,
two in one row on OPL, and four per row on OPZX7S / RHYTHM, with the same graph aspect ratio as the
large view (the wave height is unchanged). Clicking one moves TARGET there.

## Zooming graphs sideways

Channel graphs and the large TARGET view of operators / pads can now be stretched 1.0–32.0 times
sideways. Change the zoom with the knob or buttons (x1–x32) below, or the mouse wheel over the graph,
and move along by dragging with the right button or with the scroll bar. Long envelopes such as
SSG SW ENV[256] with many steps can now be read in detail.

## Edit buttons for curve editing

ADV curve editing now has **CUT, COPY, PASTE, DUPLICATE and CLEAR** per parameter. PASTE works
across Positions and Targets, and DUPLICATE copies to several other parameters of the same Target at
once. See [ADV](/2686V_2686VFX/en/chips/adv/) for details.

## A board behind the TARGET row

The top row of operator / pad tabs (TARGET, name and graph selector) now has a dark board behind it,
so it stays readable over bright wallpapers. Change its colours with `TargetRow.Bg` / `TargetRow.Border` in COLORS.

## Missing parameters go back to their defaults when a preset loads

Loading a preset or DAW state that lacks some parameters (such as one written by an older version)
left the missing ones at the values of the previously loaded preset. Saving after that wrote them with
empty values, and loading that file set them to 0 (STEPS dropping from 10 to 1, for example).
Loading now sets parameters that are missing or have empty values to their defaults, so files already
saved with empty values load those parameters at their defaults too.
