#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"

const std::array<Opzx7Core::AlgRouting, 36> Opzx7Core::routings = { {
    // in2_1, fb2_1, in3_1, in3_2, in4_1, in4_2, in4_3, out_1, out_2, out_3, out_4
    { 1.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 00
    { 1.0f, true,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 01
    { 0.0f, false, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 02
    { 0.0f, false, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 03
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 04
    { 1.0f, true,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 05
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 06
    { 1.0f, true,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 07
    { 0.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // 08
    { 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // 09
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 10
    { 1.0f, true,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 11
    { 1.0f, false, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 12
    { 0.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f }, // 13
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f }, // 14
    { 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }, // 15
    { 1.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f }, // 16
    { 1.0f, true,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f }, // 17
    { 0.0f, false, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f }, // 18
    { 0.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f }, // 19
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f }, // 20
    { 1.0f, true,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f }, // 21
    { 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f }, // 22
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f }, // 23
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f }, // 24
    { 1.0f, true,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f }, // 25
    { 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f }, // 26
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f }, // 27
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f }, // 28
    { 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f }, // 29
    { 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }, // 30
    { 0.0f, true,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 31
    { 1.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 32
    { 1.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 33
    { 0.0f, false, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // 34
    { 0.0f, true,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f }  // 35
} };

void Opzx7Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    double target = getTargetRate(m_rateIndex);

    for(int i=0; i<4; i++) {
        m_operators[i].prepare(i+1, target);
    }

    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;
    m_amSmooth = 0.0f;

    m_lfo.prepare(target);
}

void Opzx7Core::setCurveCore(CurveCore* p_curveCore)
{
    for (auto& op : m_operators) {
        op.setCurveCore(p_curveCore);
    }
}

void Opzx7Core::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void Opzx7Core::setParameters(const SynthParams& params) {
    m_algorithm = params.opzx7.algorithm; // Range: 0-27

    m_lfo.setParameters(
        params.opzx7.lfoSyncDelay,
        params.opzx7.pmEnable, params.opzx7.amEnable,
        params.opzx7.lfoFreq, params.opzx7.lfoFreq,
        params.opzx7.pgLfoWave, params.opzx7.egLfoWave,
        params.opzx7.lfoPms, params.opzx7.lfoPmd,
        params.opzx7.lfoAms, params.opzx7.lfoAmd,
        params.opzx7.lfoAmSmRt
    );

    m_panpot = params.opzx7.panpot;
    m_panpot_enable = params.opzx7.panpot_enable;

    if (m_panpot_enable) {
        float pan = (float)(m_panpot + 1) / 33.0f;

        m_panpot_l_rate = 1.0f - pan;
        m_panpot_r_rate = pan;
    }
    else {
        m_panpot_l_rate = 0.5f;
        m_panpot_r_rate = 0.5f;
    }

    if (m_rateIndex != params.opzx7.fmRateIndex) {
        m_rateIndex = params.opzx7.fmRateIndex;

        double target = getTargetRate(m_rateIndex);

        for (auto& op : m_operators) {
            op.setSampleRate(target);
        }

        m_lfo.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opzx7.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        // ALG=33のときは、OP1,OP3の両方フィードバック
        if (m_algorithm == 33)
        {
            fb = (i == 0 || i == 2) ? params.opzx7.feedback : 0.0f;
        }
        else if (i == 0)
        {
            fb = params.opzx7.feedback;
        }

        // WaveSelect=True, SSG-EG=True, OpmEg=True
        m_operators[i].setParameters(params.opzx7.op[i], fb);
        m_opMask[i] = params.opzx7.op[i].mask;
    }

    // OPX特有の外部フィードバックアルゴリズムの場合、OP0/OP2の自己FBをオフにする
    bool useExtFb = false;
    if (m_algorithm == 1 || m_algorithm == 5 || m_algorithm == 7 || m_algorithm == 11) useExtFb = true; // 4OP: OP1->OP0 FB
    if (m_algorithm == 17 || m_algorithm == 21) useExtFb = true; // 3OP: OP2->OP0 FB
    if (m_algorithm == 25) useExtFb = true; // 2OP: OP1->OP0 FB

    m_operators[0].setExternalFeedbackMode(useExtFb);
    m_operators[2].setExternalFeedbackMode(useExtFb);
}

void Opzx7Core::noteOn(float freq, float velocity, int midiNote) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    float gain = std::max(0.01f, velocity * 0.25f);

    for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);

    m_lfo.noteOn();
}

void Opzx7Core::noteOff()
{
    for (auto& op : m_operators) op.noteOff();
}

bool Opzx7Core::isPlaying() const
{
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

void Opzx7Core::setPitchBend(int pitchWheelValue)
{
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    float semitones = 2.0f;
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);
    for (auto& op : m_operators) op.setPitchBendRatio(ratio);
}

void Opzx7Core::setModulationWheel(int wheelValue)
{
    m_modWheel = (float)wheelValue / 127.0f;
}

float Opzx7Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        m_lfo.getSample();

        // Outputs
        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        int algIndex = std::clamp(m_algorithm, 0, 35);
        const auto& r = routings[algIndex];

        // =================================================================
        // 1. OP1 (入力は常に0.0)
        // =================================================================
        m_operators[0].getSample(out1, 0.0f, m_lfo, m_modWheel);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // 2. OP2 (入力: OP1)
        // =================================================================
        float in2 = out1 * r.in2_1;
        m_operators[1].getSample(out2, in2, m_lfo, m_modWheel);
        if (m_opMask[1]) out2 = 0.0f;

        // フィードバック指定があれば OP2->OP1 にフィードバックをプッシュ
        if (r.fb2_1) {
            m_operators[0].pushFeedback(out2);
        }

        // =================================================================
        // 3. OP3 (入力: OP1, OP2)
        // =================================================================
        float in3 = (out1 * r.in3_1) + (out2 * r.in3_2);
        m_operators[2].getSample(out3, in3, m_lfo, m_modWheel);
        if (m_opMask[2]) out3 = 0.0f;

        // =================================================================
        // 4. OP4 (入力: OP1, OP2, OP3)
        // =================================================================
        float in4 = (out1 * r.in4_1) + (out2 * r.in4_2) + (out3 * r.in4_3);
        m_operators[3].getSample(out4, in4, m_lfo, m_modWheel);
        if (m_opMask[3]) out4 = 0.0f;

        // =================================================================
        // 5. Final Output (各OPからマスターアウトへの加算)
        // =================================================================
        finalOut = ((out1 * r.out_1) + (out2 * r.out_2) + (out3 * r.out_3) + (out4 * r.out_4)) * 2.0f;

        // =======================================================
        // 無音(0.0)が完全に0.0になる量子化 (UI・WtCoreと完全同期)
        // =======================================================
        if (m_quantizeSteps > 0.0f) {
            int totalSteps = (int)m_quantizeSteps + 1;
            int maxIndex = totalSteps - 1;
            int zeroIndex = totalSteps / 2 - 1;
            int stepValue = 0;

            if (finalOut < 0.0f) {
                stepValue = (int)std::round(finalOut * zeroIndex + zeroIndex);
            }
            else {
                stepValue = (int)std::round(finalOut * (maxIndex - zeroIndex) + zeroIndex);
            }

            stepValue = std::clamp(stepValue, 0, maxIndex);

            if (stepValue < zeroIndex) {
                finalOut = (float)(stepValue - zeroIndex) / (float)zeroIndex;
            }
            else if (stepValue > zeroIndex) {
                finalOut = (float)(stepValue - zeroIndex) / (float)(maxIndex - zeroIndex);
            }
            else {
                finalOut = 0.0f; // ★完全な 0.0 を保証
            }
        }

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    return m_prevSample + (m_lastSample - m_prevSample) * fraction;
}

void Opzx7Core::setPcmBuffer(int opIndex, const std::vector<float>* pcmData)
{
    if (opIndex >= 0 && opIndex < 4) {
        m_operators[opIndex].setPcmBuffer(pcmData);
    }
}

void Opzx7Core::setWtBuffer(int opIndex, const std::vector<float>* wtData)
{
    if (opIndex >= 0 && opIndex < 4) {
        m_operators[opIndex].setWtBuffer(wtData);
    }
}

void Opzx7Core::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample * m_panpot_l_rate;
    outR[startSample + sampleIdx] += sample * m_panpot_r_rate;

    isActive = isPlaying();
}
