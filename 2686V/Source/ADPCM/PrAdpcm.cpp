#include "PrAdpcm.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeAdpcm + postLevel, codeAdpcm + adpcmPostLevel, adpcmVolMin, adpcmVolMax, adpcmVolDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeAdpcm + postPan, codeAdpcm + opPostPanLabel, adpcmPanMin, adpcmPanMax, adpcmPanDefault));
    layout.add(std::make_unique<juce::AudioParameterBool>(codeAdpcm + postLoop, codeAdpcm + adpcmPostLoop, adpcmLoopDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeAdpcm + postMode, codeAdpcm + adpcmPostQuality, adpcmBitMin, adpcmBitMax, adpcmBitDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeAdpcm + postRate, codeAdpcm + mPostRateTitle, adpcmRateMin, adpcmRateMax, adpcmRateDefault));

    addEnvParameters(layout, codeAdpcm);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcmLevel = *apvts.getRawParameterValue(codeAdpcm + postLevel);
    params.adpcmPan = *apvts.getRawParameterValue(codeAdpcm + postPan);
    params.adpcmLoop = (*apvts.getRawParameterValue(codeAdpcm + postLoop) > opBoolThread);
    params.adpcmQualityMode = (int)*apvts.getRawParameterValue(codeAdpcm + postMode);
    params.adpcmRateIndex = (int)*apvts.getRawParameterValue(codeAdpcm + postRate);

    params.adpcmAdsr.a = *apvts.getRawParameterValue(codeAdpcm + postAr);
    params.adpcmAdsr.d = *apvts.getRawParameterValue(codeAdpcm + postDr);
    params.adpcmAdsr.s = *apvts.getRawParameterValue(codeAdpcm + postSl);
    params.adpcmAdsr.r = *apvts.getRawParameterValue(codeAdpcm + postRr);
}
