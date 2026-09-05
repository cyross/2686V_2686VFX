---
title: WTV
description: What is in WTV, and when to reach for it
sidebar:
  order: 10
---

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/logos/WTV.png" alt="WTV logo" width="128" height="128" />
	<figcaption>Logo</figcaption>
</figure>

Narrowed down to **the three wavetable channels** — WT, WT2 and WT+.
One plugin for parts built from waveforms you draw or load yourself.

## What the plugin looks like

<figure class="shot">
	<img src="/2686V_2686VFX/ui/plugin/WTV.png" alt="WTV, the whole window" width="1426" height="738" />
	<figcaption>The whole window</figcaption>
</figure>

<figure class="shot shot--sm">
	<img src="/2686V_2686VFX/ui/about/WTV.png" alt="WTV ABOUT tab" width="373" height="486" />
	<figcaption>ABOUT tab</figcaption>
</figure>

## Channels it carries

| Channel | |
| --- | :---: |
| [WAVETABLE](/2686V_2686VFX/en/chips/wt/) | ✔ |
| [WT2](/2686V_2686VFX/en/chips/wt2/) | ✔ |
| [WTPLUS](/2686V_2686VFX/en/chips/wtplus/) | ✔ |

The ones it does not have are OPNA / OPN / OPL / OPL3 / OPM / OPZX7 / SSG / RHYTHM / ADPCM / BEEP.

What each channel holds is on its own page. The sections that turn up in the
same shape on every channel are collected under
[Shared sections](/2686V_2686VFX/en/chips/common/).

## Curve editing (ADV)

**Available.**

See [Curve editing](/2686V_2686VFX/en/chips/adv/) for the details.

## When to reach for it

- Adding a part with wavetable character
- Switching between waveforms across the 32 slots of WT+

## Files

| What | Extension |
| --- | --- |
| Preset | `.wtv.json` |
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
