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
