#include "OpnaCore.h"
#include "../synth/SynthHelpers.h"

void OpnaCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    // Update operators with current target rate
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);

    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0; // Ready to render

    updateNoiseDelta(target);

    m_amSmooth = 0.0f;
}

void OpnaCore::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm;
    m_lfoFreq = params.lfoFreq;
    m_am = params.amEnable;
    m_pm = params.pmEnable;
    m_pms = params.lfoPms;
    m_amd = params.lfoAmd;
    m_pmd = params.lfoPmd;
    m_lfoWave = params.lfoWave;
    m_amSmoothRate = params.lfoAmSmRt;
    m_lfoSyncDelay = params.lfoSyncDelay;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);

        updateNoiseDelta(target);
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
        m_operators[i].setParameters(params.fmOp[i], fb);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void OpnaCore::noteOn(float freq, float velocity, int midiNote) {
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);

    m_rateAccumulator = 1.0; // Reset timing

    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    if (m_lfoSyncDelay > 0.0f) {
        m_lfoPhase = 0.0; // 位相を0に戻す (Sync)
        m_lfoDelayCounter = m_lfoSyncDelay / 1000.0f; // ms -> 秒
    }
    else {
        m_lfoDelayCounter = 0.0f; // フリーラン継続
    }
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

        float pmLfoVal = 0.0f;
        float amLfoVal = 0.0f;

        if (m_lfoDelayCounter > 0.0f) {
            m_lfoDelayCounter -= 1.0f / (float)targetRate;
            if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;

            // ディレイ中は pm=0, am=0 となるため何もしない
        }
        else {
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;

            // ノイズの更新処理
            if (m_lfoPhase >= 1.0) {
                m_lfoPhase -= 1.0;
                unsigned int bit0 = m_lfsr & 1;
                unsigned int bit3 = (m_lfsr >> 3) & 1;
                unsigned int nextBit = bit0 ^ bit3;
                m_lfsr >>= 1;
                if (nextBit) m_lfsr |= (1 << 16);
                m_currentNoiseSample = ((m_lfsr % 1000) / 500.0f) - 1.0f;
            }

            int waveIdx = std::clamp(m_lfoWave, 0, 6);
            FmCore::LfoResult lfoVal = FmCore::lfoN8886Strategies[waveIdx](m_lfoPhase, m_currentNoiseSample);

            pmLfoVal = lfoVal.pm;
            amLfoVal = lfoVal.am;
        }

        m_amSmooth += (amLfoVal - m_amSmooth) * m_amSmoothRate;

        // ========================================================
        // 2. オペレーターの処理とルーティング
        // ========================================================
        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        // 安全のため 0〜7 の範囲に丸める
        int algIndex = std::clamp(m_algorithm, 0, 7);
        const auto& r = baseRoutings[algIndex];

        // =================================================================
        // OP1 (入力なし。フィードバックは内部で解決される)
        // =================================================================
        m_operators[0].getSample(out1, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, 0.0f, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // OP2 (入力: OP1)
        // =================================================================
        float in2 = out1 * r.in2_1;
        m_operators[1].getSample(out2, in2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, 0.0f, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[1]) out2 = 0.0f;

        // =================================================================
        // OP3 (入力: OP1, OP2)
        // =================================================================
        float in3 = (out1 * r.in3_1) + (out2 * r.in3_2);
        m_operators[2].getSample(out3, in3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, 0.0f, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[2]) out3 = 0.0f;

        // =================================================================
        // OP4 (入力: OP1, OP2, OP3)
        // =================================================================
        float in4 = (out1 * r.in4_1) + (out2 * r.in4_2) + (out3 * r.in4_3);
        m_operators[3].getSample(out4, in4, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, 0.0f, (float)m_pmd, (float)m_amd, m_modWheel);
        if (m_opMask[3]) out4 = 0.0f;

        // =================================================================
        // Final Output
        // =================================================================
        finalOut = ((out1 * r.out_1) + (out2 * r.out_2) + (out3 * r.out_3) + (out4 * r.out_4)) * 0.25f;

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

void OpnaCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample;
    outR[startSample + sampleIdx] += sample;

    isActive = isPlaying();
}
