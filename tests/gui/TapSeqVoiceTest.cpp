// ============================================================================
// TAP AMP / PITCH SEQUENCER は、打鍵 1 回につき段を 1 つだけ進める
// ============================================================================
// 4.0.0 では、段をボイスの startNote で取り出していた。そのため
//
//   ユニゾン      : 声ごとに startNote が走り、声の数だけ段が進んだ。声ごとに
//                   違う段の値で鳴り、画面の「次に使う段」ともずれた
//   モノ          : 鳴り残っているボイスはコアを直に叩く (startNote を通らない)
//                   ので、レガートや、離してすぐ押し直したときに段が進まなかった
//
// どれもビルドもテストも素通りし、鳴らして初めて分かった。
//
// 4.1.0 から、cursor は「いまの段」(最後に鍵で使った段。負は開始前) を持つ。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Synth/StepTable.h"

#include "./GuiTestChips.h"

namespace
{
    constexpr double kRate = 48000.0;
    constexpr int kBlock = 480;

    struct Rig
    {
        std::unique_ptr<AudioPlugin2686V> plugin{ std::make_unique<AudioPlugin2686V>() };
        juce::AudioBuffer<float> buffer{ 2, kBlock };

        Rig()
        {
            plugin->setRateAndBufferSizeDetails(kRate, kBlock);
            plugin->prepareToPlay(kRate, kBlock);
        }

        bool set(const juce::String& id, float value)
        {
            auto* param = plugin->apvts.getParameter(id);

            if (param == nullptr) return false;

            param->setValueNotifyingHost(param->convertTo0to1(value));

            return true;
        }

        void run(juce::MidiBuffer midi = {})
        {
            buffer.clear();
            plugin->processBlock(buffer, midi);
        }

        void send(std::initializer_list<juce::MidiMessage> messages)
        {
            juce::MidiBuffer midi;
            int at = 0;

            for (const auto& m : messages) midi.addEvent(m, at++);

            run(midi);
        }
    };

    juce::MidiMessage on(int note) { return juce::MidiMessage::noteOn(1, note, (juce::uint8)100); }
    juce::MidiMessage off(int note) { return juce::MidiMessage::noteOff(1, note); }

    // TAP AMP / PITCH を 4 段で入れ、鳴らせるチャンネルを選ぶ。選べなければ nullptr
    StepTable* prepare(Rig& rig, int mode, const GuiTestChips::Entry& chip, const char* tap)
    {
        const juce::String prefix(chip.prefix);

        REQUIRE(rig.set("MODE", (float)mode));

        if (!rig.set(prefix + tap + "_ENABLE", 1.0f)) return nullptr;

        REQUIRE(rig.set(prefix + tap + "_STEPS", 4.0f));

        auto* table = rig.plugin->stepTables.find(prefix + tap);

        REQUIRE(table != nullptr);

        tapSeqRewind(*table);

        // パラメータを音の側へ届ける
        rig.run();

        return table;
    }
}

TEST_CASE("TAP SEQUENCER: 打鍵 1 回につき段を 1 つだけ進める (ユニゾン・モノ)")
{
    int checked = 0;

    for (int m = 0; m < (int)OscMode::Count; ++m)
    {
        const auto* chip = GuiTestChips::byModeName(getModeName((OscMode)m));

        if (chip == nullptr) continue;

        const juce::String prefix(chip->prefix);

        for (const char* tap : { "_TAPAMP", "_TAPPITCH" })
        {
            CAPTURE(chip->modeName);
            CAPTURE(tap);

            // ポリフォニックのユニゾン 2 声。声ごとに進んではいけない
            {
                Rig rig;
                auto* table = prepare(rig, m, *chip, tap);

                if (table == nullptr) continue;

                REQUIRE(rig.set("MONO_MODE", 0.0f));

                const bool hasUnison = rig.set(prefix + "_UNI_VOICES", 2.0f);

                rig.run();

                CHECK(table->cursor.load() < 0);

                rig.send({ on(60) });
                CHECK(table->cursor.load() == 0);

                rig.send({ off(60) });
                rig.send({ on(62) });
                CHECK(table->cursor.load() == 1);

                if (!hasUnison) MESSAGE("UNISON が無いチャンネル");
            }

            // モノ。レガートと、離してすぐ押し直した (鳴り残っている) とき
            {
                Rig rig;
                auto* table = prepare(rig, m, *chip, tap);

                REQUIRE(table != nullptr);
                REQUIRE(rig.set("MONO_MODE", 1.0f));

                rig.run();

                rig.send({ on(60) });
                CHECK(table->cursor.load() == 0);

                // レガート (前の鍵を押したまま)
                rig.send({ on(62) });
                CHECK(table->cursor.load() == 1);

                // 離して、同じブロックのうちに押し直す。ボイスはまだリリースで鳴っている。
                // 60 を離したときは 62 を押したままなので、62 へ戻るだけで段は進まない
                rig.send({ off(60), off(62), on(64) });
                CHECK(table->cursor.load() == 2);

                rig.send({ off(64), on(65) });
                CHECK(table->cursor.load() == 3);

                // 4 段なので 1 段目へ戻る
                rig.send({ off(65), on(67) });
                CHECK(table->cursor.load() == 0);
            }

            ++checked;
        }
    }

    CHECK(checked > 0);
}
