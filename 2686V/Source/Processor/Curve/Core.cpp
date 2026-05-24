#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void CurveProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = CurvePrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterBool>(code + CurvePrKey::enable, code + CurvePrName::enable, CurvePrValue::Enable::initial));

    for (int p = 0; p < CurvePrValue::positions; p++) {
        const juce::String pCode = code + CurvePrKey::position[p];
        const juce::String pName = code + CurvePrName::position[p];

        for (int t = 0; t < CurvePrValue::targets; t++) {
            const juce::String tCode = pCode + CurvePrKey::target[t];
            const juce::String tName = pName + CurvePrName::target[t];

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                const juce::String vpCode = tCode + CurvePrKey::paramList[vp];
                const juce::String vpName = tName + CurvePrName::paramList[vp];

                layout.add(std::make_unique<juce::AudioParameterInt>(vpCode + CurvePrKey::logic, vpName + CurvePrName::logic, CurvePrValue::Logic::min, CurvePrValue::Logic::max, CurvePrValue::Logic::initial));
                layout.add(std::make_unique<juce::AudioParameterFloat>(vpCode + CurvePrKey::k, vpName + CurvePrName::k, CurvePrValue::K::min, CurvePrValue::K::max, CurvePrValue::K::initial));

                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    const juce::String vvCode = vpCode + CurvePrKey::valueList[vv];
                    const juce::String vvName = vpName + CurvePrName::valueList[vv];

                    layout.add(std::make_unique<juce::AudioParameterFloat>(vvCode, vvName, CurvePrValue::Value::min, CurvePrValue::Value::max, CurvePrValue::Value::initial));
                }
            }
        }
    }
}

void CurveProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = CurvePrKey::prefix;

    params.curve.enable = (*apvts.getRawParameterValue(code + CurvePrKey::enable) > CurvePrValue::boolThread);

    // ロジック変更前のごみ掃除
    initParams(params);

    for (int p = 0; p < CurvePrValue::positions; p++) {
        const juce::String pCode = code + CurvePrKey::position[p];

        for (int t = 0; t < CurvePrValue::targets; t++) {
            const juce::String tCode = pCode + CurvePrKey::target[t];

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                const juce::String vpCode = tCode + CurvePrKey::paramList[vp];

                int logic = (int)*apvts.getRawParameterValue(vpCode + CurvePrKey::logic) - 1;

                params.curve.params[p][t][vp].logic = logic;
                params.curve.params[p][t][vp].k = *apvts.getRawParameterValue(vpCode + CurvePrKey::k);

                switch ((CurveParams::Logic)logic) {
                case(CurveParams::Logic::Linear):
                case(CurveParams::Logic::ArcExp):
                case(CurveParams::Logic::ArcLog):
                    // 値の設定なし
                    break;
                case(CurveParams::Logic::Exp):
                    params.curve.params[p][t][vp].expCurve.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    break;
                case(CurveParams::Logic::Log):
                    params.curve.params[p][t][vp].logCurve.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    break;
                case(CurveParams::Logic::Sp1):
                    params.curve.params[p][t][vp].sp1Curve.cp.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].sp1Curve.cp.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    break;
                case(CurveParams::Logic::Sp2):
                    params.curve.params[p][t][vp].sp2Curve.cp1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].sp2Curve.cp1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].sp2Curve.cp2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].sp2Curve.cp2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    break;
                case(CurveParams::Logic::LinearArcExp):
                    params.curve.params[p][t][vp].linear1ArcExp.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear1ArcExp.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    break;
                case(CurveParams::Logic::LinearArcLog):
                    params.curve.params[p][t][vp].linear1ArcLog.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear1ArcLog.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    break;
                case(CurveParams::Logic::LinearExp):
                    params.curve.params[p][t][vp].linear1Exp.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear1Exp.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear1Exp.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    break;
                case(CurveParams::Logic::LinearLog):
                    params.curve.params[p][t][vp].linear1Log.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear1Log.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear1Log.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    break;
                case(CurveParams::Logic::LinearSp1):
                    params.curve.params[p][t][vp].linear1Sp1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear1Sp1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear1Sp1.cp.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear1Sp1.cp.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    break;
                case(CurveParams::Logic::LinearSp2):
                    params.curve.params[p][t][vp].linear1Sp2.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear1Sp2.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear1Sp2.cp1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear1Sp2.cp1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    params.curve.params[p][t][vp].linear1Sp2.cp2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[4]);
                    params.curve.params[p][t][vp].linear1Sp2.cp2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[5]);
                    break;
                case(CurveParams::Logic::ArcExpLinear):
                    params.curve.params[p][t][vp].arcExpLinear1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].arcExpLinear1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    break;
                case(CurveParams::Logic::ArcLogLinear):
                    params.curve.params[p][t][vp].arcLogLinear1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].arcLogLinear1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    break;
                case(CurveParams::Logic::ExpLinear):
                    params.curve.params[p][t][vp].expLinear1.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].expLinear1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].expLinear1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    break;
                case(CurveParams::Logic::LogLinear):
                    params.curve.params[p][t][vp].logLinear1.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].logLinear1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].logLinear1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    break;
                case(CurveParams::Logic::Sp1Linear):
                    params.curve.params[p][t][vp].sp1Linear1.cp.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].sp1Linear1.cp.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].sp1Linear1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].sp1Linear1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    break;
                case(CurveParams::Logic::Sp2Linear):
                    params.curve.params[p][t][vp].sp2Linear1.cp1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].sp2Linear1.cp1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].sp2Linear1.cp2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].sp2Linear1.cp2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    params.curve.params[p][t][vp].sp2Linear1.pos.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[4]);
                    params.curve.params[p][t][vp].sp2Linear1.pos.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[5]);
                    break;
                case(CurveParams::Logic::Linear2ArcExp):
                    params.curve.params[p][t][vp].linear2ArcExp.pos1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear2ArcExp.pos1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear2ArcExp.pos2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear2ArcExp.pos2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    break;
                case(CurveParams::Logic::Linear2ArcLog):
                    params.curve.params[p][t][vp].linear2ArcLog.pos1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear2ArcLog.pos1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear2ArcLog.pos2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear2ArcLog.pos2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    break;
                case(CurveParams::Logic::Linear2Exp):
                    params.curve.params[p][t][vp].linear2Exp.pos1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear2Exp.pos1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear2Exp.pos2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear2Exp.pos2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    params.curve.params[p][t][vp].linear2Exp.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[4]);
                    break;
                case(CurveParams::Logic::Linear2Log):
                    params.curve.params[p][t][vp].linear2Log.pos1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear2Log.pos1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear2Log.pos2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear2Log.pos2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    params.curve.params[p][t][vp].linear2Log.rate = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[4]);
                    break;
                case(CurveParams::Logic::Linear2Sp1):
                    params.curve.params[p][t][vp].linear2Sp1.pos1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear2Sp1.pos1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear2Sp1.pos2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear2Sp1.pos2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    params.curve.params[p][t][vp].linear2Sp1.cp.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[4]);
                    params.curve.params[p][t][vp].linear2Sp1.cp.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[5]);
                    break;
                case(CurveParams::Logic::Linear2Sp2):
                    params.curve.params[p][t][vp].linear2Sp2.pos1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[0]);
                    params.curve.params[p][t][vp].linear2Sp2.pos1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[1]);
                    params.curve.params[p][t][vp].linear2Sp2.pos2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[2]);
                    params.curve.params[p][t][vp].linear2Sp2.pos2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[3]);
                    params.curve.params[p][t][vp].linear2Sp2.cp1.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[4]);
                    params.curve.params[p][t][vp].linear2Sp2.cp1.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[5]);
                    params.curve.params[p][t][vp].linear2Sp2.cp2.x = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[6]);
                    params.curve.params[p][t][vp].linear2Sp2.cp2.y = *apvts.getRawParameterValue(vpCode + CurvePrKey::valueList[7]);
                    break;
                }
            }
        }
    }
}

void CurveProcessor::initParams(SynthParams& params) {
    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                params.curve.params[p][t][vp].logic = 0;
                params.curve.params[p][t][vp].k = 0.0;

                params.curve.params[p][t][vp].expCurve.rate = 0.0f;

                params.curve.params[p][t][vp].logCurve.rate = 0.0f;

                params.curve.params[p][t][vp].sp1Curve.cp.x = 0.0f;
                params.curve.params[p][t][vp].sp1Curve.cp.y = 0.0f;

                params.curve.params[p][t][vp].sp2Curve.cp1.x = 0.0f;
                params.curve.params[p][t][vp].sp2Curve.cp1.y = 0.0f;
                params.curve.params[p][t][vp].sp2Curve.cp2.x = 0.0f;
                params.curve.params[p][t][vp].sp2Curve.cp2.y = 0.0f;

                params.curve.params[p][t][vp].linear1ArcExp.pos.x = 0.0f;
                params.curve.params[p][t][vp].linear1ArcExp.pos.y = 0.0f;

                params.curve.params[p][t][vp].linear1ArcLog.pos.x = 0.0f;
                params.curve.params[p][t][vp].linear1ArcLog.pos.y = 0.0f;

                params.curve.params[p][t][vp].linear1Exp.pos.x = 0.0f;
                params.curve.params[p][t][vp].linear1Exp.pos.y = 0.0f;
                params.curve.params[p][t][vp].linear1Exp.rate = 0.0f;

                params.curve.params[p][t][vp].linear1Log.pos.x = 0.0f;
                params.curve.params[p][t][vp].linear1Log.pos.y = 0.0f;
                params.curve.params[p][t][vp].linear1Log.rate = 0.0f;

                params.curve.params[p][t][vp].linear1Sp1.pos.x = 0.0f;
                params.curve.params[p][t][vp].linear1Sp1.pos.y = 0.0f;
                params.curve.params[p][t][vp].linear1Sp1.cp.x = 0.0f;
                params.curve.params[p][t][vp].linear1Sp1.cp.y = 0.0f;

                params.curve.params[p][t][vp].linear1Sp2.pos.x = 0.0f;
                params.curve.params[p][t][vp].linear1Sp2.pos.y = 0.0f;
                params.curve.params[p][t][vp].linear1Sp2.cp1.x = 0.0f;
                params.curve.params[p][t][vp].linear1Sp2.cp1.y = 0.0f;
                params.curve.params[p][t][vp].linear1Sp2.cp2.x = 0.0f;
                params.curve.params[p][t][vp].linear1Sp2.cp2.y = 0.0f;

                params.curve.params[p][t][vp].arcExpLinear1.pos.x = 0.0f;
                params.curve.params[p][t][vp].arcExpLinear1.pos.y = 0.0f;

                params.curve.params[p][t][vp].arcLogLinear1.pos.x = 0.0f;
                params.curve.params[p][t][vp].arcLogLinear1.pos.y = 0.0f;

                params.curve.params[p][t][vp].expLinear1.pos.x = 0.0f;
                params.curve.params[p][t][vp].expLinear1.pos.y = 0.0f;
                params.curve.params[p][t][vp].expLinear1.rate = 0.0f;

                params.curve.params[p][t][vp].logLinear1.pos.x = 0.0f;
                params.curve.params[p][t][vp].logLinear1.pos.y = 0.0f;
                params.curve.params[p][t][vp].logLinear1.rate = 0.0f;

                params.curve.params[p][t][vp].sp1Linear1.pos.x = 0.0f;
                params.curve.params[p][t][vp].sp1Linear1.pos.y = 0.0f;
                params.curve.params[p][t][vp].sp1Linear1.cp.x = 0.0f;
                params.curve.params[p][t][vp].sp1Linear1.cp.y = 0.0f;

                params.curve.params[p][t][vp].sp2Linear1.pos.x = 0.0f;
                params.curve.params[p][t][vp].sp2Linear1.pos.y = 0.0f;
                params.curve.params[p][t][vp].sp2Linear1.cp1.x = 0.0f;
                params.curve.params[p][t][vp].sp2Linear1.cp1.y = 0.0f;
                params.curve.params[p][t][vp].sp2Linear1.cp2.x = 0.0f;
                params.curve.params[p][t][vp].sp2Linear1.cp2.y = 0.0f;

                params.curve.params[p][t][vp].linear2ArcExp.pos1.x = 0.0f;
                params.curve.params[p][t][vp].linear2ArcExp.pos1.y = 0.0f;
                params.curve.params[p][t][vp].linear2ArcExp.pos2.x = 0.0f;
                params.curve.params[p][t][vp].linear2ArcExp.pos2.y = 0.0f;

                params.curve.params[p][t][vp].linear2ArcLog.pos1.x = 0.0f;
                params.curve.params[p][t][vp].linear2ArcLog.pos1.y = 0.0f;
                params.curve.params[p][t][vp].linear2ArcLog.pos2.x = 0.0f;
                params.curve.params[p][t][vp].linear2ArcLog.pos2.y = 0.0f;

                params.curve.params[p][t][vp].linear2Exp.pos1.x = 0.0f;
                params.curve.params[p][t][vp].linear2Exp.pos1.y = 0.0f;
                params.curve.params[p][t][vp].linear2Exp.pos2.x = 0.0f;
                params.curve.params[p][t][vp].linear2Exp.pos2.y = 0.0f;
                params.curve.params[p][t][vp].linear2Exp.rate = 0.0f;

                params.curve.params[p][t][vp].linear2Log.pos1.x = 0.0f;
                params.curve.params[p][t][vp].linear2Log.pos1.y = 0.0f;
                params.curve.params[p][t][vp].linear2Log.pos2.x = 0.0f;
                params.curve.params[p][t][vp].linear2Log.pos2.y = 0.0f;
                params.curve.params[p][t][vp].linear2Log.rate = 0.0f;

                params.curve.params[p][t][vp].linear2Sp1.pos1.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp1.pos1.y = 0.0f;
                params.curve.params[p][t][vp].linear2Sp1.pos2.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp1.pos2.y = 0.0f;
                params.curve.params[p][t][vp].linear2Sp1.cp.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp1.cp.y = 0.0f;

                params.curve.params[p][t][vp].linear2Sp2.pos1.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.pos1.y = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.pos2.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.pos2.y = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.cp1.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.cp1.y = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.cp2.x = 0.0f;
                params.curve.params[p][t][vp].linear2Sp2.cp2.y = 0.0f;
            }
        }
    }
}