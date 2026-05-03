#include "./Core.h"
#include "../../Core/Synth/SynthHelpers.h"

void OpnCore::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = SynthHelpers::getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;

    m_lfoTimerAcc = 1.0;
    m_steppedPmLfoVal = 0.0f;
    m_steppedAmLfoVal = 0.0f;

    updateNoiseDelta(target);

    m_amSmooth = 0.0f;

    for (int i = 0; i < 4; ++i) {
        // this をキャプチャすることで、再生中の最新のLFO値などを自動参照します
        m_opGetSamples[i] = [this, i](float& out, float in) {

            m_operators[i].getSample(
                out, in,
                m_amSmooth, m_n88Lfo.pm.value, m_globalLfo.pm.enable, m_globalLfo.am.enable, m_globalLfo.pm.sensitivity, 0.0f,
                m_globalLfo.pm.depth, m_globalLfo.am.depth, m_modWheel
            );

            // マスク処理もここで吸収してしまう
            if (m_opMask[i]) out = 0.0f;
            };
    }
}

void OpnCore::setParameters(const SynthParams& params)
{
    m_alg.idx = params.opn.algorithm;
    m_lfoWave = params.opn.lfoWave;
    m_amSmoothRate = params.opn.lfoAmSmRt;
    m_lfoSyncDelay = params.opn.lfoSyncDelay;
    m_lfoFreq = params.opn.lfoFreq;
    m_pm = params.opn.pmEnable;
    m_am = params.opn.amEnable;
    m_pms = params.opn.lfoPms;
    m_amd = params.opn.lfoAmd;
    m_pmd = params.opn.lfoPmd;

    if (m_rateIndex != params.opn.rateIndex) {
        m_rateIndex = params.opn.rateIndex;
        double target = SynthHelpers::getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);

        updateNoiseDelta(target);
    }

    m_quantizeSteps = SynthHelpers::getTargetBitDepth(params.opn.bitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.opn.feedback0;
        }
        else if (i == 2) // OP2
        {
            fb = params.opn.feedback2;
        }

        // OPN: SSG-EG=False, WaveSelect=False
        m_operators[i].setParameters(params.opn.op[i], fb);
        m_opMask[i] = params.opn.op[i].mask;
    }
}

void OpnCore::noteOn(float freq, float velocity, int midiNote)
{
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);
    m_rateAccumulator = 1.0;

    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    if (m_lfoSyncDelay > 0.0f) {
        m_lfoPhase = 0.0; // 位相を0に戻す (Sync)
        m_lfoDelayCounter = m_lfoSyncDelay / 1000.0f; // ms -> 秒
    }
    else {
        m_lfoDelayCounter = 0.0f; // フリーラン継続
    }

    m_lfoCycleCount = 0;
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
    double targetRate = SynthHelpers::getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        float pmLfoVal = 0.0f;
        float amLfoVal = 0.0f;

        if (m_lfoDelayCounter > 0.0f) {
            m_lfoDelayCounter -= 1.0f / (float)targetRate;
            if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;

            // ディレイ中は pm=0, am=0 となるため何もしない
        }
        else {
            // =================================================================
            // 60Hz ソフトウェアタイマーのシミュレート
            // =================================================================
            double timerInc = 60.0 / targetRate; // 1サンプルあたりに進む60Hzタイマーの割合
            m_lfoTimerAcc += timerInc;

            // 1/60秒に1回だけ、LFOの計算をガクッと進める！
            if (m_lfoTimerAcc >= 1.0) {
                m_lfoTimerAcc -= 1.0;

                // m_lfoFreq(Hz) を 60(Hz) で割ることで、1回の割り込みあたりの位相増分になる
                m_lfoPhase += (m_lfoFreq / 60.0);

                // ノイズの更新処理とサイクルカウント
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

                // OPNの場合は 0〜3 (lfoN88Strategies)
                // OPNAの場合は 0〜5 (lfoN8886Strategies) などの配列サイズに合わせる
                int waveIdx = std::clamp(m_lfoWave, 0, 5);
                OpnCore::LfoResult lfoVal = OpnCore::lfoN8886Strategies[waveIdx](m_lfoPhase, m_currentNoiseSample);

                if ((waveIdx == 4 || waveIdx == 5) && m_lfoCycleCount > 0) {
                    lfoVal.pm = 0.0f;
                    lfoVal.am = 0.0f;
                }

                // 計算した結果を保持する
                m_steppedPmLfoVal = lfoVal.pm;
                m_steppedAmLfoVal = lfoVal.am;
            }

            // 保持されている値(階段状の波形)を出力に回す
            pmLfoVal = m_steppedPmLfoVal;
            amLfoVal = m_steppedAmLfoVal;
        }

        m_amSmooth += (amLfoVal - m_amSmooth) * m_amSmoothRate;

        float finalOut = m_alg.process(m_opGetSamples);

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

void OpnCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample;
    outR[startSample + sampleIdx] += sample;

    isActive = isPlaying();
}

const std::array<OpnCore::BaseAlgRouting, 8> OpnCore::baseRoutings = { {
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
const std::array<OpnCore::LfoCalculator, 5> OpnCore::lfoStrategies = { {
        // 0: Sine
        [](double phase, float /*noise*/) -> OpnCore::LfoResult {
            float pm = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
            return { pm, (pm + 1.0f) * 0.5f };
        },
    // 1: Saw Down
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        return { (float)(1.0 - phase), (float)(1.0 - phase) };
    },
    // 2: Square
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 3: Triangle
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 4: Noise
    [](double /*phase*/, float noise) -> OpnCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC準拠)
// -----------------------------------------------------------
const std::array<OpnCore::LfoCalculator, 4> OpnCore::lfoN88Strategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> OpnCore::LfoResult {
            return { (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0), (float)(phase) };
        },
    // 1: Square
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> OpnCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC(86)準拠)
// -----------------------------------------------------------
const std::array<OpnCore::LfoCalculator, 6> OpnCore::lfoN8886Strategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> OpnCore::LfoResult {
            return { (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0), (float)(phase) };
        },
    // 1: Square
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> OpnCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    },
    // 4: Saw Down & One Shot
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        return { (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0), (float)(phase < 0.5 ? 1.0 - phase : 0.0) };
    },
    // 5: Triangle & One Shot
    [](double phase, float /*noise*/) -> OpnCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.5)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = 0.0;

        float am = (phase < 0.5) ? (float)(phase * 2.0) : 0.0f;
        return { pm, am };
    }
} };
