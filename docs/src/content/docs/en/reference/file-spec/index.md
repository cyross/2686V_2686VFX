---
title: Overview
description: What is inside each file
sidebar:
  order: 0
---

What is inside the files the plugins read and write. They are text, so you can
edit them by hand or generate them from a script.

## The shape they all share

Every parameter file is wrapped the same way.

```json
{
  "format": "<the kind's name>",
  "version": 1,
  "values": { ... }
}
```

The reader checks `format` at the root and **will not open a file that does not
match**. Getting the keys right is not enough — without this marker, nothing
happens at all.

What sits under `values` differs by kind. **Anything you leave out keeps its
current value** — it is not reset to the default — so a file that carries only
the parts you need is perfectly valid.

The saved form can be `JSON` or `YAML`, chosen in the settings. The contents
are laid out the same either way.

## One file per knob group

| File | What it holds |
| --- | --- |
| [`.ampEnv.json`](/2686V_2686VFX/en/reference/file-spec/ampenv/) | Level envelope |
| [`.pitchEnv.json`](/2686V_2686VFX/en/reference/file-spec/pitchenv/) | Pitch envelope |
| [`.ssgHwEnv.json`](/2686V_2686VFX/en/reference/file-spec/ssghwenv/) | SSG hardware envelope |
| [`.ssgSwEnv.json`](/2686V_2686VFX/en/reference/file-spec/ssgswenv/) | SSG software envelope (6 taps) |
| [`.ssgSwEnv11.json`](/2686V_2686VFX/en/reference/file-spec/ssgswenv11/) | The same, with 11 taps |
| [`.ssgSwPEnv11.json`](/2686V_2686VFX/en/reference/file-spec/ssgswpenv11/) | SSG software pitch envelope (11 taps) |
| [`.wtmod.json`](/2686V_2686VFX/en/reference/file-spec/wtmod/) | Pitch modulation from wavetable memory |
| [`.detune.json`](/2686V_2686VFX/en/reference/file-spec/detune/) | Multiple and detune |
| [`.unison.json`](/2686V_2686VFX/en/reference/file-spec/unison/) | Unison and harmony |
| [`.quality.json`](/2686V_2686VFX/en/reference/file-spec/quality/) | Quality |
| [`.pcmQuality.json`](/2686V_2686VFX/en/reference/file-spec/pcmquality/) | PCM quality |
| [`.pcmPlay.json`](/2686V_2686VFX/en/reference/file-spec/pcmplay/) | PCM playback position |
| [`.toneNoise.json`](/2686V_2686VFX/en/reference/file-spec/tonenoise/) | Tone and noise |
| [`.lfoOpzx7.json`](/2686V_2686VFX/en/reference/file-spec/lfoopzx7/) | LFO (shared) |
| [`.opnaHwLfo.json`](/2686V_2686VFX/en/reference/file-spec/opnahwlfo/) | OPNA hardware LFO |
| [`.lfoN88.json`](/2686V_2686VFX/en/reference/file-spec/lfon88/) | N88 LFO |
| [`.lfoOpm.json`](/2686V_2686VFX/en/reference/file-spec/lfoopm/) | OPM LFO |
| [`.lfoOpl.json`](/2686V_2686VFX/en/reference/file-spec/lfoopl/) | OPL LFO |

## Channel files

| File | What it holds |
| --- | --- |
| [`.param.opna.json`](/2686V_2686VFX/en/reference/file-spec/param-opna/) | OPNA channel |
| [`.param.opn.json`](/2686V_2686VFX/en/reference/file-spec/param-opn/) | OPN channel |
| [`.param.opl.json`](/2686V_2686VFX/en/reference/file-spec/param-opl/) | OPL channel |
| [`.param.opl3.json`](/2686V_2686VFX/en/reference/file-spec/param-opl3/) | OPL3 channel |
| [`.param.opm.json`](/2686V_2686VFX/en/reference/file-spec/param-opm/) | OPM channel |
| [`.param.opzx7s.json`](/2686V_2686VFX/en/reference/file-spec/param-opzx7s/) | OPZX7 channel |
| [`.param.ssg.json`](/2686V_2686VFX/en/reference/file-spec/param-ssg/) | SSG channel |
| [`.param.wt.json`](/2686V_2686VFX/en/reference/file-spec/param-wt/) | Wavetable channel |
| [`.param.wt2.json`](/2686V_2686VFX/en/reference/file-spec/param-wt2/) | Wavetable 2 channel |
| [`.param.wtplus.json`](/2686V_2686VFX/en/reference/file-spec/param-wtplus/) | Wavetable+ channel |
| [`.param.beep.json`](/2686V_2686VFX/en/reference/file-spec/param-beep/) | BEEP channel |
| [`.param.adpcm.json`](/2686V_2686VFX/en/reference/file-spec/param-adpcm/) | ADPCM channel |
| [`.param.rhythm.json`](/2686V_2686VFX/en/reference/file-spec/param-rhythm/) | Rhythm channel |
| [`.param.<chip>.op.json`](/2686V_2686VFX/en/reference/file-spec/param-op/) | A single operator |

## The rest

| File | What it holds |
| --- | --- |
| [`.2fx.json`](/2686V_2686VFX/en/reference/file-spec/2fx/) | Effect settings |
| [`.fxo.json`](/2686V_2686VFX/en/reference/file-spec/fxo/) | Effect order |
| [`curve_*.json`](/2686V_2686VFX/en/reference/file-spec/curve/) | Curves |
| [`.ccolor.json`](/2686V_2686VFX/en/reference/file-spec/ccolor/) | Interface colours |
| [Presets](/2686V_2686VFX/en/reference/file-spec/preset/) | The state of the whole plugin |
| [`.wt` / `.wt2`](/2686V_2686VFX/en/reference/file-spec/wave/) | Wavetable (text) |

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
