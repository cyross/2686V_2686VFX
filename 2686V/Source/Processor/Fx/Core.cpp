#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void FxProcessor::prepare(double sampleRate)
{
    effects.prepare(sampleRate);
}

void FxProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // --- Bypass ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::bypass, code + PrName::masterBypass, PrValue::MBypass::initial));

    // --- Filter ---
    const juce::String filterPrefix = code + PrKey::fil;
    const juce::String filterLPrefix = code + PrName::filter;
    layout.add(std::make_unique<juce::AudioParameterBool>(filterPrefix + PrKey::bypass, filterLPrefix + PrName::Filter::bypass, PrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(filterPrefix + PrKey::Filter::type, filterLPrefix + PrName::Filter::type, PrValue::Filter::Type::min, PrValue::Filter::Type::max, PrValue::Filter::Type::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Filter::freq, filterLPrefix + PrName::Filter::freq, PrValue::Filter::Freq::min, PrValue::Filter::Freq::max, PrValue::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Filter::q, filterLPrefix + PrName::Filter::q, PrValue::Filter::Q::min, PrValue::Filter::Q::max, PrValue::Filter::Q::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::mix, filterLPrefix + PrName::Filter::mix, PrValue::Mix::min, PrValue::Mix::max, PrValue::Mix::initial));

    // --- Tremolo ---
    const juce::String trmPrefix = code + PrKey::trm;
    const juce::String trmLPrefix = code + PrName::tremolo;
    layout.add(std::make_unique<juce::AudioParameterBool>(trmPrefix + PrKey::bypass, trmLPrefix + PrName::Tremolo::bypass, PrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Tremolo::rate, trmLPrefix + PrName::Tremolo::rate, PrValue::Tremolo::Rate::min, PrValue::Tremolo::Rate::max, PrValue::Tremolo::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Tremolo::depth, trmLPrefix + PrName::Tremolo::depth, PrValue::Tremolo::Depth::min, PrValue::Tremolo::Depth::max, PrValue::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::mix, trmLPrefix + PrName::Tremolo::mix, PrValue::Mix::min, PrValue::Mix::max, PrValue::Mix::initial));

    // --- Vibrato / Detune ---
    const juce::String vibPrefix = code + PrKey::vib;
    const juce::String vibLPrefix = code + PrName::vibrato;
    layout.add(std::make_unique<juce::AudioParameterBool>(vibPrefix + PrKey::bypass, vibLPrefix + PrName::Vibrato::bypass, PrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Vibrato::rate, vibLPrefix + PrName::Vibrato::rate, PrValue::Vibrato::Rate::min, PrValue::Vibrato::Rate::max, PrValue::Vibrato::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Vibrato::depth, vibLPrefix + PrName::Vibrato::depth, PrValue::Vibrato::Dept::min, PrValue::Vibrato::Dept::max, PrValue::Vibrato::Dept::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::mix, vibLPrefix + PrName::Vibrato::mix, PrValue::Mix::min, PrValue::Mix::max, PrValue::Mix::initial));

    // --- Modern Bit Crusher ---
    const juce::String mbcPrefix = code + PrKey::mbc;
    const juce::String mbcLPrefix = code + PrName::mbc;
    layout.add(std::make_unique<juce::AudioParameterBool>(mbcPrefix + PrKey::bypass, mbcLPrefix + PrName::Mbc::bypass, PrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Mbc::rate, mbcLPrefix + PrName::Mbc::rate, PrValue::Mbc::Rate::min, PrValue::Mbc::Rate::max, PrValue::Mbc::Rate::initial)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Mbc::bit, mbcLPrefix + PrName::Mbc::bit, PrValue::Mbc::Bit::min, PrValue::Mbc::Bit::max, PrValue::Mbc::Bit::initial)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::mix, mbcLPrefix + PrName::Mbc::mix, PrValue::Mix::min, PrValue::Mix::max, PrValue::Mix::initial));

    // --- Delay ---
    const juce::String dlyPrefix = code + PrKey::dly;
    const juce::String dlyLPrefix = code + PrName::delay;
    layout.add(std::make_unique<juce::AudioParameterBool>(dlyPrefix + PrKey::bypass, dlyLPrefix + PrName::Delay::bypass, PrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Delay::time, dlyLPrefix + PrName::Delay::time, PrValue::Delay::Time::min, PrValue::Delay::Time::max, PrValue::Delay::Time::initial)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Delay::fb, dlyLPrefix + PrName::Delay::fb, PrValue::Delay::Fb::min, PrValue::Delay::Fb::max, PrValue::Delay::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::mix, dlyLPrefix + PrName::Delay::mix, PrValue::Mix::min, PrValue::Mix::max, PrValue::Mix::initial));

    // --- Reverb ---
    const juce::String rvbPrefix = code + PrKey::rvb;
    const juce::String rvbLPrefix = code + PrName::reverb;
    layout.add(std::make_unique<juce::AudioParameterBool>(rvbPrefix + PrKey::bypass, rvbLPrefix + PrName::Reverb::bypass, PrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Reverb::size, rvbLPrefix + PrName::Reverb::size, PrValue::Reverb::Size::min, PrValue::Reverb::Size::max, PrValue::Reverb::Size::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Reverb::damp, rvbLPrefix + PrName::Reverb::damp, PrValue::Reverb::Damp::min, PrValue::Reverb::Damp::max, PrValue::Reverb::Damp::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::mix, rvbLPrefix + PrName::Reverb::mix, PrValue::Mix::min, PrValue::Mix::max, PrValue::Mix::initial));
}

void FxProcessor::processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    if (*apvts.getRawParameterValue(code + PrKey::bypass) > PrValue::boolThread)
    {
        return;
    }

    // Filter
    const juce::String filterPrefix = code + PrKey::fil;
    bool flB = *apvts.getRawParameterValue(filterPrefix + PrKey::bypass) > PrValue::boolThread;
    int flType = (int)*apvts.getRawParameterValue(filterPrefix + PrKey::Filter::type);
    float flFreq = *apvts.getRawParameterValue(filterPrefix + PrKey::Filter::freq);
    float flQ = *apvts.getRawParameterValue(filterPrefix + PrKey::Filter::q);
    float flMix = *apvts.getRawParameterValue(filterPrefix + PrKey::mix);
    effects.setFilterParams(flType, flFreq, flQ, flMix);

    // Vibrato
    const juce::String vibPrefix = code + PrKey::vib;
    bool vB = *apvts.getRawParameterValue(vibPrefix + PrKey::bypass) > PrValue::boolThread;
    float vRate = *apvts.getRawParameterValue(vibPrefix + PrKey::Tremolo::rate);
    float vDepth = *apvts.getRawParameterValue(vibPrefix + PrKey::Tremolo::depth);
    float vMix = *apvts.getRawParameterValue(vibPrefix + PrKey::mix);
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    const juce::String trmPrefix = code + PrKey::trm;
    bool tB = *apvts.getRawParameterValue(trmPrefix + PrKey::bypass) > PrValue::boolThread;
    float tRate = *apvts.getRawParameterValue(trmPrefix + PrKey::Vibrato::rate);
    float tDepth = *apvts.getRawParameterValue(trmPrefix + PrKey::Vibrato::depth);
    float tMix = *apvts.getRawParameterValue(trmPrefix + PrKey::mix);
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    const juce::String mbcPrefix = code + PrKey::mbc;
    bool mcB = *apvts.getRawParameterValue(mbcPrefix + PrKey::bypass) > PrValue::boolThread;
    float mbcRate = *apvts.getRawParameterValue(mbcPrefix + PrKey::Mbc::rate);
    float mbcBits = *apvts.getRawParameterValue(mbcPrefix + PrKey::Mbc::bit);
    float mbcMix = *apvts.getRawParameterValue(mbcPrefix + PrKey::mix);
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    const juce::String dlyPrefix = code + PrKey::dly;
    bool dB = *apvts.getRawParameterValue(dlyPrefix + PrKey::bypass) > PrValue::boolThread;
    float dTime = *apvts.getRawParameterValue(dlyPrefix + PrKey::Delay::time);
    float dFb = *apvts.getRawParameterValue(dlyPrefix + PrKey::Delay::fb);
    float dMix = *apvts.getRawParameterValue(dlyPrefix + PrKey::mix);
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    const juce::String rvbPrefix = code + PrKey::rvb;
    bool rB = *apvts.getRawParameterValue(rvbPrefix + PrKey::bypass) > PrValue::boolThread;
    float rSize = *apvts.getRawParameterValue(rvbPrefix + PrKey::Reverb::size);
    float rDamp = *apvts.getRawParameterValue(rvbPrefix + PrKey::Reverb::damp);
    float rMix = *apvts.getRawParameterValue(rvbPrefix + PrKey::mix);
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