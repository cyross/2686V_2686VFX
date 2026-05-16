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
    GuiTextButton resetBtn;

    // 以降、エフェクトごとの設定

    // トレモロ(Tremolo)
    GuiToggleButton tBypassBtn;
    GuiCategoryLabel tMainCat;
    GuiSlider tRateSlider, tDepthSlider;
    GuiCategoryLabel tMixCat;
    GuiSlider tMixSlider;
    GuiTextButton tDryBtn, tHalfBtn, tWetBtn;

    // ビブラート(Vibrato)
    GuiToggleButton vBypassBtn;
    GuiCategoryLabel vMainCat;
    GuiSlider vRateSlider, vDepthSlider;
    GuiCategoryLabel vMixCat;
    GuiSlider vMixSlider;
    GuiTextButton vDryBtn, vHalfBtn, vWetBtn;

    // ビットクラッシャー(Modern Bit Crusher)
    GuiToggleButton mbcBypassBtn;
    GuiCategoryLabel mbcMainCat;
    GuiSlider mbcRateSlider, mbcBitsSlider;
    GuiCategoryLabel mbcMixCat;
    GuiSlider mbcMixSlider;
    GuiTextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // ディレイ(Delay)
    GuiToggleButton dBypassBtn;
    GuiCategoryLabel dMainCat;
    GuiSlider dTimeSlider, dFbSlider;
    GuiCategoryLabel dMixCat;
    GuiSlider dMixSlider;
    GuiTextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    GuiToggleButton rBypassBtn;
    GuiCategoryLabel rMainCat;
    GuiSlider rSizeSlider, rDampSlider;
    GuiCategoryLabel rMixCat;
    GuiSlider rMixSlider;
    GuiTextButton rDryBtn, rHalfBtn, rWetBtn;

    // Filter
    GuiToggleButton flBypassBtn;
    GuiCategoryLabel flMainCat;
    GuiComboBox flTypeSelector;
    GuiSlider flFreqSlider;
    GuiSlider flQSlider;
    GuiCategoryLabel flMixCat;
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
        resetBtn(context),
        tBypassBtn(context),
        tMainCat(context),
        tRateSlider(context),
        tDepthSlider(context),
        tMixCat(context),
        tMixSlider(context),
        tDryBtn(context),
        tHalfBtn(context),
        tWetBtn(context),
        vBypassBtn(context),
        vMainCat(context),
        vRateSlider(context),
        vDepthSlider(context),
        vMixCat(context),
        vMixSlider(context),
        vDryBtn(context),
        vHalfBtn(context),
        vWetBtn(context),
        mbcBypassBtn(context),
        mbcMainCat(context),
        mbcRateSlider(context),
        mbcBitsSlider(context),
        mbcMixCat(context),
        mbcMixSlider(context),
        mbcDryBtn(context),
        mbcHalfBtn(context),
        mbcWetBtn(context),
        dBypassBtn(context),
        dMainCat(context),
        dTimeSlider(context),
        dFbSlider(context),
        dMixCat(context),
        dMixSlider(context),
        dDryBtn(context),
        dHalfBtn(context),
        dWetBtn(context),
        rBypassBtn(context),
        rMainCat(context),
        rSizeSlider(context),
        rDampSlider(context),
        rMixCat(context),
        rMixSlider(context),
        rDryBtn(context),
        rHalfBtn(context),
        rWetBtn(context),
        flBypassBtn(context),
        flMainCat(context),
        flTypeSelector(context),
        flFreqSlider(context),
        flQSlider(context),
        flMixCat(context),
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
