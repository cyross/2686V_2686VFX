#include "./ProcessorCurve.h"

#include "./ProcessorCurveKeys.h"
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

                for (int i = 0; i < CurvePrValue::values; i++) {
                    pValues[p][t][vp][i] = apvts.getRawParameterValue(vpPrefix + CurvePrKey::valueList[i]);
                }
            }
        }
    }
}

void CurveProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.curve.enable = (pEnable->load(std::memory_order_relaxed) > CurvePrValue::boolThread);

    if (isFirst) {
		processBlockAllParams(params, apvts);

		isFirst = false;
    }
    else {
        for (int vp = 0; vp < CurvePrValue::params; vp++) {
            int logic = (int)pLogics[currentPosition][currentTarget][vp]->load(std::memory_order_relaxed);

            if (logic < 0) {
                continue;
            }

            BaseCurveParams& curveParams = params.curve.params[currentPosition][currentTarget][vp];

            curveParams.logic = logic;
            curveParams.k = pKs[currentPosition][currentTarget][vp]->load(std::memory_order_relaxed);

            std::array<std::atomic<float>*, CurvePrValue::values>& pv = pValues[currentPosition][currentTarget][vp];

            switch ((CurveParams::Logic)logic) {
            case CurveParams::Logic::Linear:
            case CurveParams::Logic::ArcExp:
            case CurveParams::Logic::ArcLog:
                break;
            case CurveParams::Logic::Exp:
                curveParams.expCurve.rate = pv[0]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Log:
                curveParams.logCurve.rate = pv[0]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sp1:
                curveParams.sp1Curve.cp1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sp1Curve.cp1.y = pv[1]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sp2:
                curveParams.sp2Curve.cp1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sp2Curve.cp1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sp2Curve.cp2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sp2Curve.cp2.y = pv[3]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LinearArcExp:
                curveParams.linear1ArcExp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear1ArcExp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LinearArcLog:
                curveParams.linear1ArcLog.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear1ArcLog.pos1.y = pv[1]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LinearExp:
                curveParams.linear1Exp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear1Exp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear1Exp.rate = pv[2]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LinearLog:
                curveParams.linear1Log.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear1Log.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear1Log.rate = pv[2]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LinearSp1:
                curveParams.linear1Sp1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear1Sp1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear1Sp1.cp1.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear1Sp1.cp1.y = pv[3]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LinearSp2:
                curveParams.linear1Sp2.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear1Sp2.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear1Sp2.cp1.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear1Sp2.cp1.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.linear1Sp2.cp2.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.linear1Sp2.cp2.y = pv[5]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::ArcExpLinear:
                curveParams.arcExpLinear1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.arcExpLinear1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::ArcLogLinear:
                curveParams.arcLogLinear1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.arcLogLinear1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::ExpLinear:
                curveParams.expLinear1.rate = pv[0]->load(std::memory_order_relaxed);
                curveParams.expLinear1.pos1.x = pv[1]->load(std::memory_order_relaxed);
                curveParams.expLinear1.pos1.y = pv[2]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::LogLinear:
                curveParams.logLinear1.rate = pv[0]->load(std::memory_order_relaxed);
                curveParams.logLinear1.pos1.x = pv[1]->load(std::memory_order_relaxed);
                curveParams.logLinear1.pos1.y = pv[2]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sp1Linear:
                curveParams.sp1Linear1.cp1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sp1Linear1.cp1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sp1Linear1.pos1.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sp1Linear1.pos1.y = pv[3]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sp2Linear:
                curveParams.sp2Linear1.cp1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sp2Linear1.cp1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sp2Linear1.cp2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sp2Linear1.cp2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.sp2Linear1.pos1.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.sp2Linear1.pos1.y = pv[5]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2ArcExp:
                curveParams.linear2ArcExp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2ArcExp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear2ArcExp.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear2ArcExp.pos2.y = pv[3]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2ArcLog:
                curveParams.linear2ArcLog.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2ArcLog.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear2ArcLog.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear2ArcLog.pos2.y = pv[3]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2Exp:
                curveParams.linear2Exp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2Exp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear2Exp.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear2Exp.pos2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.linear2Exp.rate = pv[4]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2Log:
                curveParams.linear2Log.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2Log.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear2Log.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear2Log.pos2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.linear2Log.rate = pv[4]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2Sp1:
                curveParams.linear2Sp1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2Sp1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear2Sp1.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear2Sp1.pos2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.linear2Sp1.cp1.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.linear2Sp1.cp1.y = pv[5]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2Sp2:
                curveParams.linear2Sp2.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.pos2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.cp1.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.cp1.y = pv[5]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.cp2.x = pv[6]->load(std::memory_order_relaxed);
                curveParams.linear2Sp2.cp2.y = pv[7]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear2:
                curveParams.linear2.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear2.pos1.y = pv[1]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Linear3:
                curveParams.linear3.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.linear3.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.linear3.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.linear3.pos2.y = pv[3]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sprine12:
                curveParams.sprine12.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sprine12.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sprine12.cp1.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sprine12.cp1.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.sprine12.cp2.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.sprine12.cp2.y = pv[5]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sprine22:
                curveParams.sprine22.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sprine22.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp1.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp1.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp2.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp2.y = pv[5]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp3.x = pv[6]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp3.y = pv[7]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp4.x = pv[8]->load(std::memory_order_relaxed);
                curveParams.sprine22.cp4.y = pv[9]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sprine13:
                curveParams.sprine13.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sprine13.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sprine13.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sprine13.pos2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.sprine13.cp1.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.sprine13.cp1.y = pv[5]->load(std::memory_order_relaxed);
                curveParams.sprine13.cp2.x = pv[6]->load(std::memory_order_relaxed);
                curveParams.sprine13.cp2.y = pv[7]->load(std::memory_order_relaxed);
                curveParams.sprine13.cp3.x = pv[8]->load(std::memory_order_relaxed);
                curveParams.sprine13.cp3.y = pv[9]->load(std::memory_order_relaxed);
                break;
            case CurveParams::Logic::Sprine23:
                curveParams.sprine23.pos1.x = pv[0]->load(std::memory_order_relaxed);
                curveParams.sprine23.pos1.y = pv[1]->load(std::memory_order_relaxed);
                curveParams.sprine23.pos2.x = pv[2]->load(std::memory_order_relaxed);
                curveParams.sprine23.pos2.y = pv[3]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp1.x = pv[4]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp1.y = pv[5]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp2.x = pv[6]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp2.y = pv[7]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp3.x = pv[8]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp3.y = pv[9]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp4.x = pv[10]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp4.y = pv[11]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp5.x = pv[12]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp5.y = pv[13]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp6.x = pv[14]->load(std::memory_order_relaxed);
                curveParams.sprine23.cp6.y = pv[15]->load(std::memory_order_relaxed);
                break;
            };
        }
    }
}

void CurveProcessor::processBlockAllParams(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) {
    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                int logic = (int)pLogics[p][t][vp]->load(std::memory_order_relaxed);

                if (logic < 0) {
                    continue;
                }

				BaseCurveParams &curveParams = params.curve.params[p][t][vp];

                curveParams.logic = logic;
                curveParams.k = pKs[p][t][vp]->load(std::memory_order_relaxed);

                std::array<std::atomic<float>*, CurvePrValue::values>& pv = pValues[p][t][vp];

                switch ((CurveParams::Logic)logic) {
                case CurveParams::Logic::Linear:
                case CurveParams::Logic::ArcExp:
                case CurveParams::Logic::ArcLog:
                    break;
                case CurveParams::Logic::Exp:
                    curveParams.expCurve.rate = pv[0]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Log:
                    curveParams.logCurve.rate = pv[0]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sp1:
                    curveParams.sp1Curve.cp1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sp1Curve.cp1.y = pv[1]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sp2:
                    curveParams.sp2Curve.cp1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sp2Curve.cp1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sp2Curve.cp2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sp2Curve.cp2.y = pv[3]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LinearArcExp:
                    curveParams.linear1ArcExp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear1ArcExp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LinearArcLog:
                    curveParams.linear1ArcLog.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear1ArcLog.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LinearExp:
                    curveParams.linear1Exp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear1Exp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear1Exp.rate = pv[2]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LinearLog:
                    curveParams.linear1Log.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear1Log.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear1Log.rate = pv[2]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LinearSp1:
                    curveParams.linear1Sp1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp1.cp1.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp1.cp1.y = pv[3]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LinearSp2:
                    curveParams.linear1Sp2.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp2.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp2.cp1.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp2.cp1.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp2.cp2.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.linear1Sp2.cp2.y = pv[5]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::ArcExpLinear:
                    curveParams.arcExpLinear1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.arcExpLinear1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::ArcLogLinear:
                    curveParams.arcLogLinear1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.arcLogLinear1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::ExpLinear:
                    curveParams.expLinear1.rate = pv[0]->load(std::memory_order_relaxed);
                    curveParams.expLinear1.pos1.x = pv[1]->load(std::memory_order_relaxed);
                    curveParams.expLinear1.pos1.y = pv[2]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::LogLinear:
                    curveParams.logLinear1.rate = pv[0]->load(std::memory_order_relaxed);
                    curveParams.logLinear1.pos1.x = pv[1]->load(std::memory_order_relaxed);
                    curveParams.logLinear1.pos1.y = pv[2]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sp1Linear:
                    curveParams.sp1Linear1.cp1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sp1Linear1.cp1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sp1Linear1.pos1.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sp1Linear1.pos1.y = pv[3]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sp2Linear:
                    curveParams.sp2Linear1.cp1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sp2Linear1.cp1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sp2Linear1.cp2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sp2Linear1.cp2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.sp2Linear1.pos1.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.sp2Linear1.pos1.y = pv[5]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2ArcExp:
                    curveParams.linear2ArcExp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2ArcExp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear2ArcExp.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear2ArcExp.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2ArcLog:
                    curveParams.linear2ArcLog.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2ArcLog.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear2ArcLog.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear2ArcLog.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2Exp:
                    curveParams.linear2Exp.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2Exp.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear2Exp.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear2Exp.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.linear2Exp.rate = pv[4]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2Log:
                    curveParams.linear2Log.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2Log.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear2Log.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear2Log.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.linear2Log.rate = pv[4]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2Sp1:
                    curveParams.linear2Sp1.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp1.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp1.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp1.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp1.cp1.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp1.cp1.y = pv[5]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2Sp2:
                    curveParams.linear2Sp2.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.cp1.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.cp1.y = pv[5]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.cp2.x = pv[6]->load(std::memory_order_relaxed);
                    curveParams.linear2Sp2.cp2.y = pv[7]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear2:
                    curveParams.linear2.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear2.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Linear3:
                    curveParams.linear3.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.linear3.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.linear3.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.linear3.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sprine12:
                    curveParams.sprine12.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sprine12.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sprine12.cp1.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sprine12.cp1.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.sprine12.cp2.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.sprine12.cp2.y = pv[5]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sprine22:
                    curveParams.sprine22.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sprine22.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp1.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp1.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp2.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp2.y = pv[5]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp3.x = pv[6]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp3.y = pv[7]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp4.x = pv[8]->load(std::memory_order_relaxed);
                    curveParams.sprine22.cp4.y = pv[9]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sprine13:
                    curveParams.sprine13.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sprine13.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sprine13.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sprine13.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.sprine13.cp1.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.sprine13.cp1.y = pv[5]->load(std::memory_order_relaxed);
                    curveParams.sprine13.cp2.x = pv[6]->load(std::memory_order_relaxed);
                    curveParams.sprine13.cp2.y = pv[7]->load(std::memory_order_relaxed);
                    curveParams.sprine13.cp3.x = pv[8]->load(std::memory_order_relaxed);
                    curveParams.sprine13.cp3.y = pv[9]->load(std::memory_order_relaxed);
                    break;
                case CurveParams::Logic::Sprine23:
                    curveParams.sprine23.pos1.x = pv[0]->load(std::memory_order_relaxed);
                    curveParams.sprine23.pos1.y = pv[1]->load(std::memory_order_relaxed);
                    curveParams.sprine23.pos2.x = pv[2]->load(std::memory_order_relaxed);
                    curveParams.sprine23.pos2.y = pv[3]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp1.x = pv[4]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp1.y = pv[5]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp2.x = pv[6]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp2.y = pv[7]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp3.x = pv[8]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp3.y = pv[9]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp4.x = pv[10]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp4.y = pv[11]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp5.x = pv[12]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp5.y = pv[13]->load(std::memory_order_relaxed);
                    curveParams.sprine23.cp6.x = pv[14]->load(std::memory_order_relaxed);    
                    curveParams.sprine23.cp6.y = pv[15]->load(std::memory_order_relaxed);
                    break;
                };
            }
        }
    }
}

void CurveProcessor::updateCurrent(int position, int target) {
    currentPosition = position;
    currentTarget = target;
}

void CurveProcessor::resetProcessBlock()
{
	// プリセット読み込み時などで、すべてのパラメータを更新する必要がある場合に呼び出す
    isFirst = true;
}