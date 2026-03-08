#pragma once

#include <array>

#include "Mode.h"

struct FmOpParams
{
    // --- Mask ---
    bool mask = false;

    // --- Envelope (ADSR) ---
    float attack = 0.01f;  // AR
    float decay = 0.0f;    // DR
    float sustain = 1.0f;  // SL (Sustain Level)
    float release = 0.2f;  // RR

    // --- YM2608 Specific ---
    float sustainRate = 0.0f; // SR: Sustain Rate

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7
    int detune2 = 0;       // DT2: OPL3/OPM Only
    float totalLevel = 0.0f;
    int keyScale = 0;
    int keyScaleLevel = 0; // KSL (Level Scaling: 0-3)
    int ssgEg = 0;         // SE: 0~15 (SSG-EG)
    // FM SSG-EG Frequency (SSG-EGの周期速度)
    // 0.1Hz(ゆっくり) ～ 50Hz(高速) 程度を想定
    float fmSsgEgFreq = 1.0f;

    bool amEnable = false;
    bool vibEnable = false; // VIB (LFO Pitch)
    bool egType = false;    // EG-TYP (Sustain Mode)

    // オペレーターごとのLFO個別感度（上乗せ分）
    int pms = 0; // 個別 Pitch Mod Sensitivity (0-7)
    int ams = 0; // 個別 Amp Mod Sensitivity (0-3)

    // --- OPL/OPL3 LFO Settings ---
    float oplAms = 3.7f;  // AM Speed (Hz)
    float oplAmd = 4.8f;  // AM Depth (dB)
    float oplPms = 6.4f;  // PM Speed (Hz)
    float oplPmd = 14.0f; // PM Depth (Cent)

    // CSM/Fix Mode
    bool fixedMode = false;
    float fixedFreq = 440.0f;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    float pcmOffset = 0.0f; // ms
    float pcmRatio = 1.0f;

    bool isOplMode = false; // 内部でOPL系かOPN系かを判定するフラグ
    bool regEnable = false; // RG-EN

    int rar = 31;  // AR Register (0-31 / OPL: 0-15)
    int rdr = 0;   // DR Register (0-31 / OPL: 0-15)
    int rsr = 0;   // SR / D2R Register (0-31)
    int rrr = 15;  // RR Register (0-15)
    int rsl = 0;   // SL Register (0-15)
    int rtl = 0;   // TL Register (0-127 / OPL: 0-63)
};

struct RhythmPadParams
{
    float level = 1.0f;
    float pan = 0.5f;     // 0.0(L) - 1.0(R)
    int noteNumber = 36;  // MIDI Note Number (e.g., 36=C1)

    // ADPCM quality settings (it's more interesting to be able to set these individually)
    int qualityMode = 6;  // Default: ADPCM
    int rateIndex = 3;    // Default: 16kHz

    // Whether to play as a "One Shot" or stop the sound on Note Off
    bool isOneShot = true;

    float release = 0.1f;

    float pcmOffset = 0.0f; // ms
    float pcmRatio = 1.0f;
};

struct SimpleAdsr { float a = 0.01f, d = 0.0f, s = 1.0f, r = 0.2f; };

struct SynthParams
{
    // [[ Global Parameters ]]

    // --- Synth Mode ---
    OscMode mode = OscMode::OPNA;

    // [[ FM Parameters ]]

    // --- Algorithm ---
    int algorithm = 7;

    // --- Feedback ---
    float feedback = 0.0f;
    float feedback2 = 0.0f;

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    // Default: Raw (32bit float) or 3 (8bit) depending on preference
    int fmBitDepth = 4;

    // --- Sampling Rate ---
    // 0:96k, 1:55.5k, 2:48k, 3:44.1k, 4:22.05k, 5:16k, 6:8k
    // Default: 55.5kHz (Typical FM Chip Rate)
    int fmRateIndex = 1;

    // -- OPNA/OPM/OPZX3 LFO(Global) ---

    // LFO Speed Freq (approx 3Hz - 30Hz)
    float lfoFreq = 5.0f;

    // LFO Enable Flag
    bool amEnable = false;
    bool pmEnable = false;

    // LFO Sensitivity

    // Pitch Modulation Sensitivity (0-7)
    int lfoPms = 0;

    // Amplitude Modulation Sensitivity (0-3)
    int lfoAms = 0;

    // LFO Depth (0-127)

    // Pitch Modulation Depth
    int lfoPmd = 0;

    // Amplitude Modulation Depth
    int lfoAmd = 0;

    // LFO Waveform (0:Saw, 1:Square, 2:Triangle, 3:Random)
    int lfoWave = 2;

    // LFO AM Smooth Ratio (0.005 - 0.5)
    float lfoAmSmRt = 0.005f;

    // --- OPLL Preset ---
    int opllPreset = 0; // OPLL Preset Instrument Index

    // Parameters for Operator
    std::array<FmOpParams, MaxFmOperators> fmOp;

    // [[ SSG Parameters ]]

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int ssgBitDepth = 3; // Default 8bit (or 4bit to be like real hardware volume steps?)

    // --- Sampling Rate ---
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    int ssgRateIndex = 1; // Default 55.5kHz (Standard SSG/OPNA rate)

    // --- Tone Level ---
    float ssgLevel = 1.0f;

    // --- Noise Level ---
    float ssgNoiseLevel = 0.0f; // Noise

    // --- Noise Speed Freq ---
    float ssgNoiseFreq = 12000.0f; // Noise Frequency (Hz)

    // --- Noise On Note ---
    // ノイズ周波数をMIDIキーで変更
    bool ssgNoiseOnNote = false;

    // --- Tone and Noise Mix ---
    // 0.0(Tone) - 1.0(Noise)
    float ssgMix = 0.0f;

    // --- ADSR Filter ---

    // Bypass
    bool ssgAdsrBypass = false;

    // Params
    SimpleAdsr ssgAdsr;

    // --- Waveform ---
    // 0: Pulse, 1: Triangle
    int ssgWaveform = 0;

    // --- Pulse Wave ---

    // Duty Mode
    // 0: Preset, 1: Variable
    int ssgDutyMode = 0;

    // Duty Preset
    // Index (0-8)
    int ssgDutyPreset = 0;

    // Duty Variable Value
    // 0.0-0.5
    float ssgDutyVar = 0.5f;

    // Duty Invert Switch
    bool ssgDutyInvert = false;

    // --- Triangle Params ---

    // Triangle Key Track Switch
    bool ssgTriKeyTrack = true;

    // Triangle Peak Position
    // 0.0=SawDown, 0.5=Tri, 1.0=SawUp
    float ssgTriPeak = 0.5f;

    // Triangle Manual Spped Freq
    float ssgTriFreq = 440.0f;

    // --- Hardware Envelope ---

    // Hardware Envelope Enable Switch
    bool ssgUseHwEnv = false;

    // Hardware Envelope Shape Index
    int ssgEnvShape = 0;

    // Hardware Envelope Period Freq
    float ssgEnvPeriod = 1.0f;


    // [[ Wavetable Parameters ]]


    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int  wtBitDepth = 3;

    // --- Sampling Rate ---
    // 0:96k, 1:55.5k, 2:48k, 3:44.1k, 4:22.05k, 5:16k, 6:8k
    int wtRateIndex = 6;

    int wtTableSize = 0;  // 0:32, 1:64
    int wtWaveform = 0; // Waveform Select 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    // Custom Waveform Data (32 steps)
    std::array<float, 32> wtCustomWave32 = { 0.0f };
    // Custom Waveform Data (64 steps)
    std::array<float, 64> wtCustomWave64 = { 0.0f };
    bool wtModEnable = false;
    float wtModDepth = 0.0f;
    float wtModSpeed = 1.0f; // Ratio or Hz
    float wtLevel = 1.0f;

    // --- ADSR Filter ---

    // Bypass
    bool wtAdsrBypass = false;

    // Params
    SimpleAdsr wtAdsr;


    // [[ Rhythm Parameters ]]

    // --- Rhythm (PCM) ---
    float rhythmLevel = 1.0f;
    // Global master volume
    float rhythmMasterLevel = 1.0f;
    // Parameters for each individual pad
    std::array<RhythmPadParams, MaxRhythmPads> rhythmPads;


    // [[ ADPCM Parameters ]]

    // --- ADPCM ---
    float adpcmLevel = 1.0f;
    float adpcmPan = 0.5f; // ADDED: Pan
    bool adpcmLoop = false; // Default false for drums
    int adpcmRootNote = 60; // Root Key (Middle C)
    int adpcmQualityMode = 6; // Default: ADPCM
    int adpcmRateIndex = 3;   // Default: 16kHz

    float adpcmPcmOffset = 0.0f; // ms
    float adpcmPcmRatio = 1.0f;

    // --- ADSR Filter ---

    // Bypass
    bool adpcmAdsrBypass = false;

    // Params
    SimpleAdsr adpcmAdsr;

    // [[ BEEP Parameters ]]

    // --- Level ---
    float beepLevel = 1.0f;

    // --- Fix Freq ---
    bool beepFixedMode = false;
    float beepFixedFreq = 2000.0f;

    // --- ADSR Filter ---

    // Bypass
    bool beepAdsrBypass = false;

    // Params
    SimpleAdsr beepAdsr;
};
