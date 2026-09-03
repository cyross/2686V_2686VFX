---
title: ADPCM channel
description: The channel that plays one audio file like a sampler
sidebar:
  order: 12
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_adpcm.png" alt="ADPCM channel" style="width:600px;" />
	<figcaption>ADPCM channel</figcaption>
</figure>

ADPCM **loads one audio file and plays it like a sampler**. It stands for the ADPCM playback the YM2608B had.

Where RHYTHM lays short sounds across several pads, this one **plays a single sound chromatically from the keyboard**. It suits bringing in a voice or an ambience and roughing it up until it sounds like the hardware.

:::note[Called PCM on 86V]
On 86V, to match the PC-9801-86, this channel is named **PCM** and its quality defaults to **4-bit PCM**. What it does and how you use it are unchanged.
:::

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_adpcm_86.png" alt="The PCM channel on 86V" style="width:600px;" />
	<figcaption>The PCM channel on 86V</figcaption>
</figure>

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## The sections on screen

### FORM — setting up the audio file

**Load** brings a file in. The loaded waveform is drawn on screen.

**Put LPST / LPED partway into the waveform** and only that part repeats while the key is down. It is how you get a sustained sound out of a short piece of material.

| Knob | What it does | Automation |
| --- | --- | --- |
| **LOOP** | Repeat the whole file | [`ADPCM_LOOP`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-loop) |
| **P.OF** | Where playback starts (ms) | [`ADPCM_PCM_OFFSET`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-pcm-offset) |
| **P.RT** | How fast it plays | [`ADPCM_PCM_RATIO`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-pcm-ratio) |
| **Loop Point Enable** | Set the range that repeats | [`ADPCM_LOOPPOINT_EN`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-looppoint-en) |
| **LPST / LPED** | That range, as a position from 0.0 to 1.0 of the whole | [`ADPCM_LOOPPOINT_ST`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-looppoint-st) |

### OPTIONAL

Grit can be mixed into the loaded sound (**ours**).

| Knob | What it does | Automation |
| --- | --- | --- |
| **T.LV** | How much of the loaded sound | [`ADPCM_TONE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-tone) |
| **N.LV** | How much of the grit | [`ADPCM_NOISE`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-noise) |
| **N.FQ** | How high the grit sits | [`ADPCM_NOISEFREQ`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-noisefreq) |
| **MIX** | Blend of the two | [`ADPCM_NOISEMIX`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-noisemix) |

### PAN

| Knob | What it does | Automation |
| --- | --- | --- |
| **PAN** | Position. 0 is left, 0.5 centre, 1 right | [`ADPCM_PAN`](/2686V_2686VFX/en/reference/automation/adpcm/#adpcm-pan) |

### FIX — holding the pitch

Ignores the pitch of the key and sounds at a set pitch. Use it when a sound should come out the same whatever key is pressed, the way a drum does.

For the knobs, see [FIX](/2686V_2686VFX/en/chips/common/#fix).

## Shared sections

The following do not belong to any one chip. They are all described under
[Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY(PCM)**](/2686V_2686VFX/en/chips/common/#qualitypcm) | Bit depth, storage scheme, sample rate, and how the gaps are filled on the way back |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/en/chips/common/#wt-pitch-mod) | Pitch modulation from wavetable memory |
| [**WT AMP MOD**](/2686V_2686VFX/en/chips/common/#wt-amp-mod) | Level modulation from wavetable memory |
| [**LFO**](/2686V_2686VFX/en/chips/common/#lfo) | Wobbles level and pitch at a steady rate |
| [**MUL/DET**](/2686V_2686VFX/en/chips/common/#muldet) | Shifts the pitch |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

## The bundled presets

`M-M-Pro` includes an example that builds speech out of the rhythm channel and ADPCM together.

For how it may be used, see [License](/2686V_2686VFX/en/reference/license/).

## Automation

The full list of what ADPCM exposes to the DAW is under
[ADPCM automation](/2686V_2686VFX/en/reference/automation/adpcm/), with IDs, types, ranges and
defaults.
