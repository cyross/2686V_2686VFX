#include "Opzx3Core.h"
#include "../synth/SynthHelpers.h"

void Opzx3Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;
    updateNoiseDelta(target);
}

void Opzx3Core::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm; // Range: 0-27
    m_lfoFreq = params.lfoFreq;
    m_pms = params.pms;
    m_ams = params.ams;
    m_lfoWave = params.lfoWave;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
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
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.feedback;
        }

        // WaveSelect=True, SSG-EG=False, OpmEg=True
        m_operators[i].setParameters(params.fmOp[i], fb, false, true, true);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void Opzx3Core::noteOn(float freq, float velocity) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
    m_rateAccumulator = 1.0;
}

void Opzx3Core::noteOff()
{
    for (auto& op : m_operators) op.noteOff();
}

bool Opzx3Core::isPlaying() const
{
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

void Opzx3Core::setPitchBend(int pitchWheelValue)
{
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    float semitones = 2.0f;
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);
    for (auto& op : m_operators) op.setPitchBendRatio(ratio);
}

void Opzx3Core::setModulationWheel(int wheelValue)
{
    m_modWheel = (float)wheelValue / 127.0f;
}

float Opzx3Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);
    m_rateAccumulator += targetRate / m_hostSampleRate;

    if (m_rateAccumulator >= 1.0) {
        m_rateAccumulator -= 1.0;

        // --- LFO Processing ---
        double lfoInc = m_lfoFreq / targetRate;
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

        float lfoVal = 0.0f;
        switch (m_lfoWave) {
        case 0: lfoVal = (float)(1.0 - m_lfoPhase * 2.0); break; // Saw
        case 1: lfoVal = (m_lfoPhase < 0.5) ? 1.0f : -1.0f; break; // Square
        case 2: // Triangle
            if (m_lfoPhase < 0.25) lfoVal = (float)(m_lfoPhase * 4.0);
            else if (m_lfoPhase < 0.75) lfoVal = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
            else lfoVal = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);
            break;
        case 3: // Noise
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

        // AMS / PMS
        float lfoAmpMod = 1.0f;
        if (m_ams > 0) {
            float depths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
            lfoAmpMod = 1.0f - (std::abs(lfoVal) * depths[m_ams & 3]);
        }

        float pmDepth = 0.0f;
        if (m_pms > 0) {
            // 実機(YM2151/2608)の仕様に合わせた深度テーブル
            // 0: 0
            // 1: +/- 5 cents   (~0.003)
            // 2: +/- 10 cents  (~0.006)
            // 3: +/- 20 cents  (~0.012)
            // 4: +/- 50 cents  (~0.03)
            // 5: +/- 100 cents (~0.06)  -> 半音
            // 6: +/- 400 cents (~0.26)  -> 長3度
            // 7: +/- 700 cents (~0.50)  -> 完全5度 (かなり激しい)

            float depths[] = { 0.0f, 0.003f, 0.006f, 0.012f, 0.03f, 0.06f, 0.26f, 0.5f };

            // 旧コード（参考）: { 0.0f, 0.001f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f, 0.2f };
            pmDepth = depths[m_pms & 7];
        }
        float wheelDepth = m_modWheel * 0.05f;
        float lfoPitchMod = 1.0f + (lfoVal * (pmDepth + wheelDepth));

        // Outputs
        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        // Op1 is Feedback Source
        m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // Algorithm Routing (0-27)
        // =================================================================
        switch (m_algorithm) {

            // --- 4-Operator Algorithms (0-15) ---
            // 0-7: Standard OPM/OPN Compatible
        case 0: // 1->2->3->4
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out4; break;
        case 1: // (1+2)->3->4
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out4; break;
        case 2: // 1+(2->3)->4
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out1 + out3, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out4; break;
        case 3: // (1->2) + (3->4)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out4; break;
        case 4: // (1->2) + 3 + 4
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out3 + out4; break;
        case 5: // 1->(2,3,4)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out3 + out4; break;
        case 6: // (1->2) + 3 + 4
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out3 + out4; break;
        case 7: // 1 + 2 + 3 + 4
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out1 + out2 + out3 + out4; break;

            // 8-15: Extended 4-Op Algorithms (Variations)
        case 8: // 1->3, 2->4 (Cross)
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out2, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out3 + out4; break;
        case 9: // 1->2->4, 3->4 (Mixed Input)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out2 + out3, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out4; break;
        case 10: // 1->2->3, 4 (Series + 1 Parallel)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out3 + out4; break;
        case 11: // 1->2, 3->2, 4 (Double Mod to 2)
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[1].getSample(out2, out1 + out3, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out4; break;
        case 12: // 1->(2+3+4) (Super Split)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out3 + out4; break;
        case 13: // (1+2+3)->4 (Super Mix)
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out1 + out2 + out3, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out4; break;
        case 14: // 1->2, 3->4, 1->4 (Cross Mod)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out3 + out1, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out4; break;
        case 15: // Ring Mod Simulation (1*2 -> out) - Pseudo
            // Just Parallel for now to fill 16
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[3]) out4 = 0.0f;
            finalOut = out1 + out2 + out3 + out4; break;

            // --- 3-Operator Algorithms (16-23) ---
            // Using Op1, Op2, Op3. Op4 is inactive.
        case 16: // 1->2->3
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out3; break;
        case 17: // 1->3, 2->3
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out3; break;
        case 18: // 1->2, 3
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out2 + out3; break;
        case 19: // 1, 2->3
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out1 + out3; break;
        case 20: // 1->(2,3)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out2 + out3; break;
        case 21: // 1->2->3 (duplicate of 16 for filling) -> Let's do 2->1, 3
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out1 + out2 + out3; break;
        case 22: // 1, 2, 3
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out1 + out2 + out3; break;
        case 23: // (1+2)->3
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod); if (m_opMask[2]) out3 = 0.0f;
            finalOut = out3; break;

            // --- 2-Operator Algorithms (24-27) ---
            // Using Op1, Op2.
        case 24: // 1->2
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            finalOut = out2; break;
        case 25: // 1, 2
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            finalOut = out1 + out2; break;
        case 26: // 1+(1->2) (Feedback to 2 and Mix)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            finalOut = out1 + out2; break;
        case 27: // 1->2 (Feedback on 2) - Simulation by ignoring logic
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); if (m_opMask[1]) out2 = 0.0f;
            finalOut = out2; break;

        default:
            finalOut = 0.0f; break;
        }

        // --- Update unused operators to keep EG/Phase running ---
        if (m_algorithm >= 24) { // 2-Op Mode: Run 3 & 4
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);
        }
        else if (m_algorithm >= 16) { // 3-Op Mode: Run 4
            m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod);
        }

        // Quantization
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

void Opzx3Core::updateNoiseDelta(double targetRate) {
    if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
}
