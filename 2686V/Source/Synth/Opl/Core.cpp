#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"

void OplCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;
}

void OplCore::setParameters(const SynthParams& params) {
    m_algorithm = params.opl.algorithm; // 0:Serial(FM), 1:Parallel(AM)

    if (m_rateIndex != params.opl.fmRateIndex) {
        m_rateIndex = params.opl.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opl.fmBitDepth);

    for (int i = 0; i < 2; ++i) {
        float fb = (i == 0) ? params.opl.feedback : 0.0f; // OP1のみFeedback

        m_operators[i].setParameters(params.opl.op[i], fb);

        m_opMask[i] = params.opl.op[i].mask;
    }
}

void OplCore::noteOn(float freq, float velocity, int midiNote) {
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    m_operators[0].noteOn(freq, gain, noteNum);
    m_operators[1].noteOn(freq, gain, noteNum);
    m_rateAccumulator = 1.0;
}

void OplCore::noteOff() {
    m_operators[0].noteOff();
    m_operators[1].noteOff();
}

bool OplCore::isPlaying() const {
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void OplCore::setPitchBend(int pitchWheelValue)
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
void OplCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    float modWheel = (float)wheelValue / 127.0f;

    for (int i = 0; i < 4; i++)
    {
        m_operators[i].setModWheel(modWheel);
    }
}

float OplCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        for (int i = 0; i < 2; ++i)
        {
            m_operators[i].processLfo();
        }

        float out1, out2;
        float finalOut = 0.0f;

        m_operators[0].getSample(out1, 0.0f);
        if (m_opMask[0]) out1 = 0.0f;

        if (m_algorithm == 0) { // Serial (FM)
            m_operators[1].getSample(out2, out1);
            if (m_opMask[1]) out2 = 0.0f;
            finalOut = out2;
        }
        else { // Parallel (AM)
            m_operators[1].getSample(out2, 0.0f);
            if (m_opMask[1]) out2 = 0.0f;
            finalOut = (out1 + out2) * 0.5f;
        }

        // =======================================================
        // 無音(0.0)が完全に0.0になるBipolar(双極性)量子化
        // =======================================================
        if (m_quantizeSteps > 0.0f) {
            // 単純に掛け算して丸め、割り算で戻すだけでゼロクロスが保証されます
            finalOut = std::round(finalOut * m_quantizeSteps) / m_quantizeSteps;

            // 安全のためのクリップ
            finalOut = std::clamp(finalOut, -1.0f, 1.0f);
        }

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    return m_prevSample + (m_lastSample - m_prevSample) * fraction;
}

void OplCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample * 0.5f; // モノラルなので、ステレオで言うCと同じ計算
    outR[startSample + sampleIdx] += sample * 0.5f; // モノラルなので、ステレオで言うCと同じ計算

    isActive = isPlaying();
}
