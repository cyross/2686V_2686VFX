#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiAdpcmPlus.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat adpcmPlusFormat{ "adpcmPlus", 1 };
	const Io::ParamFormat pcmPlayFormat{ "pcmPlay", 1 };
	const Io::ParamFormat pcmQualityFormat{ "pcmQuality", 1 };
	const Io::ParamFormat toneNoiseFormat{ "toneNoise", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/AdpcmPlus/ProcessorAdpcmPlusKeys.h"
#include "../../Processor/AdpcmPlus/ProcessorAdpcmPlusValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiAdpcmPlusValues.h"
#include "./GuiAdpcmPlusText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "../Adpcm/GuiAdpcmHelpers.h"


// TARGET を鍵で動かす。中身は GuiBase の共通の手続きへ預けてある。
bool GuiAdpcmPlus::keyPressed(const juce::KeyPress& key)
{
    return moveTargetByKey(slotTarget, key, false);
}
void GuiAdpcmPlus::setup()
{
    auto setupPanBtn = [this](juce::Component& parent, GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            parent.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    const juce::String code = AdpcmPlusPrKey::prefix;

    // FORM・OPTIONAL・PAN は PCM ごとに持つ。組み立てのときは
    // 0 番で束ねておき、TARGET が動いたら張り直す。
    const juce::String slot0 = code + CPK::AdpcmPlus::slot + juce::String(0);
    int tabOrder = 1;

    mainGroup.setup(*this, AdpcmPlusGuiText::Group::mainGroup);
    pcmGroup.setup(*this, AdpcmPlusGuiText::Group::pcmGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    formCat.setupHwCategory({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

	qualityPcmComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    // 鳴らす PCM。オートメーションで振れる。
    slotSlider.setup({ .parent = pcmGroup.contentCanvas, .id = code + CPK::AdpcmPlus::slot, .title = AdpcmPlusGuiText::Adpcm::slot, .isReset = true });
    slotSlider.setWantsKeyboardFocus(true);
    slotSlider.setExplicitFocusOrder(++tabOrder);
    slotSlider.onValueChange = [this] { slotPreviews.setActive((int)slotSlider.getValue()); };

    // いま画面へ出す PCM。値の置き場所を切り替えるだけで、音には効かない。
    slotTarget.setup({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Adpcm::target, .isReset = false });
    slotTarget.setRange(0.0, (double)(Global::AdpcmPlus::slots - 1), 1.0);
    slotTarget.setNumDecimalPlacesToDisplay(0);
    slotTarget.setWantsKeyboardFocus(true);
    slotTarget.setExplicitFocusOrder(++tabOrder);
    slotTarget.onValueChange = [this] { applySlotTarget(); };

    // 出力レベル
    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    toneSlider.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::Tn::tone, .title = AdpcmPlusGuiText::Adpcm::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::Tn::noise, .title = AdpcmPlusGuiText::Adpcm::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::Tn::freq, .title = AdpcmPlusGuiText::Adpcm::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::Tn::mix , .title = AdpcmPlusGuiText::Adpcm::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Adpcm::toTone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Adpcm::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Adpcm::toNoise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    optionalCat.setupSwCategory({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Category::optional, .detailVisible = true, .enableChangeDetailVisible = true });

    // ループトグルボタン
    loopButton.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::loop, .title = AdpcmPlusGuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    loopPointEnableButton.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::lpEnable, .title = AdpcmPlusGuiText::Adpcm::loopPointEnable, .isReset = true });
    loopPointEnableButton.setWantsKeyboardFocus(true);
    loopPointEnableButton.setExplicitFocusOrder(++tabOrder);

    loopPointStartSlider.setup(GuiSlider::Config{ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::lpStart, .title = AdpcmPlusGuiText::Adpcm::loopPointStart, .isReset = true });
    loopPointStartSlider.setWantsKeyboardFocus(true);
    loopPointStartSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEndSlider.setup(GuiSlider::Config{ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::lpEnd, .title = AdpcmPlusGuiText::Adpcm::loopPointEnd, .isReset = true });
    loopPointEndSlider.setWantsKeyboardFocus(true);
    loopPointEndSlider.setExplicitFocusOrder(++tabOrder);

    speedSlider.setup(GuiSlider::Config{ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::speed, .title = "SPEED", .isReset = true });
    speedSlider.setWantsKeyboardFocus(true);
    speedSlider.setExplicitFocusOrder(++tabOrder);

    optSpeedSeparator.setupComponent(pcmGroup.contentCanvas);

    loopCountSlider.setupComponent(pcmGroup.contentCanvas, slot0 + CPK::lpCount, "CNT", tabOrder, std::nullopt);

    loopCountButtons.setupComponent(pcmGroup.contentCanvas, loopCountSlider.getSlider(), tabOrder);

    optCountSeparator.setupComponent(pcmGroup.contentCanvas);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::pcmOffset, .title = AdpcmPlusGuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::pcmRatio, .title = AdpcmPlusGuiText::Adpcm::pcmRatio, .isReset = true });
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
    panCat.setupHwCategory({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::Category::pan, .detailVisible = true, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = pcmGroup.contentCanvas, .id = slot0 + CPK::pan, .title = AdpcmPlusGuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = pcmGroup.contentCanvas, .id = "", .title = AdpcmPlusGuiText::Adpcm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(0.0f, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = pcmGroup.contentCanvas, .id = "", .title = AdpcmPlusGuiText::Adpcm::Pan::c, .isReset = false});
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0.5f, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = pcmGroup.contentCanvas, .id = "", .title = AdpcmPlusGuiText::Adpcm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1.0f, juce::sendNotification);
        };

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, AdpcmPlusGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, AdpcmPlusGuiText::SsgSwEnv::bypass);

	ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, AdpcmPlusGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, AdpcmPlusGuiText::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwPEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = pcmGroup.contentCanvas, .title = Io::empty });

    samplePreview.setup(pcmGroup.contentCanvas, GuiColor::WavePreview::AudioFile);

    // 32 個あるので 1 行 3 個では縦に伸びすぎる。8 個ずつ 4 行に収める。
    slotPreviews.setup(pcmGroup.contentCanvas, GuiColor::WavePreview::AudioFile,
        Global::AdpcmPlus::slots, 8);
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    clearButton.setup({ .parent = pcmGroup.contentCanvas, .title = AdpcmPlusGuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 外すのは、いま画面へ出しているスロットの 1 本だけ
            ctx.audioProcessor.unloadAdpcmPlusFile(targetSlot());

            // 素材が外れたので、名前も並べた波形もそこから作り直す
            updateSlotFileName(targetSlot());
        };

    formSeparator.setupComponent(pcmGroup.contentCanvas);
    optLoopSepTop.setupComponent(pcmGroup.contentCanvas);
    optLoopSepBottom.setupComponent(pcmGroup.contentCanvas);

    updateAllSlotPreviews();

    // ここまでで作ったつまみを、選んでいるスロットへ向け直す
    applySlotTarget();

    slotPreviews.setActive((int)slotSlider.getValue());

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    // 大区分。音量と音程にかかわる区分を、それぞれまとめて畳めるようにする。
    // 最初は閉じておく。
    ampMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::ampEnv, .enableChangeDetailVisible = true });
    pitchMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::pitchEnv, .enableChangeDetailVisible = true });

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmPlusGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmPlusGuiText::Utility::bcLevel });
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

void GuiAdpcmPlus::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(AdpcmPlusGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(AdpcmPlusGuiValue::Group::Padding::width, AdpcmPlusGuiValue::Group::Padding::height);
    mmRect.removeFromTop(AdpcmPlusGuiValue::Group::TitlePaddingTop);

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


    // [[AMP ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool ampOpen = layoutMajorCategory(ampMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));

    ampEnvComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);
    ssgHwEnv.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnvComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv));
    ssgSwEnvComponent.layoutComponent(mRect);
    ssgSwEnv11Component.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11Component.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    ampMajorCat.endMajor(mRect);

    // [[PITCH ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool pitchOpen = layoutMajorCategory(pitchMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));

    pitchEnvComponent.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv));
    pitchEnvComponent.layoutComponent(mRect);
    ssgHwPEnv.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11Component.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11Component.layoutComponent(mRect);
    modComponent.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    pitchMajorCat.endMajor(mRect);

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

    // ==========================================================
    // PCM 設定グループ
    // ==========================================================
    auto pcmArea = pageArea.removeFromLeft(AdpcmPlusGuiValue::PcmGroup::width);

    pcmArea.removeFromBottom(40);
    pcmGroup.setBounds(pcmArea);

    auto pgRect = pcmArea.reduced(AdpcmPlusGuiValue::Group::Padding::width, AdpcmPlusGuiValue::Group::Padding::height);

    pgRect.removeFromTop(AdpcmPlusGuiValue::Group::TitlePaddingTop);

    pcmGroup.setViewportCustomBounds(pgRect.translated(-pcmArea.getX(), -pcmArea.getY()));

    juce::Rectangle<int> pRect(0, 0, pcmGroup.getContentWidth(), 2000);

    // SLOT と TARGET は畳めない場所へ置く。下の区分をすべて束ねるつまみなので、
    // 隠れてしまうと選び直せなくなる。
    layoutMain({ .mainRect = pRect, .label = &slotSlider.label, .component = &slotSlider });
    layoutMain({ .mainRect = pRect, .label = &slotTarget.label, .component = &slotTarget });

    pRect.removeFromTop(CoreGuiValue::Category::gapBelow);

    layoutFormCat(pRect);

    layoutOptionalCat(pRect);

    layoutPanCat(pRect);

    int pcmUsedHeight = 2000 - pRect.getHeight();

    pcmGroup.setContentHeight(pcmUsedHeight + 20);
}


// TARGET で選んだスロットへ、FORM・OPTIONAL・PAN のつまみを向け直す。
// 画面へ出ているのは 1 組だけで、残りはパラメータとして眠っている。
void GuiAdpcmPlus::rebindSlot()
{
    const juce::String prefix = AdpcmPlusPrKey::prefix + CPK::AdpcmPlus::slot + juce::String(targetSlot());

    toneSlider.rebind(prefix + CPK::Tn::tone);
    noiseSlider.rebind(prefix + CPK::Tn::noise);
    noiseFreqSlider.rebind(prefix + CPK::Tn::freq);
    mixSlider.rebind(prefix + CPK::Tn::mix);

    speedSlider.rebind(prefix + CPK::speed);
    loopCountSlider.getSlider().rebind(prefix + CPK::lpCount);
    pcmOffsetSlider.rebind(prefix + CPK::pcmOffset);
    pcmRatioSlider.rebind(prefix + CPK::pcmRatio);
    loopButton.rebind(prefix + CPK::loop);
    loopPointEnableButton.rebind(prefix + CPK::lpEnable);
    loopPointStartSlider.rebind(prefix + CPK::lpStart);
    loopPointEndSlider.rebind(prefix + CPK::lpEnd);

    panSlider.rebind(prefix + CPK::pan);
}


// 並べた波形の 1 つぶん。切り出す前の素材をそのまま映す。
// 切り出した範囲とループ位置は、上の大きなプレビューが受け持つ。
void GuiAdpcmPlus::updateSlotPreview(int slot)
{
    if (slot < 0 || slot >= Global::AdpcmPlus::slots) return;

    const auto& data = ctx.audioProcessor.adpcmPlusPreviewBuffers[(size_t)slot];

    if (data.empty()) {
        slotPreviews.setEnvelope(slot, {}, {});

        return;
    }

    auto env = WavePreviewSource::audioFile(
        data,
        ctx.audioProcessor.adpcmPlusPreviewRates[(size_t)slot],
        0.0f,
        1.0f);

    slotPreviews.setEnvelope(slot, env.mins, env.maxs);
}

void GuiAdpcmPlus::updateAllSlotPreviews()
{
    for (int i = 0; i < Global::AdpcmPlus::slots; ++i) updateSlotPreview(i);
}
void GuiAdpcmPlus::applySlotTarget()
{
    rebindSlot();
    updateSlotFileName(targetSlot());

    slotPreviews.setSelected(targetSlot());
}

// 読み込んだ PCM の名前を出す。出すのは画面に出ているスロットだけ。
void GuiAdpcmPlus::updateSlotFileName(int slot)
{
    // 並べた波形は、選んでいないスロットのぶんも映す
    updateSlotPreview(slot);

    if (slot != targetSlot()) return;

    const juce::String path = ctx.audioProcessor.adpcmPlusFilePaths[(size_t)slot];

    updateFileName(path.isEmpty() ? Io::empty : juce::File(path).getFileName());
}
void GuiAdpcmPlus::updateFileName(const juce::String& fileName)
{
    // どのスロットの話かが分かるよう、番号を頭に付ける
    fileNameLabel.setText(juce::String(targetSlot()).paddedLeft('0', 2) + ": " + (Io::isFileName(fileName) ? fileName : Io::empty),
        juce::dontSendNotification);

    // 名前とプレビューは常に同じサンプルを指していてほしいので、ここで揃える
    updateSamplePreview();
}

// 読み込んだサンプルの、実際に鳴る範囲を描く。
// 波形は 1 点ずつ拾っても形が分からないので、区間ごとの上下幅で出す。
void GuiAdpcmPlus::updateSamplePreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateSamplePreview(); })) return;

    const auto& data = ctx.audioProcessor.adpcmPlusPreviewBuffers[(size_t)targetSlot()];

    if (data.empty()) {
        samplePreview.clear();

        return;
    }

    auto env = WavePreviewSource::audioFile(
        data,
        ctx.audioProcessor.adpcmPlusPreviewRates[(size_t)targetSlot()],
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

bool GuiAdpcmPlus::isThis(juce::Button* button)
{
    return button == &loadButton;
}

bool GuiAdpcmPlus::isBtnPanL(juce::Button* button)
{
    return button == &panToLBtn;
}

bool GuiAdpcmPlus::isBtnPanC(juce::Button* button)
{
    return button == &panToCBtn;
}

bool GuiAdpcmPlus::isBtnPanR(juce::Button* button)
{
    return button == &panToRBtn;
}

void GuiAdpcmPlus::setPan(float pan)
{
    panSlider.setValue(pan);
}

void GuiAdpcmPlus::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

void GuiAdpcmPlus::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiAdpcmPlus::initParams()
{
    // 中で 32 スロットぶんの PCM も外れる
    this->ctx.audioProcessor.initParams("ADPCMP_");

    updateAllSlotPreviews();
    applySlotTarget();
}

void GuiAdpcmPlus::layoutUtilityCat(juce::Rectangle<int>& rect)
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

void GuiAdpcmPlus::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    loadButton.setVisible(visible);
    fileNameLabel.setVisible(visible);
    samplePreview.setVisible(visible);
    slotPreviews.setVisible(visible);
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

        // 名前の枠は、読み込みと取り外しのボタンを引いた残り全部を使う。
        // グループが本体より広いので、決め打ちの幅だと右が空いてしまう。
        layoutMainPcm({
            .rect = rect,
            .loadPcmBtn = &loadButton,
            .pcmFileNameLabel = &fileNameLabel,
            .clearPcmBtn = &clearButton,
            .pcmFileNameLabelWidth = rect.getWidth()
                - CoreGuiValue::MainGroup::Row::Pcm::LoadBtn::width
                - CoreGuiValue::MainGroup::Row::Pcm::ClearBtn::width,
        });

        samplePreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(3);

        slotPreviews.setBounds(rect.removeFromTop(slotPreviews.getNaturalHeight()));
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

void GuiAdpcmPlus::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityPcmComponent.layoutComponent(rect);
}

void GuiAdpcmPlus::layoutPanCat(juce::Rectangle<int>& rect)
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

void GuiAdpcmPlus::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    speedSlider.setVisibleWithLabel(visible);
    optSpeedSeparator.setVisible(visible);
    loopCountSlider.setVisibleWithLabel(visible);
    loopCountButtons.setVisibles(visible && loopCountSlider.isVisibleNudge());
    optCountSeparator.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    loopButton.setVisible(visible);
    optLoopSepTop.setVisible(visible);
    optLoopSepBottom.setVisible(visible);
    loopPointEnableButton.setVisible(visible);
    loopPointStartSlider.setVisibleWithLabel(visible);
    loopPointEndSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutMain({ .mainRect = rect, .label = &speedSlider.label, .component = &speedSlider });
        optSpeedSeparator.layoutComponent(rect);
        loopCountSlider.layoutComponent(rect);
        if (loopCountSlider.isVisibleNudge()) loopCountButtons.layoutComponent(rect);
        optCountSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutMain({ .mainRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
        optLoopSepTop.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &loopButton });

        optLoopSepBottom.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &loopPointEnableButton });
        layoutMain({ .mainRect = rect, .label = &loopPointStartSlider.label, .component = &loopPointStartSlider, });
        layoutMain({ .mainRect = rect, .label = &loopPointEndSlider.label, .component = &loopPointEndSlider, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiAdpcmPlus::setupGraph()
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

void GuiAdpcmPlus::setGraphMode(GraphMode mode)
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

void GuiAdpcmPlus::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(AdpcmPlusGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(AdpcmPlusGuiValue::MainGroup::Graph::ButtonHeight);
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
void GuiAdpcmPlus::updateGraph()
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

void GuiAdpcmPlus::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiAdpcmPlus::importToneNoiseParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultToneNoiseParamDir,
        { EditorGuiText::ParamBrowser::kindToneNoise },
        [this](const juce::File& file) { applyToneNoiseParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiAdpcmPlus::applyToneNoiseParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


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

void GuiAdpcmPlus::exportToneNoiseParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultToneNoiseParamDir,
        { EditorGuiText::ParamBrowser::kindToneNoise }, Io::Extension::ToneNoiseParam,
        [this](const juce::File& file) { writeToneNoiseParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiAdpcmPlus::writeToneNoiseParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(toneNoiseFormat);
    writeToneNoiseParams(writer);

    writer.writeTo(file);
}

void GuiAdpcmPlus::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindPcmQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiAdpcmPlus::applyQualityParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


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

void GuiAdpcmPlus::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindPcmQuality }, Io::Extension::PcmQualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiAdpcmPlus::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(pcmQualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

void GuiAdpcmPlus::importPcmPlayParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultPcmPlayParamDir,
        { EditorGuiText::ParamBrowser::kindPcmPlay },
        [this](const juce::File& file) { applyPcmPlayParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiAdpcmPlus::applyPcmPlayParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


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
    speedSlider.setValue(reader->getFloat("speed", (float)speedSlider.getValue()), juce::sendNotification);
    pcmRatioSlider.setValue(reader->getFloat("pcmRatio", (float)pcmRatioSlider.getValue()), juce::sendNotification);
    loopPointEnableButton.setToggleState(reader->getBool("loopPointEnable", loopPointEnableButton.getToggleState()), juce::sendNotification);
    loopPointStartSlider.setValue(reader->getFloat("loopPointStart", (float)loopPointStartSlider.getValue()), juce::sendNotification);
    loopPointEndSlider.setValue(reader->getFloat("loopPointEnd", (float)loopPointEndSlider.getValue()), juce::sendNotification);
    loopCountSlider.setValue(reader->getFloat("loopCount", (float)loopCountSlider.getValue()), juce::sendNotification);
}

void GuiAdpcmPlus::exportPcmPlayParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultPcmPlayParamDir,
        { EditorGuiText::ParamBrowser::kindPcmPlay }, Io::Extension::PcmPlayParam,
        [this](const juce::File& file) { writePcmPlayParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiAdpcmPlus::writePcmPlayParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(pcmPlayFormat);
    writePcmPlayParams(writer);

    writer.writeTo(file);
}

void GuiAdpcmPlus::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "PCM+" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiAdpcmPlus::applyChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    auto reader = Io::ParamReader::open(file, adpcmPlusFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    // Level
    levelComponent.readParams(*reader, "level");

    // 鳴らす PCM
    slotSlider.setValue(reader->getFloat("slot", (float)slotSlider.getValue()), juce::sendNotification);

    // FORM・OPTIONAL・PAN は PCM ごとの値は 32 本ぶんある。画面へ出ている
    // のは 1 組だけなので、つまみを通さずパラメータへ直に入れる。
    for (int i = 0; i < Global::AdpcmPlus::slots; ++i) {
        const juce::String slotPrefix = AdpcmPlusPrKey::prefix + CPK::AdpcmPlus::slot + juce::String(i);

        auto r = reader->child("slot" + juce::String(i));

        // 素材の場所はプロセッサが持っている
        auto path = Io::resolveSamplePath(r.getString("filePath", ctx.audioProcessor.adpcmPlusFilePaths[(size_t)i]),
            ctx.audioProcessor.defaultSampleDir);

        // 別のファイルへ変わるなら、いま持っているものを先に外す
        if (ctx.audioProcessor.adpcmPlusFilePaths[(size_t)i] != path) {
            ctx.audioProcessor.unloadAdpcmPlusFile(i);
        }

        if (Io::isFilePath(path)) {
            ctx.audioProcessor.loadAdpcmPlusFile(i, juce::File(path));
        }

        setParamValue(slotPrefix + CPK::Tn::tone,
            r.getFloat("tone", getParamValue(slotPrefix + CPK::Tn::tone)));
        setParamValue(slotPrefix + CPK::Tn::noise,
            r.getFloat("noise", getParamValue(slotPrefix + CPK::Tn::noise)));
        setParamValue(slotPrefix + CPK::Tn::freq,
            r.getFloat("noiseFreq", getParamValue(slotPrefix + CPK::Tn::freq)));
        setParamValue(slotPrefix + CPK::Tn::mix,
            r.getFloat("mix", getParamValue(slotPrefix + CPK::Tn::mix)));

        setParamValue(slotPrefix + CPK::pan,
            r.getFloat("pan", getParamValue(slotPrefix + CPK::pan)));

        setParamValue(slotPrefix + CPK::speed,
            r.getFloat("speed", getParamValue(slotPrefix + CPK::speed)));
        setParamValue(slotPrefix + CPK::pcmOffset,
            r.getFloat("pcmOffset", getParamValue(slotPrefix + CPK::pcmOffset)));
        setParamValue(slotPrefix + CPK::pcmRatio,
            r.getFloat("pcmRatio", getParamValue(slotPrefix + CPK::pcmRatio)));
        setParamValue(slotPrefix + CPK::loop,
            r.getBool("loop", getParamValue(slotPrefix + CPK::loop) > CPV::boolThread) ? 1.0f : 0.0f);
        setParamValue(slotPrefix + CPK::lpEnable,
            r.getBool("loopPointEnable", getParamValue(slotPrefix + CPK::lpEnable) > CPV::boolThread) ? 1.0f : 0.0f);
        setParamValue(slotPrefix + CPK::lpStart,
            r.getFloat("loopPointStart", getParamValue(slotPrefix + CPK::lpStart)));
        setParamValue(slotPrefix + CPK::lpEnd,
            r.getFloat("loopPointEnd", getParamValue(slotPrefix + CPK::lpEnd)));
        setParamValue(slotPrefix + CPK::lpCount,
            (float)r.getInt("loopCount", (int)getParamValue(slotPrefix + CPK::lpCount)));
    }

    // 名前と波形プレビューを、読み込んだものへそろえ直す
    updateAllSlotPreviews();
    applySlotTarget();

    slotPreviews.setActive((int)slotSlider.getValue());

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

void GuiAdpcmPlus::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "PCM+" }, Io::Extension::adpcmPlusParam,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiAdpcmPlus::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(adpcmPlusFormat);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiAdpcmPlus::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

    // 鳴らす PCM
    writer.set("slot", (float)slotSlider.getValue());

    // FORM・OPTIONAL・PAN は PCM ごとの値は 32 本ぶん
    for (int i = 0; i < Global::AdpcmPlus::slots; ++i) {
        const juce::String slotPrefix = AdpcmPlusPrKey::prefix + CPK::AdpcmPlus::slot + juce::String(i);

        auto w = writer.child("slot" + juce::String(i));

        // 名前ではなく場所を残す。読み戻すときに File を作れるようにするため。
        w.set("filePath", Io::toStoredFileName(ctx.audioProcessor.adpcmPlusFilePaths[(size_t)i]));

        w.set("tone", getParamValue(slotPrefix + CPK::Tn::tone));
        w.set("noise", getParamValue(slotPrefix + CPK::Tn::noise));
        w.set("noiseFreq", getParamValue(slotPrefix + CPK::Tn::freq));
        w.set("mix", getParamValue(slotPrefix + CPK::Tn::mix));

        w.set("pan", getParamValue(slotPrefix + CPK::pan));

        w.set("speed", getParamValue(slotPrefix + CPK::speed));
        w.set("pcmOffset", getParamValue(slotPrefix + CPK::pcmOffset));
        w.set("pcmRatio", getParamValue(slotPrefix + CPK::pcmRatio));
        w.set("loop", getParamValue(slotPrefix + CPK::loop) > CPV::boolThread);
        w.set("loopPointEnable", getParamValue(slotPrefix + CPK::lpEnable) > CPV::boolThread);
        w.set("loopPointStart", getParamValue(slotPrefix + CPK::lpStart));
        w.set("loopPointEnd", getParamValue(slotPrefix + CPK::lpEnd));
        w.set("loopCount", (int)getParamValue(slotPrefix + CPK::lpCount));
    }
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
void GuiAdpcmPlus::setImportingToneNoiseParams(juce::StringArray& lines, int& index) {
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
void GuiAdpcmPlus::writeToneNoiseParams(Io::ParamWriter& writer) {
	writer.set("tone", (float)toneSlider.getValue());
	writer.set("noise", (float)noiseSlider.getValue());
	writer.set("noiseFreq", (float)noiseFreqSlider.getValue());
	writer.set("mix", (float)mixSlider.getValue());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiAdpcmPlus::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	            if (size < 3) return;

	qualityPcmComponent.setMode(lines[0].getIntValue());
	            qualityPcmComponent.setRate(lines[1].getIntValue());
	            qualityPcmComponent.setInterp(lines[2].getIntValue());
	        
}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiAdpcmPlus::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("mode", qualityPcmComponent.getMode());
	writer.set("rate", qualityPcmComponent.getRate());
	writer.set("interp", qualityPcmComponent.getInterp());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiAdpcmPlus::setImportingPcmPlayParams(juce::StringArray& lines, int& index) {
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
void GuiAdpcmPlus::writePcmPlayParams(Io::ParamWriter& writer) {
	writer.set("pcmOffset", (float)pcmOffsetSlider.getValue());
	writer.set("speed", (float)speedSlider.getValue());
	writer.set("pcmRatio", (float)pcmRatioSlider.getValue());
	writer.set("loopPointEnable", loopPointEnableButton.getToggleState());
	writer.set("loopPointStart", (float)loopPointStartSlider.getValue());
	writer.set("loopPointEnd", (float)loopPointEndSlider.getValue());
	writer.set("loopCount", (float)loopCountSlider.getValue());

	
}

void GuiAdpcmPlus::bypassHiddenCategories()
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

void GuiAdpcmPlus::openEnabledCategories()
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

    // 大区分は中の区分に合わせる。効いている区分があれば開く。
    if (anyCategoryEnabled(ampEnvComponent, ssgHwEnv, ssgSwEnvComponent, ssgSwEnv11Component, ampModComponent)) ampMajorCat.setDetailVisible(true);
    if (anyCategoryEnabled(pitchEnvComponent, ssgHwPEnv, ssgSwPEnv11Component, modComponent)) pitchMajorCat.setDetailVisible(true);
}

void GuiAdpcmPlus::closeBypassedCategories()
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

    // 大区分は中の区分に合わせる。どれも切ってあれば閉じる。
    if (allCategoriesBypassed(ampEnvComponent, ssgHwEnv, ssgSwEnvComponent, ssgSwEnv11Component, ampModComponent)) ampMajorCat.setDetailVisible(false);
    if (allCategoriesBypassed(pitchEnvComponent, ssgHwPEnv, ssgSwPEnv11Component, modComponent)) pitchMajorCat.setDetailVisible(false);
}
