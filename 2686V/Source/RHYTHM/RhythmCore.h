#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>

#include "../synth/SynthParams.h"

// Class representing a single drum pad
class RhythmPad
{
public:
    // Holds raw data and converted data
    std::vector<float> m_rawBuffer;
    std::vector<int16_t> m_adpcmBuffer;

    // 状態管理をenumにする
    enum class State { Idle, Playing, Release };
    State m_state = State::Idle;

    double m_position = 0.0;
    double m_bufferSampleRate = 16000.0;
    double m_sourceRate = 44100.0;

    // Parameters
    int m_noteNumber = 0;
    float m_level = 1.0f;
    float m_pan = 0.5f;
    int m_qualityMode = 6; // ADPCM
    int m_rateIndex = 5;   // 16kHz
    bool m_isOneShot = true;

    float m_releaseParam = 0.1f; // パラメータ設定値
    float m_currentEnv = 1.0f;   // 現在の音量倍率 (0.0~1.0)
    float m_releaseDec = 0.0f;   // 1サンプルあたりの減衰量

    void setSampleData(const std::vector<float>& sourceData, double sourceRate);
    void setParameters(const RhythmPadParams& params);
    void triggerRelease(double hostSampleRate);
    void start();
    void stop();
    bool isPlaying() const;
    float getSample(double hostSampleRate, float pitchRatio);
private:
    void refreshAdpcmBuffer();
};

class RhythmCore
{
public:
    std::array<RhythmPad, MaxRhythmPads> pads;
    double m_sampleRate = 44100.0;

    void prepare(double sampleRate);
    void setParameters(const SynthParams& params);
    void setSampleData(int padIndex, const std::vector<float>& data, double rate);
    void noteOn(int midiNote, float velocity);
    void noteOff();
    bool isPlaying() const;
    void setPitchBend(int pitchWheelValue);
    void setModulationWheel(int wheelValue);
    void setPitchBendRatio(float ratio);
    void getSampleStereo(float& outL, float& outR);

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // LFO State
    double m_lfoPhase = 0.0;
};
