#include "./PrAdpcm.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::adpcm;

    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::level, code + PrName::Adpcm::Post::level, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::pan, code + PrName::Adpcm::Post::pan, PrValue::Pan::min, PrValue::Pan::max, PrValue::Pan::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Adpcm::loop, code + PrName::Adpcm::Post::loop, PrValue::Adpcm::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Adpcm::mode, code + PrName::Adpcm::Post::bit, PrValue::Pcm::Bit::min, PrValue::Pcm::Bit::max, PrValue::Pcm::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Adpcm::rate, code + PrName::Adpcm::Post::rate, PrValue::Pcm::Rate::min, PrValue::Pcm::Rate::max, PrValue::Pcm::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::pcmOffset, code + PrName::Adpcm::Post::pcmOffset, PrValue::Pcm::Offset::min, PrValue::Pcm::Offset::max, PrValue::Pcm::Offset::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Adpcm::pcmRatio, code + PrName::Adpcm::Post::pcmRatio, PrValue::Pcm::Ratio::min, PrValue::Pcm::Ratio::max, PrValue::Pcm::Ratio::initial));

    addEnvParameters(layout, code);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::adpcm;

    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcm.level = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::level);
    params.adpcm.pan = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::pan);
    params.adpcm.loop = (*apvts.getRawParameterValue(code + PrKey::Post::Adpcm::loop) > PrValue::boolThread);
    params.adpcm.qualityMode = (int)*apvts.getRawParameterValue(code + PrKey::Post::Adpcm::mode);
    params.adpcm.rateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Adpcm::rate);
    params.adpcm.offset = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::pcmOffset);
    params.adpcm.ratio = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::pcmRatio);

    params.adpcm.adsr.a = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::ar);
    params.adpcm.adsr.d = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::dr);
    params.adpcm.adsr.s = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::sl);
    params.adpcm.adsr.r = *apvts.getRawParameterValue(code + PrKey::Post::Adpcm::Adsr::rr);
}
