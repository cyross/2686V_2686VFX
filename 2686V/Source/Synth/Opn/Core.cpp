#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"

void OpnCore::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    double target = getTargetRate(m_rateIndex);

    // 高速化のためのループアンローリング
    m_operators[0].prepare(1, target);
    m_operators[1].prepare(2, target);
    m_operators[2].prepare(3, target);
    m_operators[3].prepare(4, target);

    m_rateAccumulator = 1.0;

    m_lfoTimerAcc = 1.0;
    m_steppedPmLfoVal = 0.0f;
    m_steppedAmLfoVal = 0.0f;
    m_amSmooth = 0.0f;

	m_noiseGen.prepare(target);
    m_n88Lfo.prepare(target);
}

void OpnCore::setCurveCore(CurveCore* p_curveCore)
{
    // 高速化のためのループアンローリング
    m_operators[0].setCurveCore(p_curveCore);
    m_operators[1].setCurveCore(p_curveCore);
    m_operators[2].setCurveCore(p_curveCore);
    m_operators[3].setCurveCore(p_curveCore);
}

void OpnCore::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void OpnCore::setParameters(const SynthParams& params)
{
    m_algorithm = params.opn.algorithm;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_n88Lfo.setParameters(
        params.opn.lfoSyncDelay,
        params.opn.pmEnable,
        params.opn.amEnable,
        params.opn.lfoFreq,
        params.opn.lfoFreq,
        params.opn.lfoWave,
        params.opn.lfoWave,
        params.opn.lfoPms,
        params.opn.lfoPmd,
        params.opn.lfoAmd,
        params.opn.lfoAmSmRt
    );

    if (m_rateIndex != params.opn.fmRateIndex) {
        m_rateIndex = params.opn.fmRateIndex;

		double target = getTargetRate(m_rateIndex);

        // 高速化のためのループアンローリング
        m_operators[0].setSampleRate(target);
        m_operators[1].setSampleRate(target);
        m_operators[2].setSampleRate(target);
        m_operators[3].setSampleRate(target);

        m_noiseGen.updateDelta(target);
        m_n88Lfo.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opn.fmBitDepth);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opn.op[0], m_algorithm != 2 ? params.opn.feedback : 0.0f);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_opMask[0] = params.opn.op[0].mask;
    m_operators[1].setParameters(params.opn.op[1], 0.0f);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_opMask[1] = params.opn.op[1].mask;
    m_operators[2].setParameters(params.opn.op[2], m_algorithm == 2 ? params.opn.feedback : 0.0f);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_opMask[2] = params.opn.op[2].mask;
    m_operators[3].setParameters(params.opn.op[3], 0.0f);
    m_operators[3].setMonoMode(m_isMonoMode);
    m_opMask[3] = params.opn.op[3].mask;
}

void OpnCore::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
    float gain = std::max(0.01f, velocity * 0.25f);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));

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

    m_n88Lfo.noteOn();
}

void OpnCore::noteOff()
{
    m_operators[0].noteOff();
    m_operators[1].noteOff();
    m_operators[2].noteOff();
    m_operators[3].noteOff();
}

bool OpnCore::isPlaying() const
{
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_operators[2].isPlaying()) return true;
    if (m_operators[3].isPlaying()) return true;

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
    m_operators[0].setPitchBendRatio(ratio);
    m_operators[1].setPitchBendRatio(ratio);
    m_operators[2].setPitchBendRatio(ratio);
    m_operators[3].setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void OpnCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float OpnCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        m_n88Lfo.getSample();

        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        // 安全のため 0〜7 の範囲に丸める
        int algIndex = std::clamp(m_algorithm, 0, 7);
        const auto& r = baseRoutings[algIndex];

        // =================================================================
        // OP1 (入力なし。フィードバックは内部で解決される)
        // =================================================================
        m_operators[0].getSample(out1, 0.0f, m_n88Lfo, m_modWheel);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // OP2 (入力: OP1)
        // =================================================================
        float in2 = out1 * r.in2_1;
        m_operators[1].getSample(out2, in2, m_n88Lfo, m_modWheel);
        if (m_opMask[1]) out2 = 0.0f;

        // =================================================================
        // OP3 (入力: OP1, OP2)
        // =================================================================
        float in3 = (out1 * r.in3_1) + (out2 * r.in3_2);
        m_operators[2].getSample(out3, in3, m_n88Lfo, m_modWheel);
        if (m_opMask[2]) out3 = 0.0f;

        // =================================================================
        // OP4 (入力: OP1, OP2, OP3)
        // =================================================================
        float in4 = (out1 * r.in4_1) + (out2 * r.in4_2) + (out3 * r.in4_3);
        m_operators[3].getSample(out4, in4, m_n88Lfo, m_modWheel);
        if (m_opMask[3]) out4 = 0.0f;

        // =================================================================
        // Final Output
        // =================================================================
        finalOut = ((out1 * r.out_1) + (out2 * r.out_2) + (out3 * r.out_3) + (out4 * r.out_4)) * 2.0f;

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

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

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
