#pragma once

#include <array>
#include <atomic>
#include <vector>

#include "./GenPcmHelper.h"

// 素材と、それを符号化したもの。
//
// 以前はボイスごとに素材を丸ごと複製していた (80 ボイスなら 80 本) 上に、
// 画面で量子化ビットやレートを動かすたびに、全ボイスがオーディオスレッドの
// 上で素材まるごとを符号化し直していた。中で確保も起きる。数秒の素材を
// 読み込んだ状態でつまみを回すと、それだけで音が途切れる。
//
// 出来上がるものはどのボイスでも同じなので、1 つだけ作って皆で指す。
struct PcmSharedData
{
    std::vector<float> raw;         // 元の素材
    double sourceRate = 44100.0;

    std::vector<int16_t> encoded;   // 符号化したもの
    double encodedRate = 16000.0;

    // encoded を作ったときの指定。今の指定と食い違っていたら作り直す。
    int qualityMode = -1;
    int rateIndex = -1;
};

// 作るのはメッセージスレッド、読むのはオーディオスレッド。
// 枠を 3 枚回すので、両者が同じ枠に触ることがない。待ちも確保もない。
class PcmSharedStore
{
public:
    // ---------------- メッセージスレッドから ----------------

    // 素材を差し替える。読み込み直後に呼ぶ。
    void setSource(const std::vector<float>& src, double rate);

    // 素材を捨てる。
    void clear();

    // 指定が今の中身と違えば符号化し直して差し出す。同じなら何もしない。
    // defaultRate は rateIndex が範囲外だったときに使う値。
    void rebuildIfNeeded(int qualityMode, int rateIndex, double defaultRate);

    // ---------------- オーディオスレッドから ----------------

    // 新しい枠が出ていたら持ち替える。ブロックの頭で 1 回だけ呼ぶ。
    void acquireForAudio() noexcept
    {
        if (m_dirty.exchange(false, std::memory_order_acquire)) {
            m_readSlot = m_ready.exchange(m_readSlot, std::memory_order_acq_rel);
        }
    }

    const PcmSharedData& forAudio() const noexcept { return m_slots[(size_t)m_readSlot]; }

    // いま出ている符号化が指定と合っているか。合っていなければ、呼び出し側が
    // メッセージスレッドへ作り直しを頼む。
    bool needsRebuild(int qualityMode, int rateIndex) const noexcept
    {
        const auto& cur = forAudio();

        if (cur.raw.empty()) return false; // 素材が無ければ作るものもない

        return cur.qualityMode != qualityMode || cur.rateIndex != rateIndex;
    }

private:
    PcmSharedData m_master; // 正本。触るのはメッセージスレッドだけ。

    std::array<PcmSharedData, 3> m_slots;
    std::atomic<int> m_ready{ 0 };      // 出来上がっている枠
    std::atomic<bool> m_dirty{ false }; // 新しい枠があるか
    int m_writeSlot = 1;                // 書き手だけが触る
    int m_readSlot = 2;                 // 読み手だけが触る

    void publish();
};
