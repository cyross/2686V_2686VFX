#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"

void OpmCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;

	m_noiseGen.prepare(target);
    m_lfo.prepare(target);
}

void OpmCore::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;

        for (auto& op : m_operators) {
            op.setSampleRate(m_hostSampleRate);
        }
    }
}

void OpmCore::setParameters(const SynthParams& params) {
    m_algorithm = params.opm.algorithm;
    m_lfo.setParameters(
        params.opm.lfoSyncDelay,
        params.opm.pmEnable, params.opm.amEnable,
        params.opm.lfoFreq, params.opm.lfoFreq,
        params.opm.pgLfoWave, params.opm.egLfoWave,
        params.opm.lfoPms, params.opm.lfoPmd,
        params.opm.lfoAms, params.opm.lfoAmd,
        params.opm.lfoAmSmRt);
     m_pan = params.opm.pan;

    if (m_rateIndex != params.opm.fmRateIndex) {
        m_rateIndex = params.opm.fmRateIndex;

        double target = getTargetRate(m_rateIndex);

        for (auto& op : m_operators) op.setSampleRate(target);

        m_noiseGen.updateDelta(target);
        m_lfo.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opm.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        // ALG=2 の時、OP3にFbを設定
        if (m_algorithm == 2)
        {
            fb = i == 2 ? params.opm.feedback : 0.0f;
        }
        else if (i == 0)
        {
            fb = params.opm.feedback;
        }

        // OPM: SSG-EG=False, WaveSelect=False
        m_operators[i].setParameters(params.opm.op[i], fb);
        m_opMask[i] = params.opm.op[i].mask;
    }
}

void OpmCore::noteOn(float freq, float velocity, int midiNote) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
    m_rateAccumulator = 1.0;

    m_lfo.noteOn();
}
void OpmCore::noteOff() {
    for (auto& op : m_operators) op.noteOff();
}

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

        m_lfo.getSample();

        float out1, out2, out3, out4;
        float finalOut = 0.0f;

        // 安全のため 0〜7 の範囲に丸める
        int algIndex = std::clamp(m_algorithm, 0, 7);
        const auto& r = baseRoutings[algIndex];

        // =================================================================
        // OP1 (入力なし。フィードバックは内部で解決される)
        // =================================================================
        m_operators[0].getSample(out1, 0.0f, m_lfo, m_modWheel);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // OP2 (入力: OP1)
        // =================================================================
        float in2 = out1 * r.in2_1;
        m_operators[1].getSample(out2, in2, m_lfo, m_modWheel);
        if (m_opMask[1]) out2 = 0.0f;

        // =================================================================
        // OP3 (入力: OP1, OP2)
        // =================================================================
        float in3 = (out1 * r.in3_1) + (out2 * r.in3_2);
        m_operators[2].getSample(out3, in3, m_lfo, m_modWheel);
        if (m_opMask[2]) out3 = 0.0f;

        // =================================================================
        // OP4 (入力: OP1, OP2, OP3)
        // =================================================================
        float in4 = (out1 * r.in4_1) + (out2 * r.in4_2) + (out3 * r.in4_3);
        m_operators[3].getSample(out4, in4, m_lfo, m_modWheel);
        if (m_opMask[3]) out4 = 0.0f;

        // =================================================================
        // Final Output
        // =================================================================
        finalOut = ((out1 * r.out_1) + (out2 * r.out_2) + (out3 * r.out_3) + (out4 * r.out_4));

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

void OpmCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += ((m_pan == 1) ? 0.0f : sample) * 0.5f;
    outR[startSample + sampleIdx] += ((m_pan == -1) ? 0.0f : sample) * 0.5f;

    isActive = isPlaying();
}
