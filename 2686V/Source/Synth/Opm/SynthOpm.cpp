#include "./SynthOpm.h"

#include "../../Core/Synth/SynthHelpers.h"

// ============================================================================
// マトリクスを簡単に構築するためのヘルパー関数 (全オペ完全対応・拡張フィードバック)
// ============================================================================
OpmCore::AlgRouting makeAlgOpm(
    std::initializer_list<int> carriers,
    std::initializer_list<std::pair<int, int>> mods,
    std::initializer_list<std::pair<int, int>> fbMods) // NEW: フィードバックの接続リスト
{
    OpmCore::AlgRouting r;

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
// アルゴリズムの定義
// ============================================================================
const std::array<OpmCore::AlgRouting, OpmPrValue::algorithms> OpmCore::routings = { {
    makeAlgOpm({3}, {{0, 1}, {1, 2}, {2, 3}}, {{0, 0}}),       // 00
    makeAlgOpm({3}, {{0, 2}, {1, 2}, {2, 3}}, {{0, 0}}),       // 01
    makeAlgOpm({3}, {{0, 1}, {1, 3}, {2, 3}}, {{2, 2}}),       // 02
    makeAlgOpm({3}, {{0, 1}, {1, 3}, {2, 3}}, {{0, 0}}),       // 03
    makeAlgOpm({1, 3}, {{0, 1}, {2, 3}}, {{0, 0}}),            // 04
    makeAlgOpm({1, 2, 3}, {{0, 1}, {0, 2}, {0, 3}}, {{0, 0}}), // 05
    makeAlgOpm({1, 2, 3}, {{0, 1}}, {{0, 0}}),                 // 06
    makeAlgOpm({0, 1, 2, 3}, {}, {{0, 0}}),                    // 07
} };

void OpmCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    double target = getTargetRate(m_rateIndex);

    // 高速化のためのループアンローリング
    m_operators[0].prepare(1, target);
    m_operators[1].prepare(2, target);
    m_operators[2].prepare(3, target);
    m_operators[3].prepare(4, target);

    m_rateAccumulator = 1.0;

	m_noiseGen.prepare(target);
    m_lfo.prepare(target);
    m_ssgSwEnv11g.prepare(0, target);
    m_ssgSwPEnv11g.prepare(0, target);

    // オペレータにチップ全体のピッチ倍率の在りかを教える
    for (int i = 0; i < OpmPrValue::ops; ++i) {
        m_operators[i].setGlobalPitchRatioSource(&m_globalPitchRatio);
    }
    m_ampEnvG.prepare(target);
    m_ssgHwEnv.prepare(target);
    m_ssgHwPEnv.prepare(target);
}

void OpmCore::setCurveCore(CurveCore* p_curveCore)
{
    // 高速化のためのループアンローリング
    m_operators[0].setCurveCore(p_curveCore);
    m_operators[1].setCurveCore(p_curveCore);
    m_operators[2].setCurveCore(p_curveCore);
    m_operators[3].setCurveCore(p_curveCore);

    m_ampEnvG.setCurveCore(p_curveCore);
}

void OpmCore::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void OpmCore::setParameters(const SynthParams& params) {
    m_level = params.opm.level;

    m_algorithm = params.opm.algFb.algorithm;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_lfo.setParameters(params.opm.glLfo);
    m_ssgSwEnv11g.setParameters(params.opm.ssgSwEnv11g);
    m_ssgSwPEnv11g.setParameters(params.opm.ssgSwPEnv11g);
    m_ampEnvG.setParameters(params.opm.ampEnvG);
    m_wtMod.setParameters(params.opm.wtMod);
    m_wtAmpMod.setParameters(params.opm.wtAmpMod);
    m_ssgHwEnv.setParameters(params.opm.ssgHwEnv);
    m_ssgHwPEnv.setParameters(params.opm.ssgHwPEnv);

    m_pan = params.opm.pan;

    if (m_pan == 0) {
        m_pan_l_rate = 1.0f;
        m_pan_r_rate = 1.0f;
    }
    else {
        m_pan_l_rate = (float)((1 - m_pan) >> 1);
        m_pan_r_rate = (float)((m_pan + 1) >> 1);
    }

    if (m_rateIndex != params.opm.quality.rate) {
        m_rateIndex = params.opm.quality.rate;

		double target = getTargetRate(m_rateIndex);

        // 高速化のためのループアンローリング
        m_operators[0].setSampleRate(target);
        m_operators[1].setSampleRate(target);
        m_operators[2].setSampleRate(target);
        m_operators[3].setSampleRate(target);

        m_noiseGen.updateDelta(target);
        m_lfo.updateTargetSampleRate(target);
        m_ssgSwEnv11g.updateTargetSampleRate(target);
        m_ssgSwPEnv11g.updateTargetSampleRate(target);
        m_ampEnvG.updateTargetSampleRate(target);
        m_ssgHwEnv.updateTargetSampleRate(target);
        m_ssgHwPEnv.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opm.quality.bit);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opm.op[0], m_algorithm != 2 ? params.opm.algFb.feedback : 0.0f);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_operators[0].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[0] = params.opm.op[0].mask;
    m_operators[1].setParameters(params.opm.op[1], 0.0f);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opm.op[1].mask;
    m_operators[2].setParameters(params.opm.op[2], m_algorithm == 2 ? params.opm.algFb.feedback : 0.0f);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_operators[2].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[2] = params.opm.op[2].mask;
    m_operators[3].setParameters(params.opm.op[3], 0.0f);
    m_operators[3].setMonoMode(m_isMonoMode);
    m_operators[3].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[3] = params.opm.op[3].mask;

    // アルゴリズムに基づくルーティングのキャッシュを更新
    updateRoutingCache();
}

void OpmCore::noteOn(float freq, float velocity, int midiNote, bool isLegato) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    float gain = std::max(0.01f, velocity * 0.25f);

    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = m_unison.applyDetune(freq);

    m_noteFreq = finalFreq;
    const float phaseOffsetNorm = m_unison.getPhaseOffset();

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

    m_lfo.noteOn();
    m_ssgHwEnv.noteOn();
    m_ssgHwPEnv.noteOn();

    if (!isLegato) {
        m_wtMod.reset();
        m_wtAmpMod.reset();

        if (!m_ampEnvG.isBypass()) {
            m_ampEnvGLevel = m_ampEnvG.noteOn();
        }

        if (!m_ssgSwEnv11g.isBypass()) {
            m_ssgSwEnv11g.noteOn();
        }

        if (!m_ssgSwPEnv11g.isBypass()) {
            m_ssgSwPEnv11g.noteOn();
        }
    }
}

void OpmCore::noteOff() {
    m_operators[0].noteOff();
    m_operators[1].noteOff();
    m_operators[2].noteOff();
    m_operators[3].noteOff();

    if (!m_ampEnvG.isBypass()) {
        m_ampEnvG.noteOff();
    }

    if (!m_ssgSwEnv11g.isBypass()) {
        m_ssgSwEnv11g.noteOff();
    }

    if (!m_ssgSwPEnv11g.isBypass()) {
        m_ssgSwPEnv11g.noteOff();
    }
}

bool OpmCore::isPlaying() const {
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_operators[2].isPlaying()) return true;
    if (m_operators[3].isPlaying()) return true;
    if (m_ampEnvG.isPlaying()) return true;
    if (m_ssgSwEnv11g.isPlaying()) return true;
    if (m_ssgSwPEnv11g.isPlaying()) return true;

    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void OpmCore::setPitchBend(int pitchWheelValue)
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
void OpmCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;

    m_wtMod.setModWheel((float)wheelValue / 127.0f);
    m_wtAmpMod.setModWheel((float)wheelValue / 127.0f);
}

float OpmCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    // MODULATION の速度は搬送波に対する比なので、ノートの位相増分を渡す
    float notePhaseDelta = (float)(m_noteFreq / targetRate);
    double stepSize = targetRate / m_hostSampleRate;

    m_rateAccumulator += stepSize;

    float currentOut[OpmPrValue::ops];

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        // オペレータより先にチップ全体のピッチ倍率を確定させる
        updateGlobalPitchRatio(notePhaseDelta);

        m_lfo.getSample();

        currentOut[0] = 0.0f;
        currentOut[1] = 0.0f;
        currentOut[2] = 0.0f;
        currentOut[3] = 0.0f;

        float finalOut = 0.0f;

        // =================================================================
        // オペレータの評価 (OP1からの正順で計算)
        // テンプレートを用いたループ展開 (Loop Unrolling) によりさらに高速化
        // =================================================================
        processAllOperators(std::make_index_sequence<OpmPrValue::ops>{}, currentOut, finalOut);

        // =================================================================
        // 履歴 (History) のシフト
        // =================================================================
        m_history2 = m_history1;

        // 生配列から std::array へのコピー
        m_history1[0] = currentOut[0];
        m_history1[1] = currentOut[1];
        m_history1[2] = currentOut[2];
        m_history1[3] = currentOut[3];

        // SSGハードウェアエンベロープ(SsgHwEnv)処理
        finalOut *= m_ssgHwEnv.process();

        // WT AMP MOD。速さは搬送波との比なので、ノートの位相増分を渡す。
        finalOut *= m_wtAmpMod.process(notePhaseDelta);

        // チップ全体の AMP ENV 処理
        if (!m_ampEnvG.isBypass()) {
            m_ampEnvGLevel = m_ampEnvG.process(m_ampEnvGLevel);
            finalOut *= m_ampEnvGLevel;
        }
        else {
            if (m_ampEnvG.isRelease()) m_ampEnvG.bypassedReleasedProcess();
        }

        // SSGソフトウェアエンベロープ(SsgSwEnv11)処理
        if (!m_ssgSwEnv11g.isBypass()) {
            finalOut *= m_ssgSwEnv11g.process();
        }
        else {
            if (m_ssgSwEnv11g.isRelease()) m_ssgSwEnv11g.bypassedReleasedProcess();
        }

        finalOut *= 2.0f; // ゲイン補正

        // 量子化 (BIT)。Raw のときは quantizeSample が素通しする
        finalOut = quantizeSample(finalOut, m_quantizeSteps);

        m_lastSample = finalOut;
    }

    // m_rateAccumulator は直近に生成したサンプルからの進み具合を
    // ソースサンプル単位 (0.0〜1.0) で保持しているので、そのまま補間係数になる。
    // prev→last を補間する形なので、出力はソース 1 サンプル分だけ遅れる。
    float fraction = (float)m_rateAccumulator;

    return (m_prevSample + (m_lastSample - m_prevSample) * fraction) * m_level;
}

void OpmCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = m_pan_l_rate;
    float basePanR = m_pan_r_rate;

    m_unison.applyPan(basePanL, basePanR);
    sample *= m_unison.getGainComp();

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}

void OpmCore::updateRoutingCache()
{
    if (m_algorithm == m_cachedAlgorithm) return;

    m_cachedAlgorithm = m_algorithm;
    int algIndex = std::clamp(m_algorithm, 0, OpmPrValue::algorithms - 1);
    const auto& r = routings[algIndex];

    for (int i = 0; i < OpmPrValue::ops; ++i) {
        m_activeRoutings[i].modCount = 0;
        m_activeRoutings[i].fbModCount = 0;
        m_activeRoutings[i].outLevel = r.out[i];

        // 通常変調の登録
        for (int src = 0; src < OpmPrValue::ops; ++src) {
            if (r.mod[i][src] > 0.0f) {
                auto& conn = m_activeRoutings[i].mods[m_activeRoutings[i].modCount++];
                conn.srcOp = src;
                conn.amount = r.mod[i][src];
                conn.isForward = (src < i);
            }
        }

        // フィードバック変調の登録
        for (int src = 0; src < OpmPrValue::ops; ++src) {
            if (r.fbMod[i][src] > 0.0f) {
                auto& conn = m_activeRoutings[i].fbMods[m_activeRoutings[i].fbModCount++];
                conn.srcOp = src;
                conn.amount = r.fbMod[i][src];
                conn.isForward = false;
            }
        }
    }
}

template<size_t I>
inline void OpmCore::processSingleOperator(float* currentOut, float& finalOut)
{
    float modulator = 0.0f;
    float fbModulator = 0.0f;
    const auto& routing = m_activeRoutings[I];

    // 1. 通常の変調入力 (接続されているもの"だけ"を処理)
    for (int m = 0; m < routing.modCount; ++m) {
        const auto& conn = routing.mods[m];
        float srcVal = conn.isForward ? currentOut[conn.srcOp] : m_history1[conn.srcOp];
        modulator += srcVal * conn.amount;
    }

    // 2. フィードバック変調入力
    for (int f = 0; f < routing.fbModCount; ++f) {
        const auto& conn = routing.fbMods[f];
        float averageFb = m_operators[conn.srcOp].getFeedbackAverage();
        fbModulator += averageFb * conn.amount;
    }

    // 3. オペレータを計算
    m_operators[I].getSample(currentOut[I], modulator, fbModulator, m_lfo, m_modWheel);

    if (m_opMask[I]) currentOut[I] = 0.0f;

    // 4. そのまま FinalOutput へ加算
    finalOut += currentOut[I] * routing.outLevel;
}
