---
title: What changed in v4.2.0
description: What's added and changed in 4.2.0
sidebar:
  order: 0
---

This page sums up the changes from 4.1.0 to 4.2.0. 4.2.0 adds more **ways to change** a slider's value.
It applies to every plugin, including 2686VFX.

## Drag the label to change the value

Press the name (label) to the left of a slider and **drag up or down** to change the value.
Up raises it, down lowers it. Hovering over the label shows an up-down arrow cursor.

- 200 px covers the whole range
- When **STEPS** has a step size selected, the value moves by that step. With 1/4 steps on 0.0-1.0, that is one step every 50 px
- For AR / DR / SR / RR / SL / TL shown the N88-BASIC(86) way, dragging up still raises the number on screen
- When STEPS is not in use, holding a key while dragging aligns the value to a step

| Key | Step |
| --- | --- |
| **Shift** | 0.1 (real-valued sliders only; does nothing on integer sliders) |
| **Ctrl** | 1 |
| **Ctrl + Shift** | 10 |

The value moves a whole step at a time, so coarser steps take more distance each. Changing keys
mid-drag starts counting again from that point.

The graph **zoom slider** aligns the same way while you drag its bar: 0.1 / 1 / 10 with the same
keys (kept within 1.0-32.0).

## Every slider gets an N button

A small orange **N** button now sits at the right end of each slider. Clicking it opens a row of
buttons that move the value, overlaid just below the slider (or above it when there is no room below).

| Button | What it does |
| --- | --- |
| **MIN** | Go to the lowest value |
| **-10** / **-1** etc. | Subtract that amount |
| **DEF** | Back to the default (**0** for sliders not bound to a parameter) |
| **+1** / **+10** etc. | Add that amount |
| **MAX** | Go to the highest value |

The amounts come from the range and interval: up to three powers of ten, no larger than half
the range, largest first.

| Range | Buttons |
| --- | --- |
| 0-7 | ±1 |
| 0-127 | ±1, ±10 |
| 0.0-1.0 | ±0.001, ±0.01, ±0.1 |

While **STEPS** has a step size selected, the row moves by whole steps (±1/4 for 1/4 steps;
±1/100 and ±10/100 for 1/100 steps). Any other amount would just be rounded back to where it was.

Click outside the row or turn the mouse wheel to close it; clicking **N** again closes it too.
Touching the slider itself or its label does not close it, so you can mix it with dragging the bar.

- **The K and value sliders in Curve editing (ADV) get it too**. K's row also has a **1** button that returns it to its default
- Sliders that already have their own N button and nudge row, such as LEVEL and FIX's FREQ,
  keep using those (no N inside the slider)
- Sliders too narrow to keep their bar once the N button is placed do not show it

Nudge buttons **darken while pressed**. An N button stays dark while its row is open. The same goes
for the existing nudge rows (LEVEL, FIX and so on) and their N buttons.

The COLORS tab gains colours for the N button (`Slider.Nudge.Toggle`), the reset button
(`Slider.Nudge.Reset`), and the row's background and border
(`Slider.Nudge.PopupBg` / `Slider.Nudge.PopupBorder`).

## Fixed the N button on ALG ENV's ALG

On an FM chip's ALG ENV, clicking the N button on ALG showed no nudge buttons. ALG ENV was the only
one without a row of buttons for its N button.

It now shows **-1** / **+1** and buttons that set the algorithm number directly (0-7 for OPN, OPM and
the like). OPZX7, with 130 algorithms, gets -10 / -1 / 0 / +1 / +10 / 129.
