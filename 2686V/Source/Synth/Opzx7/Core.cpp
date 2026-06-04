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

    // 高速化のためのループアンローリング
    m_operators[0].prepare(1, target);
    m_operators[1].prepare(2, target);
    m_operators[2].prepare(3, target);
    m_operators[3].prepare(4, target);

    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;
    m_amSmooth = 0.0f;

    m_lfo.prepare(target);
}

void Opzx7Core::setCurveCore(CurveCore* p_curveCore)
{
    // 高速化のためのループアンローリング
    m_operators[0].setCurveCore(p_curveCore);
    m_operators[1].setCurveCore(p_curveCore);
    m_operators[2].setCurveCore(p_curveCore);
    m_operators[3].setCurveCore(p_curveCore);
}

void Opzx7Core::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void Opzx7Core::setParameters(const SynthParams& params) {
    m_algorithm = params.opzx7.algorithm; // Range: 0-27

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

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
        m_panpot_l_rate = 1.0f;
        m_panpot_r_rate = 1.0f;
    }

    if (m_rateIndex != params.opzx7.fmRateIndex) {
        m_rateIndex = params.opzx7.fmRateIndex;

        double target = getTargetRate(m_rateIndex);

        // 高速化のためのループアンローリング
        m_operators[0].setSampleRate(target);
        m_operators[1].setSampleRate(target);
        m_operators[2].setSampleRate(target);
        m_operators[3].setSampleRate(target);

        m_lfo.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opzx7.fmBitDepth);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opzx7.op[0], m_algorithm != 2 ? params.opzx7.feedback : 0.0f);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_operators[0].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[0] = params.opzx7.op[0].mask;
    m_operators[1].setParameters(params.opzx7.op[1], 0.0f);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opzx7.op[1].mask;
    m_operators[2].setParameters(params.opzx7.op[2], (m_algorithm == 30 || m_algorithm == 33) ? params.opzx7.feedback : 0.0f);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_operators[2].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[2] = params.opzx7.op[2].mask;
    m_operators[3].setParameters(params.opzx7.op[3], 0.0f);
    m_operators[3].setMonoMode(m_isMonoMode);
    m_operators[3].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[3] = params.opzx7.op[3].mask;

    // OPX特有の外部フィードバックアルゴリズムの場合、OP0の自己FBをオフにする
    bool useExtFb = false;
    if (m_algorithm == 1 || m_algorithm == 5 || m_algorithm == 7 || m_algorithm == 11) useExtFb = true; // 4OP: OP1->OP0 FB
    if (m_algorithm == 17 || m_algorithm == 21) useExtFb = true; // 3OP: OP2->OP0 FB
    if (m_algorithm == 25) useExtFb = true; // 2OP: OP1->OP0 FB

    m_operators[0].setExternalFeedbackMode(useExtFb);
}

void Opzx7Core::noteOn(float freq, float velocity, int midiNote, bool isLegato) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    float gain = std::max(0.01f, velocity * 0.25f);

    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = freq;

    // ユニゾン時の位相のズレ(0.0〜1.0)を算出
    float phaseOffsetNorm = 0.0f;

    if (m_unisonTotal > 1) {
        // 現在のボイスが全体のどこに配置されるかを -1.0(一番下) 〜 1.0(一番上) で算出
        // 例(3ボイス): -1.0,  0.0,  1.0
        // 例(4ボイス): -1.0, -0.33, 0.33, 1.0
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f;

        // 最大デチューン幅に位置を掛け合わせて、このボイスのズレ量(セント)を決定
        float centOffset = spreadPos * (float)m_unisonDetuneAmt;

        // セント値(Cents) を周波数倍率(Ratio)に変換する
        // (1200セント ＝ 1オクターブ ＝ 周波数2倍)
        finalFreq = freq * std::pow(2.0f, centOffset / 1200.0f);

        // ボイスインデックスに応じて位相を均等に散らす (例: 3ボイスなら 0.0, 0.33, 0.66)
        phaseOffsetNorm = (float)m_unisonIndex / (float)m_unisonTotal;
    }

    // ユニゾン・ハーモニー向けに変更
    // 計算した位相のズレをオペレータに渡す
    m_operators[0].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[1].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[2].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[3].setUnisonPhaseOffset(phaseOffsetNorm);

    m_operators[0].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[1].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[2].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[3].noteOn(finalFreq, gain, noteNum, isLegato);

    m_lfoPhase = 0.0; // LFO位相をリセット
    m_rateAccumulator = 0.0; // レートの余りもリセット

    m_lfo.noteOn();
}

void Opzx7Core::noteOff()
{
    m_operators[0].noteOff();
    m_operators[1].noteOff();
    m_operators[2].noteOff();
    m_operators[3].noteOff();
}

bool Opzx7Core::isPlaying() const
{
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_operators[2].isPlaying()) return true;
    if (m_operators[3].isPlaying()) return true;

    return false;
}

void Opzx7Core::setPitchBend(int pitchWheelValue)
{
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    float semitones = 2.0f;
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    m_operators[0].setPitchBendRatio(ratio);
    m_operators[1].setPitchBendRatio(ratio);
    m_operators[2].setPitchBendRatio(ratio);
    m_operators[3].setPitchBendRatio(ratio);
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

    // ユニゾン・ハーモニー向けに変更
    float basePanL = m_panpot_l_rate;
    float basePanR = m_panpot_r_rate;

    if (m_unisonTotal > 1) {
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f; // -1.0 to 1.0

        // spreadPosが -1(L) の時、Right側の音量を下げる。逆も然り。
        float panOffset = spreadPos * m_unisonSpreadAmt * 0.5f; // 最大で ±0.5 動く

        basePanL = std::clamp(basePanL - panOffset, 0.0f, 1.0f);
        basePanR = std::clamp(basePanR + panOffset, 0.0f, 1.0f);

        // 音量補正 (ボイス数が増えると爆音になるため下げる)
        // ルートを取るか、単純に割るかは好みですが、単純割りの方が安全です
        float gainComp = 1.0f / std::sqrt((float)m_unisonTotal);
        sample *= gainComp;
    }

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
