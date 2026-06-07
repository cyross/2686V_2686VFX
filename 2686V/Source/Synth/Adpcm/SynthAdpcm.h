#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

// --- Core Class ---

class AdpcmCore : public SynthCore
{
public:
    AdpcmCore(): SynthCore() {}

    void prepare(double sampleRate) override;
	void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void setSampleData(const std::vector<float>& sourceData, double sourceRate);
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getCurrentPan() const;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニー用
    void setUnisonParams(int index, int total, float detune, float spread) {
        m_unisonIndex = index;
        m_unisonTotal = total;
        m_unisonDetuneAmt = detune;
        m_unisonSpreadAmt = spread;

        // ユニゾンのインデックスに応じて位相を均等にずらす (0.0 〜 1.0)
        // (例: 3ボイスなら 0.0, 0.33, 0.66)
        m_unisonPhaseOffset = (total > 1) ? ((float)index / (float)total) : 0.0f;
    }
private:
    double m_sampleRate = 44100.0; // DAW Host Sample Rate
    double m_sourceRate = 44100.0;
    double m_bufferSampleRate = 16000.0; // Internal Data Sample Rate

    // Processed ADPCM Data (stored as int16 for playback)
    std::vector<float> m_rawBuffer;     // Raw Data (32bit)
    std::vector<int16_t> m_pcmBuffer;   // Processed Data (4bit ADPCM/DPCM)
    int m_qualityMode = 6;
    int m_rateIndex = 3;

    double m_position = 0.0;
    float m_pitchRatio = 1.0f;
    int m_rootNote = 60; // Middle C

    // Params
    float m_level = 1.0f;
    float m_pan = 0.5f;
	float m_panL = 1.0f;
	float m_panR = 1.0f;

    float m_pcmOffset = 0.0f;
    float m_pcmRatio = 1.0f;

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    FixMode m_fixMode;

    float m_currentLevel = 0.0f;
    float m_baseLevel = 0.0f;

    bool m_isLooping = false;
    bool m_hasFinished = false;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // LFO
    double m_lfoPhase = 0.0;

    void refreshPcmBuffer();

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    int m_unisonIndex = 0;
    int m_unisonTotal = 1;
    float m_unisonDetuneAmt = 0.0f;
    float m_unisonSpreadAmt = 0.0f;
    float m_unisonPhaseOffset = 0.0f;
};
