#pragma once
#include "FmCommon.h"
#include <random>

class OpmCore
{
public:
    OpmCore() {}
    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);
        m_lfoPhase = 0.0;
        m_rateAccumulator = 1.0;

        updateNoiseDelta(target);
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;
        m_lfoFreq = params.lfoFreq;
        m_pms = params.pms;
        m_ams = params.ams;
        m_lfoWave = params.lfoWave; // OPM LFO Wave

        if (m_rateIndex != params.fmRateIndex) {
            m_rateIndex = params.fmRateIndex;
            double target = getTargetRate();
            for (auto& op : m_operators) op.setSampleRate(target);
            updateNoiseDelta(target);
        }

        switch (params.fmBitDepth) {
            case 0: m_quantizeSteps = 15.0f; break;
            case 1: m_quantizeSteps = 31.0f; break;
            case 2: m_quantizeSteps = 63.0f; break;
            case 3: m_quantizeSteps = 255.0f; break;
            case 4: m_quantizeSteps = 0.0f; break;
            default: m_quantizeSteps = 0.0f; break;
        }

        for (int i = 0; i < 4; ++i) {
            float fb = (i == 0) ? params.feedback : 0.0f;
            // OPM: SSG-EG=False, WaveSelect=False
            m_operators[i].setParameters(params.fmOp[i], fb, false, false, true);
            m_opMask[i] = params.fmOp[i].mask;
        }
    }

    void noteOn(float freq, float velocity) {
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        for(auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
        m_rateAccumulator = 1.0;
    }
    void noteOff() { for(auto& op : m_operators) op.noteOff(); }
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

            // --- OPM LFO (at Target Rate) ---
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;
            if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

            float lfoVal = 0.0f;

            switch (m_lfoWave) {
            case 0: lfoVal = (float)(1.0 - m_lfoPhase * 2.0); break; // Saw Down
            case 1: lfoVal = (m_lfoPhase < 0.5) ? 1.0f : -1.0f; break; // Square
            case 2: // Triangle
                if (m_lfoPhase < 0.25) lfoVal = (float)(m_lfoPhase * 4.0);
                else if (m_lfoPhase < 0.75) lfoVal = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
                else lfoVal = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);
                break;
            case 3: // Noise LFO
                m_noisePhase += m_noiseDelta;
                if (m_noisePhase >= 1.0f) {
                    m_noisePhase -= 1.0f;
                    unsigned int bit0 = m_lfsr & 1;
                    unsigned int bit3 = (m_lfsr >> 3) & 1;
                    unsigned int nextBit = bit0 ^ bit3;
                    m_lfsr >>= 1;
                    if (nextBit) m_lfsr |= (1 << 16);
                    m_currentNoiseSample = (m_lfsr & 1) ? 1.0f : -1.0f;
                }
                lfoVal = m_currentNoiseSample;
                break;
            }

            // AMS (Amplitude Modulation Sensitivity)
            float lfoAmpMod = 1.0f;
            if (m_ams > 0) {
                float depths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
                lfoAmpMod = 1.0f - (std::abs(lfoVal) * depths[m_ams & 3]);
            }

            // PMS (Pitch Modulation Sensitivity) + Mod Wheel
            // ★修正: PMS設定値とホイール値を合算します
            float pmDepth = 0.0f;
            if (m_pms > 0) {
                float depths[] = { 0.0f, 0.001f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f, 0.2f };
                pmDepth = depths[m_pms & 7];
            }

            // ホイールによる追加深度 (0.05 = +/- 50cents)
            float wheelDepth = m_modWheel * 0.05f;

            // 最終的なピッチ変調率
            float lfoPitchMod = 1.0f + (lfoVal * (pmDepth + wheelDepth));


            float out1, out2, out3, out4;
            float finalOut = 0.0f;

            // 各オペレーターに渡す
            m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

            if (m_opMask[0]) out1 = 0.0f; // Mask

            switch (m_algorithm) {
            case 0:
                m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out4;

                break;
            case 1:
                m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out4;

                break;
            case 2:
                m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, out3 + out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out4;

                break;
            case 3:
                m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out2 + out4;
                break;
            case 4:
                m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out2 + out3 + out4;

                break;
            case 5:
                m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out2 + out3 + out4;
                break;
            case 6:
                m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out2 + out3 + out4;

                break;
            default:
                m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[1]) out2 = 0.0f; // Mask

                m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[2]) out3 = 0.0f; // Mask

                m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);

                if (m_opMask[3]) out4 = 0.0f; // Mask

                finalOut = out1 + out2 + out3 + out4;
                break;
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

    void updateNoiseDelta(double targetRate) {
        if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
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

    // OPM LFO
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    int m_pms = 0; int m_ams = 0; int m_lfoWave = 2;

    // Noise LFSR Variables
    unsigned int m_lfsr = 0x1FFFF; // 17-bit Shift Register (Seed must be non-zero)
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;

    float m_modWheel = 0.0f;
};