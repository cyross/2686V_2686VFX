#include "doctest/doctest.h"

#include <array>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "TestHelpers.h"

#include "Gui/Components/FmToOpzx7/FmToOpzx7Tables.h"
#include "Effect/Lfo/Opna/LfoOpna.h"
#include "Effect/Lfo/Opm/LfoOpm.h"
#include "Effect/Detune/Opn/DetuneOpn.h"
#include "Effect/Detune/Opm/DetuneOpm.h"

// ============================================================================
// FM → OPZX7S の変換が持つ表の写し
// ============================================================================
// OPZX7S のプラグインは FM 音源の本体を持たないので、変換に要る表を
// FmToOpzx7Tables.h へ写してある。元の表を直して写しを忘れると、読み込んだ
// 音色が黙って変わるので、ここで突き合わせる。

namespace
{
    using FmToOpzx7::Tables::Routing;

    std::string readSource(const std::string& rel)
    {
        std::ifstream in(std::string(repoRoot()) + "/2686V/Source/" + rel, std::ios::binary);

        std::stringstream ss;
        ss << in.rdbuf();

        return ss.str();
    }

    // makeAlgOpna({3}, {{0, 1}, {1, 2}}, {{0, 0}}) の並びを、書かれた順に読む
    std::vector<Routing> parseRoutings(const std::string& text, const std::string& maker)
    {
        const std::string pairs = R"(((?:\{\s*\d+\s*,\s*\d+\s*\}\s*,?\s*)*))";
        const std::regex call(maker + R"(\(\{([\d\s,]*)\},\s*\{)" + pairs + R"(\},\s*\{)" + pairs + R"(\}\))");
        const std::regex number(R"(\d+)");
        const std::regex pair(R"(\{\s*(\d+)\s*,\s*(\d+)\s*\})");

        std::vector<Routing> result;

        for (std::sregex_iterator it(text.begin(), text.end(), call), end; it != end; ++it)
        {
            Routing r;

            const std::string carriers = (*it)[1].str();
            for (std::sregex_iterator n(carriers.begin(), carriers.end(), number), e; n != e; ++n) {
                r.out[(size_t)std::stoi(n->str())] = true;
            }

            const std::string mods = (*it)[2].str();
            for (std::sregex_iterator p(mods.begin(), mods.end(), pair), e; p != e; ++p) {
                r.mod[(size_t)std::stoi((*p)[2].str())][(size_t)std::stoi((*p)[1].str())] = true;
            }

            const std::string fbMods = (*it)[3].str();
            for (std::sregex_iterator p(fbMods.begin(), fbMods.end(), pair), e; p != e; ++p) {
                r.fbMod[(size_t)std::stoi((*p)[2].str())][(size_t)std::stoi((*p)[1].str())] = true;
            }

            result.push_back(r);
        }

        return result;
    }

    template <size_t N>
    void checkRoutings(const std::array<Routing, N>& copy, const std::string& rel, const std::string& maker)
    {
        const auto original = parseRoutings(readSource(rel), maker);

        REQUIRE(original.size() == N);

        for (size_t i = 0; i < N; ++i)
        {
            CAPTURE(rel);
            CAPTURE(i);

            CHECK(copy[i].out == original[i].out);
            CHECK(copy[i].mod == original[i].mod);
            CHECK(copy[i].fbMod == original[i].fbMod);
        }
    }
}

TEST_CASE("FmToOpzx7: routing copies match the synth cores")
{
    checkRoutings(FmToOpzx7::Tables::opnRoutings, "Synth/Opna/SynthOpna.cpp", "makeAlgOpna");
    checkRoutings(FmToOpzx7::Tables::opnRoutings, "Synth/Opn/SynthOpn.cpp", "makeAlgOpn");
    checkRoutings(FmToOpzx7::Tables::opnRoutings, "Synth/Opm/SynthOpm.cpp", "makeAlgOpm");
    checkRoutings(FmToOpzx7::Tables::oplRoutings, "Synth/Opl/SynthOpl.cpp", "makeAlgOpl");
    checkRoutings(FmToOpzx7::Tables::opl3Routings, "Synth/Opl3/SynthOpl3.cpp", "makeAlgOpl3");
}

TEST_CASE("FmToOpzx7: LFO and detune copies match the effects")
{
    CHECK(FmToOpzx7::Tables::opnaLfoFreqs == OpnaLfoCore::freqs);
    CHECK(FmToOpzx7::Tables::opnaPmsDepths == OpnaLfoCore::pmsDepths);
    CHECK(FmToOpzx7::Tables::opnaAmsDepths == OpnaLfoCore::amsDepths);

    CHECK(FmToOpzx7::Tables::opmPmsDepths == OpmLfoCore::pmsDepths);
    CHECK(FmToOpzx7::Tables::opmAmsDepths == OpmLfoCore::amsDepths);

    CHECK(FmToOpzx7::Tables::opnDtScales == OpnDetune::dtScales);
    CHECK(FmToOpzx7::Tables::opmDtScales == OpmDetune::dtScales);
}
