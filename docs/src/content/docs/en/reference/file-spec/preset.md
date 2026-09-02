---
title: Presets
description: What is inside .2686v.json and its kin
sidebar:
  order: 54
---

The state of the whole plugin. Beyond the channel settings it holds which
channel is selected, the effects, the curves, and where the audio files are.

| Plugin | Extension |
| --- | --- |
| 2686V | `.2686v.json` |
| 2686VLight | `.2686vl.json` |
| 26V | `.26v.json` |
| 86V | `.86v.json` |
| OPZX7S | `.opzx7s.json` |

The `format` is always `preset` and the version is `1`. The extensions differ,
so one `Presets` folder can hold them all and each plugin still lists only its
own.

## Shape

```json
{
  "format": "preset",
  "version": 1,
  "values": {
    "meta": {
      "presetName": "My Sound",
      "presetAuthor": "",
      "presetVersion": "1.0.0",
      "presetComment": "",
      "presetGenre": "",
      "activeModeName": "OPNA",
      "pluginVersion": "3.0.0",
      "adpcmPath": "",
      "rhythmPath0": ""
    },
    "params": {
      "MODE": "0",
      "OPNA_ALG": "4"
    }
  }
}
```

## meta

Holds the heading information and where the audio files are.

| Key | What it holds |
| --- | --- |
| `presetName` / `presetAuthor` / `presetVersion` | Name, author, version |
| `presetComment` / `presetGenre` | Comment and genre |
| `activeModeName` | The channel name at the time of saving |
| `pluginVersion` | The version of the plugin that saved it |
| `adpcmPath` | The ADPCM audio file |
| `rhythmPath0` – `rhythmPath7` | The audio file on each rhythm pad |
| `opzx7PcmPath0` – / `opzx7WtPath0` – / `opzx7Wt2Path0` – | The wave on each OPZX7 operator |
| `wtPlusWavePath0` – | The wave in each WT+ slot |

**Locations are written relative to the `Samples` folder.** An absolute path is
used as it stands, so a preset someone else made will sound as intended as long
as you put the material underneath that folder.

## params

**The automation names are used as the keys**, with the values held as text.
Names, types and ranges are in the
[automation reference](/2686V_2686VFX/en/reference/automation/).

**Any parameter you leave out keeps its current value.** A preset can carry only
the channels you actually use. Names of channels a plugin does not have are
skipped, so a preset made on 2686V can be loaded on 26V.

:::note[Presets from before 3.0.0]
They used to be XML. Reading them is still supported, so anything you built up
still works. Writing only produces the new form. See
[converting the old format](/2686V_2686VFX/en/files/convert/).
:::

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
