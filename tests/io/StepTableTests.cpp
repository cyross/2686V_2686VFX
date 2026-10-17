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
}
