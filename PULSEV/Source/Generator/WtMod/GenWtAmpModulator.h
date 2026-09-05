#pragma once

#include <JuceHeader.h>
#include <array>
#include <cmath>

#include "../../Core/Synth/CommonParams.h"
#include "../Fds/GenFdsModTable.h"

// ============================================================================
// WT AMP MOD
// ============================================================================
// WT PITCH MOD (GenWtModulator.h) と同じ変調波形を、周波数ではなく音量へ
// 当てるもの。実機の波形メモリ音源は変調をピッチへ掛けるので、こちらは
// このプラグインの追加になる。
//
// 返すのは出力へ掛ける倍率で、MIN〜MAX の間を動く。
// DEPTH は「MAX からどれだけ下げるか」の割合。0.0 なら MAX のまま動かず、
// 1.0 で MIN〜MAX の全域を使う。WT PITCH MOD の DEPTH が 0.0 のときに
// 音程が動かないのと同じ考え方で揃えてある。
//
// 変調波形の系統は 3 つとも受け付ける。ピッチ側では周波数分周器の扱いが
// 系統ごとに違ったが、音量側では「-1.0〜1.0 の波」として同じに扱える。
//   FDS (2C33)      : 32 段の階段波 (作り置き 4 種とユーザー編集)
//   WonderSwan ch3  : 1 周でひと続きに動くスイープ
//   HuC6280         : 波形メモリ 32 サンプル
class WtAmpModulator {
    bool m_modEnable = false;
    float m_modDepth = 0.0f;
    float m_modSpeed = 1.0f;
    int m_modShape = 0;

    // 出力の下端と上端
    float m_min = 0.0f;
    float m_max = 1.0f;

    // モジュレーションホイールぶんの上乗せ (0.0〜1.0)
    float m_modWheel = 0.0f;

    // HuC6280 モード用の変調波形
    std::array<float, 32> m_modWave = { 0.0f };

    // FdsUser モード用。レジスタ値のテーブルと、それを積算した階段波。
    // 積算はテーブルが変わったときだけ行う。
    std::array<int, 32> m_modFdsTable = { 0 };
    std::array<float, 32> m_modFdsSteps = { 0.0f };

    double m_modPhase = 0.0;
public:
    void setParameters(const WtAmpModParams& params) {
        m_modEnable = params.enable;
        m_modDepth = params.depth;
        m_modSpeed = params.speed;
        m_modShape = params.shape;
        m_min = params.min;
        m_max = params.max;
        m_modWave = params.wave;

        if (m_modFdsTable != params.fdsTable) {
            m_modFdsTable = params.fdsTable;
            m_modFdsSteps = FdsMod::makeSteps(m_modFdsTable);
        }
    }

    void setModWheel(float wheel) { m_modWheel = wheel; }

    void reset() { m_modPhase = 0.0; }

    // 1 サンプルぶん進めて、出力へ掛ける倍率を返す。
    // newPhaseDelta は搬送波の位相増分 (変調速度を搬送波との比で扱うため)。
    float process(float newPhaseDelta) {
        // Min / Max は出力レンジ。大小が逆に設定されても破綻しないよう並べ替える。
        const float lo = std::min(m_min, m_max);
        const float hi = std::max(m_min, m_max);

        if (!m_modEnable && m_modWheel <= 0.0f) return hi;

        float totalModDepth = m_modDepth + (m_modWheel * 0.1f);

        // 変調波。どの系統も -1.0〜1.0 の両振りとして扱う。
        float modLfoVal;

        if (m_modShape == (int)WtModShape::WsSweepUp) {
            // 1 周のあいだ下端から上端へ一続きに上がる
            modLfoVal = (float)(m_modPhase * 2.0 - 1.0);
        }
        else if (m_modShape == (int)WtModShape::WsSweepDown) {
            modLfoVal = (float)(1.0 - m_modPhase * 2.0);
        }
        else if (m_modShape == (int)WtModShape::HuC6280Wave) {
            int index = (int)((float)m_modPhase * 32.0f) & 31;

            modLfoVal = m_modWave[index];
        }
        else if (m_modShape == (int)WtModShape::FdsUser) {
            modLfoVal = FdsMod::valueFromSteps(m_modFdsSteps, (float)m_modPhase);
        }
        else if (m_modShape >= 1) {
            modLfoVal = FdsMod::value(m_modShape - 1, (float)m_modPhase);
        }
        else {
            modLfoVal = std::sin(m_modPhase * 2.0 * juce::MathConstants<float>::pi);
        }

        // 上端から、波の低い側へ向かって下げる。
        // modLfoVal = +1 で MAX、-1 で MIN (Depth が 1.0 のとき)。
        float gain = hi - (hi - lo) * totalModDepth * 0.5f * (1.0f - modLfoVal);

        gain = std::clamp(gain, lo, hi);

        // Mod Speed は搬送波に対する比率。ピッチ側と同じ扱いにしてある。
        m_modPhase += (newPhaseDelta * m_modSpeed);
        while (m_modPhase >= 1.0f) m_modPhase -= 1.0f;

        return gain;
    }
};
