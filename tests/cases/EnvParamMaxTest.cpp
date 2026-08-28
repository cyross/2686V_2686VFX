#include "doctest/doctest.h"

#include <cmath>

#include "Effect/Envelope/Amp/OplAdsr/EnvOplAdsr.h"
#include "Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssr.h"
#include "Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddr.h"
#include "Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.h"

// ============================================================================
// setParamMax() を呼ばなくても壊れないこと
// ============================================================================
// レジスタ幅 (arMax など) が 0 のまま setParameters() が来ると、
// 増減量の計算で 0 除算が起き、(int)inf が時間テーブルの範囲外を指して
// クラッシュしていた。出荷経路ではオペレータの prepare() が必ず
// setParamMax() を先に呼ぶので実害は無かったが、順序を間違えたときに
// 落ちない作りにしてある。

namespace
{
    constexpr double kRate = 48000.0;
}

// OplAdsr : setParamMax なしでも落ちない
TEST_CASE("OplAdsr: survives setParameters without setParamMax")
{
    OplAdsr env;
    env.prepare(1, kRate);

    OplAdsrParams p;
    p.ar = 15;
    p.dr = 15;
    p.sl = 15;
    p.rr = 15;
    p.tl = 0;

    env.setParameters(p);

    float level = env.noteOn(1.0f, 60);

    for (int i = 0; i < 1000; ++i) {
        level = env.updateEnvelopeState(level);

        REQUIRE(std::isfinite(level));
    }
}

// FmRgAdssr : setParamMax なしでも落ちない
TEST_CASE("FmRgAdssr: survives setParameters without setParamMax")
{
    FmRgAdssr env;
    env.prepare(1, kRate);

    FmRgAdssrParams p;

    env.setParameters(p);

    float level = env.noteOn(1.0f, 60);

    REQUIRE(std::isfinite(level));
}

// FmRgAdddr : setParamMax なしでも落ちない
TEST_CASE("FmRgAdddr: survives setParameters without setParamMax")
{
    FmRgAdddr env;
    env.prepare(1, kRate);

    FmRgAdddrParams p;

    env.setParameters(p);

    float level = env.noteOn(1.0f, 60);

    REQUIRE(std::isfinite(level));
}

// Opzx7Adddr : setParamMax なしでも落ちない
TEST_CASE("Opzx7Adddr: survives setParameters without setParamMax")
{
    Opzx7Adddr env;
    env.prepare(1, kRate);

    Opzx7AdddrParams p;

    env.setParameters(p);

    float level = env.noteOn(1.0f, 60);

    REQUIRE(std::isfinite(level));
}
