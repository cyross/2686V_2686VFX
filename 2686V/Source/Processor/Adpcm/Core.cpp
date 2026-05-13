#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::level, code + PrName::level, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::pan, code + PrName::pan, PrValue::Pan::min, PrValue::Pan::max, PrValue::Pan::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::loop, code + PrName::loop, PrValue::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::mode, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::pcmOffset, code + PrName::pcmOffset, PrValue::Offset::min, PrValue::Offset::max, PrValue::Offset::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::pcmRatio, code + PrName::pcmRatio, PrValue::Ratio::min, PrValue::Ratio::max, PrValue::Ratio::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::adsr + PrKey::bypass, code + PrName::Adsr::bypass, PrValue::Adsr::Bypass::initial));

    addEnvParameters(layout, code);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcm.level = *apvts.getRawParameterValue(code + PrKey::level);
    params.adpcm.pan = *apvts.getRawParameterValue(code + PrKey::pan);
    params.adpcm.loop = (*apvts.getRawParameterValue(code + PrKey::loop) > PrValue::boolThread);
    params.adpcm.qualityMode = (int)*apvts.getRawParameterValue(code + PrKey::mode);
    params.adpcm.rateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);
    params.adpcm.offset = *apvts.getRawParameterValue(code + PrKey::pcmOffset);
    params.adpcm.ratio = *apvts.getRawParameterValue(code + PrKey::pcmRatio);

    params.adpcm.adsr.bypass = (*apvts.getRawParameterValue(code + PrKey::adsr + PrKey::bypass) > PrValue::boolThread);
    params.adpcm.adsr.ar = *apvts.getRawParameterValue(code + PrKey::Adsr::ar);
    params.adpcm.adsr.dr = *apvts.getRawParameterValue(code + PrKey::Adsr::dr);
    params.adpcm.adsr.sl = *apvts.getRawParameterValue(code + PrKey::Adsr::sl);
    params.adpcm.adsr.rr = *apvts.getRawParameterValue(code + PrKey::Adsr::rr);
}
