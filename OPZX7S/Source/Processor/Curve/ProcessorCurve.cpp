#include "./ProcessorCurve.h"
#include "./ProcessorCurveKeys.h"
#include "./ProcessorCurveNames.h"

CurveProcessor::CurveProcessor() {
    resetToDefault();
}

void CurveProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) {
    // 13000個のパラメータ登録を廃止！
}

void CurveProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    // APVTSからのポインタ取得を廃止！
}

void CurveProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) {
    // 独自に管理しているパラメータ構造体をオーディオスレッドにコピーするだけ
    params.curve = m_curveParams;
    params.curve.enable = true;
}

void CurveProcessor::resetProcessBlock() {
    // 何もしない
}

void CurveProcessor::setLogic(int p, int t, int vp, int logic) {
    rawLogic[p][t][vp] = logic;
    updateCurveParamStructure(p, t, vp);
}

void CurveProcessor::setK(int p, int t, int vp, float k) {
    rawK[p][t][vp] = k;
    updateCurveParamStructure(p, t, vp);
}

void CurveProcessor::setValue(int p, int t, int vp, int vv, float val) {
    rawValues[p][t][vp][vv] = val;
    updateCurveParamStructure(p, t, vp);
}

void CurveProcessor::updateCurveParamStructure(int p, int t, int vp) {
    int logic = rawLogic[p][t][vp];
    BaseCurveParams& curveParams = m_curveParams.params[p][t][vp];
    curveParams.logic = logic;
    curveParams.k = rawK[p][t][vp];

    float* pv = rawValues[p][t][vp];

    switch ((CurveParams::Logic)logic) {
    case CurveParams::Logic::Linear:
    case CurveParams::Logic::ArcExp:
    case CurveParams::Logic::ArcLog:
        break;
    case CurveParams::Logic::Exp:
        curveParams.expCurve.rate = pv[0];
        break;
    case CurveParams::Logic::Log:
        curveParams.logCurve.rate = pv[0];
        break;
    case CurveParams::Logic::Sp1:
        curveParams.sp1Curve.cp1.x = pv[0];
        curveParams.sp1Curve.cp1.y = pv[1];
        break;
    case CurveParams::Logic::Sp2:
        curveParams.sp2Curve.cp1.x = pv[0];
        curveParams.sp2Curve.cp1.y = pv[1];
        curveParams.sp2Curve.cp2.x = pv[2];
        curveParams.sp2Curve.cp2.y = pv[3];
        break;
    case CurveParams::Logic::LinearArcExp:
        curveParams.linear1ArcExp.pos1.x = pv[0];
        curveParams.linear1ArcExp.pos1.y = pv[1];
        break;
    case CurveParams::Logic::LinearArcLog:
        curveParams.linear1ArcLog.pos1.x = pv[0];
        curveParams.linear1ArcLog.pos1.y = pv[1];
        break;
    case CurveParams::Logic::LinearExp:
        curveParams.linear1Exp.pos1.x = pv[0];
        curveParams.linear1Exp.pos1.y = pv[1];
        curveParams.linear1Exp.rate = pv[2];
        break;
    case CurveParams::Logic::LinearLog:
        curveParams.linear1Log.pos1.x = pv[0];
        curveParams.linear1Log.pos1.y = pv[1];
        curveParams.linear1Log.rate = pv[2];
        break;
    case CurveParams::Logic::LinearSp1:
        curveParams.linear1Sp1.pos1.x = pv[0];
        curveParams.linear1Sp1.pos1.y = pv[1];
        curveParams.linear1Sp1.cp1.x = pv[2];
        curveParams.linear1Sp1.cp1.y = pv[3];
        break;
    case CurveParams::Logic::LinearSp2:
        curveParams.linear1Sp2.pos1.x = pv[0];
        curveParams.linear1Sp2.pos1.y = pv[1];
        curveParams.linear1Sp2.cp1.x = pv[2];
        curveParams.linear1Sp2.cp1.y = pv[3];
        curveParams.linear1Sp2.cp2.x = pv[4];
        curveParams.linear1Sp2.cp2.y = pv[5];
        break;
    case CurveParams::Logic::ArcExpLinear:
        curveParams.arcExpLinear1.pos1.x = pv[0];
        curveParams.arcExpLinear1.pos1.y = pv[1];
        break;
    case CurveParams::Logic::ArcLogLinear:
        curveParams.arcLogLinear1.pos1.x = pv[0];
        curveParams.arcLogLinear1.pos1.y = pv[1];
        break;
    case CurveParams::Logic::ExpLinear:
        curveParams.expLinear1.rate = pv[0];
        curveParams.expLinear1.pos1.x = pv[1];
        curveParams.expLinear1.pos1.y = pv[2];
        break;
    case CurveParams::Logic::LogLinear:
        curveParams.logLinear1.rate = pv[0];
        curveParams.logLinear1.pos1.x = pv[1];
        curveParams.logLinear1.pos1.y = pv[2];
        break;
    case CurveParams::Logic::Sp1Linear:
        curveParams.sp1Linear1.cp1.x = pv[0];
        curveParams.sp1Linear1.cp1.y = pv[1];
        curveParams.sp1Linear1.pos1.x = pv[2];
        curveParams.sp1Linear1.pos1.y = pv[3];
        break;
    case CurveParams::Logic::Sp2Linear:
        curveParams.sp2Linear1.cp1.x = pv[0];
        curveParams.sp2Linear1.cp1.y = pv[1];
        curveParams.sp2Linear1.cp2.x = pv[2];
        curveParams.sp2Linear1.cp2.y = pv[3];
        curveParams.sp2Linear1.pos1.x = pv[4];
        curveParams.sp2Linear1.pos1.y = pv[5];
        break;
    case CurveParams::Logic::Linear2ArcExp:
        curveParams.linear2ArcExp.pos1.x = pv[0];
        curveParams.linear2ArcExp.pos1.y = pv[1];
        curveParams.linear2ArcExp.pos2.x = pv[2];
        curveParams.linear2ArcExp.pos2.y = pv[3];
        break;
    case CurveParams::Logic::Linear2ArcLog:
        curveParams.linear2ArcLog.pos1.x = pv[0];
        curveParams.linear2ArcLog.pos1.y = pv[1];
        curveParams.linear2ArcLog.pos2.x = pv[2];
        curveParams.linear2ArcLog.pos2.y = pv[3];
        break;
    case CurveParams::Logic::Linear2Exp:
        curveParams.linear2Exp.pos1.x = pv[0];
        curveParams.linear2Exp.pos1.y = pv[1];
        curveParams.linear2Exp.pos2.x = pv[2];
        curveParams.linear2Exp.pos2.y = pv[3];
        curveParams.linear2Exp.rate = pv[4];
        break;
    case CurveParams::Logic::Linear2Log:
        curveParams.linear2Log.pos1.x = pv[0];
        curveParams.linear2Log.pos1.y = pv[1];
        curveParams.linear2Log.pos2.x = pv[2];
        curveParams.linear2Log.pos2.y = pv[3];
        curveParams.linear2Log.rate = pv[4];
        break;
    case CurveParams::Logic::Linear2Sp1:
        curveParams.linear2Sp1.pos1.x = pv[0];
        curveParams.linear2Sp1.pos1.y = pv[1];
        curveParams.linear2Sp1.pos2.x = pv[2];
        curveParams.linear2Sp1.pos2.y = pv[3];
        curveParams.linear2Sp1.cp1.x = pv[4];
        curveParams.linear2Sp1.cp1.y = pv[5];
        break;
    case CurveParams::Logic::Linear2Sp2:
        curveParams.linear2Sp2.pos1.x = pv[0];
        curveParams.linear2Sp2.pos1.y = pv[1];
        curveParams.linear2Sp2.pos2.x = pv[2];
        curveParams.linear2Sp2.pos2.y = pv[3];
        curveParams.linear2Sp2.cp1.x = pv[4];
        curveParams.linear2Sp2.cp1.y = pv[5];
        curveParams.linear2Sp2.cp2.x = pv[6];
        curveParams.linear2Sp2.cp2.y = pv[7];
        break;
    case CurveParams::Logic::Linear2:
        curveParams.linear2.pos1.x = pv[0];
        curveParams.linear2.pos1.y = pv[1];
        break;
    case CurveParams::Logic::Linear3:
        curveParams.linear3.pos1.x = pv[0];
        curveParams.linear3.pos1.y = pv[1];
        curveParams.linear3.pos2.x = pv[2];
        curveParams.linear3.pos2.y = pv[3];
        break;
    case CurveParams::Logic::Sprine12:
        curveParams.sprine12.pos1.x = pv[0];
        curveParams.sprine12.pos1.y = pv[1];
        curveParams.sprine12.cp1.x = pv[2];
        curveParams.sprine12.cp1.y = pv[3];
        curveParams.sprine12.cp2.x = pv[4];
        curveParams.sprine12.cp2.y = pv[5];
        break;
    case CurveParams::Logic::Sprine22:
        curveParams.sprine22.pos1.x = pv[0];
        curveParams.sprine22.pos1.y = pv[1];
        curveParams.sprine22.cp1.x = pv[2];
        curveParams.sprine22.cp1.y = pv[3];
        curveParams.sprine22.cp2.x = pv[4];
        curveParams.sprine22.cp2.y = pv[5];
        curveParams.sprine22.cp3.x = pv[6];
        curveParams.sprine22.cp3.y = pv[7];
        curveParams.sprine22.cp4.x = pv[8];
        curveParams.sprine22.cp4.y = pv[9];
        break;
    case CurveParams::Logic::Sprine13:
        curveParams.sprine13.pos1.x = pv[0];
        curveParams.sprine13.pos1.y = pv[1];
        curveParams.sprine13.pos2.x = pv[2];
        curveParams.sprine13.pos2.y = pv[3];
        curveParams.sprine13.cp1.x = pv[4];
        curveParams.sprine13.cp1.y = pv[5];
        curveParams.sprine13.cp2.x = pv[6];
        curveParams.sprine13.cp2.y = pv[7];
        curveParams.sprine13.cp3.x = pv[8];
        curveParams.sprine13.cp3.y = pv[9];
        break;
    case CurveParams::Logic::Sprine23:
        curveParams.sprine23.pos1.x = pv[0];
        curveParams.sprine23.pos1.y = pv[1];
        curveParams.sprine23.pos2.x = pv[2];
        curveParams.sprine23.pos2.y = pv[3];
        curveParams.sprine23.cp1.x = pv[4];
        curveParams.sprine23.cp1.y = pv[5];
        curveParams.sprine23.cp2.x = pv[6];
        curveParams.sprine23.cp2.y = pv[7];
        curveParams.sprine23.cp3.x = pv[8];
        curveParams.sprine23.cp3.y = pv[9];
        curveParams.sprine23.cp4.x = pv[10];
        curveParams.sprine23.cp4.y = pv[11];
        curveParams.sprine23.cp5.x = pv[12];
        curveParams.sprine23.cp5.y = pv[13];
        curveParams.sprine23.cp6.x = pv[14];
        curveParams.sprine23.cp6.y = pv[15];
        break;
    };
}

void CurveProcessor::saveToXml(juce::XmlElement* xml) {
    auto* curveXml = new juce::XmlElement("CURVE_DATA");

    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                int logic = rawLogic[p][t][vp];

                // 初期値のままであれば保存をスキップして軽量化
                bool isDefault = (logic == 0 && std::abs(rawK[p][t][vp] - 1.0f) < 0.001f);
                if (isDefault) {
                    bool allZero = true;
                    for (int vv = 0; vv < CurvePrValue::values; vv++) {
                        if (std::abs(rawValues[p][t][vp][vv]) > 0.001f) { allZero = false; break; }
                    }
                    if (allZero) continue;
                }

                auto* paramXml = new juce::XmlElement("CP");
                paramXml->setAttribute("p", p);
                paramXml->setAttribute("t", t);
                paramXml->setAttribute("vp", vp);
                paramXml->setAttribute("l", logic);
                paramXml->setAttribute("k", rawK[p][t][vp]);

                juce::StringArray vals;
                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    vals.add(juce::String(rawValues[p][t][vp][vv]));
                }
                paramXml->setAttribute("v", vals.joinIntoString(","));

                curveXml->addChildElement(paramXml);
            }
        }
    }
    xml->addChildElement(curveXml);
}

void CurveProcessor::loadFromXml(juce::XmlElement* xml) {
    // 最初に全てをデフォルトにリセット
    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                rawLogic[p][t][vp] = 0;
                rawK[p][t][vp] = 1.0f;
                for (int vv = 0; vv < CurvePrValue::values; vv++) rawValues[p][t][vp][vv] = 0.0f;
                updateCurveParamStructure(p, t, vp);
            }
        }
    }

    auto* curveXml = xml->getChildByName("CURVE_DATA");
    if (curveXml != nullptr) {
        for (auto* paramXml : curveXml->getChildIterator()) {
            if (paramXml->hasTagName("CP")) {
                int p = paramXml->getIntAttribute("p");
                int t = paramXml->getIntAttribute("t");
                int vp = paramXml->getIntAttribute("vp");

                if (p >= 0 && p < CurvePrValue::positions && t >= 0 && t < CurvePrValue::targets && vp >= 0 && vp < CurvePrValue::params) {
                    rawLogic[p][t][vp] = paramXml->getIntAttribute("l");
                    rawK[p][t][vp] = paramXml->getDoubleAttribute("k", 1.0);

                    juce::String vStr = paramXml->getStringAttribute("v");
                    juce::StringArray vals;
                    vals.addTokens(vStr, ",", "");
                    for (int vv = 0; vv < CurvePrValue::values && vv < vals.size(); vv++) {
                        rawValues[p][t][vp][vv] = vals[vv].getFloatValue();
                    }
                    updateCurveParamStructure(p, t, vp);
                }
            }
        }
    }
}

void CurveProcessor::resetToDefault() {
    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                rawLogic[p][t][vp] = 0;
                rawK[p][t][vp] = 1.0f;
                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    rawValues[p][t][vp][vv] = 0.0f;
                }
                updateCurveParamStructure(p, t, vp);
            }
        }
    }
}
