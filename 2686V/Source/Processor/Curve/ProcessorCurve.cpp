#include "./ProcessorCurve.h"

#include "./ProcessorCurveKeys.h"
#include "./ProcessorCurveValues.h"
#include "./ProcessorCurveNames.h"

void CurveProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = CurvePrKey::prefix;
    const juce::String prefixName = CurvePrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CurvePrKey::enable, prefix + CurvePrName::enable, CurvePrValue::Enable::initial));

    for (int p = 0; p < CurvePrValue::positions; p++) {
        const juce::String pPrefix = prefix + CurvePrKey::position[p];
        const juce::String pName = prefixName + CurvePrName::position[p];

        for (int t = 0; t < CurvePrValue::targets; t++) {
            const juce::String tPrefix = pPrefix + CurvePrKey::target[t];
            const juce::String tName = pName + CurvePrName::target[t];

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                const juce::String vpPrefix = tPrefix + CurvePrKey::paramList[vp];
                const juce::String vpName = tName + CurvePrName::paramList[vp];

                layout.add(std::make_unique<juce::AudioParameterInt>(vpPrefix + CurvePrKey::logic, vpName + CurvePrName::logic, CurvePrValue::Logic::min, CurvePrValue::Logic::max, CurvePrValue::Logic::initial));
                layout.add(std::make_unique<juce::AudioParameterFloat>(vpPrefix + CurvePrKey::k, vpName + CurvePrName::k, CurvePrValue::K::min, CurvePrValue::K::max, CurvePrValue::K::initial));

                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    const juce::String vvPrefix = vpPrefix + CurvePrKey::valueList[vv];
                    const juce::String vvName = vpName + CurvePrName::valueList[vv];

                    layout.add(std::make_unique<juce::AudioParameterFloat>(vvPrefix, vvName, CurvePrValue::Value::min, CurvePrValue::Value::max, CurvePrValue::Value::initial));
                }
            }
        }
    }
}

void CurveProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = CurvePrKey::prefix;

    pEnable = apvts.getRawParameterValue(prefix + CurvePrKey::enable);

    for (int p = 0; p < CurvePrValue::positions; p++) {
        const juce::String pPrefix = prefix + CurvePrKey::position[p];

        for (int t = 0; t < CurvePrValue::targets; t++) {
            const juce::String tPrefix = pPrefix + CurvePrKey::target[t];

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                const juce::String vpPrefix = tPrefix + CurvePrKey::paramList[vp];

                pLogics[p][t][vp] = apvts.getRawParameterValue(vpPrefix + CurvePrKey::logic);
                pKs[p][t][vp] = apvts.getRawParameterValue(vpPrefix + CurvePrKey::k);

                for (int i = 0; i < 8; i++) {
                    pValues[p][t][vp][i] = apvts.getRawParameterValue(vpPrefix + CurvePrKey::valueList[i]);
                }
            }
        }
    }

    lLogics[(int)CurveParams::Logic::Linear] = [this](SynthParams& params, int p, int t, int vp) {};
    lLogics[(int)CurveParams::Logic::ArcExp] = [this](SynthParams& params, int p, int t, int vp) {};
    lLogics[(int)CurveParams::Logic::ArcLog] = [this](SynthParams& params, int p, int t, int vp) {};
    lLogics[(int)CurveParams::Logic::Exp] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].expCurve.rate = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Log] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].logCurve.rate = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Sp1] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].sp1Curve.cp.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp1Curve.cp.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Sp2] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].sp2Curve.cp1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Curve.cp1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Curve.cp2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Curve.cp2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LinearArcExp] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear1ArcExp.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1ArcExp.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LinearArcLog] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear1ArcLog.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1ArcLog.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LinearExp] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear1Exp.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Exp.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Exp.rate = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LinearLog] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear1Log.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Log.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Log.rate = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LinearSp1] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear1Sp1.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp1.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp1.cp.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp1.cp.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LinearSp2] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear1Sp2.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp2.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp2.cp1.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp2.cp1.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp2.cp2.x = pValues[p][t][vp][4]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear1Sp2.cp2.y = pValues[p][t][vp][5]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::ArcExpLinear] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].arcExpLinear1.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].arcExpLinear1.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::ArcLogLinear] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].arcLogLinear1.pos.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].arcLogLinear1.pos.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::ExpLinear] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].expLinear1.rate = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].expLinear1.pos.x = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].expLinear1.pos.y = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::LogLinear] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].logLinear1.rate = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].logLinear1.pos.x = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].logLinear1.pos.y = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Sp1Linear] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].sp1Linear1.cp.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp1Linear1.cp.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp1Linear1.pos.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp1Linear1.pos.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Sp2Linear] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].sp2Linear1.cp1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Linear1.cp1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Linear1.cp2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Linear1.cp2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Linear1.pos.x = pValues[p][t][vp][4]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].sp2Linear1.pos.y = pValues[p][t][vp][5]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Linear2ArcExp] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear2ArcExp.pos1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2ArcExp.pos1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2ArcExp.pos2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2ArcExp.pos2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Linear2ArcLog] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear2ArcLog.pos1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2ArcLog.pos1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2ArcLog.pos2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2ArcLog.pos2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Linear2Exp] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear2Exp.pos1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Exp.pos1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Exp.pos2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Exp.pos2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Exp.rate = pValues[p][t][vp][4]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Linear2Log] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear2Log.pos1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Log.pos1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Log.pos2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Log.pos2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Log.rate = pValues[p][t][vp][4]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Linear2Sp1] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear2Sp1.pos1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp1.pos1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp1.pos2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp1.pos2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp1.cp.x = pValues[p][t][vp][4]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp1.cp.y = pValues[p][t][vp][5]->load(std::memory_order_relaxed);
        };
    lLogics[(int)CurveParams::Logic::Linear2Sp2] = [this](SynthParams& params, int p, int t, int vp) {
        params.curve.params[p][t][vp].linear2Sp2.pos1.x = pValues[p][t][vp][0]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.pos1.y = pValues[p][t][vp][1]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.pos2.x = pValues[p][t][vp][2]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.pos2.y = pValues[p][t][vp][3]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.cp1.x = pValues[p][t][vp][4]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.cp1.y = pValues[p][t][vp][5]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.cp2.x = pValues[p][t][vp][6]->load(std::memory_order_relaxed);
        params.curve.params[p][t][vp].linear2Sp2.cp2.y = pValues[p][t][vp][7]->load(std::memory_order_relaxed);
        };
}

void CurveProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.curve.enable = (pEnable->load(std::memory_order_relaxed) > CurvePrValue::boolThread);

    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                int logic = pLogics[p][t][vp]->load(std::memory_order_relaxed);

                if (logic < 0) {
                    continue;
                }

                params.curve.params[p][t][vp].logic = logic;
                params.curve.params[p][t][vp].k = pKs[p][t][vp]->load(std::memory_order_relaxed);

                lLogics[logic](params, p, t, vp);
            }
        }
    }
}
