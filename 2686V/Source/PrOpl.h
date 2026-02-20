#pragma once

#include <JuceHeader.h>
#include "SimpleEffects.h"
#include "GlobalConstants.h"
#include "OpConstants.h"
#include "FileConstants.h"
#include "PresetConstants.h"
#include "LabelConstants.h"
#include "OpValueRange.h"
#include "PrBase.h"

class OplProcessor : PrBase
{
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        // ==========================================
        // OPL (YM3526) Parameters
        // ==========================================
        layout.add(std::make_unique<juce::AudioParameterInt>(codeOpl + postAlg, codeOpl + mPostAlgTitle, mOplAlgMin, mOplAlgMax, mOplAlgDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(codeOpl + postFb0, codeOpl + mPostFb0Title, mFb0Min, mFb0Max, mFb0Default));
        layout.add(std::make_unique<juce::AudioParameterInt>(codeOpl + postBit, codeOpl + mPostBitTitle, mBitMin, mBitMax, mBitDefault));
        layout.add(std::make_unique<juce::AudioParameterInt>(codeOpl + postRate, codeOpl + mPostRateTitle, mRateMin, mRateMax, mRateDefault)); // Default 6 (16kHz)

        for (int op = 0; op < 2; ++op)
        {
            juce::String prefix = codeOpl + codeOp + juce::String(op);
            juce::String namePrefix = codeOpl + opLabel + juce::String(op + 1);

            layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, opMulMin, opMulMax, opMulDefault));
            layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDtLabel, opDtMin, opDtMax, opDtDefault));
            layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, opTlMin, opTlMax, opTlDefault));
            layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, opArMin, opArMax, opArDefault));
            layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, opDrMin, opDrMax, opDrDefault));
            layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, opSrMin, opSrMax, opSrDefault));
            layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, opSlMin, opSlMax, opSlDefault));
            layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
            layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postAm, namePrefix + opPostAmLabel, opAmDefault)); // AM Enable (Switch)
            layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postVib, namePrefix + opPostVibLabel, opVibDefault));
            layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postEgType, namePrefix + opPostEgTypeLabel, opEgTypeDefault)); // 1=Sustain, 0=Decay
            layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postKsr, namePrefix + opPostKsrLabel, opKsrDefault)); // 0=Low, 1=High
            layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKsl, namePrefix + opPostKslLabel, opKslMin, opKslMax, opKslDefault));
            layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postEg, namePrefix + opPostEgLabel, opEgMin, opEgMax, opEgDefault));
            layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, opMaskDefault)); // OP Mask (Switch)
        }
    }

    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
    {
        params.algorithm = (int)*apvts.getRawParameterValue(codeOpl + postAlg);
        params.feedback = *apvts.getRawParameterValue(codeOpl + postFb0);
        params.fmBitDepth = (int)*apvts.getRawParameterValue(codeOpl + postBit);
        params.fmRateIndex = (int)*apvts.getRawParameterValue(codeOpl + postRate);

        for (int op = 0; op < 2; ++op)
        {
            juce::String p = codeOpl + codeOp + juce::String(op);

            params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
            params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
            params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
            params.fmOp[op].decay = *apvts.getRawParameterValue(p + postDr);
            params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postSl);
            params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
            bool ksrOn = *apvts.getRawParameterValue(p + postKsr) > opBoolThread;
            params.fmOp[op].keyScale = ksrOn ? 3 : 0;
            params.fmOp[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + postKsl);
            params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
            params.fmOp[op].sustainRate = 0.0f;
            params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
            params.fmOp[op].fmSsgEgFreq = 0.0f;
            params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
            params.fmOp[op].fixedFreq = 0.0f;
            params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + postEg);
            params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + postAm) > opBoolThread);
            params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + postVib) > opBoolThread;
            params.fmOp[op].egType = *apvts.getRawParameterValue(p + postEgType) > opBoolThread;
            params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > opBoolThread);
        }
    }
};
