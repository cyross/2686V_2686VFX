#include "doctest/doctest.h"

#include <filesystem>
#include <string>
#include <vector>

#include "TestHelpers.h"

// ============================================================================
// MML へ書き出すマスクの並び
// ============================================================================
// Ctrl + C で出す MML には、オペレータのマスクを 1 つの数へ畳んだ値が入る。
// genMask4 は上の桁から OP1・OP2・OP3・OP4 の順に受け取る約束だが、呼ぶ側で
// 2 番目を二度渡し、4 番目を渡していないところが 4 タブぶんあった。
// mask4 は計算だけされて捨てられていて、ビルドも警告も素通りする。
//
// 実際の値を作るには juce::String が要り、この試験の JUCE は差し替えのため
// 使えない。代わりに、呼び出しの並びをソースの字面で見張る。
//
// 渡し方は 2 つある。
//
//   つまみから読む形       genMask4(mask1, mask2, mask3, mask4)
//   パラメータから読む形   genMask4(opMask(0), opMask(1), opMask(2), opMask(3))
//
// オペレータごとにつまみを並べるのをやめたタブは後者になる。設定を
// ひとそろいへ畳むと、画面に出ているつまみが 1 本ぶんしか無くなるため。
// どちらの形であっても、4 本を順に渡していることを見る。

namespace
{
    namespace fs = std::filesystem;

    // マスクを 4 本持つタブ。OPL は 2 本なので genMask2 を使っており対象外。
    const std::vector<std::string> kTabs = {
        "Gui/Opna/GuiOpna.cpp",
        "Gui/Opn/GuiOpn.cpp",
        "Gui/Opl3/GuiOpl3.cpp",
        "Gui/Opm/GuiOpm.cpp",
    };

    const std::vector<std::string> kProjects = {
        "2686V", "2686VLight", "26V", "2686VOrigin", "OPZX7S", "2686VFX",
    };

    bool has(const std::string& src, const std::string& needle)
    {
        return src.find(needle) != std::string::npos;
    }
}

TEST_CASE("genMask4 は上の桁から OP1..OP4 を並べる")
{
    std::string src;

    REQUIRE(readNormalized(repoRoot() + "/2686V/Source/Core/Fm/FmMmlFormatter.h", src));

    // 引数の名前と桁の対応。ここが変わったら呼ぶ側も見直すこと。
    CHECK(has(src, "genMask4 = [](bool mask1, bool mask2, bool mask3, bool mask4)"));
    CHECK(has(src, "(mask1 << 3) + (mask2 << 2) + (mask3 << 1) + mask4"));
}

TEST_CASE("MML のマスクは 4 本とも渡している")
{
    int seen = 0;

    for (const auto& proj : kProjects) {
        for (const auto& tab : kTabs) {
            const std::string path = repoRoot() + "/" + proj + "/Source/" + tab;

            if (!fs::exists(path)) continue;

            std::string src;

            REQUIRE(readNormalized(path, src));

            INFO(proj << " / " << tab);

            // 取り違えていた形が残っていないこと
            CHECK_FALSE(has(src, "genMask4(mask1, mask2, mask2, mask3)"));

            const bool fromSliders = has(src, "genMask4(mask1, mask2, mask3, mask4)");
            const bool fromParams = has(src, "genMask4(opMask(0), opMask(1), opMask(2), opMask(3))");

            // どちらかの形で、4 本を順に渡していること
            CHECK((fromSliders || fromParams));

            if (fromSliders) {
                // 受け皿そのものが OP1..OP4 から採れていること
                CHECK(has(src, "mask1 = FmMml::bool2Int(!mask[0]"));
                CHECK(has(src, "mask2 = FmMml::bool2Int(!mask[1]"));
                CHECK(has(src, "mask3 = FmMml::bool2Int(!mask[2]"));
                CHECK(has(src, "mask4 = FmMml::bool2Int(!mask[3]"));
            }

            if (fromParams) {
                // 番号からマスクのパラメータを引いていること。
                // 番号を渡し忘れると全部同じ値になり、気付きにくい。
                CHECK(has(src, "auto opMask = [&apvts](int index)"));
                CHECK(has(src, "juce::String(index) + CPK::Fm::mask"));
            }

            ++seen;
        }
    }

    // 見に行く先が無くなっていたら、この試験は何も守っていない
    CHECK(seen >= 4);
}
