---
title: SFC echo
description: Setting up the SFC echo
sidebar:
  order: 9
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_sfc_echo.png" alt="SFC echo" style="width:400px;" />
	<figcaption>SFC echo</figcaption>
</figure>

A recreation of the echo built into the Super Famicom's sound hardware. It is a delay with **an eight-coefficient filter** on it, so the tone changes with every repeat.

## Knobs

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **TIME** | How long it is held back (ms) | 0 – 240 | 0 | [`FX_SFCECHO_TIME`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-time) |
| **FB** | How much is fed back. **Negative values** invert the phase | -1 – 1 | 0 | [`FX_SFCECHO_FB`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fb) |
| **MIX** | Blend against the dry signal | 0 – 1 | 0 | [`FX_SFCECHO_MIX`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-mix) |

**MIX** is how much of the effect is blended with the dry signal. At 0.0 you hear the input untouched; at 1.0 you hear only the effect.

**Bypass** takes the effect out of the chain. It starts switched on, and MIX starts at 0.0 — switch bypass off and raise MIX before you reach for anything else.

### FIR coefficients

The filter the repeats pass through. Eight coefficients decide its shape.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **FC0** | Coefficient 0 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC1** | Coefficient 1 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC2** | Coefficient 2 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC3** | Coefficient 3 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC4** | Coefficient 4 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC5** | Coefficient 5 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC6** | Coefficient 6 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |
| **FC7** | Coefficient 7 | -128 – 127 | 0 | [`FX_SFCECHO_FIR_COEF[0-7]`](/2686V_2686VFX/en/reference/automation/fx-plugin/#fx-sfcecho-fir-coef-0-7) |

## Where to use it

Being able to put **FB** negative is straight from the hardware. Every repeat flips over, which gives the effect its particular swirl.

**FC0–FC7** are the coefficients of the filter the repeats pass through. Make FC0 alone large for plain repeats; alternate the signs and the top end is emphasised.

## Which plugins have it

All six: the five instruments (2686V / 2686VLight / 26V / 86V / OPZX7S) and 2686VFX.

For how to change the order, see [About FX](/2686V_2686VFX/en/fx/).
