#include "./Core.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void FxProcessor::prepare(double sampleRate)
{
    effects.prepare(sampleRate);
}

void FxProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::fx;

    // --- Bypass ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::bypass, code + PrName::Fx::masterBypass, PrValue::Fx::MBypass::initial));

    // --- Filter ---
    const juce::String filterPrefix = code + PrKey::Innder::Fx::fil;
    const juce::String filterLPrefix = code + PrName::Fx::Post::filter;
    layout.add(std::make_unique<juce::AudioParameterBool>(filterPrefix + PrKey::Post::bypass, filterLPrefix + PrName::Fx::Post::Filter::bypass, PrValue::Fx::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(filterPrefix + PrKey::Post::Fx::Filter::type, filterLPrefix + PrName::Fx::Post::Filter::type, PrValue::Fx::Filter::Type::min, PrValue::Fx::Filter::Type::max, PrValue::Fx::Filter::Type::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Post::Fx::Filter::freq, filterLPrefix + PrName::Fx::Post::Filter::freq, PrValue::Fx::Filter::Freq::min, PrValue::Fx::Filter::Freq::max, PrValue::Fx::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Post::Fx::Filter::q, filterLPrefix + PrName::Fx::Post::Filter::q, PrValue::Fx::Filter::Q::min, PrValue::Fx::Filter::Q::max, PrValue::Fx::Filter::Q::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Post::Fx::mix, filterLPrefix + PrName::Fx::Post::Filter::mix, PrValue::Fx::Mix::min, PrValue::Fx::Mix::max, PrValue::Fx::Mix::initial));

    // --- Tremolo ---
    const juce::String trmPrefix = code + PrKey::Innder::Fx::trm;
    const juce::String trmLPrefix = code + PrName::Fx::Post::tremolo;
    layout.add(std::make_unique<juce::AudioParameterBool>(trmPrefix + PrKey::Post::bypass, trmLPrefix + PrName::Fx::Post::Tremolo::bypass, PrValue::Fx::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Post::Fx::Tremolo::rate, trmLPrefix + PrName::Fx::Post::Tremolo::rate, PrValue::Fx::Tremolo::Rate::min, PrValue::Fx::Tremolo::Rate::max, PrValue::Fx::Tremolo::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Post::Fx::Tremolo::depth, trmLPrefix + PrName::Fx::Post::Tremolo::depth, PrValue::Fx::Tremolo::Depth::min, PrValue::Fx::Tremolo::Depth::max, PrValue::Fx::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Post::Fx::mix, trmLPrefix + PrName::Fx::Post::Tremolo::mix, PrValue::Fx::Mix::min, PrValue::Fx::Mix::max, PrValue::Fx::Mix::initial));

    // --- Vibrato / Detune ---
    const juce::String vibPrefix = code + PrKey::Innder::Fx::vib;
    const juce::String vibLPrefix = code + PrName::Fx::Post::vibrato;
    layout.add(std::make_unique<juce::AudioParameterBool>(vibPrefix + PrKey::Post::bypass, vibLPrefix + PrName::Fx::Post::Vibrato::bypass, PrValue::Fx::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Post::Fx::Vibrato::rate, vibLPrefix + PrName::Fx::Post::Vibrato::rate, PrValue::Fx::Vibrato::Rate::min, PrValue::Fx::Vibrato::Rate::max, PrValue::Fx::Vibrato::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Post::Fx::Vibrato::depth, vibLPrefix + PrName::Fx::Post::Vibrato::depth, PrValue::Fx::Vibrato::Dept::min, PrValue::Fx::Vibrato::Dept::max, PrValue::Fx::Vibrato::Dept::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Post::Fx::mix, vibLPrefix + PrName::Fx::Post::Vibrato::mix, PrValue::Fx::Mix::min, PrValue::Fx::Mix::max, PrValue::Fx::Mix::initial));

    // --- Modern Bit Crusher ---
    const juce::String mbcPrefix = code + PrKey::Innder::Fx::mbc;
    const juce::String mbcLPrefix = code + PrName::Fx::Post::mbc;
    layout.add(std::make_unique<juce::AudioParameterBool>(mbcPrefix + PrKey::Post::bypass, mbcLPrefix + PrName::Fx::Post::Mbc::bypass, PrValue::Fx::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Post::Fx::Mbc::rate, mbcLPrefix + PrName::Fx::Post::Mbc::rate, PrValue::Fx::Mbc::Rate::min, PrValue::Fx::Mbc::Rate::max, PrValue::Fx::Mbc::Rate::initial)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Post::Fx::Mbc::bit, mbcLPrefix + PrName::Fx::Post::Mbc::bit, PrValue::Fx::Mbc::Bit::min, PrValue::Fx::Mbc::Bit::max, PrValue::Fx::Mbc::Bit::initial)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Post::Fx::mix, mbcLPrefix + PrName::Fx::Post::Mbc::mix, PrValue::Fx::Mix::min, PrValue::Fx::Mix::max, PrValue::Fx::Mix::initial));

    // --- Delay ---
    const juce::String dlyPrefix = code + PrKey::Innder::Fx::dly;
    const juce::String dlyLPrefix = code + PrName::Fx::Post::delay;
    layout.add(std::make_unique<juce::AudioParameterBool>(dlyPrefix + PrKey::Post::bypass, dlyLPrefix + PrName::Fx::Post::Delay::bypass, PrValue::Fx::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Post::Fx::Delay::time, dlyLPrefix + PrName::Fx::Post::Delay::time, PrValue::Fx::Delay::Time::min, PrValue::Fx::Delay::Time::max, PrValue::Fx::Delay::Time::initial)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Post::Fx::Delay::fb, dlyLPrefix + PrName::Fx::Post::Delay::fb, PrValue::Fx::Delay::Fb::min, PrValue::Fx::Delay::Fb::max, PrValue::Fx::Delay::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Post::Fx::mix, dlyLPrefix + PrName::Fx::Post::Delay::mix, PrValue::Fx::Mix::min, PrValue::Fx::Mix::max, PrValue::Fx::Mix::initial));

    // --- Reverb ---
    const juce::String rvbPrefix = code + PrKey::Innder::Fx::rvb;
    const juce::String rvbLPrefix = code + PrName::Fx::Post::reverb;
    layout.add(std::make_unique<juce::AudioParameterBool>(rvbPrefix + PrKey::Post::bypass, rvbLPrefix + PrName::Fx::Post::Reverb::bypass, PrValue::Fx::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Post::Fx::Reverb::size, rvbLPrefix + PrName::Fx::Post::Reverb::size, PrValue::Fx::Reverb::Size::min, PrValue::Fx::Reverb::Size::max, PrValue::Fx::Reverb::Size::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Post::Fx::Reverb::damp, rvbLPrefix + PrName::Fx::Post::Reverb::damp, PrValue::Fx::Reverb::Damp::min, PrValue::Fx::Reverb::Damp::max, PrValue::Fx::Reverb::Damp::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Post::Fx::mix, rvbLPrefix + PrName::Fx::Post::Reverb::mix, PrValue::Fx::Mix::min, PrValue::Fx::Mix::max, PrValue::Fx::Mix::initial));
}

void FxProcessor::processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::fx;

    if (*apvts.getRawParameterValue(code + PrKey::Post::bypass) > PrValue::boolThread)
    {
        return;
    }

    // Filter
    const juce::String filterPrefix = code + PrKey::Innder::Fx::fil;
    bool flB = *apvts.getRawParameterValue(filterPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    int flType = (int)*apvts.getRawParameterValue(filterPrefix + PrKey::Post::Fx::Filter::type);
    float flFreq = *apvts.getRawParameterValue(filterPrefix + PrKey::Post::Fx::Filter::freq);
    float flQ = *apvts.getRawParameterValue(filterPrefix + PrKey::Post::Fx::Filter::q);
    float flMix = *apvts.getRawParameterValue(filterPrefix + PrKey::Post::Fx::mix);
    effects.setFilterParams(flType, flFreq, flQ, flMix);

    // Vibrato
    const juce::String vibPrefix = code + PrKey::Innder::Fx::vib;
    bool vB = *apvts.getRawParameterValue(vibPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    float vRate = *apvts.getRawParameterValue(vibPrefix + PrKey::Post::Fx::Tremolo::rate);
    float vDepth = *apvts.getRawParameterValue(vibPrefix + PrKey::Post::Fx::Tremolo::depth);
    float vMix = *apvts.getRawParameterValue(vibPrefix + PrKey::Post::Fx::mix);
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    const juce::String trmPrefix = code + PrKey::Innder::Fx::trm;
    bool tB = *apvts.getRawParameterValue(trmPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    float tRate = *apvts.getRawParameterValue(trmPrefix + PrKey::Post::Fx::Vibrato::rate);
    float tDepth = *apvts.getRawParameterValue(trmPrefix + PrKey::Post::Fx::Vibrato::depth);
    float tMix = *apvts.getRawParameterValue(trmPrefix + PrKey::Post::Fx::mix);
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    const juce::String mbcPrefix = code + PrKey::Innder::Fx::mbc;
    bool mcB = *apvts.getRawParameterValue(mbcPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    float mbcRate = *apvts.getRawParameterValue(mbcPrefix + PrKey::Post::Fx::Mbc::rate);
    float mbcBits = *apvts.getRawParameterValue(mbcPrefix + PrKey::Post::Fx::Mbc::bit);
    float mbcMix = *apvts.getRawParameterValue(mbcPrefix + PrKey::Post::Fx::mix);
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    const juce::String dlyPrefix = code + PrKey::Innder::Fx::dly;
    bool dB = *apvts.getRawParameterValue(dlyPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    float dTime = *apvts.getRawParameterValue(dlyPrefix + PrKey::Post::Fx::Delay::time);
    float dFb = *apvts.getRawParameterValue(dlyPrefix + PrKey::Post::Fx::Delay::fb);
    float dMix = *apvts.getRawParameterValue(dlyPrefix + PrKey::Post::Fx::mix);
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    const juce::String rvbPrefix = code + PrKey::Innder::Fx::rvb;
    bool rB = *apvts.getRawParameterValue(rvbPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    float rSize = *apvts.getRawParameterValue(rvbPrefix + PrKey::Post::Fx::Reverb::size);
    float rDamp = *apvts.getRawParameterValue(rvbPrefix + PrKey::Post::Fx::Reverb::damp);
    float rMix = *apvts.getRawParameterValue(rvbPrefix + PrKey::Post::Fx::mix);
    effects.setReverbParams(rSize, rDamp, 1.0f, rMix); // Width=1.0固定

    // バイパス設定
    effects.setBypasses(flB, tB, vB, mcB, dB, rB);

    // エフェクト処理実行
    effects.process(buffer);
}

void FxProcessor::clear()
{
    effects.clear();
}