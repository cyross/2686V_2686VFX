#include "doctest/doctest.h"

#include <cmath>
#include <memory>

#include "Advanced/Curve/AdvancedCurve.h"

namespace
{
    // CurveParams は 1 つ 439KB あるので、スタックへは置かない。
    std::unique_ptr<CurveParams> makeCurve(bool enable, CurveParams::Logic logic)
    {
        auto p = std::make_unique<CurveParams>();
        p->enable = enable;

        for (int pos = 0; pos < (int)CurveParams::Position::Size; ++pos) {
            for (int t = 0; t < (int)CurveParams::Target::Size; ++t) {
                for (int vp = 0; vp < 16; ++vp) {
                    p->params[(size_t)pos][(size_t)t][(size_t)vp].logic = (int)logic;
                }
            }
        }

        return p;
    }

    constexpr int kPos = (int)CurveParams::Position::Common;
    constexpr int kTarget = (int)CurveParams::Target::AmpEnv;
    constexpr int kParam = (int)CurveParams::TargetAmpEnv::Ar;

    // ArcExp は 1 - sqrt(1 - x^2)
    float arcExp(float x) { return 1.0f - std::sqrt(1.0f - x * x); }
}

// CurveCore : カーブモードが切なら、曲がるロジックでも素通しする
TEST_CASE("CurveCore: disabled curve passes the input through")
{
    // 以前は enable をどこも見ておらず、画面で切っても設定済みのカーブが
    // 音に掛かり続けていた。
    auto core = std::make_unique<CurveCore>();
    core->setParameters(*makeCurve(false, CurveParams::Logic::ArcExp));
    core->acquireForAudio();

    for (float x : { 0.25f, 0.5f, 0.75f }) {
        CHECK(core->processAudio(kPos, kTarget, kParam, x) == doctest::Approx(x));
        CHECK(core->process(kPos, kTarget, kParam, x) == doctest::Approx(x));
    }
}

// CurveCore : カーブモードが入なら曲がる
TEST_CASE("CurveCore: enabled curve bends the input")
{
    auto core = std::make_unique<CurveCore>();
    core->setParameters(*makeCurve(true, CurveParams::Logic::ArcExp));
    core->acquireForAudio();

    CHECK(core->processAudio(kPos, kTarget, kParam, 0.5f)
          == doctest::Approx(arcExp(0.5f)).epsilon(0.0001));
    CHECK(core->process(kPos, kTarget, kParam, 0.5f)
          == doctest::Approx(arcExp(0.5f)).epsilon(0.0001));
}

// CurveCore : Linear は恒等写像
TEST_CASE("CurveCore: linear logic is the identity")
{
    auto core = std::make_unique<CurveCore>();
    core->setParameters(*makeCurve(true, CurveParams::Logic::Linear));
    core->acquireForAudio();

    for (float x : { 0.1f, 0.5f, 0.9f }) {
        CHECK(core->processAudio(kPos, kTarget, kParam, x) == doctest::Approx(x));
    }
}

// CurveCore : 画面が書いた中身は acquireForAudio まではオーディオ側へ届かない
TEST_CASE("CurveCore: audio side only switches on acquireForAudio")
{
    // 画面 (メッセージスレッド) とオーディオスレッドで枠を分けているので、
    // 書いた瞬間にオーディオ側が変わってはいけない。書きかけの中身を
    // 読ませないための仕組みなので、ここが崩れると競合が戻る。
    auto core = std::make_unique<CurveCore>();

    core->setParameters(*makeCurve(true, CurveParams::Logic::Linear));
    core->acquireForAudio();
    REQUIRE(core->processAudio(kPos, kTarget, kParam, 0.5f) == doctest::Approx(0.5f));

    // 画面側が書いた直後。まだ持ち替えていないので、オーディオ側は前のまま。
    core->setParameters(*makeCurve(true, CurveParams::Logic::ArcExp));
    CHECK(core->processAudio(kPos, kTarget, kParam, 0.5f) == doctest::Approx(0.5f));

    // 画面側 (正本) はもう新しい値を見ている
    CHECK(core->process(kPos, kTarget, kParam, 0.5f)
          == doctest::Approx(arcExp(0.5f)).epsilon(0.0001));

    // 持ち替えるとオーディオ側にも届く
    core->acquireForAudio();
    CHECK(core->processAudio(kPos, kTarget, kParam, 0.5f)
          == doctest::Approx(arcExp(0.5f)).epsilon(0.0001));
}

// CurveCore : 枠を何度回しても、最後に渡した中身が読める
TEST_CASE("CurveCore: repeated publishes keep the latest content")
{
    // 枠は 3 枚を回して使う。番号の付け替えを間違えると、何周かしたところで
    // 前の中身や書きかけの枠を読み始める。
    auto core = std::make_unique<CurveCore>();

    for (int i = 0; i < 16; ++i) {
        // 1 回の持ち替えの間に 2 回書く (画面をドラッグしている状況)
        core->setParameters(*makeCurve(true, CurveParams::Logic::Linear));
        core->setParameters(*makeCurve(true, CurveParams::Logic::ArcExp));
        core->acquireForAudio();

        CHECK(core->processAudio(kPos, kTarget, kParam, 0.5f)
              == doctest::Approx(arcExp(0.5f)).epsilon(0.0001));
    }
}

// CurveCore : 持ち替えるものが無ければ、前の中身をそのまま使い続ける
TEST_CASE("CurveCore: acquire without a new publish keeps the previous content")
{
    // processBlock は毎ブロック acquireForAudio を呼ぶ。新しい枠が出ていない
    // ときに空の枠を掴むと、カーブが一瞬消える。
    auto core = std::make_unique<CurveCore>();
    core->setParameters(*makeCurve(true, CurveParams::Logic::ArcExp));

    for (int i = 0; i < 8; ++i) {
        core->acquireForAudio();

        CHECK(core->processAudio(kPos, kTarget, kParam, 0.5f)
              == doctest::Approx(arcExp(0.5f)).epsilon(0.0001));
    }
}

// CurveCore : 添字が範囲の外でも落ちず、線形として扱う
TEST_CASE("CurveCore: an out-of-range logic index falls back to linear")
{
    auto core = std::make_unique<CurveCore>();

    auto p = makeCurve(true, CurveParams::Logic::Linear);
    p->params[(size_t)kPos][(size_t)kTarget][(size_t)kParam].logic = 9999;
    core->setParameters(*p);
    core->acquireForAudio();

    CHECK(core->processAudio(kPos, kTarget, kParam, 0.5f) == doctest::Approx(0.5f));
}
