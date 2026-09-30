// ============================================================================
// 2686VFX: ノイズリダクション (3.6.3)
// ============================================================================
// 音源の QUALITY (PCM) にあるノイズリダクションを移したもの。
//   NR: Resample  PCM ビットクラッシャーの中。間引く前に目的のレートの
//                 ナイキストの手前で切り、折り返しを防ぐ
//   ノイズリダクション (単独の効果)
//     GATE        小さい音を 0 まで絞る
//     LPF         RATE の帯域の上端から見て高い成分を削る
// どれも既定は切れていて、これまでの音は変わらない。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <cmath>
#include <functional>

#include "Core/Processor/PluginProcessor.h"
#include "Processor/Fx/ProcessorFxKeys.h"
#include "Effect/Fx/Fx.h"

namespace
{
    constexpr double kRate = 48000.0;
    constexpr int kBlock = 480;   // 10 ミリ秒
    constexpr int kBlocks = 30;   // 0.3 秒

    constexpr float kBit32 = 1.0f;     // 32bit (丸めない)
    constexpr float kRate16k = 9.0f;   // 16kHz
    constexpr float kLinear = 1.0f;

    const juce::String pcm = FxPrKey::prefix + FxPrKey::pcm;
    const juce::String nr = FxPrKey::prefix + FxPrKey::nr;

    void setReal(AudioPlugin2686V& p, const juce::String& id, float value)
    {
        auto* param = p.apvts.getParameter(id);

        REQUIRE_MESSAGE(param != nullptr, id.toStdString());

        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    float valueOf(AudioPlugin2686V& p, const juce::String& id)
    {
        auto* param = p.apvts.getParameter(id);

        REQUIRE_MESSAGE(param != nullptr, id.toStdString());

        return param->convertFrom0to1(param->getValue());
    }

    // PCM ビットクラッシャーだけを、16kHz・32bit・線形補間・WET で入れる
    void enablePcm(AudioPlugin2686V& p)
    {
        setReal(p, pcm + FxPrKey::bypass, 0.0f);
        setReal(p, pcm + FxPrKey::Pcm::bit, kBit32);
        setReal(p, pcm + FxPrKey::Pcm::rate, kRate16k);
        setReal(p, pcm + FxPrKey::Pcm::interp, kLinear);
        setReal(p, pcm + FxPrKey::mix, 1.0f);
    }

    // ノイズリダクションだけを、RATE 16kHz・WET で入れる
    void enableNr(AudioPlugin2686V& p)
    {
        setReal(p, nr + FxPrKey::bypass, 0.0f);
        setReal(p, nr + FxPrKey::Nr::rate, kRate16k);
        setReal(p, nr + FxPrKey::mix, 1.0f);
    }

    // 設定してから正弦波を流し、最後の 1/3 の実効値を返す
    float rmsOfTail(double freq, float amplitude, const std::function<void(AudioPlugin2686V&)>& configure)
    {
        auto processor = std::make_unique<AudioPlugin2686V>();

        configure(*processor);

        processor->setPlayConfigDetails(2, 2, kRate, kBlock);
        processor->prepareToPlay(kRate, kBlock);

        juce::AudioBuffer<float> buffer(2, kBlock);
        juce::MidiBuffer midi;

        double sum = 0.0;
        int count = 0;

        for (int b = 0; b < kBlocks; ++b)
        {
            for (int i = 0; i < kBlock; ++i)
            {
                const double t = (double)(b * kBlock + i) / kRate;
                const float x = amplitude * (float)std::sin(juce::MathConstants<double>::twoPi * freq * t);

                buffer.setSample(0, i, x);
                buffer.setSample(1, i, x);
            }

            processor->processBlock(buffer, midi);

            if (b < kBlocks * 2 / 3) continue;

            for (int i = 0; i < kBlock; ++i)
            {
                const double y = buffer.getSample(0, i);

                sum += y * y;
                ++count;
            }
        }

        processor->releaseResources();

        return (float)std::sqrt(sum / juce::jmax(1, count));
    }
}

TEST_CASE("2686VFX NR: 既定ではどれも切れていて、ノイズリダクションはバイパスされている")
{
    auto processor = std::make_unique<AudioPlugin2686V>();

    CHECK(valueOf(*processor, pcm + FxPrKey::Pcm::nrResample) == 0.0f);

    CHECK(valueOf(*processor, nr + FxPrKey::bypass) == 1.0f);
    CHECK(valueOf(*processor, nr + FxPrKey::Nr::gate) == 0.0f);
    CHECK(valueOf(*processor, nr + FxPrKey::Nr::lpf) == 1.0f);   // 1: 切
}

TEST_CASE("2686VFX NR: 効果の順番の名前は noiseReduction で、最後に並ぶ")
{
    CHECK(NumEffects == 10);
    CHECK(fxTypeName((int)FxType::NoiseReduction) == "noiseReduction");
    CHECK(fxTypeFromName("noiseReduction") == (int)FxType::NoiseReduction);
}

TEST_CASE("2686VFX PCM: NR: Resample を入れると、目的のレートで表せない音が折り返さない")
{
    // 16kHz へ間引くと 8kHz より上は表せない。15kHz は 1kHz へ折り返す。
    const float plain = rmsOfTail(15000.0, 0.25f, [](AudioPlugin2686V& p) { enablePcm(p); });
    const float clean = rmsOfTail(15000.0, 0.25f, [](AudioPlugin2686V& p) {
        enablePcm(p);
        setReal(p, pcm + FxPrKey::Pcm::nrResample, 1.0f);
    });

    INFO("plain " << plain << " clean " << clean);

    // 折り返しがはっきり出ることを先に確かめておく
    CHECK(plain > 0.05f);
    CHECK(clean < plain * 0.05f);
}

TEST_CASE("2686VFX PCM: NR: Resample は通してよい低い音を削らない")
{
    const float plain = rmsOfTail(440.0, 0.25f, [](AudioPlugin2686V& p) { enablePcm(p); });
    const float clean = rmsOfTail(440.0, 0.25f, [](AudioPlugin2686V& p) {
        enablePcm(p);
        setReal(p, pcm + FxPrKey::Pcm::nrResample, 1.0f);
    });

    CHECK(clean == doctest::Approx(plain).epsilon(0.02));
}

TEST_CASE("2686VFX NR: GATE も LPF も切れていれば素通し")
{
    const float plain = rmsOfTail(6000.0, 0.25f, [](AudioPlugin2686V&) {});
    const float nrOn = rmsOfTail(6000.0, 0.25f, [](AudioPlugin2686V& p) { enableNr(p); });

    CHECK(nrOn == doctest::Approx(plain).epsilon(1.0e-6));
}

TEST_CASE("2686VFX NR: GATE を入れると、しきい値より小さい音を 0 まで絞る")
{
    // -80dBFS。しきい値の -60dBFS より小さい
    const float open = rmsOfTail(220.0, 1.0e-4f, [](AudioPlugin2686V& p) { enableNr(p); });
    const float gated = rmsOfTail(220.0, 1.0e-4f, [](AudioPlugin2686V& p) {
        enableNr(p);
        setReal(p, nr + FxPrKey::Nr::gate, 1.0f);
        setReal(p, nr + FxPrKey::Nr::gateLevel, -60.0f);
    });

    INFO("open " << open << " gated " << gated);

    CHECK(open > 5.0e-5f);
    CHECK(gated < open * 0.01f);
}

TEST_CASE("2686VFX NR: GATE はしきい値より大きい音を通す")
{
    const float open = rmsOfTail(220.0, 0.25f, [](AudioPlugin2686V& p) { enableNr(p); });
    const float gated = rmsOfTail(220.0, 0.25f, [](AudioPlugin2686V& p) {
        enableNr(p);
        setReal(p, nr + FxPrKey::Nr::gate, 1.0f);
        setReal(p, nr + FxPrKey::Nr::gateLevel, -60.0f);
    });

    CHECK(gated == doctest::Approx(open).epsilon(0.02));
}

TEST_CASE("2686VFX NR: LPF は段階が上がるほど高域を削る")
{
    // RATE 16kHz の帯域の上端 (8kHz) に近い 6kHz
    const float off = rmsOfTail(6000.0, 0.25f, [](AudioPlugin2686V& p) { enableNr(p); });
    const float light = rmsOfTail(6000.0, 0.25f, [](AudioPlugin2686V& p) {
        enableNr(p);
        setReal(p, nr + FxPrKey::Nr::lpf, 2.0f);
    });
    const float strong = rmsOfTail(6000.0, 0.25f, [](AudioPlugin2686V& p) {
        enableNr(p);
        setReal(p, nr + FxPrKey::Nr::lpf, 4.0f);
    });

    INFO("off " << off << " light " << light << " strong " << strong);

    CHECK(light < off);
    CHECK(strong < light);
    CHECK(strong < off * 0.5f);
}

TEST_CASE("2686VFX NR: LPF の切れ目は RATE で決まる")
{
    // 同じ 3kHz でも、RATE を下げるほど切れ目が下がって削られる
    auto at = [](float rateIndex) {
        return rmsOfTail(3000.0, 0.25f, [rateIndex](AudioPlugin2686V& p) {
            enableNr(p);
            setReal(p, nr + FxPrKey::Nr::rate, rateIndex);
            setReal(p, nr + FxPrKey::Nr::lpf, 4.0f);   // 強: 上端の 5 割
        });
    };

    const float at16k = at(kRate16k);
    const float at8k = at(12.0f);   // 12: 8kHz

    // 16kHz なら 4kHz、8kHz なら 2kHz から上を削る
    INFO("16k " << at16k << " 8k " << at8k);

    CHECK(at8k < at16k * 0.7f);
}
