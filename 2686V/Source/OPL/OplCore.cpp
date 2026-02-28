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

        // LFO Logic (Run at Target Rate)
                // AM Phase (Tremolo)
        m_amPhase += (3.7 / targetRate);
        if (m_amPhase >= 1.0) m_amPhase -= 1.0;

        float amVal = 0.0f;
        if (m_amPhase < 0.25)           amVal = (float)(m_amPhase * 4.0);
        else if (m_amPhase < 0.75)      amVal = (float)(1.0 - (m_amPhase - 0.25) * 4.0);
        else                            amVal = (float)(-1.0 + (m_amPhase - 0.75) * 4.0);

        // 実機OPLのトレモロ深度(-4.8dB = 音量比 約0.575)に合わせる
        // amVal(-1.0 ~ 1.0) を 0.0 ~ 1.0 に正規化
        float normAm = (amVal + 1.0f) * 0.5f;
        // 最大で 0.425 (約42.5%) だけ音量が減るようにする
        float lfoAmpVal = 1.0f - (normAm * 0.425f);

        // VIB Phase (Vibrato)
        m_vibPhase += (6.4 / targetRate);
        if (m_vibPhase >= 1.0) m_vibPhase -= 1.0;

        float vibVal = 0.0f;
        if (m_vibPhase < 0.25)          vibVal = (float)(m_vibPhase * 4.0);
        else if (m_vibPhase < 0.75)     vibVal = (float)(1.0 - (m_vibPhase - 0.25) * 4.0);
        else                            vibVal = (float)(-1.0 + (m_vibPhase - 0.75) * 4.0);

        // ModWheelによる手動ビブラート(最大50セント)と、OPL内蔵ビブラート(約14セント)の合成
        float pmDepth = 0.03f * m_modWheel;
        // OPL内蔵ビブラート(約14セント = 0.008)を加算
        float lfoPitchVal = 1.0f + (vibVal * (0.008f + pmDepth));

        // -------------------------------

        float out1, out2;
        float finalOut = 0.0f;

        // オペレーターのフラグ(amEnable / vibEnable)を見て、LFOを適用するか決める
        // FmOperator::getSample は引数で渡されたものをそのまま掛ける仕様なので、ここで1.0f(無効)か変調値かを渡す
        float finalAmVal = m_operators[0].m_params.amEnable ? lfoAmpVal : 1.0f;
        float finalPmVal = m_operators[0].m_params.vibEnable ? lfoPitchVal : 1.0f;

        m_operators[0].getSample(out1, 0.0f, finalAmVal, finalPmVal);

        if (m_opMask[0]) out1 = 0.0f;

        if (m_algorithm == 0) { // Serial (FM)
            // OP1 -> OP2
            m_operators[1].getSample(out2, out1, finalAmVal, finalPmVal);
            if (m_opMask[1]) out2 = 0.0f;
            finalOut = out2;
        }
        else { // Parallel (AM)
            m_operators[1].getSample(out2, 0.0f, finalAmVal, finalPmVal);
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
