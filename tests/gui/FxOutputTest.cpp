// ============================================================================
// 2686VFX: 出力の音量 (LEVEL) と全体のバイパス
// ============================================================================
// 3.6.1 まで、2686VFX は入力を 1/4 (−12dB) に絞っていた。音源と同じ
// 「ヘッドルーム」を掛けていたためで、効果を何も入れなくても出力が小さかった。
//
// 全体のバイパスは効果の中だけで見ていたので、その手前のヘッドルームと変調は
// 掛かったままだった。バイパスしても音が変わっていた。
//
// どちらも、ビルドも警告も素通りして、鳴らして初めて分かった。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <cmath>
#include <functional>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Processor/Fx/ProcessorFxKeys.h"

namespace
{
    constexpr double kRate = 48000.0;
    constexpr int kBlock = 480;   // 10 ミリ秒
    constexpr int kBlocks = 20;   // 0.2 秒

    void setReal(AudioPlugin2686V& p, const juce::String& id, float value)
    {
        auto* param = p.apvts.getParameter(id);

        REQUIRE_MESSAGE(param != nullptr, id.toStdString());

        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    float input(int sample)
    {
        const double t = (double)sample / kRate;

        return 0.25f * (float)std::sin(juce::MathConstants<double>::twoPi * 220.0 * t);
    }

    // 設定してから立ち上げ、220Hz の正弦波を流す。最後のブロックについて、
    // 出力 ÷ 入力の比の最小と最大を返す (入力が小さいところは見ない)。
    std::pair<float, float> gainOfLastBlock(const std::function<void(AudioPlugin2686V&)>& configure)
    {
        auto processor = std::make_unique<AudioPlugin2686V>();

        configure(*processor);

        processor->setPlayConfigDetails(2, 2, kRate, kBlock);
        processor->prepareToPlay(kRate, kBlock);

        juce::AudioBuffer<float> buffer(2, kBlock);

        float low = 1.0e9f;
        float high = -1.0e9f;

        for (int b = 0; b < kBlocks; ++b)
        {
            for (int i = 0; i < kBlock; ++i)
            {
                buffer.setSample(0, i, input(b * kBlock + i));
                buffer.setSample(1, i, input(b * kBlock + i));
            }

            juce::MidiBuffer midi;

            processor->processBlock(buffer, midi);

            if (b != kBlocks - 1) continue;

            for (int i = 0; i < kBlock; ++i)
            {
                const float in = input(b * kBlock + i);

                if (std::abs(in) < 0.05f) continue;

                const float ratio = buffer.getSample(0, i) / in;

                low = juce::jmin(low, ratio);
                high = juce::jmax(high, ratio);
            }
        }

        processor->releaseResources();

        return { low, high };
    }

    // トレモロを深く掛ける。入っていれば出力の大きさが揺れる。
    void enableTremolo(AudioPlugin2686V& p)
    {
        const juce::String trm = FxPrKey::prefix + FxPrKey::trm;

        setReal(p, trm + FxPrKey::bypass, 0.0f);
        setReal(p, trm + FxPrKey::Tremolo::depth, 1.0f);
        setReal(p, trm + FxPrKey::mix, 1.0f);
    }
}

TEST_CASE("2686VFX: 何も効かせていなければ、入ってきた音がそのままの大きさで出る")
{
    const auto [low, high] = gainOfLastBlock([](AudioPlugin2686V&) {});

    CHECK(low == doctest::Approx(1.0f).epsilon(1.0e-4));
    CHECK(high == doctest::Approx(1.0f).epsilon(1.0e-4));
}

TEST_CASE("2686VFX: LEVEL の倍率で出力の大きさが変わる")
{
    const auto [low, high] = gainOfLastBlock([](AudioPlugin2686V& p) {
        setReal(p, FxPrKey::prefix + CPK::level, 2.0f);
    });

    CHECK(low == doctest::Approx(2.0f).epsilon(1.0e-4));
    CHECK(high == doctest::Approx(2.0f).epsilon(1.0e-4));
}

TEST_CASE("2686VFX: 全体のバイパスは、効果も LEVEL も通さず素通しにする")
{
    // 効かせると音が変わることを先に確かめておく。変わらない設定で
    // 素通しを確かめても意味が無い。
    {
        const auto [low, high] = gainOfLastBlock([](AudioPlugin2686V& p) {
            enableTremolo(p);
            setReal(p, FxPrKey::prefix + CPK::level, 2.0f);
        });

        CHECK((high - low) > 0.1f);
    }

    const auto [low, high] = gainOfLastBlock([](AudioPlugin2686V& p) {
        enableTremolo(p);
        setReal(p, FxPrKey::prefix + CPK::level, 2.0f);
        setReal(p, FxPrKey::prefix + FxPrKey::bypass, 1.0f);
    });

    CHECK(low == doctest::Approx(1.0f).epsilon(1.0e-6));
    CHECK(high == doctest::Approx(1.0f).epsilon(1.0e-6));
}

// 3.6.2 まで、2686VFX は reset() を持たず、残響は「無い」と申告していた。
// Cubase は入力が無音になると処理を止めるので、再生を止めると DELAY などが
// 中身を持ったまま凍り、動き出したときに一瞬鳴っていた。
TEST_CASE("2686VFX: 残響を尽きないものとして申告する")
{
    auto processor = std::make_unique<AudioPlugin2686V>();

    CHECK(std::isinf(processor->getTailLengthSeconds()));
}

TEST_CASE("2686VFX: reset() で溜めてある残響を捨てる")
{
    auto processor = std::make_unique<AudioPlugin2686V>();

    const juce::String dly = FxPrKey::prefix + FxPrKey::dly;

    setReal(*processor, dly + FxPrKey::bypass, 0.0f);
    setReal(*processor, dly + FxPrKey::Delay::fb, 0.8f);
    setReal(*processor, dly + FxPrKey::mix, 1.0f);

    processor->setPlayConfigDetails(2, 2, kRate, kBlock);
    processor->prepareToPlay(kRate, kBlock);

    juce::AudioBuffer<float> buffer(2, kBlock);
    juce::MidiBuffer midi;

    auto peakOfSilence = [&](int blocks) {
        float peak = 0.0f;

        for (int b = 0; b < blocks; ++b)
        {
            buffer.clear();
            processor->processBlock(buffer, midi);
            peak = juce::jmax(peak, buffer.getMagnitude(0, 0, kBlock));
        }

        return peak;
    };

    auto feed = [&] {
        for (int b = 0; b < kBlocks; ++b)
        {
            for (int i = 0; i < kBlock; ++i)
            {
                buffer.setSample(0, i, input(b * kBlock + i));
                buffer.setSample(1, i, input(b * kBlock + i));
            }

            processor->processBlock(buffer, midi);
        }
    };

    // 残響が残ることを先に確かめておく。残らない設定で消えたことを
    // 確かめても意味が無い。
    feed();
    CHECK(peakOfSilence(kBlocks * 2) > 0.01f);

    feed();
    processor->reset();
    CHECK(peakOfSilence(kBlocks * 2) < 1.0e-6f);

    processor->releaseResources();
}
