#include "OpnaCore.h"
#include "../synth/SynthHelpers.h"

void OpnaCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    // Update operators with current target rate
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);

    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0; // Ready to render
}

void OpnaCore::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm;
    m_lfoFreq = params.lfoFreq;
    m_am = params.amEnable;
    m_pm = params.pmEnable;
    m_pms = params.lfoPms;
    m_ams = params.lfoAms;

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

        // OPNA: SSG-EG=True, WaveSelect=False
        // params.fmSsgEgFreq を渡す (第6引数)
        // 第5引数(useOpmEg)は false
        m_operators[i].setParameters(params.fmOp[i], fb, true, false, false, params.fmOp[i].fmSsgEgFreq);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void OpnaCore::noteOn(float freq, float velocity) {
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);

    m_rateAccumulator = 1.0; // Reset timing
}

void OpnaCore::noteOff() { for (auto& op : m_operators) op.noteOff(); }
bool OpnaCore::isPlaying() const {
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}
// ピッチベンド (0 - 16383, Center=8192)
void OpnaCore::setPitchBend(int pitchWheelValue)
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
void OpnaCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float OpnaCore::getSample() {
    // --- Rate Emulation ---
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        // ========================================================
        // 1. LFO 波形の生成（ここは残します！）
        // ========================================================
        double lfoInc = m_lfoFreq / targetRate;
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

        // ★この lfoValue が m_operators[i].getSample() に渡されます
        float lfoValue = 0.0f;
        if (m_lfoPhase < 0.25)      lfoValue = (float)(m_lfoPhase * 4.0);
        else if (m_lfoPhase < 0.75) lfoValue = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
        else                        lfoValue = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

        // ========================================================
        // 2. オペレーターの処理とルーティング
        // ========================================================
        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        m_operators[0].getSample(out1, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);
        if (m_opMask[0]) out1 = 0.0f;

        switch (m_algorithm) {
        case 0:
            m_operators[1].getSample(out2, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 1:
            m_operators[1].getSample(out2, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out1 + out2, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 2:
            m_operators[1].getSample(out2, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3 + out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 3:
            m_operators[1].getSample(out2, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out2 + out3, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 4:
            m_operators[1].getSample(out2, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out4;

            break;
        case 5:
            m_operators[1].getSample(out2, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out3 + out4;

            break;
        case 6:
            m_operators[1].getSample(out2, out1, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out3 + out4;

            break;
        default:
            m_operators[1].getSample(out2, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, lfoValue, m_pm, m_am, m_pms, m_ams, -1.0f, -1.0f, m_modWheel);

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