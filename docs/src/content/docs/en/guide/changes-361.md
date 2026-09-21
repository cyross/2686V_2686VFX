---
title: What changed in v3.6.1
description: What's fixed and changed in 3.6.1
sidebar:
  order: 3
---

This page covers changes from 3.6.0 to 3.6.1. **Your files remain compatible.**
No parameters were added or removed, so opening an existing project does not
change its sound.

## Loaded waveforms missing after reopening a project

**A waveform loaded into WT+ could be missing after the project was saved and
reopened.** It was found in FL Studio, but it happened in every DAW and in the
standalone too.

Wave file locations are saved as paths relative to the **wavetable folder** in
SETTINGS. Every time a waveform was loaded, however, that wavetable folder was
**rewritten to the folder of the loaded file**. A waveform loaded from a
subfolder was therefore saved as a bare file name. After a restart the
wavetable folder is back to its setting, and the bare name no longer pointed
at the file.

The same rewrite happened when loading:

- WT / WT2 waveforms
- OPZX7 operator WT / WT2
- WT MOD / WT AMP MOD modulation waves

In 3.6.1, loading **no longer rewrites the wavetable folder**. The file list
still opens where you left it, since it remembers that on its own, and the
wavetable folder in SETTINGS no longer changes behind your back.

**Projects and presets saved with 3.6.0 or earlier**: if a wave file is not at
its saved path, it is **looked up by name** under the wavetable folder, then
under the plugin's own folder. When more than one file has that name, nothing
is loaded rather than guessing. If the waveform comes back, save once more and
the correct path is stored from then on.

Also fixed: loading a preset while the WT+ tab was open left the old slot
waveforms and name on screen.

## File picker

- A **Pause all / Play all** button left of the cycle switch (1 / 2 / 5 / 10)
  pauses or resumes every row preview at once
- A **Default folder** button next to Delete folder jumps back to the folder
  set in SETTINGS
- A **Search** heading was added, and the keyword, kind and file format sit
  together inside a **white frame**
- When saving, the heading, name field and save button sit on a **grey
  plate**, so the field is not mistaken for the search box
- The Name and Search headings are bold and 2 pixels larger
- The current folder is shown on a **blue strip** above the list
- Folder names are bold, in a slightly brighter gold
- Row waveform previews are 1.5 times taller, with **pause, seek bar and
  vertical zoom** underneath (next section)

## Pause, frame stepping and vertical zoom for waveform previews

The generated-waveform preview (below the real-time waveform preview) is 1.5
times taller and has three new controls underneath. The rows of the file
picker have the same controls.

| Control | What it does |
| --- | --- |
| ▶ / ❚❚ | Pause and resume |
| Seek bar | One cycle is one frame. Click or drag to move; the mouse wheel steps one frame (and pauses). The middle shows "current frame / frames". Right-click to type a frame number (the total is shown beside it) |
| Zoom | Stretches the waveform vertically, x1 to x8, without changing the height of the view |

Use them to look closely at the waveform at one moment. For the still pictures
of wave and audio files, only the zoom applies.
