---
title: OPNA channel
description: The four-operator FM channel that stands for the YM2608
sidebar:
  order: 1
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opna.png" alt="OPNA channel" style="width:600px;" />
	<figcaption>OPNA channel</figcaption>
</figure>

OPNA is the channel that stands for the FM part of the **YM2608B** — the chip on
the PC-9801-86 board, and where this series gets its name.

Alongside four-operator FM it carries what the hardware had: **the SSG hardware
envelope**, **the hardware LFO**, and even the **software LFO** N88-BASIC used at
the time. Among the FM channels it is one of the best equipped.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

When you are after the hardware's sound, leaving the **ours** entries at their defaults will get you closer.

## Operators and algorithms

| Item | |
| --- | --- |
| Operators | **4** |
| Algorithms | **0–7** (the hardware's eight) |
| Feedback | OP1 only (as the hardware had it) |

The algorithm diagram is drawn on screen as you work, so you can see which
operator is modulating what the moment you pick one.

For the details, see [the reference](/2686V_2686VFX/en/reference/lists-algorithm/#opna).

## The sections on screen

### ALGORITHM / FEEDBACK (hardware)

Picks the algorithm and the feedback. The diagram appears here.

| Knob | What it does | Automation |
| --- | --- | --- |
| **ALG** | How the operators are wired. The hardware's eight | [`OPNA_ALG`](/2686V_2686VFX/en/reference/automation/opna/#opna-alg) |
| **FB** | How much OP1 feeds back into itself. 0 is none, 7 the most | [`OPNA_FB1`](/2686V_2686VFX/en/reference/automation/opna/#opna-fb1) |

### AMP ENV (hardware / ours)

The level envelope on each operator. Alongside the hardware's
**AR / DR / SR / SL / RR / TL** there are these of ours:

| Item | What it does |
| --- | --- |
| **KOR** | The level does not decay until a set time after release |
| **XOF** | Ignores RR and does not decay |
| **Bypass** | Does not run the level envelope |

All three are for **making the SSG software envelope the main event** — stop the
FM envelope, and shape the level entirely with the SSG side's steps.

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Attack rate. Higher rises faster | [`OPNA_OP[0-3]_R_AR`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-r-ar) |
| **DR** | Decay rate. Higher falls faster | [`OPNA_OP[0-3]_R_DR`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-r-dr) |
| **SL** | Sustain level. Where DR settles | [`OPNA_OP[0-3]_R_SL`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-r-sl) |
| **SR** | Sustain rate. How fast it keeps falling while held | [`OPNA_OP[0-3]_R_SR`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-r-sr) |
| **RR** | Release rate. How fast it dies after the key comes up | [`OPNA_OP[0-3]_R_RR`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-r-rr) |
| **TL** | Total level. **Higher makes it quieter** | [`OPNA_OP[0-3]_R_TL`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-r-tl) |
| **XOF** | Ignore RR and never decay (**ours**) | [`OPNA_OP[0-3]_XOF`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-xof) |
| **KOR** | Hold the level for a set time after release (**ours**) | [`OPNA_OP[0-3]_KOR`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-kor) |
| **Bypass** | Do not run this operator's level envelope (**ours**) | [`OPNA_OP[0-3]_AMP_BYPASS`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-amp-bypass) |

### MUL / DET (hardware)

Multiple and detune. Detuning the operators against each other is what gives you beating and extra overtones.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MUL** | Multiple. 0 is ×0.5; 1–15 are the multiple itself | [`OPNA_OP[0-3]_MUL`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-mul) |
| **DT** | Detune. The hardware's eight steps (0 and ±1–3) | [`OPNA_OP[0-3]_DT`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-dt) |


### HW LFO (hardware)

The hardware LFO the YM2608 carried. It applies to the whole channel.

The rate was one setting for the whole chip on the hardware; here the rate and
the amount are set **per operator** (**ours**).

| Knob | What it does | Automation |
| --- | --- | --- |
| **FQ** | How fast it wobbles. The hardware's eight steps (3.98Hz–72.2Hz) | [`OPNA_OP[0-3]_LFO_FREQS`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-lfo-freqs) |
| **SDLY** | Wait after key-down before it starts (**ours**) | [`OPNA_OP[0-3]_LFO_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-lfo-sync-delay) |
| **PM** | Apply the wobble on the pitch side | [`OPNA_OP[0-3]_LFO_VIB`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-lfo-vib) |
| **PMS** | Pitch swing. Eight steps from 0 to ±80 cents | [`OPNA_OP[0-3]_LFO_PMS`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-lfo-pms) |
| **AM** | Apply the wobble on the level side | [`OPNA_OP[0-3]_LFO_AM`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-lfo-am) |
| **AMS** | Level swing. 0dB / 1.4dB / 5.9dB / 11.8dB | [`OPNA_OP[0-3]_LFO_AMS`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-lfo-ams) |

### N88 LFO (from the hardware)

A recreation of **the software LFO N88-BASIC used**. It runs separately from the hardware LFO, so you can have both at once.

What the programs of the day did by hand to wobble a pitch, you can now build from the screen.

| Knob | What it does | Automation |
| --- | --- | --- |
| **SP** | How fast it wobbles | [`OPNA_LFO_FREQ`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-freq) |
| **SH** | Shape. Saw Up / Square / Triangle / S&H, plus two that move once | [`OPNA_LFO_SHAPE`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-shape) |
| **SDLY** | Wait after key-down before it starts | [`OPNA_LFO_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-sync-delay) |
| **PM Enable** | Apply the wobble on the pitch side | [`OPNA_LFO_PM`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-pm) |
| **PMS** | How much it reaches the pitch | [`OPNA_LFO_PMS`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-pms) |
| **PMD** | Depth on the pitch. Negative values flip the direction | [`OPNA_LFO_PMD`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-pmd) |
| **AM Enable** | Apply the wobble on the level side | [`OPNA_LFO_AM`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-am) |
| **AMD** | Depth on the level | [`OPNA_LFO_AMD`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-amd) |
| **AMSR** | Rounding of the corners on the level side | [`OPNA_LFO_ASMRT`](/2686V_2686VFX/en/reference/automation/opna/#opna-lfo-asmrt) |
| **AMS** | How much it reaches the level, per operator | [`OPNA_OP[0-3]_N88AMS`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-n88ams) |


### SSG HW ENV (hardware / ours)

The SSG's hardware envelope, usable on the FM side too. Alongside the
hardware's eight shapes there are **36 of ours**, and the MIN / MAX of the range
can be set as well (**ours**).

This is the SSG envelope applied per operator. The one that applies to the whole
channel is [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env).

| Knob | What it does | Automation |
| --- | --- | --- |
| **SHPE** | Shape. Normal (off) plus the hardware's eight | [`OPNA_OP[0-3]_SE`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-se) |
| **FQ** | How fast it repeats | [`OPNA_OP[0-3]_SEFREQ`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-sefreq) |

### KEY SCALE (hardware)

Changes level and rates with how high the note is.

| Knob | What it does | Automation |
| --- | --- | --- |
| **KS** | How much higher notes speed the envelope up. OFF / weak / medium / strong | [`OPNA_OP[0-3]_KS`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-ks) |

### MASK (hardware)

Silences an operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MASK** | Do not sound this operator | [`OPNA_OP[0-3]_MASK`](/2686V_2686VFX/en/reference/automation/opna/#opna-op-0-3-mask) |


### PAN (hardware)

Position in the stereo field.

| Knob | What it does | Automation |
| --- | --- | --- |
| **PAN** | Position. −1 is left, 0 centre, +1 right | [`OPNA_PAN`](/2686V_2686VFX/en/reference/automation/opna/#opna-pan) |

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

## MML entry

Each operator has an **MML** button. OPNA accepts `AR` `DR` `SR` `SL` `RR` `TL` `MUL` `DT` `KS` `MASK`.

For how to write it, see [MML entry](/2686V_2686VFX/en/reference/mml/).

## Automation

The full list of what OPNA exposes to the DAW is under
[OPNA automation](/2686V_2686VFX/en/reference/automation/opna/), with IDs, types, ranges and
defaults.
