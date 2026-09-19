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

- **86V's N88 LFO file had not been moved to the 3.0.0 format.** It could only read
  and write the old line-based format, so it could not load N88 LFO files exported
  from 2686V and the other plugins. It now uses the same format as the rest of the
  family; old-format files are read and then rewritten in the new format.
- In **2686VFX**, the PCM bit crusher row in the Effects frame's order list had no name.
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
- The N88 LFO screen is now a component like the other sections (no visible change).
- Added tests that drive the real editor and processor to check that controls are
  disabled with their switches and that key assignments trigger the right targets.
