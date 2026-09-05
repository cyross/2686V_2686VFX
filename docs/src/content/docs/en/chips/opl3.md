---
title: OPL3 channel
description: The four-operator FM channel that stands for the YMF262
sidebar:
  order: 4
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opl3.png" alt="OPL3 channel" style="width:600px;" />
	<figcaption>OPL3 channel</figcaption>
</figure>

OPL3 is the channel that stands for the FM part of the **YMF262**. It carries on from OPL with twice the operators and twice the waveforms — the chip everyone knows from DOS-era sound cards.

It is OPL's knack for choosing waveforms with the OPN family's four operators added on top.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## Operators and algorithms

| Item | |
| --- | --- |
| Operators | **4** |
| Algorithms | **0–7** |

Alongside the hardware's four we have added **2OPx2**, which puts two pairs of operators side by side (**ours**). It lets one channel stack two voices.

For the details, see [the reference](/2686V_2686VFX/en/reference/lists-algorithm/#opl3).

## How it differs from OPL

| Section | OPL | OPL3 |
| --- | :---: | :---: |
| Operators | 2 | **4** |
| Algorithms | 2 | **8** (ours included) |
| Waveforms | 4 | **8** |
| SSG HW ENV | ✔ | — |

## The sections on screen

### ALGORITHM / FEEDBACK (hardware / ours)

Picks the algorithm and the feedback. 2OPx2 is chosen here.

| Knob | What it does | Automation |
| --- | --- | --- |
| **ALG** | How the operators are wired | [`OPL3_ALG`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-alg) |
| **FB** | How much OP1 feeds back into itself | [`OPL3_FB1`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-fb1) |

### SHAPE (hardware)

Up from OPL's four to **eight**: sines cut, folded, or used only by halves.

| Knob | What it does | Automation |
| --- | --- | --- |
| **EG** | The operator's waveform — how the sine is cut away | [`OPL3_OP[0-3]_EG`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-eg) |

### AMP ENV (hardware / ours)

The level envelope on each operator, with **KOR / XOF / Bypass** of ours added.

The OPL family has no SR. How the decay carries on is decided by EGTP under EG SHAPE.

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Attack rate | [`OPL3_OP[0-3]_R_AR`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-r-ar) |
| **DR** | Decay rate | [`OPL3_OP[0-3]_R_DR`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-r-dr) |
| **SL** | Sustain level | [`OPL3_OP[0-3]_R_SL`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-r-sl) |
| **RR** | Release rate | [`OPL3_OP[0-3]_R_RR`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-r-rr) |
| **TL** | Total level. **Higher makes it quieter** | [`OPL3_OP[0-3]_R_TL`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-r-tl) |
| **XOF** | Ignore RR and never decay (**ours**) | [`OPL3_OP[0-3]_XOF`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-xof) |
| **KOR** | Hold the level for a set time after release (**ours**) | [`OPL3_OP[0-3]_KOR`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-kor) |
| **Bypass** | Do not run this operator's level envelope (**ours**) | [`OPL3_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-amp-bypass) |

### EG SHAPE (hardware)

Picks the type of envelope.

| Knob | What it does | Automation |
| --- | --- | --- |
| **EGTP** | Hold the level while the key is down (on), or keep falling (off) | [`OPL3_OP[0-3]_EG_TYP`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-eg-typ) |

### LFO (hardware)

An LFO that is independent per operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **VIB** | Apply the wobble on the pitch side | [`OPL3_OP[0-3]_LFO_VIB`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-lfo-vib) |
| **PMS** | How fast the pitch wobbles (Hz) | [`OPL3_OP[0-3]_LFO_PMS`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-lfo-pms) |
| **PMD** | How deep the pitch wobbles (cents) | [`OPL3_OP[0-3]_LFO_PMD`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-lfo-pmd) |
| **AM** | Apply the wobble on the level side | [`OPL3_OP[0-3]_LFO_AM`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-lfo-am) |
| **AMS** | How fast the level wobbles (Hz) | [`OPL3_OP[0-3]_LFO_AMS`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-lfo-ams) |
| **AMD** | How deep the level wobbles (dB) | [`OPL3_OP[0-3]_LFO_AMD`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-lfo-amd) |

### MUL / DET (hardware)

Multiple and detune.

The OPL family has no detune. To shift the pitch, use DT3 under [MUL/DET](/2686V_2686VFX/en/chips/common/#muldet).

| Knob | What it does | Automation |
| --- | --- | --- |
| **MUL** | Multiple | [`OPL3_OP[0-3]_MUL`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-mul) |

### KEY SCALE (hardware)

**KSL (level) and KSR (rate)** are set separately.

| Knob | What it does | Automation |
| --- | --- | --- |
| **KSR** | Speed the envelope up as notes go higher | [`OPL3_OP[0-3]_KSR`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-ksr) |
| **KSL** | How much the level drops as notes go higher. OFF / 1.5 / 3.0 / 6.0 dB/oct | [`OPL3_OP[0-3]_KSL`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-ksl) |

### MASK (hardware)

Silences an operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MASK** | Do not sound this operator | [`OPL3_OP[0-3]_MASK`](/2686V_2686VFX/en/reference/automation/opl3/#opl3-op-0-3-mask) |

### OPTIONAL (hardware)

Extra behaviour the hardware had.

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

:::tip[Moving between OPL and OPL3]
UTILITY lets you **carry parameters between OPL and OPL3**. Bring a two-operator patch built on OPL across, and add the other two here.
:::

## MML entry

It takes the same symbols as OPL: `AR` `DR` `RR` `SL` `TL` `MUL` `MASK` `KSL` `KSR` `AM` `PM` (`VIB`) `EGTYPE`.

For how to write it, see [MML entry](/2686V_2686VFX/en/reference/mml/).

## Automation

The full list of what OPL3 exposes to the DAW is under
[OPL3 automation](/2686V_2686VFX/en/reference/automation/opl3/), with IDs, types, ranges and
defaults.
