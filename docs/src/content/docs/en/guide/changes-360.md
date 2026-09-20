---
title: What changed in v3.6.0
description: What's new and changed in 3.6.0
sidebar:
  order: 4
---

This page covers changes from 3.5.0 to 3.6.0. **Your files remain compatible.**
Presets and parameter files made in 3.5.0 or earlier can be loaded without changes.
Every new setting defaults to the previous behavior, so opening an existing
project does not change its sound.

## 2686VFX: choose which keys drive the modulation

**You can now choose, per target, which key drives each envelope and LFO.**
For example, assign AMP ENV to C3, PITCH ENV to D3 and LFO AM to E3, and you can
switch between the modulations just by playing different keys.

A **Key assign** selector has been added to the Effects frame, above the file
import/export buttons.

| Choice | Behavior |
| --- | --- |
| **Single key** (default) | Same as before. Any key drives everything |
| **Customize** | Each target only responds to its own key. A list of keys appears below |

There are 13 targets. Keys range from C-2 (0) to G8 (127), and every target
defaults to **C3 (60)**.

| Target | What its key does |
| --- | --- |
| AMP ENV / SSG HW AMP ENV / WT AMP MOD / SSG SW AMP ENV[11] | Starts on key down, releases on key up (same as before) |
| PITCH ENV / SSG HW PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD | Starts on key down, releases on key up (same as before) |
| LFO AM / LFO PM | Applied **only while the key is held**. Restarts from the top on key down |
| MUL/DET | Applied **only while the key is held** |
| UNISON/HARMONY | Applied **only while the key is held** |
| ARPEGGIO | Arpeggiates **only while the key is held**. Starts from the first voice on key down |

- The same key can be assigned to several targets; pressing it drives all of them.
- In Customize mode, the colour of each modulation panel's title shows whether it
  is active: grey while its key is not held, off-white while it is. The LFO panel
  lights up for either the AM or the PM key, and the UNISON/HARMONY panel for either
  the UNISON or the ARPEGGIO key. In Single key mode every title stays off-white,
  as before.
- When several keys are held, everything assigned to each of them runs.
- Targets are processed in the same order as before.
- In Single key mode, LFO, MUL/DET and UNISON/HARMONY still apply regardless of
  the keys, as before. They only become "while held" in Customize mode.
- Assignments are saved in the FX parameter file (`.2fx`). Older files without a
  key assignment leave the current assignment untouched.
- The automation parameters are `MOD_KEYASSIGN_MODE` and `MOD_KEY_*` (one per target).

See [Modulating the output](/2686V_2686VFX/en/fx/mod/#key-assign--which-key-drives-the-modulation) for details.

## Controls in a switched-off section are now disabled

**When a section's Bypass or Enable switch has it turned off, the controls inside
are now disabled and dimmed**, so you no longer tweak controls that have no effect
on the sound. The switch itself always stays clickable.

| Section | What gets disabled |
| --- | --- |
| AMP ENV / PITCH ENV | Every control inside while Bypass is on |
| SSG HW AMP ENV / SSG HW PITCH ENV | The controls and the waveform preview while Bypass is on |
| SSG SW AMP ENV / SSG SW AMP ENV[11] / SSG SW PITCH ENV[11] | Every control inside while Bypass is on |
| N88 LFO / OPZX7 LFO / OPM LFO | The PM side follows PM Enable, the AM side follows AM Enable. Shared controls (speed, shape, sync) stay enabled while either one is on |
| FIX | The controls and separators while Enable is off |

Switching the TARGET or loading a preset updates this to match the switch of the
new target.

## INTERP reached the non-PCM QUALITY

**The FM, SSG and wavetable channels can now choose how the gaps are filled
(INTERP).** The chip builds its waveform at the rate you pick with SMP.RATE, and that
waveform is then brought up to the host rate; this setting decides how the space
between those points is filled.

The 7 choices are the same as the PCM INTERP.

**Each channel starts on whatever matches 3.6.0 and earlier**, so nothing changes just
by opening a patch.

| Channel | Default | How it used to be played |
| --- | --- | --- |
| FM (OPNA / OPN / OPL / OPL3 / OPM / OPZX7) and SSG | Linear | Linear interpolation |
| Wavetable (WT / WT+) | ZOH (zero-order hold) | Stepped, held |
| WT2 | Linear | Linear interpolation |

WT2 alone starts on a different setting from the rest of the wavetable family. Its
QUALITY never reached the chip before 3.6.0 (see "Fixes" below), so it was played with
linear interpolation.

**ZOH is the closest to the hardware**, which puts out its staircase as it is, so the
grit of a low rate stays. B-spline and Lagrange go the other way and cut the imaging a
lot (playing A4 at 8kHz: 46.9 dB with Linear, 97.3 dB with B-spline).

- Gaussian, B-spline and Lagrange need the next point, so those three delay the sound
  by one chip sample (18 microseconds at 55.5kHz, 0.45 milliseconds at 2.21kHz)
- The setting is saved in the channel parameter file; older files without it leave the
  current value alone
- The automation ID is `<channel>_INTERP`

## Noise reduction for PCM QUALITY

**The QUALITY section of ADPCM / ADPCM+ / RHYTHM gains noise reduction settings.**
All of them are off by default, so existing sounds do not change.

| Control | Description |
| --- | --- |
| **NR: Resample** | Uses a clean (windowed-sinc) resampler when converting to a compression format (BIT 13-21). Only available while a compression format is selected |
| **NR: Gate** | Removes faint noise in near-silent passages, and removes DC offset |
| **GATE.LV** | The level (dB) where the gate closes. -96 to -24, default -60. Only available while NR: Gate is on |
| **NR.LPF** | Cuts the high end to tame the grit. Off / Light / Medium / Strong |

We also re-measured the noise. Plain bit reduction (BIT 1-12) produces the
theoretically expected amount. The compression formats, however, used a crude
decimation when converting, which capped the signal-to-distortion ratio at about
28 dB regardless of the other settings. Turning on **NR: Resample** raises it to
about 90 dB.

- The settings are saved in the QUALITY file and in CH Params.
- The RHYTHM pad copy (COPY / PASTE) now also carries INTERP and the noise
  reduction settings. Previously it only copied BIT and RATE.

## Thicker scroll bars

The scroll bars in the channel tabs are thicker and easier to grab:
vertical from 8 px to 10 px, horizontal from 8 px to 12 px.

## Fixes

- **The playing lamp could stay lit.** The lamp at the bottom left was driven by
  whether a voice was alive. A voice lives until its envelopes finish, so **with a slow
  release the lamp stayed on for seconds after the sound had gone** (5 seconds with an
  OPNA patch whose RR is 0). It now follows **the sound itself**, and goes out 0.15
  seconds after the sound stops.
  It also did not go out in these cases.
  - When **All Notes Off (CC123) or All Sound Off (CC120) arrived**, which is what a
    DAW sends when it stops the transport. The synth handles that message directly,
    without going through note-off, so the keys were left marked as held.
  - When **you switched channel tabs**. Switching stops the previous channel's sound,
    but the keys were still marked as held. Switching is now treated as releasing
    every key.
  - When **PANIC was pressed**. It only detached the voices; the chip was never told
    the key had been released, so the lamp stayed lit although the sound had stopped.
  Releasing one key of a chord also used to mark everything as released internally;
  that is fixed too.
- **2686VFX's playing lamp never lit.** Because the plugin makes no sound of its own,
  it always reported idle. It now lights while a key is held and while the modulation
  envelope is running.
- **WT2's QUALITY never reached the chip.** Whatever you picked for WT2's BIT and
  SMP.RATE made no difference to the sound, because it was handed to WT's side
  internally. It always played at **BIT 7-bit / SMP.RATE 55.5kHz**. The controls now
  do what they say.
  When a preset or a DAW session saved before 3.6.0 is loaded, **the two are set back
  to the 7-bit / 55.5kHz that were actually playing**, so it sounds exactly as it did,
  and the panel now shows what was really being played. Saving again in 3.6.0 keeps
  those values.
- **86V's N88 LFO file had not been moved to the 3.0.0 format.** It could only read
  and write the old line-based format, so it could not load N88 LFO files exported
  from 2686V and the other plugins. It now uses the same format as the rest of the
  family; old-format files are read and then rewritten in the new format.
- Switching **HOLD** / **KEEP** (hold and partial playback) did not take effect right
  away. In SSG HW AMP ENV and SSG HW PITCH ENV, toggling HOLD did not update the
  waveform preview; in WT AMP MOD, WT PITCH MOD and others, turning KEEP on left START
  and END disabled. Both stayed stale until another control was touched.
- In **2686VFX**, the PCM bit crusher row in the Effects frame's order list had no name.
- In QUALITY and QUALITY(PCM), RATE 14 and 15 were **labelled 10% away from the rate
  they actually use**. "4kHz" is really 4410 Hz and "2kHz" is really 2205 Hz (44.1kHz
  divided by 10 and by 20), so the labels now read **4.41kHz** and **2.21kHz**. RATE 13
  is really 5551 Hz, so it now reads **5.55kHz** too. The rate itself is unchanged, so
  patches sound exactly as before.
- In QUALITY and QUALITY(PCM), the RATE choices "5.5kHz" and "4kHz" were numbered
  12 and 13 by mistake. They are now 13 and 14. Only the displayed numbers were
  wrong; the selected rate is unchanged.

## For developers

These changes only matter if you build from source.

- **Tests are no longer part of the default build.** The build preset decides
  whether they are built:

  | Build preset | Builds |
  | --- | --- |
  | `*-debug` | Plugins and tests |
  | `*-release` | Plugins only |
  | `*-release-tests` | Plugins and tests (new) |

  To build only the tests, pass `--target AllTests`.
- **The manual tools are out of the default build too.** `ParamDump` and
  `ParamDumpFx`, which regenerate the parameter lists for the manual, compile the
  plugin sources and the shared code all over again and took about four minutes of a
  Release build. Build them with `--target AllTools` when you need the lists.
- The N88 LFO screen is now a component like the other sections (no visible change).
- Added tests that drive the real editor and processor to check that controls are
  disabled with their switches and that key assignments trigger the right targets.
- **The two bugs fixed this time now have permanent tests.** Neither showed up in a
  build or a warning; you had to listen for them.
  - That each channel's QUALITY (BIT / SMP.RATE / INTERP) reaches that channel's chip.
    This catches the WT2 bug, where the values were written to WT's side.
  - The playing lamp: that it goes out when the sound stops, stays on while a chord is
    partly held, and goes out on All Notes Off, a channel switch and PANIC.

### Code that was identical in all twelve plugins now lives in one place

**Until now every plugin carried its own copy of the same code under
`<plugin>/Source/`.** That code now lives in shared libraries under `Shared/`,
which each plugin links against. It covers the generators, the effects, the chip
cores, the chip processors, and the GUI base and components. JUCE is built once
for all twelve as well (`cy_juce`).

| | Up to 3.5.0 | 3.6.0 |
| --- | --- | --- |
| Clean build | 12 min 33 s | **8 min 5 s** |
| Intermediate files (Release) | 2,201 files, 2.2 GB | **1,046 files, 0.9 GB** |

(Measured on the same machine, Release, all twelve plugins.)

Three rules hold the structure together.

- GUI components never touch the processor or the editor directly; they go
  through an interface (`Shared/Core/Gui/GuiHost.h`).
- Per-plugin differences (86V's six RHYTHM pads, OPZX7S's always-on curve, and so
  on) are passed in when the object is built, never switched with `#if`.
- The tabs (the chip screens, SETTINGS, PRESET and so on) stay per plugin. They
  are what makes each plugin itself, so they were deliberately left alone.

**Sound, parameters and saved files are unchanged.** For all twelve plugins the
parameter IDs, names, ranges and defaults were compared before and after and are
identical. Only the order in which a DAW lists the parameters changes, in
2686VLight, 26V and 86V; saved state and automation are keyed by parameter ID, so
nothing is affected.
