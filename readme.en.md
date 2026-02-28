# Retro-Style VST "2686V" + Retro-Style Effect VST "2686VFX"

(C) 2026 CYROSS

**This document is translated by `Google Gemini3`**

## Introduction

"2686V" and "2686VFX" (collectively referred to as "this software") are plugins designed to evoke the sound of classic retro sound chips.

2686V: A synthesizer plugin generating retro-style sounds (FM, SSG, ADPCM, etc.).

2686VFX: An effect plugin that transforms any audio track into a retro-style sound.

## Motivation

I wanted to create music on my DAW that perfectly replicated the "PC-9801-26" and "PC-9801-86" sound boards. However, I couldn't find any tools that met my specific standards. So, I decided to build my own... or at least, that was the plan.

## Overview

### 2686V

"My Ultimate Retro Sound Source VST Plugin."

Since perfectly replicating the original hardware was beyond my current technical stack, I focused on creating a plugin that effectively emulates various sound chip characteristics.

As my primary goal was "making music," the code was largely implemented by `Google Gemini 3`. I focused on the core architecture, testing, and reviewing.

### 2686VFX

An effect version of the features found in 2686V, allowing them to be applied to other tracks—simply because I thought it would be interesting.

## Features

### FM Synthesis (Emulated)

- Support for various FM operators: OPL, OPL3, OPM, OPN, OPNA.
- Flexible Parameter Ranges: Supports floating-point values.
- MML Support: Reflect parameter values via MML notation.
- Advanced Control: Operator masking and "Sound Effect Mode" applicable to all operators.
- Original Operator "OPZX3":
- A hybrid operator combining elements from OPZ (operators), OPX (algorithms), and MA-3/SD-1 (base waveforms).
- Includes approximate implementations of more mysterious elements (especially OPX algorithms).
- Supports wide parameter ranges, MML-style notation, masking, and Sound Effect Mode.

### SSG (Emulated)

- An enhanced version that incorporates PSG features rather than a strict replication.
- Pulse Waves: Supports duty cycles for SSG, NES, and KONAMI VRC VI.
- Phase Inversion support.

### Wavetable (Emulated)

- Supports multiple bitrates and 32/64 sample lengths.
- Includes preset wavetables.

### Rhythm Sound Source (Emulated)

- Drum machine with built-in bitcrusher (adjustable bitrate and sampling rate).
- 4-bit Mode: Selectable between Linear PCM and ADPCM.
- 8 Pads: Expanded from the original 2608B's 6 pads. Supports file assignment and MIDI CC mapping.

### ADPCM (Emulated)

- Sampler-style playback of a single audio file.
- Adjustable bitrate/sampling rate. 4-bit mode selectable between Linear PCM and ADPCM.
- Independent Effects
  - Vibrato, Tremolo, Reverb, Delay, Modern Bitcrusher, and Retro Bitcrusher.
- Bypass support for each.

## General Specs

- Master Volume: Default -6dB.
- Value Range Tooltips: Hover over slider buttons to see parameter ranges (toggleable in settings).
- Polyphony: Up to 32 voices with adjustable headroom gain.
- Full Automation support.
- MIDI Control: Supports Pitch Bend and Modulation.
- Preset System: Save/Load presets with relative directory paths for ADPCM/Rhythm samples.
- Customization: Supports custom wallpaper settings.

## System Requirements

- OS: Windows 10 (x64) or later.
- macOS: Not supported (sorry!).
- Format: VST3 (version 3.8).

## Manual

Operations manual provided via Google Docs:

2686V Operation Manual

Note: The manual is currently under construction. Please be patient! (The UI is designed to be intuitive enough to use without it).

## Distribution

Builds will be available on GitHub Release Notes or Booth.

## Build Instructions

### Prerequisites

Microsoft Visual Studio 2026 Community or later.

Developer PowerShell or Developer Command Prompt.

CMake and other CLI tools installed.

Git (with Path configured and GitHub access confirmed).

### Step 1: Open Terminal

Launch your Developer PowerShell or Command Prompt.

### Step 2: Clone the Repository

Navigate to your project directory and run:

```bash
cd /path/of/root
git clone git@github.com:cyross/2686V_2686VFX.git 2686V_2686VFX
cd 2686V_2686VFX
```

### Step 3: Check Branch/Version

```bash
git branch # Confirm you are on "main"
git switch -d vx.y.z # Optional: Switch to a specific version
```

### Step 4: Update Submodules

Ensure all external libraries are up to date. From the project root:

```bash
# Update JUCE
cd ./external/JUCE
git submodule update --init --recursive
git switch -d [Latest JUCE Version, e.g., "8.0.12"]

# Update VST3 SDK and components
cd ../vst3sdk
git submodule update --init --recursive
git switch -d [Latest SDK Version, e.g., "v3.8.0_build_66"]
cd base; git submodule update --init --recursive
cd ../cmake; git submodule update --init --recursive
cd ../doc; git submodule update --init --recursive
cd ../pluginterfaces; git submodule update --init --recursive
cd ../public.sdk; git submodule update --init --recursive
cd ../tutrials; git submodule update --init --recursive
cd ../vstgui4; git submodule update --init --recursive
```

### Step 5: CMake Build for VS2026

Open Visual Studio 2026 and select "Open a local folder".

Select your Project Root.

Go to the "Project" menu and select "Configure 2686V".

Wait for the configuration to complete without errors.

### Step 6: Edit & Build

Find `2686V_Bundle.sln` in the Solution Explorer and open it.

You can now edit the source and build the VST3 or Standalone EXE files.

## Roadmap

- Thorough refactoring.
- Completing the documentation.

## License

The source code is released under the MIT License.

However, the compiled binary files (VST3 plugins) are subject to the GPLv3 (GNU General Public License v3). This is because the software links against the JUCE framework and the VST3 SDK, inheriting their respective licenses (GPLv3/AGPLv3).

Prohibited: Redistributing this software or its included assets while misrepresenting the author, or selling it as a commercial product without authorization, is strictly prohibited. (Modification or removal of copyright notices as per GPLv3 is not permitted).

## Permissions & Disclaimer

Usage in Music: You are free to use the audio (music, sound effects, etc.) created with this software for both commercial and non-commercial purposes, royalty-free. The copyright of the generated audio belongs to the user.

Disclaimer: This software is provided "as is". The author shall not be held liable for any damages, disadvantages, or troubles resulting from its use.

Trademarks: VST is a trademark of Steinberg Media Technologies GmbH.

## Contact & Socials

Email: `cyross4github@gmail.com `

- [X](https://x.com/cyross4fortnite)
- [Youtube](https://www.youtube.com/@%E3%81%95%E3%81%84%E3%82%8D%E3%81%99)
- [Nico Nico](https://www.nicovideo.jp/user/3223933)
- [Piapro](https://piapro.jp/cyross_p)
- [Booth](https://cyross.booth.pm/)

## References


- Machine Language Sound Programming by Manabu Aoyama / Toru Hidaka (ASCII, 1994)
- PC-9801-86 Sound Board User's Manual (NEC, 1993)
- Inside X68000 by Masahiko Kuwano (Softbank, 1992)
- [Wikipedia: YAMAHA YM2414](https://en.wikipedia.org/wiki/Yamaha_YM2414)
- [YAMAHAのFM音源チップ、YMF825がMAKE市場向けに登場。Maker Faireで先行発売が決定だ！](https://www.dtmstation.com/archives/52000321.html)
- [MAMEのFM271部分](https://git.libretro.com/libretro/mame/-/blob/f3218cc336813f24adf720e7c48b544e7bfc2d0e/src/devices/sound/ymf271.cpp)
- [OPL3解析ドキュメント](https://moddingwiki.shikadi.net/wiki/OPL_chip)

## Change Log

(Latest updates shown)

### 2026.02.26 (v0.4.0)

- <2686V> Fixed FM engine bugs causing noise based on velocity/settings.
- <2686V> (OPNA/OPM/OPZX3)Separated LFO parameters into global (all operators) and per-operator settings.
- <2686V> Changed DT/DT1 display range to -3 to 3 (internally 0–7).
- <2686V> Clicking the MML button now focuses the text box.
- <2686V/VFX> Added "Filter" and "SoftClipper (tanH)" to the FX tab.
- <2686VFX> Added waveform preview.
- <2686V> Waveform preview is now visible on all tabs and enabled by default.

(Previous logs omitted for brevity in this display)
