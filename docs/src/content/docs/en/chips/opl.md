---
title: OPL channel
description: The two-operator FM channel that stands for the YM3812 / YM2413
sidebar:
  order: 3
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opl.png" alt="OPL channel" style="width:600px;" />
	<figcaption>OPL channel</figcaption>
</figure>

OPL is the channel that stands for the FM part of the **YM3812 (OPL2)** and the **YM2413 (OPLL)** — the family behind MSX-MUSIC and a great many arcade boards. It builds its sound quite differently from the OPN family.

The biggest difference is that it has **only two operators**. That limits what you can combine, but it comes with a weapon of its own: **you can pick the waveform**.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## Operators and algorithms

| Item | |
| --- | --- |
| Operators | **2** |
| Algorithms | **0–1** (FM and additive) |

Having only two algorithms is exactly as the hardware was: either OP2 modulates OP1 (FM), or the two are summed (additive).

For the details, see [the reference](/2686V_2686VFX/en/reference/lists-algorithm/#opl).

## The sections on screen

### ALGORITHM / FEEDBACK (hardware)

Picks FM or additive. The diagram appears here.

| Knob | What it does | Automation |
| --- | --- | --- |
| **ALG** | How the operators are wired | [`OPL_ALG`](/2686V_2686VFX/en/reference/automation/opl/#opl-alg) |
| **FB** | How much OP1 feeds back into itself | [`OPL_FB1`](/2686V_2686VFX/en/reference/automation/opl/#opl-fb1) |

### SHAPE (hardware)

**This is the heart of the family.** You pick a waveform made by cutting or folding a sine. Where the OPN family can only put out a sine, OPL shapes its tone by changing the wave itself.

| Knob | What it does | Automation |
| --- | --- | --- |
| **EG** | The operator's waveform — how the sine is cut away | [`OPL_OP[0-1]_EG`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-eg) |

### AMP ENV (hardware / ours)

The level envelope on each operator, with **KOR / XOF / Bypass** of ours added.

The OPL family has no SR. How the decay carries on is decided by EGTP under EG SHAPE.

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Attack rate | [`OPL_OP[0-1]_R_AR`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-r-ar) |
| **DR** | Decay rate | [`OPL_OP[0-1]_R_DR`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-r-dr) |
| **SL** | Sustain level | [`OPL_OP[0-1]_R_SL`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-r-sl) |
| **RR** | Release rate | [`OPL_OP[0-1]_R_RR`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-r-rr) |
| **TL** | Total level. **Higher makes it quieter** | [`OPL_OP[0-1]_R_TL`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-r-tl) |
| **SUS** | Hold the sound on after key-off | [`OPL_OP[0-1]_SUS`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-sus) |
| **XOF** | Ignore RR and never decay (**ours**) | [`OPL_OP[0-1]_XOF`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-xof) |
| **KOR** | Hold the level for a set time after release (**ours**) | [`OPL_OP[0-1]_KOR`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-kor) |
| **Bypass** | Do not run this operator's level envelope (**ours**) | [`OPL_OP[0-1]_AMP_BYPASS`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-amp-bypass) |

### EG SHAPE (hardware)

Picks the type of envelope: one that decays away, or one that holds while the key is down.

| Knob | What it does | Automation |
| --- | --- | --- |
| **EGTP** | Hold the level while the key is down (on), or keep falling (off) | [`OPL_OP[0-1]_EG_TYP`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-eg-typ) |

### LFO (hardware)

A hardware LFO that is **independent per operator**. Unlike the OPN family's LFO, which works on the whole channel, this one lets you wobble just one of the two.

| Knob | What it does | Automation |
| --- | --- | --- |
| **VIB** | Apply the wobble on the pitch side | [`OPL_OP[0-1]_LFO_VIB`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-lfo-vib) |
| **PMS** | How fast the pitch wobbles (Hz) | [`OPL_OP[0-1]_LFO_PMS`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-lfo-pms) |
| **PMD** | How deep the pitch wobbles (cents) | [`OPL_OP[0-1]_LFO_PMD`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-lfo-pmd) |
| **AM** | Apply the wobble on the level side | [`OPL_OP[0-1]_LFO_AM`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-lfo-am) |
| **AMS** | How fast the level wobbles (Hz) | [`OPL_OP[0-1]_LFO_AMS`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-lfo-ams) |
| **AMD** | How deep the level wobbles (dB) | [`OPL_OP[0-1]_LFO_AMD`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-lfo-amd) |

### MUL / DET (hardware)

Multiple and detune.

The OPL family has no detune. To shift the pitch, use DT3 under [MUL/DET](/2686V_2686VFX/en/chips/common/#muldet).

| Knob | What it does | Automation |
| --- | --- | --- |
| **MUL** | Multiple | [`OPL_OP[0-1]_MUL`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-mul) |

### KEY SCALE (hardware)

**KSL (level) and KSR (rate)** are set separately. Having those two apart is characteristic of the OPL family.

| Knob | What it does | Automation |
| --- | --- | --- |
| **KSR** | Speed the envelope up as notes go higher | [`OPL_OP[0-1]_KSR`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-ksr) |
| **KSL** | How much the level drops as notes go higher. OFF / 1.5 / 3.0 / 6.0 dB/oct | [`OPL_OP[0-1]_KSL`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-ksl) |

### MASK (hardware)

Silences an operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MASK** | Do not sound this operator | [`OPL_OP[0-1]_MASK`](/2686V_2686VFX/en/reference/automation/opl/#opl-op-0-1-mask) |

### INITIALIZE / OPTIONAL

Resetting, and the extra behaviour the hardware had.

## Shared sections

The following do not belong to any one chip. They are all described under
[Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/en/chips/common/#quality) | Drops bit depth and sample rate for texture |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/en/chips/common/#wt-pitch-mod) | Pitch modulation from wavetable memory |
| [**WT AMP MOD**](/2686V_2686VFX/en/chips/common/#wt-amp-mod) | Level modulation from wavetable memory |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

For the details of QUALITY, see [the reference](/2686V_2686VFX/en/reference/lists-quality/).

:::tip[Trying the OPLL voices]
The bundled presets include an `OPLL Roms` folder. They trace the OPLL's ROM voices, so you can hear the period sound straight away. For how they may be used, see [License](/2686V_2686VFX/en/reference/license/).
:::

## MML entry

On top of `AR` `DR` `RR` `SL` `TL` `MUL` `MASK`, OPL accepts the ones peculiar to this family: `KSL` `KSR` `AM` `PM` (`VIB`) `EGTYPE`.

For how to write it, see [MML entry](/2686V_2686VFX/en/reference/mml/).

## Automation

The full list of what OPL exposes to the DAW is under
[OPL automation](/2686V_2686VFX/en/reference/automation/opl/), with IDs, types, ranges and
defaults.
