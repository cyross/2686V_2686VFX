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
    const juce::String code = FxPrKey::prefix;

    // --- Bypass ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + FxPrKey::bypass, code + FxPrName::masterBypass, FxPrValue::MBypass::initial));

    // --- Filter ---
    const juce::String filterPrefix = code + FxPrKey::fil;
    const juce::String filterLPrefix = code + FxPrName::filter;
    layout.add(std::make_unique<juce::AudioParameterBool>(filterPrefix + FxPrKey::bypass, filterLPrefix + FxPrName::Filter::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(filterPrefix + FxPrKey::Filter::type, filterLPrefix + FxPrName::Filter::type, FxPrValue::Filter::Type::min, FxPrValue::Filter::Type::max, FxPrValue::Filter::Type::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + FxPrKey::Filter::freq, filterLPrefix + FxPrName::Filter::freq, FxPrValue::Filter::Freq::min, FxPrValue::Filter::Freq::max, FxPrValue::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + FxPrKey::Filter::q, filterLPrefix + FxPrName::Filter::q, FxPrValue::Filter::Q::min, FxPrValue::Filter::Q::max, FxPrValue::Filter::Q::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + FxPrKey::mix, filterLPrefix + FxPrName::Filter::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Tremolo ---
    const juce::String trmPrefix = code + FxPrKey::trm;
    const juce::String trmLPrefix = code + FxPrName::tremolo;
    layout.add(std::make_unique<juce::AudioParameterBool>(trmPrefix + FxPrKey::bypass, trmLPrefix + FxPrName::Tremolo::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + FxPrKey::Tremolo::rate, trmLPrefix + FxPrName::Tremolo::rate, FxPrValue::Tremolo::Rate::min, FxPrValue::Tremolo::Rate::max, FxPrValue::Tremolo::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + FxPrKey::Tremolo::depth, trmLPrefix + FxPrName::Tremolo::depth, FxPrValue::Tremolo::Depth::min, FxPrValue::Tremolo::Depth::max, FxPrValue::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + FxPrKey::mix, trmLPrefix + FxPrName::Tremolo::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Vibrato / Detune ---
    const juce::String vibPrefix = code + FxPrKey::vib;
    const juce::String vibLPrefix = code + FxPrName::vibrato;
    layout.add(std::make_unique<juce::AudioParameterBool>(vibPrefix + FxPrKey::bypass, vibLPrefix + FxPrName::Vibrato::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + FxPrKey::Vibrato::rate, vibLPrefix + FxPrName::Vibrato::rate, FxPrValue::Vibrato::Rate::min, FxPrValue::Vibrato::Rate::max, FxPrValue::Vibrato::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + FxPrKey::Vibrato::depth, vibLPrefix + FxPrName::Vibrato::depth, FxPrValue::Vibrato::Dept::min, FxPrValue::Vibrato::Dept::max, FxPrValue::Vibrato::Dept::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + FxPrKey::mix, vibLPrefix + FxPrName::Vibrato::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Modern Bit Crusher ---
    const juce::String mbcPrefix = code + FxPrKey::mbc;
    const juce::String mbcLPrefix = code + FxPrName::mbc;
    layout.add(std::make_unique<juce::AudioParameterBool>(mbcPrefix + FxPrKey::bypass, mbcLPrefix + FxPrName::Mbc::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + FxPrKey::Mbc::rate, mbcLPrefix + FxPrName::Mbc::rate, FxPrValue::Mbc::Rate::min, FxPrValue::Mbc::Rate::max, FxPrValue::Mbc::Rate::initial)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + FxPrKey::Mbc::bit, mbcLPrefix + FxPrName::Mbc::bit, FxPrValue::Mbc::Bit::min, FxPrValue::Mbc::Bit::max, FxPrValue::Mbc::Bit::initial)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + FxPrKey::mix, mbcLPrefix + FxPrName::Mbc::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Delay ---
    const juce::String dlyPrefix = code + FxPrKey::dly;
    const juce::String dlyLPrefix = code + FxPrName::delay;
    layout.add(std::make_unique<juce::AudioParameterBool>(dlyPrefix + FxPrKey::bypass, dlyLPrefix + FxPrName::Delay::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + FxPrKey::Delay::time, dlyLPrefix + FxPrName::Delay::time, FxPrValue::Delay::Time::min, FxPrValue::Delay::Time::max, FxPrValue::Delay::Time::initial)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + FxPrKey::Delay::fb, dlyLPrefix + FxPrName::Delay::fb, FxPrValue::Delay::Fb::min, FxPrValue::Delay::Fb::max, FxPrValue::Delay::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + FxPrKey::mix, dlyLPrefix + FxPrName::Delay::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Reverb ---
    const juce::String rvbPrefix = code + FxPrKey::rvb;
    const juce::String rvbLPrefix = code + FxPrName::reverb;
    layout.add(std::make_unique<juce::AudioParameterBool>(rvbPrefix + FxPrKey::bypass, rvbLPrefix + FxPrName::Reverb::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + FxPrKey::Reverb::size, rvbLPrefix + FxPrName::Reverb::size, FxPrValue::Reverb::Size::min, FxPrValue::Reverb::Size::max, FxPrValue::Reverb::Size::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + FxPrKey::Reverb::damp, rvbLPrefix + FxPrName::Reverb::damp, FxPrValue::Reverb::Damp::min, FxPrValue::Reverb::Damp::max, FxPrValue::Reverb::Damp::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + FxPrKey::mix, rvbLPrefix + FxPrName::Reverb::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- 3Band EQ ---
    const juce::String eq3bPrefix = code + FxPrKey::eq3b;
    const juce::String eq3bLPrefix = code + FxPrName::eq3b;
    layout.add(std::make_unique<juce::AudioParameterBool>(eq3bPrefix + FxPrKey::bypass, eq3bLPrefix + FxPrName::Eq3b::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::lowGainDb, eq3bLPrefix + FxPrName::Eq3b::lowGainDb, FxPrValue::Eq3b::LowGainDb::min, FxPrValue::Eq3b::LowGainDb::max, FxPrValue::Eq3b::LowGainDb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::midFreq, eq3bLPrefix + FxPrName::Eq3b::midFreq, FxPrValue::Eq3b::MidFreq::min, FxPrValue::Eq3b::MidFreq::max, FxPrValue::Eq3b::MidFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::midGainDb, eq3bLPrefix + FxPrName::Eq3b::midGainDb, FxPrValue::Eq3b::MidGainDb::min, FxPrValue::Eq3b::MidGainDb::max, FxPrValue::Eq3b::MidGainDb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::highGainDb, eq3bLPrefix + FxPrName::Eq3b::highGainDb, FxPrValue::Eq3b::HighGainDb::min, FxPrValue::Eq3b::HighGainDb::max, FxPrValue::Eq3b::HighGainDb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::mix, eq3bLPrefix + FxPrName::Eq3b::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));
}

void FxProcessor::processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = FxPrKey::prefix;

    if (*apvts.getRawParameterValue(code + FxPrKey::bypass) > FxPrValue::boolThread)
    {
        return;
    }

    // Filter
    const juce::String filterPrefix = code + FxPrKey::fil;
    bool flB = *apvts.getRawParameterValue(filterPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    int flType = (int)*apvts.getRawParameterValue(filterPrefix + FxPrKey::Filter::type);
    float flFreq = *apvts.getRawParameterValue(filterPrefix + FxPrKey::Filter::freq);
    float flQ = *apvts.getRawParameterValue(filterPrefix + FxPrKey::Filter::q);
    float flMix = *apvts.getRawParameterValue(filterPrefix + FxPrKey::mix);
    effects.setFilterParams(flType, flFreq, flQ, flMix);

    // 3Band EQ
    const juce::String eq3bPrefix = code + FxPrKey::eq3b;
    bool eq3bB = *apvts.getRawParameterValue(eq3bPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    float eq3bLowGainDb = *apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::lowGainDb);
    float eq3bMidFreq = *apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::midFreq);
    float eq3bMidGainDb = *apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::midGainDb);
    float eq3bHighGainDb = *apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::highGainDb);
    float eq3bMix = *apvts.getRawParameterValue(eq3bPrefix + FxPrKey::mix);
    effects.setEq3bParams(eq3bLowGainDb, eq3bMidFreq, eq3bMidGainDb, eq3bHighGainDb, eq3bMix);

    // Vibrato
    const juce::String vibPrefix = code + FxPrKey::vib;
    bool vB = *apvts.getRawParameterValue(vibPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    float vRate = *apvts.getRawParameterValue(vibPrefix + FxPrKey::Tremolo::rate);
    float vDepth = *apvts.getRawParameterValue(vibPrefix + FxPrKey::Tremolo::depth);
    float vMix = *apvts.getRawParameterValue(vibPrefix + FxPrKey::mix);
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    const juce::String trmPrefix = code + FxPrKey::trm;
    bool tB = *apvts.getRawParameterValue(trmPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    float tRate = *apvts.getRawParameterValue(trmPrefix + FxPrKey::Vibrato::rate);
    float tDepth = *apvts.getRawParameterValue(trmPrefix + FxPrKey::Vibrato::depth);
    float tMix = *apvts.getRawParameterValue(trmPrefix + FxPrKey::mix);
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    const juce::String mbcPrefix = code + FxPrKey::mbc;
    bool mcB = *apvts.getRawParameterValue(mbcPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    float mbcRate = *apvts.getRawParameterValue(mbcPrefix + FxPrKey::Mbc::rate);
    float mbcBits = *apvts.getRawParameterValue(mbcPrefix + FxPrKey::Mbc::bit);
    float mbcMix = *apvts.getRawParameterValue(mbcPrefix + FxPrKey::mix);
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    const juce::String dlyPrefix = code + FxPrKey::dly;
    bool dB = *apvts.getRawParameterValue(dlyPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    float dTime = *apvts.getRawParameterValue(dlyPrefix + FxPrKey::Delay::time);
    float dFb = *apvts.getRawParameterValue(dlyPrefix + FxPrKey::Delay::fb);
    float dMix = *apvts.getRawParameterValue(dlyPrefix + FxPrKey::mix);
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    const juce::String rvbPrefix = code + FxPrKey::rvb;
    bool rB = *apvts.getRawParameterValue(rvbPrefix + FxPrKey::bypass) > FxPrValue::boolThread;
    float rSize = *apvts.getRawParameterValue(rvbPrefix + FxPrKey::Reverb::size);
    float rDamp = *apvts.getRawParameterValue(rvbPrefix + FxPrKey::Reverb::damp);
    float rMix = *apvts.getRawParameterValue(rvbPrefix + FxPrKey::mix);
    effects.setReverbParams(rSize, rDamp, 1.0f, rMix); // Width=1.0固定

    // バイパス設定
    effects.setBypasses(flB, eq3bB, tB, vB, mcB, dB, rB);

    // エフェクト処理実行
    effects.process(buffer);
}

void FxProcessor::clear()
{
    effects.clear();
}