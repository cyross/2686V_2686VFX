---
title: Feature list
description: What each plugin carries
sidebar:
  order: 0
---

A list of what is included, and which plugin carries it.

- ✔ … present
- — … absent

## Channels (sound chips)

| Channel | What it is | 2686V | 2686VLight | 26V | 86V | OPZX7S |
| --- | --- | :---: | :---: | :---: | :---: | :---: |
| OPNA | FM, with hardware LFO and the SSG hardware envelope | ✔ | ✔ | — | ✔ | — |
| OPN | FM, with the N88-BASIC software LFO | ✔ | ✔ | ✔ | — | — |
| OPL | FM, with a hardware LFO per operator | ✔ | ✔ | — | — | — |
| OPL3 | FM, with original algorithms (2OPx2) added | ✔ | ✔ | — | — | — |
| OPM | FM, with the OPP key scale added | ✔ | ✔ | — | — | — |
| OPZX7 | an original modern FM chip, up to 8 operators | ✔ | ✔ | — | — | ✔ |
| SSG | square, triangle and noise | ✔ | ✔ | ✔ | ✔ | — |
| WAVETABLE | wave memory, set as real numbers | ✔ | ✔ | — | — | — |
| WT2 | wave memory, set in steps | ✔ | ✔ | — | — | — |
| WTPLUS | switching between wave memories (up to 32) | ✔ | ✔ | — | — | — |
| RHYTHM | drum machine, 8 pads (6 on 86V) | ✔ | ✔ | — | ✔ | — |
| ADPCM | a sampler for one audio file (called PCM on 86V) | ✔ | ✔ | — | ✔ | — |
| BEEP | the beeper | ✔ | ✔ | — | — | — |

:::note
2686VFX has no channels, because it makes no sound.
:::

## Envelopes

Available on every channel, operator and rhythm pad.

| Name | Target | What it does |
| --- | --- | --- |
| AMP ENV | level | ADSR, with the original KOR / XOF / bypass options |
| SSG HW AMP ENV | level | the hardware envelope, plus original waveforms and a settable range |
| SSG SW ENV | level | 6-tap, with a settable loop count (0 loops forever) |
| SSG SW ENV11 | level | 11-tap |
| PITCH ENV | pitch | 3-tap, ±4800 cents |
| SSG SW PENV11 | pitch | 11-tap, ±4800 cents |
| WT PITCH MOD | pitch | modulation from wave memory: FDS / WonderSwan / HuC6280 |

## Other sound-shaping

| Feature | What it does |
| --- | --- |
| MUL / DET | multiple and detune; DT3 takes a freely set value |
| LFO | independent frequency and sync for AM and PM, with original waveforms |
| Unison / harmony | up to 8 voices, with detune, spread and the pseudo fast arpeggio |
| Sound effect mode | provided per channel |
| MML input | per operator |
| Curve editing | pick from 30-plus ways a value can change (2686V / OPZX7S only) |

## Effects (FX)

| Effect | The five instruments | 2686VFX |
| --- | :---: | :---: |
| Filter | ✔ | ✔ |
| 3-band EQ | ✔ | ✔ |
| Tremolo | ✔ | ✔ |
| Vibrato | ✔ | ✔ |
| Bit crusher | ✔ | ✔ |
| Delay | ✔ | ✔ |
| Reverb | ✔ | ✔ |
| SFC echo | ✔ | ✔ |
| **PCM bit crusher** | — | ✔ |

The processing order can be rearranged, and the order file
[can be read across plugins](/2686V_2686VFX/en/files/format/).

## 2686VFX: modulating the output

This one is unique to 2686VFX. The instruments' envelopes and LFO can be applied
to the output itself, driven from a MIDI keyboard. See
[2686VFX (the effect)](/2686V_2686VFX/en/plugins/fx-plugin/) for details.

| Panel | What it moves |
| --- | --- |
| AMP ENV / SSG HW AMP ENV / SSG SW AMP ENV[11] | level |
| LFO | level (AM) and pitch (PM) |
| PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD | pitch |
| MUL・DET / UNISON・HARMONY | pitch, by a fixed amount |

## Playing

| Feature | What it does |
| --- | --- |
| Polyphony | up to 10 voices; up to 80 combined with unison |
| Monophonic | switchable |
| Pitch bend | from a MIDI keyboard |
| Modulation | from a MIDI keyboard |
| Headroom | the reserved gain can be changed |
| Automation | covers every parameter |

## Screen and files

| Feature | What it does |
| --- | --- |
| View modes | full / mini player / minimum |
| Wallpaper | put an image behind it, with a choice of how it sits |
| COLORS | change each colour; pick from the palettes of the era |
| Presets | save and load, with favourites, history and filtered search |
| Parameter I/O | per channel, operator, pad or envelope |
| File format | `JSON` and `YAML`, switchable in the settings |
| Waveform preview | see loaded audio, wave memory and envelopes on screen |

:::note
2686VFX has no PRESET tab. To keep a set of settings, use your DAW's own plugin
preset feature.
:::
