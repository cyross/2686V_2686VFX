---
title: What changed in v3.2.0
description: What 3.2.0 adds and what moved
sidebar:
  order: 5
---

The changes between 3.1.0 and 3.2.0. **Nothing stops loading.** Presets and
parameter files made up to 3.1.0 still read as they did.

## Finer steps on the controls

Every floating-point control moved in **steps of 0.01**. That was the default
of the framework underneath, not a choice made here: an envelope time of
0.0035 seconds could only be set to 0.00 or 0.01.

The step is now **0.0001**, and the readout drops trailing zeros, so `0.5`
still reads as `0.5` rather than `0.5000`.

Older files only ever held multiples of 0.01, so their values are unchanged
when read.

## A new screen for picking files

Until now this opened the operating system's file dialog. It could not tell
one kind of parameter file from another, and the only way to hear what was in
a file was to load it.

It is now a **list that covers most of the window**.

- **Filter by keyword, kind and file format.** While a keyword is typed, the
  search also reaches into the folders below
- **Click a heading to sort.** Each click cycles through unsorted, ascending
  and descending
- **Walk the folders.** The first row goes up to the parent, and folders can
  be created and deleted
- **See the waveform.** Parameter files are actually played and drawn; wave
  files and audio samples are drawn straight from their contents as a still
  picture
- **Click outside to close** without choosing anything

Rows are chosen and opened with a **double click** — folders included. One
click selects, two enters.

Parameters, presets and wave files **cannot go above the plugin's own
folder**: outside it there is nothing selectable to find. Audio samples are
not restricted, since those legitimately come from anywhere.

## A preview of the generated waveform

The channel's settings are **actually played** and drawn.

- It is built **only when the button is pressed**, never while you play
- It runs to **10 seconds at most**, and stops **0.5 seconds after the sound
  dies away**. Ten seconds of a short sound is a moment of movement followed
  by a flat line
- It is shown as **three lanes, L / M / R**, because a channel can be stereo
  or panned
- Pick 1, 2, 5 or 10 cycles. The window starts on a cycle boundary, so the
  waveform does not drift sideways
- What is built is kept in a file and not rebuilt next time. **Clear previews**
  in SETTINGS removes them all

## Waiting now looks like something

While something slow is running, **bars pulse over a dimmed screen**. The
longer jobs can be **cancelled**.

## Opening is much faster

Opening 2686V in a release build took **34.5 seconds**. The 1,990 bundled
presets were each read in full, every time, just to collect the names and
categories for the list.

An **index file** now holds that, and the full read happens in the background
after the window is up. Opening takes **1.1 seconds**.

The index is rebuilt when presets are added or removed. While that runs, the
progress is shown and it can be cancelled.

## More SSG software envelopes

SSG software envelopes were taken from FMP, FMP7 and PMD MML and are included
as parameter files.
