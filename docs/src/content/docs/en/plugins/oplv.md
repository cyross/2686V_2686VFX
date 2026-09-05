---
title: OPLV
description: What is in OPLV, and when to reach for it
sidebar:
  order: 8
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/OPLV.png" alt="OPLV logo" width="128" height="128" />
	<figcaption>Logo</figcaption>
</figure>

Narrowed down to **the two OPL-family channels** — YM3526 (OPL) and
YMF262 (OPL3). You can move between the plain two-operator sound and
the thicker four-operator one.

## What the plugin looks like

<figure class="shot">
	<img src="/2686V_2686VFX/ui/plugin/OPLV.png" alt="OPLV, the whole window" width="1426" height="740" />
	<figcaption>The whole window</figcaption>
</figure>

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/ui/about/OPLV.png" alt="OPLV ABOUT tab" width="387" height="504" />
	<figcaption>ABOUT tab</figcaption>
</figure>

## Channels it carries

| Channel | |
| --- | :---: |
| [OPL](/2686V_2686VFX/en/chips/opl/) | ✔ |
| [OPL3](/2686V_2686VFX/en/chips/opl3/) | ✔ |

The ones it does not have are OPNA / OPN / OPM / OPZX7 / SSG / WAVETABLE / WT2 / WTPLUS / RHYTHM / ADPCM / BEEP.

What each channel holds is on its own page. The sections that turn up in the
same shape on every channel are collected under
[Shared sections](/2686V_2686VFX/en/chips/common/).

## Curve editing (ADV)

**Available.**

See [Curve editing](/2686V_2686VFX/en/chips/adv/) for the details.

## When to reach for it

- When you want the light, two-operator character
- Reading an OPL parameter file into OPL3 to thicken it up

## Files

| What | Extension |
| --- | --- |
| Preset | `.oplv.json` |
| Parameter files | shared with the other plugins |

Every plugin uses its own preset extension, so you can keep one
`Presets` folder for all of them and each plugin still lists only its own.
Parameter files, on the other hand, **can be read across plugins**.

For where the files live, see
[File formats and locations](/2686V_2686VFX/en/files/format/).

:::note
Carrying fewer channels means **a lighter load when you run several at once**.
When you need everything in one place, reach for
[2686V](/2686V_2686VFX/en/plugins/2686v/).
:::
