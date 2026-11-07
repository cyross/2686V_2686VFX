// ============================================================================
// レベルメーターへ渡す音 (4.4.0)
// ============================================================================
//   ・メーターを出していない (active が切) あいだは何も書かない
//   ・L / R のピークは、前に読んでからいちばん大きかった値。読むと 0 に戻る
//   ・スペクトラム用の輪のバッファは、左右を混ぜた最新の音を古い順に返す
#include "doctest/doctest.h"

#include <array>
#include <vector>

#include "Shared/Core/Synth/LevelMeterFeed.h"

TEST_CASE("LevelMeterFeed: 出していないあいだは書かない")
{
    LevelMeterFeed feed;

    const std::array<float, 4> left{ 0.5f, -0.9f, 0.2f, 0.1f };

    feed.push(left.data(), nullptr, (int)left.size(), 48000.0);

    CHECK(feed.takePeakL() == 0.0f);
    CHECK(feed.takePeakR() == 0.0f);
}

TEST_CASE("LevelMeterFeed: ピークは読むまでの最大で、読むと 0 に戻る")
{
    LevelMeterFeed feed;
    feed.active = true;

    const std::array<float, 4> left{ 0.5f, -0.9f, 0.2f, 0.1f };
    const std::array<float, 4> right{ 0.1f, 0.3f, -0.4f, 0.2f };

    feed.push(left.data(), right.data(), 4, 48000.0);

    const std::array<float, 2> quietL{ 0.1f, 0.1f };
    feed.push(quietL.data(), quietL.data(), 2, 48000.0);

    CHECK(feed.takePeakL() == doctest::Approx(0.9f));
    CHECK(feed.takePeakR() == doctest::Approx(0.4f));

    CHECK(feed.takePeakL() == 0.0f);
    CHECK(feed.sampleRate.load() == doctest::Approx(48000.0));
}

TEST_CASE("LevelMeterFeed: 輪のバッファは左右を混ぜた最新の音を古い順に返す")
{
    LevelMeterFeed feed;
    feed.active = true;

    // 1 周より多く書いて、折り返しをまたがせる
    std::vector<float> left((size_t)LevelMeterFeed::ringSize + 10);
    std::vector<float> right(left.size());

    for (size_t i = 0; i < left.size(); ++i) {
        left[i] = (float)i;
        right[i] = (float)i + 2.0f;
    }

    feed.push(left.data(), right.data(), (int)left.size(), 44100.0);

    std::array<float, 4> latest{};
    feed.copyLatest(latest.data(), 4);

    const float last = (float)(left.size() - 1);

    CHECK(latest[0] == doctest::Approx(last - 3.0f + 1.0f));
    CHECK(latest[3] == doctest::Approx(last + 1.0f));
}
