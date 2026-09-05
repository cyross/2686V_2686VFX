---
title: OPMV
description: What is in OPMV, and when to reach for it
sidebar:
  order: 9
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/OPMV.png" alt="OPMV logo" width="128" height="128" />
	<figcaption>Logo</figcaption>
</figure>

Narrowed down to **OPM (YM2151) alone**. A good fit when you want one
part with the X68000 or arcade character.

## Channels it carries

| Channel | |
| --- | :---: |
| [OPM](/2686V_2686VFX/en/chips/opm/) | ✔ |

The ones it does not have are OPNA / OPN / OPL / OPL3 / OPZX7 / SSG / WAVETABLE / WT2 / WTPLUS / RHYTHM / ADPCM / BEEP.

What each channel holds is on its own page. The sections that turn up in the
same shape on every channel are collected under
[Shared sections](/2686V_2686VFX/en/chips/common/).

## Curve editing (ADV)

**Available.**

See [Curve editing](/2686V_2686VFX/en/chips/adv/) for the details.

## When to reach for it

- Adding a single X68000 or arcade part
- Concentrating on OPM patch design

## Files

| What | Extension |
| --- | --- |
| Preset | `.opmv.json` |
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
