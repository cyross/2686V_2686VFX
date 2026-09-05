---
title: BEEP channel
description: The channel that sounds the machine's own beeper
sidebar:
  order: 13
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_beep.png" alt="BEEP channel" style="width:600px;" />
	<figcaption>BEEP channel</figcaption>
</figure>

BEEP sounds **the computer's own beeper**. It was the only sound a machine without a sound board could make: one pulse wave at a fixed duty cycle, and nothing else.

The tone itself cannot be changed, but **the envelopes and modulation are all here, exactly as on the other channels.** Moving a plain pulse wave in fine detail is how you retrace what the programs of the day were doing.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real machine had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## Sound settings

| Item | What it does | Range |
| --- | --- | --- |
| **FREQ** | Frequency | 0–8000Hz (440Hz by default) |
| **TIMERCLOCK** | The tick of the underlying timer | 1–4 (1 by default) |
| **ANTIALIAS** | Hold down the aliasing (**ours**) | on / off (off by default) |

**TIMERCLOCK** corresponds to how the hardware produced the beeper's frequency. The coarser the tick, the further the result drifts from the frequency you asked for. **That drift is part of what makes it sound like the period**, so try it when you want to get close to the hardware.

**Switching ANTIALIAS on rounds the corners and gives a modern, easy-listening pulse wave.** Leave it off when you are after the hardware's sound.


| Knob | What it does | Automation |
| --- | --- | --- |
| **CLK** | Which clock it is built on. The hardware's four | [`BEEP_TIMERCLOCK`](/2686V_2686VFX/en/reference/automation/beep/#beep-timerclock) |
| **Anti Alias** | Hold down the aliasing at high pitches (**ours**) | [`BEEP_ANTIALIAS`](/2686V_2686VFX/en/reference/automation/beep/#beep-antialias) |

## OPTIONAL — sound-effect mode (hardware)

Sound-effect mode has **a button that switches to 2000Hz** — the pitch the programs of the day reached for when they needed a warning tone.

For short sounds, a deep pitch envelope turns it into a laser or an explosion.

## Shared sections

The following do not belong to any one chip. They are all described under [Shared sections](/2686V_2686VFX/en/chips/common/).

The tone is simple, but **everything for moving it is here.**

| Section | What it does |
| --- | --- |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/en/chips/common/#wt-pitch-mod) | Pitch modulation from wavetable memory |
| [**WT AMP MOD**](/2686V_2686VFX/en/chips/common/#wt-amp-mod) | Level modulation from wavetable memory |
| [**LFO**](/2686V_2686VFX/en/chips/common/#lfo) | Wobbles level and pitch at a steady rate |
| [**MUL/DET**](/2686V_2686VFX/en/chips/common/#muldet) | Shifts the pitch |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

:::tip[The plainer the sound, the more there is to move]
Because BEEP starts from something so simple, **you hear exactly what an envelope does.** It is a good place to try chopping with the SSG software envelope, or dropping the pitch with a pitch envelope.

It also makes a decent practice ground for learning how the envelopes work.
:::

## Automation

The full list of what BEEP exposes to the DAW is under [BEEP automation](/2686V_2686VFX/en/reference/automation/beep/), with IDs, types, ranges and defaults.
