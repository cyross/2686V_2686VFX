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
| **LV** | Level of the whole channel | 0 – 10 | 1 | [`SSG_LEVEL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-level) |

## QUALITY

**Ours.** Deliberately drops the bit depth and the sample rate to get the grain
of period gear. Lower numbers are coarser.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **BIT** | Bit depth. 12 steps, from 4-bit (16 levels) to Raw (no drop) | 1 – 12 | 9 | [`SSG_BIT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-bit) |
| **RATE** | Sample rate. 15 steps, from 96kHz down to 2kHz | 1 – 15 | 2 | [`SSG_RATE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-rate) |

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
| **BIT** | Bit depth and compression scheme. 21 kinds | 1 – 21 | 13 | [`ADPCM_MODE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-mode) |
| **RATE** | Sample rate. 15 steps | 1 – 15 | 9 | [`ADPCM_RATE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-rate) |
| **INTP** | How the gaps are filled on the way back. 7 kinds | 0 – 6 | 1 | [`ADPCM_INTERP`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-interp) |

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

### INTP — how the gaps are filled

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

FM operators carry a separate register-style envelope of their own
(AR / DR / SR / SL / RR / TL). That one is covered on each chip's page.

### SSG HW AMP ENV

The SSG's hardware envelope. It works by **repeating a fixed shape**, and on the
hardware it was how you moved a level in steps.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_SSGHWENV_ENABLE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-enable) |
| **SHPE** | Shape of the wave. **0–15 are the hardware's, 16 up are ours** | 0 – 43 | 0 | [`SSG_SSGHWENV_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-shape) |
| **PERD** | How fast it repeats | 0.1 – 200 | 1 | [`SSG_SSGHWENV_PERIOD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-period) |
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
| **LOOP TO** | Step it goes back to. 0–3 | 0 – 3 | 0 | [`SSG_SSGSW_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-loopto) |
| **LOOP COUNT** | How many times. **0 means forever** | 0 – 200 | 0 | [`SSG_SSGSW_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw-loopcnt) |

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
| **LOOP TO** | Step it goes back to. 0–8 | 0 – 8 | 0 | [`SSG_SSGSW11_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-loopto) |
| **LOOP COUNT** | How many times. **0 means forever** | 0 – 200 | 0 | [`SSG_SSGSW11_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw11-loopcnt) |

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

Use it to lift the pitch at the very start of a note, or to drop it away like a
laser.

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
| **LOOP TO** | Step it goes back to. 0–8 | 0 – 8 | 0 | [`SSG_SSGSWP11_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-loopto) |
| **LOOP COUNT** | How many times. **0 means forever** | 0 – 200 | 0 | [`SSG_SSGSWP11_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp11-loopcnt) |

## WT PITCH MOD

**Ours.** It wobbles the pitch using the shape held in wavetable memory. Where
an LFO wobbles with a fixed shape, here **the shape of the wave is the shape of
the wobble.**

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_MOD_ENABLE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-enable) |
| **DPTH** | How deep the wobble goes | 0.1 – 1 | 0.2 | [`SSG_MOD_DEPTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-depth) |
| **SPED** | How fast it wobbles | 0.1 – 10 | 1 | [`SSG_MOD_SPEED`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-speed) |
| **SHPE** | Shape it wobbles with. 9 kinds | 0 – 8 | 0 | [`SSG_MOD_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-shape) |
| **Smooth** | Round off the corners of the steps | False / True | True | [`SSG_MOD_WAVE_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mod-wave-smooth) |

The nine shapes are **Sine / FDS Triangle / FDS Saw / FDS Reset / FDS Pulse /
WS Sweep Up / WS Sweep Down / HuC6280 Wave / FDS Table**. They come from the
modulation the Famicom Disk System, the WonderSwan and the PC Engine chips
could apply.

You can also load a `.wt` or `.wt2` file and wobble with a shape of your own.

## LFO

Wobbles level (AM) and pitch (PM) at a steady rate. **AM and PM are set up
separately**, and you can use just one of them.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **PM Enable** | Apply the wobble on the pitch side | False / True | False | [`SSG_LFO_PM`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pm) |
| **P.SP** | Pitch side: how fast it wobbles (Hz) | 0.1 – 50 | 5 | [`SSG_LFO_PM_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pm-freq) |
| **P.SH** | Pitch side: shape. 13 kinds | 0 – 12 | 0 | [`SSG_LFO_PG_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pg-shape) |
| **SDLY** | Pitch side: wait after key-down before it starts | 0 – 255 | 0 | [`SSG_LFO_PM_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pm-sync-delay) |
| **PMS** | How much it reaches the pitch | 0 – 1 | 0 | [`SSG_LFO_PMS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pms) |
| **PMD** | Depth on the pitch | 0 – 1 | 0 | [`SSG_LFO_PMD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-pmd) |
| **AM Enable** | Apply the wobble on the level side | False / True | False | [`SSG_LFO_AM`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-am) |
| **A.SP** | Level side: how fast it wobbles (Hz) | 0.1 – 50 | 5 | [`SSG_LFO_AM_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-am-freq) |
| **A.SH** | Level side: shape. 13 kinds | 0 – 12 | 0 | [`SSG_LFO_EG_SHAPE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-eg-shape) |
| **SDLY** | Level side: wait after key-down before it starts | 0 – 255 | 0 | [`SSG_LFO_AM_SYNC_DELAY`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-am-sync-delay) |
| **AMS** | How much it reaches the level | 0 – 1 | 0 | [`SSG_LFO_AMS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-ams) |
| **AMD** | Depth on the level | 0 – 1 | 0 | [`SSG_LFO_AMD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-amd) |
| **AMSR** | Level side: rounding of the corners | 0.005 – 0.5 | 0.005 | [`SSG_LFO_ASMRT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-lfo-asmrt) |

Besides Sine / Saw Up / Saw Down / Square / Triangle / Sample & Hold there are
shapes that **move once and stop** (Saw Down & One Shot, Triangle & One Shot)
and four sample-and-hold variants with different step sizes.

Dial in SDLY and the wobble holds off for a moment after the key goes down
before it starts — the way a singer's vibrato arrives.

FM chips also carry **the hardware's own LFO** (the OPNA hardware LFO, the
N88-BASIC software LFO, the OPM LFO, the OPL's per-operator LFO). Those are
covered on each chip's page.

## MUL/DET

The part that shifts the pitch.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **MUL** | Multiple, from x0.5 to x15 | 0 – 21 | 2 | [`SSG_MUL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mul) |
| **MURT** | **Ours.** Set the multiple freely | 0.01 – 100 | 1 | [`SSG_MUL_RATIO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-mul-ratio) |
| **DT1** | Detune (the hardware's DT) | 0 – 15 | 0 | [`SSG_DT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dt) |
| **DT2** | Coarse detune (the hardware's DT2) | 0 – 3 | 0 | [`SSG_DT3`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dt3) |
| **DT3** | **Ours.** Set it freely in cents | -4800 – 4800 | 0 | [`SSG_DT3`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-dt3) |

The hardware multiples were mostly whole numbers; **MURT** lets you set one that
is not. That is how you get bells and metal, where the overtones sit off the
whole-number ratios.

## FIX

**Ours.** Ignores the pitch of the key and sounds **a frequency you set**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Use it or not | False / True | False | [`SSG_FIX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-fix) |
| **FQ** | Frequency to sound (Hz) | 0 – 8000 | 440 | [`SSG_FREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-freq) |

Use it when a sound should come out at the same pitch whatever key is pressed,
the way a drum does. On FM chips you can set it per operator, so fixing just one
of them is a way to get a metallic ring.

## UNISON/HARMONY

**Ours.** Stacks copies of the same note to thicken it.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **VOIC** | How many voices are stacked | 1 – 8 | 1 | [`SSG_UNI_VOICES`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-voices) |
| **DT** | Overall amount of shift (cents) | -4800 – 4800 | 0 | [`SSG_UNI_DETUNE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-detune) |
| **SPR** | Spread across the stereo field | 0 – 1 | 0.5 | [`SSG_UNI_SPREAD`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-spread) |
| **Arpeggio** | Sound them one after another instead of stacking | False / True | False | [`SSG_UNI_ARP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-arp) |
| **ARFQ** | How fast the arpeggio runs | 1 – 4000 | 60 | [`SSG_UNI_ARPFREQ`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-arpfreq) |
| **Arp Smooth** | Round off the corners as it switches | False / True | True | [`SSG_UNI_ARPSMTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-arpsmth) |
| **P-SP** | Distance per voice (seven of them) | 0 – 1 | 0 | [`SSG_UNI_PDIST[1-7]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-pdist-1-7) |
| **P-DT** | Shift per voice (seven of them, in cents) | -4800 – 4800 | 0 | [`SSG_UNI_PDET[1-7]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-pdet-1-7) |

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

## Automation

Almost every knob on this page can be driven from DAW automation. Names, types,
ranges and defaults are under
[Automation reference](/2686V_2686VFX/en/reference/automation/).

Curve editing (ADV) is the one exception: there are far too many slots, so it is
kept out of automation. See [Curve editing](/2686V_2686VFX/en/chips/adv/).
