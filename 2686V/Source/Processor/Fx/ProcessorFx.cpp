#include "./ProcessorFx.h"

#include "./ProcessorFxKeys.h"
#include "./ProcessorFxValues.h"
#include "./ProcessorFxNames.h"

void FxProcessor::prepare(double sampleRate)
{
    effects.prepare(sampleRate);
}

void FxProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = FxPrKey::prefix;
    const juce::String prefixName = FxPrName::prefix;

    // --- Bypass ---
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + FxPrKey::bypass, prefix + FxPrName::masterBypass, FxPrValue::MBypass::initial));

    // --- Filter ---
    const juce::String filterPrefix = prefix + FxPrKey::fil;
    const juce::String filterLPrefix = prefixName + FxPrName::filter;
    layout.add(std::make_unique<juce::AudioParameterBool>(filterPrefix + FxPrKey::bypass, filterLPrefix + FxPrName::Filter::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(filterPrefix + FxPrKey::Filter::type, filterLPrefix + FxPrName::Filter::type, FxPrValue::Filter::Type::min, FxPrValue::Filter::Type::max, FxPrValue::Filter::Type::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + FxPrKey::Filter::freq, filterLPrefix + FxPrName::Filter::freq, FxPrValue::Filter::Freq::min, FxPrValue::Filter::Freq::max, FxPrValue::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + FxPrKey::Filter::q, filterLPrefix + FxPrName::Filter::q, FxPrValue::Filter::Q::min, FxPrValue::Filter::Q::max, FxPrValue::Filter::Q::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + FxPrKey::mix, filterLPrefix + FxPrName::Filter::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Tremolo ---
    const juce::String trmPrefix = prefix + FxPrKey::trm;
    const juce::String trmLPrefix = prefixName + FxPrName::tremolo;
    layout.add(std::make_unique<juce::AudioParameterBool>(trmPrefix + FxPrKey::bypass, trmLPrefix + FxPrName::Tremolo::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + FxPrKey::Tremolo::rate, trmLPrefix + FxPrName::Tremolo::rate, FxPrValue::Tremolo::Rate::min, FxPrValue::Tremolo::Rate::max, FxPrValue::Tremolo::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + FxPrKey::Tremolo::depth, trmLPrefix + FxPrName::Tremolo::depth, FxPrValue::Tremolo::Depth::min, FxPrValue::Tremolo::Depth::max, FxPrValue::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + FxPrKey::mix, trmLPrefix + FxPrName::Tremolo::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Vibrato / Detune ---
    const juce::String vibPrefix = prefix + FxPrKey::vib;
    const juce::String vibLPrefix = prefixName + FxPrName::vibrato;
    layout.add(std::make_unique<juce::AudioParameterBool>(vibPrefix + FxPrKey::bypass, vibLPrefix + FxPrName::Vibrato::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + FxPrKey::Vibrato::rate, vibLPrefix + FxPrName::Vibrato::rate, FxPrValue::Vibrato::Rate::min, FxPrValue::Vibrato::Rate::max, FxPrValue::Vibrato::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + FxPrKey::Vibrato::depth, vibLPrefix + FxPrName::Vibrato::depth, FxPrValue::Vibrato::Dept::min, FxPrValue::Vibrato::Dept::max, FxPrValue::Vibrato::Dept::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + FxPrKey::mix, vibLPrefix + FxPrName::Vibrato::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Modern Bit Crusher ---
    const juce::String mbcPrefix = prefix + FxPrKey::mbc;
    const juce::String mbcLPrefix = prefixName + FxPrName::mbc;
    layout.add(std::make_unique<juce::AudioParameterBool>(mbcPrefix + FxPrKey::bypass, mbcLPrefix + FxPrName::Mbc::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + FxPrKey::Mbc::rate, mbcLPrefix + FxPrName::Mbc::rate, FxPrValue::Mbc::Rate::min, FxPrValue::Mbc::Rate::max, FxPrValue::Mbc::Rate::initial)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + FxPrKey::Mbc::bit, mbcLPrefix + FxPrName::Mbc::bit, FxPrValue::Mbc::Bit::min, FxPrValue::Mbc::Bit::max, FxPrValue::Mbc::Bit::initial)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + FxPrKey::mix, mbcLPrefix + FxPrName::Mbc::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Delay ---
    const juce::String dlyPrefix = prefix + FxPrKey::dly;
    const juce::String dlyLPrefix = prefixName + FxPrName::delay;
    layout.add(std::make_unique<juce::AudioParameterBool>(dlyPrefix + FxPrKey::bypass, dlyLPrefix + FxPrName::Delay::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + FxPrKey::Delay::time, dlyLPrefix + FxPrName::Delay::time, FxPrValue::Delay::Time::min, FxPrValue::Delay::Time::max, FxPrValue::Delay::Time::initial)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + FxPrKey::Delay::fb, dlyLPrefix + FxPrName::Delay::fb, FxPrValue::Delay::Fb::min, FxPrValue::Delay::Fb::max, FxPrValue::Delay::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + FxPrKey::mix, dlyLPrefix + FxPrName::Delay::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- Reverb ---
    const juce::String rvbPrefix = prefix + FxPrKey::rvb;
    const juce::String rvbLPrefix = prefixName + FxPrName::reverb;
    layout.add(std::make_unique<juce::AudioParameterBool>(rvbPrefix + FxPrKey::bypass, rvbLPrefix + FxPrName::Reverb::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + FxPrKey::Reverb::size, rvbLPrefix + FxPrName::Reverb::size, FxPrValue::Reverb::Size::min, FxPrValue::Reverb::Size::max, FxPrValue::Reverb::Size::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + FxPrKey::Reverb::damp, rvbLPrefix + FxPrName::Reverb::damp, FxPrValue::Reverb::Damp::min, FxPrValue::Reverb::Damp::max, FxPrValue::Reverb::Damp::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + FxPrKey::mix, rvbLPrefix + FxPrName::Reverb::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));

    // --- 3Band EQ ---
    const juce::String eq3bPrefix = prefix + FxPrKey::eq3b;
    const juce::String eq3bLPrefix = prefixName + FxPrName::eq3b;
    layout.add(std::make_unique<juce::AudioParameterBool>(eq3bPrefix + FxPrKey::bypass, eq3bLPrefix + FxPrName::Eq3b::bypass, FxPrValue::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::lowGainDb, eq3bLPrefix + FxPrName::Eq3b::lowGainDb, FxPrValue::Eq3b::LowGainDb::min, FxPrValue::Eq3b::LowGainDb::max, FxPrValue::Eq3b::LowGainDb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::midFreq, eq3bLPrefix + FxPrName::Eq3b::midFreq, FxPrValue::Eq3b::MidFreq::min, FxPrValue::Eq3b::MidFreq::max, FxPrValue::Eq3b::MidFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::midGainDb, eq3bLPrefix + FxPrName::Eq3b::midGainDb, FxPrValue::Eq3b::MidGainDb::min, FxPrValue::Eq3b::MidGainDb::max, FxPrValue::Eq3b::MidGainDb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::Eq3b::highGainDb, eq3bLPrefix + FxPrName::Eq3b::highGainDb, FxPrValue::Eq3b::HighGainDb::min, FxPrValue::Eq3b::HighGainDb::max, FxPrValue::Eq3b::HighGainDb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(eq3bPrefix + FxPrKey::mix, eq3bLPrefix + FxPrName::Eq3b::mix, FxPrValue::Mix::min, FxPrValue::Mix::max, FxPrValue::Mix::initial));
}

void FxProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = FxPrKey::prefix;

    pBypass = apvts.getRawParameterValue(prefix + FxPrKey::bypass);

    // Filter
    const juce::String filterPrefix = prefix + FxPrKey::fil;
    pFlBypass = apvts.getRawParameterValue(filterPrefix + FxPrKey::bypass);
    pFlType = apvts.getRawParameterValue(filterPrefix + FxPrKey::Filter::type);
    pFlFreq = apvts.getRawParameterValue(filterPrefix + FxPrKey::Filter::freq);
    pFlQ = apvts.getRawParameterValue(filterPrefix + FxPrKey::Filter::q);
    pFlMix = apvts.getRawParameterValue(filterPrefix + FxPrKey::mix);

    // 3Band EQ
    const juce::String eq3bPrefix = prefix + FxPrKey::eq3b;
    pEq3bBypass = apvts.getRawParameterValue(eq3bPrefix + FxPrKey::bypass);
    pEq3bLowGainDb = apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::lowGainDb);
    pEq3bMidFreq = apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::midFreq);
    pEq3bMidGainDb = apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::midGainDb);
    pEq3bHighGainDb = apvts.getRawParameterValue(eq3bPrefix + FxPrKey::Eq3b::highGainDb);
    pEq3bMix = apvts.getRawParameterValue(eq3bPrefix + FxPrKey::mix);

    // Vibrato
    const juce::String vibPrefix = prefix + FxPrKey::vib;
    pVBypass = apvts.getRawParameterValue(vibPrefix + FxPrKey::bypass);
    pVRate = apvts.getRawParameterValue(vibPrefix + FxPrKey::Tremolo::rate);
    pVDepth = apvts.getRawParameterValue(vibPrefix + FxPrKey::Tremolo::depth);
    pVMix = apvts.getRawParameterValue(vibPrefix + FxPrKey::mix);

    // Tremolo
    const juce::String trmPrefix = prefix + FxPrKey::trm;
    pTBypass = apvts.getRawParameterValue(trmPrefix + FxPrKey::bypass);
    pTRate = apvts.getRawParameterValue(trmPrefix + FxPrKey::Vibrato::rate);
    pTDepth = apvts.getRawParameterValue(trmPrefix + FxPrKey::Vibrato::depth);
    pTMix = apvts.getRawParameterValue(trmPrefix + FxPrKey::mix);

    // Modern Bit Crusher
    const juce::String mbcPrefix = prefix + FxPrKey::mbc;
    pMbcBypass = apvts.getRawParameterValue(mbcPrefix + FxPrKey::bypass);
    pMbcRate = apvts.getRawParameterValue(mbcPrefix + FxPrKey::Mbc::rate);
    pMbcBits = apvts.getRawParameterValue(mbcPrefix + FxPrKey::Mbc::bit);
    pMbcMix = apvts.getRawParameterValue(mbcPrefix + FxPrKey::mix);

    // Delay
    const juce::String dlyPrefix = prefix + FxPrKey::dly;
    pDBypass = apvts.getRawParameterValue(dlyPrefix + FxPrKey::bypass);
    pDTime = apvts.getRawParameterValue(dlyPrefix + FxPrKey::Delay::time);
    pDFb = apvts.getRawParameterValue(dlyPrefix + FxPrKey::Delay::fb);
    pDMix = apvts.getRawParameterValue(dlyPrefix + FxPrKey::mix);

    // Reverb
    const juce::String rvbPrefix = prefix + FxPrKey::rvb;
    pRBypass = apvts.getRawParameterValue(rvbPrefix + FxPrKey::bypass);
    pRSize = apvts.getRawParameterValue(rvbPrefix + FxPrKey::Reverb::size);
    pRDamp = apvts.getRawParameterValue(rvbPrefix + FxPrKey::Reverb::damp);
    pRMix = apvts.getRawParameterValue(rvbPrefix + FxPrKey::mix);
}

void FxProcessor::processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    if (pBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread)
    {
        return;
    }

    // Filter
    bool flB = pFlBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    int flType = (int)pFlType->load(std::memory_order_relaxed);
    float flFreq = pFlFreq->load(std::memory_order_relaxed);
    float flQ = pFlQ->load(std::memory_order_relaxed);
    float flMix = pFlMix->load(std::memory_order_relaxed);
    effects.setFilterParams(flType, flFreq, flQ, flMix);

    // 3Band EQ
    bool eq3bB = pEq3bBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    float eq3bLowGainDb = pEq3bLowGainDb->load(std::memory_order_relaxed);
    float eq3bMidFreq = pEq3bMidFreq->load(std::memory_order_relaxed);
    float eq3bMidGainDb = pEq3bMidGainDb->load(std::memory_order_relaxed);
    float eq3bHighGainDb = pEq3bHighGainDb->load(std::memory_order_relaxed);
    float eq3bMix = pEq3bMix->load(std::memory_order_relaxed);
    effects.setEq3bParams(eq3bLowGainDb, eq3bMidFreq, eq3bMidGainDb, eq3bHighGainDb, eq3bMix);

    // Vibrato
    bool vB = pVBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    float vRate = pVRate->load(std::memory_order_relaxed);
    float vDepth = pVDepth->load(std::memory_order_relaxed);
    float vMix = pVMix->load(std::memory_order_relaxed);
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    bool tB = pTBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    float tRate = pTRate->load(std::memory_order_relaxed);
    float tDepth = pTDepth->load(std::memory_order_relaxed);
    float tMix = pTMix->load(std::memory_order_relaxed);
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    bool mcB = pMbcBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    float mbcRate = pMbcRate->load(std::memory_order_relaxed);
    float mbcBits = pMbcBits->load(std::memory_order_relaxed);
    float mbcMix = pMbcMix->load(std::memory_order_relaxed);
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    bool dB = pDBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    float dTime = pDTime->load(std::memory_order_relaxed);
    float dFb = pDFb->load(std::memory_order_relaxed);
    float dMix = pDMix->load(std::memory_order_relaxed);
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    bool rB = pRBypass->load(std::memory_order_relaxed) > FxPrValue::boolThread;
    float rSize = pRSize->load(std::memory_order_relaxed);
    float rDamp = pRDamp->load(std::memory_order_relaxed);
    float rMix = pRMix->load(std::memory_order_relaxed);
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