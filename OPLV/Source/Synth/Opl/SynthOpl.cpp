#include "./SynthOpl.h"

#include "../../Core/Synth/SynthHelpers.h"

// ============================================================================
// マトリクスを簡単に構築するためのヘルパー関数 (全オペ完全対応・拡張フィードバック)
// ============================================================================
OplCore::AlgRouting makeAlgOpl(
    std::initializer_list<int> carriers,
    std::initializer_list<std::pair<int, int>> mods,
    std::initializer_list<std::pair<int, int>> fbMods) // NEW: フィードバックの接続リスト
{
    OplCore::AlgRouting r;

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
const std::array<OplCore::AlgRouting, OplPrValue::algorithms> OplCore::routings = { {
    makeAlgOpl({1}, {{0, 1}}, {{0, 0}}), // 00
    makeAlgOpl({0, 1}, {}, {{0, 0}}),    // 01
} };

void OplCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    m_targetRate = getTargetRate(m_rateIndex);
    double target = m_targetRate;

    // 高速化のためのループアンローリング
    m_operators[0].prepare(1, target);
    m_operators[1].prepare(2, target);

    m_rateAccumulator = 1.0;

    m_ssgSwEnv11g.prepare(0, target);
    m_ssgSwPEnv11g.prepare(0, target);

    // オペレータにチップ全体のピッチ倍率の在りかを教える
    for (int i = 0; i < OplPrValue::ops; ++i) {
        m_operators[i].setGlobalPitchRatioSource(&m_globalPitchRatio);
    }
    m_ampEnvG.prepare(target);
    m_ssgHwEnv.prepare(target);
    m_ssgHwPEnv.prepare(target);
}

void OplCore::setCurveCore(CurveCore* p_curveCore)
{
    // 高速化のためのループアンローリング
    m_operators[0].setCurveCore(p_curveCore);
    m_operators[1].setCurveCore(p_curveCore);

    m_ampEnvG.setCurveCore(p_curveCore);
}

void OplCore::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void OplCore::setParameters(const SynthParams& params) {
    m_level = params.opl.level;

    m_algorithm = params.opl.algFb.algorithm; // 0:Serial(FM), 1:Parallel(AM)

    m_ssgSwEnv11g.setParameters(params.opl.ssgSwEnv11g);
    m_ssgSwPEnv11g.setParameters(params.opl.ssgSwPEnv11g);
    m_ampEnvG.setParameters(params.opl.ampEnvG);
    m_wtMod.setParameters(params.opl.wtMod);
    m_wtAmpMod.setParameters(params.opl.wtAmpMod);
    m_ssgHwEnv.setParameters(params.opl.ssgHwEnv);
    m_ssgHwPEnv.setParameters(params.opl.ssgHwPEnv);

    if (m_rateIndex != params.opl.quality.rate) {
        m_rateIndex = params.opl.quality.rate;

		m_targetRate = getTargetRate(m_rateIndex);
		double target = m_targetRate;

        // 高速化のためのループアンローリング
        m_operators[0].setSampleRate(target);
        m_operators[1].setSampleRate(target);

        m_ssgSwEnv11g.updateTargetSampleRate(target);
        m_ssgSwPEnv11g.updateTargetSampleRate(target);
        m_ampEnvG.updateTargetSampleRate(target);
        m_ssgHwEnv.updateTargetSampleRate(target);
        m_ssgHwPEnv.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opl.quality.bit);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opl.op[0], params.opl.algFb.feedback);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_operators[0].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[0] = params.opl.op[0].mask;
    m_operators[1].setParameters(params.opl.op[1], 0.0f);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opl.op[1].mask;

    // アルゴリズムに基づくルーティングのキャッシュを更新
    updateRoutingCache();
}

void OplCore::noteOn(float freq, float velocity, int midiNote, bool isLegato) {
    // ※トランペット系の音が歪む課題に対応した、かなりな力技
    // 通常のvelocityでは1.0に近くなると音が歪むため、0.25倍して十分な余裕を持たせます。最低値は0.01にして完全な無音を防止します。
    float gain = std::max(0.01f, velocity * 0.25f);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));

    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = m_unison.applyDetune(freq);

    m_noteFreq = finalFreq;
    const float phaseOffsetNorm = m_unison.getPhaseOffset();

    // ユニゾン・ハーモニー向けに変更
    m_operators[0].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[0].noteOn(finalFreq, gain, noteNum, isLegato); // 元の freq ではなく、finalFreq を渡す
    m_operators[1].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[1].noteOn(finalFreq, gain, noteNum, isLegato); // 元の freq ではなく、finalFreq を渡す

    m_rateAccumulator = 0.0; // レートの余りもリセット

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

void OplCore::noteOff() {
    m_operators[0].noteOff();
    m_operators[1].noteOff();

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

bool OplCore::isPlaying() const {
    // 高速化のためのループアンローリング
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_ampEnvG.isPlaying()) return true;
    if (m_ssgSwEnv11g.isPlaying()) return true;
    if (m_ssgSwPEnv11g.isPlaying()) return true;

    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void OplCore::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    // 全オペレーターに適用
    // 高速化のためのループアンローリング
    m_operators[0].setPitchBendRatio(ratio);
    m_operators[1].setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void OplCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    float modWheel = (float)wheelValue / 127.0f;

    // 高速化のためのループアンローリング
    m_operators[0].setModWheel(modWheel);
    m_operators[1].setModWheel(modWheel);

    m_wtMod.setModWheel((float)wheelValue / 127.0f);
    m_wtAmpMod.setModWheel((float)wheelValue / 127.0f);
}

float OplCore::getSample() {
    const double targetRate = m_targetRate;

    // MODULATION の速度は搬送波に対する比なので、ノートの位相増分を渡す
    float notePhaseDelta = (float)(m_noteFreq / targetRate);
    double stepSize = targetRate / m_hostSampleRate;

    m_rateAccumulator += stepSize;

    float currentOut[OplPrValue::ops];

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        // オペレータより先にチップ全体のピッチ倍率を確定させる
        updateGlobalPitchRatio(notePhaseDelta);

        m_operators[0].processLfo();
        m_operators[1].processLfo();

        currentOut[0] = 0.0f;
        currentOut[1] = 0.0f;

        float finalOut = 0.0f;

        // =================================================================
        // オペレータの評価 (OP1からの正順で計算)
        // テンプレートを用いたループ展開 (Loop Unrolling) によりさらに高速化
        // =================================================================
        processAllOperators(std::make_index_sequence<OplPrValue::ops>{}, currentOut, finalOut);

        // =================================================================
        // 履歴 (History) のシフト
        // =================================================================

        // 生配列から std::array へのコピー
        m_history1[0] = currentOut[0];
        m_history1[1] = currentOut[1];

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

void OplCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

    m_unison.applyPan(basePanL, basePanR);
    sample *= m_unison.getGainComp();

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}

void OplCore::updateRoutingCache()
{
    if (m_algorithm == m_cachedAlgorithm) return;

    m_cachedAlgorithm = m_algorithm;
    int algIndex = std::clamp(m_algorithm, 0, OplPrValue::algorithms - 1);
    const auto& r = routings[algIndex];

    for (int i = 0; i < OplPrValue::ops; ++i) {
        m_activeRoutings[i].modCount = 0;
        m_activeRoutings[i].fbModCount = 0;
        m_activeRoutings[i].outLevel = r.out[i];

        // 通常変調の登録
        for (int src = 0; src < OplPrValue::ops; ++src) {
            if (r.mod[i][src] > 0.0f) {
                auto& conn = m_activeRoutings[i].mods[m_activeRoutings[i].modCount++];
                conn.srcOp = src;
                conn.amount = r.mod[i][src];
                conn.isForward = (src < i);
            }
        }

        // フィードバック変調の登録
        for (int src = 0; src < OplPrValue::ops; ++src) {
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
inline void OplCore::processSingleOperator(float* currentOut, float& finalOut)
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
    m_operators[I].getSample(currentOut[I], modulator, fbModulator);

    if (m_opMask[I]) currentOut[I] = 0.0f;

    // 4. そのまま FinalOutput へ加算
    finalOut += currentOut[I] * routing.outLevel;
}
