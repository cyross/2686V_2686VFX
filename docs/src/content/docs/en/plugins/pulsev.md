---
title: PULSEV
description: What is in PULSEV, and when to reach for it
sidebar:
  order: 12
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/PULSEV.png" alt="PULSEV logo" width="128" height="128" />
	<figcaption>Logo</figcaption>
</figure>

Narrowed down to **the two pulse channels** — SSG and BEEP. A good fit
when you want one thin, square-wave part.

## What the plugin looks like

<figure class="shot">
	<img src="/2686V_2686VFX/ui/plugin/PULSEV.png" alt="PULSEV, the whole window" width="1426" height="740" />
	<figcaption>The whole window</figcaption>
</figure>

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/ui/about/PULSEV.png" alt="PULSEV ABOUT tab" width="377" height="491" />
	<figcaption>ABOUT tab</figcaption>
</figure>

## Channels it carries

| Channel | |
| --- | :---: |
| [SSG](/2686V_2686VFX/en/chips/ssg/) | ✔ |
| [BEEP](/2686V_2686VFX/en/chips/beep/) | ✔ |

The ones it does not have are OPNA / OPN / OPL / OPL3 / OPM / OPZX7 / WAVETABLE / WT2 / WTPLUS / RHYTHM / ADPCM.

What each channel holds is on its own page. The sections that turn up in the
same shape on every channel are collected under
[Shared sections](/2686V_2686VFX/en/chips/common/).

## Curve editing (ADV)

**Available.**

See [Curve editing](/2686V_2686VFX/en/chips/adv/) for the details.

## When to reach for it

- Adding an SSG bass line or arpeggio
- Reaching for a bare, beeper-like square wave

## Files

| What | Extension |
| --- | --- |
| Preset | `.pulsev.json` |
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
