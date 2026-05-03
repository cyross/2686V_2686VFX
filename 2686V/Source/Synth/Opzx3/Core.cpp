#include "./Core.h"
#include "../../Core/Synth/SynthHelpers.h"

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPM PG)
// -----------------------------------------------------------
const std::array<Opzx3Core::Opzx3LfoCalculator, 8> Opzx3Core::lfoPgStrategies = { {
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
const std::array<Opzx3Core::Opzx3LfoCalculator, 8> Opzx3Core::lfoEgStrategies = { {
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


void Opzx3Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) {
        op.setSampleRate(target);
        op.setHostSampleRate(m_hostSampleRate);
    }
    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;

    updateNoiseDelta(target);

    m_amSmooth = 0.0f;

    for (int i = 0; i < 4; ++i) {
        // this をキャプチャすることで、再生中の最新のLFO値などを自動参照します
        m_opGetSamples[i] = [this, i](float& out, float in, float pmLfoVal) {

            m_operators[0].getSample(out, in, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            // マスク処理もここで吸収してしまう
            if (m_opMask[i]) out = 0.0f;
        };

        // フィードバック処理（OPZX3特有）
        m_opFbPushBacks[i] = [this, i](float fbVal) {
            m_operators[i].pushFeedback(fbVal);
        };
    }
}

void Opzx3Core::setParameters(const SynthParams& params) {
    m_alg.idx = params.opzx3.algorithm; // Range: 0-27
    m_lfoFreq = params.opzx3.lfoFreq;
    m_am = params.opzx3.amEnable;
    m_pm = params.opzx3.pmEnable;
    m_pms = params.opzx3.lfoPms;
    m_ams = params.opzx3.lfoAms;
    m_pmd = params.opzx3.lfoPmd;
    m_amd = params.opzx3.lfoAmd;
    m_lfoPgWave = params.opzx3.pgLfoWave;
    m_lfoEgWave = params.opzx3.egLfoWave;
    m_amSmoothRate = params.opzx3.lfoAmSmRt;
    m_lfoSyncDelay = params.opzx3.lfoSyncDelay;

    if (m_rateIndex != params.opzx3.rateIndex) {
        m_rateIndex = params.opzx3.rateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
        updateNoiseDelta(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opzx3.bitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.opzx3.feedback0;
        }

        if (i == 2) // OP2
        {
            fb = params.opzx3.feedback2;
        }

        // WaveSelect=True, SSG-EG=True, OpmEg=True
        m_operators[i].setParameters(params.opzx3.op[i], fb);
        m_opMask[i] = params.opzx3.op[i].mask;
    }

    // OPX特有の外部フィードバックアルゴリズムの場合、OP0/OP2の自己FBをオフにする
    bool useExtFb = m_alg.isUseExtFb();

    m_operators[0].setExternalFeedbackMode(useExtFb);
    m_operators[2].setExternalFeedbackMode(useExtFb);
}

void Opzx3Core::noteOn(float freq, float velocity, int midiNote) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
    m_rateAccumulator = 1.0;

    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    if (m_lfoSyncDelay > 0.0f) {
        m_lfoPhase = 0.0; // 位相を0に戻す (Sync)
        m_lfoDelayCounter = m_lfoSyncDelay / 1000.0f; // ms -> 秒
    }
    else {
        m_lfoDelayCounter = 0.0f; // フリーラン継続

        if (m_lfoPgWave == 6 || m_lfoPgWave == 7 || m_lfoEgWave == 6 || m_lfoEgWave == 7) {
            m_lfoPhase = 0.0;
        }
    }

    m_lfoCycleCount = 0;
}

void Opzx3Core::noteOff()
{
    for (auto& op : m_operators) op.noteOff();
}

bool Opzx3Core::isPlaying() const
{
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

void Opzx3Core::setPitchBend(int pitchWheelValue)
{
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    float semitones = 2.0f;
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);
    for (auto& op : m_operators) op.setPitchBendRatio(ratio);
}

void Opzx3Core::setModulationWheel(int wheelValue)
{
    m_modWheel = (float)wheelValue / 127.0f;
}

float Opzx3Core::getSample() {
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

                unsigned int bit0 = m_lfsr & 1;
                unsigned int bit3 = (m_lfsr >> 3) & 1;
                unsigned int nextBit = bit0 ^ bit3;
                m_lfsr >>= 1;
                if (nextBit) m_lfsr |= (1 << 16);

                m_currentNoiseSample = ((m_lfsr % 1000) / 500.0f) - 1.0f;
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

		float finalOut = m_alg.process(m_opGetSamples, m_opFbPushBacks, pmLfoVal);

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

void Opzx3Core::updateNoiseDelta(double targetRate) {
    if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
}

void Opzx3Core::setPcmBuffer(int opIndex, const std::vector<float>* pcmData)
{
    if (opIndex >= 0 && opIndex < 4) {
        m_operators[opIndex].setPcmBuffer(pcmData);
    }
}

void Opzx3Core::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample;
    outR[startSample + sampleIdx] += sample;

    isActive = isPlaying();
}

const std::array<Opzx3Core::BaseAlgRouting, 8> Opzx3Core::baseRoutings = { {
        // in2_1, in3_1, in3_2, in4_1, in4_2, in4_3, out_1, out_2, out_3, out_4
        { 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 0: 1->2->3->4
        { 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 1: (1+2)->3->4
        { 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 2: (1+3)->4, 2->3
        { 1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 3: (1->2) + 3 -> 4
        { 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 4: (1->2) + (3->4)
        { 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 5: 1 -> (2, 3, 4)
        { 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 6: (1->2) + 3 + 4
        { 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }  // 7: 1 + 2 + 3 + 4
    } };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (オリジナル)
// -----------------------------------------------------------
const std::array<Opzx3Core::LfoCalculator, 5> Opzx3Core::lfoStrategies = { {
        // 0: Sine
        [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
            float pm = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
            return { pm, (pm + 1.0f) * 0.5f };
        },
    // 1: Saw Down
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        return { (float)(1.0 - phase), (float)(1.0 - phase) };
    },
    // 2: Square
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 3: Triangle
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 4: Noise
    [](double /*phase*/, float noise) -> Opzx3Core::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC準拠)
// -----------------------------------------------------------
const std::array<Opzx3Core::LfoCalculator, 4> Opzx3Core::lfoN88Strategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
            return { (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0), (float)(phase) };
        },
    // 1: Square
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> Opzx3Core::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC(86)準拠)
// -----------------------------------------------------------
const std::array<Opzx3Core::LfoCalculator, 6> Opzx3Core::lfoN8886Strategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
            return { (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0), (float)(phase) };
        },
    // 1: Square
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> Opzx3Core::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    },
    // 4: Saw Down & One Shot
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        return { (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0), (float)(phase < 0.5 ? 1.0 - phase : 0.0) };
    },
    // 5: Triangle & One Shot
    [](double phase, float /*noise*/) -> Opzx3Core::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.5)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = 0.0;

        float am = (phase < 0.5) ? (float)(phase * 2.0) : 0.0f;
        return { pm, am };
    }
} };
