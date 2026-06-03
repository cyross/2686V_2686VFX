#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "../../Effect/Envelope/Amp/Adsr/Core.h"
#include "../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../Advanced/Curve/Core.h"

// Class representing a single drum pad
class RhythmPad
{
public:
    // Holds raw data and converted data
    std::vector<float> m_rawBuffer;
    std::vector<int16_t> m_adpcmBuffer;

    double m_position = 0.0;
    double m_bufferSampleRate = 16000.0;
    double m_sourceRate = 44100.0;

    // Parameters
    int m_noteNumber = 0;
    float m_level = 1.0f;
    float m_pan = 0.5f;
	float m_panL = 1.0f;
    float m_panR = 1.0f;
    int m_qualityMode = 6; // ADPCM
    int m_rateIndex = 5;   // 16kHz
    bool m_isOneShot = true;

    float m_pcmOffset = 0.0f;
    float m_pcmRatio = 1.0f;

    float m_releaseParam = 0.1f; // パラメータ設定値
    float m_currentEnv = 1.0f;   // 現在の音量倍率 (0.0~1.0)
    float m_baseLevel = 0.0f;
    float m_releaseDec = 0.0f;   // 1サンプルあたりの減衰量

	void prepare(double hostSampleRate);
    void setSampleRate(double sampleRate);
    void setSampleData(const std::vector<float>& sourceData, double sourceRate);
    void setParameters(const RhythmPadParams& params);
    void triggerRelease(double hostSampleRate);
    void start(float velocity, bool isLegato);
    void stop();
    bool isPlaying() const;
    float getSample(double hostSampleRate, float pitchRatio);
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニー用
    // ユニゾン時の位相オフセットを受け取る関数
    void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
    void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;

    void refreshAdpcmBuffer();

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    float m_unisonPhaseOffset = 0.0f;
};

class RhythmCore : public SynthCore
{
public:
    RhythmCore() : SynthCore() {}

    std::array<RhythmPad, MaxRhythmPads> pads;
    double m_sampleRate = 44100.0;

    void prepare(double sampleRate);
    void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params);
    void setSampleData(int padIndex, const std::vector<float>& data, double rate);
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    void setPitchBendRatio(float ratio) override;
    void getSampleStereo(float& outL, float& outR);
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setCurveCore(CurveCore* p_curveCore);

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // LFO State
    double m_lfoPhase = 0.0;

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
    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    int m_unisonIndex = 0;
    int m_unisonTotal = 1;
    float m_unisonDetuneAmt = 0.0f;
    float m_unisonSpreadAmt = 0.0f;
    float m_unisonPhaseOffset = 0.0f;
};
