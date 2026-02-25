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
    m_pms = params.pms;
    m_ams = params.ams;
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
    m_rateAccumulator += targetRate / m_hostSampleRate;

    // ★追加: アンチエイリアス（ノイズ防止）のための平均化変数
    int steps = 0;
    float sumOut = 0.0f;

    while (m_rateAccumulator >= 1.0)
    {
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
        float pmDepth = 0.0f;
        if (m_pms > 0) {
            float depths[] = { 0.0f, 0.003f, 0.006f, 0.012f, 0.03f, 0.06f, 0.26f, 0.5f };
            pmDepth = depths[m_pms & 7];
        }

        float wheelDepth = m_modWheel * 0.05f;
        float lfoPitchMod = 1.0f + (lfoVal * (pmDepth + wheelDepth));


        float out1, out2, out3, out4;
        float finalOut = 0.0f;

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
        case 3: // OP1->OP2->OP4, OP3->OP4 (出力は4のみ)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out2 + out3, lfoAmpMod, lfoPitchMod);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out4;
            break;
        case 4: // OP1->OP2, OP3->OP4 (出力は2と4)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); // モジュレーターはout3
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out2 + out4;
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

        // 複数のキャリアが加算されてもクリッピングしないように音量を下げる
        finalOut *= 0.4f;

        // Quantization
        if (m_quantizeSteps > 0.0f) {
            if (finalOut > 1.0f) finalOut = 1.0f; else if (finalOut < -1.0f) finalOut = -1.0f;
            float norm = (finalOut + 1.0f) * 0.5f;
            // ★修正: std::round を使用
            float quantized = std::round(norm * m_quantizeSteps) / m_quantizeSteps;
            finalOut = (quantized * 2.0f) - 1.0f;
        }

        // 出力を加算し、ステップ数をカウント
        sumOut += finalOut;
        steps++;
    }

    // 平均を出力（アンチエイリアス）
    if (steps > 0) {
        m_lastSample = sumOut / (float)steps;
    }

    return m_lastSample;
}

void OpmCore::updateNoiseDelta(double targetRate) {
    if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
}
