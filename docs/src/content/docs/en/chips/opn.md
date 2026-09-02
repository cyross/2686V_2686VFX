---
title: OPN channel
description: The four-operator FM channel that stands for the YM2203
sidebar:
  order: 2
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opn.png" alt="OPN channel" style="width:600px;" />
	<figcaption>OPN channel</figcaption>
</figure>

OPN is the channel that stands for the FM part of the **YM2203** — the chip on
the PC-9801-26 board, a generation earlier than the OPNA.

It is built almost exactly like OPNA, but **there is no hardware LFO and no
panning**, because the hardware had neither. When you want the line-up of a
period 26 board, this is the one to pick rather than OPNA.

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

For the details, see [the reference](/2686V_2686VFX/en/reference/lists-algorithm/#opn).

## How it differs from OPNA

| Section | OPN | OPNA |
| --- | :---: | :---: |
| HW LFO | — | ✔ |
| N88 LFO | ✔ | ✔ |
| SSG HW ENV | — | ✔ |
| PAN | — | ✔ |

To wobble a pitch, use the **N88 LFO**. On a real 26 board the wobble came from
the driver's software too; this reproduces that movement as it was.

## The sections on screen

### ALGORITHM / FEEDBACK (hardware)

Picks the algorithm and the feedback. The diagram appears here.

| Knob | What it does | Automation |
| --- | --- | --- |
| **ALG** | How the operators are wired. The hardware's eight | [`OPN_ALG`](/2686V_2686VFX/en/reference/automation/opn/#opn-alg) |
| **FB** | How much OP1 feeds back into itself. 0 is none, 7 the most | [`OPN_FB1`](/2686V_2686VFX/en/reference/automation/opn/#opn-fb1) |

### AMP ENV (hardware / ours)

The level envelope on each operator. Alongside the hardware's
**AR / DR / SR / SL / RR / TL** there are **KOR / XOF / Bypass** of ours. Reach
for them when you want the SSG software envelope to be the main event.

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Attack rate. Higher rises faster | [`OPN_OP[0-3]_R_AR`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-r-ar) |
| **DR** | Decay rate. Higher falls faster | [`OPN_OP[0-3]_R_DR`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-r-dr) |
| **SL** | Sustain level. Where DR settles | [`OPN_OP[0-3]_R_SL`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-r-sl) |
| **SR** | Sustain rate. How fast it keeps falling while held | [`OPN_OP[0-3]_R_SR`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-r-sr) |
| **RR** | Release rate. How fast it dies after the key comes up | [`OPN_OP[0-3]_R_RR`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-r-rr) |
| **TL** | Total level. **Higher makes it quieter** | [`OPN_OP[0-3]_R_TL`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-r-tl) |
| **XOF** | Ignore RR and never decay (**ours**) | [`OPN_OP[0-3]_XOF`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-xof) |
| **KOR** | Hold the level for a set time after release (**ours**) | [`OPN_OP[0-3]_KOR`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-kor) |
| **Bypass** | Do not run this operator's level envelope (**ours**) | [`OPN_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-amp-bypass) |

### MUL / DET (hardware)

Multiple and detune.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MUL** | Multiple. 0 is ×0.5; 1–15 are the multiple itself | [`OPN_OP[0-3]_MUL`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-mul) |
| **DT** | Detune. The hardware's eight steps (0 and ±1–3) | [`OPN_OP[0-3]_DT`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-dt) |


### N88 LFO (from the hardware)

A recreation of the software LFO N88-BASIC used.

OPN has no hardware LFO, so this is where the wobble comes from.

| Knob | What it does | Automation |
| --- | --- | --- |
| **SP** | How fast it wobbles | [`OPN_LFO_FREQ`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-freq) |
| **SH** | Shape. Saw Up / Square / Triangle / S&H, plus two that move once | [`OPN_LFO_SHAPE`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-shape) |
| **SDLY** | Wait after key-down before it starts | [`OPN_LFO_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-sync-delay) |
| **PM Enable** | Apply the wobble on the pitch side | [`OPN_LFO_PM`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-pm) |
| **PMS** | How much it reaches the pitch | [`OPN_LFO_PMS`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-pms) |
| **PMD** | Depth on the pitch. Negative values flip the direction | [`OPN_LFO_PMD`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-pmd) |
| **AM Enable** | Apply the wobble on the level side | [`OPN_LFO_AM`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-am) |
| **AMD** | Depth on the level | [`OPN_LFO_AMD`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-amd) |
| **AMSR** | Rounding of the corners on the level side | [`OPN_LFO_ASMRT`](/2686V_2686VFX/en/reference/automation/opn/#opn-lfo-asmrt) |
| **AMS** | How much it reaches the level, per operator | [`OPN_OP[0-3]_N88AMS`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-n88ams) |


### KEY SCALE (hardware)

Changes level and rates with how high the note is.

| Knob | What it does | Automation |
| --- | --- | --- |
| **KS** | How much higher notes speed the envelope up. OFF / weak / medium / strong | [`OPN_OP[0-3]_KS`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-ks) |

### MASK (hardware)

Silences an operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MASK** | Do not sound this operator | [`OPN_OP[0-3]_MASK`](/2686V_2686VFX/en/reference/automation/opn/#opn-op-0-3-mask) |


### OPTIONAL (hardware)

Extra behaviour the hardware had, such as sound-effect mode.

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

:::tip[Moving between OPN and OPNA]
UTILITY lets you **carry parameters between OPN and OPNA**. Build a patch on
OPN, take it over to OPNA, and add the hardware LFO there.
:::

## MML entry

Like OPNA, it accepts `AR` `DR` `SR` `SL` `RR` `TL` `MUL` `DT` `KS` `MASK`.

For how to write it, see [MML entry](/2686V_2686VFX/en/reference/mml/).

## Automation

The full list of what OPN exposes to the DAW is under
[OPN automation](/2686V_2686VFX/en/reference/automation/opn/), with IDs, types, ranges and
defaults.
