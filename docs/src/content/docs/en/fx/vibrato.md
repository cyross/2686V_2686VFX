---
title: Vibrato
description: Setting up the Vibrato
sidebar:
  order: 4
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_vibrato.png" alt="Vibrato" style="width:400px;" />
	<figcaption>Vibrato</figcaption>
</figure>

Wobbles the pitch at a steady rate.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **RATE** | How fast it wobbles (Hz) | 0.1 – 10 | 2 | [`FX_VIB_RATE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-vib-rate) |
| **DPTH** | How deep the wobble goes | 0 – 1 | 0.5 | [`FX_VIB_DEPTH`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-vib-depth) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_VIB_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-vib-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Kept shallow, it gives the unsteadiness of tape. Deep and fast, it turns metallic.

Set **MIX** around halfway and the wobbled signal sits alongside the steady one, which thickens the sound.

## Which plugins have it

All six: the five instruments (2686V / 2686VLight / 26V / 86V / OPZX7S) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
