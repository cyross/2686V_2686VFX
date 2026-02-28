#include "Opl3Core.h"
#include "../synth/SynthHelpers.h"

void Opl3Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;
}

// --- Opl3Core.cpp : setParameters() 内 ---

void Opl3Core::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) fb = params.feedback;
        else if (i == 2) fb = params.feedback2;

        // FmCommon(96dB)に対して、OPL3(48dB)のスケールを合わせるためTLを半分にする
        FmOpParams opParams = params.fmOp[i];
        opParams.totalLevel *= 0.5f;

        m_operators[i].setParameters(opParams, fb, false, true);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void Opl3Core::noteOn(float freq, float velocity) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);

    m_rateAccumulator = 1.0;
}
void Opl3Core::noteOff() { for (auto& op : m_operators) op.noteOff(); }
bool Opl3Core::isPlaying() const {
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}
// ピッチベンド (0 - 16383, Center=8192)
void Opl3Core::setPitchBend(int pitchWheelValue)
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
void Opl3Core::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

// --- Opl3Core.cpp : getSample() の全体 ---

float Opl3Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        // LFO Logic
        m_amPhase += (3.7 / targetRate);
        if (m_amPhase >= 1.0) m_amPhase -= 1.0;

        float amVal = 0.0f;
        if (m_amPhase < 0.25)           amVal = (float)(m_amPhase * 4.0);
        else if (m_amPhase < 0.75)      amVal = (float)(1.0 - (m_amPhase - 0.25) * 4.0);
        else                            amVal = (float)(-1.0 + (m_amPhase - 0.75) * 4.0);

        // OPL3のトレモロ深度(-4.8dB)に合わせる
        float normAm = (amVal + 1.0f) * 0.5f;
        float lfoAmpVal = 1.0f - (normAm * 0.425f);

        m_vibPhase += (6.4 / targetRate);
        if (m_vibPhase >= 1.0) m_vibPhase -= 1.0;

        float vibVal = 0.0f;
        if (m_vibPhase < 0.25)          vibVal = (float)(m_vibPhase * 4.0);
        else if (m_vibPhase < 0.75)     vibVal = (float)(1.0 - (m_vibPhase - 0.25) * 4.0);
        else                            vibVal = (float)(-1.0 + (m_vibPhase - 0.75) * 4.0);

        float pmDepth = 0.03f * m_modWheel;
        // OPL3内蔵ビブラート(約14セント)を加算
        float lfoPitchVal = 1.0f + (vibVal * (0.008f + pmDepth));

        // -------------------------------

        float out1, out2, out3, out4;
        float finalOut = 0.0f;

        // 各オペレーターの AM/VIB 有効フラグを個別に判定する
        auto getAm = [&](int i) { return m_operators[i].m_params.amEnable ? lfoAmpVal : 1.0f; };
        auto getPm = [&](int i) { return m_operators[i].m_params.vibEnable ? lfoPitchVal : 1.0f; };

        m_operators[0].getSample(out1, 0.0f, getAm(0), getPm(0));

        if (m_opMask[0]) out1 = 0.0f; // Mask

        switch (m_algorithm) {
        case 0:
            m_operators[1].getSample(out2, out1, getAm(1), getPm(1));

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, getAm(2), getPm(2));

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, getAm(3), getPm(3));

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 1:
            m_operators[1].getSample(out2, 0.0f, getAm(1), getPm(1));

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, getAm(2), getPm(2));

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, getAm(3), getPm(3));

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out4;

            break;
        case 2:
            m_operators[1].getSample(out2, out1, getAm(1), getPm(1));

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, getAm(2), getPm(2));

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, getAm(3), getPm(3));

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out4;

            break;
        case 3:
            m_operators[1].getSample(out2, 0.0f, getAm(1), getPm(1));

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, getAm(2), getPm(2));

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, getAm(3), getPm(3));

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out3 + out4;

            break;
        default:
            m_operators[1].getSample(out2, 0.0f, getAm(1), getPm(1));

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, getAm(2), getPm(2));

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, getAm(3), getPm(3));

            if (m_opMask[3]) out4 = 0.0f;

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
