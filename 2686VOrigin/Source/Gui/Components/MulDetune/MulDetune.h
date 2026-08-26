#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../PitchButtons/PitchButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentMulDetune : public GuiBase {
    // MULTIPLE/DETUNE
    GuiCategoryLabel cat;
    GuiComboBox mul;
    GuiComponentNudgeSliderFloat mulRatio;
    GuiTextButton mulRatioTo001;
    GuiTextButton mulRatioTo005;
    GuiTextButton mulRatioTo1;
    GuiTextButton mulRatioTo10;
    GuiTextButton mulRatioTo2757;
    GuiTextButton mulRatioTo02;
    GuiTextButton mulRatioTo025;
    GuiTextButton mulRatioTo04;
    GuiTextButton mulRatioTo05;
    GuiTextButton mulRatioTo06;
    GuiTextButton mulRatioTo075;
    GuiTextButton mulRatioTo08;
    GuiTextButton mulRatioPM10;
    GuiTextButton mulRatioPM1;
    GuiTextButton mulRatioPM01;
    GuiTextButton mulRatioPM001;
    GuiTextButton mulRatioP001;
    GuiTextButton mulRatioP01;
    GuiTextButton mulRatioP1;
    GuiTextButton mulRatioP10;
    NormalSeparator mulDetSep;
    GuiComboBox dt1;
    GuiSlider dt2;
    GuiComponentNudgeSliderFloat dt3;
    GuiComponentPitchButtons dt3Buttons;
    std::unique_ptr<juce::FileChooser> fileChooser;

public:
    GuiComponentMulDetune(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        mul(context),
        mulRatio(context),
		mulRatioTo001(context),
		mulRatioTo005(context),
   		mulRatioTo1(context),
        mulRatioTo10(context),
        mulRatioTo2757(context),
		mulRatioTo02(context),
		mulRatioTo025(context),
		mulRatioTo04(context),
		mulRatioTo05(context),
		mulRatioTo06(context),
		mulRatioTo075(context),
		mulRatioTo08(context),
		mulRatioPM10(context),
		mulRatioPM1(context),
		mulRatioPM01(context),
		mulRatioPM001(context),
		mulRatioP001(context),
		mulRatioP01(context),
		mulRatioP1(context),
		mulRatioP10(context),
		mulDetSep(context),
        dt1(context),
        dt2(context),
        dt3(context),
        dt3Buttons(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, bool isHw = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void copyParams(CopyDetuneOpzx7& copyObj);
    void pasteParams(CopyDetuneOpzx7& copyObj);
    void importParams();
    void exportParams();
    void setMul(int m);
    void setMulRatio(float r);
    void setDt1(int d1);
    void setDt2(int d2);
    void setDt3(int d3);
    int getMul();
    float getMulRatio();
    int getDt1();
    int getDt2();
    int getDt3();
    void setVisibles(bool visible);
    void setEnables(bool enable);
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
