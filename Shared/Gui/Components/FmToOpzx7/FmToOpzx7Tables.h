#pragma once

#include <array>
#include <initializer_list>

// ============================================================================
// FM 音源から OPZX7S へ直すときに使う、元の音源の表の写し
// ============================================================================
// OPZX7S のプラグインは OPNA / OPN / OPL / OPL3 / OPM の音源を持たないが、
// それらのファイルを読み込めるようにしたい。音源の本体を引き込むと
// 処理の塊ごと入ってしまうので、変換に要る表だけをここへ写しておく。
//
// 写しなので、元の表を変えたらここも合わせること。ずれはユニットテスト
// (tests/cases/FmToOpzx7TablesTest.cpp) で見つかる。
//
// JUCE を読み込まない。テストからそのまま読むため。
namespace FmToOpzx7::Tables
{
    // ------------------------------------------------------------------
    // アルゴリズム
    // ------------------------------------------------------------------
    // 4 オペレータまでのつなぎ方。どれも [dest][src] で持つ
    // (Synth*.cpp の AlgRouting と同じ向き)。
    struct Routing
    {
        std::array<bool, 4> out{};
        std::array<std::array<bool, 4>, 4> mod{};
        std::array<std::array<bool, 4>, 4> fbMod{};
    };

    // makeAlgOpna などと同じ書き方で組み立てる。組は { 接続元, 接続先 }
    constexpr Routing makeRouting(std::initializer_list<int> carriers,
        std::initializer_list<std::array<int, 2>> mods,
        std::initializer_list<std::array<int, 2>> fbMods)
    {
        Routing r;

        for (int c : carriers) r.out[(size_t)c] = true;
        for (const auto& m : mods) r.mod[(size_t)m[1]][(size_t)m[0]] = true;
        for (const auto& m : fbMods) r.fbMod[(size_t)m[1]][(size_t)m[0]] = true;

        return r;
    }

    // OPNA / OPN / OPM は同じ 8 通り (SynthOpna.cpp / SynthOpn.cpp / SynthOpm.cpp)
    inline constexpr std::array<Routing, 8> opnRoutings = {
        makeRouting({ 3 }, { { 0, 1 }, { 1, 2 }, { 2, 3 } }, { { 0, 0 } }),       // 00
        makeRouting({ 3 }, { { 0, 2 }, { 1, 2 }, { 2, 3 } }, { { 0, 0 } }),       // 01
        makeRouting({ 3 }, { { 0, 1 }, { 1, 3 }, { 2, 3 } }, { { 2, 2 } }),       // 02
        makeRouting({ 3 }, { { 0, 1 }, { 1, 3 }, { 2, 3 } }, { { 0, 0 } }),       // 03
        makeRouting({ 1, 3 }, { { 0, 1 }, { 2, 3 } }, { { 0, 0 } }),              // 04
        makeRouting({ 1, 2, 3 }, { { 0, 1 }, { 0, 2 }, { 0, 3 } }, { { 0, 0 } }), // 05
        makeRouting({ 1, 2, 3 }, { { 0, 1 } }, { { 0, 0 } }),                     // 06
        makeRouting({ 0, 1, 2, 3 }, {}, { { 0, 0 } }),                            // 07
    };

    // OPL (SynthOpl.cpp)
    inline constexpr std::array<Routing, 2> oplRoutings = {
        makeRouting({ 1 }, { { 0, 1 } }, { { 0, 0 } }), // 00
        makeRouting({ 0, 1 }, {}, { { 0, 0 } }),        // 01
    };

    // OPL3 (SynthOpl3.cpp)
    inline constexpr std::array<Routing, 8> opl3Routings = {
        makeRouting({ 3 }, { { 0, 1 }, { 1, 2 }, { 2, 3 } }, { { 0, 0 } }),    // 00
        makeRouting({ 1, 3 }, { { 0, 1 }, { 2, 3 } }, { { 0, 0 } }),           // 01
        makeRouting({ 0, 3 }, { { 1, 2 }, { 2, 3 } }, { { 0, 0 } }),           // 02
        makeRouting({ 0, 2, 3 }, { { 1, 2 } }, { { 0, 0 } }),                  // 03
        makeRouting({ 1, 2, 3 }, { { 0, 2 } }, { { 0, 0 } }),                  // 04
        makeRouting({ 0, 1, 2, 3 }, {}, { { 0, 0 } }),                         // 05
        makeRouting({ 1, 3 }, { { 0, 1 }, { 2, 3 } }, { { 0, 0 }, { 2, 2 } }), // 06
        makeRouting({ 0, 1, 2, 3 }, {}, { { 0, 0 }, { 2, 2 } }),               // 07
    };

    // ------------------------------------------------------------------
    // LFO
    // ------------------------------------------------------------------
    // OPNA のハードウェア LFO (LfoOpna.cpp の OpnaLfoCore)
    inline constexpr std::array<float, 8> opnaLfoFreqs = { 3.98f, 5.56f, 6.02f, 6.37f, 6.88f, 9.63f, 48.1f, 72.2f };
    inline constexpr std::array<float, 8> opnaPmsDepths = { 0.0f, 0.001965f, 0.003876f, 0.005793f, 0.008122f, 0.011619f, 0.023374f, 0.047294f };
    inline constexpr std::array<float, 4> opnaAmsDepths = { 0.0f, 1.4f / 24.0f, 5.9f / 24.0f, 1.0f };

    // OPM の LFO (LfoOpm.cpp の OpmLfoCore)
    inline constexpr std::array<float, 8> opmPmsDepths = { 0.0f, 0.002892f, 0.005793f, 0.011619f, 0.029302f, 0.059463f, 0.259921f, 0.498307f };
    inline constexpr std::array<float, 4> opmAmsDepths = { 0.0f, 0.25f, 0.5f, 1.0f };

    // ------------------------------------------------------------------
    // DT1
    // ------------------------------------------------------------------
    // OPN 系と OPM で同じ値 (DetuneOpn.cpp / DetuneOpm.cpp)
    inline constexpr std::array<float, 8> opnDtScales = { 0.0f, -0.0045f, -0.0025f, -0.001f, 0.0f, 0.001f, 0.0025f, 0.0045f };
    inline constexpr std::array<float, 8> opmDtScales = { 0.0f, -0.0045f, -0.0025f, -0.001f, 0.0f, 0.001f, 0.0025f, 0.0045f };
}
