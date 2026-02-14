#pragma once
#include "FmCommon.h"

// ==========================================================
// OPNA (YM2608) Core
// Features: 4 Operators, 8 Algorithms, SSG-EG, Hardware LFO
// ==========================================================
class OpnaCore
{
public:
    OpnaCore() {}

    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

        // Update operators with current target rate
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);

        m_lfoPhase = 0.0;
        m_rateAccumulator = 1.0; // Ready to render
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;
        m_lfoFreq = params.lfoFreq;
        m_pms = params.pms;
        m_ams = params.ams;

        if (m_rateIndex != params.fmRateIndex) {
            m_rateIndex = params.fmRateIndex;
            double target = getTargetRate();
            for (auto& op : m_operators) op.setSampleRate(target);
        }

        switch (params.fmBitDepth) {
        case 0: m_quantizeSteps = 15.0f; break; // 4bit
        case 1: m_quantizeSteps = 31.0f; break; // 5bit
        case 2: m_quantizeSteps = 63.0f; break; // 6bit
        case 3: m_quantizeSteps = 255.0f; break; // 8bit
        case 4: m_quantizeSteps = 0.0f; break;   // Raw
        default: m_quantizeSteps = 0.0f; break;
        }

        for (int i = 0; i < 4; ++i) {
            float fb = 0.0f;

            if (i == 0) // OP0
            {
                fb = params.feedback;
            }
            else if (i == 2) // OP2
            {
                fb = params.feedback2;
            }

            // OPNA: SSG-EG=True, WaveSelect=False
            // params.fmSsgEgFreq を渡す (第6引数)
            // 第5引数(useOpmEg)は false
            m_operators[i].setParameters(params.fmOp[i], fb, true, false, false, params.fmOp[i].fmSsgEgFreq);
            m_opMask[i] = params.fmOp[i].mask;
        }
    }

    void noteOn(float freq, float velocity) {
        float gain = std::max(0.01f, velocity);
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);

        m_rateAccumulator = 1.0; // Reset timing
    }

    void noteOff() { for (auto& op : m_operators) op.noteOff(); }
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
        // --- Rate Emulation ---
        double targetRate = getTargetRate();
        m_rateAccumulator += targetRate / m_hostSampleRate;

        if (m_rateAccumulator >= 1.0) {
            m_rateAccumulator -= 1.0;

            // --- Actual Synthesis (Run at Target Rate) ---

            // LFO Update (Using Target Rate)
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;
            if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

            float lfoValue = 0.0f;
            if (m_lfoPhase < 0.25)      lfoValue = (float)(m_lfoPhase * 4.0);
            else if (m_lfoPhase < 0.75) lfoValue = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
            else                        lfoValue = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

            // AMS
            float lfoAmpMod = 1.0f;
            if (m_ams > 0) {
                float depths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
                lfoAmpMod = 1.0f - (std::abs(lfoValue) * depths[m_ams & 3]);
            }

            // PMS (Pitch Modulation Sensitivity)
            // ★修正: モジュレーションホイールの効果を加算
            // PMSが設定されていればその深さ、ホイールがあればさらに深く、
            // PMSが0でもホイールを上げればビブラートがかかるようにする(0.05f = 約半音の半分程度の揺れ)
            float pmDepth = 0.0f;
            if (m_pms > 0) {
                float depths[] = { 0.0f, 0.001f, 0.002f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f };
                pmDepth = depths[m_pms & 7];
            }

            // ホイールによる追加深度 (最大 0.03 程度 = 30セント強)
            float wheelDepth = m_modWheel * 0.03f;

            float lfoPitchMod = 1.0f + (lfoValue * (pmDepth + wheelDepth));

            // Routing
            float out1, out2, out3, out4;
            float finalOut = 0.0f;

            // Operators update their state here
            m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

            if (m_opMask[0]) out1 = 0.0f; // Mask

            switch (m_algorithm) {
                case 0:
                    m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod);
                    finalOut = out4;
                    break;
                case 1:
                    m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod);
                    finalOut = out4;
                    break;
                case 2:
                    m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, out3 + out1, lfoAmpMod, lfoPitchMod);
                    finalOut = out4;
                    break;
                case 3:
                    m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod);
                    finalOut = out2 + out4;
                    break;
                case 4:
                    m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);
                    finalOut = out2 + out3 + out4;
                    break;
                case 5:
                    m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, out1, lfoAmpMod, lfoPitchMod);
                    finalOut = out2 + out3 + out4;
                    break;
                case 6:
                    m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);
                    finalOut = out2 + out3 + out4;
                    break;
                default:
                    m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[1]) out2 = 0.0f; // Mask
                    m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
                    if (m_opMask[2]) out3 = 0.0f; // Mask
                    m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);
                    finalOut = out1 + out2 + out3 + out4;
                    break;
            }

            // Quantization
            if (m_quantizeSteps > 0.0f) {
                // Clamp first to avoid blowing up logic
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

    std::array<FmOperator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };

    double m_hostSampleRate = 44100.0;
    int m_algorithm = 0;

    // Rate & Quality
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;
    // LFO State
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    int m_pms = 0;
    int m_ams = 0;

    float m_modWheel = 0.0f;
};
