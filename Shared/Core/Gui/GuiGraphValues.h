#pragma once

#include "Shared/Core/Synth/StepTable.h"
#include <JuceHeader.h>
#include <array>

#include "./GuiEnvelopeGraph.h"
#include "../Processor/ProcessorKeys.h"
#include "../Processor/ProcessorValues.h"

// ==========================================================
// グラフの材料を、つまみを通さずにパラメータから採る
// ==========================================================
// 区分ごとの部品を 1 組しか置かない造りでは、上に並べる小さなグラフは
// 自分のつまみを持たない。オペレータやパッドの数だけつまみを用意して
// しまうと、まとめた意味がなくなる。
//
// 値も幅も、もとはパラメータが決めている。つまみはそれを映していた
// だけなので、接頭辞さえ分かれば同じものが直に採れる。
namespace GuiGraphValues
{
    // ------------------------------------------------------
    // パラメータを引く
    // ------------------------------------------------------
    inline float value(juce::AudioProcessorValueTreeState& apvts, const juce::String& id)
    {
        auto* v = apvts.getRawParameterValue(id);

        return (v != nullptr) ? v->load() : 0.0f;
    }

    inline bool flag(juce::AudioProcessorValueTreeState& apvts, const juce::String& id)
    {
        return value(apvts, id) >= 0.5f;
    }

    // つまみが出していた上限と同じもの。幅を決めているのは範囲のほう。
    inline float maxOf(juce::AudioProcessorValueTreeState& apvts, const juce::String& id)
    {
        if (auto* p = apvts.getParameter(id)) return p->getNormalisableRange().end;

        return 1.0f;
    }

    // ------------------------------------------------------
    // AMP ENV
    // ------------------------------------------------------
    inline GuiEnvelopeGraph::AmpEnvValues ampEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        GuiEnvelopeGraph::AmpEnvValues v;

        v.stl = value(apvts, code + CPK::Adsr::stl);
        v.ar = value(apvts, code + CPK::Adsr::ar);
        v.dr = value(apvts, code + CPK::Adsr::dr);
        v.sl = value(apvts, code + CPK::Adsr::sl);
        v.rr = value(apvts, code + CPK::Adsr::rr);

        v.stlMax = maxOf(apvts, code + CPK::Adsr::stl);
        v.arMax = maxOf(apvts, code + CPK::Adsr::ar);
        v.drMax = maxOf(apvts, code + CPK::Adsr::dr);
        v.slMax = maxOf(apvts, code + CPK::Adsr::sl);
        v.rrMax = maxOf(apvts, code + CPK::Adsr::rr);

        v.kor = flag(apvts, code + CPK::Adsr::kor);
        v.endlEnable = flag(apvts, code + CPK::Adsr::endlEnable);
        v.endl = value(apvts, code + CPK::Adsr::endl);

        return v;
    }

    inline bool ampEnvBypassed(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::adsr + CPK::bypass);
    }

    // ------------------------------------------------------
    // PITCH ENV
    // ------------------------------------------------------
    inline GuiEnvelopeGraph::PitchEnvValues pitchEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        GuiEnvelopeGraph::PitchEnvValues v;

        v.ar = value(apvts, code + CPK::PitchAdsr::ar);
        v.dr = value(apvts, code + CPK::PitchAdsr::dr);
        v.rr = value(apvts, code + CPK::PitchAdsr::rr);

        v.arMax = maxOf(apvts, code + CPK::PitchAdsr::ar);
        v.drMax = maxOf(apvts, code + CPK::PitchAdsr::dr);
        v.rrMax = maxOf(apvts, code + CPK::PitchAdsr::rr);

        v.stl = value(apvts, code + CPK::PitchAdsr::stl);
        v.atl = value(apvts, code + CPK::PitchAdsr::atl);
        v.ssl = value(apvts, code + CPK::PitchAdsr::ssl);
        v.rll = value(apvts, code + CPK::PitchAdsr::rll);
        v.endlEnable = flag(apvts, code + CPK::PitchAdsr::endlEnable);
        v.endl = value(apvts, code + CPK::PitchAdsr::endl);

        return v;
    }

    inline bool pitchEnvKeep(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::PitchAdsr::keep);
    }

    // ------------------------------------------------------
    // 段のエンベロープ
    // ------------------------------------------------------
    // 256 段のエンベロープ。頭はパラメータ、段の R / L は表から引く。
    // 段の数ぶんだけ (使っている段まで) を並べる。
    inline GuiEnvelopeGraph::StepEnvValues stepEnv256(juce::AudioProcessorValueTreeState& apvts, const StepTableStore& tables,
        const juce::String& code, const juce::String& tableKey,
        const juce::String& stepsKey, const juce::String& loopKey, const juce::String& loopToKey, const juce::String& loopCountKey,
        const juce::String& stlKey, const juce::String& rrKey, const juce::String& lrKey, float levelMax)
    {
        GuiEnvelopeGraph::StepEnvValues v;

        v.head.steps = juce::jmax(1, (int)value(apvts, code + stepsKey));
        v.head.loop = flag(apvts, code + loopKey);
        v.head.loopTo = (int)value(apvts, code + loopToKey);
        v.head.loopCount = (int)value(apvts, code + loopCountKey);

        const auto* table = tables.find(code + tableKey);

        v.r.assign((size_t)v.head.steps + 1, 0.0f);
        v.l.assign((size_t)v.head.steps + 1, 0.0f);
        v.l[0] = value(apvts, code + stlKey);

        for (int s = 1; s <= v.head.steps && table != nullptr; ++s) {
            v.r[(size_t)s] = table->rate(s);
            v.l[(size_t)s] = table->level(s);
        }

        v.rr = value(apvts, code + rrKey);
        v.lr = value(apvts, code + lrKey);
        v.rMax = maxOf(apvts, code + rrKey);
        v.lMax = levelMax;

        // ENDL の鍵は LR の鍵と同じ頭を持つ (<頭>_LR / <頭>_ENDL / <頭>_ENDL_EN)
        if (lrKey.endsWith("_LR")) {
            const auto head = lrKey.dropLastCharacters(3);

            v.endlEnable = flag(apvts, code + head + "_ENDL_EN");
            v.endl = value(apvts, code + head + "_ENDL");
        }

        return v;
    }

    inline GuiEnvelopeGraph::StepEnvValues ssgSwEnv256(juce::AudioProcessorValueTreeState& apvts, const StepTableStore& tables, const juce::String& code)
    {
        return stepEnv256(apvts, tables, code, CPK::ssgSwEnv256,
            CPK::SsgSwEnv256::steps, CPK::SsgSwEnv256::loop, CPK::SsgSwEnv256::loopTo, CPK::SsgSwEnv256::loopCount,
            CPK::SsgSwEnv256::stl, CPK::SsgSwEnv256::rr, CPK::SsgSwEnv256::lr, CPV::SsgSwEnv256::L::max);
    }

    inline GuiEnvelopeGraph::StepEnvValues ssgSwPEnv256(juce::AudioProcessorValueTreeState& apvts, const StepTableStore& tables, const juce::String& code)
    {
        return stepEnv256(apvts, tables, code, CPK::ssgSwPEnv256,
            CPK::SsgSwPEnv256::steps, CPK::SsgSwPEnv256::loop, CPK::SsgSwPEnv256::loopTo, CPK::SsgSwPEnv256::loopCount,
            CPK::SsgSwPEnv256::stl, CPK::SsgSwPEnv256::rr, CPK::SsgSwPEnv256::lr, (float)CPV::SsgSwPEnv256::L::max);
    }

    inline bool ssgSwEnv256Keep(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::SsgSwEnv256::keep);
    }

    inline bool ssgSwPEnv256Keep(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::SsgSwPEnv256::keep);
    }
}
