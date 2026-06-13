#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"

struct Wt2Params
{
    // [[ Wavetable Parameters ]]

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int  bitDepth = 3;

    // --- Sampling Rate ---
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rateIndex = 3; // Default 49.7kHz

    int tableSize = 0; // 0:32, 1:64
    int waveform = 8;  // Waveform Select 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom

    // --- カスタム波形の解像度 ---
    // 0: 4-bit (0-15, センター8)
    // 1: 5-bit (0-31, センター16)
    // 2: 6-bit (0-63, センター32)
    // 3: 7-bit (0-127, センター64)
    // 4: 8-bit (0-255, センター128)
    int customWaveResolution = 0;

    // Custom Waveform Data (32 steps)
    std::array<int, 32> customWave32 = { 0 };
    // Custom Waveform Data (64 steps)
    std::array<int, 64> customWave64 = { 0 };
    // Custom Waveform Data (128 steps)
    std::array<int, 128> customWave128 = { 0 };
    // Custom Waveform Data (256 steps)
    std::array<int, 256> customWave256 = { 0 };
    bool modEnable = false;
    float modDepth = 0.0f;
    float modSpeed = 1.0f; // Ratio or Hz
    float level = 1.0f;

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

    Wt2Params() {
        customWave32.fill(8);
        customWave64.fill(8);
        customWave128.fill(8);
        customWave256.fill(8);
    }
};
