---
title: Bit crusher
description: Setting up the Bit crusher
sidebar:
  order: 5
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_mbc.png" alt="Bit crusher" style="width:400px;" />
	<figcaption>Bit crusher</figcaption>
</figure>

Drops the bit depth and the sample rate to make things coarse.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **BIT** | Bit depth. Lower is coarser | 2 – 24 | 24 | [`FX_MBC_BITS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-mbc-bits) |
| **RATE** | How much is thrown away. Higher is coarser | 1 – 50 | 1 | [`FX_MBC_RATE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-mbc-rate) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_MBC_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-mbc-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Lowering **BIT** makes the quiet parts grainy. Raising **RATE** folds the top end back down and adds metallic overtones.

Both take continuous values. When you want the steps a real chip used, reach for the [PCM bit crusher](/2686V_2686VFX/en/fx/pcm-bitcrusher/) instead.

## Which plugins have it

All twelve: the eleven instruments (2686V / 2686VLight / 26V / 86V /
OPZX7S / OPNV / OPLV / OPMV / WTV / PCMV / PULSEV) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
