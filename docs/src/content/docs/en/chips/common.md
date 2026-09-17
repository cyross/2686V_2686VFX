---
title: Shared sections
description: The sections that turn up in the same shape on every sound-chip tab
sidebar:
  order: 0
---

A sound-chip tab has sections that belong to that chip alone (the algorithm, the
choice of waveform, and so on) and sections that **turn up in the same shape on
every chip**. This page covers the second kind.

Every chip page links here from its own "shared sections" list. Which of these a
chip actually has varies, so go by the table on that chip's page.

:::note[How things are marked]
| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |
:::

:::tip[About the automation column]
Shared knobs keep **the same suffix** on every chip. The links in these tables
point at the **SSG** ones as a stand-in. On another chip only the prefix
changes — `OPNA_`, `OPM_` and so on — while the range and the default stay the
same. Where a knob sits on each operator, the operator number is folded in, as
in `OPNA_OP[0-3]_`.
:::

## LEVEL

The level of the channel as a whole.

**Level -> All Ch** under UTILITY copies this value to every other channel at
once. It is there so that levels stay even while you move between chips.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **LEVEL** | Level of the whole channel | 0 – 10 | 1 | [`SSG_LEVEL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-level) |
| **DELAY** | Ours. How long after the key before it starts, in seconds | 0 – 60 | 0 | [`SSG_DELAY`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-delay) |

## QUALITY

**Ours.** Deliberately drops the bit depth and the sample rate to get the grain
of period gear. Lower numbers are coarser.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **BIT RATE** | Bit depth. 12 steps, from 4-bit (16 levels) to Raw (no drop) | 1 – 12 | 9 | [`SSG_BIT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-bit) |
| **SMP.RATE** | Sample rate. 15 steps, from 96kHz down to 2kHz | 1 – 15 | 2 | [`SSG_RATE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-rate) |

To lean towards the sound of the 1980s, bring BIT down and take RATE down with
it.

The full list of what you can pick is under
[QUALITY lists](/2686V_2686VFX/en/reference/lists-quality/).

## QUALITY(PCM)

Channels that play audio files (**RHYTHM / ADPCM**, and OPZX7's PCM) get this
instead of QUALITY. Rather than merely dropping bits, it lets you pick **the
storage scheme the hardware actually used**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **BIT RATE** | Bit depth and compression scheme. 21 kinds | 1 – 21 | 13 | [`ADPCM_MODE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-mode) |
| **SMP.RATE** | Sample rate. 15 steps | 1 – 15 | 9 | [`ADPCM_RATE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-rate) |
| **INTERP** | How the gaps are filled on the way back. 7 kinds | 0 – 6 | 1 | [`ADPCM_INTERP`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-interp) |

### What BIT offers

| Kind | What it is |
| --- | --- |
| 32-bit to 4-bit PCM | Simply drops the bit depth |
| **4-bit ADPCM** | The YM2608's ADPCM (**hardware**) |
| **1-bit DPCM** | The Famicom's DPCM |
| SNES BRR / PS1 VAG / IMA ADPCM / CD-ROM XA / YMZ280B / K053260 / K054539 | Each machine's compression scheme (**ours**) |

**A compression scheme distorts differently from simply dropping bits.** When
you are after the texture of a particular machine, pick the scheme that machine
used.

### INTERP — how the gaps are filled

Once the sample rate has been dropped, this decides how the gaps are filled on
the way back.

| Value | Method | Character |
| --- | --- | --- |
| 0 | Nearest | No interpolation. Aliasing comes through |
| 1 | Linear | The standard choice |
| 2 | Gaussian | The roundness of the SFC |
| 3 | Zero-Order Hold | The coarsest of them |
| 4 | Cosine | Between Linear and Gaussian |
| 5 | B-Spline | A strong low-pass. Muffled |
| 6 | Lagrange | Different overtones from Gaussian |

**Drop RATE and set Nearest for the most period-correct grain.** B-Spline goes
the other way and muffles things, which puts the sound further away.

## ENVELOPE

The parts that move level or pitch over time. Several kinds can run at once.

| Name | Moves | Steps | Marked |
| --- | --- | --- | --- |
| [AMP ENV](#amp-env) | level | ADSR | hardware / ours |
| [SSG HW AMP ENV](#ssg-hw-amp-env) | level | 44 shapes | hardware / ours |
| [SSG SW AMP ENV](#ssg-sw-amp-env) | level | 6 taps | ours |
| [SSG SW AMP ENV\[11\]](#ssg-sw-amp-env11) | level | 11 taps | ours |
| [PITCH ENV](#pitch-env) | pitch | 3 taps | ours |
| [SSG HW PITCH ENV](#ssg-hw-pitch-env) | pitch | 44 shapes | ours |
| [SSG SW PITCH ENV\[11\]](#ssg-sw-pitch-env11) | pitch | 11 taps | ours |

On the level side they multiply together. The usual arrangement is to shape the
whole with AMP ENV and then lay finer movement on top with SSG HW AMP ENV or
SSG SW AMP ENV.

### AMP ENV

The basic shape of the level. It runs **start level → attack → decay → sustain
→ release**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass** | **Ours.** Do not apply this envelope | False / True | False | [`SSG_ADSR_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-adsr-bypass) |
| **STL** | Level at the moment the key goes down | 0 – 1 | 0 | [`SSG_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-stl) |
| **AR** | Time to reach full level (seconds) | 0.001 – 10 | 0.001 | [`SSG_AR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ar) |
| **DR** | Time to fall to sustain (seconds) | 0.001 – 10 | 0.001 | [`SSG_DR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dr) |
| **SL** | Level held while the key is down | 0 – 1 | 1 | [`SSG_SL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-sl) |
| **RR** | Time to fade out after release (seconds) | 0.001 – 10 | 0.001 | [`SSG_RR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-rr) |
| **KOR** | **Ours.** Play the envelope out even after release | False / True | False | [`SSG_KOR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-kor) |
| **Use Endl** | Ours. Leave sound behind after the release | False / True | False | [`SSG_ENDL_EN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-endl-en) |
| **ENDL** | Ours. The level held once it is there | 0 – 1 | 0 | [`SSG_ENDL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-endl) |

FM operators carry a separate register-style envelope of their own
(AR / DR / SR / SL / RR / TL). That one is covered on each chip's page.

### SSG HW AMP ENV

The SSG's hardware envelope. It works by **repeating a fixed shape**, and on the
hardware it was how you moved a level in steps.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_SSGHWENV_ENABLE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-enable) |
| **SHAPE** | Shape of the wave. **0–15 are the hardware's, 16 up are ours** | 0 – 43 | 0 | [`SSG_SSGHWENV_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-shape) |
| **PERIOD** | How fast it repeats | 0.1 – 200 | 1 | [`SSG_SSGHWENV_PERIOD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-period) |
| **MIN** | **Ours.** Bottom of the range it moves in | 0 – 1 | 0 | [`SSG_SSGHWENV_MIN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-min) |
| **MAX** | **Ours.** Top of the range it moves in | 0 – 1 | 1 | [`SSG_SSGHWENV_MAX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-max) |
| **Smooth** | **Ours.** Round off the corners of the steps | False / True | False | [`SSG_SSGHWENV_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-smooth) |

The hardware had sixteen shapes over a fixed range. Here there are also versions
with a different number of steps, sample-and-hold, sine, random, and more.

The full list of shapes is under
[Waveform lists](/2686V_2686VFX/en/reference/lists-waveform/).

### SSG SW AMP ENV

**Ours.** It takes the level movement a driver used to produce in software and
lets you draw it directly, as **a series of line segments**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | Use it or not. On a channel it reads as bypass, on an operator as enable | False / True | True | [`SSG_SSGSW_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-bypass) |
| **STEP** | How many steps are used | 1 – 5 | 5 | [`SSG_SSGSW_STEPS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-steps) |
| **STL** | Level it starts from | 0 – 1 | 0 | [`SSG_SSGSW_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-stl) |
| **R1** | Time to reach step 1 | 0.001 – 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L1** | Level at step 1 | 0 – 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R2** | Time to reach step 2 | 0.001 – 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L2** | Level at step 2 | 0 – 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R3** | Time to reach step 3 | 0.001 – 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L3** | Level at step 3 | 0 – 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R4** | Time to reach step 4 | 0.001 – 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L4** | Level at step 4 | 0 – 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R5** | Time to reach step 5 | 0.001 – 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L5** | Level at step 5 | 0 – 1 | 1 | [`SSG_SSGSW_L6`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-l6) |
| **R6** | Time to reach step 6 | 0.001 – 10 | 0.001 | [`SSG_SSGSW_R[1-6]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-r-1-6) |
| **L6** | Level at step 6 | 0 – 1 | 0 | [`SSG_SSGSW_L6`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-l6) |
| **LOOP** | Repeat | False / True | False | [`SSG_SSGSW_LOOP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-loop) |
| **LOOP.TO** | Step it goes back to. 0–3 | 0 – 3 | 0 | [`SSG_SSGSW_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-loopto) |
| **LOOP.CNT** | How many times. **0 means forever** | 0 – 200 | 0 | [`SSG_SSGSW_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-loopcnt) |

### SSG SW AMP ENV[11]

The same thing with the steps taken up to **11 taps**. When you want finer
movement, use this one rather than the 6-tap version.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | Use it or not. On a channel it reads as bypass, on an operator as enable | False / True | True | [`SSG_SSGSW11_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-bypass) |
| **STEP** | How many steps are used | 1 – 10 | 10 | [`SSG_SSGSW11_STEPS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-steps) |
| **STL** | Level it starts from | 0 – 1 | 0 | [`SSG_SSGSW11_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-stl) |
| **R1** | Time to reach step 1 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L1** | Level at step 1 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R2** | Time to reach step 2 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L2** | Level at step 2 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R3** | Time to reach step 3 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L3** | Level at step 3 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R4** | Time to reach step 4 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L4** | Level at step 4 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R5** | Time to reach step 5 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L5** | Level at step 5 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R6** | Time to reach step 6 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L6** | Level at step 6 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R7** | Time to reach step 7 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L7** | Level at step 7 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R8** | Time to reach step 8 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L8** | Level at step 8 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R9** | Time to reach step 9 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L9** | Level at step 9 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R10** | Time to reach step 10 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L10** | Level at step 10 | 0 – 1 | 1 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **R11** | Time to reach step 11 | 0.001 – 10 | 0.001 | [`SSG_SSGSW11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-r-1-11) |
| **L11** | Level at step 11 | 0 – 1 | 0 | [`SSG_SSGSW11_L11`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-l11) |
| **LOOP** | Repeat | False / True | False | [`SSG_SSGSW11_LOOP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-loop) |
| **LOOP.TO** | Step it goes back to. 0–8 | 0 – 8 | 0 | [`SSG_SSGSW11_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-loopto) |
| **LOOP.CNT** | How many times. **0 means forever** | 0 – 200 | 0 | [`SSG_SSGSW11_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-loopcnt) |

### PITCH ENV

Moves the pitch over time. Values are in **cents**, from **−4800 to +4800**
(four octaves either way). 100 is a semitone; 1200 is an octave.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass** | Use it or not | False / True | False | [`SSG_PITCHENV_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitchenv-bypass) |
| **STL** | Pitch at the moment the key goes down | -4800 – 4800 | 0 | [`SSG_PITCH_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-stl) |
| **AR** | Time to reach ATL (seconds) | 0.001 – 10 | 0.001 | [`SSG_PITCH_AR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-ar) |
| **ATL** | Pitch at the peak | -4800 – 4800 | 0 | [`SSG_PITCH_ATL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-atl) |
| **DR** | Time to reach SSL (seconds) | 0.001 – 10 | 0.001 | [`SSG_PITCH_DR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-dr) |
| **SSL** | Pitch held while the key is down | -4800 – 4800 | 0 | [`SSG_PITCH_SSL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-ssl) |
| **RR** | Time to reach RLL (seconds) | 0.001 – 10 | 0.001 | [`SSG_PITCH_RR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-rr) |
| **RLL** | Pitch it ends on | -4800 – 4800 | 0 | [`SSG_PITCH_RLL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-rll) |
| **Use Endl** | Ours. Leave the pitch offset in place after the release | False / True | False | [`SSG_PITCH_ENDL_EN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-endl-en) |
| **ENDL** | Ours. The pitch held once it is there | -4800 – 4800 | 0 | [`SSG_PITCH_ENDL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-endl) |
| **KEEP** | Ours. Hold each stage's value instead of sloping between them | False / True | False | [`SSG_PITCH_KEEP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-pitch-keep) |

Use it to lift the pitch at the very start of a note, or to drop it away like a
laser.

### SSG HW PITCH ENV

**Ours.** It drives pitch with the **same shapes** as
[SSG HW AMP ENV](#ssg-hw-amp-env). The hardware envelope on the real chip only
touched level, but pointing the same shape at pitch gives repeating arpeggios
and trills.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_SSGHWPENV_ENABLE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-enable) |
| **SHAPE** | Shape. The same 44 as SSG HW AMP ENV | 0 – 43 | 0 | [`SSG_SSGHWPENV_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-shape) |
| **PERIOD** | How fast it repeats | 0.1 – 200 | 1 | [`SSG_SSGHWPENV_PERIOD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-period) |
| **MIN** | Bottom of the range, in cents | -4800 – 4800 | 0 | [`SSG_SSGHWPENV_MIN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-min) |
| **MAX** | Top of the range, in cents | -4800 – 4800 | 1200 | [`SSG_SSGHWPENV_MAX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-max) |
| **Smooth** | Round off the corners of the steps | False / True | False | [`SSG_SSGHWPENV_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-smooth) |

MIN and MAX are in **cents**; 1200 cents is one octave, so enabling it with the
defaults sweeps the pitch over an octave. Swapping the two numbers flips the
direction.

A step in pitch is heard as a jump, so turn Smooth on once PERIOD is high.

The shapes are listed under [Waveforms](/2686V_2686VFX/en/reference/lists-waveform/).

### SSG SW PITCH ENV[11]

The **11-tap** version on the pitch side. It is built like
[SSG SW AMP ENV\[11\]](#ssg-sw-amp-env11), except that L1–L11 hold **cents**
(±4800) rather than levels.

Because you can place a pitch on every step, you can build an **arpeggio** or a
siren inside a single note. Switch LOOP on and it repeats for as long as the
note sounds.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | Use it or not. On a channel it reads as bypass, on an operator as enable | False / True | True | [`SSG_SSGSWP11_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-bypass) |
| **STEP** | How many steps are used | 1 – 10 | 10 | [`SSG_SSGSWP11_STEPS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-steps) |
| **STL** | Pitch it starts from | -4800 – 4800 | 0 | [`SSG_SSGSWP11_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-stl) |
| **R1** | Time to reach step 1 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L1** | Pitch at step 1 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R2** | Time to reach step 2 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L2** | Pitch at step 2 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R3** | Time to reach step 3 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L3** | Pitch at step 3 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R4** | Time to reach step 4 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L4** | Pitch at step 4 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R5** | Time to reach step 5 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L5** | Pitch at step 5 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R6** | Time to reach step 6 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L6** | Pitch at step 6 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R7** | Time to reach step 7 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L7** | Pitch at step 7 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R8** | Time to reach step 8 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L8** | Pitch at step 8 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R9** | Time to reach step 9 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L9** | Pitch at step 9 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R10** | Time to reach step 10 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L10** | Pitch at step 10 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **R11** | Time to reach step 11 | 0.001 – 10 | 0.001 | [`SSG_SSGSWP11_R[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-r-1-11) |
| **L11** | Pitch at step 11 (cents) | -4800 – 4800 | 0 | [`SSG_SSGSWP11_L[1-11]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-l-1-11) |
| **LOOP** | Repeat | False / True | False | [`SSG_SSGSWP11_LOOP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-loop) |
| **LOOP.TO** | Step it goes back to. 0–8 | 0 – 8 | 0 | [`SSG_SSGSWP11_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-loopto) |
| **LOOP.CNT** | How many times. **0 means forever** | 0 – 200 | 0 | [`SSG_SSGSWP11_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-loopcnt) |

## SPEED – playback rate

Ours, new in 3.3.0. A multiplier on the rate at which a wave or a sample is
read. It changes **how fast the shape travels** while the pitch stays where it
was.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **SPEED** | Multiplier on the read rate | 0.0001 – 100 | 1 | [`SSG_SPEED`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-speed) |

## HOLD / KEEP – stopping a wave, or playing part of it

Ours, new in 3.3.0. Two things live here: **stop the wave after so many cycles**
(HOLD), and **play only a chosen stretch of one cycle** (KEEP).

They appear in the places below, and behave the same everywhere.

| Where | What it stops |
| --- | --- |
| OPTIONAL on WT / WT2 / WT+ / SSG | The sounding wave itself |
| WT PITCH MOD / WT AMP MOD | The modulation wave |
| SSG HW AMP ENV / SSG HW PITCH ENV | The envelope wave |
| LFO | The sway wave |

:::note[Not offered on shapes that already stop]
When a shape that **finishes in one cycle** is chosen – SSG HW ENV 1, 3, 5 and 7,
or LFO 6 and 7 – HOLD is greyed out, because stopping something twice does
nothing.
:::

### HOLD – stop after so many cycles

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **HOLD** | On or off | False / True | False | [`SSG_HOLD_EN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-hold-en) |
| **COUNT** | How many cycles before it stops | 1 – 3000 | 8 | [`SSG_HOLD_CNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-hold-cnt) |
| **TARGET** | Which side is held once stopped | 0 = MIN / 1 = MAX | 1 | [`SSG_HOLD_TGT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-hold-tgt) |
| **HOLD MIN** | The lower value | 0 – 1 | 0 | [`SSG_HOLD_MIN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-hold-min) |
| **HOLD MAX** | The upper value | 0 – 1 | 1 | [`SSG_HOLD_MAX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-hold-max) |

**Which side is held is a switch, not a reading of the wave.** Stopping on
whatever value happened to be passing would give a different sound on every
note.

What MIN and MAX mean depends on what they feed. Where they scale a level they
are 0.0–1.0; where they shift pitch they are −4800 to 4800 cents; on the LFO
they are −1.0 to 1.0.

### KEEP – play only part of a cycle

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **KEEP** | On or off | False / True | False | [`SSG_KEEP_EN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-keep-en) |
| **START** | Where the stretch begins | 0 – 1 | 0 | [`SSG_WAVE_ST`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-wave-st) |
| **KEEP START** | What happens before START | False / True | False | [`SSG_KEEP_ST`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-keep-st) |
| **END** | Where it ends | 0 – 1 | 1 | [`SSG_WAVE_ED`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-wave-ed) |
| **KEEP END** | What happens after END | False / True | False | [`SSG_KEEP_ED`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-keep-ed) |

START and END are positions **across one cycle, from 0.0 to 1.0**. END can only
sit after START.

KEEP START and KEEP END decide what happens outside the stretch. **On holds the
value at that edge; off gives 0.** Either way **the cycle keeps its length** –
the stretch is not cut out and looped faster.

## WT PITCH MOD

**Ours.** It wobbles the pitch using the shape held in wavetable memory. Where
an LFO wobbles with a fixed shape, here **the shape of the wave is the shape of
the wobble.**

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_MOD_ENABLE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-enable) |
| **DEPTH** | How deep the wobble goes | 0.1 – 1 | 0.2 | [`SSG_MOD_DEPTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-depth) |
| **SPEED** | How fast it wobbles | 0.1 – 10 | 1 | [`SSG_MOD_SPEED`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-speed) |
| **SHAPE** | Shape it wobbles with. 9 kinds | 0 – 8 | 0 | [`SSG_MOD_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-shape) |
| **Smooth** | Round off the corners of the steps | False / True | True | [`SSG_MOD_WAVE_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-wave-smooth) |

The nine shapes are **Sine / FDS Triangle / FDS Saw / FDS Reset / FDS Pulse /
WS Sweep Up / WS Sweep Down / HuC6280 Wave / FDS Table**. They come from the
modulation the Famicom Disk System, the WonderSwan and the PC Engine(TurboGrafx-16) chips
could apply.

You can also load a `.wt` or `.wt2` file and wobble with a shape of your own.

When SHAPE is **FDS Table**, the **FDS PITCH TABLE** below lets you draw the
32 register values directly. It is kept separately from the FDS AMP TABLE on
the level side.

## WT AMP MOD

**Ours.** It uses the **same modulation shapes** as WT PITCH MOD, but drives
level instead of pitch. The output moves **between MIN and MAX**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_AMPMOD_ENABLE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-enable) |
| **DEPTH** | How far the level is pulled down from MAX | 0 – 1 | 0.5 | [`SSG_AMPMOD_DEPTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-depth) |
| **SPEED** | How fast it wobbles | 0.1 – 10 | 1 | [`SSG_AMPMOD_SPEED`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-speed) |
| **SHAPE** | Shape it wobbles with. The same 9 as WT PITCH MOD | 0 – 8 | 0 | [`SSG_AMPMOD_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-shape) |
| **MIN** | Bottom of the range | 0 – 1 | 0 | [`SSG_AMPMOD_MIN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-min) |
| **MAX** | Top of the range | 0 – 1 | 1 | [`SSG_AMPMOD_MAX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-max) |
| **Smooth** | Round off the corners of the steps | False / True | True | [`SSG_AMPMOD_WAVE_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ampmod-wave-smooth) |

**DEPTH is how far the level is pulled down from MAX.** Left at 0 nothing
moves and the sound is unchanged; at 1 the whole MIN to MAX range is used.
This mirrors WT PITCH MOD, where a DEPTH of 0 leaves the pitch alone.

MIN and MAX are held **once for the whole modulator**, not per wave slot.
Switching slots or shapes does not change the range it swings over.

When SHAPE is **FDS Table**, the **FDS AMP TABLE** below lets you draw the
32 register values directly. It is kept separately from the FDS PITCH TABLE
on the pitch side.

## LFO

Wobbles level (AM) and pitch (PM) at a steady rate. **AM and PM are set up
separately**, and you can use just one of them.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **PM Enable** | Apply the wobble on the pitch side | False / True | False | [`SSG_LFO_PM`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pm) |
| **FREQ** | Pitch side: how fast it wobbles (Hz) | 0.1 – 50 | 5 | [`SSG_LFO_PM_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pm-freq) |
| **SHAPE** | Pitch side: shape. 13 kinds | 0 – 12 | 0 | [`SSG_LFO_PG_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pg-shape) |
| **SY.DELAY** | Pitch side: wait after key-down before it starts | 0 – 255 | 0 | [`SSG_LFO_PM_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pm-sync-delay) |
| **PMS** | How much it reaches the pitch | 0 – 1 | 0 | [`SSG_LFO_PMS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pms) |
| **PMD** | Depth on the pitch | 0 – 1 | 0 | [`SSG_LFO_PMD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pmd) |
| **AM Enable** | Apply the wobble on the level side | False / True | False | [`SSG_LFO_AM`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-am) |
| **FREQ** | Level side: how fast it wobbles (Hz) | 0.1 – 50 | 5 | [`SSG_LFO_AM_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-am-freq) |
| **SHAPE** | Level side: shape. 13 kinds | 0 – 12 | 0 | [`SSG_LFO_EG_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-eg-shape) |
| **SY.DELAY** | Level side: wait after key-down before it starts | 0 – 255 | 0 | [`SSG_LFO_AM_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-am-sync-delay) |
| **AMS** | How much it reaches the level | 0 – 1 | 0 | [`SSG_LFO_AMS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-ams) |
| **AMD** | Depth on the level | 0 – 1 | 0 | [`SSG_LFO_AMD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-amd) |
| **SM.RATIO** | Level side: rounding of the corners | 0.005 – 0.5 | 0.005 | [`SSG_LFO_ASMRT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-asmrt) |

Besides Sine / Saw Up / Saw Down / Square / Triangle / Sample & Hold there are
shapes that **move once and stop** (Saw Down & One Shot, Triangle & One Shot)
and four sample-and-hold variants with different step sizes.

Dial in SY.DELAY and the wobble holds off for a moment after the key goes down
before it starts — the way a singer's vibrato arrives.

FM chips also carry **the hardware's own LFO** (the OPNA hardware LFO, the
N88-BASIC software LFO, the OPM LFO, the OPL's per-operator LFO). Those are
covered on each chip's page.

## MUL/DET

The part that shifts the pitch.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **MUL** | Multiple, from x0.5 to x15 | 0 – 21 | 2 | [`SSG_MUL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mul) |
| **MUL.RATE** | **Ours.** Set the multiple freely | 0.01 – 100 | 1 | [`SSG_MUL_RATIO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mul-ratio) |
| **DT1** | Detune (the hardware's DT) | 0 – 15 | 0 | [`SSG_DT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dt) |
| **DT2** | Coarse detune (the hardware's DT2) | 0 – 3 | 0 | [`SSG_DT3`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dt3) |
| **DT3** | **Ours.** Set it freely in cents | -4800 – 4800 | 0 | [`SSG_DT3`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dt3) |

The hardware multiples were mostly whole numbers; **MUL.RATE** lets you set one that
is not. That is how you get bells and metal, where the overtones sit off the
whole-number ratios.

## FIX

**Ours.** Ignores the pitch of the key and sounds **a frequency you set**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_FIX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-fix) |
| **FREQ** | Frequency to sound (Hz) | 0 – 8000 | 440 | [`SSG_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-freq) |

Use it when a sound should come out at the same pitch whatever key is pressed,
the way a drum does. On FM chips you can set it per operator, so fixing just one
of them is a way to get a metallic ring.

## UNISON/HARMONY

**Ours.** Stacks copies of the same note to thicken it.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **VOICES** | How many voices are stacked | 1 – 8 | 1 | [`SSG_UNI_VOICES`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-voices) |
| **DETUNE** | Overall amount of shift (cents) | -4800 – 4800 | 0 | [`SSG_UNI_DETUNE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-detune) |
| **SPREAD** | Spread across the stereo field | 0 – 1 | 0.5 | [`SSG_UNI_SPREAD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-spread) |
| **Arpeggio** | Sound them one after another instead of stacking | False / True | False | [`SSG_UNI_ARP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-arp) |
| **AR.FREQ** | How fast the arpeggio runs | 1 – 4000 | 60 | [`SSG_UNI_ARPFREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-arpfreq) |
| **Arp Smooth** | Round off the corners as it switches | False / True | True | [`SSG_UNI_ARPSMTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-arpsmth) |
| **P-SPREAD** | Distance per voice (seven of them) | 0 – 1 | 0 | [`SSG_UNI_PDIST[1-7]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-pdist-1-7) |
| **P-DETUNE** | Shift per voice (seven of them, in cents) | -4800 – 4800 | 0 | [`SSG_UNI_PDET[1-7]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-pdet-1-7) |

From the second voice on, **the distance and the amount of shift are set per
voice** (seven of them). Beyond spreading them evenly you can weight them to one
side, stack them in octaves, and so on.

Switch **Arpeggio** on and, instead of stacking, the voices sound one after
another — the way hardware with few voices used to imply a chord. Put the
per-voice shifts on semitones and it works as a broken chord straight away.

## OPTIONAL

Extra behaviour a chip happened to have. What is in it depends on the chip:
sound-effect mode (giving each operator its own pitch), how noise is handled,
what happens at key-off, and so on.

What appears is covered on each chip's page.

## UTILITY

Reading and writing parameters. `[IM]` imports and `[EX]` exports.

Each section can be moved in and out as its own file, so you can carry just the
AMP ENV over to another channel, or reuse only the LFO settings. A whole channel
can also go into a single file.

**Level -> All Ch** copies this channel's level to every other channel at once.

For the formats and where they live, see
[File formats and locations](/2686V_2686VFX/en/files/format/). Parameter files **can be read
across chips** — an AMP ENV built on OPN can be loaded on SSG.

### Passing sounds between the FM chips and OPZX7S

From 3.4.0, **a sound built on an FM chip can be carried over as an OPZX7S
sound.** There are two buttons, and both convert in exactly the same way.

| Button | Where | What it does |
| --- | --- | --- |
| **[EX]OPZX7S Params** | UTILITY on OPNA / OPN / OPL / OPL3 / OPM | writes the current channel out as an OPZX7S CH Params file |
| **[IM]FM Params** | UTILITY on OPZX7S | picks an OPNA / OPN / OPL / OPL3 / OPM CH Params file, converts it to OPZX7S values and loads it |

[EX]OPZX7S Params is in 2686V, 2686VLight, 26V, 86V, OPNV, OPLV and OPMV;
[IM]FM Params is in 2686V, 2686VLight and OPZX7S. **The OPZX7S plugin has no FM
chips of its own, but it can read files saved by the other plugins.**

After [IM]FM Params loads a file, a dialog says so. Closing it opens the save
picker, so **the result can be kept straight away as an OPZX7S CH Params file.**
The name comes from the original file (`piano.param.opm.json` becomes
`piano.param.opzx7s.json`). Cancel if you do not want to keep it.

#### How it converts

| On the FM chip | On OPZX7S |
| --- | --- |
| Algorithm | the same routing is copied into the **algorithm matrix** |
| FB | goes to the FB of the operator that receives the feedback, at the value that gives the same depth |
| AR / DR (D1R) / SR (D2R) / RR | converted to seconds in **real mode** (Register Mode off), using the hardware rate table |
| SL (D1L) / TL | converted to real-mode levels (SL in 3 dB steps, TL in 0.75 dB steps) |
| EG TYPE off (OPL / OPL3) | a decaying sound: SR is set so that, after reaching SL, it keeps falling at the RR rate |
| SUS (OPL) | the release falls at rate 5, so that rate goes into RR |
| MUL / MUL.RATIO | to the MUL with the same ratio; OPM's MUL.RATIO is used as it is |
| DT1 / DT2 | DT1 goes to DT3 as the same amount in cents; DT2 goes to DT2 |
| KS / KSR / KSL | OPN and OPM go to the OPZ key scale, OPL and OPL3 to the MA-7 one (the steps are the same) |
| Waveform (OPL / OPL3) | to the WAVE SHAPE of the same name; the OPN family and OPM use Sine |
| SSG-EG (OPNA) | copied as it is |
| OPNA hardware LFO | to the operator LFO, as a triangle with the same rate and depth |
| N88 LFO (OPNA / OPN) | PM to the channel-wide LFO; AM to the operator LFO, at the depth set by N88 AMS |
| OPM LFO | PM to the channel-wide LFO; AM to the LFO of the operators with AMS-EN on |
| AM / VIB (OPL / OPL3) | to the operator LFO |
| PAN (OPNA / OPM) | left or right goes to the end of the OPZX7S pan; centre switches pan off |

**Sections built from the same parts on every chip** — LEVEL, AMP ENV, SSG HW
ENV, UNISON, QUALITY, the operators' PITCH ENV and so on — are copied as they
are.

Operators the original chip does not have (3 to 8 for OPL, 5 to 8 otherwise)
are **left unconnected** and given OPZX7S's defaults. They make no sound.

#### What gets approximated

Anything that cannot behave the same way is moved to the nearest value. **If
anything was approximated, a dialog lists it at the end.**

- DT1 does not fall on OPZX7S's steps, so it goes into DT3 rounded to the cent
- The real-mode key scale (KSR) shortens times differently from the register one
- Any stage longer than 10 seconds becomes 10 seconds; an SR (D2R) longer than 10 seconds becomes "no decay"
- An operator with AR at 0 (never rises) gets the longest AR, 10 seconds
- FB too shallow for OPZX7S becomes the shallowest value it can express
- A pan hard to one side goes to the end of the OPZX7S pan, where the other side is still faintly heard
- LFO rate and depth are kept within OPZX7S's ranges
- N88 LFO PMD / AMD below zero (a reversed swing) is turned the right way round
- An operator using AM from both the HW LFO and the N88 LFO keeps only the HW LFO's AM

:::note
- [IM]FM Params cannot read files in the **line-ordered format** from before 3.0.0
- When loading, the PITCH ENV and similar sections of operators the original chip does not have keep the values they had before. They are not connected, so they make no sound
:::

## Automation

Almost every knob on this page can be driven from DAW automation. Names, types,
ranges and defaults are under
[Automation reference](/2686V_2686VFX/en/reference/automation/).

Curve editing (ADV) is the one exception: there are far too many slots, so it is
kept out of automation. See [Curve editing](/2686V_2686VFX/en/chips/adv/).
