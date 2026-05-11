# Retro Sound Module Style VST "2686V"

(C)2026 CYROSS

**Please refer to `README.md` for the Japanese version.**

## Introduction

"2686V" (hereinafter collectively referred to as "this software") provide two plugins: "2686V", an instrument plugin designed to produce retro game sound module (FM/SSG/ADPCM, etc.) `-style` sounds `as close as possible` `(hopefully)` to the vibe of the game music I played the heck out of in the 80s and 90s, and "2686VFX", an effect plugin that converts your audio into retro game module `-style` sounds.

## Objective

I wanted to create tracks simulating the "PC-9801-26" and "PC-9801-86" sound boards within my DAW, but I couldn't find any existing plugins that met my expectations, so I decided to build my own... or so I thought...

## Overview

* **2686V**: My Ultimate Retro Game Sound Module VST Plugin
* Considering that a 100% perfect reproduction was impossible with my tech stack, and wanting to increase the freedom of sound design, I decided to create a plugin that provides just what is necessary and sufficient to evoke the atmosphere of that era, rather than strictly imitating existing hardware.
* Therefore, I have zero intention of pursuing exact accuracy while constantly comparing it to actual hardware sounds (Sorry!).

* Also, since my main goal was "to make music," the code was almost entirely implemented by `Google Gemini 3`. My main role was operation checking and reviewing.

## Features

* **Supports numerous FM sound module operators (style)**
* OPL / OPL3 / OPM / OPN / OPNA.
* Wide range of parameter values.
* Floating-point support.


* Applying parameter values via MML-style notation.
* Operator masking support.
* Sound effect (SE) mode applicable to all operators.
* (OPL/OPM) LFO parameters separated for global vs. individual operators.
* **Original FM Operator "OPZX3" Support**
* An operator incorporating OPZ operators, OPX algorithms, and MA-3 (SD-1) source waveforms.
* Since there are many unknown parts (especially regarding the OPX algorithms), this is an approximate implementation.


* Furthermore, it includes a mode to use PCM sound sources (audio files) as operator waveforms, similar to the OPX.
* Moreover, by including numerous original waveforms recommended by Gemini, **over 60 types** of waveforms are available.


* (OPNA/OPM/OPZX3) Reorganization of hardware LFO specifications and addition of Software LFOs.
* Separated and implemented global (entire synth) LFOs and channel-specific LFOs, which were previously mixed up in hardware specs.

* **SSG (style sound module) Support**
* An enhanced version incorporating PSG features, rather than a perfect reproduction.
* Pulse waves support duty cycles not only for SSG but also for Famicom (NES) and KONAMI VRC VI.
* Phase inversion capability.


* **Wavetable (style sound module) Support**
* Supports multiple bit rates and 32/64/128/256 samples.
* Preset wavetable memories.


* **Rhythm Sound Module (style) Support**
* Drum machine with a built-in bitcrusher.
* Adjustable bit rate and sampling rate.
* Selectable between linear PCM and ADPCM when in 4-bit mode.


* Includes 8 pads (expanded from the 6 pads in the 2608B).
* Audio file assignment support for each pad.
* MIDI CC assignment support.




* **ADPCM (style sound module) Support**
* Can play a single audio file like a sampler.
* Adjustable bit rate and sampling rate.
* Selectable between linear PCM and ADPCM when in 4-bit mode.




* **BEEP Sound Support**
* Two modes available: musical scale playback mode and specified fixed frequency mode.
* ADSR envelope support.


* **Independent Effects Support**
* Included effects:
* Filter
* Vibrato
* Tremolo
* Reverb
* Delay
* Modern Bit Crusher


* Bypass support for individual FX and global bypass.


* **Master Volume Support**
* Default: -3.0dB.


* **Value Range Check**
* Hovering over a slider button displays its range.
* Can be toggled ON/OFF in the settings screen.


* **Polyphonic Playback**
* Up to 32 simultaneous voices.
* Adjustable headroom gain.


* **Automation Support** (Of course!)
* **MIDI Keyboard Pitch Bend & Modulation Wheel Support**
* **Preset Save/Load Support**
* Saves ADPCM/Rhythm audio files with relative directory paths.


* **Custom Wallpaper Support**
* **Waveform Preview Feature**
* Static timbre preview.
* Real-time preview.


* **Virtual MIDI Keyboard Support**

## Target OS

* Windows 10 (x64) or later
* Mac is... sorry! (Not supported yet)
  * We recently received a report that the plugin was successfully built on macOS using `clang++` with just a few minor modifications. Huge thanks!
    * A special shout-out and thank you to `atsushieno`!
  * The suggested fixes have been incorporated into the v1.2.0 source code.

## Plugin Format & Version

* VST3 (3.8)

## Operation Manual

* Provided via `GitHub Wiki`.
* [2686V Operation Manual](https://github.com/cyross/2686V_2686VFX/wiki/2686V-2686VFX-%E3%82%AA%E3%83%9A%E3%83%AC%E3%83%BC%E3%82%B7%E3%83%A7%E3%83%B3%E3%83%9E%E3%83%8B%E3%83%A5%E3%82%A2%E3%83%AB)
* **However, it is barely finished, so please be patient... (Sorry!)**
* That being said, you should be able to figure things out intuitively just by looking at the UI...

## Distribution of Compiled VST3 Files

* Planned to be distributed via `GitHub` Releases or `Booth`.

## Build Instructions

### Prerequisites

* `Microsoft Visual Studio 2026 Community` or later
* An environment where `Developer PowerShell` or `Developer Command Prompt` can be launched.
* Command-line tools like `CMake` must be installed.


* `Git`
* Ensure it is added to your PATH.
* Ensure you have access to GitHub.



### Step 1: Open the Terminal

* Launch `Developer PowerShell` or `Developer Command Prompt`.

### Step 2: Checkout 2686V_2686VFX

* In the terminal, navigate to a suitable directory (e.g., your project management directory).
* Run the following commands:

```bash
cd /path/of/root
git clone git@github.com:cyross/2686V_2686VFX.git 2686V_2686VFX

```

* *Note: From here on, the `[your_directory]/2686V_2686VFX` path will be referred to as the **Project Root**.*
* Navigate to the project root:

```bash
cd 2686V_2686VFX

```

### Step 3: Check & Update Project Version

* Once cloned, check the project branch.
* You can switch to a specific version here if needed.

```bash
git branch # Ensure the branch is "main"
git switch -d vx.y.z # Switch to a specific version if necessary

```

### Step 4: Update Submodules

* Next, update the submodules to their latest versions.
* Ensure your current directory is the Project Root.
* Run the following commands in the terminal:

```bash
cd ./external/JUCE
git submodule update --init --recursive
git switch -d [Latest JUCE version number, e.g., "8.0.12"]
cd ../vst3sdk
git submodule update --init --recursive
git switch -d [Latest vst3sdk version number, e.g., "v3.8.0_build_66"]

```

### Step 5: CMake Build for Visual Studio 2026

* Open Visual Studio 2026 and select "Open a local folder".
* When the folder selection dialog opens, select the Project Root and click "OK".
* Once the Visual Studio workspace is open, select "Build 2686V Configuration" from the "Project" menu.
* The configuration build is complete if it finishes without errors.

### Step 6: Edit & Build within the Solution

* In the "Solution Explorer", select `2686V_Bundle.sln`.
* You can now edit the source code or build the project to generate the VST3 files and standalone `.exe` files.

## Future Issues

* Thorough refactoring.
* Documentation improvements.

## License

The source code in this repository is available under the MIT License (see the `LICENSE` file in the repository for details).

However, the compiled binary files (VST3 plugins) are subject to the **GPLv3 (GNU General Public License v3)**. This is because this software links against the "JUCE" framework and the "VST3 SDK", inheriting their licenses (GPLv3/AGPLv3).

**Prohibitions:** You may not misrepresent the authorship of this software or any material files included within it, nor may you sell it as a commercial product without the author's permission (modification or removal of copyright notices under GPLv3 provisions is not permitted).

## Terms of Use / Disclaimer

* **[Regarding use in music production]** Audio works (music, sound effects, etc.) created using this software can be used freely and royalty-free, regardless of whether for commercial or non-commercial purposes. The copyright of the generated/created works belongs to the creator (the user).
* **[Disclaimer]** This software is provided "as is". The author shall not be held liable for any damages, disadvantages, or troubles arising from the use of this software.
* **[Trademarks]** VST is a trademark of Steinberg Media Technologies GmbH.

### vst3sdk Redistribution License

The vst3sdk license can be found at `external/vst3sdk/LICENSE.txt`.

### JUCE Redistribution License

The JUCE license can be found at `external/JUCE/LICENSE.md`.

## Contact

Please feel free to contact me at the following:

* `cyross4github@gmail.com`

## SNS & Links

* [X (Twitter)](https://x.com/cyross4fortnite)
* [YouTube](https://www.youtube.com/@%E3%81%95%E3%81%84%E3%82%8D%E3%81%99)
* [Nico Nico Douga](https://www.nicovideo.jp/user/3223933)
* [Piapro](https://piapro.jp/cyross_p)
* [Booth](https://cyross.booth.pm/)

## References

[Please Check Here](https://github.com/cyross/2686V_2686VFX/wiki/%E5%8F%82%E8%80%83%E6%96%87%E7%8C%AE)

## Changelog

[Please Check Here](https://github.com/cyross/2686V_2686VFX/wiki/%E6%9B%B4%E6%96%B0%E5%B1%A5%E6%AD%B4)
