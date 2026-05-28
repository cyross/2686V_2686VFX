#pragma once

#include <JuceHeader.h>
#include <array>
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

class GuiFx : public GuiBase
{
    GuiGroup mainGroup;

    GuiGroup tremGroup;
    GuiGroup vibGroup;
    GuiGroup mbcGroup;
    GuiGroup delayGroup;
    GuiGroup reverbGroup;
    GuiGroup filterGroup;

    GuiToggleButton bypassToggle;
    GuiSeparator mainSeparator;
    GuiTextButton resetBtn;

    // 以降、エフェクトごとの設定

    // トレモロ(Tremolo)
    GuiToggleButton tBypassBtn;
    GuiSeparator tSeparator;
    GuiSlider tRateSlider, tDepthSlider;
    GuiSlider tMixSlider;
    GuiTextButton tDryBtn, tHalfBtn, tWetBtn;

    // ビブラート(Vibrato)
    GuiToggleButton vBypassBtn;
    GuiSeparator vSeparator;
    GuiSlider vRateSlider, vDepthSlider;
    GuiSlider vMixSlider;
    GuiTextButton vDryBtn, vHalfBtn, vWetBtn;

    // ビットクラッシャー(Modern Bit Crusher)
    GuiToggleButton mbcBypassBtn;
    GuiSeparator mbcSeparator;
    GuiSlider mbcRateSlider, mbcBitsSlider;
    GuiSlider mbcMixSlider;
    GuiTextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // ディレイ(Delay)
    GuiToggleButton dBypassBtn;
    GuiSeparator dSeparator;
    GuiSlider dTimeSlider, dFbSlider;
    GuiSlider dMixSlider;
    GuiTextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    GuiToggleButton rBypassBtn;
    GuiSeparator rSeparator;
    GuiSlider rSizeSlider, rDampSlider;
    GuiSlider rMixSlider;
    GuiTextButton rDryBtn, rHalfBtn, rWetBtn;

    // Filter
    GuiToggleButton flBypassBtn;
    GuiSeparator flSeparator;
    GuiComboBox flTypeSelector;
    GuiSlider flFreqSlider;
    GuiSlider flQSlider;
    GuiSlider flMixSlider;
    GuiTextButton flDryBtn, flHalfBtn, flWetBtn;
public:
	GuiFx(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        tremGroup(context),
        vibGroup(context),
        mbcGroup(context),
        delayGroup(context),
        reverbGroup(context),
        filterGroup(context),
        bypassToggle(context),
        mainSeparator(context),
        resetBtn(context),
        tBypassBtn(context),
        tSeparator(context),
        tRateSlider(context),
        tDepthSlider(context),
        tMixSlider(context),
        tDryBtn(context),
        tHalfBtn(context),
        tWetBtn(context),
        vBypassBtn(context),
        vSeparator(context),
        vRateSlider(context),
        vDepthSlider(context),
        vMixSlider(context),
        vDryBtn(context),
        vHalfBtn(context),
        vWetBtn(context),
        mbcBypassBtn(context),
        mbcSeparator(context),
        mbcRateSlider(context),
        mbcBitsSlider(context),
        mbcMixSlider(context),
        mbcDryBtn(context),
        mbcHalfBtn(context),
        mbcWetBtn(context),
        dBypassBtn(context),
        dSeparator(context),
        dTimeSlider(context),
        dFbSlider(context),
        dMixSlider(context),
        dDryBtn(context),
        dHalfBtn(context),
        dWetBtn(context),
        rBypassBtn(context),
        rSeparator(context),
        rSizeSlider(context),
        rDampSlider(context),
        rMixSlider(context),
        rDryBtn(context),
        rHalfBtn(context),
        rWetBtn(context),
        flBypassBtn(context),
        flSeparator(context),
        flTypeSelector(context),
        flFreqSlider(context),
        flQSlider(context),
        flMixSlider(context),
        flDryBtn(context),
        flHalfBtn(context),
        flWetBtn(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
