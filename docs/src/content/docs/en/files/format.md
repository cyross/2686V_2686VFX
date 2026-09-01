---
title: Formats and locations
description: File formats, extensions and folder layout in 3.0.0
sidebar:
  order: 1
---

## Choose between JSON and YAML

Version 3.0.0 moved parameter files to a **named** format. Previously values
were held by line position alone, so adding a field broke older files, and a
mistake in the ordering silently loaded the wrong value. Names avoid both.

Pick the format under **"File format"** on the SETTINGS tab.

- **JSON** — the default; fastest to read and write
- **YAML** — easier to read and edit by hand

:::note
**Switching does not strand your existing files.** Reading is decided by
inspecting the content rather than the name, so files still load even if their
extension has been changed. Only newly written files are affected.
:::

## Where files live

Everything is kept in the shared folder under your Documents folder:

```text
(Documents)\2686V\
```

| Folder | Contents |
| --- | --- |
| `Presets` | Presets |
| `Samples` | Audio samples you have loaded |
| `Wavetables` | Wave tables (.wt / .wt2) |
| `FxOrders` | Effect ordering |
| `FxParams` | Effect parameters |
| `ChannelParams` | Channel parameters |
| `CurveParams` | Curves |
| `LfoParams` | LFO settings |
| `AmpEnvParams` | Amplitude envelopes |
| `PitchEnvParams` | Pitch envelopes |
| `SsgHwEnvParams` | SSG hardware envelopes |
| `SsgSwEnvParams` | SSG software envelopes |
| `DetuneParams` | Detune settings |
| `UnisonParams` | Unison settings |
| `QualityParams` | Quality settings |
| `PcmPlayParams` | PCM playback settings |
| `ToneNoiseParams` | Tone and noise settings |
| `CustomizeColorSettings` | Colour schemes |
| `Resources` | Wallpapers and other assets |

Two files sit directly in that folder:

| File | Contents |
| --- | --- |
| `init.settings.json` | Default settings, read at launch |
| `2686V.library.json` | Favourites and history (one per plugin) |

:::tip
Both become `.yaml` when YAML is selected. At launch both names are looked for;
whichever exists is used, and the newer one wins if both are present.
:::

## Extensions

Each plugin uses its own preset extension.

| Plugin | Extension |
| --- | --- |
| 2686V | `.2686v.json` |
| 2686VLight | `.2686vl.json` |
| 86V | `.86v.json` |
| OPZX7S | `.opzx7s.json` |
| 2686VFX | no presets |

Parameter files have an extension per kind: OPNA channel parameters are
`.param.opna.json`, an amplitude envelope is `.ampEnv.json`, and so on.

## Files that cross between plugins

Unlike presets, **FX files can be read by any plugin.**

| Kind | Extension |
| --- | --- |
| FX order | `.fxo.json` |
| FX parameters | `.2fx.json` |

The order file stores effects by **name**. Storing them by number would mean
that adding an effect shifts every position, turning one effect into another.
Names survive a differing effect count between plugins.

- Unknown names are **skipped** — an effect the plugin doesn't have won't break
  anything
- Effects that weren't listed are **appended** — the set always ends up complete

For example the PCM bit crusher, which only 2686VFX has, is skipped when an
instrument reads the file; and a file written by an instrument gains it at the
end when 2686VFX reads it.

:::caution
**Wave data (`.wt` / `.wt2`) was not migrated.** These hold waveforms rather
than lists of values, so they keep their original format.
:::

## Files written before 3.0.0

- **Presets (`.xml`)** — can be read. Open one and save it again to move it to
  the new format
- **Parameter files** — converted automatically when you open them. See
  [Converting older files](/2686V_2686VFX/en/files/convert/)
