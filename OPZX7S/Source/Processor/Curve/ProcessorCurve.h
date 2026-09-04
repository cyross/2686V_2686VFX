#pragma once

#include <JuceHeader.h>
#include <array>
#include <algorithm>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"
#include "./ProcessorCurveValues.h"

class CurveProcessor : public PrBase {
    int rawLogic[CurvePrValue::positions][CurvePrValue::targets][CurvePrValue::params] = { 0 };
    float rawK[CurvePrValue::positions][CurvePrValue::targets][CurvePrValue::params] = { 0.0f };
    float rawValues[CurvePrValue::positions][CurvePrValue::targets][CurvePrValue::params][CurvePrValue::values] = { 0.0f };

    void updateCurveParamStructure(int p, int t, int vp); 
public:
    CurveParams m_curveParams;

    CurveProcessor();
    void init(juce::AudioProcessorValueTreeState& apvts);
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void resetProcessBlock();

    // GUI / XML 用のセッターとゲッター
    void setLogic(int p, int t, int vp, int logic);
    void setK(int p, int t, int vp, float k);
    void setValue(int p, int t, int vp, int vv, float val);

    int getLogic(int p, int t, int vp) const { return rawLogic[p][t][vp]; }
    float getK(int p, int t, int vp) const { return rawK[p][t][vp]; }
    float getValue(int p, int t, int vp, int vv) const { return rawValues[p][t][vp][vv]; }

    void saveToXml(juce::XmlElement* xml);
    void loadFromXml(juce::XmlElement* xml);
    void resetToDefault();
};
