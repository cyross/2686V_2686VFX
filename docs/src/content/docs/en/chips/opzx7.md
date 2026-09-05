---
title: OPZX7S channel
description: An eight-operator FM channel of our own, taking the best of OPZ, OPX, MA-7 and OPS
sidebar:
  order: 6
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opzx7s.png" alt="OPZX7S channel" style="width:600px;" />
	<figcaption>OPZX7S channel</figcaption>
</figure>

OPZX7S is **a channel of our own — no such chip ever existed**. It takes what is good from the FM sections of the OPZ, the OPX, the MA-7 and the OPS, and adds extensions of ours on top.

It is **the most open-ended** channel in the series, and the dedicated [OPZX7S](/2686V_2686VFX/en/plugins/compare/) plugin narrows itself down to this one alone.

:::note[Not a recreation of any real chip]
A good deal about the chips it draws on is still not known — the OPX especially — so **a 100% recreation is not possible.** What could be reproduced is reproduced; the rest is built as close as we could get.
:::

## Operators and algorithms

| Item | |
| --- | --- |
| Operators | **up to 8** |
| Algorithms | Pick from OPX / OPZ / MA-7 / OPS, plus 1OP–8OP of our own |
| Feedback | **independent per operator** (a real number, -8.0 to 8.0) |

Feedback set **per operator, as a real number** is something none of the chips it draws on could do. Negative values are allowed too.

For the details, see [the reference](/2686V_2686VFX/en/reference/lists-algorithm/#opzx7s).

### The algorithm matrix

Instead of picking a fixed algorithm, **you can wire the modulation and feedback paths yourself with checkboxes.**

- From 1OP to 8OP, say directly which operator reaches which
- Routings no real specification would allow are available

The algorithm diagram is drawn on screen as you work, so you can see the wiring you just built.

## The sections on screen

### ALGORITHM / FEEDBACK

Choosing the algorithm, the matrix, and the feedback on each operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **ALG MODE** | Use a built-in algorithm, or build one in the matrix (not exposed to automation) | — |
| **ALG** | Built-in algorithms. 130 of them | [`OPZX7_ALG`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-alg) |
| **FB** | Feedback, **per operator**. Negative values allowed (**ours**) | [`OPZX7_FB[1-8]`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-fb-1-8) |

### WAVE SHAPE

**The heart of this channel.** Picks the waveform an operator puts out.

| Kind | What it is |
| --- | --- |
| Built-in waves | The MA-7 and OPZ waves, plus ours |
| Audio files | Use a loaded sound as the wave (recreating the OPX) |
| Wavetable memory | `.wt` / `.wt2` files (recreating the MA-7) |

For audio files you can also set **where playback starts and how fast it runs**. The loaded wave is drawn on screen.

| Knob | What it does | Automation |
| --- | --- | --- |
| **WS** | The operator's waveform. 73 of them. 31 is PCM, 15 is `.wt`, 23 is `.wt2` | [`OPZX7_OP[0-7]_WS`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ws) |
| **POFF** | Where the PCM starts (ms) | [`OPZX7_OP[0-7]_PCM_OFFSET`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-pcm-offset) |
| **PRT** | How fast the PCM plays | [`OPZX7_OP[0-7]_PCM_RATIO`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-pcm-ratio) |
| **Loop Point Enable** | Use the PCM loop | [`OPZX7_OP[0-7]_LOOPPOINT_EN`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-looppoint-en) |
| **LPST / LPED** | The looped range, as a position from 0.0 to 1.0 of the whole | [`OPZX7_OP[0-7]_LOOPPOINT_ST`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-looppoint-st) |

### AMP ENV

The level envelope on each operator. It **follows the MA-7**, and can be switched over to real numbers. **KOR / XOF / Bypass** of ours are here as well.

#### Switching Register Mode

**Register Mode** switches between the hardware's steps and setting seconds and levels directly. Either way the knobs keep the MA-7's names: **AR / DR / SL / SR / RR / TL**.

| Knob | What it does | Automation |
| --- | --- | --- |
| **Register Mode** | On for register steps, off for seconds and levels | [`OPZX7_OP[0-7]_R_EN`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-en) |

#### Register Mode = on (register steps)

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Attack rate | [`OPZX7_OP[0-7]_R_AR`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-ar) |
| **DR** | Decay rate | [`OPZX7_OP[0-7]_R_D1R`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-d1r) |
| **SL** | Sustain level | [`OPZX7_OP[0-7]_R_D1L`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-d1l) |
| **SR** | Sustain rate | [`OPZX7_OP[0-7]_R_D2R`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-d2r) |
| **RR** | Release rate | [`OPZX7_OP[0-7]_R_RR`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-rr) |
| **TL** | Total level. **Higher makes it quieter** | [`OPZX7_OP[0-7]_R_TL`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-r-tl) |

#### Register Mode = off (seconds and levels)

| Knob | What it does | Automation |
| --- | --- | --- |
| **AR** | Time to rise (seconds) | [`OPZX7_OP[0-7]_AR`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ar) |
| **DR** | Time to decay (seconds) | [`OPZX7_OP[0-7]_D[1-2]R`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-d-1-2-r) |
| **SL** | Where it settles | [`OPZX7_OP[0-7]_D1L`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-d1l) |
| **SR** | Time it keeps falling while held (seconds) | [`OPZX7_OP[0-7]_D[1-2]R`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-d-1-2-r) |
| **RR** | Time of the tail (seconds) | [`OPZX7_OP[0-7]_RR`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-rr) |
| **TL** | Output level. **Higher makes it louder** | [`OPZX7_OP[0-7]_TL`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-tl) |

#### Used in either mode

| Knob | What it does | Automation |
| --- | --- | --- |
| **SUS** | Hold the sound on after key-off | [`OPZX7_OP[0-7]_SUS`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-sus) |
| **XOF** | Ignore RR and never decay (**ours**) | [`OPZX7_OP[0-7]_XOF`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-xof) |
| **KOR** | Hold the level for a set time after release (**ours**) | [`OPZX7_OP[0-7]_KOR`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-kor) |
| **Bypass** | Do not run this operator's level envelope (**ours**) | [`OPZX7_OP[0-7]_AMP_BYPASS`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-amp-bypass) |

### MUL / DET

**MUL** offers the MA-7's choices plus an extended version of the OPZ's MulRatio, with the ceiling raised a long way.

**DT** is the OPZ's DT / DT2 with **DT3** added, where you set the value freely.

This is [MUL/DET](/2686V_2686VFX/en/chips/common/#muldet) placed on each operator.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MUL** | Multiple. 22 steps | [`OPZX7_OP[0-7]_MUL`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-mul) |
| **MURT** | Set the multiple freely (**ours**) | [`OPZX7_OP[0-7]_MUL_RATIO`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-mul-ratio) |
| **DT1 / DT2 / DT3** | Detune. DT3 takes cents (**ours**) | [`OPZX7_OP[0-7]_DT`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-dt) |

### LFO

AM and PM take **their own rate and sync**, and the range of how far they reach is wide. It can be set **both per channel and per operator**, and shapes of ours are available.

### SSG HW ENV

The OPNA's SSG hardware envelope, usable here too, with our shapes and a settable range.

This is the SSG envelope applied per operator. The one that applies to the whole channel is [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env).

| Knob | What it does | Automation |
| --- | --- | --- |
| **ENV** | Shape. Normal (off) plus eight | [`OPZX7_OP[0-7]_SE`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-se) |
| **FQ** | How fast it repeats | [`OPZX7_OP[0-7]_SEFREQ`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-sefreq) |

### KEY SCALE

You can switch between the **MA-7 / OPZ / OPS** arrangements.

#### Picking the Mode

The MODE is set per operator. **Which knobs appear depends on it.**

| Knob | What it does | Automation |
| --- | --- | --- |
| **Enable** | Use key scaling | [`OPZX7_OP[0-7]_KS_EN`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ks-en) |
| **MODE** | Which arrangement applies. MA-7 / OPZ / OPS | [`OPZX7_OP[0-7]_KS_MODE`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ks-mode) |

#### MODE = MA-7

| Knob | What it does | Automation |
| --- | --- | --- |
| **KSR** | Speed the envelope up as notes go higher | [`OPZX7_OP[0-7]_KSR`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ksr) |
| **KSL** | How much the level drops as notes go higher. OFF / 1.5 / 3.0 / 6.0 dB/oct | [`OPZX7_OP[0-7]_KSL`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ksl) |

#### MODE = OPZ

| Knob | What it does | Automation |
| --- | --- | --- |
| **KSR** | Speed the envelope up as notes go higher | [`OPZX7_OP[0-7]_KSR_OPZ`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ksr-opz) |
| **KSL** | How much the level drops as notes go higher | [`OPZX7_OP[0-7]_KSL_OPZ`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ksl-opz) |

#### MODE = OPS

The OPS arrangement alone **has a break point**. Either side of a chosen pitch, the bend can be set separately.

| Knob | What it does | Automation |
| --- | --- | --- |
| **KSBP** | The pitch used as the break point (note number) | [`OPZX7_OP[0-7]_KS_BP`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ks-bp) |
| **KSLC / KSRC** | How it bends either side of the break point | [`OPZX7_OP[0-7]_KS_LC`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ks-lc) |
| **KSLD / KSRD** | How far it drops either side of the break point | [`OPZX7_OP[0-7]_KS_LD`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ks-ld) |
| **KSRS** | How much it reaches the speed of the envelope | [`OPZX7_OP[0-7]_KS_RS`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-ks-rs) |

### MASK / PANPOT / OPTIONAL

Silencing an operator, position in the stereo field, and extra behaviour.

| Knob | What it does | Automation |
| --- | --- | --- |
| **MASK** | Do not sound this operator | [`OPZX7_OP[0-7]_MASK`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-op-0-7-mask) |
| **Enable** | Use panning | [`OPZX7_PANPOT_EN`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-panpot-en) |
| **PNPT** | Position. 0 is left, 15 centre, 31 right | [`OPZX7_PANPOT`](/2686V_2686VFX/en/reference/automation/opzx7/#opzx7-panpot) |

## Envelopes per operator

This channel can give every single operator its own:

- SSG software level envelope (6 taps / 11 taps)
- Pitch envelope (3 taps / 11 taps)

They run separately from the envelopes that apply to the whole chip, so you can **have each operator move differently and then move the whole thing again** on top.

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

## Curve editing

On 2686V and OPZX7S you can pick **how a value changes** from more than thirty curves. **In the OPZX7S plugin curve editing is standard**, and there is no way back to the old linear mode.

See the ADV tab for the details.

## MML entry

It takes the same family of symbols as OPM: `AR` `D1R` `D1L` `D2R` `RR` `TL` `MUL` `MASK` `DT1` (`DT` / `DTO`) `DT2` (`DTT`).

For how to write it, see [MML entry](/2686V_2686VFX/en/reference/mml/).

## Automation

The full list of what OPZX7S exposes to the DAW is under
[OPZX7S automation](/2686V_2686VFX/en/reference/automation/opzx7/), with IDs, types, ranges and
defaults.
