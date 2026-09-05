---
title: Reverb
description: Setting up the Reverb
sidebar:
  order: 8
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_reverb.png" alt="Reverb" style="width:400px;" />
	<figcaption>Reverb</figcaption>
</figure>

Adds the sound of a space.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **SIZE** | How big the space is. Bigger rings for longer | 0 – 1 | 0.5 | [`FX_RVB_SIZE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-rvb-size) |
| **DAMP** | How fast the top end dies. Higher dies sooner | 0 – 1 | 0.5 | [`FX_RVB_DAMP`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-rvb-damp) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_RVB_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-rvb-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Raise **DAMP** and the bright part of the tail disappears early, which gives a soft decay. Lower it for a harder, brighter space.

Chip sounds are flat on their own. Even 0.2 on **MIX** is enough to pull a part together.

## Which plugins have it

All twelve: the eleven instruments (2686V / 2686VLight / 26V / 86V /
OPZX7S / OPNV / OPLV / OPMV / WTV / PCMV / PULSEV) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
