#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiSsg.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ssgFormat{ "ssg", 1 };
	const Io::ParamFormat qualityFormat{ "quality", 1 };
	const Io::ParamFormat toneNoiseFormat{ "toneNoise", 1 };
}

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Processor/Ssg/ProcessorSsgKeys.h"
#include "../../Processor/Ssg/ProcessorSsgValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiSsgValues.h"
#include "./GuiSsgText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> ssgPrItems = {
    {.name = "0: 1:1 (50%)",     .value = 1 },
    {.name = "1: 3:5 (37.5%)",   .value = 2 },
    {.name = "2: 5:11 (31.25%)", .value = 3 },
    {.name = "3: 1:3 (25%)",     .value = 4 },
    {.name = "4: 1:4 (20%)",     .value = 5 },
    {.name = "5: 3:13 (18.75%)", .value = 6 },
    {.name = "6: 1:7 (12.5%)",   .value = 7 },
    {.name = "7: 1:15 (6.25%)",  .value = 8 },
};

static std::vector<SelectItem> ssgWsItems = {
    {.name = "0: Pulse(Rect)",    .value = 1 },
    {.name = "1: Triangle / Saw", .value = 2 },
};

static std::vector<SelectItem> ssgDmItems = {
    {.name = "0: Preset Ratio",      .value = 1 },
    {.name = "1: Variable (Slider)", .value = 2 },
};

void GuiSsg::setup()
{
    const juce::String code = SsgPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, SsgGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, SsgGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, SsgGuiText::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, SsgGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, SsgGuiText::SsgSwPEnv11::bypass);

    // SSG チャンネル自身の機能なのでハード扱いにする
    ssgHwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, GuiColor::Category::HwBg);
    ssgHwPEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    hwEnvCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::hwEnv, .enableChangeDetailVisible = true });

    lfo.setupComponent(
        mainGroup.contentCanvas,
        code,
        tabOrder
    );

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    ieToneNoise.setupComponent(mainGroup.contentCanvas, tabOrder, "Tone/Noise");
    ieToneNoise.onClickImport = [this] { importToneNoiseParam(); };
    ieToneNoise.onClickExport = [this] { exportToneNoiseParam(); };

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", lfo);

    ieDetune.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Detune", mulDetuneComponent);

    ieAmpEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);

    iePitchEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Pitch Env", pitchEnvComponent);

    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnvComponent);
    ieSsgHwPEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv", ssgHwPEnvComponent);
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);
    ieWtAmpMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Mod", ampModComponent);

    ieSsgSwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW Env", ssgSwEnvComponent);

    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11Component);

    ieSsgSwPEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11Component);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    waveSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::ssgWaveform, .title = SsgGuiText::Ssg::Voice::form, .items = ssgWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);

    formSeparator.setupComponent(mainGroup.contentCanvas);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::tone, .title = SsgGuiText::Ssg::Voice::tone, .isReset = true, .regType = RegisterType::SsgVol });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::noise, .title = SsgGuiText::Ssg::Voice::noise, .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::freq, .title = SsgGuiText::Ssg::Voice::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    noiseOnNoteButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::nON, .title = SsgGuiText::Ssg::Voice::noiseOnNote, .isReset = true });
    noiseOnNoteButton.setWantsKeyboardFocus(true);
    noiseOnNoteButton.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::mix , .title = SsgGuiText::Ssg::Voice::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Ssg::Voice::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Ssg::Voice::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Ssg::Voice::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    // Duty Controls Setup
    dutyGroup.setup(*this, SsgGuiText::Group::ssgDuty);

    dutyModeSelector.setup({ .parent = *this, .id = code + CPK::SsgDuty::mode, .title = SsgGuiText::Ssg::Duty::mode, .items = ssgDmItems, .isReset = true, .isResized = true });
    dutyModeSelector.setWantsKeyboardFocus(true);
    dutyModeSelector.setExplicitFocusOrder(++tabOrder);

    dutyPresetSelector.setup({ .parent = *this, .id = code + CPK::SsgDuty::preset, .title = SsgGuiText::Ssg::Duty::preset, .items = ssgPrItems, .isReset = true, .isResized = true });
    dutyPresetSelector.setWantsKeyboardFocus(true);
    dutyPresetSelector.setExplicitFocusOrder(++tabOrder);

    dutyVarSlider.setup({ .parent = *this, .id = code + CPK::SsgDuty::var, .title = SsgGuiText::Ssg::Duty::var, .isReset = true });
    dutyVarSlider.setWantsKeyboardFocus(true);
    dutyVarSlider.setExplicitFocusOrder(++tabOrder);

    dutyFcButton.setup({ .parent = *this, .id = code + CPK::SsgDuty::fc, .title = SsgGuiText::Ssg::Duty::fc, .isReset = true });
    dutyFcButton.setWantsKeyboardFocus(true);
    dutyFcButton.setExplicitFocusOrder(++tabOrder);

    dutyFcFlucSlider.setup({ .parent = *this, .id = code + CPK::SsgDuty::fcFluc, .title = SsgGuiText::Ssg::Duty::fcFluc, .isReset = true });
    dutyFcFlucSlider.setWantsKeyboardFocus(true);
    dutyFcFlucSlider.setExplicitFocusOrder(++tabOrder);

    pulseInvCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::invert });

    dutyInvertButton.setup({ .parent = *this, .id = code + CPK::SsgDuty::inv, .title = SsgGuiText::Ssg::Duty::invert, .isReset = true });
    dutyInvertButton.setWantsKeyboardFocus(true);
    dutyInvertButton.setExplicitFocusOrder(++tabOrder);

    triGroup.setup(*this, SsgGuiText::Group::ssgTri);

    triKeyTrackButton.setup({ .parent = *this, .id = code + CPK::SsgTri::keyTrk, .title = SsgGuiText::Ssg::Tri::keyTrack, .isReset = true, .isResized = true });
    triKeyTrackButton.setWantsKeyboardFocus(true);
    triKeyTrackButton.setExplicitFocusOrder(++tabOrder);

    triFreqSlider.setup({ .parent = *this, .id = code + CPK::SsgTri::freq, .title = SsgGuiText::Ssg::Tri::manualFreq, .isReset = true });
    triFreqSlider.setWantsKeyboardFocus(true);
    triFreqSlider.setExplicitFocusOrder(++tabOrder);

    triPeakCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::peak });

    triPeakSlider.setup({ .parent = *this, .id = code + CPK::SsgTri::peak, .title = SsgGuiText::Ssg::Tri::peak, .isReset = true });
    triPeakSlider.setWantsKeyboardFocus(true);
    triPeakSlider.setExplicitFocusOrder(++tabOrder);

    triSetSawDown.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo00, .isReset = false, .isResized = false });
    triSetSawDown.setWantsKeyboardFocus(true);
    triSetSawDown.setExplicitFocusOrder(++tabOrder);
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };

    triSetTri.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo05, .isReset = false, .isResized = false });
    triSetTri.setWantsKeyboardFocus(true);
    triSetTri.setExplicitFocusOrder(++tabOrder);
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };

    triSetSawUp.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo10, .isReset = false, .isResized = false });
    triSetSawUp.setWantsKeyboardFocus(true);
    triSetSawUp.setExplicitFocusOrder(++tabOrder);
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };

    setupGraph();
    updateGraph();
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = SsgPrKey::prefix;
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(SsgGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);
    mmRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフ用の区画を確保
    layoutGraph(mmRect);
    updateGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutFormCat(mRect);

    ampEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);
    ssgHwEnvComponent.layoutComponent(mRect);
    ssgSwEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv));
    ssgSwEnvComponent.layoutComponent(mRect);
    ssgSwEnv11Component.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11Component.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    pitchEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv));
    pitchEnvComponent.layoutComponent(mRect);
    ssgHwPEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnvComponent.layoutComponent(mRect);
    ssgSwPEnv11Component.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11Component.layoutComponent(mRect);
    modComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    lfo.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Lfo));
    lfo.layoutComponent(mRect);

    mulDetuneComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::MulDet));
    mulDetuneComponent.layoutComponent(mRect);

    fixComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Fix));
    fixComponent.layoutComponent(mRect);

    unisonComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Unison));
    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    auto paramArea = pageArea.removeFromLeft(SsgGuiValue::ParamGroup::width);

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + CPK::ssgWaveform);
    int waveMode = PrHelper::floatToBoolToInt(waveParam);
    auto waveArea = paramArea.removeFromTop(140);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triKeyTrackButton.setVisible(false);
        triPeakSlider.setVisibleWithLabel(false);
        triFreqSlider.setVisibleWithLabel(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        dutyModeSelector.setVisibleWithLabel(true);
        dutyVarSlider.setVisibleWithLabel(true);
        dutyFcButton.setVisible(true);
        dutyFcFlucSlider.setVisibleWithLabel(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

        dRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = dRect, .label = &dutyModeSelector.label, .component = &dutyModeSelector });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + CPK::SsgDuty::mode);
        if (dutyModeVal < 0.5f) {
            dutyPresetSelector.setVisibleWithLabel(true);
            dutyVarSlider.setVisibleWithLabel(false);
            layoutRow({ .rowRect = dRect, .label = &dutyPresetSelector.label, .component = &dutyPresetSelector, });
        }
        else {
            dutyPresetSelector.setVisibleWithLabel(false);
            dutyVarSlider.setVisibleWithLabel(true);
            layoutRow({ .rowRect = dRect, .label = &dutyVarSlider.label, .component = &dutyVarSlider, });
        }

        layoutRow({ .rowRect = dRect, .component = &dutyFcButton });
        layoutRow({ .rowRect = dRect, .label = &dutyFcFlucSlider.label, .component = &dutyFcFlucSlider, });

        layoutRow({ .rowRect = dRect, .component = &dutyInvertButton, .paddingBottom = 0 });
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        dutyModeSelector.setVisibleWithLabel(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisibleWithLabel(false);
        dutyFcButton.setVisible(false);
        dutyFcFlucSlider.setVisibleWithLabel(false);
        dutyVarSlider.setVisibleWithLabel(false);

        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisibleWithLabel(true);
        triPeakSlider.setVisibleWithLabel(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

        tRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = tRect, .component = &triKeyTrackButton });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisibleWithLabel(!isKeyTrack);

        if (!isKeyTrack) {
            layoutRow({ .rowRect = tRect, .label = &triFreqSlider.label, .component = &triFreqSlider, });
        }
        else
        {
            tRect.removeFromTop(SsgGuiValue::Category::paddingTop);
        }

        layoutRow({ .rowRect = tRect, .label = &triPeakSlider.label, .component = &triPeakSlider });
        layoutRowThreeComps({ .rect = tRect, .comp1 = &triSetSawDown, .comp2 = &triSetTri, .comp3 = &triSetSawUp, .paddingBottom = 0 });
    }
}

void GuiSsg::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiSsg::initParams()
{
    this->ctx.audioProcessor.initParams("SSG_");
}

void GuiSsg::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    waveSelector.setVisibleWithLabel(visible);
    formSeparator.setVisible(visible);
    toneSlider.setVisibleWithLabel(visible);
    noiseSlider.setVisibleWithLabel(visible);
    noiseFreqSlider.setVisibleWithLabel(visible);
    noiseOnNoteButton.setVisible(visible);
    mixSlider.setVisibleWithLabel(visible);
    mixSetTone.setVisible(visible);
    mixSetMix.setVisible(visible);
    mixSetNoise.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &waveSelector.label, .component = &waveSelector, });
        formSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .component = &noiseOnNoteButton, });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiSsg::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiSsg::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);

    ieToneNoise.setVisible(visible);
    ieLfo.setVisible(visible);
    ieDetune.setVisible(visible);
    ieAmpEnv.setVisible(visible);
    iePitchEnv.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgHwPEnv.setVisible(visible);
    ieWtMod.setVisible(visible);
    ieWtAmpMod.setVisible(visible);
    ieSsgSwEnv.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    ieChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });
        uSep001.layoutComponent(rect);
        ieToneNoise.layoutComponent(rect);
        rect.removeFromTop(4);
        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieAmpEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        iePitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgHwEnv.layoutComponent(rect);
        ieSsgHwPEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieWtMod.layoutComponent(rect);
        ieWtAmpMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwPEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieDetune.layoutComponent(rect);
        rect.removeFromTop(4);
        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiSsg::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "PIT", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    graphBtnSsg11.setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    graphBtnSsg11.onClick = [this] { setGraphMode(GraphMode::SsgSw11); };

    graphBtnSsgP11.setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    graphBtnSsgP11.onClick = [this] { setGraphMode(GraphMode::SsgSwP11); };

    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return;

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateGraph();
        };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    ssgSwEnv11Component.setupGraph(repaintGraph);

    ssgSwPEnv11Component.setupGraph(repaintGraph);

    graphSeparator.setupComponent(*this);
}

void GuiSsg::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GraphMode::SsgSwP11, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiSsg::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(SsgGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(SsgGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 5;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsgP11.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiSsg::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11Component.updateGraph(graph);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11Component.updateGraph(graph);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        ampEnvComponent.updateGraph(graph);
    }
}

void GuiSsg::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiSsg::importToneNoiseParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultToneNoiseParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importToneNoiseParamFile, defaultDir, Io::ExtensionGlob::ToneNoiseParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

                // 3.0.0 より前のファイルは、当時の処理で読み込んでから
                // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
                // 読み込みは当時のものをそのまま使う。
                if (Io::isLegacyFile(file)) {
                    juce::StringArray lines;

                    file.readLines(lines);

                    int index = 0;

                    {
                        // 読み終えてからまとめて描き直す
                        GuiRefresh::Batch batch;

                        setImportingToneNoiseParams(lines, index);
                    }

                    Io::ParamWriter writer(toneNoiseFormat);

                    writeToneNoiseParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, toneNoiseFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                toneSlider.setValue(reader->getFloat("tone", (float)toneSlider.getValue()), juce::sendNotification);
                noiseSlider.setValue(reader->getFloat("noise", (float)noiseSlider.getValue()), juce::sendNotification);
                noiseFreqSlider.setValue(reader->getFloat("noiseFreq", (float)noiseFreqSlider.getValue()), juce::sendNotification);
                mixSlider.setValue(reader->getFloat("mix", (float)mixSlider.getValue()), juce::sendNotification);
            }
        });
}

void GuiSsg::exportToneNoiseParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultToneNoiseParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportToneNoiseParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::ToneNoiseParam)), Io::saveGlob(Io::Extension::ToneNoiseParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(toneNoiseFormat);
                writeToneNoiseParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiSsg::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                // 3.0.0 より前のファイルは、当時の処理で読み込んでから
                // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
                // 読み込みは当時のものをそのまま使う。
                if (Io::isLegacyFile(file)) {
                    juce::StringArray lines;

                    file.readLines(lines);

                    int index = 0;

                    {
                        // 読み終えてからまとめて描き直す
                        GuiRefresh::Batch batch;

                        setImportingQualityParams(lines, index);
                    }

                    Io::ParamWriter writer(qualityFormat);

                    writeQualityParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, qualityFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                qualityComponent.setBit(reader->getInt("bit", qualityComponent.getBit()));
                qualityComponent.setRate(reader->getInt("rate", qualityComponent.getRate()));
            }
        });
}

void GuiSsg::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::QualityParam)), Io::saveGlob(Io::Extension::QualityParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(qualityFormat);
                writeQualityParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiSsg::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::ssgParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                // 3.0.0 より前のファイルは、当時の処理で読み込んでから
                // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
                // 読み込みは当時のものをそのまま使う。
                if (Io::isLegacyFile(file)) {
                    juce::StringArray lines;

                    file.readLines(lines);

                    int index = 0;

                    {
                        // 読み終えてからまとめて描き直す
                        GuiRefresh::Batch batch;

                        setImportingChParams(lines, index);
                    }

                    Io::ParamWriter writer(ssgFormat);

                    writeChParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, ssgFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                // Level
                levelComponent.readParams(*reader, "level");

                // Form / Tone / Noise
                waveSelector.setSelectedItemIndex(reader->getInt("wave", waveSelector.getSelectedItemIndex()), juce::sendNotification);
                toneSlider.setValue(reader->getFloat("tone", (float)toneSlider.getValue()), juce::sendNotification);
                noiseSlider.setValue(reader->getFloat("noise", (float)noiseSlider.getValue()), juce::sendNotification);
                noiseFreqSlider.setValue(reader->getFloat("noiseFreq", (float)noiseFreqSlider.getValue()), juce::sendNotification);
                noiseOnNoteButton.setToggleState(reader->getBool("noiseOnNote", noiseOnNoteButton.getToggleState()), juce::sendNotification);
                mixSlider.setValue(reader->getFloat("mix", (float)mixSlider.getValue()), juce::sendNotification);

                // Duty
                dutyModeSelector.setSelectedItemIndex(reader->getInt("dutyMode", dutyModeSelector.getSelectedItemIndex()), juce::sendNotification);
                dutyPresetSelector.setSelectedItemIndex(reader->getInt("dutyPreset", dutyPresetSelector.getSelectedItemIndex()), juce::sendNotification);
                dutyVarSlider.setValue(reader->getFloat("dutyVar", (float)dutyVarSlider.getValue()), juce::sendNotification);
                dutyInvertButton.setToggleState(reader->getBool("dutyInvert", dutyInvertButton.getToggleState()), juce::sendNotification);
                dutyFcButton.setToggleState(reader->getBool("dutyFc", dutyFcButton.getToggleState()), juce::sendNotification);
                dutyFcFlucSlider.setValue(reader->getFloat("dutyFcFluc", (float)dutyFcFlucSlider.getValue()), juce::sendNotification);

                // Triangle
                triKeyTrackButton.setToggleState(reader->getBool("triKeyTrack", triKeyTrackButton.getToggleState()), juce::sendNotification);
                triFreqSlider.setValue(reader->getFloat("triFreq", (float)triFreqSlider.getValue()), juce::sendNotification);
                triPeakSlider.setValue(reader->getFloat("triPeak", (float)triPeakSlider.getValue()), juce::sendNotification);

                // Components
                ssgHwEnvComponent.readParams(*reader, "ssgHwEnv");
                ssgHwPEnvComponent.readParams(*reader, "ssgHwPEnv");
                fixComponent.readParams(*reader, "fix");
                ampEnvComponent.readParams(*reader, "ampEnv");
                pitchEnvComponent.readParams(*reader, "pitchEnv");
                ssgSwEnvComponent.readParams(*reader, "ssgSwEnv");
                ssgSwEnv11Component.readParams(*reader, "ssgSwEnv11");
                ssgSwPEnv11Component.readParams(*reader, "ssgSwPEnv11");
                mulDetuneComponent.readParams(*reader, "mulDetune");
                lfo.readParams(*reader, "lfo");
                qualityComponent.readParams(*reader, "quality");
                unisonComponent.readParams(*reader, "unison");

                modComponent.readParams(*reader, "wtMod");
                ampModComponent.readParams(*reader, "wtAmpMod");
            }
        });

}

void GuiSsg::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::ssgParam)), Io::saveGlob(Io::Extension::ssgParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(ssgFormat);
                writeChParams(writer);

                writer.writeTo(file);
            }
        });

}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiSsg::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Form / Tone / Noise
	waveSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
	toneSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	noiseSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	noiseFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	noiseOnNoteButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	mixSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Duty
	dutyModeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
	dutyPresetSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
	dutyVarSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	dutyInvertButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	dutyFcButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	dutyFcFlucSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Triangle
	triKeyTrackButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	triFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	triPeakSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Components
	ssgHwEnvComponent.setImportingParams(lines, index);
	fixComponent.setImportingParams(lines, index);
	ampEnvComponent.setImportingParams(lines, index);
	pitchEnvComponent.setImportingParams(lines, index);
	ssgSwEnvComponent.setImportingParams(lines, index);
	ssgSwEnv11Component.setImportingParams(lines, index);
	ssgSwPEnv11Component.setImportingParams(lines, index);
	mulDetuneComponent.setImportingParams(lines, index);
	lfo.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	// MODULATION は後から足したので、旧フォーマットとの互換のため
	// 行が無ければ既定のままにする。
	if (index < lines.size()) {
	    modComponent.setImportingBaseParams(lines, index);
	    modComponent.setImportingShapeParam(lines, index);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiSsg::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Form / Tone / Noise
	writer.set("wave", waveSelector.getSelectedItemIndex());
	writer.set("tone", (float)toneSlider.getValue());
	writer.set("noise", (float)noiseSlider.getValue());
	writer.set("noiseFreq", (float)noiseFreqSlider.getValue());
	writer.set("noiseOnNote", noiseOnNoteButton.getToggleState());
	writer.set("mix", (float)mixSlider.getValue());

	// Duty
	writer.set("dutyMode", dutyModeSelector.getSelectedItemIndex());
	writer.set("dutyPreset", dutyPresetSelector.getSelectedItemIndex());
	writer.set("dutyVar", (float)dutyVarSlider.getValue());
	writer.set("dutyInvert", dutyInvertButton.getToggleState());
	writer.set("dutyFc", dutyFcButton.getToggleState());
	writer.set("dutyFcFluc", (float)dutyFcFlucSlider.getValue());

	// Triangle
	writer.set("triKeyTrack", triKeyTrackButton.getToggleState());
	writer.set("triFreq", (float)triFreqSlider.getValue());
	writer.set("triPeak", (float)triPeakSlider.getValue());

	// Components
	ssgHwEnvComponent.writeParams(writer, "ssgHwEnv");
	ssgHwPEnvComponent.writeParams(writer, "ssgHwPEnv");
	fixComponent.writeParams(writer, "fix");
	ampEnvComponent.writeParams(writer, "ampEnv");
	pitchEnvComponent.writeParams(writer, "pitchEnv");
	ssgSwEnvComponent.writeParams(writer, "ssgSwEnv");
	ssgSwEnv11Component.writeParams(writer, "ssgSwEnv11");
	ssgSwPEnv11Component.writeParams(writer, "ssgSwPEnv11");
	mulDetuneComponent.writeParams(writer, "mulDetune");
	lfo.writeParams(writer, "lfo");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");

	// MODULATION (旧フォーマットと互換を保つため末尾に置く)
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiSsg::setImportingToneNoiseParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 4) return;

	toneSlider.setValue(lines[0].getFloatValue(), juce::sendNotification);
	noiseSlider.setValue(lines[1].getFloatValue(), juce::sendNotification);
	noiseFreqSlider.setValue(lines[2].getFloatValue(), juce::sendNotification);
	mixSlider.setValue(lines[3].getFloatValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiSsg::writeToneNoiseParams(Io::ParamWriter& writer) {
	writer.set("tone", (float)toneSlider.getValue());
	writer.set("noise", (float)noiseSlider.getValue());
	writer.set("noiseFreq", (float)noiseFreqSlider.getValue());
	writer.set("mix", (float)mixSlider.getValue());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiSsg::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiSsg::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}
