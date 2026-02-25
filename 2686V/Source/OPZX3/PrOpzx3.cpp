#include "PrOpzx3.h"

#include "../core/OpConstants.h"
#include "../core/OpValueRange.h"
#include "../core/LabelConstants.h"

void Opzx3Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpzx3 + postAlg, codeOpzx3 + mPostAlgTitle, mOpxz3AlgMin, mOpxz3AlgMax, mOpxz3AlgDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpzx3 + postFb0, codeOpzx3 + mPostFb0Title, mFb0Min, mFb0Max, mFb0Default));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpzx3 + postBit, codeOpzx3 + mPostBitTitle, mBitMin, mBitMax, mBitDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpzx3 + postRate, codeOpzx3 + mPostRateTitle, mRateMin, mRateMax, mRateDefault)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpzx3 + postLFreq, codeOpzx3 + mPostLfoFreq, mLfoFreqMin, mLfoFreqMax, mLfoFreqDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpzx3 + postLPms, codeOpzx3 + mPostLfoPms, mLfoPmsMin, mLfoPmsMax, mLfoPmsDefault));
    layout.add(std::make_unique<juce::AudioParameterInt>(codeOpzx3 + postLAms, codeOpzx3 + mPostLfoAms, mLfoAmsMin, mLfoAmsMax, mLfoAmsMax));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = codeOpzx3 + codeOp + juce::String(op);
        juce::String namePrefix = codeOpzx3 + opLabel + juce::String(op + 1);

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
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postWs, namePrefix + opPostWsLabel, opWsMin, opWsMax, opWsDefault));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, opMaskDefault)); // OP Mask (Switch)
    }
}

void Opzx3Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.algorithm = (int)*apvts.getRawParameterValue(codeOpzx3 + postAlg);
    params.feedback = *apvts.getRawParameterValue(codeOpzx3 + postFb0);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(codeOpzx3 + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(codeOpzx3 + postRate);
    params.lfoFreq = *apvts.getRawParameterValue(codeOpzx3 + postLFreq);
    params.pms = (int)*apvts.getRawParameterValue(codeOpzx3 + postLPms);
    params.ams = (int)*apvts.getRawParameterValue(codeOpzx3 + postLAms);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = codeOpzx3 + codeOp + juce::String(op);

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
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + postWs);
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > opBoolThread);
    }
}
