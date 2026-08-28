#include "doctest/doctest.h"

#include <cmath>

#include "Generator/WtMod/GenWtModulator.h"

namespace
{
    WtModParams makeParams(int shape, float depth = 0.5f, float speed = 1.0f, bool enable = true)
    {
        WtModParams p;
        p.enable = enable;
        p.depth = depth;
        p.speed = speed;
        p.shape = shape;
        return p;
    }
}

// WtModulator : 無効なら比は 1.0 のまま
TEST_CASE("WtModulator: disabled keeps the ratio at 1.0")
{
    WtModulator m;
    m.setParameters(makeParams((int)WtModShape::Sine, 0.5f, 1.0f, false));

    for (int i = 0; i < 1000; ++i) {
        CHECK(m.process(0.01f) == doctest::Approx(1.0f));
    }
}

// WtModulator : モジュレーションホイールだけでも効く
TEST_CASE("WtModulator: the mod wheel alone drives it")
{
    // Enable が off でもホイールを上げれば掛かる (WT の従来仕様)。
    WtModulator m;
    m.setParameters(makeParams((int)WtModShape::Sine, 0.5f, 1.0f, false));
    m.setModWheel(1.0f);

    bool moved = false;

    for (int i = 0; i < 1000; ++i) {
        if (std::fabs(m.process(0.01f) - 1.0f) > 1.0e-4f) moved = true;
    }

    CHECK(moved);
}

// WtModulator : 位相 0 では比が 1.0 (発音の頭で音程が飛ばない)
TEST_CASE("WtModulator: ratio is 1.0 at phase 0")
{
    for (int shape = 0; shape <= (int)WtModShape::FdsUser; ++shape) {
        if (shape == (int)WtModShape::HuC6280Wave) continue;   // 波形が空だと 1.0 固定

        WtModulator m;
        m.setParameters(makeParams(shape));

        INFO("shape = " << shape);

        CHECK(m.process(0.01f) == doctest::Approx(1.0f).epsilon(0.001));
    }
}

// WtModulator : 全 Shape が有限で正の比を返す
TEST_CASE("WtModulator: every shape returns a finite positive ratio")
{
    // 比が 0 以下になると位相が止まる / 逆行するので、そこを守る。
    for (int shape = 0; shape <= (int)WtModShape::FdsUser; ++shape) {
        WtModulator m;
        m.setParameters(makeParams(shape, 1.0f, 1.0f));

        INFO("shape = " << shape);

        for (int i = 0; i < 20000; ++i) {
            float r = m.process(0.01f);

            REQUIRE(std::isfinite(r));
            REQUIRE(r >= 0.0f);
        }
    }
}

// WtModulator : FDS は周波数側、WonderSwan と HuC6280 は分周器側
TEST_CASE("WtModulator: FDS drives frequency, WonderSwan drives the divider")
{
    // FDS は freq * (1 + temp/64) なので比が 1 より大きくなる局面がある。
    // WonderSwan と HuC6280 は分周器を動かすので比は逆数側に効く。
    WtModulator fds;
    fds.setParameters(makeParams((int)WtModShape::FdsTriangle, 0.5f));

    float fdsMax = 0.0f;

    for (int i = 0; i < 20000; ++i) {
        fdsMax = std::fmax(fdsMax, fds.process(0.01f));
    }

    CHECK(fdsMax > 1.0f);

    WtModulator ws;
    ws.setParameters(makeParams((int)WtModShape::WsSweepDown, 0.5f));

    float wsMax = 0.0f;

    for (int i = 0; i < 20000; ++i) {
        wsMax = std::fmax(wsMax, ws.process(0.01f));
    }

    CHECK(wsMax <= 1.0f + 1.0e-4f);   // 下降スイープは 1.0 を超えない
}

// WtModulator : reset で位相が戻る
TEST_CASE("WtModulator: reset rewinds the phase")
{
    WtModulator a;
    a.setParameters(makeParams((int)WtModShape::FdsSaw, 0.5f));

    WtModulator b;
    b.setParameters(makeParams((int)WtModShape::FdsSaw, 0.5f));

    for (int i = 0; i < 137; ++i) a.process(0.01f);

    a.reset();

    for (int i = 0; i < 500; ++i) {
        REQUIRE(a.process(0.01f) == doctest::Approx(b.process(0.01f)));
    }
}

// WtModulator : Depth 0 に近いほど振れ幅が小さい
TEST_CASE("WtModulator: smaller depth gives a smaller swing")
{
    auto swing = [](float depth) {
        WtModulator m;
        m.setParameters(makeParams((int)WtModShape::Sine, depth));

        float mn = 1.0e9f;
        float mx = -1.0e9f;

        for (int i = 0; i < 20000; ++i) {
            float r = m.process(0.01f);
            mn = std::fmin(mn, r);
            mx = std::fmax(mx, r);
        }

        return mx - mn;
    };

    CHECK(swing(0.1f) < swing(0.5f));
    CHECK(swing(0.5f) < swing(1.0f));
}

// WtModulator : FdsUser はテーブルどおりに動く
TEST_CASE("WtModulator: FdsUser follows the edited table")
{
    // 平坦なテーブル (全部 +0) なら変調は掛からない。
    WtModParams flat = makeParams((int)WtModShape::FdsUser, 1.0f);
    flat.fdsTable.fill(0);

    WtModulator m;
    m.setParameters(flat);

    for (int i = 0; i < 5000; ++i) {
        CHECK(m.process(0.01f) == doctest::Approx(1.0f));
    }

    // 三角テーブルを入れれば振れる
    WtModParams tri = makeParams((int)WtModShape::FdsUser, 1.0f);
    tri.fdsTable = FdsMod::tables[0];

    WtModulator m2;
    m2.setParameters(tri);

    float mx = 0.0f;

    for (int i = 0; i < 20000; ++i) {
        mx = std::fmax(mx, std::fabs(m2.process(0.01f) - 1.0f));
    }

    CHECK(mx > 0.1f);
}
