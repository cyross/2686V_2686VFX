#include "./SynthOpzx7.h"

#include "../../Core/Synth/SynthHelpers.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"

// ============================================================================
// マトリクスを簡単に構築するためのヘルパー関数 (6オペ完全対応・拡張フィードバック)
// ============================================================================
Opzx7Core::AlgRouting makeAlgOpzx7(
    std::initializer_list<int> carriers,
    std::initializer_list<std::pair<int, int>> mods,
    std::initializer_list<std::pair<int, int>> fbMods) // NEW: フィードバックの接続リスト
{
    Opzx7Core::AlgRouting r;

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
const std::array<Opzx7Core::AlgRouting, Opzx7PrValue::algorithms> Opzx7Core::routings = { {
        makeAlgOpzx7({3}, {{0, 1}, {1, 2}, {2, 3}}, {{0, 0}}), // 00
        makeAlgOpzx7({3}, {{0, 1}, {1, 2}, {2, 3}}, {{1, 0}}), // 01
        makeAlgOpzx7({3}, {{0, 2}, {1, 2}, {2, 3}}, {{0, 0}}), // 02
        makeAlgOpzx7({3}, {{0, 3}, {1, 2}, {2, 3}}, {{0, 0}}), // 03
        makeAlgOpzx7({3}, {{0, 1}, {1, 3}, {2, 3}}, {{0, 0}}), // 04
        makeAlgOpzx7({3}, {{0, 1}, {1, 3}, {2, 3}}, {{1, 0}}), // 05
        makeAlgOpzx7({1, 3}, {{0, 1}, {2, 3}}, {{0, 0}}), // 06
        makeAlgOpzx7({1, 3}, {{0, 1}, {2, 3}}, {{1, 0}}), // 07
        makeAlgOpzx7({0, 3}, {{1, 2}, {2, 3}}, {{0, 0}}), // 08
        makeAlgOpzx7({0, 3}, {{1, 3}, {2, 3}}, {{0, 0}}), // 09
        makeAlgOpzx7({1, 2, 3}, {{0, 1}}, {{0, 0}}), // 10
        makeAlgOpzx7({1, 2, 3}, {{0, 1}}, {{1, 0}}), // 11
        makeAlgOpzx7({1, 2, 3}, {{0, 1}, {0, 2}, {0, 3}}, {{0, 0}}), // 12
        makeAlgOpzx7({0, 2, 3}, {{1, 2}}, {{0, 0}}), // 13
        makeAlgOpzx7({0, 1, 3}, {{0, 1}, { 2, 3 }}, {{0, 0}}), // 14
        makeAlgOpzx7({0, 1, 2, 3}, {}, {{0, 0}}), // 15
        makeAlgOpzx7({2}, {{0,1}, {1,2}}, {{0, 0}}), // 16
        makeAlgOpzx7({2}, {{0,1}, {1,2}}, {{1, 0}}), // 17
        makeAlgOpzx7({2}, {{0,2}, {1,2}}, {{0, 0}}), // 18
        makeAlgOpzx7({0, 2}, {{1,2}}, {{0, 0}}), // 19
        makeAlgOpzx7({1, 2}, {{0,1}}, {{0, 0}}), // 20
        makeAlgOpzx7({1, 2}, {{0,1}}, {{1, 0}}), // 21
        makeAlgOpzx7({0, 1, 2}, {}, {{0, 0}}), // 22
        makeAlgOpzx7({0, 1, 2}, {{0, 1}}, {{0, 0}}), // 23
        makeAlgOpzx7({1}, {{0, 1}}, {{0, 0}}), // 24
        makeAlgOpzx7({1}, {{0, 1}}, {{1, 0}}), // 25
        makeAlgOpzx7({0, 1}, {}, {{0, 0}}), // 26
        makeAlgOpzx7({0, 1}, {{0, 1}}, {{0, 0}}), // 27
        makeAlgOpzx7({1}, {{0, 1}}, {{0, 0}}), // 28
        makeAlgOpzx7({0, 1}, {}, {{0, 0}}), // 29
        makeAlgOpzx7({0, 1, 2, 3}, {}, {{0, 0}, {2, 2}}), // 30
        makeAlgOpzx7({3}, {{0, 3}, {1, 2}, {2, 3}}, {{0, 0}}), // 31
        makeAlgOpzx7({3}, {{0, 1}, {1, 2}, {2, 3}}, {{0, 0}}), // 32
        makeAlgOpzx7({1, 3}, {{0, 1}, {2, 3}}, {{0, 0}, {2, 2}}), // 33
        makeAlgOpzx7({0, 3}, {{1, 2}, {2, 3}}, {{0, 0}}), // 34
        makeAlgOpzx7({0, 2, 3}, {{1, 2}}, {{0, 0}}), // 35
        makeAlgOpzx7({3, 5}, {{0, 1}, {1, 2}, {2, 3}, {4, 5}}, {{0, 0}}), // 36
        makeAlgOpzx7({3, 5}, {{0, 1}, {1, 2}, {2, 3}, {4, 5}}, {{4, 4}}), // 37
        makeAlgOpzx7({2, 5}, {{0, 1}, {1, 2}, {3, 4}, {4, 5}}, {{0, 0}}), // 38
        makeAlgOpzx7({2, 5}, {{0, 1}, {1, 2}, {3, 4}, {4, 5}}, {{2, 0}}), // 39
        makeAlgOpzx7({1, 3, 5}, {{0, 1}, {2, 3}, {4, 5}}, {{0, 0}}), // 40
        makeAlgOpzx7({1, 3, 5}, {{0, 1}, {2, 3}, {4, 5}}, {{1, 0}}), // 41
        makeAlgOpzx7({3, 5}, {{0, 1}, {1, 3}, {2, 3}, {4, 5}}, {{0, 0}}), // 42
        makeAlgOpzx7({3, 5}, {{0, 1}, {1, 3}, {2, 3}, {4, 5}}, {{2, 2}}), // 43
        makeAlgOpzx7({3, 5}, {{0, 1}, {1, 3}, {2, 3}, {4, 5}}, {{4, 4}}), // 44
        makeAlgOpzx7({2, 5}, {{0, 2}, {1, 2}, {3, 4}, {4, 5}}, {{3, 3}}), // 45
        makeAlgOpzx7({2, 5}, {{0, 2}, {1, 2}, {3, 4}, {4, 5}}, {{0, 0}}), // 46
        makeAlgOpzx7({3, 5}, {{0, 3}, {1, 3}, {2, 3}, {4, 5}}, {{4, 4}}), // 47
        makeAlgOpzx7({3, 5}, {{0, 3}, {1, 3}, {2, 3}, {4, 5}}, {{0, 0}}), // 48
        makeAlgOpzx7({3, 5}, {{0, 2}, {1, 2}, {2, 3}, {4, 5}}, {{0, 0}}), // 49
        makeAlgOpzx7({3, 5}, {{0, 2}, {1, 2}, {2, 3}, {4, 5}}, {{4, 4}}), // 50
        makeAlgOpzx7({1, 5}, {{0, 1}, {2, 3}, {3, 5}, {4, 5}}, {{0, 0}}), // 51
        makeAlgOpzx7({1, 5}, {{0, 1}, {2, 3}, {3, 5}, {4, 5}}, {{4, 4}}), // 52
        makeAlgOpzx7({2, 5}, {{0, 1}, {1, 2}, {3, 4}, {4, 5}}, {{3, 3}}), // 53
        makeAlgOpzx7({1, 2, 5}, {{0, 1}, {0, 2}, {3, 4}, {4, 5}}, {{0, 0}}), // 54
        makeAlgOpzx7({2, 4, 5}, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {3, 5}}, {{3, 3}}), // 55
        makeAlgOpzx7({1, 2, 4, 5}, {{0, 1}, {0, 2}, {3, 4}, {4, 5}}, {{3, 3}}), // 56
        makeAlgOpzx7({1, 2, 3, 5}, {{0, 1}, {0, 2}, {0, 3}, {4, 5}}, {{0, 0}}), // 57
        makeAlgOpzx7({1, 2, 4, 5}, {{0, 1}, {0, 2}, {3, 4}}, {{0, 0}}), // 58
        makeAlgOpzx7({1, 2, 3, 4, 5}, {{0, 1}, {0, 2}, {0, 3}}, {{0, 0}}), // 59
        makeAlgOpzx7({1, 2, 3, 4, 5}, {{0, 1}, {0, 2}}, {{0, 0}}), // 60
        makeAlgOpzx7({2, 4, 5}, {{0, 2}, {1, 2}, {3, 4}}, {{0, 0}}), // 61
        makeAlgOpzx7({2, 4, 5}, {{0, 2}, {1, 2}, {3, 4}}, {{3, 3}}), // 62
        makeAlgOpzx7({0, 3, 5}, {{1, 2}, {2, 3}, {4, 5}}, {{1, 1}}), // 63
        makeAlgOpzx7({1, 3, 4, 5}, {{0, 1}, {2, 3}}, {{0, 0}}), // 64
        makeAlgOpzx7({0, 3, 4, 5}, {{1, 2}, {2, 3}}, {{1, 1}}), // 65
        makeAlgOpzx7({1, 2, 3, 4, 5}, {{0, 1}}, {{0, 0}}), // 66
        makeAlgOpzx7({0, 1, 2, 3, 4, 5}, {}, {{0, 0}}), // 67
        makeAlgOpzx7({0}, {}, {{0, 0}}), // 68
        makeAlgOpzx7({0, 1}, {}, {{0, 0}, {1, 1}}), // 69
        makeAlgOpzx7({1, 3}, {{0, 1}, {2, 3}}, {{1, 0}, {3, 2}}), // 70
        makeAlgOpzx7({0, 1, 2, 3}, {}, {{0, 0}, {1, 1}, {2, 2}, {3, 3}}), // 71
    } };

void Opzx7Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

    double target = getTargetRate(m_rateIndex);

    // 高速化のためのループアンローリング
    m_operators[0].prepare(1, target);
    m_operators[1].prepare(2, target);
    m_operators[2].prepare(3, target);
    m_operators[3].prepare(4, target);
    m_operators[4].prepare(5, target);
    m_operators[5].prepare(6, target);

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
    m_operators[4].setCurveCore(p_curveCore);
    m_operators[5].setCurveCore(p_curveCore);
}

void Opzx7Core::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void Opzx7Core::setParameters(const SynthParams& params) {
    m_level = params.opzx7.level;

    m_algorithm = params.opzx7.algorithm; // Range: 0-27
    m_algorithmCodeBase = m_algorithm << m_algorithmCodeShift; // x16

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_lfo.setParameters(
        params.opzx7.lfoPmSyncDelay,
        params.opzx7.lfoAmSyncDelay,
        params.opzx7.pmEnable,
        params.opzx7.amEnable,
        params.opzx7.lfoPmFreq,
        params.opzx7.lfoAmFreq,
        params.opzx7.pgLfoWave,
        params.opzx7.egLfoWave,
        params.opzx7.lfoPms,
        params.opzx7.lfoPmd,
        params.opzx7.lfoAms,
        params.opzx7.lfoAmd,
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
        m_operators[4].setSampleRate(target);
        m_operators[5].setSampleRate(target);

        m_lfo.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opzx7.fmBitDepth);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opzx7.op[0], params.opzx7.feedback);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_operators[0].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[0] = params.opzx7.op[0].mask;
    m_operators[1].setParameters(params.opzx7.op[1], params.opzx7.feedback);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opzx7.op[1].mask;
    m_operators[2].setParameters(params.opzx7.op[2], params.opzx7.feedback);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_operators[2].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[2] = params.opzx7.op[2].mask;
    m_operators[3].setParameters(params.opzx7.op[3], params.opzx7.feedback);
    m_operators[3].setMonoMode(m_isMonoMode);
    m_operators[3].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[3] = params.opzx7.op[3].mask;
    m_operators[4].setParameters(params.opzx7.op[4], params.opzx7.feedback);
    m_operators[4].setMonoMode(m_isMonoMode);
    m_operators[4].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[4] = params.opzx7.op[4].mask;
    m_operators[5].setParameters(params.opzx7.op[5], params.opzx7.feedback);
    m_operators[5].setMonoMode(m_isMonoMode);
    m_operators[5].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[5] = params.opzx7.op[5].mask;
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
    m_operators[4].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[5].setUnisonPhaseOffset(phaseOffsetNorm);

    m_operators[0].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[1].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[2].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[3].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[4].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[5].noteOn(finalFreq, gain, noteNum, isLegato);

    if (!isLegato) {
        // 新規ノートオン時に履歴を完全にクリアする
        m_history1.fill(0.0f);
        m_history2.fill(0.0f);
        m_lfoPhase = 0.0;
        m_rateAccumulator = 0.0;
        m_lfo.noteOn();
    }
}

void Opzx7Core::noteOff()
{
    m_operators[0].noteOff();
    m_operators[1].noteOff();
    m_operators[2].noteOff();
    m_operators[3].noteOff();
    m_operators[4].noteOff();
    m_operators[5].noteOff();
}

bool Opzx7Core::isPlaying() const
{
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_operators[2].isPlaying()) return true;
    if (m_operators[3].isPlaying()) return true;
    if (m_operators[4].isPlaying()) return true;
    if (m_operators[5].isPlaying()) return true;

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
    m_operators[4].setPitchBendRatio(ratio);
    m_operators[5].setPitchBendRatio(ratio);
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

        std::array<float, Opzx7PrValue::ops> currentOut = { 0.0f };
        float finalOut = 0.0f;

        int algIndex = std::clamp(m_algorithm, 0, Opzx7PrValue::algorithms - 1);
        const auto& r = routings[algIndex];

        // =================================================================
        // オペレータの評価 (OP1 -> OP6 の正順で計算)
        // =================================================================
        for (int i = 0; i < Opzx7PrValue::ops; ++i) { // 0 から 5 へ
            float modulator = 0.0f;
            float fbModulator = 0.0f;

            // 1. 通常の変調入力 (mod)
            for (int src = 0; src < Opzx7PrValue::ops; ++src) {
                if (r.mod[i][src] > 0.0f) {
                    // src が i より「小さい」なら既に計算済み(currentOut)、
                    // 大きいなら未計算なので1サンプル前の history1 を使う
                    float srcVal = (src < i) ? currentOut[src] : m_history1[src];

                    modulator += srcVal * r.mod[i][src];
                }
            }

            // 2. フィードバック変調入力 (fbMod)
            for (int src = 0; src < Opzx7PrValue::ops; ++src) {
                if (r.fbMod[i][src] > 0.0f) {
                    // フィードバックは常に「過去2サンプルの平均」
                    float averageFb = (m_history1[src] + m_history2[src]) * 0.5f;

                    fbModulator += averageFb * r.fbMod[i][src];
                }
            }

            // 3. オペレータを計算
            m_operators[i].getSample(currentOut[i], modulator, fbModulator, m_lfo, m_modWheel);

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
        for (int i = 0; i < Opzx7PrValue::ops; ++i) {
            finalOut += currentOut[i] * r.out[i];
        }

        finalOut *= 2.0f; // ゲイン補正

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);

    if (fraction > 1.0f) fraction = 1.0f;

    return m_prevSample + (m_lastSample - m_prevSample) * m_level * fraction;
}

void Opzx7Core::setPcmBuffer(int opIndex, std::vector<float>* pcmData)
{
    if (opIndex >= 0 && opIndex < Opzx7PrValue::ops) {
        m_operators[opIndex].setPcmBuffer(pcmData);
    }
}

void Opzx7Core::setWtBuffer(int opIndex, std::vector<float>* wtData)
{
    if (opIndex >= 0 && opIndex < Opzx7PrValue::ops) {
        m_operators[opIndex].setWtBuffer(wtData);
    }
}

void Opzx7Core::setWt2Buffer(int opIndex, std::vector<float>* wtData)
{
    if (opIndex >= 0 && opIndex < Opzx7PrValue::ops) {
        m_operators[opIndex].setWt2Buffer(wtData);
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

void Opzx7Core::clearPcmBuffer(int opIndex) {
    if (opIndex >= 0 && opIndex < Opzx7PrValue::ops) {
        m_operators[opIndex].clearPcmBuffer();
    }
}

void Opzx7Core::clearWtBuffer(int opIndex) {
    if (opIndex >= 0 && opIndex < Opzx7PrValue::ops) {
        m_operators[opIndex].clearWtBuffer();
    }
}

void Opzx7Core::clearWt2Buffer(int opIndex) {
    if (opIndex >= 0 && opIndex < Opzx7PrValue::ops) {
        m_operators[opIndex].clearWt2Buffer();
    }
}
