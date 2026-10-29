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
| **FC Mode** | Behave the way the FC did (**ours**) | on / off (off by default) | |
| **FC FLUC** | How much it fluctuates in FC Mode (**ours**) | 0.0–1.0 (0.2 by default) | |
| **More Real FC** | Sound like the FC's (2A03) pulse channel (**ours**) | on / off (off by default) | 4.0.0 |
| **More Real NES** | Sound like the NES's (2A03) pulse channel (**ours**) | on / off (off by default) | 4.0.0 |
| **More Real SSG** | Sound like the SSG's (YM2203 / YM2608B) pulse channel (**ours**) | on / off (off by default) | 4.3.0 |

**Only one of FC Mode, More Real FC, More Real NES and More Real SSG can be on** (all off is fine).
Turning one on turns the other three off.

More Real FC / More Real NES have nothing to do with FC FLUC. They mimic the 2A03 like this:

- Only the chip's four duty cycles, **12.5% / 25% / 50% / 75%**. The one closest to the ratio from MODE, PRESET, RATIO and Invert Phrase is used, played with the chip's 8-step sequence
- Pitch snaps to the 11-bit timer (fpulse = 1789772.7 / (16 × (t + 1))). Notes high enough to need t below 8 are silent
- Volume is rounded to 16 steps and sent through the chip's non-linear DAC (95.88 / (8128 / v + 100))
- The output swings between 0 and the volume, and the filters after it remove the DC. **This is where FC and NES differ**
  - More Real FC: a 37 Hz high-pass
  - More Real NES: 90 Hz and 440 Hz high-passes and a 14 kHz low-pass (thinner bass, a droop on long pulses)

More Real SSG mimics how the SSG's output is built (4.3.0):

- The duty cycle is **fixed at 50%**, as on the chip. While it is on, MODE, PRESET, RATIO, Invert Phrase and
  DUTY RATIO ENVELOPE are dimmed and have no effect
- The output only swings upwards from 0 and goes through the board's DC cut (a first-order high-pass at about
  30 Hz) to come back around 0. The flat tops of the pulse lean a little (sag), and raising the volume suddenly
  makes the DC jump with a "click"
- Volume goes through the SSG's logarithmic table (the same one as CHIP DAC), whatever CHIP DAC in QUALITY says
- **Tone and noise are combined with a logical AND.** MIX at 0 is tone only, at 1 noise only, and at 0.5 the AND
  of both (noise only gets through while the pulse is high — a gritty, metallic sound). In between, the nearer
  one is blended with the AND

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
| **FC Mode** | Make the ratio waver slightly, the way the FC's did (**ours**) | [`SSG_DUTY_FC_MODE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-fc-mode) |
| **FC FLUC** | How large that wavering is | [`SSG_DUTY_FC_FLUC`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-fc-fluc) |
| **More Real FC** | Sound like the FC's pulse channel (**ours**) | [`SSG_DUTY_MORE_FC`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-more-fc) |
| **More Real NES** | Sound like the NES's pulse channel (**ours**) | [`SSG_DUTY_MORE_NES`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-more-nes) |
| **More Real SSG** | Sound like the SSG's pulse channel (**ours**, 4.3.0) | [`SSG_DUTY_MORE_SSG`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-duty-more-ssg) |

#### DUTY RATIO ENVELOPE (ours, 4.0.0)

A 256-step envelope at the bottom of the group that moves the duty cycle over time.
It works like [SSG SW AMP ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env256),
except that the value is a **duty cycle (%)** instead of a level.

- Switch it on with **Enable** at the top. While it is on, its value replaces MODE, PRESET,
  RATIO and Invert Phrase (Invert Phrase is not applied)
- Values run **0.0–100.0%**. STL is called **STDR**, the L values **DR1–DR256**, the target knob
  **DR.TARGET** and the value knob **DUTY**
- The buttons under the value knob set the duty cycles PRESET offers
  (50 / 43.75 / 37.5 / 31.25 / 25 / 20 / 18.75 / 12.5 / 6.25%) directly
- RATE, STEP, KEEP, LOOP, RR / LR and Use Endl / ENDL work as in SSG SW AMP ENV[256]
- With More Real FC / NES, this value is also snapped to the nearest of 12.5 / 25 / 50 / 75%
- **STEPS** under the value knob (4.3.0) snaps the values (STL, the step values, LR, ENDL) to **10% / 25% / 20% / 12.5% / 6.25%**
  (1/10, 1/4, 1/5, 1/8, 1/16). The choice is only on screen and isn't saved
- The per-step R / L values are saved as a table in presets and channel parameters (they cannot be
  automated). The section's parameter file is `.dutyEnv`, read and written from the heading's right-click menu

The group grows with the number of steps, so its contents scroll vertically.

### Triangle settings (ours)

These appear when FORM is set to `1: Triangle / Saw`. **PEAK** moves where the peak of the wave sits, sweeping continuously from triangle through to saw.

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **PEAK** | Where the peak sits. `0.0` is a falling saw, `0.5` a triangle, `1.0` a rising saw | 0.0–1.0 (0.5 by default) | |
| **Key Track (Pitch)** | Follow the keyboard. Switched off, it stays at the FQ frequency | on / off (on by default) | |
| **FREQ** | The frequency used when Key Track is off | 0–8000 Hz (440 by default) | |
| **More Real FC** | Sound like the FC's (2A03) triangle channel (**ours**) | on / off (off by default) | 4.3.0 |

Below PEAK there are `0.0 (Down)`, `0.5 (Tri)` and `1.0 (Up)` buttons that jump straight to those values.

More Real FC mimics the 2A03 triangle like this (4.3.0):

- The wave is a **4-bit (0–15) × 32-step staircase**: 15, 14 … 1, 0, 0, 1 … 14, 15, with the top and bottom held
  for two steps each. The steps give the chip's buzzy overtones. PEAK and PEAK RATIO ENVELOPE are dimmed and have no effect
- Pitch snaps to the 11-bit timer (ftri = 1789772.7 / (32 × (t + 1)) — an octave below the pulse for the same timer).
  Notes high enough to need t below 2 are silent
- The step values go through the non-linear DAC shared by triangle and noise (159.79 / (8227 / T + 100))
- The output is unipolar, and the FC's output filter (a 37 Hz high-pass) removes the DC
- The real triangle has no volume register, but here AMP ENV and the other volume controls still apply


| Knob | What it does | Automation |
| --- | --- | --- |
| **Key Track (Pitch)** | Sound the pitch of the key. Switched off, it stays at FQ | [`SSG_TRI_KEYTRK`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-keytrk) |
| **FREQ** | The frequency it is fixed at | [`SSG_TRI_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-freq) |
| **PEAK** | Where the peak of the triangle sits. 0.5 is symmetrical | [`SSG_TRI_PEAK`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-peak) |
| **More Real FC** | Sound like the FC's triangle channel (**ours**, 4.3.0) | [`SSG_TRI_MORE_FC`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tri-more-fc) |

#### PEAK RATIO ENVELOPE (ours, 4.0.0)

A 256-step envelope at the bottom of the group that moves the triangle's peak over time.
It works like DUTY RATIO ENVELOPE.

- Switch it on with **Enable** at the top. While it is on, its value replaces PEAK
- Values run **0.0–1.0**. STL is called **STPK**, the L values **PK1–PK256**, the target knob
  **PK.TARGET** and the value knob **PEAK**
- The buttons under the value knob (−0.01 / −0.1 / 0.0 / 1.0 / +0.1 / +0.01) move the value
- **STEPS** under the value knob (4.3.0) snaps the values to **0.1 / 0.25 / 0.2 / 0.125 / 0.0625**
  (1/10, 1/4, 1/5, 1/8, 1/16). The choice is only on screen and isn't saved
- The section's parameter file is `.peakEnv`

### NOISE (ours, 4.3.0)

The lower half of the pulse / triangle group is the **NOISE** group (it shows whatever FORM is set to).

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **More Real FC** | Sound like the FC's (2A03) noise channel (**ours**) | on / off (off by default) | 4.3.0 |

More Real FC mimics the 2A03 noise like this:

- A **15-bit LFSR** makes the noise (bit 0 XOR bit 1 fed back into bit 14 — the long mode, 32,767 steps).
  As on the early FC (the plain RP2A03), there is no short mode (the 93-step metallic sound)
- The LFSR rate snaps to the closest entry of the chip's **16-step table** (every 4 / 8 / 16 / 32 / 64 / 96 / 128 /
  160 / 202 / 254 / 380 / 508 / 762 / 1016 / 2034 / 4068 CPU cycles) to the rate set by N.FREQ (or N.VALUE, or key tracking)
- The output is unipolar — it sounds while bit 0 is 0 — and the FC's output filter (a 37 Hz high-pass) removes the DC

| Knob | What it does | Automation |
| --- | --- | --- |
| **More Real FC** | Sound like the FC's noise channel (**ours**) | [`SSG_NOISE_MORE_FC`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-more-fc) |

## TONE / NOISE / MIX — blending the sound (hardware)

| Item | What it does | Range | Notes |
| --- | --- | --- | --- |
| **T.LEVEL** | Level of the wave (tone) | 0.0–1.0 (1.0 by default) | |
| **N.LEVEL** | Level of the noise | 0.0–1.0 (1.0 by default) | |
| **N.FREQ** | Frequency of the noise | 0–30000 Hz (12000 by default) | |
| **TONE: Use VALUE** | Multiply the played pitch by a ratio set with T.VALUE | On / off (off by default) | |
| **T.VALUE** | The tone value To | 0–4095 (47 by default) | ratio 47 ÷ To |
| **NOISE: Use VALUE** | Use N.VALUE instead of N.FREQ | On / off (off by default) | |
| **N.VALUE** | The noise value Tn | 0–31 (2 by default) | 667000 ÷ (32 × Tn) Hz |
| **MIX** | Blend of tone and noise | 0.0–1.0 (0.0 by default) | |
| **Noise On Note** | Restart the noise on every note | on / off (off by default) | |


| Knob | What it does | Automation |
| --- | --- | --- |
| **T.LEVEL** | How much of the tone side | [`SSG_TONE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tone) |
| **N.LEVEL** | How much of the grit side | [`SSG_NOISE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise) |
| **N.FREQ** | How high the grit sits | [`SSG_NOISEFREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noisefreq) |
| **TONE: Use VALUE** | Multiply the played pitch by a ratio set with T.VALUE | [`SSG_TONE_VALUE_ON`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tone-value-on) |
| **T.VALUE** | The chip's tone value To (0–4095). The ratio is 47 ÷ To (1× at the default 47) | [`SSG_TONE_VALUE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-tone-value) |
| **NOISE: Use VALUE** | Set the grit's pitch with N.VALUE instead of N.FREQ | [`SSG_NOISE_VALUE_ON`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-value-on) |
| **N.VALUE** | The chip's noise value Tn (0–31): 667000 ÷ (32 × Tn) Hz | [`SSG_NOISE_VALUE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-value) |
| **Noise On Note** | Track the grit to the keyboard (**ours**) | [`SSG_NOISE_ON_NOTE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noise-on-note) |
| **MIX** | Blend of the two. 0 is tone only, 1 is grit only | [`SSG_NOISEMIX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-noisemix) |

### Setting them with the chip's values — VALUE

Since 3.6.3, TONE / NOISE can also be set with the YM2203's own values. All of
these start off, and while they are off nothing changes.

- **TONE: Use VALUE**: multiplies the played frequency by a ratio set with
  **T.VALUE** (the tone value To, 0–4095). The ratio is Fnote(To) ÷ Fnote(47) =
  47 ÷ To, where Fnote = 667 × 1000 ÷ (32 × To): 1× at the default 47, an octave
  down at 94, and lower as To grows. With it off, the ratio is 1
- **NOISE: Use VALUE**: sets the grit's pitch with **N.VALUE** (the noise value
  Tn, 0–31) instead of N.FREQ, Fnois = 667 × 1000 ÷ (32 × Tn). N.FREQ is
  disabled meanwhile

The **N** buttons to the right of T.VALUE / N.VALUE show value buttons, as on
LEVEL. T.VALUE jumps to one or two octaves up (12 / 24), a fifth up (31), a
fourth up (35), a fifth down (70) and one or two octaves down (94 / 188);
N.VALUE to 1 / 4 / 8 / 12 / 16 / 31.

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
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | ours | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env256) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env256) |
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
