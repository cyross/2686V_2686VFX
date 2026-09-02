---
title: PCM bit crusher
description: Setting up the PCM bit crusher
sidebar:
  order: 6
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_pcmbc.png" alt="PCM bit crusher" style="width:400px;" />
	<figcaption>PCM bit crusher</figcaption>
</figure>

Crushes on the very same steps as the instruments' [QUALITY(PCM)](/2686V_2686VFX/en/chips/common/#qualitypcm). **Only 2686VFX has this one.**

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **BIT** | Bit depth and compression scheme. 12 steps | 1 – 12 | 12 | [`FX_PCMBC_BITS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-pcmbc-bits) |
| **RATE** | Sample rate. 15 steps | 1 – 15 | 9 | [`FX_PCMBC_RATE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-pcmbc-rate) |
| **INTP** | How the gaps are filled on the way back. 7 kinds | 0 – 6 | 1 | [`FX_PCMBC_INTP`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-pcmbc-intp) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_PCMBC_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-pcmbc-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Where the bit crusher takes continuous values, this one takes **the steps the hardware actually had**. When you are after the texture of a particular machine, pick the scheme that machine used.

Drop **RATE** and set **INTP** to Nearest for the most period-correct grain. B-Spline goes the other way — it smooths things over, so the sound feels further away.

The full list of what you can pick is in [QUALITY lists](/2686V_2686VFX/en/reference/lists-quality/).

## Which plugins have it

**Only 2686VFX.**

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
