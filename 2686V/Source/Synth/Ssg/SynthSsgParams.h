#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"

struct SsgParams
{
    float level = 1.0f;

    // [[ SSG Parameters ]]

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int bitDepth = 3; // Default 8bit (or 4bit to be like real hardware volume steps?)

    // --- Sampling Rate ---
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rateIndex = 3; // Default 49.7kHz

    // --- Tone Level ---
    float tone = 1.0f;

    // --- Noise Level ---
    float noiseLevel = 0.0f; // Noise

    // --- Noise Speed Freq ---
    float noiseFreq = 12000.0f; // Noise Frequency (Hz)

    // --- Noise On Note ---
    // ノイズ周波数をMIDIキーで変更
    bool noiseOnNote = false;

    // --- Tone and Noise Mix ---
    // 0.0(Tone) - 1.0(Noise)
    float mix = 0.0f;

    // --- Fix Freq ---
    bool fixedMode = false;
    float fixedFreq = 2000.0f;

    // --- ADSR Filter ---

    // Params
    AmpAdsrParams adsr;

    // --- Pitch Envelope ---
	PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;

    // --- Waveform ---
    // 0: Pulse, 1: Triangle
    int waveform = 0;

    // --- Pulse Wave ---

    // Duty Mode
    // 0: Preset, 1: Variable
    int dutyMode = 0;

    // Duty Preset
    // Index (0-8)
    int dutyPreset = 0;

    // Duty Variable Value
    // 0.0-0.5
    float dutyVar = 0.5f;

    // Duty Invert Switch
    bool dutyInvert = false;

    // Famicom Mode
    bool dutyFc = false;

    // Famicom Mode Fluctuation
    float dutyFcFluc = 0.2f;

    // --- Triangle Params ---

    // Triangle Key Track Switch
    bool triKeyTrack = true;

    // Triangle Peak Position
    // 0.0=SawDown, 0.5=Tri, 1.0=SawUp
    float triPeak = 0.5f;

    // Triangle Manual Spped Freq
    float triFreq = 440.0f;

    // --- Hardware Envelope ---

    // Hardware Envelope Enable Switch
    bool useHwEnv = false;

    // Hardware Envelope Shape Index
    int envShape = 0;

    // Hardware Envelope Period Freq
    float envPeriod = 1.0f;

    // multiple & detune
    int multiple = 2;      // MULTI: 0~21
    float multipleRatio = 0.5f; // MULTI Ratio 0.5-27.57
    int detune = 0;
    int detune2 = 0;
    int detune3 = 0;

    // LFO Speed Freq (approx 3Hz - 30Hz)
    float lfoAmFreq = 5.0f;
    float lfoPmFreq = 5.0f;

    // LFO Enable Flag
    bool lfoAmEnable = false;
    bool lfoPmEnable = false;

    // LFO Sensitivity

    // Pitch Modulation Sensitivity (0-7/0.0-1.0)
    float lfoPms = 0.0f;

    // Amplitude Modulation Sensitivity (0-3/0.0-1.0)
    float lfoAms = 0.0f;

    // Pitch Modulation Depth (0-127/0.0-1.0)
    float lfoPmd = 0.0f;

    // Amplitude Modulation Depth (0-127/0.0-1.0)
    float lfoAmd = 0.0f;

    // LFO Waveform
    int lfoPmWave = 0;
    int lfoAmWave = 0;

    // LFO AM Smooth Ratio (0.005 - 0.5)
    float lfoAmSmRt = 0.005f;

    // LFO Sync Delay
    int lfoPmSyncDelay = 0;
    int lfoAmSyncDelay = 0;

    // ユニゾン・ハーモニー用
    int unisonVoices = 1;        // 1 to 8
    int unisonDetuneCents = 0;   // cents
    float unisonSpread = 1.0f;   // 0.0 to 1.0 (Stereo width)
};
