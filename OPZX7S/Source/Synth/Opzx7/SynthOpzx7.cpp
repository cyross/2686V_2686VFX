#include "./SynthOpzx7.h"

#include "../../Core/Synth/SynthHelpers.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"

// ============================================================================
// マトリクスを簡単に構築するためのヘルパー関数 (全オペ完全対応・拡張フィードバック)
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
// アルゴリズムの定義
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
        makeAlgOpzx7({5}, {{0,1},{1,2},{2,3},{3,4},{4,5}}, {{0,0}}), // 72
        makeAlgOpzx7({5}, {{0,1},{1,2},{2,3},{3,4},{4,5}}, {{1,0}}), // 73
        makeAlgOpzx7({5}, {{0,1},{1,2},{2,3},{3,4},{4,5}}, {{2,0}}), // 74
        makeAlgOpzx7({5}, {{0,1},{1,2},{2,3},{3,4},{4,5}}, {{3,0}}), // 75
        makeAlgOpzx7({5}, {{0,1},{1,2},{2,3},{3,4},{4,5}}, {{4,0}}), // 76
        makeAlgOpzx7({0, 1, 2, 3, 4, 5}, {}, {{0,0}, {1,1}, {2,2}, {3,3}, {4,4}, {5,5} }), // 77
        makeAlgOpzx7({4}, {{0,1},{1,2},{2,3},{3,4}}, { {0,0} }), // 78
        makeAlgOpzx7({4}, {{0,1},{1,2},{2,3},{3,4}}, { {1,0} }), // 79
        makeAlgOpzx7({4}, {{0,1},{1,2},{2,3},{3,4}}, { {2,0} }), // 80
        makeAlgOpzx7({4}, {{0,1},{1,2},{2,3},{3,4}}, { {3,0} }), // 81
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {0,0} }), // 82
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {1,0} }), // 83
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {2,0} }), // 84
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {4,4} }), // 85
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {0,0},{4,4} }), // 86
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {1,0},{4,4} }), // 87
        makeAlgOpzx7({ 3, 4}, {{0,1},{1,2},{2,3}}, { {2,0},{4,4} }), // 88
        makeAlgOpzx7({ 2, 4}, {{0,1},{1,2},{3,4}}, { {0,0} }), // 89
        makeAlgOpzx7({ 2, 4}, {{0,1},{1,2},{3,4}}, { {1,0} }), // 90
        makeAlgOpzx7({ 2, 4}, {{0,1},{1,2},{3,4}}, { {3,3} }), // 91
        makeAlgOpzx7({ 2, 4}, {{0,1},{1,2},{3,4}}, { {0,0},{3,3} }), // 92
        makeAlgOpzx7({ 2, 4}, {{0,1},{1,2},{3,4}}, { {1,0},{3,3} }), // 93
        makeAlgOpzx7({ 2, 4}, {{0,1},{0,3},{1,2},{3,4}}, { {0,0} }), // 94
        makeAlgOpzx7({ 0, 3, 4}, {{1,2},{2,3}}, { {0,0} }), // 95
        makeAlgOpzx7({ 0, 3, 4}, {{1,2},{2,3}}, { {1,1} }), // 96
        makeAlgOpzx7({ 0, 3, 4}, {{1,2},{2,3}}, { {2,1} }), // 97
        makeAlgOpzx7({ 0, 3, 4}, {{1,2},{2,3}}, { {0,0},{1,1} }), // 98
        makeAlgOpzx7({ 0, 3, 4}, {{1,2},{2,3}}, { {0,0},{2,1} }), // 99
        makeAlgOpzx7({ 0, 3, 4}, { {1,2},{2,3} }, { {0,0},{4,4} }), // 100
        makeAlgOpzx7({ 0, 3, 4}, { {1,2},{2,3} }, { {0,0},{1,1},{4,4} }), // 101
        makeAlgOpzx7({ 0, 3, 4 }, { {1,2},{2,3} }, { {0,0},{2,1},{4,4} }), // 102
        makeAlgOpzx7({ 0, 3, 4}, { {1,3},{2,3} }, { {1,1} }), // 103
        makeAlgOpzx7({ 0, 3, 4 }, { {1,3},{2,3} }, { { 1, 1},{ 2, 2} }), // 104
        makeAlgOpzx7({ 1, 3, 4}, { {0,1},{2,3} }, { {0,0} }), // 105
        makeAlgOpzx7({ 1, 3, 4 }, { {0,1},{2,3} }, { {0,0}, {2,2} }), // 106
        makeAlgOpzx7({ 1, 2, 3, 4}, { {0,1} }, { {0,0} }), // 107
        makeAlgOpzx7({ 1, 2, 3, 4}, { {0,1} }, { {1,0} }), // 108
        makeAlgOpzx7({ 1, 2, 3, 4}, { {0,1},{0,2} }, { {0,0} }), // 109
        makeAlgOpzx7({ 1, 2, 3, 4 }, { {0,1},{0,2},{0,3} }, { {0,0} }), // 110
        makeAlgOpzx7({ 0, 1, 2, 3, 4 }, {}, { {0,0} }), // 111
        makeAlgOpzx7({ 0, 1, 2, 3, 4 }, {}, { {0,0}, {1,1}, {2,2}, {3,3}, {4,4} }), // 112
        makeAlgOpzx7({ 0, 1 }, { {0, 1}, {0, 2} }, { {0,0} }), // 113
        makeAlgOpzx7({ 1, 2, 3 }, { {0, 1}, {0, 2} }, { {0,0} }), // 114
        makeAlgOpzx7({ 6 }, { {0,1},{1,2},{2,3},{3,4},{4,5},{5,6} }, { {0,0} }), // 115
        makeAlgOpzx7({ 3, 6 }, { {0,1},{1,2},{2,3},{4,5},{5,6} }, { {0,0} }), // 116
        makeAlgOpzx7({ 3, 6 }, { {0,1},{1,3},{2,3},{4,6},{5,6} }, { {0,0} }), // 117
        makeAlgOpzx7({ 2, 4, 6 }, { {0,1},{1,2},{3,4},{5,6} }, { {0,0} }), // 118
        makeAlgOpzx7({ 2, 4, 6 }, { {0,1},{0,3},{1,2},{3,4},{5,6}}, {{0,0}}), // 119
        makeAlgOpzx7({ 1, 3, 5, 6 }, { {0,1},{2,3},{4,5} }, { {0,0} }), // 120
        makeAlgOpzx7({ 0, 1, 2, 3, 4, 5, 6 }, {}, { {0,0} }), // 121
        makeAlgOpzx7({ 7 }, { {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7} }, { {0,0} }), // 122
        makeAlgOpzx7({ 3, 7 }, { {0,1},{1,2},{2,3},{4,5},{5,6},{6,7} }, { {0,0} }), // 123
        makeAlgOpzx7({ 3, 7 }, { {0,1},{0,2},{2,3},{4,6},{5,6},{6,7} }, { {0,0} }), // 124
        makeAlgOpzx7({ 2, 5, 7 }, { {0,1},{1,2},{3,4},{4,5},{6,7} }, { {0,0} }), // 125
        makeAlgOpzx7({ 2, 5, 7 }, { {0,2},{1,2},{3,5},{4,5},{6,7} }, { {0,0} }), // 126
        makeAlgOpzx7({ 1, 3, 5, 7 }, { {0,1},{2,3},{4,5},{6,7} }, { {0,0} }), // 127
        makeAlgOpzx7({ 1, 3, 5, 6, 7 }, { {0,1},{2,3},{4,5} }, { {0,0} }), // 128
        makeAlgOpzx7({ 0, 1, 2, 3, 4, 5, 6, 7 }, {}, { {0,0} }), // 129
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
    m_operators[6].prepare(7, target);
    m_operators[7].prepare(8, target);

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
    m_operators[6].setCurveCore(p_curveCore);
    m_operators[7].setCurveCore(p_curveCore);
}

void Opzx7Core::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_hostSampleRate = sampleRate;
    }
}

void Opzx7Core::setParameters(const SynthParams& params) {
    m_level = params.opzx7.level;

    m_algorithm = params.opzx7.algFb.algorithm; // Range: 0-27
    m_algorithmCodeBase = m_algorithm << m_algorithmCodeShift; // x16
	m_algMatrix = params.opzx7.algFb.matrix;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_lfo.setParameters(params.opzx7.glLfo);

    m_panpot = params.opzx7.panpot.pan;
    m_panpot_enable = params.opzx7.panpot.enable;

    if (m_panpot_enable) {
        float pan = (float)(m_panpot + 1) / 33.0f;

        m_panpot_l_rate = 1.0f - pan;
        m_panpot_r_rate = pan;
    }
    else {
        m_panpot_l_rate = 1.0f;
        m_panpot_r_rate = 1.0f;
    }

    if (m_rateIndex != params.opzx7.quality.rate) {
        m_rateIndex = params.opzx7.quality.rate;

        double target = getTargetRate(m_rateIndex);

        // 高速化のためのループアンローリング
        m_operators[0].setSampleRate(target);
        m_operators[1].setSampleRate(target);
        m_operators[2].setSampleRate(target);
        m_operators[3].setSampleRate(target);
        m_operators[4].setSampleRate(target);
        m_operators[5].setSampleRate(target);
        m_operators[6].setSampleRate(target);
        m_operators[7].setSampleRate(target);

        m_lfo.updateTargetSampleRate(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.opzx7.quality.bit);

    // 高速化のためのループアンローリング
    m_operators[0].setParameters(params.opzx7.op[0], params.opzx7.algFb.feedback1);
    m_operators[0].setMonoMode(m_isMonoMode);
    m_operators[0].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[0] = params.opzx7.op[0].mask;
    m_operators[1].setParameters(params.opzx7.op[1], params.opzx7.algFb.feedback2);
    m_operators[1].setMonoMode(m_isMonoMode);
    m_operators[1].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[1] = params.opzx7.op[1].mask;
    m_operators[2].setParameters(params.opzx7.op[2], params.opzx7.algFb.feedback3);
    m_operators[2].setMonoMode(m_isMonoMode);
    m_operators[2].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[2] = params.opzx7.op[2].mask;
    m_operators[3].setParameters(params.opzx7.op[3], params.opzx7.algFb.feedback4);
    m_operators[3].setMonoMode(m_isMonoMode);
    m_operators[3].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[3] = params.opzx7.op[3].mask;
    m_operators[4].setParameters(params.opzx7.op[4], params.opzx7.algFb.feedback5);
    m_operators[4].setMonoMode(m_isMonoMode);
    m_operators[4].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[4] = params.opzx7.op[4].mask;
    m_operators[5].setParameters(params.opzx7.op[5], params.opzx7.algFb.feedback6);
    m_operators[5].setMonoMode(m_isMonoMode);
    m_operators[5].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[5] = params.opzx7.op[5].mask;
    m_operators[6].setParameters(params.opzx7.op[6], params.opzx7.algFb.feedback7);
    m_operators[6].setMonoMode(m_isMonoMode);
    m_operators[6].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[6] = params.opzx7.op[6].mask;
    m_operators[7].setParameters(params.opzx7.op[7], params.opzx7.algFb.feedback8);
    m_operators[7].setMonoMode(m_isMonoMode);
    m_operators[7].m_pitchResetOnLegato = params.pitchResetOnLegato;
    m_opMask[7] = params.opzx7.op[7].mask;

    // アルゴリズムに基づくルーティングのキャッシュを更新
    updateRoutingCache();
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
    m_operators[6].setUnisonPhaseOffset(phaseOffsetNorm);
    m_operators[7].setUnisonPhaseOffset(phaseOffsetNorm);

    m_operators[0].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[1].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[2].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[3].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[4].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[5].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[6].noteOn(finalFreq, gain, noteNum, isLegato);
    m_operators[7].noteOn(finalFreq, gain, noteNum, isLegato);

    m_lfoPhase = 0.0;
    m_rateAccumulator = 0.0;
 
    m_lfo.noteOn();
}

void Opzx7Core::noteOff()
{
    m_operators[0].noteOff();
    m_operators[1].noteOff();
    m_operators[2].noteOff();
    m_operators[3].noteOff();
    m_operators[4].noteOff();
    m_operators[5].noteOff();
    m_operators[6].noteOff();
    m_operators[7].noteOff();
}

bool Opzx7Core::isPlaying() const
{
    if (m_operators[0].isPlaying()) return true;
    if (m_operators[1].isPlaying()) return true;
    if (m_operators[2].isPlaying()) return true;
    if (m_operators[3].isPlaying()) return true;
    if (m_operators[4].isPlaying()) return true;
    if (m_operators[5].isPlaying()) return true;
    if (m_operators[6].isPlaying()) return true;
    if (m_operators[7].isPlaying()) return true;

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
    m_operators[6].setPitchBendRatio(ratio);
    m_operators[7].setPitchBendRatio(ratio);
}

void Opzx7Core::setModulationWheel(int wheelValue)
{
    m_modWheel = (float)wheelValue / 127.0f;
}

float Opzx7Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);
    double stepSize = targetRate / m_hostSampleRate;

    m_rateAccumulator += stepSize;

    float currentOut[Opzx7PrValue::ops];

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        m_lfo.getSample();

        currentOut[0] = 0.0f;
        currentOut[1] = 0.0f;
        currentOut[2] = 0.0f;
        currentOut[3] = 0.0f;
        currentOut[4] = 0.0f;
        currentOut[5] = 0.0f;
        currentOut[6] = 0.0f;
        currentOut[7] = 0.0f;

        float finalOut = 0.0f;

        // =================================================================
        // オペレータの評価 (OP1からの正順で計算)
        // テンプレートを用いたループ展開 (Loop Unrolling) によりさらに高速化
        // =================================================================
        processAllOperators(std::make_index_sequence<Opzx7PrValue::ops>{}, currentOut, finalOut);

        // =================================================================
        // 履歴 (History) のシフト
        // =================================================================
        m_history2 = m_history1;

        // 生配列から std::array へのコピー
        m_history1[0] = currentOut[0];
        m_history1[1] = currentOut[1];
        m_history1[2] = currentOut[2];
        m_history1[3] = currentOut[3];
        m_history1[4] = currentOut[4];
        m_history1[5] = currentOut[5];
        m_history1[6] = currentOut[6];
        m_history1[7] = currentOut[7];

        finalOut *= 2.0f; // ゲイン補正

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);

    if (fraction > 1.0f) fraction = 1.0f;

    return (m_prevSample + (m_lastSample - m_prevSample) * fraction) * m_level;
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
void Opzx7Core::updateRoutingCache()
{
    bool modeChanged = (m_algMatrix.mode != m_cachedAlgMode);
    bool algChanged = (m_algorithm != m_cachedAlgorithm);

    if (m_algMatrix.mode == 1) {
        Opzx7Core::AlgRouting customRouting;
        customRouting.out.fill(0.0f);
        for (auto& row : customRouting.mod) row.fill(0.0f);
        for (auto& row : customRouting.fbMod) row.fill(0.0f);

        // キャリアの設定
        for (int i = 0; i < Opzx7PrValue::ops; ++i) {
            if (m_algMatrix.isCarrier[i]) {
                customRouting.out[i] = 1.0f;
            }
        }

        // モジュレータの設定 (順方向)
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            for (int dest = src + 1; dest < Opzx7PrValue::ops; ++dest) {
                if (m_algMatrix.mod[src][dest]) {
                    customRouting.mod[dest][src] = 1.0f;
                }
            }
        }

        // フィードバックの設定 (逆方向・自身)
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            for (int dest = 0; dest <= src; ++dest) {
                if (m_algMatrix.fbMod[src][dest]) {
                    customRouting.fbMod[dest][src] = 1.0f;
                }
            }
        }

        applyRoutingToCache(customRouting);
        m_cachedAlgMode = 1;
    }
    else {
        // マトリックスモードから戻ってきたか、アルゴリズムが変わった場合のみ更新
        if (!modeChanged && !algChanged) return;

        m_cachedAlgMode = 0;
        m_cachedAlgorithm = m_algorithm;

        int algIndex = std::clamp(m_algorithm, 0, Opzx7PrValue::algorithms - 1);
        applyRoutingToCache(routings[algIndex]);
    }
}

void Opzx7Core::applyRoutingToCache(const AlgRouting& r)
{
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        m_activeRoutings[i].modCount = 0;
        m_activeRoutings[i].fbModCount = 0;
        m_activeRoutings[i].outLevel = r.out[i];

        // 順方向
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            if (r.mod[i][src] > 0.0f) {
                auto& conn = m_activeRoutings[i].mods[m_activeRoutings[i].modCount++];
                conn.srcOp = src; conn.amount = r.mod[i][src]; conn.isForward = true;
            }
        }
        // 逆方向(FB)
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            if (r.fbMod[i][src] > 0.0f) {
                auto& conn = m_activeRoutings[i].fbMods[m_activeRoutings[i].fbModCount++];
                conn.srcOp = src; conn.amount = r.fbMod[i][src]; conn.isForward = false;
            }
        }
    }
}

template<size_t I>
inline void Opzx7Core::processSingleOperator(float* currentOut, float& finalOut)
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
