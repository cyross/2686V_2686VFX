#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Shared/Core/Synth/StepTableIo.h"
#include "Shared/Core/Io/IoStepEnvLegacy.h"

TEST_CASE("StepTable: 文字へ書き出して読み戻せる")
{
    StepTable a(0.001f, 1.0f);
    a.setRate(3, 0.5f);
    a.setLevel(1, 0.25f);
    a.setLevel(200, 0.75f);

    StepTable b(0.001f, 1.0f);
    b.setLevel(5, 0.1f);
    StepTableIo::fromText(b, StepTableIo::toText(a));

    CHECK(b.rate(3) == doctest::Approx(0.5f));
    CHECK(b.level(1) == doctest::Approx(0.25f));
    CHECK(b.level(200) == doctest::Approx(0.75f));

    // 書かれていない段は初期の値へ戻る
    CHECK(b.level(5) == doctest::Approx(1.0f));
    CHECK(b.rate(256) == doctest::Approx(0.001f));
}

TEST_CASE("StepTable: ループの組も文字へ書き出して読み戻せる。組が無ければこれまでと同じ文字")
{
    StepTable a(0.001f, 1.0f);
    a.setLevel(2, 0.5f);

    const auto plain = StepTableIo::toText(a);

    CHECK(plain.indexOfChar('|') == plain.lastIndexOfChar('|'));

    a.setLoops({ { 3, 5, 0 }, { 1, 8, 2 } });

    StepTable b(0.001f, 1.0f);
    b.setLoops({ { 7, 7, 7 } });
    StepTableIo::fromText(b, StepTableIo::toText(a));

    REQUIRE(b.loopCount() == 2);
    CHECK(b.loopAt(0) == StepLoop::Entry{ 1, 8, 2 });
    CHECK(b.loopAt(1) == StepLoop::Entry{ 3, 5, 0 });
    CHECK(b.level(2) == doctest::Approx(0.5f));

    // 4.0.x までの文字 (組が無い) を読むと、組は空になる
    StepTableIo::fromText(b, plain);

    CHECK(b.loopCount() == 0);
}

TEST_CASE("StepTable: SKIP の段も文字へ書き出して読み戻せる。組が無くても挟んで書く")
{
    StepTable a(0.0f, 1.0f);
    a.setLevel(2, 0.5f);
    a.setSkip(3, true);
    a.setSkip(200, true);

    const auto text = StepTableIo::toText(a);

    CHECK(text.endsWith("||3,200"));

    StepTable b(0.0f, 1.0f);
    b.setSkip(5, true);
    StepTableIo::fromText(b, text);

    CHECK(b.skipped(3));
    CHECK(b.skipped(200));
    CHECK_FALSE(b.skipped(5));
    CHECK(b.loopCount() == 0);
    CHECK(b.level(2) == doctest::Approx(0.5f));

    // 組と並べても読める
    a.setLoops({ { 1, 2, 1 } });
    StepTableIo::fromText(b, StepTableIo::toText(a));

    CHECK(b.loopCount() == 1);
    CHECK(b.skipped(3));

    // SKIP の無い文字 (4.0.x まで) を読むと、SKIP は外れる
    StepTable plain(0.0f, 1.0f);
    plain.setLevel(2, 0.5f);
    StepTableIo::fromText(b, StepTableIo::toText(plain));

    CHECK_FALSE(b.skipped(3));
    CHECK_FALSE(b.hasSkips());
}

TEST_CASE("StepEnvLegacy: [11] の state を [256] のパラメータと表へ写す")
{
    StepTableStore store;
    auto& table = store.get("OPNA_OP1_SSGSW256", 0.001f, 1.0f);

    juce::XmlElement state("Parameters");

    auto add = [&](const juce::String& id, const juce::String& value) {
        auto* p = state.createNewChildElement("PARAM");
        p->setAttribute("id", id);
        p->setAttribute("value", value);
    };

    add("OPNA_OP1_SSGSW11_ENABLE", "1");
    add("OPNA_OP1_SSGSW11_STEPS", "3");
    add("OPNA_OP1_SSGSW11_STL", "0.1");
    add("OPNA_OP1_SSGSW11_R2", "0.25");
    add("OPNA_OP1_SSGSW11_L2", "0.5");
    add("OPNA_OP1_SSGSW11_R11", "0.4");
    add("OPNA_OP1_SSGSW11_L11", "0.05");

    CHECK(StepEnvLegacy::convertState(state, store, StepEnvLegacy::ssgSwEnv));

    auto valueOf = [&](const juce::String& id) {
        for (auto* p : state.getChildWithTagNameIterator("PARAM"))
            if (p->getStringAttribute("id") == id) return p->getStringAttribute("value");

        return juce::String("missing");
    };

    CHECK(valueOf("OPNA_OP1_SSGSW256_ENABLE") == "1");
    CHECK(valueOf("OPNA_OP1_SSGSW256_STEPS") == "3");
    CHECK(valueOf("OPNA_OP1_SSGSW256_STL") == "0.1");
    CHECK(valueOf("OPNA_OP1_SSGSW256_RR") == "0.4");
    CHECK(valueOf("OPNA_OP1_SSGSW256_LR") == "0.05");

    CHECK(table.rate(2) == doctest::Approx(0.25f));
    CHECK(table.level(2) == doctest::Approx(0.5f));

    // 写した古いパラメータは外れる (残すと読み込むたびに表を上書きする)
    CHECK(valueOf("OPNA_OP1_SSGSW11_STEPS") == "missing");
    CHECK(valueOf("OPNA_OP1_SSGSW11_R2") == "missing");
}

namespace
{
    struct SixStepState
    {
        juce::XmlElement state{ "Parameters" };

        void add(const juce::String& id, const juce::String& value)
        {
            auto* p = state.createNewChildElement("PARAM");
            p->setAttribute("id", id);
            p->setAttribute("value", value);
        }

        juce::String valueOf(const juce::String& id) const
        {
            for (auto* p : state.getChildWithTagNameIterator("PARAM"))
                if (p->getStringAttribute("id") == id) return p->getStringAttribute("value");

            return "missing";
        }
    };
}

TEST_CASE("StepEnvLegacy: 6 段だけが使われていれば [256] へ写す")
{
    StepTableStore store;
    auto& table = store.get("SSG_SSGSW256", 0.001f, 1.0f);
    table.setLevel(8, 0.3f);

    SixStepState s;
    s.add("SSG_SSGSW_BYPASS", "0");
    s.add("SSG_SSGSW_STEPS", "4");
    s.add("SSG_SSGSW_LOOP", "1");
    s.add("SSG_SSGSW_LOOPTO", "1");
    s.add("SSG_SSGSW_STL", "0.2");
    s.add("SSG_SSGSW_R3", "0.25");
    s.add("SSG_SSGSW_L3", "0.5");
    s.add("SSG_SSGSW_R6", "0.4");
    s.add("SSG_SSGSW_L6", "0.05");
    s.add("SSG_SSGSW256_BYPASS", "1");
    s.add("SSG_SSGSW256_KEEP", "1");

    CHECK(StepEnvLegacy::convertSixStep(s.state, store).isEmpty());

    CHECK(s.valueOf("SSG_SSGSW256_BYPASS") == "0");
    CHECK(s.valueOf("SSG_SSGSW256_STEPS") == "4");
    CHECK(s.valueOf("SSG_SSGSW256_LOOP") == "1");
    CHECK(s.valueOf("SSG_SSGSW256_LOOPTO") == "1");
    CHECK(s.valueOf("SSG_SSGSW256_STL") == "0.2");
    CHECK(s.valueOf("SSG_SSGSW256_RR") == "0.4");
    CHECK(s.valueOf("SSG_SSGSW256_LR") == "0.05");
    CHECK(s.valueOf("SSG_SSGSW256_KEEP") == "0");
    CHECK(s.valueOf("SSG_SSGSW256_ENDL_EN") == "0");

    CHECK(table.rate(3) == doctest::Approx(0.25f));
    CHECK(table.level(3) == doctest::Approx(0.5f));

    // 写す前の表は消える
    CHECK(table.level(8) == doctest::Approx(1.0f));

    // 6 段のパラメータは外れる
    CHECK(s.valueOf("SSG_SSGSW_STEPS") == "missing");
    CHECK(s.valueOf("SSG_SSGSW_BYPASS") == "missing");
}

TEST_CASE("StepEnvLegacy: オペレータの 6 段は ENABLE で入切を見る")
{
    StepTableStore store;
    auto& table = store.get("OPNA_OP2_SSGSW256", 0.001f, 1.0f);

    SixStepState s;
    s.add("OPNA_OP2_SSGSW_ENABLE", "1");
    s.add("OPNA_OP2_SSGSW_STEPS", "2");
    s.add("OPNA_OP2_SSGSW_L1", "0.7");
    s.add("OPNA_OP2_SSGSW256_ENABLE", "0");

    // 切れている 6 段は写さない
    s.add("OPNA_OP3_SSGSW_ENABLE", "0");
    s.add("OPNA_OP3_SSGSW_STEPS", "3");
    s.add("OPNA_OP3_SSGSW256_ENABLE", "0");
    s.add("OPNA_OP3_SSGSW256_STEPS", "10");

    CHECK(StepEnvLegacy::convertSixStep(s.state, store).isEmpty());

    CHECK(s.valueOf("OPNA_OP2_SSGSW256_ENABLE") == "1");
    CHECK(s.valueOf("OPNA_OP2_SSGSW256_STEPS") == "2");
    CHECK(s.valueOf("OPNA_OP2_SSGSW256_BYPASS") == "missing");
    CHECK(table.level(1) == doctest::Approx(0.7f));

    CHECK(s.valueOf("OPNA_OP3_SSGSW256_ENABLE") == "0");
    CHECK(s.valueOf("OPNA_OP3_SSGSW256_STEPS") == "10");
}

TEST_CASE("StepEnvLegacy: [256] も使われていれば 6 段は捨てる")
{
    StepTableStore store;
    auto& table = store.get("SSG_SSGSW256", 0.001f, 1.0f);
    table.setLevel(2, 0.4f);

    SixStepState s;
    s.add("SSG_SSGSW_BYPASS", "0");
    s.add("SSG_SSGSW_STEPS", "3");
    s.add("SSG_SSGSW_L2", "0.9");
    s.add("SSG_SSGSW256_BYPASS", "0");
    s.add("SSG_SSGSW256_STEPS", "7");

    const auto dropped = StepEnvLegacy::convertSixStep(s.state, store);

    CHECK(dropped.size() == 1);
    CHECK(dropped[0] == "SSG");
    CHECK(s.valueOf("SSG_SSGSW256_STEPS") == "7");
    CHECK(table.level(2) == doctest::Approx(0.4f));
}
