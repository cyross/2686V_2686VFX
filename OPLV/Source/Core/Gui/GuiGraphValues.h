#pragma once

#include <JuceHeader.h>
#include <array>

#include "./GuiEnvelopeGraph.h"
#include "../Processor/ProcessorKeys.h"

// ==========================================================
// グラフの材料を、つまみを通さずにパラメータから採る
// ==========================================================
// 区分ごとの部品を 1 組しか置かない造りでは、上に並べる小さなグラフは
// 自分のつまみを持たない。オペレータやパッドの数だけつまみを用意して
// しまうと、まとめた意味がなくなる。
//
// 値も幅も、もとはパラメータが決めている。つまみはそれを映していた
// だけなので、接頭辞さえ分かれば同じものが直に採れる。
//
// 段の名前はここに一本化してある。区分の側もここから引くこと。
// 二か所に並べると、足したときに片方だけ伸びる。
namespace GuiGraphValues
{
    // ------------------------------------------------------
    // 段ごとのパラメータ名。並びが番号と一致していることが前提。
    // ------------------------------------------------------
    namespace Keys
    {
        inline const juce::String ssgSwEnvRate[] = {
            CPK::SsgSwEnv::r1, CPK::SsgSwEnv::r2, CPK::SsgSwEnv::r3,
            CPK::SsgSwEnv::r4, CPK::SsgSwEnv::r5, CPK::SsgSwEnv::r6
        };

        // 先頭は STL。画面の対象つまみで 0 を選んだときがこれ。
        inline const juce::String ssgSwEnvLevel[] = {
            CPK::SsgSwEnv::stl, CPK::SsgSwEnv::l1, CPK::SsgSwEnv::l2, CPK::SsgSwEnv::l3,
            CPK::SsgSwEnv::l4, CPK::SsgSwEnv::l5, CPK::SsgSwEnv::l6
        };

        inline const juce::String ssgSwEnv11Rate[] = {
            CPK::SsgSwEnv11::r1, CPK::SsgSwEnv11::r2, CPK::SsgSwEnv11::r3, CPK::SsgSwEnv11::r4,
            CPK::SsgSwEnv11::r5, CPK::SsgSwEnv11::r6, CPK::SsgSwEnv11::r7, CPK::SsgSwEnv11::r8,
            CPK::SsgSwEnv11::r9, CPK::SsgSwEnv11::r10, CPK::SsgSwEnv11::r11
        };

        inline const juce::String ssgSwEnv11Level[] = {
            CPK::SsgSwEnv11::stl, CPK::SsgSwEnv11::l1, CPK::SsgSwEnv11::l2, CPK::SsgSwEnv11::l3,
            CPK::SsgSwEnv11::l4, CPK::SsgSwEnv11::l5, CPK::SsgSwEnv11::l6, CPK::SsgSwEnv11::l7,
            CPK::SsgSwEnv11::l8, CPK::SsgSwEnv11::l9, CPK::SsgSwEnv11::l10, CPK::SsgSwEnv11::l11
        };

        inline const juce::String ssgSwPEnv11Rate[] = {
            CPK::SsgSwPEnv11::r1, CPK::SsgSwPEnv11::r2, CPK::SsgSwPEnv11::r3, CPK::SsgSwPEnv11::r4,
            CPK::SsgSwPEnv11::r5, CPK::SsgSwPEnv11::r6, CPK::SsgSwPEnv11::r7, CPK::SsgSwPEnv11::r8,
            CPK::SsgSwPEnv11::r9, CPK::SsgSwPEnv11::r10, CPK::SsgSwPEnv11::r11
        };

        inline const juce::String ssgSwPEnv11Level[] = {
            CPK::SsgSwPEnv11::stl, CPK::SsgSwPEnv11::l1, CPK::SsgSwPEnv11::l2, CPK::SsgSwPEnv11::l3,
            CPK::SsgSwPEnv11::l4, CPK::SsgSwPEnv11::l5, CPK::SsgSwPEnv11::l6, CPK::SsgSwPEnv11::l7,
            CPK::SsgSwPEnv11::l8, CPK::SsgSwPEnv11::l9, CPK::SsgSwPEnv11::l10, CPK::SsgSwPEnv11::l11
        };
    }

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

        return v;
    }

    inline bool pitchEnvKeep(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::PitchAdsr::keep);
    }

    // ------------------------------------------------------
    // 段のエンベロープ
    // ------------------------------------------------------
    // 配列の並びは元のつまみの並びと同じ。R 側の [0] は使わない。
    template <size_t N>
    struct StepEnvValues {
        GuiEnvelopeGraph::StepEnvHead head;
        std::array<float, N> rVal{};
        float rMax = 1.0f;
        std::array<float, N> lVal{};
        float lMax = 1.0f;
    };

    template <size_t N>
    inline StepEnvValues<N> stepEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& code,
        const juce::String& stepsKey, const juce::String& loopKey,
        const juce::String& loopToKey, const juce::String& loopCountKey,
        const juce::String* rateKeys, const juce::String* levelKeys)
    {
        StepEnvValues<N> v;

        v.head.steps = (int)value(apvts, code + stepsKey);
        v.head.loop = flag(apvts, code + loopKey);
        v.head.loopTo = (int)value(apvts, code + loopToKey);
        v.head.loopCount = (int)value(apvts, code + loopCountKey);

        // R は 1 番から。先頭は使わないので空のままにしておく。
        for (size_t i = 0; i + 1 < N; ++i) v.rVal[i + 1] = value(apvts, code + rateKeys[i]);

        for (size_t i = 0; i < N; ++i) v.lVal[i] = value(apvts, code + levelKeys[i]);

        v.rMax = maxOf(apvts, code + rateKeys[0]);
        v.lMax = maxOf(apvts, code + levelKeys[1]);

        return v;
    }

    inline StepEnvValues<7> ssgSwEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return stepEnv<7>(apvts, code,
            CPK::SsgSwEnv::steps, CPK::SsgSwEnv::loop, CPK::SsgSwEnv::loopTo, CPK::SsgSwEnv::loopCount,
            Keys::ssgSwEnvRate, Keys::ssgSwEnvLevel);
    }

    inline StepEnvValues<12> ssgSwEnv11(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return stepEnv<12>(apvts, code,
            CPK::SsgSwEnv11::steps, CPK::SsgSwEnv11::loop, CPK::SsgSwEnv11::loopTo, CPK::SsgSwEnv11::loopCount,
            Keys::ssgSwEnv11Rate, Keys::ssgSwEnv11Level);
    }

    inline StepEnvValues<12> ssgSwPEnv11(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return stepEnv<12>(apvts, code,
            CPK::SsgSwPEnv11::steps, CPK::SsgSwPEnv11::loop, CPK::SsgSwPEnv11::loopTo, CPK::SsgSwPEnv11::loopCount,
            Keys::ssgSwPEnv11Rate, Keys::ssgSwPEnv11Level);
    }

    inline bool ssgSwEnv11Keep(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::SsgSwEnv11::keep);
    }

    inline bool ssgSwPEnv11Keep(juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
    {
        return flag(apvts, code + CPK::SsgSwPEnv11::keep);
    }
}
