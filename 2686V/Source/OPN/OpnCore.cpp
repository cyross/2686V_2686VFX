#include "OpnCore.h"
#include "../synth/SynthHelpers.h"

void OpnCore::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;
}

void OpnCore::setParameters(const SynthParams& params)
{
    m_algorithm = params.algorithm;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.fmBitDepth);

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

        // OPN: SSG-EG=False, WaveSelect=False
        m_operators[i].setParameters(params.fmOp[i], fb, false, false);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void OpnCore::noteOn(float freq, float velocity)
{
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);
    m_rateAccumulator = 1.0;
}

void OpnCore::noteOff()
{
    for (auto& op : m_operators) op.noteOff();
}

bool OpnCore::isPlaying() const
{
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void OpnCore::setPitchBend(int pitchWheelValue)
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
void OpnCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float OpnCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);
    m_rateAccumulator += targetRate / m_hostSampleRate;

    int steps = 0;
    float sumOut = 0.0f;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // --- ソフトウェアLFO (ビブラート用) ---
        // OPNには本来ありませんが、ModWheel対応のために擬似的に計算します
        double lfoInc = m_lfoFreq / targetRate;
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

        // 三角波 (-1.0 ~ 1.0)
        float lfoValue = 0.0f;
        if (m_lfoPhase < 0.25)      lfoValue = (float)(m_lfoPhase * 4.0);
        else if (m_lfoPhase < 0.75) lfoValue = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
        else                        lfoValue = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

        // モジュレーション深度計算 (最大約半音程度の揺れ)
        float wheelDepth = m_modWheel * 0.05f;
        float lfoPitchMod = 1.0f + (lfoValue * wheelDepth);
        float lfoAmpMod = 1.0f; // トレモロはなし

        // ------------------------------------

        float out1, out2, out3, out4;

        // 各getSample呼び出しに lfoPitchMod を渡すように修正
        m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

        if (m_opMask[0]) out1 = 0.0f; // Mask

        float finalOut = 0.0f;
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
            if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
            if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out2 + out3, lfoAmpMod, lfoPitchMod); // out2 + out3
            if (m_opMask[3]) out4 = 0.0f;
            finalOut = out4; // 出力は4のみ
            break;
        case 4: // OP1->OP2, OP3->OP4 (出力は2と4)
            m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod);
            if (m_opMask[1]) out2 = 0.0f;
            m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod);
            if (m_opMask[2]) out3 = 0.0f;
            m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); // モジュレーターはout3
            if (m_opMask[3]) out4 = 0.0f;
            finalOut = out2 + out4; // 出力は2と4のみ
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

        finalOut *= 0.4f;

        // Quantization
        if (m_quantizeSteps > 0.0f) {
            if (finalOut > 1.0f) finalOut = 1.0f; else if (finalOut < -1.0f) finalOut = -1.0f;
            float norm = (finalOut + 1.0f) * 0.5f;
            // ★修正: floorをroundにして波形の中心を安定させる
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
