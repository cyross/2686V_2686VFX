// ============================================================================
// PARAM ENV[256] と TAP PARAM SEQUENCER (4.3.0)
// ============================================================================
// どちらも、音源のパラメータをボイスごとに別の値へ置き換える。置き換える先は
// APVTS のパラメータそのもので、ボイスはプロセッサに読み直してもらった値で鳴る。
//
//   TAP PARAM SEQUENCER : 押したときの段の組で、その音のあいだ置き換える
//   PARAM ENV (オペレーター) : TL は 4.0.0 の TL ENV と同じ道、それ以外は読み直し
//   4.2.x の TL ENV の state は、PARAM ENV (TARGET が TL) へ写して読む
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <cmath>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Synth/StepTableIo.h"

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

        bool has(const juce::String& id) const { return plugin->apvts.getParameter(id) != nullptr; }

        bool set(const juce::String& id, float value)
        {
            auto* param = plugin->apvts.getParameter(id);

            if (param == nullptr) return false;

            param->setValueNotifyingHost(param->convertTo0to1(value));

            return true;
        }

        float get(const juce::String& id) const
        {
            auto* raw = plugin->apvts.getRawParameterValue(id);

            return raw != nullptr ? raw->load() : -1.0f;
        }

        // 鳴らした大きさ (RMS)
        double run(juce::MidiBuffer midi = {})
        {
            buffer.clear();
            plugin->processBlock(buffer, midi);

            double sum = 0.0;

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
                const auto* p = buffer.getReadPointer(ch);

                for (int i = 0; i < kBlock; ++i) sum += (double)p[i] * (double)p[i];
            }

            return std::sqrt(sum / (double)(kBlock * buffer.getNumChannels()));
        }

        // 鍵を押して、blocks ブロックぶんの大きさを足す
        double play(int note, int blocks)
        {
            juce::MidiBuffer midi;

            midi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)100), 0);

            double level = run(midi);

            for (int i = 1; i < blocks; ++i) level += run();

            juce::MidiBuffer up;

            up.addEvent(juce::MidiMessage::noteOff(1, note), 0);
            run(up);

            // 余韻を流しきる
            for (int i = 0; i < 40; ++i) run();

            return level;
        }
    };

    // FM 音源 (オペレーターを持つチャンネル)。番号と、オペレーターの数
    struct FmChip
    {
        int mode;
        juce::String prefix;
        int ops;
    };

    std::vector<FmChip> fmChips(Rig& rig, std::initializer_list<const char*> names)
    {
        std::vector<FmChip> found;

        for (int m = 0; m < (int)OscMode::Count; ++m) {
            const auto name = getModeName((OscMode)m);

            bool wanted = false;

            for (const char* n : names) wanted = wanted || name == n;

            if (!wanted) continue;

            const auto* chip = GuiTestChips::byModeName(name);

            if (chip == nullptr) continue;

            const juce::String prefix(chip->prefix);

            int ops = 0;

            while (rig.has(prefix + "_OP" + juce::String(ops) + "_R_TL")) ++ops;

            if (ops > 0) found.push_back({ m, prefix, ops });
        }

        return found;
    }

    const auto allFm = { "OPNA", "OPN", "OPL", "OPL3", "OPM", "OPZX7" };
}

TEST_CASE("TAP PARAM SEQUENCER: 押したときの段の組で、その音のあいだパラメータを置き換える")
{
    Rig probe;

    const auto chips = fmChips(probe, allFm);

    if (chips.empty()) {
        MESSAGE("FM 音源を持たないプラグイン");
        return;
    }

    for (const auto& chip : chips) {
        CAPTURE(chip.prefix);

        Rig rig;

        REQUIRE(rig.set("MODE", (float)chip.mode));
        REQUIRE(rig.set("MONO_MODE", 0.0f));
        rig.run();

        const double base = rig.play(60, 10);

        REQUIRE(base > 1.0e-4);

        // オペレーターごとの TAP PARAM SEQUENCER。1 段目: そのオペレーターの TL を最大
        // (いちばん小さい音) にする。2 段目: SKIP。どのオペレーターも別々に段を進める
        std::vector<StepTable*> tables;

        for (int op = 0; op < chip.ops; ++op) {
            const auto head = chip.prefix + "_OP" + juce::String(op) + "_TAPPARAM";
            auto* table = rig.plugin->stepTables.find(head);

            REQUIRE(table != nullptr);
            REQUIRE(table->hasPairs());

            // 組の scope は表の持ち主で決まる (-1 を書く)。0 は一覧の TL
            table->setPairs(1, { { -1, 0, 1.0f } });
            table->setSkip(2, true);

            REQUIRE(rig.set(head + "_ENABLE", 1.0f));
            REQUIRE(rig.set(head + "_STEPS", 2.0f));

            tables.push_back(table);
        }

        rig.run();

        const double muted = rig.play(60, 10);

        for (auto* t : tables) CHECK(t->cursor.load() == 0);

        CHECK(muted < base * 0.05);

        // SKIP の段は置き換えない
        const double skipped = rig.play(60, 10);

        for (auto* t : tables) CHECK(t->cursor.load() == 1);

        CHECK(skipped > base * 0.5);

        // チャンネルの TAP PARAM SEQUENCER は使っていないので、開始前のまま
        auto* channel = rig.plugin->stepTables.find(chip.prefix + "_TAPPARAM");

        REQUIRE(channel != nullptr);
        CHECK(channel->cursor.load() < 0);

        // 置き換えたのはボイスだけ。パラメータそのものは変わらない
        CHECK(rig.get(chip.prefix + "_OP0_R_TL") == doctest::Approx(0.0f));
    }
}

TEST_CASE("PARAM ENV (オペレーター): TL 以外を指せば、その値で読み直す")
{
    Rig probe;

    // AR がレジスタ値で、0 なら音が立ち上がらない音源
    const auto chips = fmChips(probe, { "OPNA", "OPN", "OPL", "OPL3", "OPM" });

    if (chips.empty()) {
        MESSAGE("対象の FM 音源を持たないプラグイン");
        return;
    }

    for (const auto& chip : chips) {
        CAPTURE(chip.prefix);

        Rig rig;

        REQUIRE(rig.set("MODE", (float)chip.mode));
        rig.run();

        const double base = rig.play(60, 10);

        REQUIRE(base > 1.0e-4);

        // すべてのオペレーターの PARAM ENV を AR (一覧の 1 番) へ向け、STL と段を 0 にする
        for (int op = 0; op < chip.ops; ++op) {
            const auto p = chip.prefix + "_OP" + juce::String(op) + "_PRMENV";

            REQUIRE(rig.set(p + "_ENABLE", 1.0f));
            REQUIRE(rig.set(p + "_TARGET", 1.0f));
            REQUIRE(rig.set(p + "_STL", 0.0f));
            REQUIRE(rig.set(p + "_STEPS", 1.0f));
        }

        rig.run();

        const double silent = rig.play(60, 10);

        CHECK(silent < base * 0.05);

        // パラメータの AR はそのまま
        CHECK(rig.get(chip.prefix + "_OP0_R_AR") > 0.0f);
    }
}

TEST_CASE("PARAM ENV (オペレーター): 4.2.x の TL ENV の state は TARGET が TL の PARAM ENV へ写す")
{
    Rig rig;

    const auto chips = fmChips(rig, allFm);

    if (chips.empty()) {
        MESSAGE("FM 音源を持たないプラグイン");
        return;
    }

    const auto& chip = chips.front();
    const auto head = chip.prefix + "_OP0";
    const float tlMax = (chip.prefix == "OPNA" || chip.prefix == "OPN" || chip.prefix == "OPM") ? 127.0f : 63.0f;

    juce::MemoryBlock block;

    rig.plugin->getStateInformation(block);

    auto xml = juce::AudioProcessor::getXmlFromBinary(block.getData(), (int)block.getSize());

    REQUIRE(xml != nullptr);

    // 4.3.0 の PARAM ENV を外し、4.2.x の TL ENV を書く
    std::vector<juce::XmlElement*> drop;

    for (auto* p : xml->getChildWithTagNameIterator("PARAM"))
        if (p->getStringAttribute("id").startsWith(head + "_PRMENV_")) drop.push_back(p);

    for (auto* p : drop) xml->removeChildElement(p, true);

    auto addParam = [&xml](const juce::String& id, const juce::String& value) {
        auto* p = xml->createNewChildElement("PARAM");

        p->setAttribute("id", id);
        p->setAttribute("value", value);
    };

    addParam(head + "_TLENV_ENABLE", "1");
    addParam(head + "_TLENV_STEPS", "3");
    addParam(head + "_TLENV_STL", juce::String(tlMax));
    addParam(head + "_TLENV_LR", "10");
    addParam(head + "_TLENV_RR", "0.5");

    auto* tables = xml->getChildByName(StepTableIo::xmlTag);

    if (tables == nullptr) tables = xml->createNewChildElement(StepTableIo::xmlTag);

    auto* item = tables->createNewChildElement(StepTableIo::xmlItem);

    item->setAttribute("key", head + "_TLENV");
    item->setAttribute("values", "0.2,0.3|32,48");

    juce::MemoryBlock old;

    juce::AudioProcessor::copyXmlToBinary(*xml, old);

    rig.plugin->setStateInformation(old.getData(), (int)old.getSize());

    CHECK(rig.get(head + "_PRMENV_ENABLE") == doctest::Approx(1.0f));
    CHECK(rig.get(head + "_PRMENV_TARGET") == doctest::Approx(0.0f));
    CHECK(rig.get(head + "_PRMENV_STEPS") == doctest::Approx(3.0f));
    CHECK(rig.get(head + "_PRMENV_STL") == doctest::Approx(1.0f).epsilon(0.001));
    CHECK(rig.get(head + "_PRMENV_LR") == doctest::Approx(10.0f / tlMax).epsilon(0.001));
    CHECK(rig.get(head + "_PRMENV_RR") == doctest::Approx(0.5f).epsilon(0.001));

    auto* table = rig.plugin->stepTables.find(head + "_PRMENV");

    REQUIRE(table != nullptr);

    CHECK(table->rate(1) == doctest::Approx(0.2f));
    CHECK(table->rate(2) == doctest::Approx(0.3f));
    CHECK(table->level(1) == doctest::Approx(32.0f / tlMax).epsilon(0.001));
    CHECK(table->level(2) == doctest::Approx(48.0f / tlMax).epsilon(0.001));

    // 写したあとに書き出した state には、TL ENV は残らない
    juce::MemoryBlock saved;

    rig.plugin->getStateInformation(saved);

    auto again = juce::AudioProcessor::getXmlFromBinary(saved.getData(), (int)saved.getSize());

    REQUIRE(again != nullptr);
    CHECK(again->getChildByAttribute("id", head + "_TLENV_ENABLE") == nullptr);
}

TEST_CASE("TAP PARAM SEQUENCER: 段の組は state へ書き、読み直せる")
{
    Rig rig;

    for (const auto& chip : GuiTestChips::all) {
        const juce::String prefix(chip.prefix);

        // RHYTHM のパッドの頭ではなく、チャンネルの頭に表がある
        const juce::String head = prefix.upToFirstOccurrenceOf("_PAD", false, false);

        auto* table = rig.plugin->stepTables.find(head + "_TAPPARAM");

        if (table == nullptr) continue;

        CAPTURE(head);

        table->setPairs(1, { { -1, 0, 0.25f }, { 0, 1, 0.75f } });
        table->setPairs(3, { { -1, 1, 1.0f } });

        juce::MemoryBlock block;

        rig.plugin->getStateInformation(block);

        table->reset();

        REQUIRE(table->pairCount(1) == 0);

        rig.plugin->setStateInformation(block.getData(), (int)block.getSize());

        REQUIRE(table->pairCount(1) == 2);
        CHECK(table->pairAt(1, 0) == StepPair{ -1, 0, 0.25f });
        CHECK(table->pairAt(1, 1) == StepPair{ 0, 1, 0.75f });
        CHECK(table->pairCount(2) == 0);
        REQUIRE(table->pairCount(3) == 1);
        CHECK(table->pairAt(3, 0) == StepPair{ -1, 1, 1.0f });
    }
}
