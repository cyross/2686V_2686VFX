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

    GuiCategoryLabel bypassCat;

    GuiToggleButton bypassToggle;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // 以降、エフェクトごとの設定

    // トレモロ(Tremolo)
    GuiCategoryLabel tBypassCat;
    GuiToggleButton tBypassBtn;
    GuiCategoryLabel tMainCat;
    GuiSlider tRateSlider, tDepthSlider;
    GuiCategoryLabel tMixCat;
    GuiSlider tMixSlider;
    GuiTextButton tDryBtn, tHalfBtn, tWetBtn;

    // ビブラート(Vibrato)
    GuiCategoryLabel vBypassCat;
    GuiToggleButton vBypassBtn;
    GuiCategoryLabel vMainCat;
    GuiSlider vRateSlider, vDepthSlider;
    GuiCategoryLabel vMixCat;
    GuiSlider vMixSlider;
    GuiTextButton vDryBtn, vHalfBtn, vWetBtn;

    // ビットクラッシャー(Modern Bit Crusher)
    GuiCategoryLabel mbcBypassCat;
    GuiToggleButton mbcBypassBtn;
    GuiCategoryLabel mbcMainCat;
    GuiSlider mbcRateSlider, mbcBitsSlider;
    GuiCategoryLabel mbcMixCat;
    GuiSlider mbcMixSlider;
    GuiTextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // ディレイ(Delay)
    GuiCategoryLabel dBypassCat;
    GuiToggleButton dBypassBtn;
    GuiCategoryLabel dMainCat;
    GuiSlider dTimeSlider, dFbSlider;
    GuiCategoryLabel dMixCat;
    GuiSlider dMixSlider;
    GuiTextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    GuiCategoryLabel rBypassCat;
    GuiToggleButton rBypassBtn;
    GuiCategoryLabel rMainCat;
    GuiSlider rSizeSlider, rDampSlider;
    GuiCategoryLabel rMixCat;
    GuiSlider rMixSlider;
    GuiTextButton rDryBtn, rHalfBtn, rWetBtn;

    // Filter
    GuiCategoryLabel flBypassCat;
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
        bypassCat(context),
        bypassToggle(context),
        mvolCat(context),
        masterVolSlider(context),
        tBypassCat(context),
        tBypassBtn(context),
        tMainCat(context),
        tRateSlider(context),
        tDepthSlider(context),
        tMixCat(context),
        tMixSlider(context),
        tDryBtn(context),
        tHalfBtn(context),
        tWetBtn(context),
        vBypassCat(context),
        vBypassBtn(context),
        vMainCat(context),
        vRateSlider(context),
        vDepthSlider(context),
        vMixCat(context),
        vMixSlider(context),
        vDryBtn(context),
        vHalfBtn(context),
        vWetBtn(context),
        mbcBypassCat(context),
        mbcBypassBtn(context),
        mbcMainCat(context),
        mbcRateSlider(context),
        mbcBitsSlider(context),
        mbcMixCat(context),
        mbcMixSlider(context),
        mbcDryBtn(context),
        mbcHalfBtn(context),
        mbcWetBtn(context),
        dBypassCat(context),
        dBypassBtn(context),
        dMainCat(context),
        dTimeSlider(context),
        dFbSlider(context),
        dMixCat(context),
        dMixSlider(context),
        dDryBtn(context),
        dHalfBtn(context),
        dWetBtn(context),
        rBypassCat(context),
        rBypassBtn(context),
        rMainCat(context),
        rSizeSlider(context),
        rDampSlider(context),
        rMixCat(context),
        rMixSlider(context),
        rDryBtn(context),
        rHalfBtn(context),
        rWetBtn(context),
        flBypassCat(context),
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
