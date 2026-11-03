#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>

#include <JuceHeader.h>

#include "./StepLoop.h"
#include "./LivePosition.h"
#include "./StepRate.h"

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
// TAP PARAM SEQUENCER (4.3.0) は、段ごとに (パラメータ, 値) の組をいくつも持つ。
// 組の置き場 (pairs) は、それを使う表だけが持つ (StepTableStore::getWithPairs)。
//
// PARAM ENV[256] (4.3.0) は、指し先 (TARGET) ごとに値 (STL・段の L・LR・ENDL) を別に持つ。
// 値の組 (sets) も、それを使う表だけが持つ (StepTableStore::getWithLevelSets)。R・ループ・
// SKIP は指し先によらず 1 つ。
//
// 段ごとの進め方 (TIME / RATE)・変化量 D・1 刻みの時間 T (4.3.0, StepRate.h) もここに持つ。
// リリースのぶんは段の後ろ (releaseStep) に置く。T SYNC で使うホストのテンポは
// StepTableStore が持ち、表はそれを指す (tempo)。
//
// 文字や state (XML) への出し入れは StepTableIo.h にある。

// TAP PARAM SEQUENCER の組。scope は -1 がチャンネル、0 からがオペレーター (RHYTHM は
// パッド)。index はその scope の指し先の一覧の番号。value はパラメータの 0〜1。
struct StepPair
{
    int scope = -1;
    int index = 0;
    float value = 0.0f;

    bool operator==(const StepPair& o) const { return scope == o.scope && index == o.index && value == o.value; }
};

struct StepTable
{
    static constexpr int size = 256;

    // TAP PARAM SEQUENCER の 1 段に持てる組の数
    static constexpr int maxPairs = 16;

    // PARAM ENV の値の組の並び。[0] が STL、[1〜256] が段の L、そのあとが LR と ENDL
    static constexpr int setStl = 0;
    static constexpr int setLr = size + 1;
    static constexpr int setEndl = size + 2;
    static constexpr int setSlots = size + 3;

    // 値の組の置き場。組ごとに初期値 (指し先のパラメータの初期値) を持つ
    struct LevelSets
    {
        std::vector<std::unique_ptr<std::array<std::atomic<float>, setSlots>>> values;
        std::vector<float> defaults;
    };

    // 組の置き場。1 つの組を 64 bit に詰めて原子的に読み書きする
    //   0〜31 : 値 (float のビット) / 32〜47 : index / 48〜55 : scope + 1
    struct Pairs
    {
        std::array<std::array<std::atomic<std::uint64_t>, maxPairs>, size> items{};
        std::array<std::atomic<int>, size> counts{};

        static std::uint64_t pack(const StepPair& p)
        {
            std::uint32_t bits = 0;

            std::memcpy(&bits, &p.value, sizeof(bits));

            return (std::uint64_t)bits
                | ((std::uint64_t)(std::uint16_t)std::clamp(p.index, 0, 0xffff) << 32)
                | ((std::uint64_t)(std::uint8_t)std::clamp(p.scope + 1, 0, 0xff) << 48);
        }

        static StepPair unpack(std::uint64_t v)
        {
            StepPair p;
            const auto bits = (std::uint32_t)(v & 0xffffffffu);

            std::memcpy(&p.value, &bits, sizeof(bits));

            p.index = (int)((v >> 32) & 0xffff);
            p.scope = (int)((v >> 48) & 0xff) - 1;

            return p;
        }
    };

    // 段ごとの進め方を持つ数。段 (1〜256) のあとに、リリースのぶんが 1 つ続く
    static constexpr int releaseStep = size + 1;
    static constexpr int rateSlots = size + 1;

    std::array<std::atomic<float>, size> r;
    std::array<std::atomic<float>, size> l;
    std::atomic<std::uint32_t> version{ 1 };

    // 段ごとの進め方 (StepRate::Step を分けて持つ)。[0〜255] が段、[256] がリリース
    std::array<std::atomic<bool>, rateSlots> rateModes{};
    std::array<std::atomic<float>, rateSlots> deltas{};
    std::array<std::atomic<float>, rateSlots> tickSecs{};
    std::array<std::atomic<int>, rateSlots> tickNotes{};

    // T SYNC で使うホストのテンポ (StepTableStore が持つ)。nullptr なら StepRate::defaultBpm
    const std::atomic<double>* tempo = nullptr;

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

    // TAP PARAM SEQUENCER の組。使わない表では nullptr
    std::unique_ptr<Pairs> pairs;

    // PARAM ENV の指し先ごとの値の組。使わない表では nullptr
    std::unique_ptr<LevelSets> sets;

    // 鳴っている位置 (グラフの縦線。4.3.0)。音の側は表を const で持つので、
    // ここだけは const の表からも書けるようにしておく
    mutable LivePosition live;

    StepTable(float defR, float defL, bool withPairs = false, const std::vector<float>* setDefaults = nullptr) : defaultR(defR), defaultL(defL)
    {
        if (withPairs) pairs = std::make_unique<Pairs>();

        if (setDefaults != nullptr) {
            sets = std::make_unique<LevelSets>();
            sets->defaults = *setDefaults;

            for (size_t i = 0; i < setDefaults->size(); ++i) sets->values.push_back(std::make_unique<std::array<std::atomic<float>, setSlots>>());
        }

        reset();
    }

    int setCount() const { return sets != nullptr ? (int)sets->values.size() : 0; }

    float setDefault(int set) const
    {
        return set >= 0 && set < setCount() ? sets->defaults[(size_t)set] : 0.0f;
    }

    // set は 0 から、slot は setStl / 1〜256 / setLr / setEndl
    float setValue(int set, int slot) const
    {
        if (set < 0 || set >= setCount()) return 0.0f;

        return (*sets->values[(size_t)set])[(size_t)std::clamp(slot, 0, setSlots - 1)].load(std::memory_order_relaxed);
    }

    void setSetValue(int set, int slot, float value)
    {
        if (set < 0 || set >= setCount()) return;

        (*sets->values[(size_t)set])[(size_t)std::clamp(slot, 0, setSlots - 1)].store(value, std::memory_order_relaxed);
        version.fetch_add(1, std::memory_order_release);
    }

    bool hasPairs() const { return pairs != nullptr; }

    // step は 1〜256
    int pairCount(int step) const
    {
        if (pairs == nullptr) return 0;

        return std::clamp(pairs->counts[(size_t)index(step)].load(std::memory_order_relaxed), 0, maxPairs);
    }

    StepPair pairAt(int step, int i) const
    {
        if (pairs == nullptr) return {};

        return Pairs::unpack(pairs->items[(size_t)index(step)][(size_t)std::clamp(i, 0, maxPairs - 1)].load(std::memory_order_relaxed));
    }

    std::vector<StepPair> pairList(int step) const
    {
        std::vector<StepPair> list;

        for (int i = 0; i < pairCount(step); ++i) list.push_back(pairAt(step, i));

        return list;
    }

    // 段の組を丸ごと差し替える。maxPairs より後ろは捨てる
    void setPairs(int step, const std::vector<StepPair>& list)
    {
        if (pairs == nullptr) return;

        const auto s = (size_t)index(step);
        const int n = std::min((int)list.size(), maxPairs);

        for (int i = 0; i < n; ++i) pairs->items[s][(size_t)i].store(Pairs::pack(list[(size_t)i]), std::memory_order_relaxed);

        pairs->counts[s].store(n, std::memory_order_relaxed);
        version.fetch_add(1, std::memory_order_release);
    }

    bool hasAnyPairs() const
    {
        if (pairs == nullptr) return false;

        for (const auto& c : pairs->counts) if (c.load(std::memory_order_relaxed) > 0) return true;

        return false;
    }

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

    // step は 1〜256、releaseStep (257) がリリース
    StepRate::Step rateStep(int step) const
    {
        const auto i = (size_t)rateIndex(step);

        return {
            rateModes[i].load(std::memory_order_relaxed),
            deltas[i].load(std::memory_order_relaxed),
            tickSecs[i].load(std::memory_order_relaxed),
            tickNotes[i].load(std::memory_order_relaxed)
        };
    }

    void setRateStep(int step, const StepRate::Step& value)
    {
        const auto i = (size_t)rateIndex(step);

        rateModes[i].store(value.rate, std::memory_order_relaxed);
        deltas[i].store(std::max(0.0f, value.delta), std::memory_order_relaxed);
        tickSecs[i].store(std::max(0.0f, value.tickSec), std::memory_order_relaxed);
        tickNotes[i].store(std::clamp(value.tickNote, 0, StepRate::noteCount - 1), std::memory_order_relaxed);
        version.fetch_add(1, std::memory_order_release);
    }

    // 初期値 (TIME・D 0・T 0) と違う段があるか
    bool hasRateSteps() const
    {
        for (int s = 1; s <= releaseStep; ++s) if (rateStep(s) != StepRate::Step{}) return true;

        return false;
    }

    double tempoBpm() const
    {
        const double bpm = tempo != nullptr ? tempo->load(std::memory_order_relaxed) : 0.0;

        return bpm > 0.0 ? bpm : StepRate::defaultBpm;
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

            if (pairs != nullptr) pairs->counts[(size_t)i].store(0, std::memory_order_relaxed);
        }

        for (int set = 0; set < setCount(); ++set) {
            for (auto& v : *sets->values[(size_t)set]) v.store(sets->defaults[(size_t)set], std::memory_order_relaxed);
        }

        resetRateSteps();

        loopNum.store(0, std::memory_order_relaxed);
        clearCursor();
        version.fetch_add(1, std::memory_order_release);
    }

    // 段の進め方をすべて TIME (D 0・T 0) へ戻す。版は進めない (呼ぶ側が進める)
    void resetRateSteps()
    {
        for (int i = 0; i < rateSlots; ++i) {
            rateModes[(size_t)i].store(false, std::memory_order_relaxed);
            deltas[(size_t)i].store(0.0f, std::memory_order_relaxed);
            tickSecs[(size_t)i].store(0.0f, std::memory_order_relaxed);
            tickNotes[(size_t)i].store(0, std::memory_order_relaxed);
        }
    }

    // PARAM ENV の指し先ごとの値を初期値へ戻す
    void resetSets()
    {
        for (int set = 0; set < setCount(); ++set) {
            for (auto& v : *sets->values[(size_t)set]) v.store(sets->defaults[(size_t)set], std::memory_order_relaxed);
        }

        version.fetch_add(1, std::memory_order_release);
    }

private:
    static int index(int step) { return std::clamp(step - 1, 0, size - 1); }
    static int rateIndex(int step) { return std::clamp(step - 1, 0, rateSlots - 1); }
};

// プラグインの実体ごとに 1 つ。キーは APVTS のパラメータ ID と同じ形
// (OPNA_OP0_SSGSW256 など)。
//
// 表はプロセッサを組み立てるとき (音を出し始める前) に get で作る。
// 作ったあとは消さない。音の側へ配ったポインタが動かないようにするため。
// 鳴っている位置の鍵 (StepTableStore::position)。チャンネル・オペレーター・パッドの頭に足す
namespace LiveKey
{
    inline const juce::String ampEnv = "_LIVE_AMPENV";
    inline const juce::String pitchEnv = "_LIVE_PITCHENV";
    inline const juce::String eg = "_LIVE_EG";       // FM 音源のオペレーターの EG
}

class StepTableStore
{
public:
    // 無ければ作る。メッセージスレッドから、音を出し始める前に呼ぶこと。
    StepTable& get(const juce::String& key, float defaultR, float defaultL)
    {
        auto it = tables.find(key);

        if (it == tables.end()) it = tables.emplace(key, std::make_unique<StepTable>(defaultR, defaultL)).first;

        it->second->tempo = &bpm;

        return *it->second;
    }

    // PARAM ENV の、指し先ごとの値の組を持つ表。無ければ作る。defaults は組ごとの初期値
    StepTable& getWithLevelSets(const juce::String& key, float defaultR, const std::vector<float>& defaults)
    {
        auto it = tables.find(key);

        if (it == tables.end()) it = tables.emplace(key, std::make_unique<StepTable>(defaultR, 0.0f, false, &defaults)).first;

        it->second->tempo = &bpm;

        return *it->second;
    }

    // TAP PARAM SEQUENCER の組の置き場を持つ表。無ければ作る
    StepTable& getWithPairs(const juce::String& key)
    {
        auto it = tables.find(key);

        if (it == tables.end()) it = tables.emplace(key, std::make_unique<StepTable>(0.0f, 0.0f, true)).first;

        it->second->tempo = &bpm;

        return *it->second;
    }

    StepTable* find(const juce::String& key) const
    {
        auto it = tables.find(key);

        return it == tables.end() ? nullptr : it->second.get();
    }

    // 段の表を持たないエンベロープ (AMP / PITCH ENV・FM 音源の EG) の、鳴っている位置
    // (4.3.0)。鍵は頭に LiveKey を足したもの。無ければ作る。メッセージスレッドから、
    // 音を出し始める前に呼ぶこと。作ったものは消さない (音の側が指したまま持つ)
    LivePosition& position(const juce::String& key)
    {
        auto it = positions.find(key);

        if (it == positions.end()) it = positions.emplace(key, std::make_unique<LivePosition>()).first;

        return *it->second;
    }

    const LivePosition* findPosition(const juce::String& key) const
    {
        auto it = positions.find(key);

        return it == positions.end() ? nullptr : it->second.get();
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
    //
    // ホストのテンポもここで読む (T SYNC, 4.3.0)。くれないときは前の値のまま
    // (初めから無ければ StepRate::defaultBpm)。
    template <typename PlayHead>
    void followTransport(PlayHead* head)
    {
        bool playing = false;

        if (head != nullptr) {
            if (const auto position = head->getPosition()) {
                playing = position->getIsPlaying();

                if (const auto hostBpm = position->getBpm(); hostBpm && *hostBpm > 0.0) bpm.store(*hostBpm, std::memory_order_relaxed);
            }
        }

        if (wasPlaying && !playing) clearCursors();

        wasPlaying = playing;
    }

    // ホストのテンポ (followTransport が書く)。表は tempo でこれを指す
    std::atomic<double> bpm{ StepRate::defaultBpm };

    double tempoBpm() const { return bpm.load(std::memory_order_relaxed); }

private:
    std::map<juce::String, std::unique_ptr<StepTable>> tables;
    std::map<juce::String, std::unique_ptr<LivePosition>> positions;

    // 前のブロックで DAW が再生していたか (followTransport)
    bool wasPlaying = false;
};
