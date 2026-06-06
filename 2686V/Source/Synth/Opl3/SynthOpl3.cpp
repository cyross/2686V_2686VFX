#include "./SynthOpl3.h"

#include "../../Core/Synth/SynthHelpers.h"

const std::array<Opl3Core::AlgRouting, 7> Opl3Core::routings = { {
    // in2_1, in3_2, in4_3, out_1, out_2, out_3, out_4
    { 1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 0: 1(FB) -> 2 -> 3 -> 4
    { 0.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // 1: 1(FB) + (2 -> 3 -> 4)
    { 1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 2: (1(FB) -> 2) + (3 -> 4)
    { 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f }, // 3: 1(FB) + (2 -> 3) + 4
    { 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }, // 4: 1(FB) + 2 + 3 + 4
    { 1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 5: (1(FB) -> 2) + (3(FB) -> 4)
    { 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }  // 6: 1(FB) + 2 + 3(FB) + 4
} };

void Opl3Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    double target = getTargetRate(m_rateIndex);

    // 高速化のためのループアンローリング
    m_operators[0].prepare(1, target);
    m_operators[1].prepare(2, target);
    m_operators[2].prepare(3, target);
    m_operators[3].prepare(4, target);

    m_rateAccumulator = 1.0;
}

void Opl3Core::setCurveCore(CurveCore* p_curveCore)
{
    // 高速化のためのループアンローリング
    m_operators[0].setCurveCore(p_curveCore);
    m_operators[1].setCurveCore(p_curveCore);
    m_operators[2].setCurveCore(p_curveCore);
    m_operators[3].setCurveCore(p_curveCore);
}

void Opl3Core::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

// --- Opl3Core.cpp : setParameters() 内 ---

void Opl3Core::setParameters(const SynthParams& params) {
    m_algorithm = params.opl3.algorithm;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    if (m_rateIndex != params.opl3.fmRateIndex) {
        m_rateIndex = params.opl3.fmRateIndex;

		double target = getTargetRate(m_rateIndex);

        // 高速化のためのループアンローリング
		m_operators[0].setSampleRate(target);
		m_operators[1].setSampleRate(target);
		m_operators[2].setSampleRate(target);
		m_operators[3].setSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opl3.fmBitDepth);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opl3.op[0], params.opl3.feedback);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_operators[0].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[0] = params.opl3.op[0].mask;
    m_operators[1].setParameters(params.opl3.op[1], 0.0f);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opl3.op[1].mask;
    m_operators[2].setParameters(params.opl3.op[2], (m_algorithm == 5 || m_algorithm == 6) ? params.opl3.feedback : 0.0f);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_operators[2].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[2] = params.opl3.op[2].mask;
    m_operators[3].setParameters(params.opl3.op[3], 0.0f);
    m_operators[3].setMonoMode(m_isMonoMode);
    m_operators[3].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[3] = params.opl3.op[3].mask;
}

void Opl3Core::noteOn(float freq, float velocity, int midiNote, bool isLegato) {
    // ※トランペット系の音が歪む課題に対応した、かなりな力技
    // 通常のvelocityでは1.0に近くなると音が歪むため、0.25倍して十分な余裕を持たせます。最低値は0.01にして完全な無音を防止します。
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

    m_rateAccumulator = 0.0; // レートの余りもリセット
}
void Opl3Core::noteOff() {
    m_operators[0].noteOff();
    m_operators[1].noteOff();
    m_operators[2].noteOff();
    m_operators[3].noteOff();
}
bool Opl3Core::isPlaying() const {
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_operators[2].isPlaying()) return true;
    if (m_operators[3].isPlaying()) return true;

    return false;
}
// ピッチベンド (0 - 16383, Center=8192)
void Opl3Core::setPitchBend(int pitchWheelValue)
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
void Opl3Core::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    float modWheel = (float)wheelValue / 127.0f;

    m_operators[0].setModWheel(modWheel);
    m_operators[1].setModWheel(modWheel);
    m_operators[2].setModWheel(modWheel);
    m_operators[3].setModWheel(modWheel);
}

// --- Opl3Core.cpp : getSample() の全体 ---

float Opl3Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;
        m_prevSample = m_lastSample;

        m_operators[0].processLfo();
        m_operators[1].processLfo();
        m_operators[2].processLfo();
        m_operators[3].processLfo();

        // -------------------------------

        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        // 安全のため 0〜6 の範囲に丸める
        int algIndex = std::clamp(m_algorithm, 0, 6);
        const auto& r = routings[algIndex];

        // =================================================================
        // OP1 (入力なし)
        // =================================================================
        m_operators[0].getSample(out1, 0.0f);
        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // OP2 (入力: OP1)
        // =================================================================
        float in2 = out1 * r.in2_1;
        m_operators[1].getSample(out2, in2);
        if (m_opMask[1]) out2 = 0.0f;

        // =================================================================
        // OP3 (入力: OP2)
        // =================================================================
        float in3 = out2 * r.in3_2;
        m_operators[2].getSample(out3, in3);
        if (m_opMask[2]) out3 = 0.0f;

        // =================================================================
        // OP4 (入力: OP3)
        // =================================================================
        float in4 = out3 * r.in4_3;
        m_operators[3].getSample(out4, in4);
        if (m_opMask[3]) out4 = 0.0f;

        // =================================================================
        // Final Output
        // =================================================================
        // velocity を 0.25倍した補正として、finalOut を 2 倍しています。
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

void Opl3Core::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
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
