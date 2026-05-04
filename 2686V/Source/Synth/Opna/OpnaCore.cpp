#include "./OpnaCore.h"
#include "../../Core/Synth/SynthHelpers.h"

void OpnaCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    // Update operators with current target rate
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);

    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0; // Ready to render

    m_lfoTimerAcc = 1.0;
    m_steppedPmLfoVal = 0.0f;
    m_steppedAmLfoVal = 0.0f;

    updateNoiseDelta(target);

    m_amSmooth = 0.0f;
}

void OpnaCore::setParameters(const SynthParams& params) {
    m_algorithm = params.opna.algorithm;
    m_lfoFreq = params.opna.lfoFreq;
    m_am = params.opna.amEnable;
    m_pm = params.opna.pmEnable;
    m_pms = params.opna.lfoPms;
    m_amd = params.opna.lfoAmd;
    m_pmd = params.opna.lfoPmd;
    m_lfoWave = params.opna.lfoWave;
    m_amSmoothRate = params.opna.lfoAmSmRt;
    m_lfoSyncDelay = params.opna.lfoSyncDelay;

    if (m_rateIndex != params.opna.fmRateIndex) {
        m_rateIndex = params.opna.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);

        updateNoiseDelta(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opna.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.opna.feedback;
        }
        else if (i == 2) // OP2
        {
            fb = params.opna.feedback2;
        }

        // OPNA: SSG-EG=True, WaveSelect=False
        // params.fmSsgEgFreq を渡す (第6引数)
        // 第5引数(useOpmEg)は false
        m_operators[i].setParameters(params.opna.op[i], fb);
        m_opMask[i] = params.opna.op[i].mask;
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

    m_lfoCycleCount = 0;
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
                FmCore::LfoResult lfoVal = FmCore::lfoN8886Strategies[waveIdx](m_lfoPhase, m_currentNoiseSample);

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
