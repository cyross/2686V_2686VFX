#include "PrOpn.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpn + postAlg, codeOpn + mPostAlgTitle, mAlgMin, mAlgMax, mAlgDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpn + postFb0, codeOpn + mPostFb0Title, mFb0Min, mFb0Max, mFb0Default));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpn + postFb2, codeOpn + mPostFb2Title, mFb2Min, mFb2Max, mFb2Default));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpn + postBit, codeOpn + mPostBitTitle, mBitMin, mBitMax, mBitDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpn + postRate, codeOpn + mPostRateTitle, mRateMin, mRateMax, mRateDefault)); // Default 6 (16kHz)

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = codeOpn + codeOp + juce::String(op);
        juce::String namePrefix = codeOpn + opLabel + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, opMulMin, opMulMax, opMulDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDtLabel, opDtMin, opDtMax, opDtDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, opArMin, opArMax, opArDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, opDrMin, opDrMax, opDrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, opSrMin, opSrMax, opSrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, opSlMin, opSlMax, opSlDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, opTlMin, opTlMax, opTlDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, opKsMin, opKsMax, opKsDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, opFixDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, opFixFreqRange, opFixFreqDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, opMaskDefault)); // OP Mask (Switch)
    }
}

void OpnProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.algorithm = (int)*apvts.getRawParameterValue(codeOpn + postAlg);
    params.feedback = *apvts.getRawParameterValue(codeOpn + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(codeOpn + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(codeOpn + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(codeOpn + postRate);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = codeOpn + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postDr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postSl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + postKs);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + postSr);
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = 0.0f;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > opBoolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false; // OPNにはAMは無い
        params.fmOp[op].vibEnable = false;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > opBoolThread);
    }
}
