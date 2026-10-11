---
title: SSG channel
description: How to use the SSG channel — pulse, triangle and noise
sidebar:
  order: 7
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_ssg.png" alt="SSG channel" style="width:600px;" />
	<figcaption>SSG channel</figcaption>
</figure>

SSG is the channel that stands for the SSG part of the YM-2203 / YM-2608B — which traces back to the AY-3-8910 family, the chips usually called PSG. It sounds pulse waves and noise, and moves the level with a hardware envelope. Beyond the hardware we have added triangle and saw waves, plus a few things for shaping the sound more finely.

## How things are marked

Throughout this manual each section on screen carries one of these marks. **They match the colours of the headings in the plugin.**

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |
| **shared** | A feature of the plugin as a whole, not of any one chip |

When you are after the hardware's sound, leaving the **ours** entries at their defaults will get you closer.

## FORM — choosing the waveform (hardware)

Picks which kind of wave is sounded.

| Value | Waveform |
| --- | --- |
| `0: Pulse(Rect)` | Pulse. As the hardware had it |
| `1: Triangle / Saw` | Triangle and saw (**ours**) |

What appears below changes with the waveform you pick.


| Knob | What it does | Automation |
| --- | --- | --- |
| **FORM** | Pulse or triangle | [`SSG_WAVEFORM`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-waveform) |

### Pulse (duty cycle) settings

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **MODE** | How the duty cycle is chosen. `0: Preset Ratio` picks from the eight the hardware had; `1: Variable (Slider)` moves continuously (**ours**) | — | |
| **PRESET** | The eight duty cycles the hardware had | see below | |
| **RATIO** | The ratio when MODE is Variable | 0.0–1.0 | |
| **FC Mode** | Behave the way the Famicom did (**ours**) | on / off (off by default) | |
| **FC FLUC** | How much it fluctuates in FC Mode (**ours**) | 0.0–1.0 (0.2 by default) | |

PRESET offers these eight duty cycles.

| Value | Ratio | Duty |
| --- | --- | --- |
| 0 | 1:1 | 50% |
| 1 | 3:5 | 37.5% |
| 2 | 5:11 | 31.25% |
| 3 | 1:3 | 25% |
| 4 | 1:4 | 20% |
| 5 | 3:13 | 18.75% |
| 6 | 1:7 | 12.5% |
| 7 | 1:15 | 6.25% |


| Knob | What it does | Automation |
| --- | --- | --- |
| **MODE** | Pick from fixed ratios, or set one freely | [`SSG_DUTY_MODE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-mode) |
| **PRESET** | The ratios the hardware used. 9 of them | [`SSG_DUTY_PRESET`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-preset) |
| **RATIO** | The ratio when you set it freely | [`SSG_DUTY_VAR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-var) |
| **Invert Phrase** | Turn the wave upside down | [`SSG_DUTY_INV`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-inv) |
| **FC Mode** | Make the ratio waver slightly, the way the Famicom's did (**ours**) | [`SSG_DUTY_FC_MODE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-fc-mode) |
| **FC FLUC** | How large that wavering is | [`SSG_DUTY_FC_FLUC`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-fc-fluc) |

### Triangle settings (ours)

These appear when FORM is set to `1: Triangle / Saw`. **PEAK** moves where the peak of the wave sits, sweeping continuously from triangle through to saw.

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **PEAK** | Where the peak sits. `0.0` is a falling saw, `0.5` a triangle, `1.0` a rising saw | 0.0–1.0 (0.5 by default) | |
| **Key Track (Pitch)** | Follow the keyboard. Switched off, it stays at the FQ frequency | on / off (on by default) | |
| **FREQ** | The frequency used when Key Track is off | 0–8000 Hz (440 by default) | |

Below PEAK there are `0.0 (Down)`, `0.5 (Tri)` and `1.0 (Up)` buttons that jump straight to those values.


| Knob | What it does | Automation |
| --- | --- | --- |
| **Key Track (Pitch)** | Sound the pitch of the key. Switched off, it stays at FQ | [`SSG_TRI_KEYTRK`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-keytrk) |
| **FREQ** | The frequency it is fixed at | [`SSG_TRI_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-freq) |
| **PEAK** | Where the peak of the triangle sits. 0.5 is symmetrical | [`SSG_TRI_PEAK`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-peak) |

## TONE / NOISE / MIX — blending the sound (hardware)

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **T.LEVEL** | Level of the wave (tone) | 0.0–1.0 (1.0 by default) | |
| **N.LEVEL** | Level of the noise | 0.0–1.0 (1.0 by default) | |
| **N.FREQ** | Frequency of the noise | 0–30000 Hz (12000 by default) | |
| **TONE: Use VALUE** | Snap the pitch to the chip's tone value To | On / off (off by default) | |
| **NOISE: Use VALUE** | Use N.VALUE instead of N.FREQ | On / off (off by default) | |
| **N.VALUE** | The noise value Tn | 0–31 (2 by default) | 667000 ÷ (32 × Tn) Hz |
| **MIX** | Blend of tone and noise | 0.0–1.0 (0.0 by default) | |
| **Noise On Note** | Restart the noise on every note | on / off (off by default) | |


| Knob | What it does | Automation |
| --- | --- | --- |
| **T.LEVEL** | How much of the tone side | [`SSG_TONE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tone) |
| **N.LEVEL** | How much of the grit side | [`SSG_NOISE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise) |
| **N.FREQ** | How high the grit sits | [`SSG_NOISEFREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noisefreq) |
| **TONE: Use VALUE** | Snap the pitch to the integer grid of the chip's tone value To | [`SSG_TONE_VALUE_ON`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tone-value-on) |
| **NOISE: Use VALUE** | Set the grit's pitch with N.VALUE instead of N.FREQ | [`SSG_NOISE_VALUE_ON`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-value-on) |
| **N.VALUE** | The chip's noise value Tn (0–31): 667000 ÷ (32 × Tn) Hz | [`SSG_NOISE_VALUE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-value) |
| **Noise On Note** | Track the grit to the keyboard (**ours**) | [`SSG_NOISE_ON_NOTE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-on-note) |
| **MIX** | Blend of the two. 0 is tone only, 1 is grit only | [`SSG_NOISEMIX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noisemix) |

### Setting them with the chip's values — VALUE

Since 3.6.3, TONE / NOISE can also be set with the YM2203's own values. All of
these start off, and while they are off nothing changes.

- **TONE: Use VALUE**: snaps the pitch to the integer grid of the tone value To
  (1–4095), Fnote = 667 × 1000 ÷ (32 × To). As on the chip, the higher the note,
  the coarser the tuning
- **NOISE: Use VALUE**: sets the grit's pitch with **N.VALUE** (the noise value
  Tn, 0–31) instead of N.FREQ, Fnois = 667 × 1000 ÷ (32 × Tn). N.FREQ is
  disabled meanwhile

0 counts as 1, as on the chip. FORM on RHYTHM, ADPCM and ADPCM+ works the same way.

## HW ENV — the hardware envelope (hardware)

The mechanism the real SSG had for moving the level on its own. Switch **Enable** on to use it.

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **Enable** | Use the hardware envelope | on / off (off by default) | |
| **SHAPE** | Which shape | 0–43 | |
| **PERIOD** | How long one cycle takes | 0.1–200.0 (1.0 by default) | |
| **Use VALUE** | Use VALUE instead of PERIOD | On / off (off by default) | |
| **VALUE** | The envelope value Te | 0–65535 (1303 by default) | 667000 ÷ (512 × Te) Hz |

:::note
The hardware had eight shapes; here you can pick from **43**. Everything past the eighth is ours. When you are after the hardware's sound, stay within the first eight.
:::

## The other sections

The following do not belong to any one chip. They are all described under [Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | Marked | What it does |
| --- | --- | --- |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | ours | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**LFO**](/2686V_2686VFX/en/chips/common/#lfo) | ours | Wobbles level and pitch at a steady rate |
| [**MUL/DET**](/2686V_2686VFX/en/chips/common/#muldet) | hardware / ours | Shifts the pitch |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | ours | Stacks copies of the note to thicken it |
| [**QUALITY**](/2686V_2686VFX/en/chips/common/#quality) | — | Drops bit depth and sample rate for texture |
| [**EDIT**](/2686V_2686VFX/en/chips/common/#edit) | shared | Cut, copy, paste and clear, and setting levels across channels |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | shared | Reading and writing files |

For the details of QUALITY, see [the reference](/2686V_2686VFX/en/reference/lists-quality/).

## UTILITY — reading and writing files (shared)

`[IM]` imports and `[EX]` exports. What can be moved in and out: LFO, AMP ENV, PITCH ENV, SSG SW ENV, DETUNE, UNISON, QUALITY, TONE/NOISE, and the channel as a whole.

**Level -> All Ch** (copies this channel's level to every other channel at once) moved to [EDIT](/2686V_2686VFX/en/chips/common/#edit) in 3.6.3.

For the formats and where they live, see [File formats and locations](/2686V_2686VFX/en/files/format/).

## Automation

The full list of what SSG exposes to the DAW is under [SSG automation](/2686V_2686VFX/en/reference/automation/ssg/), with IDs, types, ranges and defaults.
