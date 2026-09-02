---
title: RHYTHM channel
description: A drum machine that maps audio files onto pads
sidebar:
  order: 11
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_rhythm.png" alt="RHYTHM channel" style="width:600px;" />
	<figcaption>RHYTHM channel</figcaption>
</figure>

RHYTHM is the channel that stands for **the YM2608B's rhythm section**. The hardware kept six drum sounds in ROM; here you **map whatever audio files you like onto the pads**.

Because the quality-reduction machinery is attached, you can pull a loaded sound towards the texture of a period drum chip.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## The pads

| Plugin | Pads |
| --- | :---: |
| 2686V / 2686VLight | **8** |
| 86V | **6** |

The hardware (YM2608B) had six; we have **taken it to eight** (**ours**). 86V keeps six, to match the 86 board.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_rhythm_86.png" alt="The RHYTHM channel on 86V" style="width:600px;" />
	<figcaption>The RHYTHM channel on 86V</figcaption>
</figure>

The pad names follow the hardware's drum line-up.

| # | Name | |
| :---: | --- | --- |
| 1 | **BD** | Bass drum |
| 2 | **SD** | Snare drum |
| 3 | **RIM** | Rim shot |
| 4 | **TOM** | Tom |
| 5 | **CYMBAL** | Cymbal |
| 6 | **HI-HAT** | Hi-hat |
| 7 | **PERC1** | Percussion (**ours**) |
| 8 | **PERC2** | Percussion (**ours**) |

:::tip[What the eight pads are for]
RHYTHM has eight pads on 2686V and 2686VLight, which also makes it **a stand-in for the eight-voice SSGPCM (PSGPCM) that PPZ8 and its kin provided.**
Lining up sounds that are not drums is a handy way to recreate that kind of switched playback too.
:::

## The sections on screen

Every pad has the following of its own.

### FORM — setting up the audio file

**Load** brings a file in. The loaded waveform is drawn on screen.

| Knob | What it does | Automation |
| --- | --- | --- |
| **One Shot** | Play right through even after the key comes up | [`RHYTHM_PAD[0-7]_ONESHOT`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-oneshot) |
| **POFF** | Where playback starts (ms) | [`RHYTHM_PAD[0-7]_PCM_OFFSET`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-pcm-offset) |
| **PRT** | How fast it plays | [`RHYTHM_PAD[0-7]_PCM_RATIO`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-pcm-ratio) |
| **Loop Point Enable** | Set the range that repeats | [`RHYTHM_PAD[0-7]_LOOPPOINT_EN`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-looppoint-en) |
| **LPST / LPED** | That range, as a position from 0.0 to 1.0 of the whole | [`RHYTHM_PAD[0-7]_LOOPPOINT_ST`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-looppoint-st) |

### NOTE — mapping to MIDI keys

The MIDI note number that sounds the pad. **You can change the mapping freely**, so it can be made to fit a drum map you already use.

| Knob | What it does | Automation |
| --- | --- | --- |
| **NOTE** | The note number that sounds this pad | [`RHYTHM_PAD0_NOTE`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad0-note) |

### OPTIONAL

Grit can be mixed into the loaded sound (**ours**).

| Knob | What it does | Automation |
| --- | --- | --- |
| **T.LV** | How much of the loaded sound | [`RHYTHM_PAD[0-7]_TONE`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-tone) |
| **N.LV** | How much of the grit | [`RHYTHM_PAD[0-7]_NOISE`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-noise) |
| **N.FQ** | How high the grit sits | [`RHYTHM_PAD[0-7]_NOISEFREQ`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-noisefreq) |
| **MIX** | Blend of the two | [`RHYTHM_PAD[0-7]_NOISEMIX`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-noisemix) |

### LEVEL / PAN

| Knob | What it does | Automation |
| --- | --- | --- |
| **LV** | Level of that pad | [`RHYTHM_PAD[0-7]_VOL`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-vol) |
| **PAN** | Position of that pad | [`RHYTHM_PAD[0-7]_PAN`](/2686V_2686VFX/en/reference/automation/rhythm/#rhythm-pad-0-7-pan) |

### Envelopes

Every pad can carry the envelopes from [Shared sections](/2686V_2686VFX/en/chips/common/#envelope) — so you can, for instance, move the pitch on every individual hit.

[MUL/DET](/2686V_2686VFX/en/chips/common/#muldet) for shifting the pitch and [LFO](/2686V_2686VFX/en/chips/common/#lfo) for wobbling it are also placed per pad.

## Shared sections

The following do not belong to any one chip. They are all described under
[Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY(PCM)**](/2686V_2686VFX/en/chips/common/#qualitypcm) | Bit depth, storage scheme, sample rate, and how the gaps are filled on the way back |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**OPTIONAL**](/2686V_2686VFX/en/chips/common/#optional) | Extra behaviour |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

## The bundled samples

`Assets/2686V/Samples` holds drum sounds built on the SSG. The `SSG RHYTHM Sample` preset shows one way of putting them together.

For how they may be used, see [License](/2686V_2686VFX/en/reference/license/).

## Automation

The full list of what RHYTHM exposes to the DAW is under
[RHYTHM automation](/2686V_2686VFX/en/reference/automation/rhythm/), with IDs, types, ranges and
defaults.
