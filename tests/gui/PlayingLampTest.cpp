// ============================================================================
// 再生ランプ
// ============================================================================
// 画面の左下のランプは、音が出ている間と、鍵盤を押している間に点く。
//
// 3.6.0 より前は「ボイスが生きているか」で点けていた。ボイスは包絡が走り
// 終わるまで生き続けるので、リリースの遅い音色では、耳に届かなくなってから
// 何秒も点いたままだった。鍵盤の押し離しも 1 つの札で覚えていたため、和音の
// 1 つを離しただけで倒れ、逆にオールノートオフ (CC123 / CC120) は noteOff を
// 通らないので立ったまま残った。
//
// どれも音を聞かないと分からない類で、ビルドも警告も素通りする。ここで
// 見張る。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Processor/PluginProcessor.h"

#include "./GuiTestChips.h"

namespace
{
    constexpr double kSampleRate = 48000.0;
    constexpr int kBlockSize = 480; // 0.01 秒

    struct LampRig
    {
        std::unique_ptr<AudioPlugin2686V> plugin{ std::make_unique<AudioPlugin2686V>() };
        juce::AudioBuffer<float> buffer{ 2, kBlockSize };

        LampRig()
        {
            // ホストがしていることと同じ順で立ち上げる。これをしないと
            // getSampleRate() が 0 のままになる。
            plugin->setRateAndBufferSizeDetails(kSampleRate, kBlockSize);
            plugin->prepareToPlay(kSampleRate, kBlockSize);
        }

        void run(int blocks, juce::MidiBuffer midi = {})
        {
            for (int i = 0; i < blocks; ++i) {
                buffer.clear();
                plugin->processBlock(buffer, midi);
                midi.clear();
            }
        }

        void set(const juce::String& id, float value)
        {
            auto* param = plugin->apvts.getParameter(id);
            REQUIRE(param != nullptr);
            param->setValueNotifyingHost(param->convertTo0to1(value));
        }

        bool has(const juce::String& id) const { return plugin->apvts.getParameter(id) != nullptr; }

        // ランプが点いているか (画面が見ているものと同じ条件)
        bool lamp() { return plugin->isPlaying() || plugin->isMidiProcessing(); }

        // そのチャンネルのリリースを、どれもいちばん遅くする。
        //
        // 音源によって、ボイスを生かしているのがどの包絡かは違う (FM は
        // オペレータ、ほかはチャンネルの包絡)。名前で拾ってまとめて寝かせる。
        void slowestReleases(const juce::String& prefix)
        {
            for (auto* ap : plugin->getParameters())
            {
                auto* param = dynamic_cast<juce::RangedAudioParameter*>(ap);

                if (param == nullptr) continue;

                const juce::String id = param->getParameterID();

                if (!id.startsWith(prefix + "_") || !id.endsWith("_RR")) continue;

                const auto& range = param->getNormalisableRange();

                // 実機の綴りのもの (0〜15) は 0 がいちばん遅い。
                // 秒で持つものは、大きいほど遅い。
                const float slowest = (range.start == 0.0f && range.end == 15.0f)
                    ? range.start
                    : range.end;

                param->setValueNotifyingHost(range.convertTo0to1(slowest));
            }
        }
    };

    juce::MidiBuffer noteOn(int note)
    {
        juce::MidiBuffer m;
        m.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)100), 0);
        return m;
    }

    juce::MidiBuffer noteOff(int note)
    {
        juce::MidiBuffer m;
        m.addEvent(juce::MidiMessage::noteOff(1, note), 0);
        return m;
    }

    juce::MidiBuffer allNotesOff()
    {
        juce::MidiBuffer m;
        m.addEvent(juce::MidiMessage::allNotesOff(1), 0);
        return m;
    }

    // 音が出るチャンネルを 1 つ見つける。
    //
    // 波形や PCM を読み込まないと鳴らないチャンネル (WT・WT+・PCM 系) は、
    // ここでは選ばない。見つからないプラグイン (2686VFX) では、音に関わる
    // 場面を飛ばす。
    struct Chosen
    {
        bool found = false;
        int mode = 0;
        juce::String prefix;
    };

    Chosen chooseAudibleChannel(LampRig& rig)
    {
        for (int m = 0; m < (int)OscMode::Count; ++m)
        {
            const auto* chip = GuiTestChips::byModeName(getModeName((OscMode)m));

            if (chip == nullptr || chip->isPcm) continue;

            const juce::String prefix(chip->prefix);

            if (!rig.has(prefix + "_LEVEL")) continue;

            LampRig probe;
            probe.set("MODE", (float)m);
            probe.run(1, noteOn(69));
            probe.run(5);

            if (probe.plugin->isPlaying()) return { true, m, prefix };
        }

        return {};
    }
}

TEST_CASE("鍵盤を押している間はランプが点き、離すと消える")
{
    LampRig rig;

    CHECK_FALSE(rig.lamp());

    rig.run(1, noteOn(69));
    rig.run(2);

    CHECK(rig.lamp());

    rig.run(1, noteOff(69));
    rig.run(100); // 1 秒

    CHECK_FALSE(rig.lamp());
}

TEST_CASE("和音の 1 つを離しても、押している鍵盤が残っていればランプは点いたまま")
{
    LampRig rig;

    rig.run(1, noteOn(60));
    rig.run(1, noteOn(64));
    rig.run(2);

    CHECK(rig.lamp());

    rig.run(1, noteOff(64));
    rig.run(2);

    // 60 を押したままなので、まだ点いている
    CHECK(rig.plugin->isMidiProcessing());

    rig.run(1, noteOff(60));
    rig.run(100);

    CHECK_FALSE(rig.lamp());
}

TEST_CASE("オールノートオフでランプが消える")
{
    // CC123 は音源の中で直に処理され、鍵盤を離す処理を通らない。
    // DAW が再生を止めるたびに送ってくるので、これで消えないと点きっぱなしになる。
    LampRig rig;

    rig.run(1, noteOn(69));
    rig.run(2);

    REQUIRE(rig.lamp());

    rig.run(1, allNotesOff());
    rig.run(100);

    CHECK_FALSE(rig.lamp());
}

TEST_CASE("音が出ていなければ、ボイスが残っていてもランプは点かない")
{
    // ボイスは包絡が走り終わるまで生き続ける。リリースを長くした音色では、
    // 耳に届かなくなってからも何秒か残る。その間にランプが点いていると、
    // 音が消えているのに点いたままに見える。
    LampRig rig;

    const auto chosen = chooseAudibleChannel(rig);

    if (!chosen.found) return; // 音を作らないプラグイン (2686VFX)

    INFO("チャンネル: ", chosen.prefix);

    rig.set("MODE", (float)chosen.mode);

    // 音が出ていれば点く、を先に確かめておく
    rig.run(1, noteOn(69));
    rig.run(5);

    REQUIRE(rig.plugin->isPlaying());

    rig.run(1, noteOff(69));
    rig.run(200);

    // ここからが本題。音量を 0 にして音を消し、リリースはどれもいちばん
    // 長くして、ボイスが長く生き残るようにする。
    rig.set(chosen.prefix + "_LEVEL", 0.0f);

    rig.slowestReleases(chosen.prefix);

    rig.run(1, noteOn(69));
    rig.run(5);
    rig.run(1, noteOff(69));
    rig.run(100); // 1 秒。包絡はまだ走っている

    CHECK_FALSE(rig.lamp());
}

TEST_CASE("チャンネルを切り替えるとランプが消える")
{
    LampRig rig;

    const auto chosen = chooseAudibleChannel(rig);

    if (!chosen.found) return;

    rig.set("MODE", (float)chosen.mode);
    rig.run(1, noteOn(69));
    rig.run(5);

    REQUIRE(rig.lamp());

    // 押したまま別のチャンネルへ。切り替えた先では前の音は鳴らないので、
    // 押しっぱなしの扱いは残さない。
    //
    // チャンネルが 1 つしかない本 (OPMV や OPZX7S) には切り替え先が無いので、
    // そのときは見るものが無い。
    if ((int)OscMode::Count < 2) return;

    const int other = (chosen.mode + 1) % (int)OscMode::Count;

    rig.set("MODE", (float)other);
    rig.run(100);

    CHECK_FALSE(rig.lamp());
}

TEST_CASE("PANIC でランプが消える")
{
    LampRig rig;

    const auto chosen = chooseAudibleChannel(rig);

    if (!chosen.found) return;

    rig.set("MODE", (float)chosen.mode);
    rig.run(1, noteOn(69));
    rig.run(5);

    REQUIRE(rig.lamp());

    rig.plugin->panic();
    rig.run(100);

    CHECK_FALSE(rig.lamp());
}
