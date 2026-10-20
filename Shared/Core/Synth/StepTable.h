#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>

#include <JuceHeader.h>

#include "./StepLoop.h"

// ============================================================================
// 段の表 (SSG SW AMP / PITCH ENV[256] など)
// ============================================================================
// 段ごとの R (秒) と L (値) を 256 段ぶん持つ。
//
// 256 段 × 2 をチャンネル・オペレーター・パッドごとにパラメータで持つと
// 数万になるため、WT PITCH MOD の変調波形と同じく実データはプロセッサが
// 持ち、state へ書く。パラメータに残すのは STEPS や LOOP などの頭だけで、
// 段の値はオートメーションでは動かさない (画面とファイルで決める)。
//
// 音の側は表のポインタを持ち続けてよい (StepTableStore の要素は動かない)。
// 書き換えるたびに version を進めるので、音の側は版が変わったときだけ
// 写し直す。値は原子的に読み書きするので、どのスレッドから読んでもよい。
//
// 段のループ (Rich Mode / TAP SEQUENCER の LOOP) の START・END・COUNT の組も、
// 同じ理由でここに持つ (StepLoop.h)。組はいつも START・END の昇順に並べておく。
// TAP SEQUENCER の SKIP (その段では補正しない) の印も、段ごとにここへ持つ (4.1.0)。
//
// 文字や state (XML) への出し入れは StepTableIo.h にある。
struct StepTable
{
    static constexpr int size = 256;

    std::array<std::atomic<float>, size> r;
    std::array<std::atomic<float>, size> l;
    std::atomic<std::uint32_t> version{ 1 };

    // いまの段 (0 から。最後に鍵で使った段)。負は開始前で、次に押すと 1 段目を使う。
    // TAP SEQUENCER が鍵を押すたびに進める。保存はしない (読み込みや初期化、
    // DAW の再生を止めたときに開始前へ戻る)。
    //
    // 4.0.0 では「次に使う段」を持っていたので、画面は押す前から 1 段目を指し、
    // 押すと鳴らした段の次を指してずれて見えた。
    std::atomic<int> cursor{ -1 };

    // TAP SEQUENCER の SKIP。立っている段では補正しない (音量 1 倍・音程 0 セント)。
    std::array<std::atomic<bool>, size> skips{};

    // ループの組。loops[0..loopNum) を使う。並びは START・END の昇順。
    std::array<std::atomic<std::uint32_t>, StepLoop::maxEntries> loops{};
    std::atomic<int> loopNum{ 0 };

    // TAP SEQUENCER のループの進み具合。cursor と同じくチャンネルで 1 つ。保存はしない。
    StepLoop::AtomicState tapLoops;

    const float defaultR;
    const float defaultL;

    StepTable(float defR, float defL) : defaultR(defR), defaultL(defL) { reset(); }

    // step は 1〜256
    float rate(int step) const { return r[(size_t)index(step)].load(std::memory_order_relaxed); }
    float level(int step) const { return l[(size_t)index(step)].load(std::memory_order_relaxed); }

    void setRate(int step, float value)
    {
        r[(size_t)index(step)].store(value, std::memory_order_relaxed);
        version.fetch_add(1, std::memory_order_release);
    }

    void setLevel(int step, float value)
    {
        l[(size_t)index(step)].store(value, std::memory_order_relaxed);
        version.fetch_add(1, std::memory_order_release);
    }

    bool skipped(int step) const { return skips[(size_t)index(step)].load(std::memory_order_relaxed); }

    void setSkip(int step, bool value)
    {
        skips[(size_t)index(step)].store(value, std::memory_order_relaxed);
        version.fetch_add(1, std::memory_order_release);
    }

    bool hasSkips() const
    {
        for (const auto& s : skips) if (s.load(std::memory_order_relaxed)) return true;

        return false;
    }

    // TAP SEQUENCER を開始前へ戻す。ループも数え直す
    void clearCursor()
    {
        cursor.store(-1, std::memory_order_relaxed);
        tapLoops.clear();
    }

    int loopCount() const { return std::clamp(loopNum.load(std::memory_order_relaxed), 0, StepLoop::maxEntries); }

    // i は 0 から
    StepLoop::Entry loopAt(int i) const
    {
        return StepLoop::unpack(loops[(size_t)std::clamp(i, 0, StepLoop::maxEntries - 1)].load(std::memory_order_relaxed));
    }

    std::vector<StepLoop::Entry> loopList() const
    {
        std::vector<StepLoop::Entry> list;

        for (int i = 0; i < loopCount(); ++i) list.push_back(loopAt(i));

        return list;
    }

    // 組を丸ごと差し替える。範囲へ収めて並べ直す。256 個より後ろは捨てる
    void setLoops(std::vector<StepLoop::Entry> list)
    {
        for (auto& e : list) e = StepLoop::normalize(e, size);

        StepLoop::sort(list);

        const int n = std::min((int)list.size(), StepLoop::maxEntries);

        for (int i = 0; i < n; ++i) loops[(size_t)i].store(StepLoop::pack(list[(size_t)i]), std::memory_order_relaxed);

        loopNum.store(n, std::memory_order_relaxed);
        tapLoops.clear();
        version.fetch_add(1, std::memory_order_release);
    }

    // 音の側がループの組を引くときの形 (StepLoop::next に渡す)
    struct LoopView
    {
        const StepTable& table;

        StepLoop::Entry operator[](int i) const { return table.loopAt(i); }
    };

    LoopView loopView() const { return { *this }; }

    void reset()
    {
        for (int i = 0; i < size; ++i) {
            r[(size_t)i].store(defaultR, std::memory_order_relaxed);
            l[(size_t)i].store(defaultL, std::memory_order_relaxed);
            skips[(size_t)i].store(false, std::memory_order_relaxed);
        }

        loopNum.store(0, std::memory_order_relaxed);
        clearCursor();
        version.fetch_add(1, std::memory_order_release);
    }

    void copyFrom(const StepTable& other)
    {
        for (int i = 0; i < size; ++i) {
            r[(size_t)i].store(other.r[(size_t)i].load(std::memory_order_relaxed), std::memory_order_relaxed);
            l[(size_t)i].store(other.l[(size_t)i].load(std::memory_order_relaxed), std::memory_order_relaxed);
            skips[(size_t)i].store(other.skips[(size_t)i].load(std::memory_order_relaxed), std::memory_order_relaxed);
        }

        for (int i = 0; i < StepLoop::maxEntries; ++i) loops[(size_t)i].store(other.loops[(size_t)i].load(std::memory_order_relaxed), std::memory_order_relaxed);

        loopNum.store(other.loopNum.load(std::memory_order_relaxed), std::memory_order_relaxed);
        tapLoops.clear();

        version.fetch_add(1, std::memory_order_release);
    }

private:
    static int index(int step) { return std::clamp(step - 1, 0, size - 1); }
};

// プラグインの実体ごとに 1 つ。キーは APVTS のパラメータ ID と同じ形
// (OPNA_OP0_SSGSW256 など)。
//
// 表はプロセッサを組み立てるとき (音を出し始める前) に get で作る。
// 作ったあとは消さない。音の側へ配ったポインタが動かないようにするため。
class StepTableStore
{
public:
    // 無ければ作る。メッセージスレッドから、音を出し始める前に呼ぶこと。
    StepTable& get(const juce::String& key, float defaultR, float defaultL)
    {
        auto it = tables.find(key);

        if (it == tables.end()) it = tables.emplace(key, std::make_unique<StepTable>(defaultR, defaultL)).first;

        return *it->second;
    }

    StepTable* find(const juce::String& key) const
    {
        auto it = tables.find(key);

        return it == tables.end() ? nullptr : it->second.get();
    }

    void forEach(const std::function<void(const juce::String&, StepTable&)>& fn) const
    {
        for (const auto& [key, table] : tables) fn(key, *table);
    }

    void resetAll()
    {
        for (auto& [key, table] : tables) table->reset();
    }

    // TAP SEQUENCER をすべて開始前へ戻す。音のスレッドから呼んでよい
    void clearCursors()
    {
        for (auto& [key, table] : tables) table->clearCursor();
    }

    // DAW の再生を止めたら、TAP SEQUENCER をすべて開始前へ戻す (4.1.0)。
    // 曲の頭から再生し直すたびに、同じ並びで鳴るようにする。
    // processBlock の頭 (鍵を見る前) で、音のスレッドから呼ぶ。
    //
    // head は juce::AudioPlayHead。型を決め打ちしないのは、この表を音のモジュール
    // (juce_audio_basics) を組まないテストでも使うため。
    template <typename PlayHead>
    void followTransport(PlayHead* head)
    {
        bool playing = false;

        if (head != nullptr) {
            if (const auto position = head->getPosition()) playing = position->getIsPlaying();
        }

        if (wasPlaying && !playing) clearCursors();

        wasPlaying = playing;
    }

private:
    std::map<juce::String, std::unique_ptr<StepTable>> tables;

    // 前のブロックで DAW が再生していたか (followTransport)
    bool wasPlaying = false;
};
