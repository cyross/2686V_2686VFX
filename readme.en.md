# Retro Sound VST "2686V" v3.0.0 README

(C)2026 CYROSS

## 1. Introduction

"2686V" (hereafter referred to collectively as "this software") provides retro sound (FM/SSG/ADPCM, etc.) style VST plugins: "2686V", "2686V Light", "2686V Origin", and "OPZX7S".

## 2. Purpose

I wanted to create tracks in my DAW that sounded like they were made on the "PC-9801-26" and "PC-9801-86" sound boards. Since I couldn't find anything that met my standards, I decided to build a plugin myself... or at least, that was the plan...

## 3. Overview

- **2686V**:
  - My dream ultimate retro sound VST plugin.
  - Since a 100% accurate reproduction was impossible with my current tech stack, I created a plugin that approximates the sound of various retro sound chips to the best of my ability.
  - Also, because my main goal was "to make music," the code during early development was almost entirely implemented with the help of `Gemini3`.
- **2686V Light**:
  - A lightweight version of 2686V.
  - Omitted the Curve Edit Mode from 2686V to reduce CPU load.
- **2686V Origin**:
  - A version of 2686V Light structured closer to the "PC-9801-86" hardware.
    - Channels are limited to OPNA, SSG, RHYTHM, and PCM only.
    - Renamed the "ADPCM" channel to "PCM" and changed its initial audio quality to "4-bit PCM".
    - Changed the number of RHYTHM channels to 6.
- **OPZX7S**:
  - A plugin positioned as a modern FM synthesis operator.
  - Extracted the OPZX7S channel and Curve Edit Mode from 2686V to reduce CPU load.
  - Applies Curve Edit Mode by default (the traditional linear mode is omitted).

## 4. Features

- **Enjoy a wide variety of retro game sound sources (channels)**
  - *Each sound source is categorized by a "channel".*
  - Channels can be switched via tabs.
  - **However, only 1 channel can be played simultaneously per plugin instance.**
    - Designed with the philosophy that routing and structuring are more flexible when handled via DAW tracks.
- **Numerous hardware-derived FM synthesis channels (OPNA/OPN/OPL/OPL3/OPM)**
  - Added extensions to various FM operators that (probably) don't break their original design philosophies.
  - OPL/OPL3/OPM/OPNA
    - Includes their respective hardware LFOs.
  - OPN/OPNA
    - Includes the software LFO used in N88-BASIC.
  - OPM
    - Added OPP key scale settings.
  - OPL3
    - Added custom algorithms (2OPx2).
  - Amp Envelope optional features built into operators:
    - KOR (Keep On Release)
      - Volume does not decay until a specified time after release.
      - Used when prioritizing the SSG software envelope.
    - XOF
      - Ignores RR and never decays infinitely.
      - Used when prioritizing the SSG software envelope.
    - Bypass
      - Bypasses the amp envelope.
      - Used when prioritizing the SSG software envelope.
  - Hardware features: Sound Effect (SE) mode and Operator Mask available per operator.
  - Software features: Pitch Envelope (3-tap/11-tap) and SSG Software Envelope (6-tap/11-tap) available per operator.
  - Special feature: MML input function available per operator.
- **Original modern-style FM synthesis channel: OPZX7S**
  - My ideal ultimate FM synthesis operator.
    - Combines the best of OPZ/OPX/MA-7/OPS along with original features.
      - Packed with the desire to "reproduce as much as possible, even if 100% reproduction is impossible due to many mysteries (especially OPX)," resulting in an approximate implementation.
  - Up to 8 operators.
    - Selectable OPX/OPZ/MA-7/OPS algorithms.
    - Selectable custom 1OP to 8OP algorithms.
  - Algorithm Matrix.
    - Flexibly customize algorithms from 1OP to 8OP.
    - Users can specify modulation and feedback routing via checkboxes.
    - Allows feedback settings impossible in traditional specs.
    - *Only one type of feedback value can be set.
  - Amp Envelope is MA-7 compliant.
    - Can also be set using real numbers via a switch.
  - Key Scale can be switched between MA-7/OPZ/OPS settings.
  - MUL is a combination of MA-7 and an extended OPZ version.
    - MA-7 compliant MUL selection.
    - Extended OPZ MUL value (MulRatio) settings, with a significantly increased upper limit.
  - DT includes OPZ's DT/DT2, plus an added DT3 for freely setting values.
  - Wide variety of selectable waveforms.
    - MA-7/OPZ waveforms + custom waveforms.
      - *I also included a fake MA-3 waveform that Gemini originally brought up (which looks nothing like the actual MA-7 waveform) because it seemed interesting.*
    - Audio files can be used as waveforms (OPX reproduction).
      - Supports playback start position and playback ratio.
    - Wavetable memory (`.wt`, `.wt2` files supported, MA-7 reproduction).
  - Powerful LFO.
    - Independent frequency/sync settings for AM/PM.
    - Wide range of D/S setting values.
    - Configurable per CH and per operator.
  - Can use OPNA hardware SSG envelope.
  - Software features: SE mode, operator mask, pitch envelope (3-tap/11-tap), and SSG software envelope (6-tap/11-tap) available per operator.
    - *Pitch envelopes and SSG software envelopes are detailed below.*
  - Special feature: MML input function available per operator.
- **SSG (PSG) Channel**
  - Not a perfect reproduction, but an enhanced version incorporating PSG features.
  - Provides square and triangle waves.
  - Pulse waves support not only SSG but also Famicom and KONAMI VRC VI duty cycles.
  - Arbitrary duty cycle settings are also possible.
  - Phase inversion capable.
  - Includes a switch to change to a Famicom-style waveform.
  - Triangle wave peak position can be adjusted.
    - This allows for creating sawtooth waves.
  - Custom LFO included.
  - SE mode, OPZX7S LFO, OPZX7S MUL/DT, pitch envelope (3-tap/11-tap), and SSG software envelope (6-tap/11-tap) included.
- **Wavetable Channel (WT/WT2)**
  - Provides preset wavetables and custom wavetables.
  - Includes WT (set with real numbers) and WT2 (set by discrete steps).
  - Custom wavetable memory supports 32/64/128/256 samples.
  - Various auxiliary buttons provided.
  - Supports value alignment via simultaneous key presses.
    - 0.01, 0.05, 0.1, 16 steps, 32 steps, 64 steps.
  - Wavetable import/export.
    - Uses `.wt`, `.wt2` files (text files).
  - Custom modulation included.
  - SE mode, OPZX7S LFO, OPZX7S MUL/DT, operator mask, pitch env, and SSG sw env included.
- **Rhythm Sound Channel (RHYTHM)**
  - Drum machine with a bitcrusher.
    - Multiple bit rates and sampling rates selectable.
      - Supports 4bit-ADPCM.
      - Supports 1bit-DPCM.
    - Supports playback start position, playback ratio, and loop points.
  - Includes 8 pads (6 in Origin).
    - Expanded to 8 pads (YM-2608B originally had 6).
      - Remains 6 in 2686V Origin to comply with the 86 sound board.
    - Supports audio file assignment per pad.
    - Custom MIDI key assignment support.
  - SE mode, OPZX7S LFO, OPZX7S MUL/DT, operator mask, pitch env, and SSG sw env per pad.
- **Audio File Channel (ADPCM/PCM)**
  - Play a single audio file like a sampler.
    - Multiple bit rates and sampling rates selectable.
      - Supports 4bit-ADPCM.
      - Supports 1bit-DPCM.
    - Supports playback start position, playback ratio, and loop points.
  - SE mode, OPZX7S LFO, OPZX7S MUL/DT, operator mask, pitch env, and SSG sw env included.
- **BEEP Channel**
  - Reproduces beep sounds (fixed duty cycle square wave).
    - SE mode includes a button to change to 2000Hz.
  - SE mode, OPZX7S LFO, OPZX7S MUL/DT, operator mask, pitch env, and SSG sw env included.
- **Various Additional Envelopes**
  - Added the following 4 envelopes (treated as software envelopes):
    - **6-Tap SSG Software Envelope (SSG SW ENV)**
      - Based on and improved from FMP7's 6-tap SSG software envelope.
      - Uses R1–R6, STL, L1–L6 parameters (R6, L6 are release parameters, same as FMP7).
        - Rate configurable from 0.0s to 10.0s.
        - Level configurable from 0.0 to 1.0.
      - Supports looping between specified taps (number of loops can be specified).
        - Up to 200 loops.
        - Set to 0 for infinite loop.
    - **11-Tap SSG Software Envelope (SSG SW ENV11)**
      - Expands SSG SW ENV to 11 taps.
        - Rate: 0.0s to 10.0s.
        - Level: 0.0 to 1.0.
      - Release parameters changed to R11/L11.
      - Inherits looping function from SSG SW ENV.
    - **3-Tap Pitch Envelope (Pitch Env)**
      - Envelope to change pitch across 3 taps.
      - Rates: AR/DR/RR.
        - 0.0s to 10.0s.
      - Levels: STL/ATL/SSL/RLL.
        - -4800 cents to 4800 cents.
    - **11-Tap Pitch Envelope (SSG SW PENV11)**
      - Pitch envelope version of SSG SW ENV11.
      - Allows for richer pitch variations.
        - Rate/Level usage is the same as Pitch Env.
          - Rate: 0.0s to 10.0s.
          - Level: -4800 cents to 4800 cents.
  - Applicable to all channels, operators, and rhythm pads.
- **Independent Effects Support**
  - Comes standard with many modern effects.
    - Filter
    - 3-Band EQ
    - Vibrato
    - Tremolo
    - Reverb
    - Delay
    - Modern Bitcrusher
    - SFC Echo
      - SPC-700 equivalent 8-tap FIR filter.
  - Bypass support.
- **Advanced Settings / Curve Edit Mode (2686V/OPZX7S)**
  - Mode to flexibly change register value variations and envelope level variations.
    - Switchable from the traditional mode via a toggle.
    - OPZX7S supports Curve Edit Mode by default (traditional mode omitted).
    - 2686V Light and 2686V Origin do not include Curve Edit Mode.
      - (To reduce CPU load).
  - Expands the traditional linear variation mode.
    - Over 30 variation logics.
      - Supports different variation ratios based on pass-through points, exponential curves, logarithmic curves, and spline curves (1-point, 2-point).
- **Value Range Confirmation**
  - Hovering over a slide bar button displays its parameter range.
  - Can be toggled ON/OFF in the settings screen.
- **Unison / Harmony Mode**
  - Supports up to 8 voices.
  - Harmony support via detune and spatial spread.
- **Polyphonic Playback**
  - Switchable with Monophonic mode.
  - Up to 10 simultaneous voices.
    - Combined with Unison/Harmony, allows up to 80 voices.
  - Configurable headroom gain.
- **Full Automation Support**
- **Supports MIDI keyboard Pitch Bend and Modulation functions**
- **3 Display Modes**
  - Full display
  - Mini-player display
  - Minimum display
- **Save and Load Presets**
  - Loaded audio/waveform files are saved with relative paths.
  - Includes a simple preset search function.
- **Import/Export Parameters per Channel**
  - I/O via text files.
  - I/O per envelope also possible.
  - Curve editing parameters can be imported/exported.
- **Customizable Wallpaper**
  - Multiple display modes available.

## 5. Supported OS

- Windows 11 (x64/ARM64) or later
  - ARM64 version behavior is unconfirmed.
- Linux version distribution is temporarily paused.
- Mac... sorry!
  - I have received reports that it can be built on macOS using `clang++` with minor modifications.
  - In version 1.2.0, the received patches have been integrated.

## 6. Plugin Format / Version

- VST3 (3.8)

## 7. Tested DAWs

- Steinberg Cubase Pro 15 (Windows)
- Image-Line FL Studio 2025 V25.2.4 (Windows)

## 8. Directory Structure

- This plugin consists of the following files:

```plaintext
- Windows
  - x86-64
    - 2686V.vst3 : Synth plugin "2686V"
    - 2686V.exe : Standalone program for 2686V
    - 2686VLight.vst3 : Synth plugin "2686VLight"
    - 2686VLight.exe : Standalone program for 2686VLight
    - 2686VOrigin.vst3 : Synth plugin "2686VOrigin"
    - 2686VOrigin.exe : Standalone program for 2686VOrigin
    - OPZX7S.vst3 : Synth plugin "OPZX7S"
    - OPZX7S.exe : Standalone program for OPZX7S
  - ARM64
    - 2686V.vst3 : Synth plugin "2686V"
    - 2686V.exe : Standalone program for 2686V
    - 2686VLight.vst3 : Synth plugin "2686VLight"
    - 2686VLight.exe : Standalone program for 2686VLight
    - 2686VOrigin.vst3 : Synth plugin "2686VOrigin"
    - 2686VOrigin.exe : Standalone program for 2686VOrigin
    - OPZX7S.vst3 : Synth plugin "OPZX7S"
    - OPZX7S.exe : Standalone program for OPZX7S
- Readme.md : Quick documentation (Japanese)
- Readme.en.md : Quick documentation (English)
- COPYING.txt : GPLv3 License terms
- Assets : Asset files folder
  - 2686V
    - Presets                 : Preset files folder for 2686V
      - OPLL Roms             : OPLL ROM preset patches (Please make sure to read "Regarding Presets in the OPLL Roms Folder" before use!)
      - SSG Drums             : Base preset files used for the drum samples in "SSG RHYTHM Sample.xml"
      - WT Samples            : Reference preset files for wavetables
      - SSG RHYTHM Sample.xml : Preset for rhythm sound samples.
      - Tekitou PSG.xml       : An arbitrarily created reference PSG preset.
      - M-M-Pro               : Rhythm/ADPCM (DPCM quality) channel preset mimicking a certain baseball game's voice synthesis.
    - 2686VLPresets           : Preset files folder for 2686V Light
    - 2686VOPresets           : Preset files folder for 2686V Origin
    - OPZX7Presets            : Preset files folder for OPZX7S
    - Resources               : Resource files (**NEVER rename, edit, or delete the folder or files inside!**)
    - Samples                 : Initial sample (audio) files folder
      - Noise Close HiHat.wav : Drum sample file used in "SSG RHYTHM Sample.xml"
      - Noise Open HiHat.wav  : Drum sample file used in "SSG RHYTHM Sample.xml"
      - Noise Snare.wav       : Drum sample file used in "SSG RHYTHM Sample.xml"
      - SSG Cymbal.wav        : Drum sample file used in "SSG RHYTHM Sample.xml"
      - SSG Kick.wav          : Drum sample file used in "SSG RHYTHM Sample.xml"
      - SSG Perc.wav          : Drum sample file used in "SSG RHYTHM Sample.xml"
      - M-Pro                 : A certain baseball game voice synthesis (feat. a certain virtual singer)
    - Wavetables              : Folder for managing wavetable memory files
    - ChannelParams           : Folder for managing parameter files for each sound channel (operator/pad)
    - FxOrders                : Folder for managing FX pane effect order files
    - FxParams                : Folder for managing FX pane parameter files
    - AmpEnvParams            : Folder for managing SSG/RHYTHM/WT/WT2/ADPCM/PCM/BEEP amp envelope parameter files
    - DetuneParams            : Folder for managing OPZX7S MUL/DT parameter files
    - LfoParams               : Folder for managing various LFO parameter files
    - PcmPlayParams           : Folder for managing PCM playback setting parameter files
    - PitchEnvParams          : Folder for managing pitch envelope (incl. 11-tap) parameter files
    - QualityParams           : Folder for managing audio quality parameter files
    - SshSwEnvParams          : Folder for managing SSG software envelope (incl. 11-tap) parameter files
    - ToneNoiseParams         : Folder for managing tone/noise mix parameter files
    - UnisonParams            : Folder for managing unison/harmony parameter files
    - CurveParams             : Folder for managing Curve Edit Mode parameter files
    - sample_bg.png           : Sample wallpaper
```

### 8-1. About Included Preset Files

Unauthorized reproduction of the included preset files (excluding those in the OPLL Roms folder) is strictly prohibited (However, you are free to use and modify them for your own works/music).

#### 8-1-1. Regarding Presets in the OPLL Roms Folder

The presets in the OPLL Roms folder are based on the patch data published by David (Plogue) on GitHub: [Copyright free OPLL(x) ROM patches](https://github.com/plgDavid/misc/wiki/Copyright-free-OPLL(x)-ROM-patches).

Since these patch data are released under CC BY-SA, these specific presets can be redistributed and reused under CC BY-SA 4.0.

[CC-BY SA 4.0 (English)](https://creativecommons.org/licenses/by-sa/4.0/deed.en)
[CC-BY SA 4.0 (日本語)](https://creativecommons.org/licenses/by-sa/4.0/deed.ja)

### 8-2. About Included Sample Files

Unauthorized reproduction of the included sample files is strictly prohibited (However, you are free to use and modify them for your own works/music).

### 8-3. Usage of Included Presets and Samples

You do not need prior permission or copyright attribution to use the included presets and samples (unless otherwise specified) for your works. However, the author would be delighted if you let them know when you use them, so please feel free to drop a message!

## 9. Installation

Extract the archive file to reveal the files listed in the "Directory Structure" section, and proceed as follows:

### 9-1. 2686V.vst3 / 2686VLight.vst3 / 2686VOrigin.vst3 / OPZX7S.vst3

Copy them into your VST3 folder.

The VST3 folder is usually located at:

C:\Program Files\Common Files\VST3

Administrator privileges are required to copy files into the VST3 folder.

### 9-2. Assets Folder

Inside the Assets folder, there is a 2686V folder. You MUST copy this 2686V folder directly into your Documents folder.

### 9-3. 2686V.exe / 2686VLight.exe / 2686VOrigin.exe / OPZX7S.exe

Copy these standalone executables to any folder of your choice.

## 10. Uninstallation

Simply delete the .vst3 files from your VST3 folder to complete the uninstallation.

## 11. How to Start

### 11-1. 2686V.vst3 / 2686VLight.vst3 / 2686VOrigin.vst3 / OPZX7S.vst3

Launch your DAW and insert the respective plugin into an instrument track or effect slot.

For detailed instructions, please refer to your DAW's manual.

### 11-2. 2686V.exe / 2686VLight.exe / 2686VOrigin.exe / OPZX7S.exe

Double-click the respective executable file from Windows Explorer.

It is convenient to create a shortcut for them.

## 12. Operation Manual

The manual is provided on the GitHub Wiki:

[2686V オペレーションマニュアル](https://github.com/cyross/2686V_2686VFX/wiki/2686V-2686VFX-Operation-manual)

## 13. License

本ソフトウェア（VST3プラグイン）には GPLv3 (GNU General Public License v3) が適用されます。 これは、本ソフトウェアが「JUCE」フレームワークおよび「VST3 SDK」とリンクしており、それらのライセンス（GPLv3/AGPLv3）を継承するためです。

Source code in this repository is available under the MIT License. However, the compiled binary links against the JUCE framework and VST3 SDK, which are subject to the GPLv3. Therefore, any distributed binaries must comply with the GPLv3 terms.

禁止事項: 本ソフトウェア、または本ソフトウェアに含まれる素材ファイル等を、著作者を偽って再配布すること、および著作者の許諾なく商用製品として販売することを禁じます（GPLv3の規定に基づく著作権表示の改変・削除は認められません）。

## 14. Terms of Use & Disclaimer

[Regarding Use in Music/Audio]
Audio works (music, sound effects, etc.) created using this software can be used freely and royalty-free, whether for commercial or non-commercial purposes. The copyright of the works generated or created using this software belongs to the creator of that work (the user).

[Disclaimer]
This software is provided "as is". The author assumes no responsibility for any damages, disadvantages, or troubles arising from the use of this software.

[Trademarks]
VST is a trademark of Steinberg Media Technologies GmbH.

## 15. Contact

Please send any inquiries to the following address:

`cyross4github_at_gmail.com`

## 16. SNS & Links

- [X](https://x.com/cyross4fortnite)
- [Youtube](https://www.youtube.com/@%E3%81%95%E3%81%84%E3%82%8D%E3%81%99)
- [ニコニコ動画](https://www.nicovideo.jp/user/3223933)
- [Piapro](https://piapro.jp/cyross\_p)
- [Booth](https://cyross.booth.pm/)
