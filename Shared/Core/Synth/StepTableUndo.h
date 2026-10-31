#pragma once

#include <JuceHeader.h>

#include <functional>
#include <map>
#include <vector>

#include "./StepTable.h"
#include "./StepTableIo.h"

// ============================================================================
// 段の表の Undo (4.3.0)
// ============================================================================
// 段の表 (StepTable) はパラメータではないので、書き換えても APVTS の Undo には
// 載らず、DAW にも変わったことが伝わらない。PARAM ENV の値 (STL・L・LR・ENDL) を
// TARGET ごとの組に持つと決めた (パラメータへ戻さない) 代わりに、表の書き換えを
// ここで Undo に載せ、DAW へ知らせる。
//
// 表の中身を丸ごと文字 (StepTableIo) にして、前と後を持つ。画面の部品の一つ一つに
// 手を入れずに済むよう、書き換えを拾うのは画面の側の見張り (GuiStepTableUndo) が
// 決まった間隔でする。ここには、プロセッサが持つ「最後に Undo へ載せた中身」と、
// Undo の 1 手を置く。エディタを閉じても Undo の手は残るので、手が指すものは
// どれもプロセッサが持つ (エディタの側を指さない)。
namespace StepTableUndo
{
    // 表の中身。ループの組・SKIP・TAP PARAM SEQUENCER の組・PARAM ENV の値の組まで含む
    struct Snapshot
    {
        juce::String steps;
        juce::String pairs;
        juce::String sets;

        static Snapshot take(const StepTable& table)
        {
            return {
                StepTableIo::toText(table),
                table.hasPairs() ? StepTableIo::pairsToText(table) : juce::String(),
                table.setCount() > 0 ? StepTableIo::setsToText(table) : juce::String()
            };
        }

        void apply(StepTable& table) const
        {
            StepTableIo::fromText(table, steps);

            if (table.hasPairs()) StepTableIo::pairsFromText(table, pairs);

            if (table.setCount() > 0) {
                table.resetSets();
                StepTableIo::setsFromText(table, sets);
            }
        }

        bool operator==(const Snapshot& o) const { return steps == o.steps && pairs == o.pairs && sets == o.sets; }
        bool operator!=(const Snapshot& o) const { return !(*this == o); }

        int size() const { return steps.length() + pairs.length() + sets.length(); }
    };

    // 最後に Undo へ載せた (または読み込んだ) 表の中身。プロセッサに 1 つ
    class History
    {
    public:
        struct Entry
        {
            std::uint32_t version = 0;
            Snapshot snapshot;
        };

        // 表の中身を書き換えたあとに呼ぶ (DAW へ知らせる)。プロセッサが入れる
        std::function<void()> notify;

        // 見張りが最後に見た state の差し替えの回数 (GuiProcessorHost::stateLoads)
        int stateLoads = -1;

        // 見張りが最後に見た、Undo に載った手の量 (UndoManager の units)
        int undoUnits = -1;

        std::map<juce::String, Entry> entries;

        // いまの中身を「載せた中身」として覚え直す (読み込んだとき・初めて見るとき)
        void resync(const StepTableStore& store)
        {
            entries.clear();

            store.forEach([&](const juce::String& key, StepTable& table) {
                entries[key] = { table.version.load(std::memory_order_acquire), Snapshot::take(table) };
            });
        }

        void mark(const juce::String& key, const StepTable& table, const Snapshot& snapshot)
        {
            entries[key] = { table.version.load(std::memory_order_acquire), snapshot };
        }
    };

    // Undo の 1 手。1 度の操作で書き換わった表をまとめて持つ
    class Action : public juce::UndoableAction
    {
    public:
        struct Change
        {
            juce::String key;
            StepTable* table = nullptr;
            Snapshot before;
            Snapshot after;
        };

        Action(History& h, std::vector<Change> c) : history(h), changes(std::move(c)) {}

        bool perform() override
        {
            // 初めの 1 回は、書き換えたあとの中身がもう表に入っている
            if (!performed) {
                performed = true;
                return true;
            }

            for (auto& c : changes) put(c, c.after);

            if (history.notify) history.notify();

            return true;
        }

        bool undo() override
        {
            for (auto& c : changes) put(c, c.before);

            if (history.notify) history.notify();

            return true;
        }

        int getSizeInUnits() override
        {
            int units = 0;

            for (const auto& c : changes) units += c.before.size() + c.after.size();

            return juce::jmax(1, units / 64);
        }

    private:
        void put(Change& c, const Snapshot& s)
        {
            s.apply(*c.table);
            history.mark(c.key, *c.table, s);
        }

        History& history;
        std::vector<Change> changes;
        bool performed = false;
    };
}
