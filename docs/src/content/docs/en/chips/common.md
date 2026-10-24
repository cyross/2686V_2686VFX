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

### DAC — switch everything to match a machine

QUALITY and QUALITY(PCM) start with a **DAC** row (since 3.6.3). Pick a machine and
press **Apply** to set BIT RATE, SMP.RATE and INTERP to match that machine's output in
one go. The machine itself is not saved, only the values it set. The settings for
each machine are listed in [What changed in v3.6.3](/2686V_2686VFX/en/guide/changes-363/#quality-gains-a-dac-row).

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
| **NR: Resample** | Uses a clean decimation when converting to a compression scheme | on / off | off | [`ADPCM_NR_RESAMPLE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-nr-resample) |
| **NR: Gate** | Removes faint noise and DC offset in near-silent passages | on / off | off | [`ADPCM_NR_GATE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-nr-gate) |
| **GATE.LV** | The level (dB) where the gate closes | -96 – -24 | -60 | [`ADPCM_NR_GATE_LV`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-nr-gate-lv) |
| **NR.LPF** | Cuts the high end to tame the grit. Off / Light / Medium / Strong | 1 – 4 | 1 (Off) | [`ADPCM_NR_LPF`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-nr-lpf) |

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

### Noise reduction — NR

An **original** addition (new in 3.6.0). It keeps the period texture and only
tames the noise that grates. **All of it starts switched off**, so leaving it
alone keeps the sound as it was.

| Knob | What it does |
| --- | --- |
| **NR: Resample** | When converting to a compression scheme (BIT 13-21), decimates the source with a clean **windowed-sinc** filter. The old decimation was crude and capped the signal-to-distortion ratio at about 28 dB; with this on it reaches about 90 dB. Only available while a compression scheme is selected |
| **NR: Gate** | Closes when the sound falls below **GATE.LV**, silencing faint noise. It opens over 1 ms and closes over 30 ms, so attacks and tails are not clipped. It also removes DC offset (a shifted center) |
| **GATE.LV** | The level at which the gate closes. Only available while NR: Gate is on |
| **NR.LPF** | Trims the top of the range the source actually contains. Light, Medium and Strong cut from 90%, 70% and 50% of that upper edge |

Plain bit reduction (BIT 1-12) produces exactly the noise theory predicts. When
the grain bothers you it is usually a compression scheme, so try **NR: Resample**
first.

## ENVELOPE

The parts that move level or pitch over time. Several kinds can run at once.

| Name | Moves | Steps | Marked |
| --- | --- | --- | --- |
| [AMP ENV](#amp-env) | level | ADSR | hardware / ours |
| [SSG HW AMP ENV](#ssg-hw-amp-env) | level | 44 shapes | hardware / ours |
| [SSG SW AMP ENV\[256\]](#ssg-sw-amp-env256) | level | up to 256 steps | ours |
| [PITCH ENV](#pitch-env) | pitch | 3 taps | ours |
| [SSG HW PITCH ENV](#ssg-hw-pitch-env) | pitch | 44 shapes | ours |
| [SSG SW PITCH ENV\[256\]](#ssg-sw-pitch-env256) | pitch | up to 256 steps | ours |

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
| **STEPS** | Snaps SL to a fixed step (screen only, not saved) | Free / 1/2–1/256, 1/3, 1/6, 1/7, 1/10, 1/100 | Free | — |
| **RR** | Time to fade out after release (seconds) | 0.001 – 10 | 0.001 | [`SSG_RR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-rr) |
| **KOR** | **Ours.** Play the envelope out even after release | False / True | False | [`SSG_KOR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-kor) |
| **Use Endl** | Ours. Leave sound behind after the release | False / True | False | [`SSG_ENDL_EN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-endl-en) |
| **ENDL** | Ours. The level held once it is there | 0 – 1 | 0 | [`SSG_ENDL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-endl) |

**STEPS** works like the one on LEVEL: it snaps the value to the step you pick
(from 3.6.3). As the range is 0–1, there is no "by 1" step. SSG HW AMP ENV,
SSG SW AMP ENV, SSG SW AMP ENV[256], WT PITCH MOD and WT AMP MOD have the same
STEPS, on every channel, operator and pad.

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
| **Use VALUE** | Set the speed with VALUE instead of PERIOD | False / True | False | [`SSG_SSGHWENV_VALUE_ON`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-value-on) |
| **VALUE** | The chip's envelope value Te: 667000 ÷ (512 × Te) Hz | 0 – 65535 | 1303 | [`SSG_SSGHWENV_VALUE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-value) |
| **MIN** | **Ours.** Bottom of the range it moves in | 0 – 1 | 0 | [`SSG_SSGHWENV_MIN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-min) |
| **MAX** | **Ours.** Top of the range it moves in | 0 – 1 | 1 | [`SSG_SSGHWENV_MAX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-max) |
| **STEPS** | Snaps MIN / MAX to a fixed step (screen only, not saved) | Free / 1/2–1/256, 1/3, 1/6, 1/7, 1/10, 1/100 | Free | — |
| **Smooth** | **Ours.** Round off the corners of the steps | False / True | False | [`SSG_SSGHWENV_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwenv-smooth) |

The hardware had sixteen shapes over a fixed range. Here there are also versions
with a different number of steps, sample-and-hold, sine, random, and more.

The full list of shapes is under
[Waveform lists](/2686V_2686VFX/en/reference/lists-waveform/).

#### Setting it with the chip's value — VALUE

PERIOD sets the speed in Hz. Turn on **Use VALUE** to set it with the YM2203's
own value **Te** instead (since 3.6.3). The speed is

> Fenv = 667 × 1000 ÷ (512 × Te)

A larger Te is slower. The default 1303 is the closest to PERIOD's default
(1 Hz). 0 counts as 1, as on the chip. SSG HW PITCH ENV works the same way.

The **N** button to the right of VALUE shows value buttons, as on LEVEL: ±1,
±10, ±100 and ±1000, plus the values for 0.25 / 0.5 / 1 / 2 / 4 / 8 Hz
(5211 / 2606 / 1303 / 651 / 326 / 163).

FORM's TONE / NOISE have the same kind of switch (see
[SSG](/2686V_2686VFX/en/chips/ssg/)).

### SSG SW AMP ENV[256]

A section of **our own**. It draws a volume contour of up to **256 steps**, the
kind of change drivers used to make in software. In 4.0.0 it replaced SSG SW AMP
ENV[11] (10 steps plus release) and the 6-step SSG SW AMP ENV from 3.6.3 and earlier.

A key press starts at **STL**, moves to **L1** over **R1** seconds, to **L2** over
**R2** seconds and so on up to **STEP** steps, then holds the last level until
release (sustain; the last step's value is held even with KEEP on). On release it heads from the current level to **LR** over **RR** seconds.

| Control | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | Use it or not. "Bypass" on a channel, "Enable" on an operator | False / True | True | [`SSG_SSGSW256_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-bypass) |
| **STEP** | Number of steps | 1 – 256 | 10 | [`SSG_SSGSW256_STEPS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-steps) |
| **KEEP** | Hold each step's starting value instead of sloping between steps | False / True | False | [`SSG_SSGSW256_KEEP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-keep) |
| **LOOP** | Repeat | False / True | False | [`SSG_SSGSW256_LOOP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-loop) |
| **LOOP MODE** | How to repeat: Original Mode / Rich Mode ([below](#loop-mode--rich-mode)) | 0 – 1 | 0 (Original) | [`SSG_SSGSW256_LOOPMODE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-loopmode) |
| **LOOP.TO** | Original Mode. Step to return to | 0 – 254 | 0 | [`SSG_SSGSW256_LOOPTO`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-loopto) |
| **LOOP.CNT** | Original Mode. How many times. **0 repeats forever** | 0 – 200 | 0 | [`SSG_SSGSW256_LOOPCNT`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-loopcnt) |
| **LP.TARGET** / **START** / **END** / **COUNT** | Rich Mode. The loop set you edit, and its START, END and COUNT | 1 – number of sets / 1 – STEP / 1 – STEP / 0 – 200 | — | — (kept in a table) |
| **ADD** / **DELETE** | Rich Mode. Add or remove a loop set (up to 256) | — | — | — |
| **R.TARGET** / **RATE** | The step whose R you edit, and its time (seconds) | 1 – STEP / 0.001 – 10 | — / 0.001 | — (kept in a table) |
| **L.TARGET** / **LEVEL** | The step whose L you edit (0 is STL), and its volume | 0 – STEP / 0 – 1 | — / 1 | STL only: [`SSG_SSGSW256_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-stl) |
| **STEPS** | Snap LEVEL, LR and ENDL to fixed steps (screen only, not saved) | free / 1/2–1/256, 1/3, 1/6, 1/7, 1/10, 1/100 | free | — |
| **RR** | Time from release to reaching LR (seconds) | 0.001 – 10 | 0.001 | [`SSG_SSGSW256_RR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-rr) |
| **LR** | Volume to head for after release | 0 – 1 | 0 | [`SSG_SSGSW256_LR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-lr) |
| **Use Endl** | After the release, hold ENDL instead of LR | False / True | False | [`SSG_SSGSW256_ENDL_EN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-endl-en) |
| **ENDL** | The volume to hold then | 0 – 1 | 0 | [`SSG_SSGSW256_ENDL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgsw256-endl) |

With 256 × 2 values per envelope, the per-step R / L **are saved as a table in
presets and channel parameters, not as parameters**, so they can't be automated
(the head values such as STL, RR and LR are parameters).

- There is one pair of controls, a selector (R.TARGET / L.TARGET) plus a value;
  the other steps are listed in the strip below. **The strip lists only STEP
  steps**, so adding steps makes the section taller
- **N** next to RATE / LEVEL shows nudge buttons
- Right-clicking the heading copies the table too (CUT / COPY / PASTE / CLEAR /
  DUPLICATE)

#### LOOP MODE – Rich Mode

**LOOP MODE**, under LOOP, picks how the envelope repeats (from 4.1.0). The default is
**Original Mode**: as before, the last step goes back to **LOOP.TO**, **LOOP.CNT** times.

**Rich Mode** replaces LOOP.TO / LOOP.CNT with up to **256 sets of START, END and COUNT**.
In either mode, nothing repeats while LOOP is off.

- After the **END** step finishes, playback goes back to the **START** step, **COUNT** times
  (**0 is endless**), so START to END plays COUNT + 1 times (counted the same way as LOOP.CNT).
  On the way back it moves from L(END) to L(START) over R(START) seconds
- The sets are always sorted by START, then END. Pick one with **LP.TARGET** and edit it with
  START / END / COUNT. If moving START or END reorders the sets, LP.TARGET keeps pointing at the
  set you moved
- **ADD** adds a set that goes back over 1 to STEP once; **DELETE** removes the selected set
- The sets are listed below in **No / START / END / COUNT** columns, and the section grows taller
  as sets are added. Clicking a row also picks that set. Sets that reach past STEP have no effect,
  so they are dimmed
- The sets are saved with the step table. Only LOOP MODE can be automated

With several sets, they take effect **from the one whose START comes first** (with the same START,
the one whose END comes first).

- **Nested**: the inner loop finishes first, then the outer one goes back, and the inner one starts
  counting again each time. For example, 1–4 once and 2–3 once plays "1 2 3 2 3 4 1 2 3 2 3 4"
- **Crossing**: when a set starts inside an earlier loop and ends after its END, the earlier loop
  finishes first, then playback moves on to the START of the next loop. For example, 1–3 once and
  2–5 once plays "1 2 3 1 2 3 2 3 4 5 2 3 4 5", and 1–3 does not repeat again

:::caution[Do not make an inner or crossed loop endless]
If an inner loop, or a loop that is crossed, has COUNT 0 (endless), playback never gets to the outer
or crossing loop. It just plays that way and does not hang, but the list shows an orange **! INF**
in its COUNT.
:::

#### From [11] (3.6.3 and earlier)

Presets, DAW state, channel parameters and section files (`.ssgSwEnv11`) are
converted to [256] when loaded: R1–R10 / L1–L10 become steps 1–10 of the table, and
R11 / L11 (the release) become RR / LR, so they sound the same. Exports are written
as `.ssgSwEnv256`.

### PARAM ENV[256]

**Ours** (from 4.3.0). The same 256-step envelope as [SSG SW AMP ENV\[256\]](#ssg-sw-amp-env256),
driving the parameter chosen with **TARGET** step by step.

| Where | What TARGET lists |
| --- | --- |
| Channel (the **[[PARAM ENV]]** major section, above [[TAP SEQUENCER]]) | The channel's parameters. On RHYTHM, the pads' parameters too (PAD1 SPEED and so on) |
| FM operators (directly below the AMP ENV section, in the same column) | The operator's parameters. The default is **TL** |

TARGET lists the same parameters as [TAP PARAM SEQUENCER](#tap-param-sequencer).

- Switch it on with **Enable** at the top, and choose the parameter with **TARGET** below it (TARGET can be automated)
- Values are kept as **a fraction of the target parameter's range** and shown the way the target shows them
  (0–127 for TL, Hz for FIX FREQ). Changing TARGET keeps each value as "where in the range"
- STL is called **STL**, the L values **L1–L256**, the target knob **L.TARGET** and the value knob **VALUE**.
  The buttons under the value knob are **MIN** / **-10%** / **-1** / **+1** / **+10%** / **MAX** (±1 is one step of the target)
- Integer and choice parameters (AR, MUL, ALG, waveform numbers and so on) hold their value for each step, whatever
  KEEP says. Real-valued parameters (FIX FREQ, LFO FREQ and so on) glide between steps when KEEP is off
- Only **that note** is changed; the parameter itself keeps its value. Values are read again while the note plays,
  so parameters that only act at key-on (MUL, AR and so on) also change with each step
- Key-on behaviour, RATE, STEP, LOOP (including LOOP MODE), RR / LR and Use Endl / ENDL work as in SSG SW AMP ENV[256]
- The section parameter file is `.paramEnv`, read and written from the heading's right-click menu. The target is written by
  name as well, so a file from another channel lands on the same target when the name matches

#### From TL ENV in 4.2.x and earlier

The operators' **TL ENV** of 4.0.0–4.2.x is now the PARAM ENV target **TL**. Presets, DAW state, channel parameters
(`tlEnv`) and section parameter files (`.tlEnv`) are converted to PARAM ENV (TARGET set to TL) when loaded. The values
are divided by the TL maximum (127 on OPNA / OPN / OPM, 63 on OPL / OPL3 / OPZX7S), so they sound the same. The parameter
IDs changed from `_TLENV_*` to `_PRMENV_*`; if you automated TL ENV in your DAW, assign the automation again.

### ALG ENV

**Ours.** Only the FM chips' channels have it (OPNA / OPN / OPL / OPL3 / OPM / OPZX7S), below
FEEDBACK in ALGORITHM/FEEDBACK (from 4.0.0). It switches the **ALG (algorithm)** step by step, so
the way the operators are connected changes over time.

It is the same 256-step envelope as [SSG SW AMP ENV\[256\]](#ssg-sw-amp-env256), except that the
values are **ALG numbers** instead of levels.

- Switch it on with **Enable** at the top. While it is on, the step's ALG is used instead of ALG
- Values are ALG numbers counted from 0 (0–7 on OPNA / OPN / OPM / OPL3, 0–1 on OPL, up to the
  number of ALGs on OPZX7S). STL is called **STALG**, the L values **ALG1–ALG256**, the target knob
  **ALG.TARGET** and the value knob **ALG**
- A value is held for its step (always KEEP, so there is no KEEP switch). There are no value buttons
  and no STEPS
- RATE, STEP, LOOP (including LOOP MODE), RR / LR and Use Endl / ENDL work as in SSG SW AMP ENV[256]
- Which operator gets FEEDBACK is decided by the ALG at the start of each block
- The section's parameter file is `.algEnv`, read and written from the heading's right-click menu

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
| **Use VALUE** | Set the speed with VALUE instead of PERIOD | False / True | False | [`SSG_SSGHWPENV_VALUE_ON`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-value-on) |
| **VALUE** | The chip's envelope value Te: 667000 ÷ (512 × Te) Hz | 0 – 65535 | 1303 | [`SSG_SSGHWPENV_VALUE`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-value) |
| **MIN** | Bottom of the range, in cents | -4800 – 4800 | 0 | [`SSG_SSGHWPENV_MIN`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-min) |
| **MAX** | Top of the range, in cents | -4800 – 4800 | 1200 | [`SSG_SSGHWPENV_MAX`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-max) |
| **Smooth** | Round off the corners of the steps | False / True | False | [`SSG_SSGHWPENV_SMOOTH`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssghwpenv-smooth) |

MIN and MAX are in **cents**; 1200 cents is one octave, so enabling it with the
defaults sweeps the pitch over an octave. Swapping the two numbers flips the
direction.

A step in pitch is heard as a jump, so turn Smooth on once PERIOD is high.

The shapes are listed under [Waveforms](/2686V_2686VFX/en/reference/lists-waveform/).

### SSG SW PITCH ENV[256]

The **256-step** pitch version. It works like [SSG SW AMP ENV\[256\]](#ssg-sw-amp-env256),
with STL, L, LR and ENDL in **cents** (±4800) instead of volume. In 4.0.0 it
replaced SSG SW PITCH ENV[11] from 3.6.3 and earlier.

Placing a pitch per step makes **arpeggios** and siren-like moves within one note,
and LOOP repeats them while the note sounds. After the release the pitch returns to
normal (with Use Endl it holds ENDL).

| Control | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass / Enable** | Use it or not | False / True | True | [`SSG_SSGSWP256_BYPASS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-bypass) |
| **STEP** | Number of steps | 1 – 256 | 10 | [`SSG_SSGSWP256_STEPS`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-steps) |
| **KEEP** | Hold each step's starting value | False / True | False | [`SSG_SSGSWP256_KEEP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-keep) |
| **LOOP** / **LOOP MODE** / **LOOP.TO** / **LOOP.CNT** | Repeat. LOOP MODE ([Rich Mode](#loop-mode--rich-mode)) works as on AMP | — | — | [`SSG_SSGSWP256_LOOP`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-loop) etc. |
| **LP.TARGET** / **START** / **END** / **COUNT** / **ADD** / **DELETE** | The Rich Mode loop sets | — | — | — (kept in a table) |
| **R.TARGET** / **RATE** | The step whose R you edit, and its time (seconds) | 1 – STEP / 0.001 – 10 | — / 0.001 | — (kept in a table) |
| **L.TARGET** / **LEVEL** | The step whose L you edit (0 is STL), and its pitch (cents) | 0 – STEP / -4800 – 4800 | — / 0 | STL only: [`SSG_SSGSWP256_STL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-stl) |
| **RR** / **LR** | Time from release to LR, and the pitch to head for | 0.001 – 10 / -4800 – 4800 | 0.001 / 0 | [`SSG_SSGSWP256_RR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-rr) / [`SSG_SSGSWP256_LR`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-lr) |
| **Use Endl** / **ENDL** | Keep a pitch after the release | — | — | [`SSG_SSGSWP256_ENDL`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-ssgswp256-endl) |

**N** next to LEVEL shows pitch buttons (semitone, octave and so on). [11] data from
3.6.3 and earlier is converted on load, as with AMP (`.ssgSwPEnv11` files load too).

## TAP SEQUENCER

**Ours**: the **[[TAP SEQUENCER]]** major section (from 4.0.0), in every channel's global section
below [[PITCH ENV]] (below [[PARAM ENV]] from 4.3.0) (below UNISON/HARMONY on RHYTHM). **Each key press moves on one step**, and that
step's value corrects the level or the pitch. It captures the driver trick of nudging level or pitch
note by note as a row of steps.

| Section | Step value | Range | Default | Applied as |
| --- | --- | --- | ---: | --- |
| **TAP AMP SEQUENCER** | ratio | 0.0 – 4.0 | 1.0 | multiplied into the final level |
| **TAP PITCH SEQUENCER** | cents | −2400 – 2400 | 0 | added to the final pitch |

| Knob | What it does | Range | Default |
| --- | --- | --- | ---: |
| **Enable** | Use it | on / off | off |
| **STEPS** | How many steps. With 1, every note gets the same correction | 1 – 256 | 1 |
| **LOOP** | Go back along the loop sets (from 4.1.0) | on / off | off |
| **LP.TARGET** / **START** / **END** / **COUNT** | The loop set you edit, and its START, END and COUNT (0 is endless) | 1 – number of sets / 1 – STEPS / 1 – STEPS / 0 – 200 | — |
| **ADD** / **DELETE** | Add or remove a loop set (up to 256) | — | — |
| **S.TARGET** | The step the value knob edits | 1 – STEPS | 1 |
| **RATIO / CENT** | That step's value; the buttons below nudge it | see above | see above |
| **SKIP** | Make the S.TARGET step a SKIP step (ON; the button turns red), or clear it (OFF) (from 4.1.0). A SKIP step changes nothing | OFF / ON | OFF |
| **INCREMENT** | Move the current step on by one (after the last comes 1; from not started, 1). With LOOP on, it follows the loop sets like a key press | — | — |
| **DECREMENT** | Move the current step back by one (before 1 comes the last; from not started, the last) | — | — |
| **CLEAR STEP** | Go back to not started (the next press uses step 1), and restart the loop counts. RESET in 4.0.0 | — | — |

- The step values are drawn as horizontal bars; the row with a different background is the **current step** (the one the last key press used)
- Before the first press, and after the DAW stops playing, it is **not started** and no row is highlighted. The next press uses step 1.
  4.0.0 showed the "next step" instead, so it pointed at step 1 before any press, and at the step after the one that sounded
- A **SKIP** step (from 4.1.0) still advances, but changes neither level nor pitch (level x1, pitch 0 cents).
  Its bar is full-width red (orange while S.TARGET points at it), and the list shows "SKIP".
  Press SKIP again, or move that step's RATIO / CENT, to clear it
- Below that, a list "S1: value / S2: value / …" shows four per row. Changing STEPS resizes it at once
- There is one current step per channel, so every voice moves along the same row
- The step advances by **exactly one per key press**. Unison does not advance it once per voice, and every voice plays the
  same step's value. It also advances on mono legato, and when a key is pressed again right after release. Releasing a key in mono mode and going back to a key
  still held does not advance it (fixed in 4.1.0)
- INCREMENT / DECREMENT / CLEAR STEP can also be pressed by automation (they act once when the value rises past 0.5).
  CLEAR STEP keeps the ID of 4.0.0's RESET (`_RESET`)
- Step values and SKIP are saved as a table in presets and channel parameters (they cannot be automated).
  The section parameter files are `.tapAmp` / `.tapPitch`, read and written from the heading's right-click menu
- With **LOOP** on (from 4.1.0), the step that advances on each key press goes back along the loop sets.
  The rules for the sets (START, END, COUNT, nesting, crossing and the endless mark) are the same as
  [SSG SW AMP ENV\[256\] Rich Mode](#loop-mode--rich-mode). After the last step is used it returns to
  step 1 and the loops start counting again. Like the current step, the loop progress is one per channel

### TAP PARAM SEQUENCER

The third section of [[TAP SEQUENCER]] (from 4.3.0). Each key press moves on one step, and the **(parameter, value)
pairs** stored in that step replace parameters **for that note**. As an extension of our own, one step holds
**up to 16 pairs**. Steps advance exactly as in TAP AMP / PITCH SEQUENCER (current step, not started, LOOP, SKIP,
INCREMENT / DECREMENT / CLEAR STEP, one step per key press).

| Knob | What it does |
| --- | --- |
| **S.TARGET** | The step you edit |
| **SKIP** | Replace nothing on the S.TARGET step (its pairs are kept) |
| **P.TARGET** | The pair you edit (1 – the number of pairs in the step) |
| **OP** | **CH** (channel) or operator **OP1** onwards (pad **PAD1** onwards on RHYTHM). FM chips and RHYTHM only |
| **PARAM** | The parameter to replace, from the place chosen with OP |
| **VALUE** | The value to use, shown the way the target shows it. **MIN** / **-1** / **+1** / **MAX** move it |
| **ADD PAIR** / **DELETE PAIR** / **CLEAR PAIRS** | Add the current OP / PARAM / VALUE pair at the end of the step, delete the P.TARGET pair, delete every pair of the step |

- On a step without pairs, choose OP / PARAM / VALUE and press ADD PAIR to add that pair. On a step with pairs,
  moving OP / PARAM / VALUE changes the P.TARGET pair (and clears that step's SKIP)
- The list below shows one line per step, such as "S1: OP1 AR 31, OP2 TL 20". The row with a different background is
  the current step; the row with a different text colour is the S.TARGET step
- Like PARAM ENV, values are kept as a fraction of the target's range. When a PARAM ENV drives the same parameter,
  the PARAM ENV wins
- The pairs are saved in presets, DAW state and channel parameters (`tapParam`); they cannot be automated.
  The section parameter file is `.tapParam`

Parameters you can choose (PARAM ENV's TARGET lists the same):

| Chip | Channel | Operator / pad |
| --- | --- | --- |
| OPNA / OPN | ALG / FB / N88 LFO FREQ | TL / AR / DR / SR / SL / RR / KS / MUL / DT / FIX FREQ / N88 LFO AMS (OPNA also SSG-EG / SSG-EG FREQ / HW LFO FREQ / PMS / AMS) |
| OPM | ALG / FB / HW LFO FREQ | TL / AR / D1R / D1L / D2R / RR / KS MODE / KS / KSR / KSL / MUL / MUL RATIO / DT1 / DT2 / FIX FREQ / SSG-EG FREQ |
| OPL / OPL3 | ALG / FB | TL / AR / DR / SL / RR / KSL / MUL / EG / LFO AM and PM SPEED and DEPTH |
| OPZX7S | ALG / FB OP1–8 / LFO PM FREQ / LFO AM FREQ | Register and real-valued ((F)) TL / AR / D1R / D1L / D2R / RR, the KS items, MUL / MUL RATIO / DT1–3 / WS / FIX FREQ / SSG-EG / SSG-EG FREQ / PCM SPEED, LFO PM FREQ, AM FREQ, PMS, AMS, PMD, AMD |
| SSG | DUTY PRESET / DUTY / TRI PEAK / TRI FREQ / NOISE FREQ / TONE VALUE / NOISE VALUE / SPEED | — |
| WT / WT2 | WAVE / SPEED | — |
| WT+ | WAVE (wave memory slot) | — |
| ADPCM | SPEED / NOISE FREQ | — |
| ADPCM+ | VOICE (PCM slot) | — |
| BEEP | TIMER CLOCK | — |
| RHYTHM | — | SPEED / NOISE FREQ / FIX FREQ / LFO PM FREQ / LFO AM FREQ |

Every channel (and every FM operator) also lists the WT PITCH MOD / WT AMP MOD SPEED and waveform (WAVE), and the
SSG HW AMP ENV / SSG HW PITCH ENV PERIOD. Channels other than FM add FIX FREQ and LFO PM FREQ / AM FREQ, and every
channel except RHYTHM adds ARP FREQ (the UNISON/HARMONY arpeggio). On/off toggles are not offered.

Changing the ADPCM+ VOICE can play slots other than the selected one. So from 4.3.0, every loaded slot is encoded
again when QUALITY changes.

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
| **STEPS** | Snaps DEPTH to a fixed step (screen only, not saved); steps below 0.1 go to 0.1 | Free / 1/2–1/256, 1/3, 1/6, 1/7, 1/10, 1/100 | Free | — |
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
| **STEPS** | Snaps DEPTH to a fixed step (screen only, not saved) | Free / 1/2–1/256, 1/3, 1/6, 1/7, 1/10, 1/100 | Free | — |
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

MUL/DET and FIX set the pitch from **their values when the key is pressed**.
Moving them while a note plays takes effect from the next note. Turn on
"Apply MUL/DET and FIX while notes play" in SETTINGS to have them act on the
sounding note straight away (off by default), the way writing the register on
the real chip does.

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
| **P-SPREAD** | Distance per voice (eight, including the original) | 0 – 1 | 0 | [`SSG_UNI_PDIST[0-7]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-pdist-0-7) |
| **P-DETUNE** | Shift per voice (eight, including the original, in cents) | -4800 – 4800 | 0 | [`SSG_UNI_PDET[0-7]`](/2686V_2686VFX/en/reference/automation/ssg/#ssg-uni-pdet-0-7) |

**The distance and the amount of shift are set per voice.** Pick the voice with
VOICES: 0 is the original, 1 to 7 are the stacked voices (0 arrived in 3.6.3). Beyond spreading them evenly you can weight them to one
side, stack them in octaves, and so on.

Switch **Arpeggio** on and, instead of stacking, the voices sound one after
another — the way hardware with few voices used to imply a chord. Put the
per-voice shifts on semitones and it works as a broken chord straight away.

## OPTIONAL

Extra behaviour a chip happened to have. What is in it depends on the chip:
sound-effect mode (giving each operator its own pitch), how noise is handled,
what happens at key-off, and so on.

What appears is covered on each chip's page.

## EDIT

Cut, copy, paste and clear parameters (since 3.6.3). The EDIT section for the
whole channel sits below MIDI.

| Button | What it does |
| --- | --- |
| **CUT** | Copies, then resets to the defaults |
| **COPY** | Copies |
| **PASTE** | Pastes what you copied |
| **CLEAR** | Resets to the defaults |

- These four work on the channel's **global part** (everything except the
  operators and pads). WT+ / ADPCM+ targets (slots) are included
- Pasting fills **only parameters with the same name**. You can paste into a
  channel of a different chip; anything it lacks is skipped
- Wave memory, audio files and modulation waves (WT PITCH MOD / WT AMP MOD) are
  copied too, and reloaded when you paste
- What you copy stays until the DAW closes, and can be pasted into another
  window of the same plug-in
- While pasting, clearing or duplicating, **Duplicating…** covers the screen
  until it finishes

**Level -> All Ch** and **Duplicate Op Params**, which copies one operator at a
time (Copy Op Params up to 3.6.2), have moved here from UTILITY, below the
separator.

### EDIT for an operator / pad

OPNA, OPN, OPL, OPL3, OPM and OPZX7 have a second EDIT below MASK; RHYTHM has
it below OPTIONAL. It works on **the operator / pad selected by TARGET**, and
can paste into another operator or pad, or into an operator of another channel.

### Duplicate Pad Params — copying a pad onto several pads (RHYTHM)

At the top of the global EDIT (below MIDI) on RHYTHM.

- Pick the source pad with **FROM** and any number of destinations with the
  numbered **TO** toggles (the source's own number cannot be picked)
- Every parameter of the pad is copied
- With **WITH FILE** on, the audio (wave) and its file name are copied too;
  with it off, the destinations keep their audio

This replaces the Copy Pad Params (FROM / TO sliders) that sat in UTILITY up to
3.6.2.

### With Note (RHYTHM)

A toggle in RHYTHM's global EDIT, below CUT / COPY / PASTE / CLEAR. **While it is
off, PASTE / CUT / CLEAR leave each pad's NOTE alone** (it starts off), because
changing the NOTE would change which key plays which pad.

It applies to the global EDIT, the pad EDIT, right-clicking a section (OPTIONAL
and so on) and Duplicate Pad Params alike. Turn it on only when you want the
NOTE copied or reset too.

### Right-click a section

Right-click a section heading for a CUT / COPY / PASTE / CLEAR menu that works
on **that section only**. It works on closed sections too. Paste into the same
section of another channel, operator or pad. For FX effects, right-click the
frame's heading.

Since 4.0.0 these items follow CLEAR:

| Item | What it does |
| --- | --- |
| **Duplicate** | Copies an operator, pad or TARGET section (OPTIONAL on WT+ / ADPCM+, PAN on ADPCM+ and so on) into the same section of other operators, pads or TARGETs on the same channel. A dialog with check boxes picks the destinations. Not available for global sections |
| **Import** | Same as pressing that section's **[IM]…** in UTILITY |
| **Export** | Same as pressing that section's **[EX]…** in UTILITY |

The Duplicate dialog has "Check all" / "Uncheck all" and "Duplicate" / "Cancel"
buttons. The source (what TARGET points at now) can't be picked. ESC also closes
it. Import and Export work only for sections that have [IM] / [EX] in UTILITY; those
UTILITY buttons are still there.

### Duplicate Ops Params — copying a run of operators

At the top of EDIT on OPNA, OPN, OPL3, OPM and OPZX7. Copies a run of
consecutive operators onto another run. Pick them with **FROM** and **TO**.

**SWAP** exchanges FROM and TO. Below it, **SHIFT** / **UNSHIFT** come in a FROM
row and a TO row, and move that run by one operator.

- **SHIFT** counts up (1, 2, 3, 4, 1, 2, …), **UNSHIFT** counts down
  (4, 3, 2, 1, 4, 3, …)
- A run that would run off the end wraps to the other end: with 8 operators,
  SHIFT turns [6,7,8] into [1,2,3], and UNSHIFT turns [1,2,3] into [6,7,8]
- If the shifted run is not one you can pick, it moves on to the next one that
  is ([1,2] → [3,4] on 4-operator chips, [1,2,3,4] → [5,6,7,8] on OPZX7)

| Chip | Runs you can pick |
| --- | --- |
| OPNA / OPN / OPL3 / OPM | [1,2] and [3,4] |
| OPZX7 | two: [1,2] – [7,8]; three: [1,2,3] – [6,7,8]; four: [1,2,3,4] and [5,6,7,8] |

- Either direction works ([1,2]→[3,4] as well as [3,4]→[1,2]). The order inside
  a run is kept ([1,2] cannot become [2,1])
- Combinations where source and destination overlap are disabled
- On OPZX7 the feedback is copied too. With the algorithm matrix in use, the
  connections inside the run are copied as well. If either run is connected to
  an operator outside the two, a warning appears and nothing is copied

### Duplicate Targets / Clear Targets (WT+ / ADPCM+)

At the top of EDIT. Each opens a dialog, placed so that the WAVE MEMORY / FORM
frames stay in view while you choose.

- **Duplicate Targets**: pick the source target with **FROM** and any number of
  destinations with **TO**. With **Copy parameters too** on, every parameter of
  the source is copied; with it off, only the wave (audio) and its file name
- **Clear Targets**: returns the targets picked under **TARGET** to their initial
  state, wave and parameters alike
- **Check all** / **Uncheck all** (on a yellowish background) select everything at once
- Press **Esc**, or the button that opened the dialog again, to close it. The
  **Clear** button in Clear Targets is red

### Copy history — CPHIST

The **CPHIST** system button at the top lists what you have copied (newest
first, up to 200). Filter by **tab** (channel, curve editing, FX), **unit**
(global / operator / pad / target) and **section**. **Copy again** at the right
end of a row makes that row the current copy, ready to paste. **Close**, a click
outside the list, or **Esc** closes it. The button is blue.

## UTILITY

Reading and writing parameters. `[IM]` imports and `[EX]` exports.

Each section can be moved in and out as its own file, so you can carry just the
AMP ENV over to another channel, or reuse only the LFO settings. A whole channel
can also go into a single file.

**Level -> All Ch** (copies this channel's level to every other channel at once)
moved to [EDIT](#edit) in 3.6.3.

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
