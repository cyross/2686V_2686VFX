#include "PrOpna.h"

void OpnaProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPNA (YM2608) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpna + postAlg, codeOpna + mPostAlgTitle, mAlgMin, mAlgMax, mAlgDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpna + postFb0, codeOpna + mPostFb0Title, mFb0Min, mFb0Max, mFb0Default));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpna + postFb2, codeOpna + mPostFb2Title, mFb2Min, mFb2Max, mFb2Default));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpna + postBit, codeOpna + mPostBitTitle, mBitMin, mBitMax, mBitDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpna + postRate, codeOpna + mPostRateTitle, mRateMin, mRateMax, mRateDefault)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpna + postLFreq, codeOpna + mPostLfoFreq, mLfoFreqMin, mLfoFreqMax, mLfoFreqDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpna + postLPms, codeOpna + mPostLfoPms, mLfoPmsMin, mLfoPmsMax, mLfoPmsDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpna + postLAms, codeOpna + mPostLfoAms, mLfoAmsMin, mLfoAmsMax, mLfoAmsDefault));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = codeOpna + codeOp + juce::String(op);
        juce::String namePrefix = codeOpna + opLabel + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, opMulMin, opMulMax, opMulDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDtLabel, opDtMin, opDtMax, opDtDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, opArMin, opArMax, opArDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, opDrMin, opDrMax, opDrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, opSrMin, opSrMax, opSrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, opSlMin, opSlMax, opSlDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, opTlMin, opTlMax, opTlDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, opKsMin, opKsMax, opKsDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postSe, namePrefix + opPostSeLabel, opSeMin, opSeMax, opSeDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSeFreq, namePrefix + opPostSeFreqLabel, opSeFreqMin, opSeFreqMax, opSeFreqDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postAm, namePrefix + opPostAmLabel, opAmDefault)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, opFixDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, opFixFreqRange, opFixFreqDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, opMaskDefault)); // OP Mask (Switch)
    }
}

void OpnaProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.algorithm = (int)*apvts.getRawParameterValue(codeOpna + postAlg);
    params.feedback = *apvts.getRawParameterValue(codeOpna + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(codeOpna + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(codeOpna + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(codeOpna + postRate);
    params.lfoFreq = *apvts.getRawParameterValue(codeOpna + postLFreq);
    params.pms = (int)*apvts.getRawParameterValue(codeOpna + postLPms);
    params.ams = (int)*apvts.getRawParameterValue(codeOpna + postLAms);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = codeOpna + codeOp + juce::String(op);

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
        params.fmOp[op].ssgEg = (int)*apvts.getRawParameterValue(p + postSe);
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + postSeFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > opBoolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + postAm) > opBoolThread);
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > opBoolThread);
    }
}
