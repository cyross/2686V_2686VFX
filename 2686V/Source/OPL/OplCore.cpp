#include "OplCore.h"
#include "../synth/SynthHelpers.h"

void OplCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;
}

void OplCore::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm; // 0:Serial(FM), 1:Parallel(AM)

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.fmBitDepth);

    for (int i = 0; i < 2; ++i) {
        float fb = (i == 0) ? params.feedback : 0.0f; // OP1のみFeedback

        // FmCommon(96dB)に対して、OPL(48dB)のスケールを合わせるためTLを半分にする
        FmOpParams opParams = params.fmOp[i];
        opParams.totalLevel *= 0.5f;

        m_operators[i].setParameters(params.fmOp[i], fb, false, true);

        m_opMask[i] = params.fmOp[i].mask;
    }
}

void OplCore::noteOn(float freq, float velocity) {
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
    m_modWheel = (float)wheelValue / 127.0f;
}

float OplCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        float opAmpMod[2] = { 1.0f, 1.0f };
        float opPitchMod[2] = { 1.0f, 1.0f };

        for (int i = 0; i < 2; ++i)
        {
            // --- AM (Tremolo) ---
            if (m_operators[i].m_params.amEnable) {
                m_amPhases[i] += (m_operators[i].m_params.oplAms / targetRate);
                if (m_amPhases[i] >= 1.0) m_amPhases[i] -= 1.0;

                float amVal = 0.0f;
                if (m_amPhases[i] < 0.25)           amVal = (float)(m_amPhases[i] * 4.0);
                else if (m_amPhases[i] < 0.75)      amVal = (float)(1.0 - (m_amPhases[i] - 0.25) * 4.0);
                else                                amVal = (float)(-1.0 + (m_amPhases[i] - 0.75) * 4.0);

                float normAm = (amVal + 1.0f) * 0.5f;
                float amDepthRatio = 1.0f - std::pow(10.0f, -m_operators[i].m_params.oplAmd / 20.0f);
                opAmpMod[i] = 1.0f - (normAm * amDepthRatio);
            }

            // --- VIB (Vibrato) ---
            if (m_operators[i].m_params.vibEnable) {
                m_vibPhases[i] += (m_operators[i].m_params.oplPms / targetRate);
                if (m_vibPhases[i] >= 1.0) m_vibPhases[i] -= 1.0;

                float vibVal = 0.0f;
                if (m_vibPhases[i] < 0.25)          vibVal = (float)(m_vibPhases[i] * 4.0);
                else if (m_vibPhases[i] < 0.75)     vibVal = (float)(1.0 - (m_vibPhases[i] - 0.25) * 4.0);
                else                                vibVal = (float)(-1.0 + (m_vibPhases[i] - 0.75) * 4.0);

                float pmDepthRatio = std::pow(2.0f, m_operators[i].m_params.oplPmd / 1200.0f) - 1.0f;
                float modWheelDepth = 0.03f * m_modWheel;
                opPitchMod[i] = 1.0f + (vibVal * (pmDepthRatio + modWheelDepth));
            }
        }

        float out1, out2;
        float finalOut = 0.0f;

        m_operators[0].getSample(out1, 0.0f, opAmpMod[0], opPitchMod[0]);
        if (m_opMask[0]) out1 = 0.0f;

        if (m_algorithm == 0) { // Serial (FM)
            m_operators[1].getSample(out2, out1, opAmpMod[1], opPitchMod[1]);
            if (m_opMask[1]) out2 = 0.0f;
            finalOut = out2;
        }
        else { // Parallel (AM)
            m_operators[1].getSample(out2, 0.0f, opAmpMod[1], opPitchMod[1]);
            if (m_opMask[1]) out2 = 0.0f;
            finalOut = (out1 + out2) * 0.5f;
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
