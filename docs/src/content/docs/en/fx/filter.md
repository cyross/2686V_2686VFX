---
title: Filter
description: Setting up the Filter
sidebar:
  order: 1
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_filter.png" alt="Filter" style="width:400px;" />
	<figcaption>Filter</figcaption>
</figure>

Cuts everything above — or below — a frequency you choose. The most basic tool there is for setting how bright a sound is.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **TYPE** | How it cuts: low-pass / high-pass / band-pass | 1 – 3 | 1 | [`FX_FIL_TYPE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-fil-type) |
| **FQ** | Where the cut starts (Hz) | 0 – 20000 | 20000 | [`FX_FIL_FREQ`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-fil-freq) |
| **Q** | How sharp the corner is. Higher values emphasise it | 0.1 – 10 | 0.71 | [`FX_FIL_Q`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-fil-q) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_FIL_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-fil-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Bringing **FQ** down makes the sound muffled, as if it were playing in the next room. Reach for it when you want the small speakers of the era.

Raise **Q** first, then sweep **FQ**, and the corner sings. Automate that sweep and you have the classic filter rise and fall.

## Which plugins have it

All six: the five instruments (2686V / 2686VLight / 26V / 86V / OPZX7S) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
