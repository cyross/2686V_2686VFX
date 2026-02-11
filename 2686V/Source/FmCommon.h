#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <vector>
#include <array>
#include "Mode.h"

// ==========================================================
// Shared FM Operator Class
// ==========================================================
class FmOperator
{
public:
    FmOperator() {}
    void setSampleRate(double sampleRate) { m_sampleRate = sampleRate; }

    // パラメータ適用
    // useSsgEg: OPNAのみtrue
    // useWaveSelect: OPLのみtrue
    void setParameters(const FmOpParams& params, float feedback, bool useSsgEg, bool useWaveSelect, bool useOpmEg = false, float ssgEgFreq = 1.0f)
    {
        m_params = params;
        m_feedback = feedback;
        m_useSsgEg = useSsgEg;
        m_ssgEgFreq = ssgEgFreq;
        m_useWaveSelect = useWaveSelect;
		m_useOpmEg = useOpmEg;

        // OPNA以外の場合はSSG-EGパラメータを無視
        if (!m_useSsgEg) m_params.ssgEg = 0;

        // OPNA/OPNの場合はWaveSelectを無視(Sine固定)
        if (!m_useWaveSelect) m_params.waveSelect = 0;
    }

    void noteOn(float frequency, float velocity, int noteNumber)
    {
        m_phase = 0.0;
        m_ssgPhase = 0.0;
        m_noteNumber = noteNumber;

        // Fixed Mode
        float baseFreq = frequency;
        if (m_params.fixedMode) baseFreq = m_params.fixedFreq;

        // Multi & Detune
        float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;
        float dtVal = (float)m_params.detune * 0.5f; // DT1

        // DT2 (OPM Coarse Detune)
        // YM2151: 0=0, 1=+approx 1.414, 2=+approx 1.58, 3=+approx 1.73
        // 0: x1.0
        // 1: x1.41 (600 cent up)
        // 2: x1.58 (780 cent up)
        // 3: x1.78 (950 cent up)
        float dt2Scale = 1.0f;
        switch (m_params.detune2 & 3) {
            case 0: dt2Scale = 1.0f; break;
            case 1: dt2Scale = 1.414f; break;
            case 2: dt2Scale = 1.581f; break;
            case 3: dt2Scale = 1.781f; break;
        }

        float finalFreq = (baseFreq + dtVal) * mul * dt2Scale;

        m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

        // TL Calculation
        float tlGain = 1.0f - m_params.totalLevel;
        if (tlGain < 0.0f) tlGain = 0.0f;

        float kslAttenuation = 1.0f;
        if (m_params.keyScaleLevel > 0)
        {
            float octaveDiff = (float)(noteNumber - 48) / 12.0f;
            if (octaveDiff < 0) octaveDiff = 0;
            float dbPerOct = 0.0f;
            switch (m_params.keyScaleLevel) {
                case 1: dbPerOct = 1.5f; break;
                case 2: dbPerOct = 3.0f; break;
                case 3: dbPerOct = 6.0f; break;
            }
            float totalDb = dbPerOct * octaveDiff;
            kslAttenuation = std::pow(10.0f, -totalDb / 20.0f);
        }
        m_targetLevel = velocity * tlGain * kslAttenuation;
        m_state = State::Attack;

        m_fb1 = 0.0f; m_fb2 = 0.0f;

        updateIncrementsWithKeyScale();
    }

    void noteOff() { m_state = State::Release; }
    bool isPlaying() const { return m_state != State::Idle; }
    float getCurrentEnvelope() const { return m_currentLevel; }

    void setPitchBendRatio(float ratio) { m_pitchBendRatio = ratio; }

    // --- Processing ---
    // output: 結果書き込み先
    // modulator: FM変調入力
    // lfoAmp: AM変調 (1.0 = 変調なし)
    // lfoPitch: PM変調 (1.0 = 変調なし)
    void getSample(float& output, float modulator, float lfoAmp = 1.0f, float lfoPitch = 1.0f)
    {
        if (m_state == State::Idle) { output = 0.0f; return; }

        // 1. ADSR Update
        updateEnvelopeState();

        // 2. Calculate Envelope Value
        // ここではエンベロープの形状(0.0-1.0)のみを取得し、
        // 最終的な音量は出力段で掛けます。
        float envVal = m_currentLevel;

        // SSG-EG (OPNA Only)
        if (m_useSsgEg && m_params.ssgEg > 0) {
            envVal *= getSsgEnvelopeLevel(m_ssgPhase);

            // 音程(m_phaseDelta)ではなく、設定した周波数(m_ssgEgFreq)で進める
            // これにより、音程に関わらず一定の速度でシュワシュワします
            double phaseInc = (double)m_ssgEgFreq / m_sampleRate;
            m_ssgPhase += phaseInc;
        }

        // LFO AM (OPNA Only)
        if (m_params.amEnable) {
            envVal *= lfoAmp;
        }

        // 3. Phase & Waveform
        // Feedback
        float feedbackMod = 0.0f;
        if (m_feedback > 0.0f) {
            float fbScale = std::pow(2.0f, m_feedback - 4.0f);
            if (m_feedback < 1.0f) fbScale = 0.0f;
            feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale;
        }

        // LFO PM (OPNA Only)
        float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio;

        currentPhaseDelta *= lfoPitch;

        float modulatedPhase = m_phase + modulator + feedbackMod;

        // Wave Select (OPL Only / Standard Sine)
        // ここで m_targetLevel (Velocity/TL) を適用します！
        // これにより、Attackが1.0に張り付くスパイクノイズが解消されます。
        output = calculateWave(modulatedPhase) * envVal * m_targetLevel;

        // Feedback Memory Update
        // フィードバックはGain適用後の値を使うのが一般的ですが、FMの構成によっては適用前の場合もあります。今回は適用後でOKです。
        m_fb2 = m_fb1; m_fb1 = output;

        // Phase Advance
        m_phase += currentPhaseDelta;
        if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
    }

private:
    float calculateWave(float phase)
    {
        float p = std::fmod(phase, 2.0f * juce::MathConstants<float>::pi);
        if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;
        float s = std::sin(p);

        if (!m_useWaveSelect) return s; // Default Sine

        switch (m_params.waveSelect) {
            case 0: return s;
            case 1: return (s > 0.0f) ? s : 0.0f; // Half
            case 2: return std::abs(s); // Abs
            case 3: return (p < 0.5f * juce::MathConstants<float>::pi) ? s : 0.0f; // Pulse
            default: return s;
        }
    }

    float getSsgEnvelopeLevel(double p)
    {
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);
        switch (m_params.ssgEg) {
            case 2:  return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
            case 4:  return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f; 
            case 6: return isEven ? (1.0f - (float)subPos) : (float)subPos;
            case 8: return (cycle == 0) ? (1.0f - (float)subPos) : 1.0f;
            case 9: return (cycle == 0) ? (float)subPos : 0.0f;
            case 11: return (cycle == 0) ? (float)subPos : 1.0f;
            case 13: return isEven ? (float)subPos : (1.0f - (float)subPos);
            case 15: return (cycle == 0) ? (float)subPos : 0.0f;
            default: return 1.0f;
        }
    }

    void updateEnvelopeState()
    {
        if (m_state == State::Attack) {
            m_currentLevel += m_attackInc;
            if (m_currentLevel >= 1.0f) { m_currentLevel = 1.0f; m_state = State::Decay; }
        }
        else if (m_state == State::Decay) {
            // Decayのターゲット判定ロジックを変更
            // 以前: float limitLevel = m_targetLevel * m_params.sustain;
            // 修正: エンベロープは0-1で正規化されたので、sustain値(0-1)と比較するだけでOK
            float limitLevel = m_params.sustain;
            if (m_currentLevel > limitLevel) {
                m_currentLevel -= m_decayDec;
                if (m_currentLevel <= limitLevel) {
                    m_currentLevel = limitLevel;
                    m_state = State::Sustain;
                }
            }
            else {
                m_currentLevel = limitLevel;
                m_state = State::Sustain;
            }
        }
        else if (m_state == State::Sustain) {
            // サステインフェーズのロジック

            // EG-TYP=1 (Hold): 減衰せずレベルを維持
            if (m_params.egType) {
                // そのまま維持 (何もしない)
            }
            // EG-TYP=0 (Normal): SR (Sustain Rate) に従って0まで減衰
            else {
                // SR=0 なら減衰しない (無限サステイン)
                if (m_sustainRateDec > 0.0f) {
                    m_currentLevel -= m_sustainRateDec;
                    if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
                }
            }
        }
        else if (m_state == State::Release) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
    }

    void updateIncrementsWithKeyScale()
    {
        if (m_sampleRate <= 0.0) return;
        float rateScale = 1.0f;
        if (m_params.keyScale > 0) {
            float noteFactor = (float)(m_noteNumber) / 128.0f;
            rateScale = 1.0f + ((float)m_params.keyScale * noteFactor * 2.0f);
        }
        auto calcInc = [&](float param) { return 1.0f / (float)(std::max(0.001f, param / rateScale) * m_sampleRate); };
        m_attackInc  = calcInc(m_params.attack);
        m_decayDec   = calcInc(m_params.decay);
        m_releaseDec = calcInc(m_params.release);

        if (m_params.sustainRate <= 0.001f)
        {
            m_sustainRateDec = 0.0f;
        }
        else
        {
            // 値が大きいほど速く減衰する
            // 簡易計算: 0.001(param) -> 5.0s, 1.0(param) -> 0.005s
            float srTime = std::max(0.001f, 5.0f * (1.0f - m_params.sustainRate));
            m_sustainRateDec = 1.0f / (float)((srTime / rateScale) * m_sampleRate);
        }
    }

    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;
    float m_phase = 0.0f; float m_phaseDelta = 0.0f;
    double m_ssgPhase = 0.0;
    float m_ssgEgFreq = 1.0f;
    int m_noteNumber = 60;

    FmOpParams m_params;
    float m_feedback = 0.0f;
    bool m_useSsgEg = false;
    bool m_useWaveSelect = false;
    bool m_useOpmEg = false;

    float m_currentLevel = 0.0f; float m_targetLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f; float m_sustainRateDec = 0.0f;
    float m_fb1 = 0.0f; float m_fb2 = 0.0f;

    float m_pitchBendRatio = 1.0f;
};
