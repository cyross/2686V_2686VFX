#include "PrFx.h"

#include "../core/OpConstants.h"
#include "../core/OpValueRange.h"
#include "../core/LabelConstants.h"

void FxProcessor::prepare(double sampleRate)
{
    effects.prepare(sampleRate);
}

void FxProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // --- Bypass ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + postBypass, codeFx + masterBypassLabel, fxMBypassDefault));

    // --- Filter ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + "fl" + postBypass, "Filter Bypass", fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeFx + "flType", "Filter Type", fxFlTypeMin, fxFlTypeMax, fxFlTypeDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + "flFreq", "Filter Freq", fxFlFreqRange, fxFlFreqDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + "flQ", "Filter Q", fxFlQMin, fxFlQMax, fxFlQDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + "flMix", "Filter Mix", fxFlMixMin, fxFlMixMax, fxFlMixDefault));

    // --- Tremolo ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + codeFxTrm + postBypass, codeFx + fxTremoloLabel + fxPostBypassLabel, fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxTrm + postRate, codeFx + fxTremoloLabel + fxPostRateLabel, fxTrmRateMin, fxTrmRateMax, fxTrmRateDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxTrm + postDepth, codeFx + fxTremoloLabel + fxPostDepthLabel, fxTrmDepthMin, fxTrmDepthMax, fxTrmDepthDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxTrm + postMix, codeFx + fxTremoloLabel + fxPostMixLabel, fxMixMin, fxMixMax, fxMixDefault));

    // --- Vibrato / Detune ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + codeFxVib + postBypass, codeFx + fxVibratoLabel + fxPostBypassLabel, fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxVib + postRate, codeFx + fxVibratoLabel + fxPostRateLabel, fxVibRateMin, fxVibRateMax, fxVibRateDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxVib + postDepth, codeFx + fxVibratoLabel + fxPostDepthLabel, fxVibDepthMin, fxVibDepthMax, fxVibDepthDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxVib + postMix, codeFx + fxVibratoLabel + fxPostMixLabel, fxMixMin, fxMixMax, fxMixDefault));

    // --- Modern Bit Crusher ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + codeFxMbc + postBypass, codeFx + fxMBCLabel + fxPostBypassLabel, fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxMbc + postRate, codeFx + fxMBCLabel + fxPostRateLabel, fxMbcRateMin, fxMbcRateMax, fxMbcRateDefault)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxMbc + postFxBit, codeFx + fxMBCLabel + fxPostQualityLabel, fxMbcBitMin, fxMbcBitMax, fxMbcBitDefault)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxMbc + postMix, codeFx + fxMBCLabel + fxPostMixLabel, fxMixMin, fxMixMax, fxMixDefault));

    // --- Delay ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + codeFxDly + postBypass, codeFx + fxDelayLabel + fxPostBypassLabel, fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxDly + postFxTime, codeFx + fxDelayLabel + fxPostTimeLabel, fxDlyTimeMin, fxDlyTimeMax, fxDlyTimeDefault)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxDly + postFxFb, codeFx + fxDelayLabel + fxPostFbLabel, fxDlyFbMin, fxDlyFbMax, fxDlyFbDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxDly + postMix, codeFx + fxDelayLabel + fxPostMixLabel, fxMixMin, fxMixMax, fxMixDefault));

    // --- Reverb ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + codeFxRvb + postBypass, codeFx + fxReverbLabel + fxPostBypassLabel, fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxRvb + postFxSize, codeFx + fxReverbLabel + fxPostSizeLabel, fxRvbSizeMin, fxRvbSizeMax, fxRvbSizeDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxRvb + postFxDamp, codeFx + fxReverbLabel + fxPostDampLabel, fxRvbDampMin, fxRvbDampMax, fxRvbDampDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxRvb + postMix, codeFx + fxReverbLabel + fxPostMixLabel, fxMixMin, fxMixMax, fxMixDefault));

    // --- Retro Bit Crusher ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + codeFxRbc + postBypass, codeFx + fxRBCLabel + fxPostBypassLabel, fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeFx + codeFxRbc + postRate, codeFx + fxRBCLabel + fxPostRateLabel, fxRbcRateMin, fxRbcRateMax, fxRbcRateDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeFx + codeFxRbc + postFxBit, codeFx + fxRBCLabel + fxPostQualityLabel, fxRbcBitMin, fxRbcBitMax, fxRbcBitDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + codeFxRbc + postMix, codeFx + fxRBCLabel + fxPostMixLabel, fxMixMin, fxMixMax, fxMixDefault));

    // --- Soft Clipper (tanH) ---
    layout.add(std::make_unique<juce::AudioParameterBool>(codeFx + "sc" + postBypass, "Clipper Bypass", fxBypassDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeFx + "scMix", "Clipper Mix", fxScMixMin, fxScMixMax, fxScMixDefault));
}

void FxProcessor::processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    if (*apvts.getRawParameterValue(codeFx + postBypass) > opBoolThread)
    {
        return;
    }

    // Filter
    bool flB = *apvts.getRawParameterValue(codeFx + "fl" + postBypass) > opBoolThread;
    int flType = (int)*apvts.getRawParameterValue(codeFx + "flType");
    float flFreq = *apvts.getRawParameterValue(codeFx + "flFreq");
    float flQ = *apvts.getRawParameterValue(codeFx + "flQ");
    float flMix = *apvts.getRawParameterValue(codeFx + "flMix");
    effects.setFilterParams(flType, flFreq, flQ, flMix);

    // Vibrato
    bool vB = *apvts.getRawParameterValue(codeFx + codeFxVib + postBypass) > opBoolThread;
    float vRate = *apvts.getRawParameterValue(codeFx + codeFxVib + postRate);
    float vDepth = *apvts.getRawParameterValue(codeFx + codeFxVib + postDepth);
    float vMix = *apvts.getRawParameterValue(codeFx + codeFxVib + postMix);
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    bool tB = *apvts.getRawParameterValue(codeFx + codeFxTrm + postBypass) > opBoolThread;
    float tRate = *apvts.getRawParameterValue(codeFx + codeFxTrm + postRate);
    float tDepth = *apvts.getRawParameterValue(codeFx + codeFxTrm + postDepth);
    float tMix = *apvts.getRawParameterValue(codeFx + codeFxTrm + postMix);
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    bool mcB = *apvts.getRawParameterValue(codeFx + codeFxMbc + postBypass) > opBoolThread;
    float mbcRate = *apvts.getRawParameterValue(codeFx + codeFxMbc + postRate);
    float mbcBits = *apvts.getRawParameterValue(codeFx + codeFxMbc + postFxBit);
    float mbcMix = *apvts.getRawParameterValue(codeFx + codeFxMbc + postMix);
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    bool dB = *apvts.getRawParameterValue(codeFx + codeFxDly + postBypass) > opBoolThread;
    float dTime = *apvts.getRawParameterValue(codeFx + codeFxDly + postFxTime);
    float dFb = *apvts.getRawParameterValue(codeFx + codeFxDly + postFxFb);
    float dMix = *apvts.getRawParameterValue(codeFx + codeFxDly + postMix);
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    bool rB = *apvts.getRawParameterValue(codeFx + codeFxRvb + postBypass) > opBoolThread;
    float rSize = *apvts.getRawParameterValue(codeFx + codeFxRvb + postFxSize);
    float rDamp = *apvts.getRawParameterValue(codeFx + codeFxRvb + postFxDamp);
    float rMix = *apvts.getRawParameterValue(codeFx + codeFxRvb + postMix);

    // Retro Bit Crusher
    bool rcB = *apvts.getRawParameterValue(codeFx + codeFxRbc + postBypass) > opBoolThread;
    int rbcRate = (int)*apvts.getRawParameterValue(codeFx + codeFxRbc + postRate);
    int rbcBits = (int)*apvts.getRawParameterValue(codeFx + codeFxRbc + postFxBit);
    float rbcMix = *apvts.getRawParameterValue(codeFx + codeFxRbc + postMix);
    effects.setRetroBitCrusherParams(rbcRate, rbcBits, rbcMix);

    effects.setReverbParams(rSize, rDamp, 1.0f, rMix); // Width=1.0固定

    // Soft Clipper (tanH)
    bool scB = *apvts.getRawParameterValue(codeFx + "sc" + postBypass) > opBoolThread;
    float scMix = *apvts.getRawParameterValue(codeFx + "scMix");
    effects.setSoftClipperParams(scMix);

    // バイパス設定
    effects.setBypasses(flB, tB, vB, mcB, rcB, dB, rB, scB);

    // エフェクト処理実行
    effects.process(buffer);
}
