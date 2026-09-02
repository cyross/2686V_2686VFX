---
title: Delay
description: Setting up the Delay
sidebar:
  order: 7
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_delay.png" alt="Delay" style="width:400px;" />
	<figcaption>Delay</figcaption>
</figure>

Holds the sound back and repeats it.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **TIME** | How long it is held back (ms) | 10 – 1000 | 375 | [`FX_DLY_TIME`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-dly-time) |
| **FB** | How much is fed back. Higher repeats for longer | 0 – 0.95 | 0.4 | [`FX_DLY_FB`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-dly-fb) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_DLY_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-dly-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Line **TIME** up with the tempo and the repeats lock to the groove. A quarter note is `60000 ÷ BPM` milliseconds — at 120 BPM that is 500, and an eighth note is 250.

**FB** stops at 0.95. Beyond that the signal runs away and never dies, so there is a ceiling.

## Which plugins have it

All six: the five instruments (2686V / 2686VLight / 26V / 86V / OPZX7S) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
