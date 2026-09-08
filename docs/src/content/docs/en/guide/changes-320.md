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

## Presets can be filtered by channel

A preset replaces the whole channel, so most of the time you only want to see
the ones for the chip you are working with. A **channel filter row** now sits
between the search box and the list on the PRESET tab.

A checkbox per channel would stretch the row and push the search box out of the
way, so it folds into **a combo box that picks the channel and a checkbox that
turns it on or off**. Whatever is currently on is listed to the right.

- **Only what is ticked is shown. With nothing ticked, nothing is filtered**
- **All on / All off** switch the whole set at once
- The combo box lists only the channels that plugin actually carries

The starting state depends on the plugin. **The narrower instruments start with
their own channels ticked.** 2686V and 2686VLight carry all thirteen, where
starting filtered would only get in the way, so they start with none ticked.

| Plugin | Ticked at first |
| --- | --- |
| 2686V / 2686VLight | (none) |
| 26V | OPN / SSG |
| 86V | OPNA / SSG / RHYTHM / PCM |
| OPZX7S | OPZX7S |
| OPNV | OPNA / OPN |
| OPLV | OPL / OPL3 |
| OPMV | OPM |
| WTV | WT / WT2 / WT+ |
| PCMV | RHYTHM / PCM |
| PULSEV | SSG / BEEP |

## Sorting and filtering now survive

Sorting the PRESET list by a column heading **went back to the original order
as soon as a preset was loaded**. The list is rebuilt every time it is
filtered, but the sort was only applied at the moment of the click. What you
sorted by is now remembered and re-applied after each rebuild, so the order
holds through searching, filtering and starring.

Sorting by the date or format column also **sorted by the wrong column** — the
column numbering was off by one. That is fixed.

The file picker likewise **cleared the keyword and the format filter every time
it opened**, which meant retyping them to look in the same place again. They
now carry over. The kind filter and the sort order already did.

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
