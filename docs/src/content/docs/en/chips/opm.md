---
title: OPM channel
description: The four-operator FM channel that stands for the YM2151
sidebar:
  order: 5
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opm.png" alt="OPM channel" style="width:600px;" />
	<figcaption>OPM channel</figcaption>
</figure>

OPM is the channel that stands for the FM part of the **YM2151**. It was everywhere on the X68000 and on arcade boards, and for a lot of people it *is* the sound of FM.

It has four operators, like the OPN family, but **the envelope is put together differently and the detuning is finer**. **DT2** in particular belongs to the OPM family alone: it shifts the pitch a long way and is how you get a metallic ring.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## Operators and algorithms

| Item | |
| --- | --- |
| Operators | **4** |
| Algorithms | **0–7** (the hardware's eight) |
| Feedback | OP1 only (as the hardware had it) |

For the details, see [the reference](/2686V_2686VFX/en/reference/lists-algorithm/#opm).

## How it differs from the OPN family

| Item | OPN / OPNA | OPM |
| --- | --- | --- |
| Envelope | AR / DR / SR / SL / RR | **AR / D1R / D1L / D2R / RR** |
| Detune | DT alone | **DT1 and DT2** |
| Key scale | KS | KS (**the OPP arrangement is available too**) |

Beyond the different names, what marks OPM out is that **the decay comes in two stages**. D1R sets how it first falls, D1L how far it falls, and D2R how it carries on from there.

## The sections on screen

### ALGORITHM / FEEDBACK (hardware)

Picks the algorithm and the feedback. The diagram appears here.

| Knob | What it does | Automation |
| --- | --- | --- |
| **ALG** | How the operators are wired. The hardware's eight | [`OPM_ALG`](/2686V_2686VFX/en/reference/automation/opm/#opm-alg) |
| **FB** | How much OP1 feeds back into itself | [`OPM_FB1`](/2686V_2686VFX/en/reference/automation/opm/#opm-fb1) |

### AMP ENV (hardware / ours)

**AR / D1R / D1L / D2R / RR / TL**, with **KOR / XOF / Bypass** of ours added.

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Attack rate | [`OPM_OP[0-3]_R_AR`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-r-ar) |
| **D1R** | How fast the first stage decays (DR elsewhere) | [`OPM_OP[0-3]_R_D1R`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-r-d1r) |
| **D1L** | Where the first stage settles (SL elsewhere) | [`OPM_OP[0-3]_R_D1L`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-r-d1l) |
| **D2R** | How fast the second stage decays (SR elsewhere) | [`OPM_OP[0-3]_R_D2R`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-r-d2r) |
| **RR** | Release rate | [`OPM_OP[0-3]_R_RR`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-r-rr) |
| **TL** | Total level. **Higher makes it quieter** | [`OPM_OP[0-3]_R_TL`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-r-tl) |
| **XOF** | Ignore RR and never decay (**ours**) | [`OPM_OP[0-3]_XOF`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-xof) |
| **KOR** | Hold the level for a set time after release (**ours**) | [`OPM_OP[0-3]_KOR`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-kor) |
| **Bypass** | Do not run this operator's level envelope (**ours**) | [`OPM_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-amp-bypass) |

### MUL / DET (hardware / ours)

The multiple, plus **DT1 / DT2**. DT2 shifts the pitch a long way in steps, and it is the key to bells and metal.

There is also DT3, which is ours: it ignores the frame of DT1 / DT2 and lets you set the value freely (**ours**).

| Knob | What it does | Automation |
| --- | --- | --- |
| **MUL** | Multiple | [`OPM_OP[0-3]_MUL`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-mul) |
| **MURT** | Take the multiple off the whole numbers and set it freely (**ours**) | [`OPM_OP[0-3]_MUL_RATIO`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-mul-ratio) |
| **DT1** | Fine detune | [`OPM_OP[0-3]_DT`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-dt) |
| **DT2** | Coarse detune. The hardware's four steps | [`OPM_OP[0-3]_DT2`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-dt2) |

### HW LFO (hardware)

The YM2151's hardware LFO. Shape, rate and how far it reaches are all set here.

| Knob | What it does | Automation |
| --- | --- | --- |
| **FQ** | How fast it wobbles | [`OPM_LFO_FREQ`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-freq) |
| **SDLY** | Wait after key-down before it starts | [`OPM_LFO_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-sync-delay) |
| **PM Enable** | Apply the wobble on the pitch side | [`OPM_LFO_PM`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-pm) |
| **P.SH** | Pitch side: shape. The hardware's four | [`OPM_LFO_PG_SHAPE`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-pg-shape) |
| **PMS** | How much it reaches the pitch | [`OPM_LFO_PMS`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-pms) |
| **PMD** | Depth on the pitch | [`OPM_LFO_PMD`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-pmd) |
| **AM Enable** | Apply the wobble on the level side | [`OPM_LFO_AM`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-am) |
| **A.SH** | Level side: shape | [`OPM_LFO_EG_SHAPE`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-eg-shape) |
| **AMS** | How much it reaches the level | [`OPM_LFO_AMS`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-ams) |
| **AMD** | Depth on the level | [`OPM_LFO_AMD`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-amd) |
| **AMSR** | Rounding of the corners on the level side (**ours**) | [`OPM_LFO_ASMRT`](/2686V_2686VFX/en/reference/automation/opm/#opm-lfo-asmrt) |
| **AMS Enable** | Whether each operator takes the level wobble | [`OPM_OP[0-3]_AMS_EN`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-ams-en) |

### KEY SCALE (hardware / ours)

How things change with the pitch of the note. Alongside the hardware's arrangement you can also pick **the OPP key scale** (**ours**).

#### Picking the Mode

The Mode is set per operator. **Which knobs appear depends on it.**

| Knob | What it does | Automation |
| --- | --- | --- |
| **Mode** | Which arrangement applies. OPM / OPP | [`OPM_OP[0-3]_KS_MODE`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-ks-mode) |

#### Mode = OPM

| Knob | What it does | Automation |
| --- | --- | --- |
| **KS** | The hardware's key scale. OFF / weak / medium / strong | [`OPM_OP[0-3]_KS`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-ks) |

#### Mode = OPP (**ours**)

| Knob | What it does | Automation |
| --- | --- | --- |
| **KSR** | Speed the envelope up as notes go higher | [`OPM_OP[0-3]_KSR_OPP`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-ksr-opp) |
| **KSL** | Lower the level as notes go higher | [`OPM_OP[0-3]_KSL_OPP`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-ksl-opp) |

### MASK (hardware)

Silences an operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MASK** | Do not sound this operator | [`OPM_OP[0-3]_MASK`](/2686V_2686VFX/en/reference/automation/opm/#opm-op-0-3-mask) |

### PAN (hardware)

Position in the stereo field.

| Knob | What it does | Automation |
| --- | --- | --- |
| **PAN** | Position. −1 is left, 0 centre, +1 right | [`OPM_PAN`](/2686V_2686VFX/en/reference/automation/opm/#opm-pan) |

### OPTIONAL (hardware)

Extra behaviour the hardware had.

## Shared sections

The following do not belong to any one chip. They are all described under
[Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/en/chips/common/#quality) | Drops bit depth and sample rate for texture |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/en/chips/common/#wt-pitch-mod) | Pitch modulation from wavetable memory |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

For the details of QUALITY, see [the reference](/2686V_2686VFX/en/reference/lists-quality/).

## MML entry

On top of `AR` `D1R` `D1L` `D2R` `RR` `TL` `MUL` `MASK`, OPM accepts `DT1` (`DT` / `DTO`), `DT2` (`DTT`) and `KS`.

For how to write it, see [MML entry](/2686V_2686VFX/en/reference/mml/).

## Automation

The full list of what OPM exposes to the DAW is under
[OPM automation](/2686V_2686VFX/en/reference/automation/opm/), with IDs, types, ranges and
defaults.
