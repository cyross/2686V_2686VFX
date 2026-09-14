---
title: ADPCM+ channel
description: The channel that lines up 32 audio files and switches between them
sidebar:
  order: 13
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_adpcmp.png" alt="ADPCM+ channel" style="width:600px;" />
	<figcaption>ADPCM+ channel</figcaption>
</figure>

ADPCM+ is for **loading up to 32 audio files and switching between them while you play**. It is new in 3.3.0.

Where ADPCM plays one piece of material across the keyboard, ADPCM+ **picks one out of a row and plays that.** Think of it as what [WT+](/2686V_2686VFX/en/chips/wtplus/) does for wavetable memory, moved over to PCM.

The big difference from ADPCM is that **the sound shaping is split per file.** FORM, OPTIONAL and PAN are held separately for each of the 32 slots, so switching material switches how it is played as well.

:::note[This channel is ours]
Nothing on the hardware corresponds to it directly. It packages up, in a form that is easy to work with, what the sound drivers of the day did when they swapped the ADPCM read position mid-performance to change the tone.
:::

:::tip[Acknowledgement]
This channel was added after taking inspiration from [a post](https://x.com/yuzokoshiro/status/2098008081573855457) by Yuzo Koshiro. It was a tremendous help. Thank you for so generously sharing your knowledge.
:::

## How to read this page

| Mark | Meaning |
| --- | --- |
| **Hardware** | A feature the real chip has. It behaves as the chip did |
| **Ours** | An extension of our own that the hardware does not have |

## How the screen is split

The screen is in two groups.

| Group | What is in it |
| --- | --- |
| **ADPCM+ channel settings** | Everything that applies to the whole channel: level, envelopes, LFO, QUALITY |
| **PCM settings** | Everything held per file: FORM, OPTIONAL, PAN |

The **PCM settings** group on the right is the heart of this channel.

## PCM settings — per-file settings

### SLOT and TARGET

The two knobs at the top of the group have clearly different jobs.

| Knob | What it does | Automation |
| --- | --- | --- |
| **SLOT** | Which PCM **sounds**. 0–31 | [`ADPCMP_SLOT`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot) |
| **TARGET** | Which PCM is **shown on screen**. 0–31 | — |

**SLOT affects the sound.** It is an automation target, so you can draw the material changes against time in the DAW.

**TARGET does not affect the sound.** It only decides whose values the FORM, OPTIONAL and PAN below are showing, so you can edit one PCM while a different one is sounding.

:::tip[They sit where they cannot be folded away]
SLOT and TARGET are outside the collapsible sections. They govern everything below them, so if they folded out of sight you could no longer pick a slot.
:::

### FORM — the audio file

**Load** reads a file into the slot TARGET is pointing at. **X** empties that slot.

Loaded material is shown two ways.

- **The single view on top** draws the TARGET slot over the range that P.OFFSET and P.RATIO cut out. When Loop Point is in use, its positions appear as vertical lines.
- **The 32 below** are the material itself, laid out in a grid, so you can see at a glance what is in which slot. The one with a **border** is TARGET; the one with a **coloured background** is SLOT.

The four knobs that follow mix grit into the loaded sound (**ours**).

| Knob | What it does | Automation |
| --- | --- | --- |
| **T.LEVEL** | How much of the loaded sound | [`ADPCMP_SLOT[0-31]_TONE`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-tone) |
| **N.LEVEL** | How much grit | [`ADPCMP_SLOT[0-31]_NOISE`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-noise) |
| **N.FREQ** | How high the grit sits | [`ADPCMP_SLOT[0-31]_NOISEFREQ`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-noisefreq) |
| **MIX** | The balance between the two | [`ADPCMP_SLOT[0-31]_NOISEMIX`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-noisemix) |

### OPTIONAL — how it is played

**Put LP.START and LP.END partway into the wave** and only that stretch repeats while the key is held, so a short piece of material can become a sustaining sound.

| Knob | What it does | Automation |
| --- | --- | --- |
| **SPEED** | Multiplier on the playback rate | [`ADPCMP_SLOT[0-31]_SPEED`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-speed) |
| **CNT** | How many times to go round LP.START–LP.END before moving on. 0 means forever | [`ADPCMP_SLOT[0-31]_LOOPPOINT_CNT`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-looppoint-cnt) |
| **P.OFFSET** | Where to start, in milliseconds | [`ADPCMP_SLOT[0-31]_PCM_OFFSET`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-pcm-offset) |
| **P.RATIO** | How much of the remaining length to play | [`ADPCMP_SLOT[0-31]_PCM_RATIO`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-pcm-ratio) |
| **LOOP** | Repeat the whole file | [`ADPCMP_SLOT[0-31]_LOOP`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-loop) |
| **Loop Point Enable** | Set the stretch that repeats | [`ADPCMP_SLOT[0-31]_LOOPPOINT_EN`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-looppoint-en) |
| **LP.START / LP.END** | That stretch, as 0.0–1.0 across the whole | [`ADPCMP_SLOT[0-31]_LOOPPOINT_ST`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-looppoint-st) |

### PAN

| Knob | What it does | Automation |
| --- | --- | --- |
| **PAN** | Position. 0 is left, 0.5 centre, 1 right | [`ADPCMP_SLOT[0-31]_PAN`](/2686V_2686VFX/en/reference/automation/adpcmplus/#adpcmp-slot-0-31-pan) |

Since it is held per file, **you can spread a row of PCMs across the stereo field as they are.**

## Whole-channel settings

### FIX — a fixed pitch

Ignores the pitch of the key and plays at a set height. Use it when every key should sound at the same pitch, as with percussion.

The knobs are described under [FIX](/2686V_2686VFX/en/chips/common/#fix).

### QUALITY(PCM)

Bit depth, encoding and sample rate are **one setting for the whole channel**. They correspond to the chip side of the hardware, so they are not split per file.

## One way to use it

:::tip[Switching through a row of material]

1. Load the same instrument at different strengths, softest first
2. Move SLOT with automation
3. Within a single note, only the attack changes

Vary P.OFFSET and PAN per slot as well and the switch reads differently again.
:::

## Shared sections

The following do not depend on the chip. Each is described in full under [shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY(PCM)**](/2686V_2686VFX/en/chips/common/#qualitypcm) | Bit depth, encoding and sample rate, plus the interpolation on read-back |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**WT PITCH MOD**](/2686V_2686VFX/en/chips/common/#wt-pitch-mod) | Pitch modulation from wavetable memory |
| [**WT AMP MOD**](/2686V_2686VFX/en/chips/common/#wt-amp-mod) | Level modulation from wavetable memory |
| [**LFO**](/2686V_2686VFX/en/chips/common/#lfo) | Sway the level and pitch periodically |
| [**MUL/DET**](/2686V_2686VFX/en/chips/common/#muldet) | Shift the pitch |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stack the same note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Read and write parameters |

For the detail on QUALITY, see the [reference](/2686V_2686VFX/en/reference/lists-quality/).

## About the files

The channel parameter file is `.param.adpcmplus`. It holds **all 32 sets of settings, and where each of them reads its file from**. It is a different kind from ADPCM's `.param.adpcm`, so the two cannot be confused.

The material itself does not go into the file. Only its location is remembered, and it is read back from there. See [file structure](/2686V_2686VFX/en/files/format/) for the detail.

## Related pages

- [ADPCM](/2686V_2686VFX/en/chips/adpcm/) — plays one piece of material
- [WT+](/2686V_2686VFX/en/chips/wtplus/) — the same idea for wavetable memory
- [RHYTHM](/2686V_2686VFX/en/chips/rhythm/) — assigns material to keys and plays several at once

## Automation

The full list of parameters ADPCM+ exposes to the DAW is at [ADPCM+ automation](/2686V_2686VFX/en/reference/automation/adpcmplus/), with ID, type, range and default.
