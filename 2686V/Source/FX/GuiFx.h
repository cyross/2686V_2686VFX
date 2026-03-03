#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiFx : public GuiBase
{
    GuiGroup mainGroup;
    GuiGroup tremGroup;
    GuiGroup vibGroup;
    GuiGroup mbcGroup;
    GuiGroup delayGroup;
    GuiGroup reverbGroup;
    GuiGroup rbcGroup;
    GuiGroup filterGroup;
    GuiGroup softClipperGroup;

    GuiCategoryLabel bypassCat;

    GuiToggleButton bypassToggle;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // 以降、エフェクトごとの設定

    // レトロLFO(Retro LFO)
    GuiGroup rlfoGroup;
    GuiCategoryLabel rlfoBypassCat;
    GuiToggleButton rlfoBypassBtn;
    GuiCategoryLabel rlfoMainCat;
    GuiComboBox rlfoWaveSelector;
    GuiSlider rlfoFreqSlider, rlfoAmsSlider, rlfoPmsSlider, rlfoAmdSlider, rlfoPmdSlider;
    GuiCategoryLabel rlfoMixCat;
    GuiSlider rlfoMixSlider;
    GuiTextButton rlfoDryBtn, rlfoHalfBtn, rlfoWetBtn;

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

    // Retro Bit Crusher
    GuiCategoryLabel rbcBypassCat;
    GuiToggleButton rbcBypassBtn;
    GuiCategoryLabel rbcMainCat;
    GuiComboBox rbcRateSelector, rbcBitsSelector;
    GuiCategoryLabel rbcMixCat;
    GuiSlider rbcMixSlider;
    GuiTextButton rbcDryBtn, rbcHalfBtn, rbcWetBtn;

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

    // Soft Clipper
    GuiCategoryLabel scBypassCat;
    GuiToggleButton scBypassBtn;
    GuiCategoryLabel scMixCat;
    GuiSlider scMixSlider;
    GuiTextButton scDryBtn, scHalfBtn, scWetBtn;
public:
	GuiFx(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        tremGroup(context),
        vibGroup(context),
        mbcGroup(context),
        delayGroup(context),
        reverbGroup(context),
        rbcGroup(context),
        filterGroup(context),
        softClipperGroup(context),
        bypassCat(context),
        bypassToggle(context),
        mvolCat(context),
        masterVolSlider(context),
        rlfoGroup(context),
        rlfoBypassCat(context),
        rlfoBypassBtn(context),
        rlfoMainCat(context),
        rlfoWaveSelector(context),
        rlfoFreqSlider(context),
        rlfoAmsSlider(context),
        rlfoPmsSlider(context),
        rlfoAmdSlider(context),
        rlfoPmdSlider(context),
        rlfoMixCat(context),
        rlfoMixSlider(context),
        rlfoDryBtn(context),
        rlfoHalfBtn(context),
        rlfoWetBtn(context),
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
        rbcBypassCat(context),
        rbcBypassBtn(context),
        rbcMainCat(context),
        rbcRateSelector(context),
        rbcBitsSelector(context),
        rbcMixCat(context),
        rbcMixSlider(context),
        rbcDryBtn(context),
        rbcHalfBtn(context),
        rbcWetBtn(context),
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
        flWetBtn(context),
        scBypassCat(context),
        scBypassBtn(context),
        scMixCat(context),
        scMixSlider(context),
        scDryBtn(context),
        scHalfBtn(context),
        scWetBtn(context)
    {}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
