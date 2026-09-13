---
title: Keyboard shortcuts
description: What you can do without the mouse
sidebar:
  order: 3
---

## Anywhere

| Key | What it does |
| --- | --- |
| **Ctrl** + **Z** | undo |
| **Ctrl** + **Y** | redo |
| **Ctrl** + **Shift** + **Z** | redo (the Mac convention) |
| **Ctrl** + **C** | copy the open channel's parameters as text |
| **Ctrl** + **I** | reset the open tab's parameters to their defaults |
| **Q** | reset the MIDI settings |

:::note
On a Mac, **Command** stands in for **Ctrl**. The plugin sorts that out for you.
:::

## On an FM channel's tab

| Key | What it does |
| --- | --- |
| **1**–**8** | open that operator's MML input |
| **Alt** + **1**–**8** | toggle that operator's mask |
| **Ctrl** + **1**–**8** | move TARGET to that operator |

The numeric keypad works too. The number of operators varies by channel, so
pressing the number of an operator that isn't there does nothing.

For the notation, see [MML input](/2686V_2686VFX/en/reference/mml/).

## On a tab that has a TARGET

OPNA, OPN, OPL, OPL3, OPM, OPZX7, RHYTHM, WT+ and ADPCM+ each have a
**TARGET** that picks what the reading, writing and editing apply to.

| Key | What it does |
| --- | --- |
| **Ctrl** + **←** | move TARGET back one |
| **Ctrl** + **→** | move TARGET on one |
| **Ctrl** + **1**–**8** | move TARGET to that number |

**It wraps at the ends.** With four operators, one past 4 is 1, and one before
1 is 4.

**Ctrl** + **1**–**8** only applies where there are at most eight to choose
from. Wave memory (WT+) and PCM (ADPCM+) have 32 slots, which numbers cannot
reach, so those take **←** and **→** only.

## Editing wave memory (WT)

When you draw a waveform by dragging it directly, **holding a modifier** snaps
the values to a step. Useful for tidying a shape up after drawing it freehand.

| Key | Step |
| --- | --- |
| **Shift** + drag | 0.01 |
| **Ctrl** + drag | 0.05 |
| **Alt** + drag | 0.1 |

Drag with nothing held and you draw freely, with no stepping.

:::note
WT2's values are already stepped by nature, so it has no modifier snapping. You
choose the number of steps on screen.
:::

## In dialogs

| Key | What it does |
| --- | --- |
| **Enter** | confirm |
| **Esc** | cancel |

## Moving between controls

**Tab** moves through the controls in order. That order is set to follow the way
the screen is laid out.
