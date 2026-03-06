#include "OpmCore.h"
#include "../synth/SynthHelpers.h"

void OpmCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;

    updateNoiseDelta(target);
}

void OpmCore::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm;
    m_lfoFreq = params.lfoFreq;
    m_am = params.amEnable;
    m_pm = params.pmEnable;
    m_pms = params.lfoPms;
    m_ams = params.lfoAms;
    m_pmd = params.lfoPmd;
    m_amd = params.lfoAmd;
    m_lfoWave = params.lfoWave; // OPM LFO Wave

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
        updateNoiseDelta(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.feedback;
        }

        if (i == 2) // OP2
        {
            fb = params.feedback2;
        }

        // OPM: SSG-EG=False, WaveSelect=False
        m_operators[i].setParameters(params.fmOp[i], fb, false, false, true);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void OpmCore::noteOn(float freq, float velocity) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
    m_rateAccumulator = 1.0;
}
void OpmCore::noteOff() { for (auto& op : m_operators) op.noteOff(); }
bool OpmCore::isPlaying() const {
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}
// ピッチベンド (0 - 16383, Center=8192)
void OpmCore::setPitchBend(int pitchWheelValue)
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
void OpmCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float OpmCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

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

        float out1, out2, out3, out4;
        float finalOut = 0.0f;

        m_operators[0].getSample(out1, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

        if (m_opMask[0]) out1 = 0.0f; // Mask

        switch (m_algorithm) {
        case 0:
            m_operators[1].getSample(out2, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out2, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 1:
            m_operators[1].getSample(out2, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 2:
            m_operators[1].getSample(out2, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out2, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 3:
            m_operators[1].getSample(out2, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out2, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 4:
            m_operators[1].getSample(out2, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out2 + out4;

            break;
        case 5:
            m_operators[1].getSample(out2, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out2 + out3 + out4;

            break;
        case 6:
            m_operators[1].getSample(out2, out1, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out2 + out3 + out4;

            break;
        default:
            m_operators[1].getSample(out2, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, 0.0f, lfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out1 + out2 + out3 + out4;

            break;
        }

        // =======================================================
        // ★対策2: 複数のキャリアが加算されても1.0を超えないように音量を調整
        // =======================================================
        finalOut *= 0.25f;

        // 絶対安全ガード (万が一1.0を超えてもDAWを破壊しない)
        finalOut = std::clamp(finalOut, -1.0f, 1.0f);

        // Quantization
        if (m_quantizeSteps > 0.0f) {
            if (finalOut > 1.0f) finalOut = 1.0f; else if (finalOut < -1.0f) finalOut = -1.0f;
            float norm = (finalOut + 1.0f) * 0.5f;
            float quantized = std::round(norm * m_quantizeSteps) / m_quantizeSteps;
            finalOut = (quantized * 2.0f) - 1.0f;
        }

        // =======================================================
        // ★対策1: 平均化(sumOut/steps)をやめ、最後に計算した値を保持する(Sample & Hold)
        // =======================================================
        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    return m_prevSample + (m_lastSample - m_prevSample) * fraction;
}

void OpmCore::updateNoiseDelta(double targetRate) {
    if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
}
