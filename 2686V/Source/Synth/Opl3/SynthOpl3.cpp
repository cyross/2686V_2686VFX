#include "./SynthOpl3.h"

#include "../../Core/Synth/SynthHelpers.h"

// ============================================================================
// マトリクスを簡単に構築するためのヘルパー関数 (6オペ完全対応・拡張フィードバック)
// ============================================================================
Opl3Core::AlgRouting makeAlgOpl3(
    std::initializer_list<int> carriers,
    std::initializer_list<std::pair<int, int>> mods,
    std::initializer_list<std::pair<int, int>> fbMods) // NEW: フィードバックの接続リスト
{
    Opl3Core::AlgRouting r;

    // 出力マトリクスを初期化
    r.out.fill(0.0f);
    for (int c : carriers) r.out[c] = 1.0f;

    // 通常の変調マトリクスを初期化
    for (auto& row : r.mod) row.fill(0.0f);
    for (auto& m : mods) {
        // m.first = 接続元(src), m.second = 接続先(dest)
        r.mod[m.second][m.first] = 1.0f;
    }

    // フィードバック変調マトリクスを初期化
    for (auto& row : r.fbMod) row.fill(0.0f);
    for (auto& m : fbMods) {
        // m.first = 接続元(src), m.second = 接続先(dest)
        r.fbMod[m.second][m.first] = 1.0f;
    }

    return r;
}

// ============================================================================
// アルゴリズムの定義 (例: 32種類と仮定)
// ============================================================================
const std::array<Opl3Core::AlgRouting, Opl3PrValue::algorithms> Opl3Core::routings = { {
    makeAlgOpl3({3}, {{0, 1}, {1, 2}, {2, 3}}, {{0, 0}}),    // 00
    makeAlgOpl3({1, 3}, {{0, 1}, {2, 3}}, {{0, 0}}),         // 01
    makeAlgOpl3({0, 3}, {{1, 2}, {2, 3}}, {{0, 0}}),         // 02
    makeAlgOpl3({0, 2, 3}, {{1, 2}}, {{0, 0}}),              // 03
    makeAlgOpl3({1, 2, 3}, {{0, 2}}, {{0, 0}}),              // 04
    makeAlgOpl3({0, 1, 2, 3}, {}, {{0, 0}}),                 // 05
    makeAlgOpl3({1, 3}, {{0, 1}, {2, 3}}, {{0, 0}, {2, 2}}), // 06
    makeAlgOpl3({0, 1, 2, 3}, {}, {{0, 0}, {2, 2}}),         // 07
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
    m_level = params.opl3.level;

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
    m_operators[1].setParameters(params.opl3.op[1], params.opl3.feedback);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opl3.op[1].mask;
    m_operators[2].setParameters(params.opl3.op[2], params.opl3.feedback);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_operators[2].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[2] = params.opl3.op[2].mask;
    m_operators[3].setParameters(params.opl3.op[3], params.opl3.feedback);
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

        std::array<float, Opl3PrValue::ops> currentOut = { 0.0f };
        float finalOut = 0.0f;

        int algIndex = std::clamp(m_algorithm, 0, Opl3PrValue::algorithms - 1);
        const auto& r = routings[algIndex];

        // =================================================================
        // オペレータの評価 (OP1 -> OP6 の正順で計算)
        // =================================================================
        for (int i = 0; i < Opl3PrValue::ops; ++i) { // 0 から 5 へ
            float modulator = 0.0f;
            float fbModulator = 0.0f;

            // 1. 通常の変調入力 (mod)
            for (int src = 0; src < Opl3PrValue::ops; ++src) {
                if (r.mod[i][src] > 0.0f) {
                    // src が i より「小さい」なら既に計算済み(currentOut)、
                    // 大きいなら未計算なので1サンプル前の history1 を使う
                    float srcVal = (src < i) ? currentOut[src] : m_history1[src];

                    modulator += srcVal * r.mod[i][src];
                }
            }

            // 2. フィードバック変調入力 (fbMod)
            for (int src = 0; src < Opl3PrValue::ops; ++src) {
                if (r.fbMod[i][src] > 0.0f) {
                    // フィードバックは常に「過去2サンプルの平均」
                    float averageFb = (m_history1[src] + m_history2[src]) * 0.5f;

                    fbModulator += averageFb * r.fbMod[i][src];
                }
            }

            // 3. オペレータを計算
            m_operators[i].getSample(currentOut[i], modulator, fbModulator);

            if (m_opMask[i]) currentOut[i] = 0.0f;
        }

        // =================================================================
        // 履歴 (History) のシフト
        // =================================================================
        m_history2 = m_history1;
        m_history1 = currentOut;

        // =================================================================
        // Final Output (各OPからマスターアウトへの加算)
        // =================================================================
        for (int i = 0; i < Opl3PrValue::ops; ++i) {
            finalOut += currentOut[i] * r.out[i];
        }

        finalOut *= 2.0f; // ゲイン補正

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    return (m_prevSample + (m_lastSample - m_prevSample) * fraction) * m_level;
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
