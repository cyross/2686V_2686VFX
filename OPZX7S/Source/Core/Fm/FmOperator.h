#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <vector>
#include <array>
#include <algorithm>

#include "../Synth/SynthParams.h"
#include "./FmOpParams.h"

// ==========================================================
// Shared FM Operator Class
// ==========================================================
class FmOperator
{
public:
    FmOperator() {}

    double m_hostSampleRate = 44100.0;

    bool m_pitchResetOnLegato = false;

    FmOpParams m_params;

    void virtual setSampleRate(double sampleRate) { m_sampleRate = sampleRate; }
    void virtual setHostSampleRate(double hostRate) { m_hostSampleRate = hostRate; }
    void virtual setParameters(const FmOpParams& params, int feedback);
    void virtual noteOn(float frequency, float velocity, int noteNumber, bool isLegato = false) {};
    void virtual noteOff();
    bool virtual isPlaying() const { return m_state != State::Idle; }
    float virtual getCurrentEnvelope() const { return m_currentLevel; }
    void virtual setPitchBendRatio(float ratio) { m_pitchBendRatio = ratio; }

    // チップ全体に掛かるピッチ倍率。コアが毎サンプル書き換える値を参照する。
    // 毎サンプル全オペレータへ配ると書き込みが増えるので、コア側の 1 つを指す。
    // 繋がないうちは 1.0 を指しているので素通しになる。
    void setGlobalPitchRatioSource(const float* source) {
        m_p_globalPitchRatio = (source != nullptr) ? source : &unityPitchRatio;
    }
    void virtual setExternalFeedbackMode(bool isExternal) { m_isExternalFeedback = isExternal; }
    void virtual pushFeedback(float fbValue) { m_fb2 = m_fb1; m_fb1 = fbValue; }
    float virtual calcWaveform(double phase, int wave);
    void virtual updateIncrementsWithKeyScale();
protected:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;
    // 位相はサイクル単位 (1.0 で 1 周)。
    // 高域でも位相のずれが溜まりにくいよう double で持つ。
    double m_phase = 0.0;
    double m_phaseDelta = 0.0;
    double m_ssgPhase = 0.0;
    float m_ssgEgFreq = 1.0f;
    int m_noteNumber = 60;

    int m_feedback = 0;
    bool m_isExternalFeedback = false;

    float m_currentLevel = 0.0f;
    float m_targetLevel = 0.0f;
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;
    float m_sustainRateDec = 0.0f;
    float m_fb1 = 0.0f; float m_fb2 = 0.0f;

    float m_pitchBendRatio = 1.0f;

    static inline constexpr float unityPitchRatio = 1.0f;
    const float* m_p_globalPitchRatio = &unityPitchRatio;

    float m_susReleaseDec = 0.0f;
    float m_currentReleaseDec = 0.0f;

    void virtual updateEnvelopeState();
};
