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

    GuiToggleButton bypassToggle;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // 以降、エフェクトごとの設定

    // レトロLFO(Retro LFO)
    GuiGroup rlfoGroup;
    GuiToggleButton rlfoBypassBtn;
    GuiComboBox rlfoWaveSelector;
    GuiSlider rlfoFreqSlider, rlfoAmsSlider, rlfoPmsSlider, rlfoAmdSlider, rlfoPmdSlider, rlfoMixSlider;
    GuiTextButton rlfoDryBtn, rlfoHalfBtn, rlfoWetBtn;

    // トレモロ(Tremolo)
    GuiToggleButton tBypassBtn;
    GuiSlider tRateSlider, tDepthSlider, tMixSlider;
    GuiTextButton tDryBtn, tHalfBtn, tWetBtn;

    // ビブラート(Vibrato)
    GuiToggleButton vBypassBtn;
    GuiSlider vRateSlider, vDepthSlider, vMixSlider;
    GuiTextButton vDryBtn, vHalfBtn, vWetBtn;

    // ビットクラッシャー(Modern Bit Crusher)
    GuiToggleButton mbcBypassBtn;
    GuiSlider mbcRateSlider, mbcBitsSlider, mbcMixSlider;
    GuiTextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // ディレイ(Delay)
    GuiToggleButton dBypassBtn;
    GuiSlider dTimeSlider, dFbSlider, dMixSlider;
    GuiTextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    GuiToggleButton rBypassBtn;
    GuiSlider rSizeSlider, rDampSlider, rMixSlider;
    GuiTextButton rDryBtn, rHalfBtn, rWetBtn;

    // Retro Bit Crusher
    GuiToggleButton rbcBypassBtn;
    GuiComboBox rbcRateSelector, rbcBitsSelector;
    GuiSlider rbcMixSlider;
    GuiTextButton rbcDryBtn, rbcHalfBtn, rbcWetBtn;

    // Filter
    GuiToggleButton flBypassBtn;
    GuiComboBox flTypeSelector;
    GuiSlider flFreqSlider;
    GuiSlider flQSlider;
    GuiSlider flMixSlider;
    GuiTextButton flDryBtn, flHalfBtn, flWetBtn;

    // Soft Clipper
    GuiToggleButton scBypassBtn;
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
        bypassToggle(context),
        masterVolSlider(context),
        rlfoGroup(context),
        rlfoBypassBtn(context),
        rlfoWaveSelector(context),
        rlfoFreqSlider(context),
        rlfoAmsSlider(context),
        rlfoPmsSlider(context),
        rlfoAmdSlider(context),
        rlfoPmdSlider(context),
        rlfoMixSlider(context),
        rlfoDryBtn(context),
        rlfoHalfBtn(context),
        rlfoWetBtn(context),
        tBypassBtn(context),
        tRateSlider(context),
        tDepthSlider(context),
        tMixSlider(context),
        tDryBtn(context),
        tHalfBtn(context),
        tWetBtn(context),
        vBypassBtn(context),
        vRateSlider(context),
        vDepthSlider(context),
        vMixSlider(context),
        vDryBtn(context),
        vHalfBtn(context),
        vWetBtn(context),
        mbcBypassBtn(context),
        mbcRateSlider(context),
        mbcBitsSlider(context),
        mbcMixSlider(context),
        mbcDryBtn(context),
        mbcHalfBtn(context),
        mbcWetBtn(context),
        dBypassBtn(context),
        dTimeSlider(context),
        dFbSlider(context),
        dMixSlider(context),
        dDryBtn(context),
        dHalfBtn(context),
        dWetBtn(context),
        rBypassBtn(context),
        rSizeSlider(context),
        rDampSlider(context),
        rMixSlider(context),
        rDryBtn(context),
        rHalfBtn(context),
        rWetBtn(context),
        rbcBypassBtn(context),
        rbcRateSelector(context),
        rbcBitsSelector(context),
        rbcMixSlider(context),
        rbcDryBtn(context),
        rbcHalfBtn(context),
        rbcWetBtn(context),
        flBypassBtn(context),
        flTypeSelector(context),
        flFreqSlider(context),
        flQSlider(context),
        flMixSlider(context),
        flDryBtn(context),
        flHalfBtn(context),
        flWetBtn(context),
        scBypassBtn(context),
        scMixSlider(context),
        scDryBtn(context),
        scHalfBtn(context),
        scWetBtn(context)
    {}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
