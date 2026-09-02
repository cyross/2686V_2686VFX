---
title: Tremolo
description: Setting up the Tremolo
sidebar:
  order: 3
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_tremollo.png" alt="Tremolo" style="width:400px;" />
	<figcaption>Tremolo</figcaption>
</figure>

Wobbles the level at a steady rate.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **RATE** | How fast it wobbles (Hz) | 0.1 – 20 | 5 | [`FX_TRM_RATE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-trm-rate) |
| **DPTH** | How deep the wobble goes | 0 – 1 | 1 | [`FX_TRM_DEPTH`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-trm-depth) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_TRM_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-trm-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Fast and deep, it sounds as though the note is being chopped. Line **RATE** up with the tempo and it becomes part of the rhythm.

The instruments have their own way of wobbling the level ([LFO](/2686V_2686VFX/en/chips/common/#lfo)) — but that one works per note, while this one works on everything coming out.

## Which plugins have it

All six: the five instruments (2686V / 2686VLight / 26V / 86V / OPZX7S) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
