#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"

struct AdpcmParams
{
    // [[ ADPCM Parameters ]]

    // --- ADPCM ---
    float level = 1.0f;
    float pan = 0.5f; // ADDED: Pan
    bool loop = false; // Default false for drums
    int rootNote = 60; // Root Key (Middle C)
    int qualityMode = 6; // Default: ADPCM
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rateIndex = 7;   // Default: 16kHz
    int interpolationMode = 1;

    float tone = 1.0f;
    float noiseLevel = 0.0f; // Noise
    float noiseFreq = 12000.0f; // Noise Frequency (Hz)
    float mix = 0.0f;

    float offset = 0.0f; // ms
    float ratio = 1.0f;
    bool loopPointEnable = false;
    float loopPointStart = 0.0f; // 0.0 to 1.0
    float loopPointEnd = 1.0f;   // 0.0 to 1.0

    // --- Fix Freq ---
    bool fixedMode = false;
    float fixedFreq = 2000.0f;

	// AmpEnvelope Params
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
};
