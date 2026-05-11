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

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPM PG)
// -----------------------------------------------------------
const std::array<Opzx7Core::Opzx7LfoCalculator, 8> Opzx7Core::lfoPgStrategies = { {
    // 0: Sine
    [](double phase, float /*noise*/) -> float {
        return (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
    },
    // 1: Saw Up
    [](double phase, float /*noise*/) -> float {
        float pm = 0.0f;
        if (phase < 0.5) pm = (float)(phase * 2.0);
        else             pm = (float)(-1.0 + (phase - 0.5) * 2.0);

        return pm;
    },
    // 2: Saw Down
    [](double phase, float /*noise*/) -> float {
        return (float)(1.0 - phase * 2.0);
    },
    // 3: Square
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? 1.0f : -1.0f;
    },
    // 4: Triangle
    [](double phase, float /*noise*/) -> float {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        return pm;
    },
    // 5: Sample & Hold
    [](double /*phase*/, float noise) -> float {
        return noise;
    },
    // 6: Saw Down & One Shot
    [](double phase, float /*noise*/) -> float {
        return (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0);
    },
    // 7: Triangle & One Shot
    [](double phase, float /*noise*/) -> float {
        if (phase < 0.25)      return (float)(phase * 4.0);
        else if (phase < 0.5)  return (float)(1.0 - (phase - 0.25) * 4.0);
        else                   return 0.0;
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPM EG)
// -----------------------------------------------------------
const std::array<Opzx7Core::Opzx7LfoCalculator, 8> Opzx7Core::lfoEgStrategies = { {
    // 0: Sine
    [] (double phase, float /*noise*/)-> float {
        float pm = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);

        return (pm + 1.0f) * 0.5f;
    },
    // 1: Saw Up
    [](double phase, float /*noise*/) -> float {
        return (float)phase;
    },
    // 2: Saw Down
    [](double phase, float /*noise*/) -> float {
        return (float)(1.0 - phase);
    },
    // 3: Square
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? 1.0f : 0.0f;
    },
    // 4: Triangle
    [](double phase, float /*noise*/) -> float {
        float am = 0.0f;

        if (phase < 0.5) am = (float)(1.0 - phase * 2.0);
        else             am = (float)((phase - 0.5) * 2.0);

        return am;
    },
    // 5: Sample & Hold
    [](double /*phase*/, float noise) -> float {
        return (noise + 1.0f) * 0.5f;
    },
    // 6: Saw Down & One Shot
    [](double phase, float /*noise*/) -> float {
        return (float)(phase < 0.5 ? 1.0 - phase : 0.0);
    },
    // 7: Triangle & One Shot
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? (float)(phase * 2.0) : 0.0f;
    }
} };

void Opzx7Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    double target = getTargetRate(m_rateIndex);

    for (auto& op : m_operators) {
        op.prepare(sampleRate);
        op.setSampleRate(target);
        op.setHostSampleRate(m_hostSampleRate);
    }

    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;
    m_amSmooth = 0.0f;

    m_noiseGen.prepare(target);
}

void Opzx7Core::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;

        for (auto& op : m_operators) {
            op.updateSampleRate(m_hostSampleRate);
        }
    }
}

void Opzx7Core::setParameters(const SynthParams& params) {
    m_algorithm = params.opzx7.algorithm; // Range: 0-27

    m_lfoFreq = params.opzx7.lfoFreq;
    m_am = params.opzx7.amEnable;
    m_pm = params.opzx7.pmEnable;
    m_pms = params.opzx7.lfoPms;
    m_ams = params.opzx7.lfoAms;
    m_pmd = params.opzx7.lfoPmd;
    m_amd = params.opzx7.lfoAmd;
    m_lfoPgWave = params.opzx7.pgLfoWave;
    m_lfoEgWave = params.opzx7.egLfoWave;
    m_amSmoothRate = params.opzx7.lfoAmSmRt;
    m_lfoSyncDelayParam = params.opm.lfoSyncDelay;
    m_lfoSyncDelay = (float)(m_lfoSyncDelayParam - 1) * (1000.0f / 60.0f);

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

        for (auto& op : m_operators) op.setSampleRate(target);

        m_noiseGen.prepare(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opzx7.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.opzx7.feedback;
        }

        if (i == 2) // OP2
        {
            fb = params.opzx7.feedback2;
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
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
    m_rateAccumulator = 1.0;

    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    if (m_lfoSyncDelayParam == 0) {
        m_lfoDelayCounter = 0.0f; // フリーラン継続

        if (m_lfoPgWave == 6 || m_lfoPgWave == 7 || m_lfoEgWave == 6 || m_lfoEgWave == 7) {
            m_lfoPhase = 0.0;
        }
    }
    else if (m_lfoSyncDelayParam == 1) {
        m_lfoPhase = 0.0; // 位相を0に戻す (Sync)
        m_lfoDelayCounter = 0.0f; // ms -> 秒
    }
    else {
        m_lfoPhase = 0.0; // 位相を0に戻す (Sync)
        m_lfoDelayCounter = m_lfoSyncDelay / 1000.0f; // ms -> 秒
    }

    m_lfoCycleCount = 0;
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

        float pmLfoVal = 0.0f;
        float amLfoVal = 0.0f;

        if (m_lfoDelayCounter > 0.0f) {
            // カウントダウン (stepSize / targetRate は 1.0/m_hostSampleRate と等価)
            m_lfoDelayCounter -= 1.0f / (float)m_hostSampleRate;
            if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;

            // ディレイ中は pm=0, am=0 となるため何もしない
        }
        else {
            // --- 通常の LFO Processing ---
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;

            if (m_lfoPhase >= 1.0) {
                m_lfoPhase -= 1.0;
                m_lfoCycleCount++;
                m_currentNoiseSample = m_noiseGen.generate();
            }

            // ストラテジー配列を使ってLFO値を計算
            int pgWaveIdx = std::clamp(m_lfoPgWave, 0, 7);

            // ワンショット波形 (6, 7) のミュート処理
            if ((pgWaveIdx == 6 || pgWaveIdx == 7) && m_lfoCycleCount > 0)
            {
                pmLfoVal = 0.0f;
            }
            else
            {
                pmLfoVal = lfoPgStrategies[pgWaveIdx](m_lfoPhase, m_currentNoiseSample);
            }

            int egWaveIdx = std::clamp(m_lfoEgWave, 0, 7);

            // ワンショット波形 (6, 7) のミュート処理
            if ((egWaveIdx == 6 || egWaveIdx == 7) && m_lfoCycleCount > 0)
            {
                amLfoVal = 0.0f;
            }
            else
            {
                amLfoVal = lfoEgStrategies[egWaveIdx](m_lfoPhase, m_currentNoiseSample);
            }
        }

        m_amSmooth += (amLfoVal - m_amSmooth) * m_amSmoothRate;

        // Outputs
        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        int algIndex = std::clamp(m_algorithm, 0, 35);
        const auto& r = routings[algIndex];

        // =================================================================
        // 1. OP1 (入力は常に0.0)
        // =================================================================
        m_operators[0].getSample(out1, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // 2. OP2 (入力: OP1)
        // =================================================================
        float in2 = out1 * r.in2_1;
        m_operators[1].getSample(out2, in2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[1]) out2 = 0.0f;

        // フィードバック指定があれば OP2->OP1 にフィードバックをプッシュ
        if (r.fb2_1) {
            m_operators[0].pushFeedback(out2);
        }

        // =================================================================
        // 3. OP3 (入力: OP1, OP2)
        // =================================================================
        float in3 = (out1 * r.in3_1) + (out2 * r.in3_2);
        m_operators[2].getSample(out3, in3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[2]) out3 = 0.0f;

        // =================================================================
        // 4. OP4 (入力: OP1, OP2, OP3)
        // =================================================================
        float in4 = (out1 * r.in4_1) + (out2 * r.in4_2) + (out3 * r.in4_3);
        m_operators[3].getSample(out4, in4, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[3]) out4 = 0.0f;

        // =================================================================
        // 5. Final Output (各OPからマスターアウトへの加算)
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
