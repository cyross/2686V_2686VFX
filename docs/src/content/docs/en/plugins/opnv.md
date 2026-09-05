---
title: OPNV
description: What is in OPNV, and when to reach for it
sidebar:
  order: 7
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/OPNV.png" alt="OPNV logo" width="128" height="128" />
	<figcaption>Logo</figcaption>
</figure>

Narrowed down to **the two OPN-family channels** — YM2608 (OPNA) and
YM2203 (OPN). Handy when you want one FM part in a track without
carrying every other channel along with it.

## Channels it carries

| Channel | |
| --- | :---: |
| [OPNA](/2686V_2686VFX/en/chips/opna/) | ✔ |
| [OPN](/2686V_2686VFX/en/chips/opn/) | ✔ |

The ones it does not have are OPL / OPL3 / OPM / OPZX7 / SSG / WAVETABLE / WT2 / WTPLUS / RHYTHM / ADPCM / BEEP.

What each channel holds is on its own page. The sections that turn up in the
same shape on every channel are collected under
[Shared sections](/2686V_2686VFX/en/chips/common/).

## Curve editing (ADV)

**Available.**

See [Curve editing](/2686V_2686VFX/en/chips/adv/) for the details.

## When to reach for it

- Adding a single FM part to a track
- Writing PC-9801-flavoured FM patches without the SSG side

## Files

| What | Extension |
| --- | --- |
| Preset | `.opnv.json` |
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
