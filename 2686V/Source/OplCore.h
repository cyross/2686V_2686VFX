#pragma once
#include "FmCommon.h"

// ==========================================================
// OPL (YM3526/3812) Core
// Features: 2 Operators, 2 Algorithms (FM/AM), Wave Select
// ==========================================================
class OplCore
{
public:
    OplCore() {}

    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);
        m_rateAccumulator = 1.0;
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm; // 0:Serial(FM), 1:Parallel(AM)

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

        // OP1: Feedbackあり, OP2: なし
        // OPL: SSG-EG=False, WaveSelect=True
        m_operators[0].setParameters(params.fmOp[0], params.feedback, false, true);
        m_operators[1].setParameters(params.fmOp[1], 0.0f,            false, true);
    }

    void noteOn(float freq, float velocity) {
        float gain = std::max(0.01f, velocity);
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        m_operators[0].noteOn(freq, gain, noteNum);
        m_operators[1].noteOn(freq, gain, noteNum);
        m_rateAccumulator = 1.0;
    }

    void noteOff() {
        m_operators[0].noteOff();
        m_operators[1].noteOff();
    }

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
        double targetRate = getTargetRate();
        m_rateAccumulator += targetRate / m_hostSampleRate;

        if (m_rateAccumulator >= 1.0) {
            m_rateAccumulator -= 1.0;

            // --- Software LFO for Vibrato ---
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;
            if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

            // Triangle LFO (-1.0 ~ 1.0)
            float lfoValue = 0.0f;
            if (m_lfoPhase < 0.25)      lfoValue = (float)(m_lfoPhase * 4.0);
            else if (m_lfoPhase < 0.75) lfoValue = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
            else                        lfoValue = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

            // Pitch Mod Depth
            float wheelDepth = m_modWheel * 0.05f;
            float lfoPitchMod = 1.0f + (lfoValue * wheelDepth);
            float lfoAmpMod = 1.0f;

            // -------------------------------

            float out1, out2;

            // getSample に LFO 値を渡す
            m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

            float finalOut = 0.0f;
            if (m_algorithm == 0) { // Serial (FM)
                // OP1 -> OP2
                m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
                finalOut = out2;
            }
            else { // Parallel (AM)
                m_operators[1].getSample(out2, 0.0f, lfoAmpMod, lfoPitchMod);
                finalOut = (out1 + out2) * 0.5f;
            }

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
    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    float m_modWheel = 0.0f;
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
};
