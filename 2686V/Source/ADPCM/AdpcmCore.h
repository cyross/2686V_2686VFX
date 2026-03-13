#pragma once

#include <JuceHeader.h>

#include "../synth/SynthParams.h"
#include "../synth/Pcm.h"
#include "../synth/SynthCore.h"

// --- Core Class ---

class AdpcmCore : public SynthCore
{
public:
    AdpcmCore(): SynthCore() {}

    void prepare(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void setSampleData(const std::vector<float>& sourceData, double sourceRate);
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getCurrentPan() const;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0; // DAW Host Sample Rate
    double m_sourceRate = 44100.0;
    double m_bufferSampleRate = 16000.0; // Internal Data Sample Rate

    // Processed ADPCM Data (stored as int16 for playback)
    std::vector<float> m_rawBuffer;       // Raw Data (32bit)
    std::vector<int16_t> m_adpcmBuffer;   // Processed Data (4bit ADPCM)
    int m_qualityMode = 6;
    int m_rateIndex = 3;

    double m_position = 0.0;
    float m_pitchRatio = 1.0f;
    int m_rootNote = 60; // Middle C

    // Params
    float m_level = 1.0f;
    float m_pan = 0.5f;

    float m_pcmOffset = 0.0f;
    float m_pcmRatio = 1.0f;

    SimpleAdsr m_adsr;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;

    bool m_isLooping = false;
    bool m_hasFinished = false;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // LFO
    double m_lfoPhase = 0.0;

    void refreshAdpcmBuffer();
    void processAdsr();
    void updateIncrements();
};
