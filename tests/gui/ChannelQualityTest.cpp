// ============================================================================
// チャンネルごとの QUALITY が、その音源へ届いているか
// ============================================================================
// 3.6.0 より前、WT2 のプロセッサは QUALITY を WT の側へ書き込んでいた。
// BIT も SMP.RATE も何を選んでも音に出ず、WT2 はいつも BIT 7bit /
// SMP.RATE 55.5kHz で鳴っていた。ビルドも警告も素通りし、3 つの版をまたいで
// 気づかれなかった。
//
// つまみの値が、その音源の設定へ着くところまでを見張る。プロセッサが
// 組み立てた SynthParams をそのまま読むので、音を鳴らさずに確かめられる。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Processor/ProcessorKeys.h"

#include "./GuiTestChips.h"

namespace
{
    // SynthCoreParams は 12 本で共通なので、持っていないチャンネルの入れ物も
    // ある。名前で選べば、どのプラグインでも同じ書きかたで読める。
    const QualityParams* qualityOf(const SynthParams& p, const juce::String& name)
    {
        if (name == "OPNA")      return &p.opna.quality;
        if (name == "OPN")       return &p.opn.quality;
        if (name == "OPL")       return &p.opl.quality;
        if (name == "OPL3")      return &p.opl3.quality;
        if (name == "OPM")       return &p.opm.quality;
        if (name == "OPZX7")     return &p.opzx7.quality;
        if (name == "SSG")       return &p.ssg.quality;
        if (name == "WAVETABLE") return &p.wt.quality;
        if (name == "WT2")       return &p.wt2.quality;
        if (name == "WTPLUS")    return &p.wtPlus.quality;

        return nullptr;
    }

    const QualityPcmParams* qualityPcmOf(const SynthParams& p, const juce::String& name)
    {
        if (name == "RHYTHM")  return &p.rhythm.pads[0].quality;
        if (name == "ADPCM")   return &p.adpcm.quality;
        if (name == "ADPCM+")  return &p.adpcmPlus.quality;

        return nullptr;
    }

    void setParam(AudioPlugin2686V& p, const juce::String& id, int value)
    {
        auto* param = p.apvts.getParameter(id);
        REQUIRE(param != nullptr);
        param->setValueNotifyingHost(param->convertTo0to1((float)value));
    }

    // 既定と違う値を選ぶ。端の値なら反対の端へ寄せる。
    int otherThanDefault(const juce::RangedAudioParameter& param)
    {
        const auto& range = param.getNormalisableRange();

        const int now = (int)range.convertFrom0to1(param.getDefaultValue());
        const int lo = (int)range.start;
        const int hi = (int)range.end;

        return (now == lo) ? hi : lo;
    }
}

TEST_CASE("チャンネルごとの QUALITY が、その音源の設定へ届く")
{
    auto holder = std::make_unique<AudioPlugin2686V>();
    auto& plugin = *holder;

    plugin.prepareToPlay(48000.0, 512);

    int checked = 0;

    for (int m = 0; m < (int)OscMode::Count; ++m)
    {
        const juce::String name = getModeName((OscMode)m);

        const auto* chip = GuiTestChips::byModeName(name);

        if (chip == nullptr) continue;

        const juce::String prefix(chip->prefix);

        // 持っていないチャンネルは、そもそもパラメータが無い
        const juce::String bitId = prefix + (chip->isPcm ? CPK::QualityPcm::mode : CPK::Quality::bit);

        auto* bitParam = dynamic_cast<juce::RangedAudioParameter*>(plugin.apvts.getParameter(bitId));

        if (bitParam == nullptr) continue;

        ++checked;

        INFO("チャンネル: ", name);

        auto* rateParam = dynamic_cast<juce::RangedAudioParameter*>(
            plugin.apvts.getParameter(prefix + CPK::Quality::rate));
        auto* interpParam = dynamic_cast<juce::RangedAudioParameter*>(
            plugin.apvts.getParameter(prefix + CPK::Quality::interp));

        REQUIRE(rateParam != nullptr);
        REQUIRE(interpParam != nullptr);

        const int bit = otherThanDefault(*bitParam);
        const int rate = otherThanDefault(*rateParam);
        const int interp = otherThanDefault(*interpParam);

        setParam(plugin, "MODE", m);
        setParam(plugin, bitId, bit);
        setParam(plugin, prefix + CPK::Quality::rate, rate);
        setParam(plugin, prefix + CPK::Quality::interp, interp);

        const SynthParams params = plugin.buildRenderParams();

        if (chip->isPcm)
        {
            const auto* q = qualityPcmOf(params, name);
            REQUIRE(q != nullptr);

            CHECK(q->mode == bit);
            CHECK(q->rate == rate);
            CHECK(q->interp == interp);
        }
        else
        {
            const auto* q = qualityOf(params, name);
            REQUIRE(q != nullptr);

            CHECK(q->bit == bit);
            CHECK(q->rate == rate);
            CHECK(q->interp == interp);
        }
    }

    // 1 つも見ていないなら、上の表かプラグインの側が変わっている
    CHECK(checked > 0);
}
