#pragma once
#include <JuceHeader.h>

static constexpr int MaxRhythmPads = 8;

static constexpr int MaxFmOperators = 8;

enum class OscMode
{
    OPNA = 0,   // YM2608
    OPN = 1,    // YM2203
    OPL = 2,    // YM3526 (2op + WaveSelect)
    OPLL = 3, // YM2413 (MSX-MUSIC)
    OPL3 = 4, // YMF262
    OPM = 5,   // YM2151 (X68000/Arcade)
    SSG = 6,
    WAVETABLE = 7,
    RHYTHM = 8,
    ADPCM = 9,
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
        case OscMode::OPNA: return "OPNA";
        case OscMode::OPN:  return "OPN";
        case OscMode::OPL:  return "OPL";
        case OscMode::OPLL: return "OPLL";
        case OscMode::OPL3: return "OPL3";
        case OscMode::OPM:  return "OPM";
        case OscMode::SSG:  return "SSG";
        case OscMode::WAVETABLE: return "WAVETABLE";
        case OscMode::RHYTHM: return "RHYTHM";
        case OscMode::ADPCM:  return "ADPCM";
        default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPNA") return OscMode::OPNA;
    if (name == "OPN")  return OscMode::OPN;
    if (name == "OPL")  return OscMode::OPL;
    if (name == "OPLL") return OscMode::OPLL;
    if (name == "OPL3") return OscMode::OPL3;
    if (name == "OPM")  return OscMode::OPM;
    if (name == "SSG")  return OscMode::SSG;
    if (name == "WAVETABLE") return OscMode::WAVETABLE;
    if (name == "RHYTHM") return OscMode::RHYTHM;
    if (name == "ADPCM")  return OscMode::ADPCM;
    return OscMode::OPNA; // Default
}

struct FmOpParams
{
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

    bool amEnable = false;
    bool vibEnable = false; // VIB (LFO Pitch)
    bool egType = false;    // EG-TYP (Sustain Mode)

    // CSM/Fix Mode
    bool fixedMode = false;
    float fixedFreq = 440.0f;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;
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
};

struct SimpleAdsr { float a = 0.01f, d = 0.0f, s = 1.0f, r = 0.2f; };

struct SynthParams
{
    OscMode mode = OscMode::OPNA;

    // --- FM Parameters ---
	int opllPreset = 0; // OPLL Preset Instrument Index
    int algorithm = 7;
    float feedback = 0.0f;
    // FM LFO (OPNA Global)
    float lfoFreq = 5.0f; // LFO Speed (approx 3Hz - 30Hz)
    int pms = 0;          // Pitch Modulation Sensitivity (0-7)
    int ams = 0;          // Amplitude Modulation Sensitivity (0-3)
	int lfoWave = 2; // LFO Waveform (0:Saw, 1:Square, 2:Triangle, 3:Random)
    std::array<FmOpParams, MaxFmOperators> fmOp;

    // ADD: FM Quality & Rate (Shared across all FM modes)
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int fmBitDepth = 4; // Default: Raw (32bit float) or 3 (8bit) depending on preference
    // 0:96k, 1:55.5k, 2:48k, 3:44.1k, 4:22.05k, 5:16k, 6:8k
    int fmRateIndex = 1; // Default: 55.5kHz (Typical FM Chip Rate)

    // --- SSG Parameters ---
    float ssgLevel = 1.0f;
    float ssgNoiseLevel = 0.0f; // Noise
	float ssgNoiseFreq = 12000.0f; // Noise Frequency (Hz)
    float ssgMix = 0.0f; // 0.0(Tone) ~ 1.0(Noise)
    int ssgWaveform = 0; // 0: Pulse, 1: Triangle

    // ADD: SSG Quality & Rate
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int ssgBitDepth = 3; // Default 8bit (or 4bit to be like real hardware volume steps?)
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    int ssgRateIndex = 1; // Default 55.5kHz (Standard SSG/OPNA rate)

    // SSG Hardware Envelope Params
    bool ssgUseHwEnv = false;
    int ssgEnvShape = 0;
    float ssgEnvPeriod = 1.0f;

    // Duty Cycle Params
    int ssgDutyMode = 0;      // 0: Preset, 1: Variable
    int ssgDutyPreset = 0;    // Preset Index (0-8)
    float ssgDutyVar = 0.5f;  // Variable Value (0.0-0.5)
    bool ssgDutyInvert = false; // Invert Switch

    // Triangle Params
    bool ssgTriKeyTrack = true;
    float ssgTriPeak = 0.5f; // 0.0=SawDown, 0.5=Tri, 1.0=SawUp
    float ssgTriFreq = 440.0f;

    // ADSR Bypass
    bool ssgAdsrBypass = false;

    // --- Wavetable ---
    int wtBitDepth = 3;   // 0:4bit, 1:5bit, 2:6bit, 3:8bit
    int wtTableSize = 0;  // 0:32, 1:64
    int wtWaveform = 0; // Waveform Select 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    int wtRateIndex = 6;  // Default: 16kHz
    // Custom Waveform Data (32 steps)
    std::array<float, 32> wtCustomWave32 = { 0.0f };
    // Custom Waveform Data (64 steps)
    std::array<float, 64> wtCustomWave64 = { 0.0f };
    bool wtModEnable = false;
    float wtModDepth = 0.0f;
    float wtModSpeed = 1.0f; // Ratio or Hz
    float wtLevel = 1.0f;

    // --- Rhythm (PCM) ---
    float rhythmLevel = 1.0f;
    // Global master volume
    float rhythmMasterLevel = 1.0f;
    // Parameters for each individual pad
    std::array<RhythmPadParams, MaxRhythmPads> rhythmPads;

    // --- ADPCM ---
    float adpcmLevel = 1.0f;
    float adpcmPan = 0.5f; // ADDED: Pan
    bool adpcmLoop = false; // Default false for drums
    int adpcmRootNote = 60; // Root Key (Middle C)
    int adpcmQualityMode = 6; // Default: ADPCM
    int adpcmRateIndex = 3;   // Default: 16kHz

    SimpleAdsr ssgAdsr, adpcmAdsr, wtAdsr;
};
