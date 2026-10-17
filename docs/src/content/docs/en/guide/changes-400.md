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
