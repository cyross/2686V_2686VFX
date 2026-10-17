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
        // 大域の配列にしてはいけない。材料の CPK の文字列も起動時に作られる
        // もので、どちらが先に出来るかは言語が決めていない。リンカの並べ方
        // しだいで、まだ出来ていない文字列を写し取り、起動直後に落ちる。
        // 実際に、組み直すたびに落ちたり落ちなかったりした。
        //
        // そこで並びは関数の中で作り、最初に使うとき (起動を終えたあと) に
        // 出来るようにしてある。外に置くのは作る関数を指すだけの札で、
        // これは起動時の処理を伴わない。引く側は配列と同じく keys[i] で書ける。
        class List
        {
            const juce::String* (*make)();

        public:
            constexpr explicit List(const juce::String* (*maker)()) : make(maker) {}

            const juce::String* data() const { return make(); }
            const juce::String& operator[](int i) const { return make()[i]; }
        };

        inline const juce::String* makeSsgSwEnvRate()
        {
            static const juce::String keys[] = {
                CPK::SsgSwEnv::r1, CPK::SsgSwEnv::r2, CPK::SsgSwEnv::r3,
                CPK::SsgSwEnv::r4, CPK::SsgSwEnv::r5, CPK::SsgSwEnv::r6
            };

            return keys;
        }

        inline constexpr List ssgSwEnvRate{ makeSsgSwEnvRate };

        // 先頭は STL。画面の対象つまみで 0 を選んだときがこれ。
        inline const juce::String* makeSsgSwEnvLevel()
        {
            static const juce::String keys[] = {
                CPK::SsgSwEnv::stl, CPK::SsgSwEnv::l1, CPK::SsgSwEnv::l2, CPK::SsgSwEnv::l3,
                CPK::SsgSwEnv::l4, CPK::SsgSwEnv::l5, CPK::SsgSwEnv::l6
            };

            return keys;
        }

        inline constexpr List ssgSwEnvLevel{ makeSsgSwEnvLevel };

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
            Keys::ssgSwEnvRate.data(), Keys::ssgSwEnvLevel.data());
    }

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
