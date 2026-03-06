#include "PrAdpcm.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::adpcm;

    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::level, code + PrName::Adpcm::Post::level, PrValue::Adpcm::Vol::min, PrValue::Adpcm::Vol::max, PrValue::Adpcm::Vol::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::pan, code + PrName::Adpcm::Post::pan, PrValue::Adpcm::Pan::min, PrValue::Adpcm::Pan::max, PrValue::Adpcm::Pan::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Adpcm::loop, code + PrName::Adpcm::Post::loop, PrValue::Adpcm::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Adpcm::mode, code + PrName::Adpcm::Post::bit, PrValue::Adpcm::Bit::min, PrValue::Adpcm::Bit::max, PrValue::Adpcm::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Adpcm::rate, code + PrName::Adpcm::Post::rate, PrValue::Adpcm::Rate::min, PrValue::Adpcm::Rate::max, PrValue::Adpcm::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::pcmOffset, code + PrName::Adpcm::Post::pcmOffset, PrValue::Adpcm::PcmOffset::min, PrValue::Adpcm::PcmOffset::max, PrValue::Adpcm::PcmOffset::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::pcmRatio, code + PrName::Adpcm::Post::pcmRatio, PrValue::Adpcm::PcmRatio::min, PrValue::Adpcm::PcmRatio::max, PrValue::Adpcm::PcmRatio::initial));

    addEnvParameters(layout, code);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::adpcm;

    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcmLevel = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::level);
    params.adpcmPan = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::pan);
    params.adpcmLoop = (*apvts.getRawParameterValue(code + PrKey::Post::Adpcm::loop) > PrValue::boolThread);
    params.adpcmQualityMode = (int)*apvts.getRawParameterValue(code + PrKey::Post::Adpcm::mode);
    params.adpcmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Adpcm::rate);
    params.adpcmPcmOffset = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::pcmOffset);
    params.adpcmPcmRatio = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::pcmRatio);

    params.adpcmAdsr.a = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::ar);
    params.adpcmAdsr.d = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::dr);
    params.adpcmAdsr.s = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::sl);
    params.adpcmAdsr.r = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::rr);
}
