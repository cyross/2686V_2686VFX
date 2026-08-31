---
title: Automation reference
description: Every parameter you can automate from your DAW
sidebar:
  order: 0
---

Every parameter you can drive from your DAW's automation — **7296** in total,
split into one page per channel.

The list is written out from what the plugin actually registers, so it cannot
drift from the plugin the way a hand-written list would.

## Reading the tables

| Column | Meaning |
| --- | --- |
| **Automation name** | The name shown in your DAW's automation list |
| **ID** | The name used inside files; useful when editing a preset by hand |
| **Type** | Int, Float, Bool or Choice |
| **Range** | The values it accepts |
| **Default** | The value it starts at |

## How booleans work

Automation values arrive as 0.0–1.0. Boolean parameters read them like this:

- **0.00 – 0.49** … False
- **0.50 – 1.00** … True

Defaults follow the same rule: send **0.0 for False, 1.0 for True**.

## A note on curves (ADV)

:::caution
**Curve settings cannot be automated.**

The curve grid is 9 (Position) × 6 (Target) × 16 (Param) = 864 slots, each
holding 18 parameters. Registering them all would add more than 15,000
parameters and make a DAW's automation list unusable, so they are not exposed
as ordinary parameters.

Curve settings are saved with the preset, so switching presets swaps them all
at once.
:::
