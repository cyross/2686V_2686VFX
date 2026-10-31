#pragma once

#include <JuceHeader.h>
#include <vector>

#include "Shared/Core/Gui/GuiHost.h"
#include "Shared/Core/Synth/StepTableUndo.h"

// ============================================================================
// 段の表の書き換えを Undo に載せる見張り (4.3.0)
// ============================================================================
// エディタに 1 つ置く。決まった間隔で表の版 (StepTable::version) を見て、変わった
// 表の中身を前に載せた中身と比べ、違えば Undo の 1 手にして DAW へ知らせる。
//
// マウスのボタンを押しているあいだは載せない (グラフを引いている途中を 1 手ずつ
// 載せない)。離したあとに、押していたあいだの書き換えをまとめて 1 手にする。
//
// 区分の貼り付けなどでパラメータも一緒に変わったときは、その操作の手と同じ
// まとまりに入れる (Undo 1 回で両方が戻る)。前に見たときから Undo の手の量が
// 増えていれば、パラメータが変わったものとみなす。
//
// プリセットや DAW の状態を読んだとき (stateLoads が進んだとき) は載せずに、
// 読んだ中身を覚え直すだけにする。
class GuiStepTableUndo : private juce::Timer
{
public:
    GuiStepTableUndo(GuiProcessorHost& h, juce::AudioProcessor& processor) : host(h)
    {
        auto& history = host.stepTableHistory;

        history.notify = [&processor] {
            processor.updateHostDisplay(juce::AudioProcessor::ChangeDetails().withNonParameterStateChanged(true));
        };

        const int loads = host.stateLoads.load(std::memory_order_acquire);

        // 初めて開いたときと、閉じているあいだに読み込んだときは覚え直す
        if (history.stateLoads != loads || history.entries.empty()) {
            history.resync(host.stepTables);
            history.stateLoads = loads;
        }

        history.undoUnits = host.undoManager.getNumberOfUnitsTakenUpByStoredCommands();

        startTimer(pollMs);
    }

    ~GuiStepTableUndo() override
    {
        stopTimer();
    }

private:
    void timerCallback() override
    {
        auto& history = host.stepTableHistory;
        auto& undo = host.undoManager;

        const int loads = host.stateLoads.load(std::memory_order_acquire);

        if (history.stateLoads != loads) {
            history.resync(host.stepTables);
            history.stateLoads = loads;
            history.undoUnits = undo.getNumberOfUnitsTakenUpByStoredCommands();
            return;
        }

        if (juce::ModifierKeys::currentModifiers.isAnyMouseButtonDown()) return;

        std::vector<StepTableUndo::Action::Change> changes;

        host.stepTables.forEach([&](const juce::String& key, StepTable& table) {
            const auto version = table.version.load(std::memory_order_acquire);
            auto& entry = history.entries[key];

            if (entry.version == version) return;

            auto now = StepTableUndo::Snapshot::take(table);

            if (now != entry.snapshot) changes.push_back({ key, &table, entry.snapshot, now });

            entry = { version, std::move(now) };
        });

        const int units = undo.getNumberOfUnitsTakenUpByStoredCommands();

        if (!changes.empty()) {
            // 前に見たときから手が増えていなければ、表だけの操作なので新しいまとまりにする
            if (units == history.undoUnits) undo.beginNewTransaction();

            undo.perform(new StepTableUndo::Action(history, std::move(changes)));

            if (history.notify) history.notify();
        }

        history.undoUnits = undo.getNumberOfUnitsTakenUpByStoredCommands();
    }

    GuiProcessorHost& host;

    // 表を見に行く間隔 (ms)
    static inline constexpr int pollMs = 100;
};
