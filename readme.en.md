# Retro Sound VST "2686V" v3.1.0 README

(C)2026 CYROSS

## 1. Introduction

"2686V" (hereafter referred to collectively as "this software") provides plugins that produce retro sound chip (FM/SSG/ADPCM, etc.) style audio — "2686V", "2686VLight", "26V", "86V" and "OPZX7S" — plus the effect plugin "2686VFX".

## 2. Purpose

I wanted to write tracks in my DAW that sounded like they came off a "PC-9801-26" or a "PC-9801-86", and nothing out there met my standards. So I set out to build a plugin... or at least, that was the plan...

## 3. Overview

### 3-0. What v3.1.0 adds and changes

- New modulation
  - **SSG HW PITCH ENV**
    - The SSG HW AMP ENV shapes pointed at pitch instead of level.
    - MIN/MAX are in cents (-4800 to 4800; MIN=0 / MAX=1200 by default).
    - Nudge buttons included.
    - Available on every channel (FM included), every operator and every RHYTHM pad.
    - Import / export supported (`.ssgHwPEnv`).
  - **WT AMP MOD**
    - The WT PITCH MOD shapes pointed at level instead of pitch.
    - Swings between MIN and MAX (MIN=0.0 / MAX=1.0 by default).
      - MIN/MAX apply across every wave and every SLOT it carries.
    - Nudge buttons included.
    - HuC6280 and FDS TABLE supported.
    - Available on every channel (FM included), every operator and every RHYTHM pad.
    - Import / export supported (`.wtampmod`).
  - **FDS TABLE is now two tables**
    - `FDS PITCH TABLE` on the WT PITCH MOD side, `FDS AMP TABLE` on the WT AMP MOD side.
- Now available per operator
  - **SSG HW AMP ENV on every operator**
    - Coexists with the existing SSG HW ENV on OPNA and OPZX7S.
  - **WT PITCH MOD on every operator**
    - The wave slots are per operator too.
  - Both support import / export.
- 2686VFX
  - **SSG HW PITCH ENV and WT AMP MOD added as modulation.**
- On screen
  - **Simple view**
    - Hides some of the sections on screen to keep the layout short.
      - Display only; the sound is unaffected.
    - Sits above "Show tooltips" in the SETTINGS tab. Off by default.
    - What gets hidden is decided per tab.
      - A section the channel has as a feature of the real chip stays.
        - SSG keeps SSG HW AMP ENV; WT/WT2/WT+ keep WT PITCH MOD.
      - 2686VFX hides the modulation panels and keeps the effect panels.
    - Turning it on reveals "Simple view customisation" (collapsed at first).
      - Anything ticked there stays on screen even while simple view is on.
      - The thirteen sections are AMP ENV / SSG HW AMP ENV / SSG SW AMP ENV /
        SSG SW AMP ENV[11] / WT AMP MOD / PITCH ENV / SSG HW PITCH ENV /
        SSG SW PITCH ENV[11] / WT PITCH MOD / LFO / MUL・DET / FIX /
        UNISON・HARMONY.
    - The setting is saved to the settings file.
  - **Buttons to bypass and to fold sections in bulk**
    - "Bypass hidden sections" button
      - SETTINGS tab, to the right of the simple view switch.
      - Switches off only the sections that are currently hidden.
      - Disabled while simple view is off.
    - "OP" and "CL" sit to the left of the view mode button, top right.
      - Ultramarine background, off-white text.
      - "OP": expands every section that is not switched off.
      - "CL": collapses every section that is switched off.
      - They cover the sections simple view hides, and apply to every channel, operator and pad at once.
    - None of them touch MUL・DET or UNISON・HARMONY, which carry no on/off switch.
    - On the LFO, PM and AM are handled together.
  - **Software-side section colours split more finely**
    - Level → blue, pitch → turquoise, LFO → purple.
    - OPTIONAL / UNISON・HARMONY / FIX / MUL・DET stay cyan.
    - Hardware-derived, quality and other sections are unchanged.
  - **2686VFX panel colours split by section**
    - Effects (including the PCM bit crusher) stay blue.
    - Modulation → red, LFO → green, everything else → cyan.
  - **Sections put in a tidier order**
    - Where hardware- and software-derived sections both exist, the hardware one comes first.
    - Level, then pitch, then the LFO.
- Fixed
  - On RHYTHM pads, a change of sample rate was not passed on to SSG HW AMP ENV.
  - WT PITCH MOD was missing from the RHYTHM pad parameter files.

### 3-1. Overall

*Bold entries are new or extended in v3.0.0.*

- 2686V:
  - My dream ultimate retro sound VST plugin.
  - A 100% accurate reproduction was impossible with my tech stack, so instead I built a plugin that gets as close as it can to the character of a range of sound chips.
  - Since the goal was "to make music", early development was written almost entirely by `Gemini3`.
    - **To work faster still, development now uses `Claude Code`.**
- **2686VLight**:
  - A lightweight version of 2686V.
  - Curve Edit Mode is removed from 2686V to lower the CPU load.
- **26V**:
  - 2686VLight brought closer to the "PC-9801-26(K)" configuration.
    - Channels are OPN and SSG only.
- **86V**:
  - 2686VLight brought closer to the "PC-9801-86" configuration.
    - Channels are OPNA, SSG, RHYTHM and PCM only.
    - The "ADPCM" channel is renamed "PCM", and its initial audio quality is changed to "4-bit PCM".
    - The RHYTHM channel has 6 pads.
      - Parameter file compatibility is preserved.
- **OPZX7S**:
  - A plugin positioned as a modern FM synthesis operator.
  - The OPZX7S channel and Curve Edit Mode extracted from 2686V, with a lower load.
  - Curve Edit Mode applies by default (the traditional linear mode is omitted).
- **2686VFX**:
  - An effect plugin derived from 2686V.
  - The effects and modulation used in 2686V, now usable on your DAW's instrument and audio tracks.
  - Omitted in v2.x.x; brought back for v3.0.0.
  - Carries the FX pane from 2686V as its main effect section.
    - The UI is adjusted to match.
  - Carries an FX unique to 2686VFX: the "PCM Bit Crusher".
    - The "QUALITY" section from the PCM-family channels, repurposed as an effect.
    - Only part of the BIT list is available.
  - Reordering applies to the FX only (including the PCM Bit Crusher).
  - On top of that, the following modulation from 2686V's sound engine has been added:
    - AMP ENV
      - Based on the one used in SSG and elsewhere.
    - SSG HW AMP ENV
      - Based on the one used in SSG and elsewhere.
    - SSG HW PITCH ENV
      - Based on the one used in SSG and elsewhere.
    - SSG SW AMP ENV11
      - Based on the one used in SSG and elsewhere.
    - PITCH ENV
      - Based on the one used in SSG and elsewhere.
    - SSG SW PITCH ENV11
      - Based on the one used in SSG and elsewhere.
    - WT PITCH MOD
      - Based on the one used in WT/WT2/WT+ and elsewhere.
    - WT AMP MOD
      - Based on the one used in WT/WT2/WT+ and elsewhere.
    - LFO
      - Based on the one used in OPZX7S.
    - MUL/DET
      - Based on the one used in OPZX7S.
      - Simplified version.
    - UNISON/HARMONY
      - Based on the one used in the sound channels.
      - Simplified version.
    - *Switched on and off by MIDI input.*
    - *The UI follows the FX groups.*
    - *Bypass can be set per modulation, and all together from the main group.*
  - The FX and modulation groups sit in a single horizontal row; use the scroll bar to bring each into view.

## 4. Features

*Bold entries are new or extended in v3.0.0.*

- Enjoy a wide variety of retro game sound sources (channels)
  - *Each sound source is divided into a unit called a "channel".*
  - Channels can be switched via tabs.
  - **However, only one channel can sound at a time per plugin.**
    - Designed on the idea that a DAW's tracks give you more flexibility in arranging the setup.
- A hand-picked (and invented) set of FM channels, from traditional to modern
  - Numerous hardware-derived FM channels
    - Reproductions of many FM operators, with extensions that stay within the original design philosophy
      - OPNA
        - Covers OPNA's basic features.
        - Carries the software LFO used in N88-BASIC.
        - Carries the hardware LFO and the SSG hardware amp envelope.
      - OPN
        - Covers OPN's basic features.
        - Carries the software LFO used in N88-BASIC.
      - OPL
        - Covers the basic features of OPL/OPLL.
        - Carries a hardware LFO per operator.
      - OPL3
        - Covers OPL3's basic features.
        - Adds original algorithms (2OPx2).
        - Carries a hardware LFO per operator.
      - OPM
        - Covers OPM's basic features, hardware LFO included.
        - Adds OPP's key scale setting.
  - An original modern-style FM channel: OPZX7S
    - My dream ultimate FM operator
      - An operator that takes the best of OPZ/OPX/MA-7/OPS and adds original features.
        - A lot of it is still a mystery (OPX especially), so this is an approximation, driven by the wish that **even if 100% reproduction is out of reach, whatever can be reproduced should be.**
    - Up to 8 operators
      - OPX/OPZ/MA-7/OPS algorithms are selectable.
      - Original 1OP–8OP algorithms are selectable.
      - **The algorithm diagram is now drawn in real time instead of from an image.**
    - **Feedback can be set independently per operator.**
      - The value is a real number in the range -8.0 to 8.0 (default: 0.0).
    - **Algorithm Matrix**
      - Customise algorithms freely, from 1OP to 8OP.
      - You specify the modulation and feedback routing with checkboxes.
      - Feedback settings that the original specs could never express are possible.
    - The per-operator amp envelope follows MA-7.
      - A switch also lets you set it with real numbers.
    - Key scale switches between the MA-7/OPZ/OPS settings.
    - MUL combines MA-7 with an extended OPZ version.
      - MA-7 compliant MUL selection.
      - The OPZ MUL value (MulRatio) setting is extended, with a much higher upper limit.
    - DT adds a freely settable DT3 on top of OPZ's DT/DT2.
    - A wide range of waveforms
      - The MA-7/OPZ waveforms, plus original ones.
        - *The MA-3-like waveforms Gemini first brought in (which look nothing like the real MA-7 ones) seemed fun, so they went in as extra waveforms.*
      - Audio files can be used as waveforms (reproducing OPX).
        - Playback start position and playback ratio are supported.
      - Wave memory can be used as waveforms (`.wt` and `.wt2` files are supported, reproducing MA-7).
      - **Waveforms in loaded files can be previewed.**
    - A powerful LFO
      - Independent frequency and sync settings for AM and PM.
      - A wide range of D/S values.
      - Settable per channel and per operator.
      - **Original waveforms added.**
    - OPNA's hardware SSG envelope is available.
    - Software envelopes per operator
      - SSG software amp envelope (6-tap / **11-tap**)
      - Pitch envelope (3-tap / **11-tap**)
    - **Global software envelopes**
      - Amp envelope
      - SSG hardware amp envelope
      - SSG software amp envelope (11-tap)
      - Pitch envelope (3-tap / 11-tap)
      - WT pitch modulation
      - A parameter graph for the global set
    - As a special feature, MML input per operator.
  - Original options on the per-operator amp envelope of every FM channel
    - KOR (Keep On Release)
      - The level does not decay until the specified time after release.
      - Used when you want the SSG software envelope to take priority.
    - XOF
      - Ignores RR and never decays.
      - Used when you want the SSG software envelope to take priority.
    - Bypass
      - Bypasses the amp envelope.
      - Used when you want the SSG software envelope to take priority.
  - As hardware features, a sound effect mode and an operator mask per operator.
  - As software features, envelopes per operator
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
  - **As software features, global envelopes**
    - Amp envelope
    - SSG hardware amp envelope
    - SSG software amp envelope (11-tap)
    - Pitch envelope (3-tap / **11-tap**)
    - WT pitch modulation
    - A parameter graph for the global set
  - As a special feature, MML input per operator.
  - **The algorithm diagram is now drawn in real time instead of from an image.**
- The SSG (PSG) channel, familiar from the YM2303/YM2608
  - Not an exact reproduction, but an enhanced version that folds in PSG features.
  - Square and triangle waves are provided.
  - The pulse wave covers the duty ratios of the SSG, the Famicom and the KONAMI VRC VI.
  - The duty ratio can also be set freely.
  - The phase can be inverted.
  - A switch changes the waveform to a Famicom-like one.
  - The triangle wave's peak position can be adjusted.
    - This also gives you a sawtooth.
  - As software features, global envelopes
    - Amp envelope
    - SSG hardware amp envelope
      - **Original waveforms added.**
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
    - **WT pitch modulation**
    - MUL/DET
      - Uses the one from OPZX7S.
    - LFO
      - Uses the one from OPZX7S.
      - **Original waveforms added.**
    - Sound effect mode
- The YM2303 and YM2608 never had it, but no retro game sound is complete without it! Wave memory channels (WT/WT2)
  - Preset wave memory and custom wave memory are provided.
  - WT sets values as real numbers; WT2 sets them in steps.
  - Custom wave memory supports 32/64/128/256 samples.
  - A range of helper buttons is provided.
  - Value alignment while holding a key
    - 0.01, 0.05, 0.1, 16 steps, 32 steps, 64 steps
  - Import and export of wave memory
    - Uses `.wt` and `.wt2` files (text files).
  - As software features, global envelopes
    - Amp envelope
    - SSG hardware amp envelope
      - **Original waveforms added.**
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
    - **WT pitch modulation**
    - MUL/DET
      - Uses the one from OPZX7S.
    - LFO
      - Uses the one from OPZX7S.
      - **Original waveforms added.**
    - Sound effect mode
  - **The waveform of each loaded file can be previewed.**
- **For anyone who cannot live with only one waveform in WT/WT2! A wave memory switching channel (WT+)**
  - Plays waveforms from existing `.wt` and `.wt2` files.
  - Up to 32 can be loaded, switched in real time with a slider.
  - They can also be switched in real time from DAW automation.
  - As software features, global envelopes
    - Amp envelope
    - SSG hardware amp envelope
      - **Original waveforms added.**
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
    - **WT pitch modulation**
    - MUL/DET
      - Uses the one from OPZX7S.
    - LFO
      - Uses the one from OPZX7S.
      - **Original waveforms added.**
    - Sound effect mode
  - **The waveform of each loaded file can be previewed.**
- The rhythm channel (RHYTHM)
  - A drum machine with a bit crusher
    - A wide choice of bit rates and sampling rates
      - 4-bit ADPCM supported
      - 1-bit DPCM supported
    - Playback start position, playback ratio and loop points are supported.
  - 8 pads (6 in 86V)
    - The YM-2608B had 6; this is extended to 8.
      - Can also stand in for the 8-voice SSGPCM (PSGPCM) used by PPZ8 and the like.
      - 86V keeps 6 to follow the 86 sound board.
    - An audio file can be assigned to each pad.
    - MIDI key assignment can be customised.
  - As software features, envelopes on each pad
    - Amp envelope
    - SSG hardware amp envelope
      - **Original waveforms added.**
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
    - **WT pitch modulation**
    - MUL/DET
      - Uses the one from OPZX7S.
    - LFO
      - Uses the one from OPZX7S.
      - **Original waveforms added.**
    - Sound effect mode
  - **The waveform loaded on each pad can be previewed.**
- The audio file channel (ADPCM/PCM)
  - Plays a single audio file like a sampler
    - A wide choice of bit rates and sampling rates
      - 4-bit ADPCM supported
      - 1-bit DPCM supported
    - Playback start position, playback ratio and loop points are supported.
  - As software features, global envelopes
    - Amp envelope
    - SSG hardware amp envelope
      - **Original waveforms added.**
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
    - **WT pitch modulation**
    - MUL/DET
      - Uses the one from OPZX7S.
    - LFO
      - Uses the one from OPZX7S.
      - **Original waveforms added.**
    - Sound effect mode
  - **The waveform of the loaded file can be previewed.**
- The BEEP channel
  - Reproduces the beep (a square wave at a fixed duty ratio).
    - Sound effect mode carries a button to switch to 2000Hz.
  - As software features, global envelopes
    - Amp envelope
    - SSG hardware amp envelope
      - **Original waveforms added.**
    - SSG software amp envelope (6-tap / **11-tap**)
    - Pitch envelope (3-tap / **11-tap**)
    - **WT pitch modulation**
    - MUL/DET
      - Uses the one from OPZX7S.
    - LFO
      - Uses the one from OPZX7S.
      - **Original waveforms added.**
    - Sound effect mode
- Additional envelopes
  - The following four envelopes are added (treated as software envelopes)
    - 6-tap SSG software envelope (SSG SW ENV)
      - Based on FMP7's 6-tap SSG software envelope, with improvements.
      - Same as FMP7 in having R1–R6, STL and L1–L6, with R6 and L6 as the release parameters.
        - Rates can be set from 0.0 to 10.0 seconds.
        - Levels can be set from 0.0 to 1.0.
      - Looping between chosen taps is supported (the loop count is settable).
        - Up to 200 loops.
        - 0 means loop forever.
    - **SSG hardware amp envelope**
      - The SSG HW ENV carried by OPNA/OPZX7S/SSG, considerably extended.
      - Original waveforms added.
      - The MIN/MAX of the range can be set.
      - Waveform preview.
    - 11-tap SSG software envelope (SSG SW ENV11)
      - Extends the tap count to 11.
        - Rates can be set from 0.0 to 10.0 seconds.
        - Levels can be set from 0.0 to 1.0.
      - The release parameters move to R11/L11.
      - The loop feature is inherited from SSG SW ENV.
    - 3-tap pitch envelope (Pitch Env)
      - A 3-tap envelope that changes pitch.
      - Three rates: AR/DR/RR
        - Settable from 0.0 to 10.0 seconds.
      - Four levels: STL/ATL/SSL/RLL
        - Each settable from -4800 to 4800 cents.
    - 11-tap pitch envelope (SSG SW PENV11)
      - The pitch envelope version of SSG SW ENV11.
      - Richer pitch movement.
        - Rates and levels match the existing pitch envelope.
          - Rates from 0.0 to 10.0 seconds.
          - Levels from -4800 to 4800 cents.
    - **Wave memory pitch modulation**
      - The MODULATION carried by WT/WT, considerably extended.
        - Many more waveforms to choose from.
      - Also carries the PCE feature where the wave memory itself is used as the modulator.
        - Switchable between 8 waveforms in real time.
      - The FDS feature of customising the waveform on the spot is available.
  - Applies to every channel, operator and rhythm pad.
- Independent effects
  - A set of modern effects is built in
    - Filter
    - 3-band EQ
    - Vibrato
    - Tremolo
    - Reverb
    - Delay
    - Modern bit crusher
    - SFC echo
      - An 8-tap FIR filter equivalent to the SPC-700.
  - Bypass supported.
- Extended settings and Curve Edit Mode (2686V/OPZX7S)
  - A mode that gives you flexible control over how register values and envelope levels change
    - A switch returns you to the traditional mode.
    - OPZX7 uses Curve Edit Mode as standard (it cannot be switched back).
    - 2686VLight and 86V do not carry Curve Edit Mode.
      - To keep the load down.
  - Extends the traditional linear mode
    - Over 30 kinds of change logic
      - Splitting the rate of change by pass-through points, and changes based on exponential curves, logarithmic curves and splines (one or two points).
- Value ranges can be checked
  - Hover over a slider's button to show its range.
  - Can be switched on and off in the settings screen.
- Unison / harmony mode
  - Up to 8 voices.
  - Harmony through detune and stereo spread.
  - **Arpeggio**
    - A fast arpeggio can be reproduced by frequency.
    - The arpeggio content is fixed (Voice0 -> Voice1 -> ... -> VoiceN -> Voice0 -> ...).
  - **Detune and spread can be set per voice.**
- Chords can be played
  - Switchable with monophonic mode.
  - Up to 10 simultaneous voices.
    - Combined with unison/harmony, up to 80 voices.
  - The headroom gain can be changed.
- Automation, of course.
- Supports the pitch bend and modulation features of a MIDI keyboard.
- Plenty of ways to nudge LEVEL
  - Nudge buttons
  - **Stepped input via Steps**
- Three screen modes
  - Full view
  - Mini player view
  - Minimum view
- Presets can be saved and loaded
  - Loaded audio and waveform files are saved as relative paths.
  - A simple preset search is built in.
  - **Reading and writing in `JSON` and `YAML` is supported.**
    - `XML` is now read-only.
  - **Favourites**
    - Unlimited in theory.
  - **Load history**
    - Unlimited in theory.
  - **Filtering by favourites and history while searching**
- Each channel's parameters can be imported and exported
  - **Input and output through `JSON` or `YAML` files**
    - Plain text files are now read-only.
  - Import and export per envelope is also possible.
  - Curve Edit parameters can be imported and exported.
- A wallpaper can be set
  - Several display modes are provided.
- The UTILITY category has been extended
  - **Import and export of channel, operator and pad parameters**
- **The file format used for presets, settings and parameters has moved to `JSON` and `YAML`**
  - `JSON` or `YAML` is switchable in the settings.
  - The previous formats are read-only.
    - Reading a parameter file converts it to `JSON` or `YAML` automatically.
  - `.wt` and `.wt2` keep their previous format.
    - Because of how they are used by OPZX7S/WT+/WT PITCH MOD.
- **The COLORS tab**
  - Customise the colours used in the UI.
  - One button restores the defaults.
  - Besides RGB/HSV, colours can be taken from colour names (JUCE plus original ones) and palettes (PC-8801/PC-9801/X68000/PC Engine/Mega Drive).
  - Take a look through the entries — you may spot a few names you recognise...
- **A major UI overhaul**
  - The tabs are a little wider.
  - The group title style has changed.
  - Category labels have changed considerably.
    - The open/closed state is now shown by a lamp rather than text.
      - The colours are an homage to Hatsune Miku's hair clip and headphones.
    - The style of the various components has changed.
  - Shadows have been added to the components.
  - Nudge buttons can be shown or hidden.
    - Toggled with the "N" button.
  - Many more waveform previews
    - The global envelope graph on the FM channels
    - The waveform loaded into SSG HW AMP ENV
    - OPZX7S waveforms (WT, WT2, PCM)
    - The audio files loaded into RHYTHM pads and ADPCM (PCM)
    - The WS PITCH MOD waveform
    - The waveforms loaded into WT+

## 5. Supported OS

- Windows 11 (x64/ARM64) or later
  - The ARM64 build is untested.
- Linux builds are on hold for the time being.
  - Build settings are provided, however.
- Mac — sorry!
  - I have received a report that it builds with `clang++` on macOS with only small changes.
  - The patch I was given was folded in for 1.2.0.
  - Build settings are provided (untested).

## 6. Plugin format and version

- VST3 (3.8)

## 7. DAWs verified

- Steinberg Cubase Pro 15 (Windows)
- Image-Line FL Studio 2025 V25.2.4 (Windows)
- Ableton Live 12 Lite (Windows)
- Bandlab Cakewalk Sonar (Windows)

## 8. Contents

- This plugin consists of the following files.

*There are far too many preset and parameter files to list individually, so the file names inside those folders are omitted.*

```plaintext
- Windows
  - x86-64
    - VST3
      - 2686V.vst3 : the "2686V" synth plugin
      - 2686VLight.vst3 : the "2686VLight" synth plugin
      - 26V.vst3 : the "26V" synth plugin
      - 86V.vst3 : the "86V" synth plugin
      - OPZX7S.vst3 : the "OPZX7S" synth plugin
      - 2686VFX.vst3 : the "2686VFX" effect plugin
    - Standalone
      - 2686V.exe : the standalone program for 2686V
      - 2686VLight.exe : the standalone program for 2686VLight
      - 26V.exe : the standalone program for 26V
      - 86V.exe : the standalone program for 86V
      - OPZX7S.exe : the standalone program for OPZX7S
      - 2686VFX.exe : the standalone program for 2686VFX
  - ARM64
    - VST3
      - 2686V.vst3 : the "2686V" synth plugin
      - 2686VLight.vst3 : the "2686VLight" synth plugin
      - 26V.vst3 : the "26V" synth plugin
      - 86V.vst3 : the "86V" synth plugin
      - OPZX7S.vst3 : the "OPZX7S" synth plugin
      - 2686VFX.vst3 : the "2686VFX" effect plugin
    - Standalone
      - 2686V.exe : the standalone program for 2686V
      - 2686VLight.exe : the standalone program for 2686VLight
      - 26V.exe : the standalone program for 26V
      - 86V.exe : the standalone program for 86V
      - OPZX7S.exe : the standalone program for OPZX7S
      - 2686VFX.exe : the standalone program for 2686VFX
- Readme.md : brief documentation
- Readme.en.md : brief documentation (English)
- COPYING.txt : the GPLv3 licence terms
- [Dir]Assets : the asset file folder
  - [Dir]2686V
    - [Dir]Presets                 : the preset file folder for 2686V
      - [Dir]fromCC                : the folder of files made for me by Claude Code
        - [Dir]26V                 : the preset file folder for 26V
        - [Dir]86V                 : the preset file folder for 86V
        - [Dir]2686V               : the preset file folder for 2686V
        - [Dir]2686VLight          : the preset file folder for 2686VLight
        - [Dir]OPZX7S              : the preset file folder for OPZX7S
      - [Dir]OPLL Roms             : OPLL ROM preset voices (before using these, please be sure to read "About the presets in the OPLL Roms folder"!)
      - [Dir]SSG Drums             : the preset files the drum samples used by "SSG RHYTHM Sample.xml" were made from
      - [Dir]WT Samples            : reference preset files for wave memory
      - SSG RHYTHM Sample.xml      : a preset for the rhythm sound source sample
      - Tekitou PSG.xml            : a rough-and-ready PSG preset, for reference
      - M-M-Pro                    : presets for the rhythm and ADPCM (DPCM quality) channels doing the voice synthesis from a certain baseball game
    - [Dir]Resources               : resource files (**never rename, edit or delete this folder or its contents!**)
    - [Dir]Samples                 : the folder for the initial sample (audio) files
      - [Dir]fromCC                : the folder of files made for me by Claude Code
      - [Dir]M-Pro                 : voice synthesis from a certain baseball game (feat. a certain virtual singer)
      - Noise Close HiHat.wav      : a drum sample file used by "SSG RHYTHM Sample.xml"
      - Noise Open HiHat.wav       : a drum sample file used by "SSG RHYTHM Sample.xml"
      - Noise Snare.wav            : a drum sample file used by "SSG RHYTHM Sample.xml"
      - SSG Cymbal.wav             : a drum sample file used by "SSG RHYTHM Sample.xml"
      - SSG Kick.wav               : a drum sample file used by "SSG RHYTHM Sample.xml"
      - SSG Perc.wav               : a drum sample file used by "SSG RHYTHM Sample.xml"
    - [Dir]AmpEnvParams            : the parameter file folder for the global Amp Env
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]ChannelParams           : the parameter file folder for each channel
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]CustomizeColorSettings  : the folder for colour customisation files
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]DetuneParams            : the parameter file folder for MUL/DET
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]FxOrders                : the folder for files that store the effect order
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]FxParams                : the folder for files that store the effect parameters
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]LfoParams               : the folder for files that store the LFO parameters
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]PcmPlayParams           : the parameter file folder for the PCM playback position and so on
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]PitchEnvParams          : the parameter file folder for pitch envelopes
      - [Dir]fromCC                : the folder of files made for me by Claude Code
        - [Dir]PitchEnv            : the parameter file folder for the traditional pitch envelope
        - [Dir]SsgSwPenv11         : the parameter file folder for SSG SW Pitch ENV11
    - [Dir]QualityParams           : the parameter file folder for audio quality
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]SsgHwEnvParams          : the parameter file folder for SSG HW AMP ENV and SSG HW PITCH ENV
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]SsgSwEnvParams          : the parameter file folder for SSG SW ENV(11)
      - [Dir]fromCC                : the folder of files made for me by Claude Code
        - [Dir]SsgSwEnv            : the parameter file folder for the traditional SSG SW AMP ENV
        - [Dir]SsgSwEnv11          : the parameter file folder for SSG SW AMP ENV(11)
    - [Dir]ToneNoiseParams         : the parameter file folder for the Tone/Noise mix and so on
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]UnisonParams            : the parameter file folder for UNISON/HARMONY
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - [Dir]Wavetables              : the waveform file folder
      - [Dir]fromCC                : the folder of files made for me by Claude Code
        - [Dir]wt                  : the `.wt` files
        - [Dir]wt2                 : the `.wt2` files
    - [Dir]WtModParams             : the parameter file folder for WT PITCH MOD and WT AMP MOD
      - [Dir]fromCC                : the folder of files made for me by Claude Code
    - sample_bg.png                : a sample wallpaper
```plaintext
- Windows
  - x86-64
    - VST3
      - 2686V.vst3 : the "2686V" synth plugin
      - 2686VLight.vst3 : the "2686VLight" synth plugin
      - 86V.vst3 : the "86V" synth plugin
      - OPZX7S.vst3 : the "OPZX7S" synth plugin
      - 2686VFX.vst3 : the "2686VFX" effect plugin
    - Standalone
      - 2686V.exe : the standalone program for 2686V
      - 2686VLight.exe : the standalone program for 2686VLight
      - 86V.exe : the standalone program for 86V
      - OPZX7S.exe : the standalone program for OPZX7S
      - 2686VFX.exe : the standalone program for 2686VFX
  - ARM64
    - VST3
      - 2686V.vst3 : the "2686V" synth plugin
      - 2686VLight.vst3 : the "2686VLight" synth plugin
      - 86V.vst3 : the "86V" synth plugin
      - OPZX7S.vst3 : the "OPZX7S" synth plugin
      - 2686VFX.vst3 : the "2686VFX" effect plugin
    - Standalone
      - 2686V.exe : the standalone program for 2686V
      - 2686VLight.exe : the standalone program for 2686VLight
      - 86V.exe : the standalone program for 86V
      - OPZX7S.exe : the standalone program for OPZX7S
      - 2686VFX.exe : the standalone program for 2686VFX
- Readme.md : brief documentation
- Readme.en.md : brief documentation (English)
- COPYING.txt : the GPLv3 licence terms
- Assets : the asset file folder
  - 2686V
    - Presets                 : the preset file folder for 2686V
      - OPLL Roms             : OPLL ROM preset voices (before using these, please be sure to read "About the presets in the OPLL Roms folder"!)
      - SSG Drums             : the preset files the drum samples used by "SSG RHYTHM Sample.xml" were made from
      - WT Samples            : reference preset files for wave memory
      - SSG RHYTHM Sample.xml : a preset for the rhythm sound source sample.
      - Tekitou PSG.xml       : a rough-and-ready PSG preset, for reference.
      - M-M-Pro               : presets for the rhythm and ADPCM (DPCM quality) channels doing the voice synthesis from a certain baseball game
    - Resources               : resource files (**never rename, edit or delete this folder or its contents!**)
    - Samples                 : the folder for the initial sample (audio) files
      - Noise Close HiHat.wav : a drum sample file used by "SSG RHYTHM Sample.xml"
      - Noise Open HiHat.wav  : a drum sample file used by "SSG RHYTHM Sample.xml"
      - Noise Snare.wav       : a drum sample file used by "SSG RHYTHM Sample.xml"
      - SSG Cymbal.wav        : a drum sample file used by "SSG RHYTHM Sample.xml"
      - SSG Kick.wav          : a drum sample file used by "SSG RHYTHM Sample.xml"
      - SSG Perc.wav          : a drum sample file used by "SSG RHYTHM Sample.xml"
      - M-Pro                 : voice synthesis from a certain baseball game (feat. a certain virtual singer)
    - sample_bg.png           : a sample wallpaper
```

### 8-1. About the bundled preset files

- Except for the presets in the `OPLL Roms` folder, **redistribution of the bundled preset files without permission is strictly prohibited** (you are free to use and modify them in your own work).

#### 8-1-1. About the presets in the `OPLL Roms` folder

- The presets in the `OPLL Roms` folder are based on the patch data in [Copyright free OPLL(x) ROM patches](https://github.com/plgDavid/misc/wiki/Copyright-free-OPLL(x)-ROM-patches), published on GitHub by David of Plogue.
- That patch data is published under `CC BY-SA`, so these presets may be redistributed and reused under `CC BY-SA 4.0`.
  - [CC-BY SA 4.0 (English)](https://creativecommons.org/licenses/by-sa/4.0/deed.en)
  - [CC-BY SA 4.0 (Japanese)](https://creativecommons.org/licenses/by-sa/4.0/deed.ja)

### 8-2. About the bundled sample files

- **Redistribution of the bundled sample files without permission is strictly prohibited** (you are free to use and modify them in your own work).

### 8-3. About using the bundled presets and samples

- You do not need to apply to the author in advance or credit them to use the bundled presets and samples (except where explicitly stated otherwise), but **the author would be delighted to hear from you**, so please do get in touch.

## 9. Installation

- Extracting the archive creates the files listed under "Contents". Then do the following.

### 9-1. 2686V.vst3 / 2686VLight.vst3 / 26V.vst3 / 86V.vst3 / OPZX7S.vst3 / 2686VFX.vst3

- Copy them into your VST3 folder.
  - The VST3 folder is usually here:
    - `C:\Program Files\Common Files\VST3`
  - **Copying into the VST3 folder requires administrator rights.**

### 9-2. The Assets folder

- Directly under the `Assets` folder there is a `2686V` folder. You **must** copy it **directly into your Documents folder**.

### 9-3. 2686V.exe / 2686VLight.exe / 26V.exe / 86V.exe / OPZX7S.exe / 2686VFX.exe

- Copy them into any folder you like.

## 10. Uninstallation

- Deleting each `.vst3` from the VST3 folder completes the uninstall.

## 11. Launching

### 11-1. 2686V.vst3 / 2686VLight.vst3 / 26V.vst3 / 86V.vst3 / OPZX7S.vst3 / 2686VFX.vst3

- Start your DAW and insert each plugin into a track or an effect slot.
- For the details of how to do that, follow your DAW's own instructions.

### 11-2. 2686V.exe / 2686VLight.exe / 26V.exe / 86V.exe / OPZX7S.exe / 2686VFX.exe

- Double-click each executable in Windows Explorer.
- Creating a shortcut is handy.

## 12. Operation manual

- Provided on GitHub Pages.
  - [2686V Series Users manual](https://cyross.github.io/2686V_2686VFX/)

## 13. License

This software (the VST3 plugins) is covered by the GPLv3 (GNU General Public License v3). This is because it links against the "JUCE" framework and the "VST3 SDK", and inherits their licences (GPLv3/AGPLv3).

Source code in this repository is available under the MIT License. However, the compiled binary links against the JUCE framework and VST3 SDK, which are subject to the GPLv3. Therefore, any distributed binaries must comply with the GPLv3 terms.

Prohibited: redistributing this software, or the material files it contains, while misrepresenting its authorship, and selling it as a commercial product without the author's permission (altering or removing the copyright notice is not permitted under the terms of the GPLv3).

## 14. Terms of use and disclaimer

**On using it in your music.** Music, sound effects and other audio works produced with this software may be used freely and royalty-free, commercially or otherwise. The copyright in works created with this software belongs to the person who created them — that is, to you.

**Disclaimer.** This software is provided as is. The author accepts no responsibility whatsoever for any damage, disadvantage or trouble arising from its use.

**Trademarks.** VST is a trademark of Steinberg Media Technologies GmbH.

## 15. Contact

Please contact me at:

- `cyross4github_at_gmail.com`

## 16. Social and elsewhere

- [X](https://x.com/cyross4fortnite)
- [Youtube](https://www.youtube.com/@%E3%81%95%E3%81%84%E3%82%8D%E3%81%99)
- [Niconico](https://www.nicovideo.jp/user/3223933)
- [Piapro](https://piapro.jp/cyross\_p)
- [Booth](https://cyross.booth.pm/)
