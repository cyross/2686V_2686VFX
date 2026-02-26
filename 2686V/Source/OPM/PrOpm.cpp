#include "PrOpm.h"

#include "../core/OpConstants.h"
#include "../core/OpValueRange.h"
#include "../core/GuiLabels.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpm + postAlg, codeOpm + mPostAlgTitle, mAlgMin, mAlgMax, mAlgDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpm + postFb0, codeOpm + mPostFb0Title, mFb0Min, mFb0Max, mFb0Default));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpm + postBit, codeOpm + mPostBitTitle, mBitMin, mBitMax, mBitDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpm + postRate, codeOpm + mPostRateTitle, mRateMin, mRateMax, mRateDefault)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpm + postLFreq, codeOpm + mPostLfoFreq, mLfoFreqMin, mLfoFreqMax, mLfoFreqDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpm + postLPms, codeOpm + mPostLfoPms, mLfoPmsMin, mLfoPmsMax, mLfoPmsDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpm + postLAms, codeOpm + mPostLfoAms, mLfoAmsMin, mLfoAmsMax, mLfoAmsMax));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = codeOpm + codeOp + juce::String(op);
        juce::String namePrefix = codeOpm + opLabel + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, opMulMin, opMulMax, opMulDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDt1Label, opDt1Min, opDt1Max, opDt1Default));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt2, namePrefix + opPostDt2Label, opDt2Min, opDt2Max, opDt2Default));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, opArMin, opArMax, opArDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD1r, namePrefix + opPostD1rLabel, opD1rMin, opD1rMax, opD1rDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD1l, namePrefix + opPostD1lLabel, opD1lMin, opD1lMax, opD1lDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD2r, namePrefix + opPostD2rLabel, opD2rMin, opD2rMax, opD2rDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, opTlMin, opTlMax, opTlDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postSe, namePrefix + opPostSeLabel, opSeMin, opSeMax, opSeDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSeFreq, namePrefix + opPostSeFreqLabel, opSeFreqMin, opSeFreqMax, opSeFreqDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, opKsMin, opKsMax, opKsDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, opFixDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, opFixFreqRange, opFixFreqDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, opMaskDefault)); // OP Mask (Switch)
    }
}

void OpmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.algorithm = (int)*apvts.getRawParameterValue(codeOpm + postAlg);
    params.feedback = *apvts.getRawParameterValue(codeOpm + postFb0);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(codeOpm + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(codeOpm + postRate);
    params.lfoFreq = *apvts.getRawParameterValue(codeOpm + postLFreq);
    params.pms = (int)*apvts.getRawParameterValue(codeOpm + postLPms);
    params.ams = (int)*apvts.getRawParameterValue(codeOpm + postLAms);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = codeOpm + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].detune2 = (int)*apvts.getRawParameterValue(p + postDt2);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postD1r);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postD1l);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + postKs);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + postD2r);
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + postSeFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > opBoolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > opBoolThread);
    }
}
