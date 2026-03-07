プラグインの完成、本当におめでとうございます！ここまで一緒に開発を進められて、私も本当に楽しかったですし、光栄に思います。（READMEの中で私の名前も挙げていただき、AI冥利に尽きます…！）

最新のREADMEの熱意とユーモア（「ぼくのかんがえたさいきょうの〜」や「気長にお待ち下さい…」などのニュアンス）を損なわないよう、英語圏のDTM・プラグイン開発界隈で自然に伝わるトーンで翻訳いたしました。
`readme.en.md` 用として、以下のテキストをご活用ください！

---

# Retro Sound Module Style VST "2686V" + Retro Sound Effect VST "2686VFX"

(C)2026 CYROSS

**Please refer to `README.md` for the Japanese version.**

## Introduction

"2686V" and "2686VFX" (hereinafter collectively referred to as "this software") provide two plugins: "2686V", an instrument plugin designed to produce retro game sound module (FM/SSG/ADPCM, etc.) `-style` sounds `as close as possible` `(hopefully)` to the vibe of the game music I played the heck out of in the 80s and 90s, and "2686VFX", an effect plugin that converts your audio into retro game module `-style` sounds.

## Objective

I wanted to create tracks simulating the "PC-9801-26" and "PC-9801-86" sound boards within my DAW, but I couldn't find any existing plugins that met my expectations, so I decided to build my own... or so I thought...

## Overview

* **2686V**: My Ultimate Retro Game Sound Module VST Plugin
* Considering that a 100% perfect reproduction was impossible with my tech stack, and wanting to increase the freedom of sound design, I decided to create a plugin that provides just what is necessary and sufficient to evoke the atmosphere of that era, rather than strictly imitating existing hardware.
* Therefore, I have zero intention of pursuing exact accuracy while constantly comparing it to actual hardware sounds (Sorry!).


* Also, since my main goal was "to make music," the code was almost entirely implemented by `Google Gemini 3`. My main role was operation checking and reviewing.


* **2686VFX**: A plugin that allows you to use the effect features of "2686V" on any track in your DAW.
* Simply because I thought it would be fun.



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
* Supports multiple bit rates and 32/64 samples.
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
* Software LFO
* Vibrato
* Tremolo
* Reverb
* Delay
* Modern Bit Crusher
* Retro Bit Crusher
* Soft Clipper


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

* `Microsoft Visual Studio Code`
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
cd base
git submodule update --init --recursive
cd ../cmake
git submodule update --init --recursive
cd ../doc
git submodule update --init --recursive
cd ../pluginterfaces
git submodule update --init --recursive
cd ../public.sdk
git submodule update --init --recursive
cd ../tutrials
git submodule update --init --recursive
cd ../vstgui4
git submodule update --init --recursive

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

* "Machine Language Sound Programming" (マシン語サウンドプログラミング)
* By Manabu Aoyama / Toru Hidaka
* ASCII Publishing (1994)


* "PC-9801-86 Sound Board User's Manual" (PC-9801-86サウンドボード ユーザーズマニュアル)
* Edited by NEC
* NEC (1993)


* "Inside X68000"
* By Masahiko Kuwano
* SoftBank (1992)


* "MA-5 Authoring Tool User Manual (ATS-SMAFPhraseL2 Edition)" (MA-5 オーサリングツール ユーザーマニュアル)
* Edited by Yamaha Corporation
* Yamaha Corporation (2006)


* [Wikipedia: Yamaha YM2414](https://en.wikipedia.org/wiki/Yamaha_YM2414)
* [DTM Station: YMF825 Review](https://www.dtmstation.com/archives/52000321.html)
* [MAME: ymf271.cpp](https://git.libretro.com/libretro/mame/-/blob/f3218cc336813f24adf720e7c48b544e7bfc2d0e/src/devices/sound/ymf271.cpp)
* [OPL3 Chip Analysis Document](https://moddingwiki.shikadi.net/wiki/OPL_chip)
* [Yamaha V50 Owner's Manual](https://jp.yamaha.com/support/manuals/index.html?l=ja&k=V50)

## Changelog

### 2026.03.07

* v1.0.0
* <2686V><2686VFX> GUI: Adjusted spacing between parameters.
* <2686V><2686VFX> GUI: Changed height between parameters.
* <2686V> Added MA-3 algorithms to OPZX3 algorithms.
* <2686V> Added FB2 to OPM/OPZX3.
* <2686V> Added playback offset and playback length ratio settings to OPZX3 / RHYTHM / ADPCM.
* <2686V> Changed algorithm notation for OPNA / OPN / OPL / OPL3 / OPM / OPZX3.
* <2686V> Deactivated unused OPs in OPZX3 depending on the selected algorithm.
* <2686V> ***BREAKING CHANGE*** Changed the end of the automation name for each FM synth algorithm from `_ALGORITHM` to `_ALG`.
* <2686V> ***BREAKING CHANGE*** Changed the end of the automation name for each FM synth feedback from `_FEEDBACK` / `_FEEDBACK2` to `_FB1` / `_FB3`.
* <2686V> Changed feedback notation for OPNA / OPN / OPL / OPL3 / OPM / OPZX3 to `FB1`, `FB1/2`, `FB3`.
* <2686V> Fixed an issue in OPZX3 where MA-3 series algorithms produced unexpected waveforms.
* <2686V> Significantly increased the number of waveforms available in OPZX3.
* Restored waveforms previously omitted in v0.7.0.


* Started using Wiki as the manual.
* Other minor fixes.

### 2026.03.05

* v0.7.0
* <2686V><2686VFX> UI modifications.
* <2686V> Added PMS/PMD/AMS/AMD to per-OP LFO settings in OPL/OPL3.
* <2686V> Revised OPZX3 waveforms to the correct shapes, as they were deviating from the actual MA-3 ones.

### 2026.03.02

* v0.6.0
* <2686V><2686VFX> Added "Software LFO" to the FX tab.
* <2686V> Expanded the preset scanning range to include subdirectories.
* <2686V> Categorized settings display.

### 2026.03.01

* v0.5.0
* <2686V> Fixed bugs related to SSG and WS.
* <2686V> Added Fixed Waveform Mode to BEEP.

### 2026.02.28

* v0.4.0
* <2686V> Eradicated bugs in the FM sound module section.
* Resolved an issue where noise would occur depending on settings or key velocity.


* <2686V> (OPL/OPM/OPZX3) Separated LFO parameters for all OPs vs. individual OPs.
* <2686V> (OPM/OPZX3) Added PMD and AMD to global LFO parameters.
* <2686V> (OPZX3) Adjusted playback speed when loading PCM.
* <2686V> Changed DT/DT1 notation to -3 to 3.
* Internally remains 0 to 7.


* <2686V> Focus now automatically shifts to the text box when the MML button is clicked.
* <2686V><2686VFX> Added two effects to the FX tab:
* Filter
* SoftClipper(tanH)


* <2686VFX> Added waveform preview.
* <2686V> Enabled waveform preview across all tabs.
* <2686V> Changed waveform preview to display by default.
* <2686V> Added `Last Modified` column to the PRESET tab.
* <2686V> Added `BEEP` tab.
* Converted Operation Manual to PDF (included in the distribution package).
* Minor bug fixes.

### 2026.02.24

* v0.3.0
* Fixed numerous audio output bugs.
* Especially Rhythm and ADPCM modules.


* <2686V> Added two types of waveform previews.
* <2686V> Added Virtual MIDI Keyboard.
* <2686V> Added search function to the Preset tab.
* Adjusted the UI of the ABOUT section.
* Added logo.
* Manual revisions.
* Minor bug fixes.

### 2026.02.23

* v0.2.0
* <2686V> Removed Feedback2 from OPM/OPZX3 parameter settings.
* <2686V> Revised OPZX3 algorithms.
* <2686V> Added the ability to load any PCM file to each OP of OPZX3 (treated as a WS extension feature).
* <2686V> Added the following features to custom wavetable memory:
* Hovering and dragging on the wavetable edit screen now displays values via tooltip.
* Added buttons to change individual sliders to -1.0, -0.5, 0.0, 0.5, 1.0.
* Added buttons to change all sliders to -1.0, 1.0.


* Changed Master Volume default value to -3.0dB.
* Refactoring Phase 2:
* Major modularization.
* Separation of constants and structures.


* Fixed a parameter setting bug.
* Manual revisions.
* Minor bug fixes.

### 2026.02.19

* v0.1.0
* <2686V> Added Noise On Note mode to SSG.
* <2686V> Changed SSG Noise Freq range to 0 - 30000.
* Overall UI changes/compacting.
* Fixed some parameter setting bugs.
* Refactoring Phase 1.
* Manual revisions.
* Minor bug fixes.

### 2026.02.16

* v0.0.3
* <2686V> Added comment field to presets.
* <2686V> Sanitization support for preset metadata.
* <2686V> Length limitation support for preset metadata.
* <2686V> Added TL to OPL.
* <2686V> Unified SR/D2R values to 0.0 - 10.0 across all modules.
* <2686V> Fixed a bug where parameter value ranges were incorrect.
* Changed OPM/OPZX3 automation parameters to their original hardware names:
* DR -> D1R, SL -> D1L, SR -> D2R


* *Note: Presets created in older versions may no longer load correctly.*


* <2686V> Fixed bugs in MML mode.
* <2686V> Bundled Standalone version.
* <2686VFX> Reduced file size.
* Changed build environment setup method.
* Added sample presets.


* v0.0.2
* <2686V> Added Headroom settings.

### 2026.02.15

* v0.0.1
* <2686V> Initial Release.
* <2686VFX> Initial Release.