---
title: Choosing colours
description: The names and palettes behind the COLORS tab
sidebar:
  order: 4
---

The **COLORS** tab lets you change each colour the interface uses. There are
four ways to pick one.

| Method | What it is |
| --- | --- |
| RGB | by red, green and blue values |
| HSV | by hue, saturation and value |
| Colour name | pick from a list of names |
| Palette | pick from what a real machine could display |

Whichever you use, the result is the same single colour. **One button puts the
defaults back.**

## Colour names

The list of names comes from two places.

### Names given by this plugin

The eight colours the interface is built on. They are the place to start if you
want to shift the overall feel.

| Name | Where it's used |
| --- | --- |
| `OffWhite` | the light base |
| `OffBlack` | the dark base |
| `BorderGray` | outlines |
| `ShadowGray` | shadows |
| `SliderBarBlue` | slider bars |
| `DialogOkBackBlue` | the confirm button in dialogs |
| `MikuPink` | the lamp on an open category |
| `MikuBlack` | the lamp on a closed category |

:::note
The last two trace the colours of Hatsune Miku's hair clip and headphones. They
are used for the lamps on the category headings.
:::

### JUCE colour names

Widely used names like `aliceblue` and `tomato` are available too. There are
many of them, so narrowing by name is usually faster than scrolling the list.

Where both sources name the same colour, **the name given by this plugin wins**.

## Palettes

These list only the colours a machine of the era **could actually display**. Use
them when you want the look of that machine.

| Machine | How its colours are built |
| --- | --- |
| **PC-8801** | 3 bits each of R, G and B |
| **PC Engine** | 3 bits each |
| **Mega Drive** | 3 bits each, but the steps are not evenly spaced |
| **PC-9801** | 4 bits each |
| **X68000** | 5 bits each plus 1 bit of brightness |

They are ordered by how few colours they have. **The Mega Drive is the odd one
out**: its steps are not evenly spaced, so despite also being 3-bit it produces
different colours from the other two. The hardware worked that way, and this
reproduces it.

## How the setting is kept

Colours you change are saved to a file and are still there next time. They are
separate from presets, so **loading a preset does not change your colours**.

For where the file lives, see
[File formats and locations](/2686V_2686VFX/en/files/format/).
