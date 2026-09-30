---
title: What changed in v3.6.3
description: What's fixed and changed in 3.6.3
sidebar:
  order: 2
---

This page covers changes from 3.6.2 to 3.6.3. **Your files remain compatible**,
and existing patches sound the same.

## 2686VFX: no more blip when you stop playback

**With DELAY / REVERB / SFC ECHO or modulation (ENV, LFO, PITCH and so on) in
use, stopping playback let out a short burst of sound.**

2686VFX told the host it had no tail. Hosts such as Cubase stop processing a VST3
plug-in once its input goes silent. When you stopped playback, the delay lines and
the modulation buffers froze with audio still in them, and that leftover audio came
out the next time processing resumed.

3.6.3 fixes both halves:

- The plug-in now reports an unlimited tail, so tails ring out fully after you stop
- When the host stops processing, everything held in the buffers is discarded

## FM operator graphs now redraw after loading a channel parameter file

**After loading a channel parameter file, the envelope graphs of operators not
selected with TARGET kept their old shape** until you selected them.

3.6.3 redraws every operator's graph after the load. Loading a single operator's
parameter file redraws that operator's graph as well.

## UNISON/HARMONY: P-SPREAD and P-DETUNE can now target voice 0 (the original)

Per-voice P-SPREAD and P-DETUNE used to cover voices 1 to 7 only. In 3.6.3 **VOICES
also offers 0, the original voice**, so you can shift its position and pitch like
any other voice.

- The default is 0, so existing sounds do not change
- Automation IDs are `…_UNI_PDIST0` / `…_UNI_PDET0`
- Files store it as `mainVoice`; the `paraVoices` list still holds voices 1 to 7

## A more compact algorithm matrix and algorithm diagram

### Matrix (OPZX7)

The routing and feedback matrices used to sit one above the other. Now **a switch
shows one at a time**: pick **ROUTING** or **FEEDBACK** at the top (they work like
radio buttons). The algorithm diagram moves up into the space this frees.

Which matrix is showing is screen-only state and is not saved.

### Algorithm diagram (all FM channels)

- Operator boxes are 2px smaller on every side, and the gaps between them shrink to match
- The self-feedback loop is 2px smaller in diameter and is centred on the box's top-left corner
- Feedback lines to other operators run 3px less to the side
- The "FB" labels are gone (colour and dashes already tell them apart)
- OP8's colour is lighter so its number is easier to read
- The diagram is shorter overall

## QUALITY gains a "DAC" row

QUALITY (non-PCM and PCM) and the 2686VFX PCM Bit Crusher now start with a **DAC**
row. Pick a machine and press **Apply** to set BIT RATE, SMP.RATE and INTERP to
match that machine's output in one go.

- The machine you pick is screen-only; it is not written to presets or channel
  parameter files. What stays is the BIT RATE, SMP.RATE and INTERP it set
- Machines that give identical settings share one entry, such as "98/88"
- Where a machine's sample rate is not on the list, the nearest one is used (for
  example, the X68000's OPM runs at 62.5kHz and gets 55.5kHz)

### QUALITY (non-PCM)

Matches the DAC the synth output goes through.

| Machine | BIT RATE | SMP.RATE | INTERP |
| --- | --- | --- | --- |
| 98/88/X68K | 10-bit | 55.5kHz | ZOH |
| MSX | 9-bit | 49.7kHz | ZOH |
| FC/GB | 4-bit | 96kHz | ZOH |
| SFC | 16-bit | 32kHz | ZOH |
| PCE | 5-bit | 96kHz | ZOH |
| MD/TOWNS | 9-bit | 55.5kHz | ZOH |
| PS1 | 16-bit | 44.1kHz | ZOH |

### QUALITY (PCM)

Matches the circuit that plays samples.

| Machine | BIT RATE | SMP.RATE | INTERP |
| --- | --- | --- | --- |
| 98 | 16-bit PCM (PC-9801-86) | 44.1kHz | ZOH |
| 88 | 4-bit ADPCM (OPNA) | 16kHz | ZOH |
| MSX | 8-bit PCM (turbo R) | 16kHz | ZOH |
| X68K | IMA ADPCM (close to the MSM6258) | 16kHz | ZOH |
| FC | 1-bit DPCM | 33.08kHz | ZOH |
| SFC | SNES BRR | 32kHz | Gaussian |
| PCE | 5-bit PCM | 8kHz | ZOH |
| MD/TOWNS | 8-bit PCM | 22.05kHz | ZOH |
| GB | 4-bit PCM | 8kHz | ZOH |
| PS1 | PS1 VAG | 44.1kHz | Gaussian |

### 2686VFX PCM Bit Crusher

ADPCM-style formats are not available here, so each machine is approximated by the
bit depth its hardware decodes to.

| Machine | BIT | RATE | INTERP |
| --- | --- | --- | --- |
| 98 | 16-bit PCM | 44.1kHz | ZOH |
| 88 | 16-bit PCM | 16kHz | ZOH |
| MSX | 8-bit PCM | 16kHz | ZOH |
| X68K | 12-bit PCM | 16kHz | ZOH |
| FC | 7-bit PCM | 33.08kHz | ZOH |
| SFC | 16-bit PCM | 32kHz | Gaussian |
| PCE | 5-bit PCM | 8kHz | ZOH |
| MD/TOWNS | 8-bit PCM | 22.05kHz | ZOH |
| GB | 4-bit PCM | 8kHz | ZOH |
| PS1 | 16-bit PCM | 44.1kHz | Gaussian |

## 2686VFX: noise reduction

The noise reduction from the instruments' QUALITY(PCM) is now available in 2686VFX.

- A new effect, **[Noise reduction](/2686V_2686VFX/en/fx/noise-reduction/)** —
  QUALITY(PCM)'s **GATE** (pulls quiet sound down to 0) and **LPF** (high cut).
  **RATE** sets where the LPF cuts. Place it after the PCM bit crusher with the
  same RATE and it acts just as it does in the instruments
- **NR: Resample** in the **[PCM bit crusher](/2686V_2686VFX/en/fx/pcm-bitcrusher/)** —
  cuts the highs before decimating so they do not alias. It only makes sense
  together with the decimation, so it lives inside the crusher

Everything starts switched off (noise reduction is bypassed), so existing
sounds do not change. Noise reduction is added at the end of the effect order.

## Drag and drop files onto the window to load them

Drop a file from Explorer (or any file manager) onto the plug-in window and it loads
straight away, behaving exactly as if you had loaded it with the usual button.

| File | After loading |
| --- | --- |
| Preset | Opens that channel's tab |
| Channel parameters | Opens that channel's tab |
| FX order (`.fxo`) | The FX order changes |
| FX parameters (`.2fx`) | The FX values change |

2686VFX accepts FX order and FX parameter files; the screen stays as it is.
When you drop several files at once, they load in order.

## Choose the base for relative paths

Presets and channel parameter files record where audio and wave files are, and
until now relative paths were always taken from the folders set in SETTINGS
(`Samples` / `Wavetable`). In 3.6.3 you can pick the base:

| Choice | Base |
| --- | --- |
| Settings folder | The folders set in SETTINGS (as before; the default) |
| File's folder | The folder of the preset or channel parameter file being read or written |

- Pick it with **REL.PATH** in the UTILITY section of OPZX7, RHYTHM, PCM (ADPCM),
  PCM+ (ADPCM+), WT, WT2 and WT+, or at the top of the folder settings in SETTINGS.
  There is one value for the whole plug-in; changing it in one place changes it
  everywhere
- It is stored in the settings file (when you save in SETTINGS)
- With "File's folder", channel parameter files also record locations relative to
  themselves, so you can keep a sound and its material in one folder and move the
  folder as a whole
- When reading, the other base is tried as well, so files written either way load
- Saving into a DAW project has no file to go by, so it uses the settings folders

Alongside this, **WT, WT2 and WT+ channel parameter files now record where their
wave files are**: the modulation waves of WT PITCH MOD and WT AMP MOD (`waves`) and
each WT+ slot's wave (`waveFile`). Loading such a file reloads the waves. Older
files do not have them, so loading one leaves the current waves in place.
