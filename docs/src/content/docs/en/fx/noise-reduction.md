---
title: Noise reduction
description: Setting up the noise reduction
sidebar:
  order: 7
---

The **gate** and the **high cut** that the instruments' [QUALITY(PCM)](/2686V_2686VFX/en/chips/common/#qualitypcm) noise reduction applies during playback, as an effect of their own. **Only 2686VFX has this one** (since 3.6.3).

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **RATE** | The sample rate the LPF takes as its reference. 15 steps | 1 – 15 | 9 (16kHz) | [`FX_NR_RATE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-nr-rate) |
| **GATE** | Closes once the sound falls below **GATE.LV**, removing faint noise | on / off | off | [`FX_NR_GATE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-nr-gate) |
| **GATE.LV** | The level the gate closes at (dB) | -96 – -24 | -60 | [`FX_NR_GATE_LV`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-nr-gate-lv) |
| **LPF** | Cuts the highs to tame the grain. Off / Light / Medium / Strong | 1 – 4 | 1 (Off) | [`FX_NR_LPF`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-nr-lpf) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_NR_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-nr-mix) |

It works exactly like QUALITY(PCM) in the instruments.

- **GATE** takes 1 ms to open and 30 ms to close, so neither the attack nor the tail gets clipped. It also removes DC (an off-centre offset). **GATE.LV** is only available while GATE is on
- **LPF** cuts from 90%, 70% or 50% (Light, Medium, Strong) of the top of **RATE**'s band, which is half the rate. At 16kHz and Medium, for example, everything above 5.6kHz is cut

With both GATE and LPF off, the sound passes through untouched.

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

## Where to use it

Place it **after** the [PCM bit crusher](/2686V_2686VFX/en/fx/pcm-bitcrusher/) and set **RATE** to the same rate as the crusher, and it acts just as QUALITY(PCM) does in the instruments: the interpolation's aliasing and the hiss of coarse quantisation go away.

**NR: Resample**, which cuts the highs before decimating, only makes sense together with the decimation itself, so it lives in the PCM bit crusher.

**GATE** helps with material such as 1-bit DPCM, whose output does not return to 0 when the sound stops.

## Which plugins have it

**Only 2686VFX.**

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
