#pragma once

#include <atomic>

#include <JuceHeader.h>

// ============================================================================
// パラメータを触った履歴 (4.4.1)
// ============================================================================
// 生成過程プレビュー (GuiStageWave) の自動生成が、「触ったか」「最後に触ってから
// どれだけ経ったか」を見るために使う。中身は触った回数と最後に触った時刻だけ。
//
// 音の側 (オートメーション) からも書かれるので、どちらも atomic に持つ。
// 書くのは ParamTouchWatch (エディタが開いているあいだ) と、プレビューの窓の操作。
class ParamTouchLog
{
public:
    // 触った。どのスレッドから呼んでもよい
    void touch()
    {
        m_lastMs.store(juce::Time::getMillisecondCounterHiRes(), std::memory_order_relaxed);
        m_count.fetch_add(1, std::memory_order_release);
    }

    // 触った回数。0 なら一度も触っていない
    juce::uint32 count() const { return m_count.load(std::memory_order_acquire); }

    // 最後に触った時刻 (juce::Time::getMillisecondCounterHiRes)
    double lastMs() const { return m_lastMs.load(std::memory_order_relaxed); }

private:
    std::atomic<juce::uint32> m_count{ 0 };
    std::atomic<double> m_lastMs{ 0.0 };
};

// プラグインの全部のパラメータと Undo の履歴を見張り、変わったら ParamTouchLog へ書く。
// Undo も見るのは、パラメータの外に持つもの (段の表など) の書き換えも拾うため。
// エディタの部品 (GuiGenWave) が持ち、エディタが開いているあいだだけ見張る。
class ParamTouchWatch : private juce::AudioProcessorParameter::Listener, private juce::ChangeListener
{
public:
    ParamTouchWatch(juce::AudioProcessor& processor, juce::UndoManager& undo, ParamTouchLog& log)
        : m_processor(processor), m_undo(undo), m_log(log)
    {
        for (auto* p : m_processor.getParameters()) p->addListener(this);

        m_undo.addChangeListener(this);
    }

    ~ParamTouchWatch() override
    {
        m_undo.removeChangeListener(this);

        for (auto* p : m_processor.getParameters()) p->removeListener(this);
    }

    ParamTouchWatch(const ParamTouchWatch&) = delete;
    ParamTouchWatch& operator=(const ParamTouchWatch&) = delete;

private:
    juce::AudioProcessor& m_processor;
    juce::UndoManager& m_undo;
    ParamTouchLog& m_log;

    void parameterValueChanged(int, float) override { m_log.touch(); }
    void parameterGestureChanged(int, bool) override {}
    void changeListenerCallback(juce::ChangeBroadcaster*) override { m_log.touch(); }
};
