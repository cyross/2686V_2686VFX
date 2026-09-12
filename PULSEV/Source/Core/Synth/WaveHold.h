#pragma once

#include <algorithm>

// ============================================================================
// ホールドと部分再生
// ============================================================================
// 波形を周期で回すものに共通で付く仕組み。実機の SSG が持つ「1 周して
// 止まる」エンベロープ (Saw Down & Hold など) を、どの波形でも使える
// ように広げたもの。
//
//   ホールド    COUNT 周ぶん回したら、そこから先はキーが離れるまで
//               MIN か MAX を保つ。どちらで止まるかは TARGET。
//   部分再生    波形の START〜END だけを鳴らす。区間の外は KEEP START /
//               KEEP END が入っていれば端の値を保ち、切っていれば 0。
//
// どちらも「使う」つまみが切のときは何も変えない。既にあるパッチの鳴りを
// 値から推し量って変えてしまわないよう、明示的に入れてもらう造りにしてある。
//
// 波形の側がすでに止まる振る舞いを持つもの (SSG HW ENV の Saw Down & Hold
// など) は対象外。二重に止めても意味がないので、画面の側で閉じる。
// 保つ値の単位。値の範囲と、画面に出る刻みボタンがこれで決まる。
enum class WaveHoldUnit {
    // 音量へ掛ける倍率 (0.0〜1.0)
    Level = 0,

    // 音程 (-4800〜4800 セント)
    Cent,

    // 両振りの波形そのもの (-1.0〜1.0)。LFO のように、深さを掛ける前の
    // 形を保つものはこれを使う。
    Bipolar,
};

struct WaveHoldParams {
    // --- ホールド ---
    bool holdEnable = false;

    // 保ちに入るまでに回す回数
    int holdCount = 8;

    // 止まる側。0 = MIN / 1 = MAX
    int holdTarget = 1;

    // 保つ値。音量へ掛けるものは倍率 (0.0〜1.0)、音程へ効くものはセント。
    float holdMin = 0.0f;
    float holdMax = 1.0f;

    // --- 部分再生 ---
    bool keepEnable = false;
    float waveStart = 0.0f;
    bool keepStart = false;
    float waveEnd = 1.0f;
    bool keepEnd = false;
};

// 位相の進み具合を見て、止まったかどうかと、波形を引く位相を決める。
// 音を出し始めるときに reset() を呼ぶこと。
class WaveHold {
    WaveHoldParams m_params;

    int m_cycleCount = 0;
    bool m_holding = false;
public:
    void setParameters(const WaveHoldParams& params) { m_params = params; }

    const WaveHoldParams& getParameters() const { return m_params; }

    void reset() {
        m_cycleCount = 0;
        m_holding = false;
    }

    bool isHolding() const { return m_holding; }

    // 止まったときに返す値。単位は呼ぶ側のまま (倍率でもセントでも)。
    float holdValue() const {
        return m_params.holdTarget == 0 ? m_params.holdMin : m_params.holdMax;
    }

    // 1 周するたびに呼ぶ。決めた回数まで回っていたら true を返す。
    bool countCycle() {
        if (!m_params.holdEnable) return false;

        if (m_holding) return true;

        if (++m_cycleCount >= m_params.holdCount) m_holding = true;

        return m_holding;
    }

    // 波形を引く位相 (0.0〜1.0) を区間へ収める。
    // 区間の外で、端の値を保たない側にいるときは true を返す。
    // その場合は波形の値を 0 (変調なし) として扱う。
    bool windowPhase(double& phase) const {
        if (!m_params.keepEnable) return false;

        // End は Start より手前へは行かない
        const float st = std::clamp(m_params.waveStart, 0.0f, 1.0f);
        const float ed = std::max(std::clamp(m_params.waveEnd, 0.0f, 1.0f), st);

        if (phase < st) {
            if (!m_params.keepStart) return true;

            phase = st;
        }
        else if (phase > ed) {
            if (!m_params.keepEnd) return true;

            phase = ed;
        }

        return false;
    }
};
