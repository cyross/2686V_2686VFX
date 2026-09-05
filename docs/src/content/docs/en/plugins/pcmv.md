---
title: PCMV
description: What is in PCMV, and when to reach for it
sidebar:
  order: 11
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/PCMV.png" alt="PCMV logo" width="128" height="128" />
	<figcaption>Logo</figcaption>
</figure>

Narrowed down to **the two sample-playing channels** — RHYTHM and
ADPCM. One plugin covering both the drum kit and sample playback.

## What the plugin looks like

<figure class="shot">
	<img src="/2686V_2686VFX/ui/plugin/PCMV.png" alt="PCMV, the whole window" width="1426" height="736" />
	<figcaption>The whole window</figcaption>
</figure>

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/ui/about/PCMV.png" alt="PCMV ABOUT tab" width="384" height="497" />
	<figcaption>ABOUT tab</figcaption>
</figure>

## Channels it carries

| Channel | |
| --- | :---: |
| [RHYTHM](/2686V_2686VFX/en/chips/rhythm/) | ✔ |
| [ADPCM](/2686V_2686VFX/en/chips/adpcm/) | ✔ |

The ones it does not have are OPNA / OPN / OPL / OPL3 / OPM / OPZX7 / SSG / WAVETABLE / WT2 / WTPLUS / BEEP.

What each channel holds is on its own page. The sections that turn up in the
same shape on every channel are collected under
[Shared sections](/2686V_2686VFX/en/chips/common/).

## Curve editing (ADV)

**Available.**

See [Curve editing](/2686V_2686VFX/en/chips/adv/) for the details.

## When to reach for it

- Covering the rhythm part with a single plugin
- Playing back audio you have loaded

## Files

| What | Extension |
| --- | --- |
| Preset | `.pcmv.json` |
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
