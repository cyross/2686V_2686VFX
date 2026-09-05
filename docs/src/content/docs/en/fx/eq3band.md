---
title: 3-band EQ
description: Setting up the 3-band EQ
sidebar:
  order: 2
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_3beq.png" alt="3-band EQ" style="width:400px;" />
	<figcaption>3-band EQ</figcaption>
</figure>

Splits the sound into low, middle and high, and lets you raise or lower each one.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **LGn** | How much low end (dB) | -15 – 15 | 0 | [`FX_EQ3B_LGDB`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-eq3b-lgdb) |
| **MFq** | Which frequency counts as the middle (Hz) | 500 – 3000 | 2000 | [`FX_EQ3B_MFREQ`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-eq3b-mfreq) |
| **MGn** | How much middle (dB) | -15 – 15 | 0 | [`FX_EQ3B_MGDB`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-eq3b-mgdb) |
| **HGn** | How much high end (dB) | -15 – 15 | 0 | [`FX_EQ3B_HGDB`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-eq3b-hgdb) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_EQ3B_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-eq3b-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Where the filter takes away, this adds and subtracts. When you want to change the character but keep the core of the sound, this is the better tool.

Moving **MFq** changes what the same amount of gain lands on. Lift the band a voice sits in, bring out the body of a drum, and so on.

## Which plugins have it

All twelve: the eleven instruments (2686V / 2686VLight / 26V / 86V /
OPZX7S / OPNV / OPLV / OPMV / WTV / PCMV / PULSEV) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
