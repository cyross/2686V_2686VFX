#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = AdpcmPrKey::prefix;

    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::level, code + AdpcmPrName::level, AdpcmPrValue::Level::min, AdpcmPrValue::Level::max, AdpcmPrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::pan, code + AdpcmPrName::pan, AdpcmPrValue::Pan::min, AdpcmPrValue::Pan::max, AdpcmPrValue::Pan::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::loop, code + AdpcmPrName::loop, AdpcmPrValue::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::mode, code + AdpcmPrName::bit, AdpcmPrValue::Bit::min, AdpcmPrValue::Bit::max, AdpcmPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::rate, code + AdpcmPrName::rate, AdpcmPrValue::Rate::min, AdpcmPrValue::Rate::max, AdpcmPrValue::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::pcmOffset, code + AdpcmPrName::pcmOffset, AdpcmPrValue::Offset::min, AdpcmPrValue::Offset::max, AdpcmPrValue::Offset::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::pcmRatio, code + AdpcmPrName::pcmRatio, AdpcmPrValue::Ratio::min, AdpcmPrValue::Ratio::max, AdpcmPrValue::Ratio::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::adsr + AdpcmPrKey::bypass, code + AdpcmPrName::Adsr::bypass, AdpcmPrValue::Adsr::Bypass::initial));

    addEnvParameters(layout, code);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = AdpcmPrKey::prefix;

    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcm.level = *apvts.getRawParameterValue(code + AdpcmPrKey::level);
    params.adpcm.pan = *apvts.getRawParameterValue(code + AdpcmPrKey::pan);
    params.adpcm.loop = (*apvts.getRawParameterValue(code + AdpcmPrKey::loop) > AdpcmPrValue::boolThread);
    params.adpcm.qualityMode = (int)*apvts.getRawParameterValue(code + AdpcmPrKey::mode);
    params.adpcm.rateIndex = (int)*apvts.getRawParameterValue(code + AdpcmPrKey::rate);
    params.adpcm.offset = *apvts.getRawParameterValue(code + AdpcmPrKey::pcmOffset);
    params.adpcm.ratio = *apvts.getRawParameterValue(code + AdpcmPrKey::pcmRatio);

    params.adpcm.adsr.bypass = (*apvts.getRawParameterValue(code + AdpcmPrKey::adsr + AdpcmPrKey::bypass) > AdpcmPrValue::boolThread);
	params.adpcm.adsr.stl = *apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::stl);
    params.adpcm.adsr.ar = *apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::ar);
    params.adpcm.adsr.dr = *apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::dr);
    params.adpcm.adsr.sl = *apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::sl);
    params.adpcm.adsr.rr = *apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::rr);
}
