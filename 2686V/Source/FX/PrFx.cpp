#include "PrFx.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void FxProcessor::prepare(double sampleRate)
{
    effects.prepare(sampleRate);
}

void FxProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::fx;

    // --- Bypass ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::bypass, code + PrName::Fx::masterBypass, PrValue::Fx::MBypass::initial));

    // --- Retro LFO ---
    const juce::String rlfoPrefix = code + PrKey::Innder::Fx::rlfo;
    const juce::String rlfoLPrefix = code + PrName::Fx::Post::rlfo;
    layout.add(std::make_unique<juce::AudioParameterBool>(rlfoPrefix + PrKey::Post::bypass, rlfoLPrefix + PrName::Fx::Post::RetroLfo::bypass, PrValue::Fx::RetroLfo::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(rlfoPrefix + PrKey::Post::Fx::RLfo::wave, rlfoLPrefix + PrName::Fx::Post::RetroLfo::wave, PrValue::Fx::RetroLfo::Wave::min, PrValue::Fx::RetroLfo::Wave::max, PrValue::Fx::RetroLfo::Wave::initial)); // 0:Saw, 1:Square, 2:Tri, 3:Noise
    layout.add(std::make_unique<juce::AudioParameterFloat>(rlfoPrefix + PrKey::Post::Fx::RLfo::freq, rlfoLPrefix + PrName::Fx::Post::RetroLfo::freq, PrValue::Fx::RetroLfo::Freq::min, PrValue::Fx::RetroLfo::Freq::max, PrValue::Fx::RetroLfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rlfoPrefix + PrKey::Post::Fx::RLfo::ams, rlfoLPrefix + PrName::Fx::Post::RetroLfo::ams, PrValue::Fx::RetroLfo::Ams::min, PrValue::Fx::RetroLfo::Ams::max, PrValue::Fx::RetroLfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rlfoPrefix + PrKey::Post::Fx::RLfo::pms, rlfoLPrefix + PrName::Fx::Post::RetroLfo::pms, PrValue::Fx::RetroLfo::Pms::min, PrValue::Fx::RetroLfo::Pms::max, PrValue::Fx::RetroLfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rlfoPrefix + PrKey::Post::Fx::RLfo::amd, rlfoLPrefix + PrName::Fx::Post::RetroLfo::amd, PrValue::Fx::RetroLfo::Amd::min, PrValue::Fx::RetroLfo::Amd::max, PrValue::Fx::RetroLfo::Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rlfoPrefix + PrKey::Post::Fx::RLfo::pmd, rlfoLPrefix + PrName::Fx::Post::RetroLfo::pmd, PrValue::Fx::RetroLfo::Pmd::min, PrValue::Fx::RetroLfo::Pmd::max, PrValue::Fx::RetroLfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rlfoPrefix + PrKey::Post::Fx::mix, PrName::Fx::Post::RetroLfo::mix, PrValue::Fx::RetroLfo::Mix::min, PrValue::Fx::RetroLfo::Mix::max, PrValue::Fx::RetroLfo::Mix::initial));

    // --- Filter ---
    const juce::String filterPrefix = code + PrKey::Innder::Fx::fil;
    const juce::String filterLPrefix = code + PrName::Fx::Post::filter;
    layout.add(std::make_unique<juce::AudioParameterBool>(filterPrefix + PrKey::Post::bypass, filterLPrefix + PrName::Fx::Post::Filter::bypass, PrValue::Fx::Filter::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(filterPrefix + PrKey::Post::Fx::Filter::type, filterLPrefix + PrName::Fx::Post::Filter::type, PrValue::Fx::Filter::Type::min, PrValue::Fx::Filter::Type::max, PrValue::Fx::Filter::Type::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Post::Fx::Filter::freq, filterLPrefix + PrName::Fx::Post::Filter::freq, PrValue::Fx::Filter::Freq::min, PrValue::Fx::Filter::Freq::max, PrValue::Fx::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Post::Fx::Filter::q, filterLPrefix + PrName::Fx::Post::Filter::q, PrValue::Fx::Filter::Q::min, PrValue::Fx::Filter::Q::max, PrValue::Fx::Filter::Q::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(filterPrefix + PrKey::Post::Fx::mix, filterLPrefix + PrName::Fx::Post::Filter::mix, PrValue::Fx::Filter::Mix::min, PrValue::Fx::Filter::Mix::max, PrValue::Fx::Filter::Mix::initial));

    // --- Tremolo ---
    const juce::String trmPrefix = code + PrKey::Innder::Fx::trm;
    const juce::String trmLPrefix = code + PrName::Fx::Post::tremolo;
    layout.add(std::make_unique<juce::AudioParameterBool>(trmPrefix + PrKey::Post::bypass, trmLPrefix + PrName::Fx::Post::Tremolo::bypass, PrValue::Fx::Tremolo::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Post::Fx::Tremolo::rate, trmLPrefix + PrName::Fx::Post::Tremolo::rate, PrValue::Fx::Tremolo::Rate::min, PrValue::Fx::Tremolo::Rate::max, PrValue::Fx::Tremolo::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Post::Fx::Tremolo::depth, trmLPrefix + PrName::Fx::Post::Tremolo::depth, PrValue::Fx::Tremolo::Depth::min, PrValue::Fx::Tremolo::Depth::max, PrValue::Fx::Filter::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(trmPrefix + PrKey::Post::Fx::mix, trmLPrefix + PrName::Fx::Post::Tremolo::mix, PrValue::Fx::Tremolo::Mix::min, PrValue::Fx::Tremolo::Mix::max, PrValue::Fx::Tremolo::Mix::initial));

    // --- Vibrato / Detune ---
    const juce::String vibPrefix = code + PrKey::Innder::Fx::vib;
    const juce::String vibLPrefix = code + PrName::Fx::Post::vibrato;
    layout.add(std::make_unique<juce::AudioParameterBool>(vibPrefix + PrKey::Post::bypass, vibLPrefix + PrName::Fx::Post::Vibrato::bypass, PrValue::Fx::Vibrato::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Post::Fx::Vibrato::rate, vibLPrefix + PrName::Fx::Post::Vibrato::rate, PrValue::Fx::Vibrato::Rate::min, PrValue::Fx::Vibrato::Rate::max, PrValue::Fx::Vibrato::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Post::Fx::Vibrato::depth, vibLPrefix + PrName::Fx::Post::Vibrato::depth, PrValue::Fx::Vibrato::Dept::min, PrValue::Fx::Vibrato::Dept::max, PrValue::Fx::Vibrato::Dept::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(vibPrefix + PrKey::Post::Fx::mix, vibLPrefix + PrName::Fx::Post::Vibrato::mix, PrValue::Fx::Vibrato::Mix::min, PrValue::Fx::Vibrato::Mix::max, PrValue::Fx::Vibrato::Mix::initial));

    // --- Modern Bit Crusher ---
    const juce::String mbcPrefix = code + PrKey::Innder::Fx::mbc;
    const juce::String mbcLPrefix = code + PrName::Fx::Post::mbc;
    layout.add(std::make_unique<juce::AudioParameterBool>(mbcPrefix + PrKey::Post::bypass, mbcLPrefix + PrName::Fx::Post::Mbc::bypass, PrValue::Fx::Mbc::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Post::Fx::Mbc::rate, mbcLPrefix + PrName::Fx::Post::Mbc::rate, PrValue::Fx::Mbc::Rate::min, PrValue::Fx::Mbc::Rate::max, PrValue::Fx::Mbc::Rate::initial)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Post::Fx::Mbc::bit, mbcLPrefix + PrName::Fx::Post::Mbc::bit, PrValue::Fx::Mbc::Bit::min, PrValue::Fx::Mbc::Bit::max, PrValue::Fx::Mbc::Bit::initial)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(mbcPrefix + PrKey::Post::Fx::mix, mbcLPrefix + PrName::Fx::Post::Mbc::mix, PrValue::Fx::Mbc::Mix::min, PrValue::Fx::Mbc::Mix::max, PrValue::Fx::Mbc::Mix::initial));

    // --- Delay ---
    const juce::String dlyPrefix = code + PrKey::Innder::Fx::dly;
    const juce::String dlyLPrefix = code + PrName::Fx::Post::delay;
    layout.add(std::make_unique<juce::AudioParameterBool>(dlyPrefix + PrKey::Post::bypass, dlyLPrefix + PrName::Fx::Post::Delay::bypass, PrValue::Fx::Delay::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Post::Fx::Delay::time, dlyLPrefix + PrName::Fx::Post::Delay::time, PrValue::Fx::Delay::Time::min, PrValue::Fx::Delay::Time::max, PrValue::Fx::Delay::Time::initial)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Post::Fx::Delay::fb, dlyLPrefix + PrName::Fx::Post::Delay::fb, PrValue::Fx::Delay::Fb::min, PrValue::Fx::Delay::Fb::max, PrValue::Fx::Delay::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dlyPrefix + PrKey::Post::Fx::mix, dlyLPrefix + PrName::Fx::Post::Delay::mix, PrValue::Fx::Delay::Mix::min, PrValue::Fx::Delay::Mix::max, PrValue::Fx::Delay::Mix::initial));

    // --- Reverb ---
    const juce::String rvbPrefix = code + PrKey::Innder::Fx::rvb;
    const juce::String rvbLPrefix = code + PrName::Fx::Post::reverb;
    layout.add(std::make_unique<juce::AudioParameterBool>(rvbPrefix + PrKey::Post::bypass, rvbLPrefix + PrName::Fx::Post::Reverb::bypass, PrValue::Fx::Reverb::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Post::Fx::Reverb::size, rvbLPrefix + PrName::Fx::Post::Reverb::size, PrValue::Fx::Reverb::Size::min, PrValue::Fx::Reverb::Size::max, PrValue::Fx::Reverb::Size::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Post::Fx::Reverb::damp, rvbLPrefix + PrName::Fx::Post::Reverb::damp, PrValue::Fx::Reverb::Damp::min, PrValue::Fx::Reverb::Damp::max, PrValue::Fx::Reverb::Damp::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rvbPrefix + PrKey::Post::Fx::mix, rvbLPrefix + PrName::Fx::Post::Reverb::mix, PrValue::Fx::Reverb::Mix::min, PrValue::Fx::Reverb::Mix::max, PrValue::Fx::Reverb::Mix::initial));

    // --- Retro Bit Crusher ---
    const juce::String rbcPrefix = code + PrKey::Innder::Fx::rbc;
    const juce::String rbcLPrefix = code + PrName::Fx::Post::rbc;
    layout.add(std::make_unique<juce::AudioParameterBool>(rbcPrefix + PrKey::Post::bypass, rbcLPrefix + PrName::Fx::Post::Rbc::bypass, PrValue::Fx::Rbc::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(rbcPrefix + PrKey::Post::Fx::Rbc::rate, rbcLPrefix + PrName::Fx::Post::Rbc::rate, PrValue::Fx::Rbc::Rate::min, PrValue::Fx::Rbc::Rate::max, PrValue::Fx::Rbc::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(rbcPrefix + PrKey::Post::Fx::Rbc::bit, rbcLPrefix + PrName::Fx::Post::Rbc::bit, PrValue::Fx::Rbc::Bit::min, PrValue::Fx::Rbc::Bit::max, PrValue::Fx::Rbc::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(rbcPrefix + PrKey::Post::Fx::mix, rbcLPrefix + PrName::Fx::Post::Rbc::mix, PrValue::Fx::Rbc::Mix::min, PrValue::Fx::Rbc::Mix::max, PrValue::Fx::Rbc::Mix::initial));

    // --- Soft Clipper (tanH) ---
    const juce::String sclPrefix = code + PrKey::Innder::Fx::scl;
    const juce::String sclLPrefix = code + PrName::Fx::Post::softClipper;
    layout.add(std::make_unique<juce::AudioParameterBool>(sclPrefix + PrKey::Post::bypass, sclLPrefix + PrName::Fx::Post::SoftClipper::bypass, PrValue::Fx::SoftClipper::Bypass::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(sclPrefix + PrKey::Post::Fx::mix, sclLPrefix + PrName::Fx::Post::SoftClipper::mix, PrValue::Fx::SoftClipper::Mix::min, PrValue::Fx::SoftClipper::Mix::max, PrValue::Fx::SoftClipper::Mix::initial));
}

void FxProcessor::processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::fx;

    if (*apvts.getRawParameterValue(code + PrKey::Post::bypass) > PrValue::boolThread)
    {
        return;
    }

    // Retro LFO
    const juce::String rlfoPrefix = code + PrKey::Innder::Fx::rlfo;
    bool rlfoB = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    int rlfoWave = (int)*apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::RLfo::wave);
    float rlfoFreq = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::RLfo::freq);
    float rlfoAms = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::RLfo::ams);
    float rlfoPms = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::RLfo::pms);
    float rlfoAmd = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::RLfo::amd);
    float rlfoPmd = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::RLfo::pmd);
    float rlfoMix = *apvts.getRawParameterValue(rlfoPrefix + PrKey::Post::Fx::mix);
    effects.setRetroLfoParams(rlfoWave, rlfoFreq, rlfoAms, rlfoPms, rlfoAmd, rlfoPmd, rlfoMix);

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

    // Retro Bit Crusher
    const juce::String rbcPrefix = code + PrKey::Innder::Fx::rbc;
    bool rcB = *apvts.getRawParameterValue(rbcPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    int rbcRate = (int)*apvts.getRawParameterValue(rbcPrefix + PrKey::Post::Fx::Rbc::rate);
    int rbcBits = (int)*apvts.getRawParameterValue(rbcPrefix + PrKey::Post::Fx::Rbc::bit);
    float rbcMix = *apvts.getRawParameterValue(rbcPrefix + PrKey::Post::Fx::mix);
    effects.setRetroBitCrusherParams(rbcRate, rbcBits, rbcMix);

    effects.setReverbParams(rSize, rDamp, 1.0f, rMix); // Width=1.0固定

    // Soft Clipper (tanH)
    const juce::String sclPrefix = code + PrKey::Innder::Fx::scl;
    bool scB = *apvts.getRawParameterValue(sclPrefix + PrKey::Post::bypass) > PrValue::boolThread;
    float scMix = *apvts.getRawParameterValue(sclPrefix + PrKey::Post::Fx::mix);
    effects.setSoftClipperParams(scMix);

    // バイパス設定
    effects.setBypasses(flB, rlfoB, tB, vB, mcB, rcB, dB, rB, scB);

    // エフェクト処理実行
    effects.process(buffer);
}
