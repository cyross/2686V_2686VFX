#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiAdpcm.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat adpcmFormat{ "adpcm", 1 };
	const Io::ParamFormat pcmPlayFormat{ "pcmPlay", 1 };
	const Io::ParamFormat pcmQualityFormat{ "pcmQuality", 1 };
	const Io::ParamFormat toneNoiseFormat{ "toneNoise", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Adpcm/ProcessorAdpcmKeys.h"
#include "../../Processor/Adpcm/ProcessorAdpcmValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiAdpcmValues.h"
#include "./GuiAdpcmText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiAdpcmHelpers.h"

void GuiAdpcm::setup()
{
    auto setupPanBtn = [this](juce::Component& parent, GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            parent.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    const juce::String code = AdpcmPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();

    mainGroup.setup(*this, AdpcmGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

	qualityPcmComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    // 出力レベル
    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::tone, .title = AdpcmGuiText::Adpcm::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::noise, .title = AdpcmGuiText::Adpcm::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::freq, .title = AdpcmGuiText::Adpcm::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::mix , .title = AdpcmGuiText::Adpcm::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::toTone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::toNoise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    optionalCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::optional, .enableChangeDetailVisible = true });

    // ループトグルボタン
    loopButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::loop, .title = AdpcmGuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    loopPointEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::lpEnable, .title = AdpcmGuiText::Adpcm::loopPointEnable, .isReset = true });
    loopPointEnableButton.setWantsKeyboardFocus(true);
    loopPointEnableButton.setExplicitFocusOrder(++tabOrder);

    loopPointStartSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::lpStart, .title = AdpcmGuiText::Adpcm::loopPointStart, .isReset = true });
    loopPointStartSlider.setWantsKeyboardFocus(true);
    loopPointStartSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEndSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::lpEnd, .title = AdpcmGuiText::Adpcm::loopPointEnd, .isReset = true });
    loopPointEndSlider.setWantsKeyboardFocus(true);
    loopPointEndSlider.setExplicitFocusOrder(++tabOrder);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::pcmOffset, .title = AdpcmGuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::pcmRatio, .title = AdpcmGuiText::Adpcm::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // 切り出しとループの設定が変わったら、プレビューも合わせる。
    // 各コンポーネントの setup() より後に付けること。setup() は APVTS との
    // 束縛を張り直すので、先に付けると束縛が壊れて操作できなくなる。
    auto refreshSamplePreview = [this]() { this->updateSamplePreview(); };

    pcmOffsetSlider.onValueChange = refreshSamplePreview;
    pcmRatioSlider.onValueChange = refreshSamplePreview;
    loopPointStartSlider.onValueChange = refreshSamplePreview;
    loopPointEndSlider.onValueChange = refreshSamplePreview;
    loopPointEnableButton.onStateChange = refreshSamplePreview;

    updateSamplePreview();

    // パンポット設定
    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::pan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::pan, .title = AdpcmGuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = AdpcmGuiText::Adpcm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(0.0f, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = AdpcmGuiText::Adpcm::Pan::c, .isReset = false});
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0.5f, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = AdpcmGuiText::Adpcm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1.0f, juce::sendNotification);
        };

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, AdpcmGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, AdpcmGuiText::SsgSwEnv::bypass);

	ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, AdpcmGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, AdpcmGuiText::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwPEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = mainGroup.contentCanvas, .title = Io::empty });

    samplePreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::AudioFile);
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    clearButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            ctx.audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    formSeparator.setupComponent(mainGroup.contentCanvas);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::bcLevel });
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

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", lfoComponent);

    ieDetune.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Detune", mulDetuneComponent);

    ieAmpEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);

    iePitchEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Pitch Env", pitchEnvComponent);

    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);
    ieSsgHwPEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv", ssgHwPEnv);
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);
    ieWtAmpMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Mod", ampModComponent);

    ieSsgSwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW Env", ssgSwEnvComponent);

    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11Component);

    ieSsgSwPEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11Component);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    iePcmPlay.setupComponent(mainGroup.contentCanvas, tabOrder, "PCM Play");
    iePcmPlay.onClickImport = [this] { importPcmPlayParam(); };
    iePcmPlay.onClickExport = [this] { exportPcmPlayParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    setupGraph();
    updateGraph();
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(AdpcmGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(AdpcmGuiValue::Group::Padding::width, AdpcmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(AdpcmGuiValue::Group::TitlePaddingTop);

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

    layoutOptionalCat(mRect);

    layoutPanCat(mRect);

    ampEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);
    ssgHwEnv.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv));
    ssgSwEnvComponent.layoutComponent(mRect);
    ssgSwEnv11Component.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11Component.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    pitchEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv));
    pitchEnvComponent.layoutComponent(mRect);
    ssgHwPEnv.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11Component.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11Component.layoutComponent(mRect);
    modComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    lfoComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Lfo));
    lfoComponent.layoutComponent(mRect);

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
}

void GuiAdpcm::updateFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);

    // 名前とプレビューは常に同じサンプルを指していてほしいので、ここで揃える
    updateSamplePreview();
}

// 読み込んだサンプルの、実際に鳴る範囲を描く。
// 波形は 1 点ずつ拾っても形が分からないので、区間ごとの上下幅で出す。
void GuiAdpcm::updateSamplePreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateSamplePreview(); })) return;

    const auto& data = ctx.audioProcessor.adpcmPreviewBuffer;

    if (data.empty()) {
        samplePreview.clear();

        return;
    }

    auto env = WavePreviewSource::audioFile(
        data,
        ctx.audioProcessor.adpcmPreviewRate,
        (float)pcmOffsetSlider.getValue(),
        (float)pcmRatioSlider.getValue());

    samplePreview.setEnvelope(env.mins, env.maxs);

    // ループ位置は切り出した範囲に対する 0.0〜1.0。使うときだけ出す。
    std::vector<float> markers;

    if (loopPointEnableButton.getToggleState()) {
        markers.push_back((float)loopPointStartSlider.getValue());
        markers.push_back((float)loopPointEndSlider.getValue());
    }

    samplePreview.setMarkers(markers);
}

bool GuiAdpcm::isThis(juce::Button* button)
{
    return button == &loadButton;
}

bool GuiAdpcm::isBtnPanL(juce::Button* button)
{
    return button == &panToLBtn;
}

bool GuiAdpcm::isBtnPanC(juce::Button* button)
{
    return button == &panToCBtn;
}

bool GuiAdpcm::isBtnPanR(juce::Button* button)
{
    return button == &panToRBtn;
}

void GuiAdpcm::setPan(float pan)
{
    panSlider.setValue(pan);
}

void GuiAdpcm::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

void GuiAdpcm::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiAdpcm::initParams()
{
    this->ctx.audioProcessor.initParams("ADPCM_");
    this->ctx.audioProcessor.unloadAdpcmFile();
    updateFileName(Io::empty);
}

void GuiAdpcm::layoutUtilityCat(juce::Rectangle<int>& rect)
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
    iePcmPlay.setVisible(visible);
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
        iePcmPlay.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiAdpcm::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    loadButton.setVisible(visible);
    fileNameLabel.setVisible(visible);
    samplePreview.setVisible(visible);
    clearButton.setVisible(visible);
    formSeparator.setVisible(visible);
    toneSlider.setVisibleWithLabel(visible);
    noiseSlider.setVisibleWithLabel(visible);
    noiseFreqSlider.setVisibleWithLabel(visible);
    mixSlider.setVisibleWithLabel(visible);
    mixSetTone.setVisible(visible);
    mixSetMix.setVisible(visible);
    mixSetNoise.setVisible(visible);

    if (visible)
    {
        layoutMainPcm({ .rect = rect, .loadPcmBtn = &loadButton, .pcmFileNameLabel = &fileNameLabel, .clearPcmBtn = &clearButton });

        samplePreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(3);

        formSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiAdpcm::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityPcmComponent.layoutComponent(rect);
}

void GuiAdpcm::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiAdpcm::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    loopButton.setVisible(visible);
    loopPointEnableButton.setVisible(visible);
    loopPointStartSlider.setVisibleWithLabel(visible);
    loopPointEndSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutMain({ .mainRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutMain({ .mainRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
        layoutMain({ .mainRect = rect, .component = &loopButton });
        layoutMain({ .mainRect = rect, .component = &loopPointEnableButton });
        layoutMain({ .mainRect = rect, .label = &loopPointStartSlider.label, .component = &loopPointStartSlider, });
        layoutMain({ .mainRect = rect, .label = &loopPointEndSlider.label, .component = &loopPointEndSlider, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiAdpcm::setupGraph()
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
        if (this->isUpdatingGraph) return; // 既に更新中なら無視

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

void GuiAdpcm::setGraphMode(GraphMode mode)
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

void GuiAdpcm::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(AdpcmGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(AdpcmGuiValue::MainGroup::Graph::ButtonHeight);
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
void GuiAdpcm::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // カーブモードが有効かどうかを判定
    // カーブを使うかどうかは処理側が持っている。画面から引くと、
    // どのタブを開いても Curve タブまで一緒に組み上がってしまう。
    bool isCurveMode = ctx.audioProcessor.prCurve.getEnable();

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11Component.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11Component.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        ampEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
}

void GuiAdpcm::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiAdpcm::importToneNoiseParam() {
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

void GuiAdpcm::exportToneNoiseParam() {
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

void GuiAdpcm::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::PcmQualityParam);
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

                    Io::ParamWriter writer(pcmQualityFormat);

                    writeQualityParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, pcmQualityFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

				qualityPcmComponent.setMode(reader->getInt("mode", qualityPcmComponent.getMode()));
                qualityPcmComponent.setRate(reader->getInt("rate", qualityPcmComponent.getRate()));
                qualityPcmComponent.setInterp(reader->getInt("interp", qualityPcmComponent.getInterp()));
            }
        });
}

void GuiAdpcm::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::PcmQualityParam)), Io::saveGlob(Io::Extension::PcmQualityParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(pcmQualityFormat);
                writeQualityParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiAdpcm::importPcmPlayParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultPcmPlayParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importPcmPlayParamFile, defaultDir, Io::ExtensionGlob::PcmPlayParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

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

                        setImportingPcmPlayParams(lines, index);
                    }

                    Io::ParamWriter writer(pcmPlayFormat);

                    writePcmPlayParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, pcmPlayFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                pcmOffsetSlider.setValue(reader->getFloat("pcmOffset", (float)pcmOffsetSlider.getValue()), juce::sendNotification);
                pcmRatioSlider.setValue(reader->getFloat("pcmRatio", (float)pcmRatioSlider.getValue()), juce::sendNotification);
                loopPointEnableButton.setToggleState(reader->getBool("loopPointEnable", loopPointEnableButton.getToggleState()), juce::sendNotification);
                loopPointStartSlider.setValue(reader->getFloat("loopPointStart", (float)loopPointStartSlider.getValue()), juce::sendNotification);
                loopPointEndSlider.setValue(reader->getFloat("loopPointEnd", (float)loopPointEndSlider.getValue()), juce::sendNotification);
            }
        });
}

void GuiAdpcm::exportPcmPlayParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultPcmPlayParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportPcmPlayParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::PcmPlayParam)), Io::saveGlob(Io::Extension::PcmPlayParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(pcmPlayFormat);
                writePcmPlayParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiAdpcm::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::adpcmParam);
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

                    Io::ParamWriter writer(adpcmFormat);

                    writeChParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, adpcmFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                // Level
				levelComponent.readParams(*reader, "level");

                // Form
				// 場所はプロセッサが持っているものを使う。ラベルはファイル名だけを
				// 出しているので、そこから File を作ることはできない。
				auto path = Io::resolveSamplePath(reader->getString("filePath", ctx.audioProcessor.adpcmFilePath), ctx.audioProcessor.defaultSampleDir);

				// 別のファイルへ変わるなら、いま持っているものを先に外す
				if (ctx.audioProcessor.adpcmFilePath != path) {
					ctx.audioProcessor.unloadAdpcmFile();
				}

				// 名前は updateFileName を通す。ラベルだけを書き換えると、
				// 波形プレビューが前のサンプルを指したまま残るため。
				if (Io::isFilePath(path)) {
					juce::File target(path);

					ctx.audioProcessor.loadAdpcmFile(target);
					updateFileName(target.getFileName());
				}
				else {
					updateFileName(Io::empty);
				}

                // Optional
				loopButton.setToggleState(reader->getBool("loop", loopButton.getToggleState()), juce::sendNotification);

                // Tone/Noise
                toneSlider.setValue(reader->getFloat("tone", (float)toneSlider.getValue()), juce::sendNotification);
                noiseSlider.setValue(reader->getFloat("noise", (float)noiseSlider.getValue()), juce::sendNotification);
                noiseFreqSlider.setValue(reader->getFloat("noiseFreq", (float)noiseFreqSlider.getValue()), juce::sendNotification);
                mixSlider.setValue(reader->getFloat("mix", (float)mixSlider.getValue()), juce::sendNotification);

                // Pan
				panSlider.setValue(reader->getFloat("pan", (float)panSlider.getValue()), juce::sendNotification);

                // PCM Play
                pcmOffsetSlider.setValue(reader->getFloat("pcmOffset", (float)pcmOffsetSlider.getValue()), juce::sendNotification);
                pcmRatioSlider.setValue(reader->getFloat("pcmRatio", (float)pcmRatioSlider.getValue()), juce::sendNotification);
                loopPointEnableButton.setToggleState(reader->getBool("loopPointEnable", loopPointEnableButton.getToggleState()), juce::sendNotification);
                loopPointStartSlider.setValue(reader->getFloat("loopPointStart", (float)loopPointStartSlider.getValue()), juce::sendNotification);
                loopPointEndSlider.setValue(reader->getFloat("loopPointEnd", (float)loopPointEndSlider.getValue()), juce::sendNotification);

                // Components
				fixComponent.readParams(*reader, "fix");
				ampEnvComponent.readParams(*reader, "ampEnv");
				pitchEnvComponent.readParams(*reader, "pitchEnv");
                ssgHwEnv.readParams(*reader, "ssgHwEnv");
                ssgHwPEnv.readParams(*reader, "ssgHwPEnv");
                ssgSwEnvComponent.readParams(*reader, "ssgSwEnv");
				ssgSwEnv11Component.readParams(*reader, "ssgSwEnv11");
				ssgSwPEnv11Component.readParams(*reader, "ssgSwPEnv11");
				mulDetuneComponent.readParams(*reader, "mulDetune");
				lfoComponent.readParams(*reader, "lfo");
				qualityPcmComponent.readParams(*reader, "qualityPcm");
                unisonComponent.readParams(*reader, "unison");

                modComponent.readParams(*reader, "wtMod");
                ampModComponent.readParams(*reader, "wtAmpMod");
            }
        });
}

void GuiAdpcm::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::adpcmParam)), Io::saveGlob(Io::Extension::adpcmParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(adpcmFormat);
                writeChParams(writer);

                writer.writeTo(file);
            }
        });

}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiAdpcm::setImportingChParams(juce::StringArray& lines, int& index) {
	            // Level
	levelComponent.setImportingParams(lines, index);

	            // Form
	if (fileNameLabel.getText() != lines[index]) {
	ctx.audioProcessor.unloadAdpcmFile();
	}

	fileNameLabel.setText(lines[index++], juce::dontSendNotification);

	if (fileNameLabel.getText().isNotEmpty()) {
	ctx.audioProcessor.loadAdpcmFile(fileNameLabel.getText());
	}

	            // Optional
	loopButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

	            // Tone/Noise
	            toneSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	            noiseSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	            noiseFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	            mixSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	            // Pan
	panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	            // PCM Play
	            pcmOffsetSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	            pcmRatioSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	            loopPointEnableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	            loopPointStartSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	            loopPointEndSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	            // Components
	fixComponent.setImportingParams(lines, index);
	ampEnvComponent.setImportingParams(lines, index);
	pitchEnvComponent.setImportingParams(lines, index);
	            ssgHwEnv.setImportingParams(lines, index);
	            ssgSwEnvComponent.setImportingParams(lines, index);
	ssgSwEnv11Component.setImportingParams(lines, index);
	ssgSwPEnv11Component.setImportingParams(lines, index);
	mulDetuneComponent.setImportingParams(lines, index);
	lfoComponent.setImportingParams(lines, index);
	qualityPcmComponent.setImportingParams(lines, index);
	            unisonComponent.setImportingParams(lines, index);

	            // MODULATION は後から足したので、旧フォーマットとの互換のため
	            // 行が無ければ既定のままにする。
	            if (index < lines.size()) {
	                modComponent.setImportingBaseParams(lines, index);
	                modComponent.setImportingShapeParam(lines, index);
	            }
	        
}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiAdpcm::writeChParams(Io::ParamWriter& writer) {
	            // Level
	            levelComponent.writeParams(writer, "level");

	            // Form
	// 名前ではなく場所を残す。読み戻すときに File を作れるようにするため。
	writer.set("filePath", Io::toStoredFileName(ctx.audioProcessor.adpcmFilePath));

	            // Optional
	writer.set("loop", loopButton.getToggleState());

	            // Tone/Noise
	            writer.set("tone", (float)toneSlider.getValue());
	            writer.set("noise", (float)noiseSlider.getValue());
	            writer.set("noiseFreq", (float)noiseFreqSlider.getValue());
	            writer.set("mix", (float)mixSlider.getValue());

	// Pan
	writer.set("pan", (float)panSlider.getValue());

	            // PCM Play
	            writer.set("pcmOffset", (float)pcmOffsetSlider.getValue());
	            writer.set("pcmRatio", (float)pcmRatioSlider.getValue());
	            writer.set("loopPointEnable", loopPointEnableButton.getToggleState());
	            writer.set("loopPointStart", (float)loopPointStartSlider.getValue());
	            writer.set("loopPointEnd", (float)loopPointEndSlider.getValue());
	            
	            // Components
	fixComponent.writeParams(writer, "fix");
	ampEnvComponent.writeParams(writer, "ampEnv");
	pitchEnvComponent.writeParams(writer, "pitchEnv");
	            ssgHwEnv.writeParams(writer, "ssgHwEnv");
	            ssgHwPEnv.writeParams(writer, "ssgHwPEnv");
	            ssgSwEnvComponent.writeParams(writer, "ssgSwEnv");
	ssgSwEnv11Component.writeParams(writer, "ssgSwEnv11");
	ssgSwPEnv11Component.writeParams(writer, "ssgSwPEnv11");
	mulDetuneComponent.writeParams(writer, "mulDetune");
	lfoComponent.writeParams(writer, "lfo");
	            qualityPcmComponent.writeParams(writer, "qualityPcm");
	            unisonComponent.writeParams(writer, "unison");

	            // MODULATION (旧フォーマットと互換を保つため末尾に置く)
	            modComponent.writeParams(writer, "wtMod");
	            ampModComponent.writeParams(writer, "wtAmpMod");

	            
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiAdpcm::setImportingToneNoiseParams(juce::StringArray& lines, int& index) {
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
void GuiAdpcm::writeToneNoiseParams(Io::ParamWriter& writer) {
	writer.set("tone", (float)toneSlider.getValue());
	writer.set("noise", (float)noiseSlider.getValue());
	writer.set("noiseFreq", (float)noiseFreqSlider.getValue());
	writer.set("mix", (float)mixSlider.getValue());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiAdpcm::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	            if (size < 3) return;

	qualityPcmComponent.setMode(lines[0].getIntValue());
	            qualityPcmComponent.setRate(lines[1].getIntValue());
	            qualityPcmComponent.setInterp(lines[2].getIntValue());
	        
}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiAdpcm::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("mode", qualityPcmComponent.getMode());
	writer.set("rate", qualityPcmComponent.getRate());
	writer.set("interp", qualityPcmComponent.getInterp());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiAdpcm::setImportingPcmPlayParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 5) return;

	pcmOffsetSlider.setValue(lines[0].getFloatValue(), juce::sendNotification);
	pcmRatioSlider.setValue(lines[1].getFloatValue(), juce::sendNotification);
	loopPointEnableButton.setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
	loopPointStartSlider.setValue(lines[3].getFloatValue(), juce::sendNotification);
	loopPointEndSlider.setValue(lines[4].getFloatValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiAdpcm::writePcmPlayParams(Io::ParamWriter& writer) {
	writer.set("pcmOffset", (float)pcmOffsetSlider.getValue());
	writer.set("pcmRatio", (float)pcmRatioSlider.getValue());
	writer.set("loopPointEnable", loopPointEnableButton.getToggleState());
	writer.set("loopPointStart", (float)loopPointStartSlider.getValue());
	writer.set("loopPointEnd", (float)loopPointEndSlider.getValue());

	
}

void GuiAdpcm::bypassHiddenCategories()
{
    // いま隠れている区分だけを切る。出したままの区分は触らない。
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)) ampEnvComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv)) ssgSwEnvComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11Component.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) ampModComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv)) pitchEnvComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11Component.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) modComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::Lfo)) lfoComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::MulDet)) mulDetuneComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::Fix)) fixComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::Unison)) unisonComponent.setCategoryBypassed(true);
}

void GuiAdpcm::openEnabledCategories()
{
    // 効いている区分を開く。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && !ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(true);
    if (ssgHwEnv.hasBypassSwitch() && !ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(true);
    if (ssgSwEnvComponent.hasBypassSwitch() && !ssgSwEnvComponent.isCategoryBypassed()) ssgSwEnvComponent.setCategoryOpen(true);
    if (ssgSwEnv11Component.hasBypassSwitch() && !ssgSwEnv11Component.isCategoryBypassed()) ssgSwEnv11Component.setCategoryOpen(true);
    if (ampModComponent.hasBypassSwitch() && !ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(true);
    if (pitchEnvComponent.hasBypassSwitch() && !pitchEnvComponent.isCategoryBypassed()) pitchEnvComponent.setCategoryOpen(true);
    if (ssgHwPEnv.hasBypassSwitch() && !ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(true);
    if (ssgSwPEnv11Component.hasBypassSwitch() && !ssgSwPEnv11Component.isCategoryBypassed()) ssgSwPEnv11Component.setCategoryOpen(true);
    if (modComponent.hasBypassSwitch() && !modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(true);
    if (lfoComponent.hasBypassSwitch() && !lfoComponent.isCategoryBypassed()) lfoComponent.setCategoryOpen(true);
    if (mulDetuneComponent.hasBypassSwitch() && !mulDetuneComponent.isCategoryBypassed()) mulDetuneComponent.setCategoryOpen(true);
    if (fixComponent.hasBypassSwitch() && !fixComponent.isCategoryBypassed()) fixComponent.setCategoryOpen(true);
    if (unisonComponent.hasBypassSwitch() && !unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(true);
}

void GuiAdpcm::closeBypassedCategories()
{
    // 切ってある区分を閉じる。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(false);
    if (ssgHwEnv.hasBypassSwitch() && ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(false);
    if (ssgSwEnvComponent.hasBypassSwitch() && ssgSwEnvComponent.isCategoryBypassed()) ssgSwEnvComponent.setCategoryOpen(false);
    if (ssgSwEnv11Component.hasBypassSwitch() && ssgSwEnv11Component.isCategoryBypassed()) ssgSwEnv11Component.setCategoryOpen(false);
    if (ampModComponent.hasBypassSwitch() && ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(false);
    if (pitchEnvComponent.hasBypassSwitch() && pitchEnvComponent.isCategoryBypassed()) pitchEnvComponent.setCategoryOpen(false);
    if (ssgHwPEnv.hasBypassSwitch() && ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(false);
    if (ssgSwPEnv11Component.hasBypassSwitch() && ssgSwPEnv11Component.isCategoryBypassed()) ssgSwPEnv11Component.setCategoryOpen(false);
    if (modComponent.hasBypassSwitch() && modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(false);
    if (lfoComponent.hasBypassSwitch() && lfoComponent.isCategoryBypassed()) lfoComponent.setCategoryOpen(false);
    if (mulDetuneComponent.hasBypassSwitch() && mulDetuneComponent.isCategoryBypassed()) mulDetuneComponent.setCategoryOpen(false);
    if (fixComponent.hasBypassSwitch() && fixComponent.isCategoryBypassed()) fixComponent.setCategoryOpen(false);
    if (unisonComponent.hasBypassSwitch() && unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(false);
}
