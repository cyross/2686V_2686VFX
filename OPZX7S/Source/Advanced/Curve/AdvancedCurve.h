#pragma once

#include <JuceHeader.h>

#include <array>
#include <vector>
#include <atomic>
#include <functional>

#include "./AdvancedCurveParams.h"
#include "../../Processor/Curve/ProcessorCurveValues.h"

class CurveCore
{
private:
    // 画面が描画で読む正本。書くのも読むのもメッセージスレッドだけ。
    CurveParams m_params;

    // オーディオスレッドへ渡す枠。3 枚を回す。
    //
    // 以前は m_params 1 つを両方のスレッドで共有していた。画面で制御点を
    // 動かしている最中にオーディオが読むと、書きかけの中身が混ざる。
    // 枠を分けたので、書き手 (メッセージスレッド) と読み手 (オーディオ
    // スレッド) が同じ枠に触ることがない。待ちも確保もない。
    //
    // CurveParams は 1 つ 439KB ある。枠 3 枚を実体に持たせるとスタックに
    // 置けない大きさになる (テストが 1MB のスタックで溢れた) ので、
    // 中身はヒープへ逃がす。確保するのは組み立てのときだけで、
    // オーディオスレッドは添字を引くだけ。
    std::vector<CurveParams> m_slots = std::vector<CurveParams>(3);
    std::atomic<int> m_ready{ 0 };       // 出来上がっている枠
    std::atomic<bool> m_dirty{ false };  // 新しい枠があるか
    int m_writeSlot = 1;                 // 書き手だけが触る
    int m_readSlot = 2;                  // 読み手だけが触る

    // ロジックごとの関数。Logic の値をそのまま添字にする。
    // 以前は std::map で、エンベロープから毎サンプル引かれていた。
    std::array<std::function<float(const CurveParams&, int, int, int, float)>,
               (size_t)CurveParams::Logic::Size> logics;

    float processRawWith(const CurveParams& src, int positionIndex, int targetIndex, int paramIndex, float x) const;

    inline float processWith(const CurveParams& src, int positionIndex, int targetIndex, int paramIndex, float x) const noexcept {
        float safeX = std::clamp(x, 0.0f, 1.0f);
        if (std::isnan(safeX)) return 0.0f;

        // LUT配列を参照せず、直接計算する（最新のCPUではこの方が安全で速い）
        float result = processRawWith(src, positionIndex, targetIndex, paramIndex, safeX);

        if (std::isnan(result) || std::isinf(result)) return safeX; // フェイルセーフ

        return std::clamp(result, 0.0f, 1.0f);
    }

public:
    CurveCore();

    // メッセージスレッドから。正本を書き換えて、オーディオ側へ渡す。
    void setParameters(const CurveParams& params);

    void bakeCurves() {}
    void bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex) {}

    // オーディオスレッドから。ブロックの頭で 1 回だけ呼ぶ。
    // 新しい枠が出ていたら持ち替える。出ていなければ前の枠をそのまま使う。
    inline void acquireForAudio() noexcept {
        if (m_dirty.exchange(false, std::memory_order_acquire)) {
            m_readSlot = m_ready.exchange(m_readSlot, std::memory_order_acq_rel);
        }
    }

    // 画面用。正本を読む。
    float processRaw(int positionIndex, int targetIndex, int paramIndex, float x) const {
        return processRawWith(m_params, positionIndex, targetIndex, paramIndex, x);
    }

    inline float process(int positionIndex, int targetIndex, int paramIndex, float x) const noexcept { // x: 正規化入力値(0.0f ~ 1.0f)
        return processWith(m_params, positionIndex, targetIndex, paramIndex, x);
    }

    // オーディオ用。acquireForAudio で受け取った枠を読む。
    inline float processAudio(int positionIndex, int targetIndex, int paramIndex, float x) const noexcept {
        return processWith(m_slots[(size_t)m_readSlot], positionIndex, targetIndex, paramIndex, x);
    }
};
