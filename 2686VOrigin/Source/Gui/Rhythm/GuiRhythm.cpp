#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiRhythm.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat pcmQualityFormat{ "pcmQuality", 1 };
	const Io::ParamFormat pcmPlayFormat{ "pcmPlay", 1 };
	const Io::ParamFormat rhythmFormat{ "rhythm", 1 };
	const Io::ParamFormat rhythmPadFormat{ "rhythmPad", 1 };
	const Io::ParamFormat toneNoiseFormat{ "toneNoise", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Rhythm/ProcessorRhythmKeys.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiRhythmValues.h"
#include "./GuiRhythmText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiRhythmHelpers.h"
#include "../../Core/Gui/GuiGraphValues.h"

#include "../../Core/Processor/PluginProcessorStateKey.h"

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);

    // 名前とプレビューは常に同じサンプルを指していてほしいので、ここで揃える
    updateSamplePreview();
}

// 読み込んだサンプルの、実際に鳴る範囲を描く。
// 波形は 1 点ずつ拾っても形が分からないので、区間ごとの上下幅で出す。
void RhythmPadGui::updateSamplePreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateSamplePreview(); })) return;

    const auto& data = ctx.audioProcessor.rhythmPreviewBuffers[m_padIndex];

    if (data.empty()) {
        samplePreview.clear();

        return;
    }

    auto env = WavePreviewSource::audioFile(
        data,
        ctx.audioProcessor.rhythmPreviewRates[m_padIndex],
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

void RhythmPadGui::setup(juce::Component &parent, int& tabOrder)
{
    // 組み立ての間だけ 1 番のパッドへ繋いでおく。
    // 実際の指し先は、組み上がったあと rebind で入れ替える。
    const int index = 0;

    auto setupPanBtn = [this](juce::Component& parent, GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            parent.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    parent.addAndMakeVisible(this);

    juce::String padPrefix = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(index);

    // ---- 区分ごとの枠 ----
    //
    // 縦に積まず、横へ並べる。1 列 1 区分が基本。
    stripViewport.setViewedComponent(&stripCanvas, false);
    stripViewport.setScrollBarsShown(false, true);
    stripViewport.setOpaque(false);

    addAndMakeVisible(stripViewport);

    colForm.setup(stripCanvas, juce::String("") + "FORM / PAN / QUALITY");
    colOptional.setup(stripCanvas, juce::String("") + "OPTIONAL");
    colAmpEnv.setup(stripCanvas, juce::String("") + "AMP ENV");
    colSsgHwEnv.setup(stripCanvas, juce::String("") + "SSG HW AMP ENV");
    colSsgSwEnv.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV");
    colSsgSwEnv11.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV[11]");
    colAmpMod.setup(stripCanvas, juce::String("") + "WT AMP MOD");
    colPitchEnv.setup(stripCanvas, juce::String("") + "PITCH ENV");
    colSsgHwPEnv.setup(stripCanvas, juce::String("") + "SSG HW PITCH ENV");
    colSsgSwPEnv11.setup(stripCanvas, juce::String("") + "SSG SW PITCH ENV[11]");
    colMod.setup(stripCanvas, juce::String("") + "WT PITCH MOD");
    colLfo.setup(stripCanvas, juce::String("") + "LFO");
    colMulDet.setup(stripCanvas, juce::String("") + "MUL/DET / FIX");

    // 今どのパッドを触っているか。名前は rebind で入れ替える。
    padNameLabel.setup({ .parent = *this, .title = Io::empty,
        .justification = juce::Justification::centredLeft });

    // TARGET。値が動いたら、入れ物の側が束縛と枠線を付け替える。
    padTarget.setup({ .parent = *this, .title = "TARGET", .isReset = false });
    padTarget.setRange(1.0, (double)RhythmPrValue::pads, 1.0);
    padTarget.setNumDecimalPlacesToDisplay(0);
    padTarget.setWantsKeyboardFocus(true);
    padTarget.setExplicitFocusOrder(++tabOrder);
    padTarget.onValueChange = [this] { if (onTargetChange) onTargetChange(); };

    padTargetSeparator.setupComponent(*this);

    // どのエンベロープを映すかの切り替え。TARGET のすぐ右へ置く。
    auto setupModeBtn = [this, &tabOrder](GuiToggleButton& btn, const juce::String& text,
        GuiEnvGraphMode mode) {
        btn.setup({ .parent = *this, .title = text, .isReset = false, .isResized = false });
        btn.setWantsKeyboardFocus(true);
        btn.setExplicitFocusOrder(++tabOrder);
        btn.onClick = [this, mode] { this->setGraphMode(mode); };
        };

    setupModeBtn(graphBtnAmp, "AMP", GuiEnvGraphMode::Amp);
    setupModeBtn(graphBtnPitch, "PIT", GuiEnvGraphMode::Pitch);
    setupModeBtn(graphBtnSsg, "SSG", GuiEnvGraphMode::SsgSw);
    setupModeBtn(graphBtnSsg11, "S11", GuiEnvGraphMode::SsgSw11);
    setupModeBtn(graphBtnSsgP11, "P11", GuiEnvGraphMode::SsgSwP11);

    graphBtnAmp.setToggleState(true, juce::dontSendNotification);

    formCat.setupHwCategory({ .parent = colForm.contentCanvas, .title = RhythmGuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

    qualityPcmComponent.setupComponent(colForm.contentCanvas, padPrefix, tabOrder);

    // 音声ファイルロードボタン
    loadButton.setup({ .parent = colForm.contentCanvas, .title = RhythmGuiText::File::load, .isReset = false });
    loadButton.addListener(&ctx.editor);
    loadButton.setWantsKeyboardFocus(true);
    loadButton.setExplicitFocusOrder(++tabOrder);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = colForm.contentCanvas, .title = Io::empty });

    m_padIndex = index;

    samplePreview.setup(colForm.contentCanvas, GuiColor::WavePreview::AudioFile);
    fileNameLabel.setJustificationType(juce::Justification::centred);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // パッド音声アンロード
    clearButton.setup({ .parent = colForm.contentCanvas, .title = RhythmGuiText::File::clear, .textColor = juce::Colours::white, .isReset = false });
    clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 1. 特定のパッドをアンロード
            ctx.audioProcessor.unloadRhythmFile(m_padIndex);

            // 2. ファイル名表示を更新
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    formSeparator.setupComponent(colForm.contentCanvas);
    optOneShotSepTop.setupComponent(colOptional.contentCanvas);
    optOneShotSepBottom.setupComponent(colOptional.contentCanvas);

    optionalCat.setupSwCategory({ .parent = colOptional.contentCanvas, .title = RhythmGuiText::Category::optional, .enableChangeDetailVisible = true });

    padDelaySlider.setupComponent(colOptional.contentCanvas, padPrefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

    padDelayNudge.setupComponent(colOptional.contentCanvas, padDelaySlider.getSlider(), tabOrder);

    optDelaySeparator.setupComponent(colOptional.contentCanvas);

    speedSlider.setup(GuiSlider::Config{ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::speed, .title = "SPEED", .isReset = true });
    speedSlider.setWantsKeyboardFocus(true);
    speedSlider.setExplicitFocusOrder(++tabOrder);

    optSpeedSeparator.setupComponent(colOptional.contentCanvas);

    loopCountSlider.setupComponent(colOptional.contentCanvas, padPrefix + CPK::lpCount, "CNT", tabOrder, std::nullopt);

    loopCountButtons.setupComponent(colOptional.contentCanvas, loopCountSlider.getSlider(), tabOrder);

    optCountSeparator.setupComponent(colOptional.contentCanvas);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::pcmOffset, .title = RhythmGuiText::Rhythm::Pad::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::pcmRatio, .title = RhythmGuiText::Rhythm::Pad::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEnableButton.setup({ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::lpEnable, .title = RhythmGuiText::Rhythm::Pad::loopPointEnable, .isReset = true });
    loopPointEnableButton.setWantsKeyboardFocus(true);
    loopPointEnableButton.setExplicitFocusOrder(++tabOrder);

    loopPointStartSlider.setup(GuiSlider::Config{ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::lpStart, .title = RhythmGuiText::Rhythm::Pad::loopPointStart, .isReset = true });
    loopPointStartSlider.setWantsKeyboardFocus(true);
    loopPointStartSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEndSlider.setup(GuiSlider::Config{ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::lpEnd, .title = RhythmGuiText::Rhythm::Pad::loopPointEnd, .isReset = true });
    loopPointEndSlider.setWantsKeyboardFocus(true);
    loopPointEndSlider.setExplicitFocusOrder(++tabOrder);

    // 切り出しとループの設定が変わったら、プレビューも合わせる。
    // 各スライダーの setup() より後に付けること。setup() は APVTS との
    // 束縛を張り直すので、先に付けると取りこぼす恐れがある。
    auto refreshSamplePreview = [this]() { this->updateSamplePreview(); };

    pcmOffsetSlider.onValueChange = refreshSamplePreview;
    pcmRatioSlider.onValueChange = refreshSamplePreview;
    loopPointStartSlider.onValueChange = refreshSamplePreview;
    loopPointEndSlider.onValueChange = refreshSamplePreview;
    loopPointEnableButton.onStateChange = refreshSamplePreview;

    updateSamplePreview();

    // Vol
    volSlider.setup({ .parent = colForm.contentCanvas, .id = padPrefix + CPK::vol, .title = RhythmGuiText::Rhythm::Pad::vol, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    toneSlider.setup({ .parent = colForm.contentCanvas, .id = padPrefix + CPK::Tn::tone, .title = RhythmGuiText::Rhythm::Pad::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = colForm.contentCanvas, .id = padPrefix + CPK::Tn::noise, .title = RhythmGuiText::Rhythm::Pad::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = colForm.contentCanvas, .id = padPrefix + CPK::Tn::freq, .title = RhythmGuiText::Rhythm::Pad::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = colForm.contentCanvas, .id = padPrefix + CPK::Tn::mix , .title = RhythmGuiText::Rhythm::Pad::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = colForm.contentCanvas, .title = RhythmGuiText::Rhythm::Pad::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = colForm.contentCanvas, .title = RhythmGuiText::Rhythm::Pad::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = colForm.contentCanvas, .title = RhythmGuiText::Rhythm::Pad::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::oneShot, .title = RhythmGuiText::Rhythm::Pad::oneShot, .isReset = true });
    oneShotButton.setWantsKeyboardFocus(true);
    oneShotButton.setExplicitFocusOrder(++tabOrder);

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = colOptional.contentCanvas, .id = padPrefix + CPK::note, .title = RhythmGuiText::Rhythm::Pad::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.setWantsKeyboardFocus(true);
    noteSlider.setExplicitFocusOrder(++tabOrder);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    panCat.setupHwCategory({ .parent = colForm.contentCanvas, .title = RhythmGuiText::Category::pan, .enableChangeDetailVisible = true });

    // パンポット
    panSlider.setup({ .parent = colForm.contentCanvas, .id = padPrefix + CPK::pan, .title = RhythmGuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);
    panSlider.setRange(0.0f, 1.0f);

    panToLBtn.setup(GuiTextButton::Config{ .parent = colForm.contentCanvas, .id = "", .title = RhythmGuiText::Rhythm::Pad::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(0.0f, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = colForm.contentCanvas, .id = "", .title = RhythmGuiText::Rhythm::Pad::Pan::c, .isReset = false });
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0.5f, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = colForm.contentCanvas, .id = "", .title = RhythmGuiText::Rhythm::Pad::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1.0f, juce::sendNotification);
        };

    fixComponent.setupComponent(colMulDet.contentCanvas, padPrefix, tabOrder, "-> 440", 440);

    ampEnvComponent.setupComponent(colAmpEnv.contentCanvas, padPrefix, tabOrder);

    pitchEnvComponent.setupComponent(colPitchEnv.contentCanvas, padPrefix, tabOrder, CPK::pitchAdsr + CPK::bypass, RhythmGuiText::Rhythm::Pad::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(colSsgSwEnv.contentCanvas, padPrefix, tabOrder, CPK::ssgSwEnv + CPK::bypass, RhythmGuiText::Rhythm::Pad::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(colSsgSwEnv11.contentCanvas, padPrefix, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, RhythmGuiText::Rhythm::Pad::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(colSsgSwPEnv11.contentCanvas, padPrefix, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, RhythmGuiText::Rhythm::Pad::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(colMulDet.contentCanvas, padPrefix, tabOrder);

    lfoComponent.setupComponent(colLfo.contentCanvas, padPrefix, tabOrder);

    ssgHwEnv.setupComponent(colSsgHwEnv.contentCanvas, padPrefix, tabOrder);
    ssgHwPEnv.setupComponent(colSsgHwPEnv.contentCanvas, padPrefix, tabOrder);
    modComponent.setupComponent(colMod.contentCanvas, padPrefix, tabOrder);
    ampModComponent.setupComponent(colAmpMod.contentCanvas, padPrefix, tabOrder);

    setupGraph();

    // 区分の中身は最初から開いておく。1 列 1 区分にしたので、
    // 畳んだままだと見出しだけの列が並ぶことになる。
    for (auto* group : {
        &colForm, &colOptional, &colAmpEnv, &colSsgHwEnv,
        &colSsgSwEnv, &colSsgSwEnv11, &colAmpMod, &colPitchEnv,
        &colSsgHwPEnv, &colSsgSwPEnv11, &colMod, &colLfo,
        &colMulDet,
        })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    auto area = content;

    // 一番上の帯。TARGET、映すものの切り替え、今指しているパッドの名前。
    // その下へ区切り線を引いて、設定と切り離す。
    {
        auto row = area.removeFromTop(20);
        auto slot = row.removeFromLeft(RhythmGuiValue::Pad::Col::width);

        layoutMain({ .mainRect = slot, .label = &padTarget.label, .component = &padTarget });

        row.removeFromLeft(RhythmGuiValue::Pad::Col::gap);

        // TARGET のすぐ右が、いま指しているものの名前。
        padNameLabel.setBounds(row.removeFromLeft(RhythmGuiValue::Pad::Col::nameWidth).withHeight(18));

        // 名前と切り替えの間だけを空ける。
        row.removeFromLeft(RhythmGuiValue::Pad::Col::nameGap);

        // その右が、どのエンベロープを映すかの切り替え。
        {
            const int w = RhythmGuiValue::Pad::Graph::ModeButtonWidth;

            auto btnRow = row.removeFromLeft(w * 5).withHeight(RhythmGuiValue::Pad::Graph::ButtonHeight);

            graphBtnAmp.setBounds(btnRow.removeFromLeft(w));
            graphBtnPitch.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg11.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsgP11.setBounds(btnRow);
        }
    }

    padTargetSeparator.layoutComponent(area);

    // ---- 区分は横へ並べる ----
    //
    // 縦に積むと、下のほうの区分が畳まれた見出しの列に埋もれて
    // 見つけられない。横に並べれば、どの区分も丈をいっぱいに使える。
    // あふれたぶんは横へ送る。
    stripViewport.setBounds(area);

    const int colW = RhythmGuiValue::Pad::Col::width;
    const int colGap = RhythmGuiValue::Pad::Col::gap;

    // 横の送り棒が出るぶんだけ、中身の丈を短くする。
    const int colH = juce::jmax(1, area.getHeight() - stripViewport.getScrollBarThickness());

    int x = 0;

    // 1 列を切り出して、中身を上から積む。
    // 簡易表示モードで隠す区分は、列ごと出さない。隠したぶんだけ
    // 右の列が左へ詰まる。
    auto layoutCol = [&](GuiScrollGroup& group, bool show, auto&& body)
        {
            if (!show) {
                group.setVisible(false);

                return;
            }

            group.setVisible(true);

            juce::Rectangle<int> colArea(x, 0, colW, colH);

            x += colW + colGap;

            group.setBounds(colArea);

            auto inner = colArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);

            inner.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

            group.setViewportCustomBounds(inner.translated(-colArea.getX(), -colArea.getY()));

            juce::Rectangle<int> rect(0, 0, group.getContentWidth(), 20000);

            body(rect);

            group.setContentHeight(rect.getY() + 20);
        };

    const auto shown = [this](SimpleView::Cat cat) { return ctx.audioProcessor.isSimpleShown(cat); };

    // FORM・PAN・QUALITY は 1 区分ずつでは丈が余るので、1 列へまとめてある。
    layoutCol(colForm, true, [&](juce::Rectangle<int>& rect) {
        layoutRow({ .rowRect = rect, .label = &volSlider.label, .component = &volSlider });
        layoutFormCat(rect);
        layoutPanCat(rect);
        layoutQualityCat(rect);
        });

    layoutCol(colOptional, true, [&](juce::Rectangle<int>& rect) { layoutOptionalCat(rect); });

    layoutCol(colAmpEnv, shown(SimpleView::AmpEnv), [&](juce::Rectangle<int>& rect) {
        ampEnvComponent.setCategoryVisible(true);
        ampEnvComponent.layoutComponent(rect);
        });

    layoutCol(colSsgHwEnv, shown(SimpleView::SsgHwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwEnv.setCategoryVisible(true);
        ssgHwEnv.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv, shown(SimpleView::SsgSwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgSwEnvComponent.setCategoryVisible(true);
        ssgSwEnvComponent.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv11, shown(SimpleView::SsgSwAmpEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv11Component.setCategoryVisible(true);
        ssgSwEnv11Component.layoutComponent(rect);
        });

    layoutCol(colAmpMod, shown(SimpleView::WtAmpMod), [&](juce::Rectangle<int>& rect) {
        ampModComponent.setCategoryVisible(true);
        ampModComponent.layoutComponent(rect);
        });

    layoutCol(colPitchEnv, shown(SimpleView::PitchEnv), [&](juce::Rectangle<int>& rect) {
        pitchEnvComponent.setCategoryVisible(true);
        pitchEnvComponent.layoutComponent(rect);
        });

    layoutCol(colSsgHwPEnv, shown(SimpleView::SsgHwPitchEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwPEnv.setCategoryVisible(true);
        ssgHwPEnv.layoutComponent(rect);
        });

    layoutCol(colSsgSwPEnv11, shown(SimpleView::SsgSwPitchEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwPEnv11Component.setCategoryVisible(true);
        ssgSwPEnv11Component.layoutComponent(rect);
        });

    layoutCol(colMod, true, [&](juce::Rectangle<int>& rect) { modComponent.layoutComponent(rect); });

    layoutCol(colLfo, shown(SimpleView::Lfo), [&](juce::Rectangle<int>& rect) {
        lfoComponent.setCategoryVisible(true);
        lfoComponent.layoutComponent(rect);
        });

    // MUL/DET と FIX も 1 列へまとめてある。どちらかだけを隠すことが
    // できるので、列を出すかどうかは両方を見て決める。
    layoutCol(colMulDet, shown(SimpleView::MulDet) || shown(SimpleView::Fix), [&](juce::Rectangle<int>& rect) {
        mulDetuneComponent.setCategoryVisible(shown(SimpleView::MulDet));
        mulDetuneComponent.layoutComponent(rect);

        fixComponent.setCategoryVisible(shown(SimpleView::Fix));
        fixComponent.layoutComponent(rect);
        });

    stripCanvas.setSize(juce::jmax(1, x - colGap), colH);
}

void RhythmPadGui::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

bool RhythmPadGui::isThis(juce::Button* button)
{
    return button == &loadButton;
}

void RhythmPadGui::layoutFormCat(Rectangle<int>& rect) {
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
        layoutRowRhythmPadPcmFile({ .rect = rect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton });

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

void RhythmPadGui::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityPcmComponent.layoutComponent(rect);
}

void RhythmPadGui::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutRowThreeComps({
            .rect = rect,
            .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn
            });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void RhythmPadGui::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    oneShotButton.setVisible(visible);
    optOneShotSepTop.setVisible(visible);
    optOneShotSepBottom.setVisible(visible);
    padDelaySlider.setVisibles(visible);
    padDelayNudge.setVisibles(visible && padDelaySlider.isVisibleNudge());
    optDelaySeparator.setVisible(visible);
    speedSlider.setVisibleWithLabel(visible);
    optSpeedSeparator.setVisible(visible);
    loopCountSlider.setVisibleWithLabel(visible);
    loopCountButtons.setVisibles(visible && loopCountSlider.isVisibleNudge());
    optCountSeparator.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    loopPointEnableButton.setVisible(visible);
    loopPointStartSlider.setVisibleWithLabel(visible);
    loopPointEndSlider.setVisibleWithLabel(visible);
    noteSlider.setVisibleWithLabel(visible);

    if (visible) {
        padDelaySlider.layoutComponentRow(rect);

        if (padDelaySlider.isVisibleNudge()) padDelayNudge.layoutComponentRow(rect);

        optDelaySeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &speedSlider.label, .component = &speedSlider });
        optSpeedSeparator.layoutComponent(rect);
        loopCountSlider.layoutComponentRow(rect);
        if (loopCountSlider.isVisibleNudge()) loopCountButtons.layoutComponentRow(rect);
        optCountSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutRow({ .rowRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
        optOneShotSepTop.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &oneShotButton });

        optOneShotSepBottom.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &loopPointEnableButton });
        layoutRow({ .rowRect = rect, .label = &loopPointStartSlider.label, .component = &loopPointStartSlider, });
        layoutRow({ .rowRect = rect, .label = &loopPointEndSlider.label, .component = &loopPointEndSlider, });
        layoutRow({ .rowRect = rect, .label = &noteSlider.label, .component = &noteSlider, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// つまみが動いたら、上の絵を描き直してもらうためのつなぎ。
//
// グラフそのものはここには無い。パッドの数だけ同時に出るので、
// 設定の側で持つわけにいかない。
void RhythmPadGui::setupGraph()
{
    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return;

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        if (this->onParamsChanged) this->onParamsChanged();
        };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    ssgSwEnv11Component.setupGraph(repaintGraph);

    ssgSwPEnv11Component.setupGraph(repaintGraph);
}

// 枠に出す波形を作り直す。
//
// 枠はパッドの数だけ同時に出るので、つまみから値を読むわけにいかない。
// 接頭辞を頼りにパラメータから直に採る。
void GuiRhythm::updatePadPreview(int p)
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this, p] { updatePadPreview(p); })) return;

    auto& cell = cells[(size_t)p];
    const auto& data = ctx.audioProcessor.rhythmPreviewBuffers[p];

    if (data.empty()) {
        cell.preview().clear();

        return;
    }

    auto& apvts = ctx.audioProcessor.apvts;
    const juce::String code = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(p);

    auto env = WavePreviewSource::audioFile(
        data,
        ctx.audioProcessor.rhythmPreviewRates[p],
        GuiGraphValues::value(apvts, code + CPK::pcmOffset),
        GuiGraphValues::value(apvts, code + CPK::pcmRatio));

    cell.preview().setEnvelope(env.mins, env.maxs);

    // ループ位置は切り出した範囲に対する 0.0〜1.0。使うときだけ出す。
    std::vector<float> markers;

    if (GuiGraphValues::flag(apvts, code + CPK::lpEnable)) {
        markers.push_back(GuiGraphValues::value(apvts, code + CPK::lpStart));
        markers.push_back(GuiGraphValues::value(apvts, code + CPK::lpEnd));
    }

    cell.preview().setMarkers(markers);
}

// 枠に出すエンベロープを描き直す。
void GuiRhythm::updatePadGraph(int p)
{
    auto& apvts = ctx.audioProcessor.apvts;
    auto& graph = cells[(size_t)p].graph();

    const juce::String code = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(p);
    const GuiEnvGraphMode mode = padPanel.graphMode();

    if (mode == GuiEnvGraphMode::Pitch) {
        // KEEP のときはカーブを効かせない。音の側も補間そのものを止めてある。
        const bool keepOn = GuiGraphValues::pitchEnvKeep(apvts, code);

        graph.updateBypass(GuiGraphValues::flag(apvts, code + CPK::pitchAdsr + CPK::bypass));
        graph.setKeepLevels(keepOn);
        graph.updatePitchEnv(GuiGraphValues::pitchEnv(apvts, code));
    }
    else if (mode == GuiEnvGraphMode::SsgSw) {
        const auto v = GuiGraphValues::ssgSwEnv(apvts, code);

        graph.updateBypass(GuiGraphValues::flag(apvts, code + CPK::ssgSwEnv + CPK::bypass));
        graph.updateSsgSwEnv(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    else if (mode == GuiEnvGraphMode::SsgSw11) {
        const auto v = GuiGraphValues::ssgSwEnv11(apvts, code);
        const bool keepOn = GuiGraphValues::ssgSwEnv11Keep(apvts, code);

        graph.updateBypass(GuiGraphValues::flag(apvts, code + CPK::ssgSwEnv11 + CPK::bypass));
        graph.setKeepLevels(keepOn);
        graph.updateSsgSwEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    else if (mode == GuiEnvGraphMode::SsgSwP11) {
        const auto v = GuiGraphValues::ssgSwPEnv11(apvts, code);
        const bool keepOn = GuiGraphValues::ssgSwPEnv11Keep(apvts, code);

        graph.updateBypass(GuiGraphValues::flag(apvts, code + CPK::ssgSwPEnv11 + CPK::bypass));
        graph.setKeepLevels(keepOn);
        graph.updateSsgSwPEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    else {
        graph.updateBypass(GuiGraphValues::ampEnvBypassed(apvts, code));
        graph.updateAmpEnv(GuiGraphValues::ampEnv(apvts, code));
    }

    graph.repaint();
}

// 絵をまとめて切り替える。
//
// 絵そのものは入れ物のほうが持っているので、決めたことだけを伝える。
void RhythmPadGui::setGraphMode(GuiEnvGraphMode mode)
{
    currentGraphMode = mode;

    // 札は排他。押したものだけを入れる。
    graphBtnAmp.setToggleState(mode == GuiEnvGraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GuiEnvGraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GuiEnvGraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GuiEnvGraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GuiEnvGraphMode::SsgSwP11, juce::dontSendNotification);

    if (onGraphModeChange) onGraphModeChange(mode);
}

// TARGET が指すパッドへ、設定の束縛を丸ごと移す。
//
// パッドごとに区分をひとそろい並べるのをやめたので、指し先はここで
// 差し替える。見た目は組み直さない。
void RhythmPadGui::rebind(int index)
{
    m_padIndex = index;

    const juce::String padPrefix = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(index);

    padNameLabel.setText(RhythmGuiText::Group::padPrefix + " " + juce::String(index + 1)
        + " (" + RhythmGuiText::padNames[(size_t)index] + ")", juce::dontSendNotification);

    qualityPcmComponent.rebind(padPrefix);

    padDelaySlider.getSlider().rebind(padPrefix + CPK::delay);
    speedSlider.rebind(padPrefix + CPK::speed);
    loopCountSlider.getSlider().rebind(padPrefix + CPK::lpCount);
    pcmOffsetSlider.rebind(padPrefix + CPK::pcmOffset);
    pcmRatioSlider.rebind(padPrefix + CPK::pcmRatio);
    loopPointEnableButton.rebind(padPrefix + CPK::lpEnable);
    loopPointStartSlider.rebind(padPrefix + CPK::lpStart);
    loopPointEndSlider.rebind(padPrefix + CPK::lpEnd);

    volSlider.rebind(padPrefix + CPK::vol);
    toneSlider.rebind(padPrefix + CPK::Tn::tone);
    noiseSlider.rebind(padPrefix + CPK::Tn::noise);
    noiseFreqSlider.rebind(padPrefix + CPK::Tn::freq);
    mixSlider.rebind(padPrefix + CPK::Tn::mix);
    oneShotButton.rebind(padPrefix + CPK::oneShot);
    noteSlider.rebind(padPrefix + CPK::note);
    panSlider.rebind(padPrefix + CPK::pan);

    fixComponent.rebind(padPrefix);
    ampEnvComponent.rebind(padPrefix);
    pitchEnvComponent.rebind(padPrefix);
    ssgSwEnvComponent.rebind(padPrefix);
    ssgSwEnv11Component.rebind(padPrefix);
    ssgSwPEnv11Component.rebind(padPrefix);
    mulDetuneComponent.rebind(padPrefix);
    lfoComponent.rebind(padPrefix);
    ssgHwEnv.rebind(padPrefix);
    ssgHwPEnv.rebind(padPrefix);
    modComponent.rebind(padPrefix);
    ampModComponent.rebind(padPrefix);

    // 読み込んでいるファイルの名前と波形も、指し先のものへ。
    updateFileNameFromProcessor();
}

// 今指しているパッドが読んでいるファイルの名前を、処理側から取り直す。
//
// 名前は画面が覚えているのではなく、処理側の持ち物。指し先を変えた
// ときは、そちらから読み直さないと前のパッドの名前が残る。
void RhythmPadGui::updateFileNameFromProcessor()
{
    const juce::String path = ctx.audioProcessor.rhythmFilePaths[(size_t)m_padIndex];

    updatePadFileName(path.isNotEmpty()
        ? ctx.audioProcessor.resolvePath(path).getFileName()
        : Io::empty);
}

void RhythmPadGui::copyParams(CopyRhythmPad& copyObj) {
    copyObj.base.level = volSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();
    copyObj.isOneShot = oneShotButton.getToggleState();
    copyObj.noteNumber = noteSlider.getValue();
    copyObj.pcm.pcmOffset = pcmOffsetSlider.getValue();
    copyObj.pcm.speed = speedSlider.getValue();
    copyObj.pcm.pcmRatio = pcmRatioSlider.getValue();
    copyObj.quality.mode = qualityPcmComponent.getMode();
    copyObj.quality.rate = qualityPcmComponent.getRate();
    copyObj.toneLevel = toneSlider.getValue();
    copyObj.noiseLevel = noiseSlider.getValue();
    copyObj.noiseFreq = noiseFreqSlider.getValue();
    copyObj.mix = mixSlider.getValue();

    ampEnvComponent.copyParams(copyObj.aAdsr);
    pitchEnvComponent.copyParams(copyObj.pAdsr);
}

void RhythmPadGui::pasteParams(CopyRhythmPad& copyObj) {
    volSlider.setValue(copyObj.base.level, juce::sendNotification);
    panSlider.setValue(copyObj.pan.pan, juce::sendNotification);
    oneShotButton.setToggleState(copyObj.isOneShot, juce::sendNotification);
    noteSlider.setValue(copyObj.noteNumber, juce::sendNotification);
    pcmOffsetSlider.setValue(copyObj.pcm.pcmOffset, juce::sendNotification);
    speedSlider.setValue(copyObj.pcm.speed, juce::sendNotification);
    pcmRatioSlider.setValue(copyObj.pcm.pcmRatio, juce::sendNotification);
    qualityPcmComponent.setMode(copyObj.quality.mode);
    qualityPcmComponent.setRate(copyObj.quality.rate);
    toneSlider.setValue(copyObj.toneLevel);
    noiseSlider.setValue(copyObj.noiseLevel);
    noiseFreqSlider.setValue(copyObj.noiseFreq);
    mixSlider.setValue(copyObj.mix);

    ampEnvComponent.pasteParams(copyObj.aAdsr);
    pitchEnvComponent.pasteParams(copyObj.pAdsr);
}

void RhythmPadGui::importToneNoiseParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultToneNoiseParamDir,
        { EditorGuiText::ParamBrowser::kindToneNoise },
        [this](const juce::File& file) { applyToneNoiseParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void RhythmPadGui::applyToneNoiseParamFile(const juce::File& file)
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

void RhythmPadGui::exportToneNoiseParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultToneNoiseParamDir,
        { EditorGuiText::ParamBrowser::kindToneNoise }, Io::Extension::ToneNoiseParam,
        [this](const juce::File& file) { writeToneNoiseParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void RhythmPadGui::writeToneNoiseParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(toneNoiseFormat);
    writeToneNoiseParams(writer);

    writer.writeTo(file);
}

void RhythmPadGui::importLfoParam() {
    lfoComponent.importParams();
}

void RhythmPadGui::exportLfoParam() {
    lfoComponent.exportParams();
}

void RhythmPadGui::importAmpEnvParam() {
    ampEnvComponent.importParams();
}

void RhythmPadGui::exportAmpEnvParam() {
    ampEnvComponent.exportParams();
}

void RhythmPadGui::importPitchEnvParam() {
    pitchEnvComponent.importParams();
}

void RhythmPadGui::exportPitchEnvParam() {
    pitchEnvComponent.exportParams();
}

void RhythmPadGui::importWtModParam() { modComponent.importParams(); }

void RhythmPadGui::exportWtModParam() { modComponent.exportParams(); }

void RhythmPadGui::importWtAmpModParam() { ampModComponent.importParams(); }

void RhythmPadGui::exportWtAmpModParam() { ampModComponent.exportParams(); }

void RhythmPadGui::importSsgHwEnvParam() {
    ssgHwEnv.importParams();
}

void RhythmPadGui::exportSsgHwEnvParam() {
    ssgHwEnv.exportParams();
}

void RhythmPadGui::importSsgHwPEnvParam() {
    ssgHwPEnv.importParams();
}

void RhythmPadGui::exportSsgHwPEnvParam() {
    ssgHwPEnv.exportParams();
}

void RhythmPadGui::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void RhythmPadGui::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void RhythmPadGui::importSsgSwEnv11Param() {
    ssgSwEnv11Component.importParams();
}

void RhythmPadGui::exportSsgSwEnv11Param() {
    ssgSwEnv11Component.exportParams();
}

void RhythmPadGui::importSsgSwPEnv11Param() {
    ssgSwPEnv11Component.importParams();
}

void RhythmPadGui::exportSsgSwPEnv11Param() {
    ssgSwPEnv11Component.exportParams();
}

void RhythmPadGui::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void RhythmPadGui::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void RhythmPadGui::importPcmPlayParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindPcmPlay },
        [this](const juce::File& file) { applyPcmPlayParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void RhythmPadGui::applyPcmPlayParamFile(const juce::File& file)
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

            setImportingPcmPlayParams(lines, index);
        }

        Io::ParamWriter writer(pcmPlayFormat);

        writePcmPlayParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, pcmPlayFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    pcmOffsetSlider.setValue(reader->getFloat("pcmOffset", (float)pcmOffsetSlider.getValue()), juce::sendNotification);
    padDelaySlider.setValue(reader->getFloat("delay", (float)padDelaySlider.getValue()), juce::sendNotification);
    speedSlider.setValue(reader->getFloat("speed", (float)speedSlider.getValue()), juce::sendNotification);
    pcmRatioSlider.setValue(reader->getFloat("pcmRatio", (float)pcmRatioSlider.getValue()), juce::sendNotification);
    loopPointEnableButton.setToggleState(reader->getBool("loopPointEnable", loopPointEnableButton.getToggleState()), juce::sendNotification);
    loopPointStartSlider.setValue(reader->getFloat("loopPointStart", (float)loopPointStartSlider.getValue()), juce::sendNotification);
    loopPointEndSlider.setValue(reader->getFloat("loopPointEnd", (float)loopPointEndSlider.getValue()), juce::sendNotification);
    loopCountSlider.setValue(reader->getFloat("loopCount", (float)loopCountSlider.getValue()), juce::sendNotification);
}

void RhythmPadGui::exportPcmPlayParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindPcmPlay }, Io::Extension::PcmPlayParam,
        [this](const juce::File& file) { writePcmPlayParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void RhythmPadGui::writePcmPlayParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(pcmPlayFormat);
    writePcmPlayParams(writer);

    writer.writeTo(file);
}

void RhythmPadGui::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindPcmQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void RhythmPadGui::applyQualityParamFile(const juce::File& file)
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

void RhythmPadGui::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindPcmQuality }, Io::Extension::PcmQualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void RhythmPadGui::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(pcmQualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

// パッド 1 つぶん。並びの中のひとつを渡してもらう。
void RhythmPadGui::readParams(int p, const Io::ParamReader& r) {
    // 場所はプロセッサが持っているものを使う。ラベルはファイル名だけを
    // 出しているので、そこから File を作ることはできない。
    auto path = Io::resolveSamplePath(r.getString("filePath", ctx.audioProcessor.rhythmFilePaths[p]), ctx.audioProcessor.defaultSampleDir);

    // 別のファイルへ変わるなら、いま持っているものを先に外す
    if (ctx.audioProcessor.rhythmFilePaths[p] != path) {
        ctx.audioProcessor.unloadRhythmFile(p);
    }

    // 名前は updatePadFileName を通す。ラベルだけを書き換えると、
    // 波形プレビューが前のサンプルを指したまま残るため。
    if (Io::isFilePath(path)) {
        juce::File target(path);

        ctx.audioProcessor.loadRhythmFile(target, p);
        updatePadFileName(target.getFileName());
    }
    else {
        updatePadFileName(Io::empty);
    }

    volSlider.setValue(r.getFloat("vol", (float)volSlider.getValue()), juce::sendNotification);
    panSlider.setValue(r.getFloat("pan", (float)panSlider.getValue()), juce::sendNotification);
    oneShotButton.setToggleState(r.getBool("oneShot", oneShotButton.getToggleState()), juce::sendNotification);
    noteSlider.setValue(r.getInt("note", (int)noteSlider.getValue()), juce::sendNotification);
    toneSlider.setValue(r.getFloat("tone", (float)toneSlider.getValue()), juce::sendNotification);
    noiseSlider.setValue(r.getFloat("noise", (float)noiseSlider.getValue()), juce::sendNotification);
    noiseFreqSlider.setValue(r.getFloat("noiseFreq", (float)noiseFreqSlider.getValue()), juce::sendNotification);
    mixSlider.setValue(r.getFloat("mix", (float)mixSlider.getValue()), juce::sendNotification);
    pcmOffsetSlider.setValue(r.getFloat("pcmOffset", (float)pcmOffsetSlider.getValue()), juce::sendNotification);
    padDelaySlider.setValue(r.getFloat("delay", (float)padDelaySlider.getValue()), juce::sendNotification);
    speedSlider.setValue(r.getFloat("speed", (float)speedSlider.getValue()), juce::sendNotification);
    pcmRatioSlider.setValue(r.getFloat("pcmRatio", (float)pcmRatioSlider.getValue()), juce::sendNotification);
    loopPointEnableButton.setToggleState(r.getBool("loopPointEnable", loopPointEnableButton.getToggleState()), juce::sendNotification);
    loopPointStartSlider.setValue(r.getFloat("loopPointStart", (float)loopPointStartSlider.getValue()), juce::sendNotification);
    loopPointEndSlider.setValue(r.getFloat("loopPointEnd", (float)loopPointEndSlider.getValue()), juce::sendNotification);
    loopCountSlider.setValue(r.getFloat("loopCount", (float)loopCountSlider.getValue()), juce::sendNotification);

    // Components
    fixComponent.readParams(r, "fix");
    ampEnvComponent.readParams(r, "ampEnv");
    pitchEnvComponent.readParams(r, "pitchEnv");
    ssgHwEnv.readParams(r, "ssgHwEnv");
    ssgHwPEnv.readParams(r, "ssgHwPEnv");
    modComponent.readParams(r, "wtMod");
    ampModComponent.readParams(r, "wtAmpMod");
    ssgSwEnvComponent.readParams(r, "ssgSwEnv");
    ssgSwEnv11Component.readParams(r, "ssgSwEnv11");
    ssgSwPEnv11Component.readParams(r, "ssgSwPEnv11");
    mulDetuneComponent.readParams(r, "mulDetune");
    lfoComponent.readParams(r, "lfo");
    qualityPcmComponent.readParams(r, "qualityPcm");
}

void RhythmPadGui::writeParams(int p, Io::ParamWriter& w) {
    // 名前ではなく場所を残す。読み戻すときに File を作れるようにするため。
    w.set("filePath", Io::toStoredFileName(ctx.audioProcessor.rhythmFilePaths[p]));

    w.set("vol", (float)volSlider.getValue());
    w.set("pan", (float)panSlider.getValue());
    w.set("oneShot", oneShotButton.getToggleState());
    w.set("note", (int)noteSlider.getValue());
    w.set("tone", (float)toneSlider.getValue());
    w.set("noise", (float)noiseSlider.getValue());
    w.set("noiseFreq", (float)noiseFreqSlider.getValue());
    w.set("mix", (float)mixSlider.getValue());
    w.set("pcmOffset", (float)pcmOffsetSlider.getValue());
    w.set("delay", (float)padDelaySlider.getValue());
    w.set("speed", (float)speedSlider.getValue());
    w.set("pcmRatio", (float)pcmRatioSlider.getValue());
    w.set("loopPointEnable", loopPointEnableButton.getToggleState());
    w.set("loopPointStart", (float)loopPointStartSlider.getValue());
    w.set("loopPointEnd", (float)loopPointEndSlider.getValue());
    w.set("loopCount", (float)loopCountSlider.getValue());

    // Components
    fixComponent.writeParams(w, "fix");
    ampEnvComponent.writeParams(w, "ampEnv");
    pitchEnvComponent.writeParams(w, "pitchEnv");
    ssgHwEnv.writeParams(w, "ssgHwEnv");
    ssgHwPEnv.writeParams(w, "ssgHwPEnv");
    modComponent.writeParams(w, "wtMod");
    ampModComponent.writeParams(w, "wtAmpMod");
    ssgSwEnvComponent.writeParams(w, "ssgSwEnv");
    ssgSwEnv11Component.writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11Component.writeParams(w, "ssgSwPEnv11");
    mulDetuneComponent.writeParams(w, "mulDetune");
    lfoComponent.writeParams(w, "lfo");
    qualityPcmComponent.writeParams(w, "qualityPcm");
}

GuiRhythm::GuiRhythm(const GuiContext& context) :
    GuiBase(context),
    mainGroup(context),
    presetName(context),
    levelComponent(context),
    unisonComponent(context),
    midiComponent(context),
    utilityCat(context),
    broadcastLevelButton(context),
    uSep001(context),
    copyPadParamBtn(context),
    copyPadFromSlider(context),
    copyPadToSlider(context),
    uSep002(context),
    ieToneNoise(context),
    ieLfo(context),
    ieAmpEnv(context),
    iePitchEnv(context),
    ieSsgHwEnv(context),
    ieSsgHwPEnv(context),
    ieWtMod(context),
    ieWtAmpMod(context),
    ieSsgSwEnv(context),
    ieSsgSwEnv11(context),
    ieSsgSwPEnv11(context),
    ieDetune(context),
    ieQuality(context),
    iePcmPlay(context),
	ieChPadParam(context),
    uSep003(context),
    ieUnison(context),
	ieChParam(context),
    padPanel(context),
    cells{ { {context}, {context}, {context}, {context}, {context}, {context} } }
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
}


// TARGET を鍵で動かす。中身は GuiBase の共通の手続きへ預けてある。
bool GuiRhythm::keyPressed(const juce::KeyPress& key)
{
    return moveTargetByKey(padPanel.targetSlider(), key);
}
void GuiRhythm::setup()
{
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;


    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    copyPadParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Pad Params", .bgColor = juce::Colours::turquoise.darker(0.3f) });
    copyPadParamBtn.setWantsKeyboardFocus(true);
    copyPadParamBtn.setExplicitFocusOrder(++tabOrder);
    copyPadParamBtn.onClick = [this] {
        int from = copyPadFromSlider.getValue() - 1;
        int to = copyPadToSlider.getValue() - 1;

        ctx.editor.copyRhythmPadParams(from, to);
        };

    copyPadFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyPadFromSlider.setRange(1.0, (double)RhythmPrValue::pads, 1.0);
    copyPadFromSlider.setNumDecimalPlacesToDisplay(0);
    copyPadFromSlider.setValue(1, juce::sendNotification);
    copyPadFromSlider.setWantsKeyboardFocus(true);
    copyPadFromSlider.setExplicitFocusOrder(++tabOrder);
    copyPadFromSlider.onValueChange = [this] {
        int from = copyPadFromSlider.getValue() - 1;
        int to = copyPadToSlider.getValue() - 1;

        copyPadParamBtn.setEnabled(from != to);
        };

    copyPadToSlider.setup({ .parent = mainGroup.contentCanvas, .title = "TO", .isReset = false });
    copyPadToSlider.setRange(1.0, (double)RhythmPrValue::pads, 1.0);
    copyPadToSlider.setNumDecimalPlacesToDisplay(0);
    copyPadToSlider.setValue(2, juce::sendNotification);
    copyPadToSlider.setWantsKeyboardFocus(true);
    copyPadToSlider.setExplicitFocusOrder(++tabOrder);
    copyPadToSlider.onValueChange = [this] {
        int from = copyPadFromSlider.getValue() - 1;
        int to = copyPadToSlider.getValue() - 1;

        copyPadParamBtn.setEnabled(from != to);
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    ieToneNoise.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Tone/Noise");
    ieToneNoise.onClickImport = [this] { const int padIndex = currentPad(); importToneNoiseParam(padIndex); };
    ieToneNoise.onClickExport = [this] { const int padIndex = currentPad(); exportToneNoiseParam(padIndex); };

    ieLfo.setupComponentOp(mainGroup.contentCanvas, tabOrder, "LFO");
    ieLfo.onClickImport = [this] { const int padIndex = currentPad(); importLfoParam(padIndex); };
    ieLfo.onClickExport = [this] { const int padIndex = currentPad(); exportLfoParam(padIndex); };

    ieDetune.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Detune");
    ieDetune.onClickImport = [this] { const int padIndex = currentPad(); importDetuneParam(padIndex); };
    ieDetune.onClickExport = [this] { const int padIndex = currentPad(); exportDetuneParam(padIndex); };

    ieAmpEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Amp Env");
    ieAmpEnv.onClickImport = [this] { const int padIndex = currentPad(); importAmpEnvParam(padIndex); };
    ieAmpEnv.onClickExport = [this] { const int padIndex = currentPad(); exportAmpEnvParam(padIndex); };

    iePitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    iePitchEnv.onClickImport = [this] { const int padIndex = currentPad(); importPitchEnvParam(padIndex); };
    iePitchEnv.onClickExport = [this] { const int padIndex = currentPad(); exportPitchEnvParam(padIndex); };

    ieSsgHwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW Env");
    ieSsgHwEnv.onClickImport = [this] { padPanel.importSsgHwEnvParam(); };
    ieSsgHwEnv.onClickExport = [this] { padPanel.exportSsgHwEnvParam(); };
    ieSsgHwPEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv");
    ieSsgHwPEnv.onClickImport = [this] { padPanel.importSsgHwPEnvParam(); };
    ieSsgHwPEnv.onClickExport = [this] { padPanel.exportSsgHwPEnvParam(); };

    ieWtMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Modulation");
    ieWtMod.onClickImport = [this] { padPanel.importWtModParam(); };
    ieWtMod.onClickExport = [this] { padPanel.exportWtModParam(); };
    ieWtAmpMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Amp Mod");
    ieWtAmpMod.onClickImport = [this] { padPanel.importWtAmpModParam(); };
    ieWtAmpMod.onClickExport = [this] { padPanel.exportWtAmpModParam(); };

    ieSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieSsgSwEnv.onClickImport = [this] { const int padIndex = currentPad(); importSsgSwEnvParam(padIndex); };
    ieSsgSwEnv.onClickExport = [this] { const int padIndex = currentPad(); exportSsgSwEnvParam(padIndex); };

    ieSsgSwEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieSsgSwEnv11.onClickImport = [this] { const int padIndex = currentPad(); importSsgSwEnv11Param(padIndex); };
    ieSsgSwEnv11.onClickExport = [this] { const int padIndex = currentPad(); exportSsgSwEnv11Param(padIndex); };

    ieSsgSwPEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieSsgSwPEnv11.onClickImport = [this] { const int padIndex = currentPad(); importSsgSwPEnv11Param(padIndex); };
    ieSsgSwPEnv11.onClickExport = [this] { const int padIndex = currentPad(); exportSsgSwPEnv11Param(padIndex); };

    ieQuality.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { const int padIndex = currentPad(); importQualityParam(padIndex); };
    ieQuality.onClickExport = [this] { const int padIndex = currentPad(); exportQualityParam(padIndex); };

    iePcmPlay.setupComponentOp(mainGroup.contentCanvas, tabOrder, "PCM Play");
    iePcmPlay.onClickImport = [this] { const int padIndex = currentPad(); importPcmPlayParam(padIndex); };
    iePcmPlay.onClickExport = [this] { const int padIndex = currentPad(); exportPcmPlayParam(padIndex); };

    ieChPadParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pad Params");
    ieChPadParam.onClickImport = [this] { const int padIndex = currentPad(); importPadChParam(padIndex); };
    ieChPadParam.onClickExport = [this] { const int padIndex = currentPad(); exportPadChParam(padIndex); };

    uSep003.setupComponent(mainGroup.contentCanvas);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    // ---- 上の絵と、下の設定 ----
    //
    // 絵はパッドの数だけ。設定はひとそろいだけ置き、TARGET で
    // 指し先を切り替える。
    for (int i = 0; i < RhythmPrValue::pads; ++i)
    {
        cells[(size_t)i].setup(*this, i,
            RhythmGuiText::Group::padPrefix + " " + juce::String(i + 1)
            + " (" + RhythmGuiText::padNames[(size_t)i] + ")", true);

        updatePadPreview(i);
        updatePadGraph(i);
        cells[(size_t)i].onSelect = [this](int index) {
            padPanel.targetSlider().setValue(index + 1, juce::sendNotification);
            };
    }


    padPanel.setup(*this, tabOrder);

    padPanel.onTargetChange = [this] { applyPadTarget(); };

    // 設定のつまみが動いたら、指しているパッドの絵を描き直す。
    // 指していないパッドは値が変わらないので、触らなくてよい。
    padPanel.onParamsChanged = [this] { updatePadGraph(currentPad()); };

    // 映すものは絵ごとではなく全部そろえる。並べた絵を見比べる
    // ためのものなので、一枚ずつ違うものを映しても比べようがない。
    padPanel.onGraphModeChange = [this](GuiEnvGraphMode mode) {
        for (int i = 0; i < RhythmPrValue::pads; ++i) updatePadGraph(i);
        };

    // 前に開いていたときの指し先から始める。
    const int saved = (int)ctx.audioProcessor.apvts.state.getProperty(ProcessorStateKey::rhythmTarget, 0);

    padPanel.targetSlider().setValue(juce::jlimit(0, RhythmPrValue::pads - 1, saved) + 1, juce::dontSendNotification);

    applyPadTarget();

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);
}

void GuiRhythm::layout(juce::Rectangle<int> content)
{
    // Top section for Master Volume
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(RhythmGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    unisonComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Unison));
    unisonComponent.layoutComponent(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // ---- 上は絵、下は設定 ----
    //
    // 絵は横 4 枚ずつ並べる。パッドが 4 の倍数でないときは、
    // 足りないところを空けたままにする (詰めると番号と場所がずれる)。
    constexpr int cols = 4;
    constexpr int rows = (RhythmPrValue::pads + cols - 1) / cols;

    const int cellW = pageArea.getWidth() / cols;

    // 枠 1 枚の丈は部品が決める。見出し・波形・グラフを積んだ高さ。
    const int cellH = GuiTargetCell::naturalHeight(true);


    auto cellsArea = pageArea.removeFromTop(cellH * rows);

    for (int r = 0; r < rows; ++r) {
        auto rowArea = cellsArea.removeFromTop(cellH);

        for (int col = 0; col < cols; ++col) {
            const int i = r * cols + col;

            auto cellArea = rowArea.removeFromLeft(cellW);

            if (i >= RhythmPrValue::pads) continue;

            cells[(size_t)i].layout(cellArea);

            updatePadGraph(i);
        }
    }

    // 残りはすべて設定。
    padPanel.setBounds(pageArea);
    padPanel.layout(padPanel.getLocalBounds());
}

void GuiRhythm::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyPadParamBtn.setVisible(visible);
    copyPadFromSlider.setVisibleWithLabel(visible);
    copyPadToSlider.setVisibleWithLabel(visible);
    uSep002.setVisible(visible);
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
    ieChPadParam.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    iePcmPlay.setVisible(visible);
    ieChParam.setVisible(visible);
    uSep003.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyPadParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyPadFromSlider.label, .component = &copyPadFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyPadToSlider.label, .component = &copyPadToSlider });

        uSep002.layoutComponent(rect);

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
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        iePcmPlay.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChPadParam.layoutComponent(rect);

        uSep003.layoutComponent(rect);

        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiRhythm::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    padPanel.removeLoadButtonListener(editor);
}

void GuiRhythm::buttonClicked(juce::Button* button)
{
    // 読み込みの札はひとつしかない。読み込む先は TARGET が指しているパッド。
    if (!padPanel.isThis(button)) return;

    const int i = currentPad();

    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 番号は値で写して渡す。
    ctx.editor.openAudioBrowser([this, i](const juce::File& file)
        {
            if (!file.existsAsFile()) return;

            if (i == currentPad()) padPanel.updatePadFileName("Loading...");

            // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
            juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

            juce::Timer::callAfterDelay(50, [this, safe, i, file]()
                {
                    // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                    // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                    if (safe == nullptr) return;

                    // Load to specific pad index
                    ctx.audioProcessor.loadRhythmFile(file, i);

                    // Update label
                    updatePadFileName(i, file.getFileName());

                    ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                });
        });
}

void GuiRhythm::updatePadFileName(int padIndex, const juce::String& fileName)
{
    // 名前を出せるのは、今指しているパッドのぶんだけ。
    // ほかのパッドは上の絵だけが変わる。
    if (padIndex == currentPad()) padPanel.updatePadFileName(fileName);

    updatePadPreview(padIndex);
}

bool GuiRhythm::isThis(int index, juce::Button* button)
{
    // 読み込みの札はひとつしかない。指しているパッドのものとして扱う。
    return index == currentPad() && padPanel.isThis(button);
}

void GuiRhythm::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiRhythm::initParams()
{
    this->ctx.audioProcessor.initParams("RHYTHM_");
    for (int i = 0; i < RhythmPrValue::pads; i++)
    {
        this->ctx.audioProcessor.unloadRhythmFile(i);
        updatePadFileName(i, Io::empty);
    }
}

// TARGET が今どのパッドを指しているか (0 から数える)。
int GuiRhythm::currentPad() const
{
    return juce::jlimit(0, RhythmPrValue::pads - 1, (int)padPanel.getTargetValue() - 1);
}

// 指し先を切り替える。
//
// 設定の束縛を移し、枠線を付け替える。開き直したときに続きから
// 触れるよう、指し先は覚えておく。
void GuiRhythm::applyPadTarget()
{
    const int pad = currentPad();

    ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::rhythmTarget, pad, nullptr);

    padPanel.rebind(pad);

    for (int i = 0; i < RhythmPrValue::pads; ++i) cells[(size_t)i].setActive(i == pad);
}

// 指し先を一時的に動かして何かをする。
//
// 設定はひとそろいしか無いので、TARGET が指していないパッドを
// 読み書きするには、いったんそこへ繋ぎ替えるしかない。
// 終わったら必ず元へ戻す。
void GuiRhythm::withPad(int index, const std::function<void(RhythmPadGui&)>& fn)
{
    const int keep = currentPad();

    if (index != keep) padPanel.rebind(index);

    fn(padPanel);

    if (index != keep) padPanel.rebind(keep);
}


void GuiRhythm::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiRhythm::copyPadParams(int p, CopyRhythmPad& copyObj) {
    withPad(p, [&copyObj](RhythmPadGui& pad) { pad.copyParams(copyObj); });
}

void GuiRhythm::pastePadParams(int p, CopyRhythmPad& copyObj) {
    withPad(p, [&copyObj](RhythmPadGui& pad) { pad.pasteParams(copyObj); });
}

void GuiRhythm::importToneNoiseParam(int p) {
    padPanel.importToneNoiseParam();
}

void GuiRhythm::exportToneNoiseParam(int p) {
    padPanel.exportToneNoiseParam();
}

void GuiRhythm::importLfoParam(int p) {
    padPanel.importLfoParam();
}

void GuiRhythm::exportLfoParam(int p) {
    padPanel.exportLfoParam();
}

void GuiRhythm::importAmpEnvParam(int p) {
    padPanel.importAmpEnvParam();
}

void GuiRhythm::exportAmpEnvParam(int p) {
    padPanel.exportAmpEnvParam();
}

void GuiRhythm::importPitchEnvParam(int p) {
    padPanel.importPitchEnvParam();
}

void GuiRhythm::exportPitchEnvParam(int p) {
    padPanel.exportPitchEnvParam();
}

void GuiRhythm::importSsgSwEnvParam(int p) {
    padPanel.importSsgSwEnvParam();
}

void GuiRhythm::exportSsgSwEnvParam(int p) {
    padPanel.exportSsgSwEnvParam();
}

void GuiRhythm::importDetuneParam(int p) {
    padPanel.importDetuneParam();
}

void GuiRhythm::exportDetuneParam(int p) {
    padPanel.exportDetuneParam();
}

void GuiRhythm::importQualityParam(int p) {
    padPanel.importQualityParam();
}

void GuiRhythm::exportQualityParam(int p) {
    padPanel.exportQualityParam();
}

void GuiRhythm::importPcmPlayParam(int p) {
    padPanel.importPcmPlayParam();
}

void GuiRhythm::exportPcmPlayParam(int p) {
    padPanel.exportPcmPlayParam();
}

void GuiRhythm::importSsgSwEnv11Param(int p) {
    padPanel.importSsgSwEnv11Param();
}

void GuiRhythm::exportSsgSwEnv11Param(int p) {
    padPanel.exportSsgSwEnv11Param();
}

void GuiRhythm::importSsgSwPEnv11Param(int p) {
    padPanel.importSsgSwPEnv11Param();
}

void GuiRhythm::exportSsgSwPEnv11Param(int p) {
    padPanel.exportSsgSwPEnv11Param();
}

void GuiRhythm::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "RHYTHM" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiRhythm::applyChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

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

        Io::ParamWriter writer(rhythmFormat);

        writeChParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, rhythmFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    // Level
    levelComponent.readParams(*reader, "level");

    // Components
    unisonComponent.readParams(*reader, "unison");

    // ファイルに無いパッドは触らない。パッド数の違う版で書いた
    // ファイルを読んでも、こちらの音が消えないようにするため。
    const int stored = reader->arraySize(Io::ParamKey::pads);

    for (int i = 0; i < RhythmPrValue::pads && i < stored; i++) {
        withPad(i, [&reader, i](RhythmPadGui& pad) { pad.readParams(i, reader->arrayItem(Io::ParamKey::pads, i)); });
    }
}

void GuiRhythm::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "RHYTHM" }, Io::Extension::rhythmParam,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiRhythm::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(rhythmFormat);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiRhythm::importPadChParam(int p)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindRhythmPad },
        [this, p](const juce::File& file) { applyPadChParamFile(p, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiRhythm::applyPadChParamFile(int p, const juce::File& file)
{
    if (!file.existsAsFile()) return;


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

            setImportingPadChParams(p, lines, index);
        }

        Io::ParamWriter writer(rhythmPadFormat);

        writePadChParams(p, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, rhythmPadFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    withPad(p, [&reader, p](RhythmPadGui& pad) { pad.readParams(p, *reader); });
}

void GuiRhythm::exportPadChParam(int p)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindRhythmPad }, Io::Extension::rhythmPadParam,
        [this, p](const juce::File& file) { writePadChParamFile(p, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiRhythm::writePadChParamFile(int p, const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(rhythmPadFormat);
    writePadChParams(p, writer);

    writer.writeTo(file);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiRhythm::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Components
	unisonComponent.setImportingParams(lines, index);

	for (int i = 0; i < RhythmPrValue::pads; i++) {
	    getImportingPadParams(i, lines, index);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiRhythm::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Components
	unisonComponent.writeParams(writer, "unison");

	for (int i = 0; i < RhythmPrValue::pads; i++) {
	    auto pad = writer.arrayItem(Io::ParamKey::pads, i);

        withPad(i, [&pad, i](RhythmPadGui& panel) { panel.writeParams(i, pad); });
	}

	
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void GuiRhythm::getImportingPadParams(int p, juce::StringArray& lines, int& index) {
    withPad(p, [&lines, &index, p](RhythmPadGui& pad) { pad.setImportingParams(p, lines, index); });
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void RhythmPadGui::setImportingParams(int p, juce::StringArray& lines, int& index) {
    // linesの終端を超えていたら（Originなどの6パッドなど、パッド数が8未満の場合）、初期値でリセットして処理を抜ける
    if (index >= lines.size()) {
        ctx.audioProcessor.unloadRhythmFile(p);
        fileNameLabel.setText(Io::empty, juce::dontSendNotification);

        // 基本パラメータ初期値
        volSlider.setValue(0.0, juce::sendNotification);
        panSlider.setValue(0.5, juce::sendNotification);
        oneShotButton.setToggleState(false, juce::sendNotification);
        noteSlider.setValue(60, juce::sendNotification);

        // PCM Play
        pcmOffsetSlider.setValue(0.0, juce::sendNotification);
        pcmRatioSlider.setValue(1.0, juce::sendNotification);
        loopPointEnableButton.setToggleState(false, juce::sendNotification);
        loopPointStartSlider.setValue(0.0, juce::sendNotification);
        loopPointEndSlider.setValue(1.0, juce::sendNotification);

        // Tone/Noise
        toneSlider.setValue(1.0, juce::sendNotification);
        noiseSlider.setValue(0.0, juce::sendNotification);
        noiseFreqSlider.setValue(0.0, juce::sendNotification);
        mixSlider.setValue(0.0, juce::sendNotification);

        juce::StringArray emptyLines;
        int dummyIndex = 0;
        fixComponent.setImportingParams(emptyLines, dummyIndex);
        ampEnvComponent.setImportingParams(emptyLines, dummyIndex);
        pitchEnvComponent.setImportingParams(emptyLines, dummyIndex);
        ssgHwEnv.setImportingParams(emptyLines, dummyIndex);
        ssgSwEnvComponent.setImportingParams(emptyLines, dummyIndex);
        ssgSwEnv11Component.setImportingParams(emptyLines, dummyIndex);
        ssgSwPEnv11Component.setImportingParams(emptyLines, dummyIndex);
        mulDetuneComponent.setImportingParams(emptyLines, dummyIndex);
        lfoComponent.setImportingParams(emptyLines, dummyIndex);
        qualityPcmComponent.setImportingParams(emptyLines, dummyIndex);

        return;
    }

    // Form
    if (fileNameLabel.getText() != lines[index]) {
        ctx.audioProcessor.unloadRhythmFile(p);
    }

    fileNameLabel.setText(lines[index++], juce::dontSendNotification);

    if (fileNameLabel.getText().isNotEmpty()) {
        ctx.audioProcessor.loadRhythmFile(fileNameLabel.getText(), p);
    }

    volSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    oneShotButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    noteSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

    // Tone/Noise
    toneSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    noiseSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    noiseFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    mixSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

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
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void RhythmPadGui::setImportingToneNoiseParams(juce::StringArray& lines, int& index) {
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
void RhythmPadGui::writeToneNoiseParams(Io::ParamWriter& writer) {
	writer.set("tone", (float)toneSlider.getValue());
	writer.set("noise", (float)noiseSlider.getValue());
	writer.set("noiseFreq", (float)noiseFreqSlider.getValue());
	writer.set("mix", (float)mixSlider.getValue());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void RhythmPadGui::setImportingPcmPlayParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 3) return;

	qualityPcmComponent.setMode(lines[0].getIntValue());
	qualityPcmComponent.setRate(lines[1].getIntValue());
	qualityPcmComponent.setInterp(lines[2].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void RhythmPadGui::writePcmPlayParams(Io::ParamWriter& writer) {
	writer.set("pcmOffset", (float)pcmOffsetSlider.getValue());
	writer.set("delay", (float)padDelaySlider.getValue());
	writer.set("speed", (float)speedSlider.getValue());
	writer.set("pcmRatio", (float)pcmRatioSlider.getValue());
	writer.set("loopPointEnable", loopPointEnableButton.getToggleState());
	writer.set("loopPointStart", (float)loopPointStartSlider.getValue());
	writer.set("loopPointEnd", (float)loopPointEndSlider.getValue());
	writer.set("loopCount", (float)loopCountSlider.getValue());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void RhythmPadGui::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 3) return;

	qualityPcmComponent.setMode(lines[0].getIntValue());
	qualityPcmComponent.setRate(lines[1].getIntValue());
	qualityPcmComponent.setInterp(lines[2].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void RhythmPadGui::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("mode", qualityPcmComponent.getMode());
	writer.set("rate", qualityPcmComponent.getRate());
	writer.set("interp", qualityPcmComponent.getInterp());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiRhythm::setImportingPadChParams(int p, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	getImportingPadParams(p, lines, index);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiRhythm::writePadChParams(int p, Io::ParamWriter& writer) {
    withPad(p, [&writer, p](RhythmPadGui& pad) { pad.writeParams(p, writer); });

	
}
