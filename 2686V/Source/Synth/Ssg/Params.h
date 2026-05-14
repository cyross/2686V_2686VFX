#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/Params.h"
#include "../../Effect/Envelope/Pitch/Adsr/Params.h"

struct SsgParams
{
    // [[ SSG Parameters ]]

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int bitDepth = 3; // Default 8bit (or 4bit to be like real hardware volume steps?)

    // --- Sampling Rate ---
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rateIndex = 3; // Default 49.7kHz

    // --- Tone Level ---
    float level = 1.0f;

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

    // --- ADSR Filter ---

    // Params
    AmpAdsrParams adsr;

    // --- Pitch Envelope ---
	PitchAdsrParams pitchAdsr;

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

    // detune
    int detune = 0;
    int detune2 = 0;
};
