#pragma once
#include "FmCommon.h"

// ==========================================================
// OPLL (YM2413) Core
// Features: 2 Operators, User Patch Only (Full Edit)
// ==========================================================
class OpllCore
{
public:
    OpllCore() {}
    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);
        m_rateAccumulator = 1.0;
    }

    void setParameters(const SynthParams& params) {
        if (m_rateIndex != params.fmRateIndex) {
            m_rateIndex = params.fmRateIndex;
            double target = getTargetRate();
            for (auto& op : m_operators) op.setSampleRate(target);
        }

        switch (params.fmBitDepth) {
            case 0: m_quantizeSteps = 15.0f; break;
            case 1: m_quantizeSteps = 31.0f; break;
            case 2: m_quantizeSteps = 63.0f; break;
            case 3: m_quantizeSteps = 255.0f; break;
            case 4: m_quantizeSteps = 0.0f; break;
            default: m_quantizeSteps = 0.0f; break;
        }

        for (int i = 0; i < 2; ++i)
        {
            FmOpParams p = params.fmOp[i];
            p.keyScale = (params.fmOp[i].keyScale > 0) ? 3 : 0;
            float fb = (i == 0) ? params.feedback : 0.0f;
            m_operators[i].setParameters(p, fb, false, true);
        }
    }

    void noteOn(float freq, float velocity) {
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        m_operators[0].noteOn(freq, velocity, noteNum);
        m_operators[1].noteOn(freq, velocity, noteNum);
        m_rateAccumulator = 1.0;
    }
    void noteOff() { m_operators[0].noteOff(); m_operators[1].noteOff(); }
    bool isPlaying() const {
        for (const auto& op : m_operators) {
            if (op.isPlaying()) return true;
        }
        return false;
    }
    // ピッチベンド (0 - 16383, Center=8192)
    void setPitchBend(int pitchWheelValue)
    {
        // 範囲を -1.0 ～ 1.0 に正規化
        float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

        // 半音単位のレンジ (例: +/- 2半音)
        float semitones = 2.0f;

        // 比率計算: 2^(semitones / 12)
        // norm * semitones で変化量を決定
        float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

        // 全オペレーターに適用
        for (auto& op : m_operators) op.setPitchBendRatio(ratio);
    }

    // モジュレーションホイール (0 - 127)
    void setModulationWheel(int wheelValue)
    {
        // 0.0 ～ 1.0 に正規化
        m_modWheel = (float)wheelValue / 127.0f;
    }

    float getSample() {
        // --- OPLL LFO Generation ---
        // AM: ~3.7 Hz, VIB: ~6.4 Hz
        double targetRate = getTargetRate();
        m_rateAccumulator += targetRate / m_hostSampleRate;

        if (m_rateAccumulator >= 1.0) {
            m_rateAccumulator -= 1.0;

            // OPLL LFO Logic (Run at Target Rate)
            m_amPhase += (3.7 / targetRate);
            if (m_amPhase >= 1.0) m_amPhase -= 1.0;

            m_vibPhase += (6.4 / targetRate);
            if (m_vibPhase >= 1.0) m_vibPhase -= 1.0;

            float amVal = (m_amPhase < 0.5) ? (float)m_amPhase * 2.0f : (float)(1.0 - m_amPhase) * 2.0f;
            float vibVal = (m_vibPhase < 0.5) ? (float)m_vibPhase * 2.0f : (float)(1.0 - m_vibPhase) * 2.0f;

            float lfoAmp = 1.0f - (amVal * 0.5f);

            // モジュレーションホイールの値を加算
            // vibValは0～1なので、(vibVal - 0.5)は -0.5～0.5
            // 元の揺れ幅(0.01)に加え、ホイール最大時に 0.1 を足すことで、
            // (-0.5 ~ 0.5) * 0.1 = +/- 0.05 (約半音) の揺れを追加
            float depth = 0.01f + (m_modWheel * 0.1f);
            float lfoPitch = 1.0f + ((vibVal - 0.5f) * depth);

            float out1, out2;
            m_operators[0].getSample(out1, 0.0f, lfoAmp, lfoPitch);
            m_operators[1].getSample(out2, out1, lfoAmp, lfoPitch);

            float finalOut = out2;

            if (m_quantizeSteps > 0.0f) {
                if (finalOut > 1.0f) finalOut = 1.0f; else if (finalOut < -1.0f) finalOut = -1.0f;
                float norm = (finalOut + 1.0f) * 0.5f;
                float quantized = std::floor(norm * m_quantizeSteps) / m_quantizeSteps;
                m_lastSample = (quantized * 2.0f) - 1.0f;
            }
            else {
                m_lastSample = finalOut;
            }
        }
        return m_lastSample;
    }

private:
    double getTargetRate() const {
        switch (m_rateIndex) {
            case 0: return 96000.0;
            case 1: return 55500.0;
            case 2: return 48000.0;
            case 3: return 44100.0;
            case 4: return 22050.0;
            case 5: return 16000.0;
            case 6: return 8000.0;
            default: return 55500.0;
        }
    }

    std::array<FmOperator, 2> m_operators;
    double m_hostSampleRate = 44100.0;
    // Rate & Quality
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    double m_amPhase = 0.0;
    double m_vibPhase = 0.0;

    float m_modWheel = 0.0f;
};
