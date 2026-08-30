---
title: Converting older files
description: Moving parameter files written before 3.0.0 to the new format
sidebar:
  order: 2
---

Parameter files written before 3.0.0 are **converted simply by opening them.**
There is no separate conversion screen.

## Steps

1. Press the import button for the kind of file you want to convert
   (for a detune file, the Import button in an operator's DETUNE section)
2. Pick the old file. Old files are the ones whose extension is neither
   `.json` nor `.yaml`
3. The values load, and **a file in the new format is written alongside it**
4. A notice tells you the original name and where the new file was saved

Opening `bass.detune`, for example, produces `bass.detune.json` next to it. The
format written follows the "File format" setting on the SETTINGS tab.

## Worth knowing

:::note
**The original file is left alone.** Converting makes a copy.
:::

:::caution
**If the converted file already exists, nothing is written** (the values still
load). Conversion runs every time you open the old file, so this prevents it
overwriting edits you made after converting. To rebuild it, delete the
converted file first.
:::

## What can be converted

Channels, operators, FX ordering and parameters, envelopes, LFOs, detune,
unison, quality, PCM playback, tone/noise, curves and rhythm pads — **anything
the older versions could export can be converted.**

Two exceptions:

- **Wave data (`.wt` / `.wt2`)** — never migrated, so there is nothing to convert
- **Presets (`.xml`)** — loaded from the PRESET tab as usual rather than through
  an import button. Save one again to move it to the new format

Import buttons that read another tab's file (reading an OPL3 file from the OPL
tab, for instance) do not convert. **Open the file from its own tab instead.**
