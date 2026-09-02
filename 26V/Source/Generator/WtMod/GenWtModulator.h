#pragma once

#include <JuceHeader.h>
#include <array>
#include <cmath>

#include "../../Core/Synth/CommonParams.h"
#include "../Fds/GenFdsModTable.h"

// ============================================================================
// WT MODULATION
// ============================================================================
// 波形メモリ音源の変調をひとまとめにしたもの。もともと WT / WT2 / WT+ が
// 同じ計算を三重に持っていたうえ、FM 音源のチップ全体にも同じものを掛ける
// ことになったので、計算だけをここへ集めてある。
//
// 返すのは搬送波の周波数に掛ける比。中身は 3 系統に分かれる。
//   FDS (2C33)      : 周波数そのものを動かす
//   WonderSwan ch3  : 周波数分周器を動かす (比は逆数側に効く)
//   HuC6280         : 別チャンネルの波形メモリ 32 サンプルで分周器を動かす
class WtModulator {
    bool m_modEnable = false;
    float m_modDepth = 0.0f;
    float m_modSpeed = 1.0f;
    int m_modShape = 0;

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
    void setParameters(const WtModParams& params) {
        m_modEnable = params.enable;
        m_modDepth = params.depth;
        m_modSpeed = params.speed;
        m_modShape = params.shape;
        m_modWave = params.wave;

        if (m_modFdsTable != params.fdsTable) {
            m_modFdsTable = params.fdsTable;
            m_modFdsSteps = FdsMod::makeSteps(m_modFdsTable);
        }
    }

    void setModWheel(float wheel) { m_modWheel = wheel; }

    void reset() { m_modPhase = 0.0; }

    // 1 サンプルぶん進めて、搬送波の周波数に掛ける比を返す。
    // newPhaseDelta は搬送波の位相増分 (変調速度を搬送波との比で扱うため)。
    float process(float newPhaseDelta) {
        float totalModDepth = m_modDepth + (m_modWheel * 0.1f);

        float modRatio = 1.0f;
        if (m_modEnable || m_modWheel > 0.0f) // Apply if enable OR wheel is up
        {
            if (m_modShape == (int)WtModShape::WsSweepUp
                || m_modShape == (int)WtModShape::WsSweepDown)
            {
                // --- WonderSwan ch3 のハードウェアスイープ ---
                // 実機は 375Hz 刻みで符号付き値を 11bit の周波数分周器へ加算し、
                // 2047 -> 0 でラップする。分周器は周期に比例するので、等間隔に
                // 足していくとピッチ変化は加速し、ラップした瞬間に反対の端へ飛ぶ。
                // Depth 1.0 で 2オクターブぶん振れるようスケールを合わせている。
                float span = 3.0f * totalModDepth;
                float u = (float)m_modPhase;

                modRatio = (m_modShape == (int)WtModShape::WsSweepUp)
                    ? 1.0f / (1.0f - u * (span / (1.0f + span)))
                    : 1.0f / (1.0f + u * span);
            }
            else if (m_modShape == (int)WtModShape::HuC6280Wave)
            {
                // --- PC Engine HuC6280 の LFO ---
                // 実機は「もう 1 本のチャンネルの波形メモリ(32 サンプル)」の値を、
                // 相手チャンネルの周波数レジスタ(=分周器)へ加算する。
                // 分周器は周期に比例するため、FDS と違って比は逆数側に効く。
                int index = (int)((float)m_modPhase * 32.0f) & 31;

                // 分周器が 0 以下や極端な値にならないよう頭打ちにする
                float divider = std::clamp(1.0f + m_modWave[index] * totalModDepth, 0.25f, 4.0f);

                modRatio = 1.0f / divider;
            }
            else
            {
                // --- ファミコンディスクシステム(2C33) の波形変調 ---
                // 実機は変調器の出力 temp(-64〜63) に対して freq * (1 + temp / 64)。
                // こちらは分周器ではなく周波数そのものを動かす。
                // Shape 0 が正弦波、1 以降が FdsMod のテーブル番号 0 以降。
                float modLfoVal;

                if (m_modShape == (int)WtModShape::FdsUser) {
                    modLfoVal = FdsMod::valueFromSteps(m_modFdsSteps, (float)m_modPhase);
                }
                else if (m_modShape >= 1) {
                    modLfoVal = FdsMod::value(m_modShape - 1, (float)m_modPhase);
                }
                else {
                    modLfoVal = std::sin(m_modPhase * 2.0 * juce::MathConstants<float>::pi);
                }

                // 実機も temp = -64 で頭打ちなので、周波数比は負にしない
                modRatio = std::max(0.0f, 1.0f + modLfoVal * totalModDepth);
            }

            // Mod Speed は搬送波に対する比率。実機でも変調周波数は搬送波と
            // 噛み合う値に設定して倍音を作るのが通例なので、比率のまま扱う。
            m_modPhase += (newPhaseDelta * m_modSpeed);
            while (m_modPhase >= 1.0f) m_modPhase -= 1.0f;
        }

        return modRatio;
    }
};
