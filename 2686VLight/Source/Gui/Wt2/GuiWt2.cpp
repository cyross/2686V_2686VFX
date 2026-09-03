#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiWt2.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// 自分で描いた波形を表す選択肢の番号
	constexpr int customWaveIndex = 8;

	// ファイルの中身を見分ける印
	const Io::ParamFormat wt2Format{ "wt2", 1 };
	const Io::ParamFormat qualityFormat{ "quality", 1 };
}

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Wt2/ProcessorWt2Keys.h"
#include "../../Processor/Wt2/ProcessorWt2Values.h"
#include "../../Core/Const/ConstFileValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiWt2Values.h"
#include "./GuiWt2Text.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiWt2Helpers.h"

static std::vector<SelectItem> wtWsItems = {
    {.name = "0: Sine",          .value = 1 },
    {.name = "1: Triangle",      .value = 2 },
    {.name = "2: Saw Up",        .value = 3 },
    {.name = "3: Saw Down",      .value = 4 },
    {.name = "4: Square (50%)",  .value = 5 },
    {.name = "5: Pulse (25%)",   .value = 6 },
    {.name = "6: Pulse (12.5%)", .value = 7 },
    {.name = "7: Digital Noise", .value = 8 },
    {.name = "8: Custom(Draw)",  .value = 9 },
};

static std::vector<SelectItem> wtTsItems = {
    {.name = "0:  32 Samples",  .value = 1 },
    {.name = "1:  64 Samples",  .value = 2 },
    {.name = "2: 128 Samples",  .value = 3 },
    {.name = "3: 256 Samples",  .value = 4 },
};

static std::vector<SelectItem> wtRsItems = {
    {.name = "1:  16", .value = 1 },
    {.name = "2:  32",  .value = 2 },
    {.name = "3:  64",  .value = 3 },
    {.name = "4: 128",  .value = 4 },
    {.name = "5: 256",  .value = 5 },
};

// MODULATION の変調波形。実機ディスクシステムは 32 段の階段状。
static std::vector<SelectItem> lfoPmShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

static std::vector<SelectItem> lfoAmShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

template <size_t tableSize>
Waveform2Container<tableSize>::Waveform2Container(const GuiContext& context) : ParamBarEditorBase(context)
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
}

template <size_t tableSize>
void Waveform2Container<tableSize>::setup(const Config& c)
{
    c.parent.addAndMakeVisible(this);

    attachParams(c.idPrefix, (int)tableSize);
}

template <size_t tableSize>
void Waveform2Container<tableSize>::setAllValues(int val)
{
    if (!isEnabledState || m_params.empty()) return;

    for (size_t i = 0; i < tableSize; ++i) {
        if (m_params[i]) {
            float normVal = m_params[i]->getNormalisableRange().convertTo0to1((float)val);
            m_params[i]->setValueNotifyingHost(normVal);
        }
    }

    repaint();
}

// =======================================================
// 外部から配列で値を一括セットする
// =======================================================
template <size_t tableSize>
void Waveform2Container<tableSize>::setValues(const std::vector<int>& values)
{
    if (m_params.empty()) return;

    for (size_t i = 0; i < tableSize && i < values.size(); ++i) {
        if (m_params[i]) {
            float normVal = m_params[i]->getNormalisableRange().convertTo0to1((float)values[i]);
            m_params[i]->setValueNotifyingHost(normVal);
        }
    }

    repaint();
}

// =======================================================
// 現在の値を配列として取得する
// =======================================================
template <size_t tableSize>
std::vector<int> Waveform2Container<tableSize>::getValues()
{
    std::vector<int> vals(tableSize, 0);
    if (m_params.empty()) return vals;

    for (size_t i = 0; i < tableSize; ++i) {
        if (m_params[i]) {
            // 0.0〜1.0 の値を、実際の整数値(0〜255など)に変換して取得
            vals[i] = (int)std::round(m_params[i]->getNormalisableRange().convertFrom0to1(m_params[i]->getValue()));
        }
    }

    return vals;
}

// =======================================================
// カスタム描画 (子コンポーネントを使わず一気に描く)
// =======================================================
template <size_t tableSize>
void Waveform2Container<tableSize>::paint(juce::Graphics& g)
{
    if (!isEnabledState || m_params.empty()) {
        g.fillAll(juce::Colours::darkgrey.withAlpha(0.7f));
        return;
    }

    // セルを完全な正方形にするための計算
    // コンポーネントの横幅を tableSize (32, 64等) で割り、1セルのサイズを決定
    float cellSize = (float)getWidth() / (float)tableSize;
    float gridHeight = cellSize * resolution;

    // グリッド全体の背景を塗りつぶす
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.fillRect(0.0f, 0.0f, (float)getWidth(), gridHeight);

    // 薄いグリッド線を描画
    g.setColour(juce::Colours::grey.withAlpha(0.2f));

    for (int y = 0; y <= resolution; ++y) g.drawLine(0.0f, y * cellSize, (float)getWidth(), y * cellSize, 1.0f);
    for (size_t x = 0; x <= tableSize; ++x) g.drawLine(x * cellSize, 0.0f, x * cellSize, gridHeight, 1.0f);

    // 中心の計算バグ修正
    // 例: resolution=16 (0〜15) のとき、中心は 8 (Yインデックスは 15 - 8 = 7) にしたい
    // 例: resolution=32 (0〜31) のとき、中心は 16 (Yインデックスは 31 - 16 = 15) にしたい
    int targetCenterY = (resolution - 1) - resCenter;

    for (size_t x = 0; x < tableSize; ++x)
    {
        int valActual = 0;

        if (m_params[x]) {
            valActual = (int)std::round(m_params[x]->getNormalisableRange().convertFrom0to1(m_params[x]->getValue()));
        }

        valActual = std::clamp(valActual, 0, resolution - 1);

        int setY = (resolution - 1) - valActual; // 上が最大値なので反転

        for (int y = 0; y < resolution; ++y)
        {
            bool isSet = (y == setY);
            bool isHovered = (x == hoveredIndex && y == hoveredY);
            bool isCellCenter = (y == targetCenterY);

            juce::Colour cellColor = juce::Colours::transparentBlack;

            // 指定された色ルールを適用
            if (isSet) {
                if (isCellCenter) cellColor = juce::Colours::green; // 中心を設定
                else cellColor = juce::Colours::blue;               // 通常を設定
            }
            else if (isHovered) {
                if (isCellCenter) cellColor = juce::Colours::lightgreen; // 中心をホバー
                else cellColor = juce::Colours::cyan;                    // 通常をホバー（明るい青）
            }

            if (!cellColor.isTransparent()) {
                g.setColour(cellColor);
                // セルの内側に少し余白を持たせて四角を描画
                g.fillRect(x * cellSize + 1.0f, y * cellSize + 1.0f, cellSize - 2.0f, cellSize - 2.0f);
            }

            // 値がセットされておらずホバーもしていない「中心線」を薄く光らせる
            if (isCellCenter && !isSet && !isHovered) {
                g.setColour(juce::Colours::green.withAlpha(0.2f));
                g.fillRect(x * cellSize + 1.0f, y * cellSize + 1.0f, cellSize - 2.0f, cellSize - 2.0f);
            }
        }
    }
}

template <size_t tableSize>
void Waveform2Container<tableSize>::updateSliderValue(const juce::MouseEvent& e)
{
    if (!isEnabledState || m_params.empty()) return;

    float cellSize = (float)getWidth() / (float)tableSize;
    float gridHeight = cellSize * resolution;

    float mouseY = std::clamp((float)e.position.y, 0.0f, gridHeight - 0.1f);

    int index = (int)(e.position.x / cellSize);
    index = std::clamp(index, 0, (int)tableSize - 1);

    int y_idx = (int)(mouseY / cellSize);
    y_idx = std::clamp(y_idx, 0, resolution - 1);

    // マウス位置から「実際の整数値」を決定
    int valActual = (resolution - 1) - y_idx;

    if (m_params[index] != nullptr) {
        // 実際の整数値を 0.0〜1.0 に正規化してセット
        float normVal = m_params[index]->getNormalisableRange().convertTo0to1((float)valActual);
        m_params[index]->setValueNotifyingHost(normVal);
    }

    repaint();
}

template <size_t tableSize>
void Waveform2Container<tableSize>::updateHoverState(const juce::MouseEvent& e)
{
    if (!isEnabledState) return;

    float cellSize = (float)getWidth() / (float)tableSize;
    float gridHeight = cellSize * resolution;

    // グリッド外にマウスがある場合はホバーを解除
    if (e.position.y > gridHeight || e.position.y < 0.0f) {
        if (hoveredIndex != -1) {
            hoveredIndex = -1;
            hoveredY = -1;
            repaint();
        }
        return;
    }

    int index = (int)(e.position.x / cellSize);
    index = std::clamp(index, 0, (int)tableSize - 1);

    int y_idx = (int)(e.position.y / cellSize);
    y_idx = std::clamp(y_idx, 0, resolution - 1);

    if (hoveredIndex != index || hoveredY != y_idx) {
        hoveredIndex = index;
        hoveredY = y_idx;
        repaint();
    }

    lastMousePos = e.position.toInt();
}

template <size_t tableSize>
void Waveform2Container<tableSize>::paintOverChildren(juce::Graphics& g)
{
    if (hoveredIndex >= 0 && hoveredY >= 0 && isEnabledState && !m_params.empty())
    {
        int valActual = (resolution - 1) - hoveredY;

        juce::String text = "[" + juce::String(hoveredIndex) + "] " + juce::String(valActual);

        paintHoverText(g, text);
    }
}

template class Waveform2Container<32>;
template class Waveform2Container<64>;
template class Waveform2Container<128>;
template class Waveform2Container<256>;

void GuiWt2::setup()
{
    auto applyCenter = [this] {
        int resoIndex = resoSelector.getSelectedId() - 1;

        resolution = 16 << resoIndex;
        resCenter = 8 << resoIndex;

        customSliders32.resolution = resolution;
        customSliders32.resCenter = resCenter;
        customSliders64.resolution = resolution;
        customSliders64.resCenter = resCenter;
        customSliders128.resolution = resolution;
        customSliders128.resCenter = resCenter;
        customSliders256.resolution = resolution;
        customSliders256.resCenter = resCenter;
        };
    const juce::String code = Wt2PrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, Wt2GuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    // Waveform
    waveSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Wt2::wave, .title = Wt2GuiText::Wt::form, .items = wtWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);
    waveSelector.onChange = [this] {
        ctx.editor.resized();
        };

    formSeparator.setupComponent(mainGroup.contentCanvas);

    // Custom Wave Size
    sizeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Wt2::sampleSize, .title = Wt2GuiText::Wt::size, .items = wtTsItems, .isReset = true, .isResized = true });
    sizeSelector.setWantsKeyboardFocus(true);
    sizeSelector.setExplicitFocusOrder(++tabOrder);

    // Resolution
    resoSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Wt2::resolution, .title = Wt2GuiText::Wt::resolution, .items = wtRsItems, .isReset = true, .isResized = true });
    resoSelector.setWantsKeyboardFocus(true);
    resoSelector.setExplicitFocusOrder(++tabOrder);
    resoSelector.onChange = [this, applyCenter] {
        applyCenter();

        ctx.editor.resized();
        };

    // 波形メモリのチャンネル自身の機能なのでハード扱いにする
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, GuiColor::Category::HwBg);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, Wt2GuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, Wt2GuiText::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, Wt2GuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, Wt2GuiText::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfo.setupComponent(
        mainGroup.contentCanvas,
        code,
        tabOrder
    );

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwPEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    customWaveImportBtn.setup({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Wt::fileImport, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveImportBtn.setWantsKeyboardFocus(true);
    customWaveImportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveImportBtn.onClick = [this] { importWavetable(); };

    customWaveExportBtn.setup({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Wt::fileExport, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkgrey.darker(0.8f), .isReset = false, .isResized = false});
    customWaveExportBtn.setWantsKeyboardFocus(true);
    customWaveExportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveExportBtn.onClick = [this] { exportWavetable(); };

    uSep002.setupComponent(mainGroup.contentCanvas);

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", lfo);

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

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    // Custom Wave Group
    customWaveGroup.setup(*this, Wt2GuiText::Group::wtCustom);

    // Custom Wave Sliders
    customSliders32.setup({ .parent = customWaveGroup.contentCanvas, .idPrefix = code + CPK::custom32 });
    customSliders64.setup({ .parent = customWaveGroup.contentCanvas, .idPrefix = code + CPK::custom64 });
    customSliders128.setup({ .parent = customWaveGroup.contentCanvas, .idPrefix = code + CPK::custom128 });
    customSliders256.setup({ .parent = customWaveGroup.contentCanvas, .idPrefix = code + CPK::custom256 });

    customWaveResetToCenterBtn.setup({ .parent = customWaveGroup.contentCanvas, .title = Wt2GuiText::Wt::Custom::toCenter, .font = labelFont, .bgColor = GuiColor::Waveform2Container::ResetBtn::ToCenter, .isReset = false, .isResized = false });
    customWaveResetToCenterBtn.setWantsKeyboardFocus(true);
    customWaveResetToCenterBtn.setExplicitFocusOrder(++tabOrder);
    customWaveResetToCenterBtn.onClick = [this] {
        customSliders32.setAllValues(resCenter);
        customSliders64.setAllValues(resCenter);
        customSliders128.setAllValues(resCenter);
        customSliders256.setAllValues(resCenter);
        ctx.editor.resized(); // 変更通知
        };

    customWaveResetToMaxBtn.setup({ .parent = customWaveGroup.contentCanvas, .title = Wt2GuiText::Wt::Custom::toMax, .font = labelFont, .bgColor = GuiColor::Waveform2Container::ResetBtn::ToMax, .isReset = false, .isResized = false });
    customWaveResetToMaxBtn.setWantsKeyboardFocus(true);
    customWaveResetToMaxBtn.setExplicitFocusOrder(++tabOrder);
    customWaveResetToMaxBtn.onClick = [this] {
        customSliders32.setAllValues(resolution - 1);
        customSliders64.setAllValues(resolution - 1);
        customSliders128.setAllValues(resolution - 1);
        customSliders256.setAllValues(resolution - 1);
        ctx.editor.resized();
        };

    customWaveResetTo0Btn.setup({ .parent = customWaveGroup.contentCanvas, .title = Wt2GuiText::Wt::Custom::to0, .font = labelFont, .bgColor = GuiColor::Waveform2Container::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.setWantsKeyboardFocus(true);
    customWaveResetTo0Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0);
        customSliders64.setAllValues(0);
        customSliders128.setAllValues(0);
        customSliders256.setAllValues(0);
        ctx.editor.resized();
        };

    applyCenter();

    setupGraph();
    updateGraph();
}

void GuiWt2::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(Wt2GuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(Wt2GuiValue::Group::Padding::width, Wt2GuiValue::Group::Padding::height);
    mmRect.removeFromTop(Wt2GuiValue::Group::TitlePaddingTop);

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

    ampEnvComponent.layoutComponent(mRect);
    ampModComponent.layoutComponent(mRect);
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnvComponent.layoutComponent(mRect);
    ssgSwEnv11Component.layoutComponent(mRect);

    modComponent.layoutComponent(mRect);
    pitchEnvComponent.layoutComponent(mRect);
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11Component.layoutComponent(mRect);

    lfo.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);

    fixComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // 波形がカスタム以外の時は波形精度選択を Disabled に
    bool isCustomWave = (waveSelector.getSelectedId() == 9);

    sizeSelector.setEnabledWithLabel(isCustomWave);
    customSliders32.setEnabled(isCustomWave);
    customSliders64.setEnabled(isCustomWave);
    customSliders128.setEnabled(isCustomWave);
    customSliders256.setEnabled(isCustomWave);
    customWaveResetToCenterBtn.setEnabled(isCustomWave);
    customWaveResetToMaxBtn.setEnabled(isCustomWave);
    customWaveResetTo0Btn.setEnabled(isCustomWave);

    auto rightArea = pageArea.removeFromLeft(Wt2GuiValue::RightWidth);
    customWaveGroup.setBounds(rightArea);

    auto cwRect = rightArea.reduced(Wt2GuiValue::Group::Padding::width, Wt2GuiValue::Group::Padding::height);
    cwRect.removeFromTop(Wt2GuiValue::Group::TitlePaddingTop);

    // 1. Viewportの領域は、グループの枠内の全領域にセットする
    customWaveGroup.setViewportCustomBounds(cwRect.translated(-rightArea.getX(), -rightArea.getY()));

    // 2. キャンバス内部のレイアウト領域 (高さはViewportの実表示高さに固定し、スクロールを抑える)
    int availableHeight = cwRect.getHeight();
    juce::Rectangle<int> innerCwRect(0, 0, cwRect.getWidth(), availableHeight);

    // 先にキャンバス内部からボタン群の領域を下から切り取る
    innerCwRect.removeFromBottom(Wt2GuiValue::Custom::ResetBtn::Padding::Bottom);
    auto resetRect = innerCwRect.removeFromBottom(Wt2GuiValue::Custom::ResetBtn::height);
    innerCwRect.removeFromBottom(Wt2GuiValue::Custom::ResetBtn::Padding::Top);

    // 選択中のWaveSizeを取得
    int waveSize = 32;
    switch (sizeSelector.getSelectedId()) {
    case 1: waveSize = 32; break;
    case 2: waveSize = 64; break;
    case 3: waveSize = 128; break;
    case 4: waveSize = 256; break;
    }

    // セルを「正方形」にしつつ、残りの領域(innerCwRect)にスクロール無しで収める計算
    float maxCellW = (float)innerCwRect.getWidth() / waveSize;
    float maxCellH = (float)innerCwRect.getHeight() / resolution;
    float cellSize = std::min(maxCellW, maxCellH); // 幅と高さの厳しい方に合わせる

    int totalW = (int)(cellSize * waveSize);
    int totalH = (int)(cellSize * resolution);

    // 波形エリアを中央揃えで配置
    auto containerArea = innerCwRect.withSizeKeepingCentre(totalW, totalH);

    customSliders32.setBounds(containerArea);
    customSliders64.setBounds(containerArea);
    customSliders128.setBounds(containerArea);
    customSliders256.setBounds(containerArea);

    customWaveGroup.setEnabled(isCustomWave);

    // 適切なコンテナのみを表示
    customSliders32.setVisible(false); customSliders32.setCustomEnabled(false);
    customSliders64.setVisible(false); customSliders64.setCustomEnabled(false);
    customSliders128.setVisible(false); customSliders128.setCustomEnabled(false);
    customSliders256.setVisible(false); customSliders256.setCustomEnabled(false);

    if (isCustomWave) {
        if (waveSize == 32) { customSliders32.setVisible(true); customSliders32.setCustomEnabled(true); }
        else if (waveSize == 64) { customSliders64.setVisible(true); customSliders64.setCustomEnabled(true); }
        else if (waveSize == 128) { customSliders128.setVisible(true); customSliders128.setCustomEnabled(true); }
        else { customSliders256.setVisible(true); customSliders256.setCustomEnabled(true); }
    }
    else {
        if (waveSize == 32) customSliders32.setVisible(true);
        else if (waveSize == 64) customSliders64.setVisible(true);
        else if (waveSize == 128) customSliders128.setVisible(true);
        else customSliders256.setVisible(true);
    }

    // 3. 確保しておいた領域にボタン群をレイアウト
    layoutRowWt2WaveValueUpdate({
        .rect = resetRect,
        .resetTo0Btn = &customWaveResetToCenterBtn,
        .resetTo1Btn = &customWaveResetToMaxBtn,
        .resetToM1Btn = &customWaveResetTo0Btn,
        .rowHeight = Wt2GuiValue::Custom::ResetBtn::height,
        .paddingRight = Wt2GuiValue::Custom::ResetBtn::Padding::Right
        });

    // スクロールコンテナの実質的な高さを、実際の表示領域に一致させる（スクロールバーを完全に隠す）
    customWaveGroup.setContentHeight(availableHeight);
}

void GuiWt2::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiWt2::importWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importWavetableFile, defaultDir, Io::ExtensionGlob::wt2);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                if (lines.size() == 0) return;

                // 1行目のサンプル数を取得
                int sampleCount = lines[0].trim().getIntValue();

                // サンプル数の検証
                if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                        "Invalid WT File", "Sample count must be 32, 64, 128, or 256.");
                    return;
                }

                int resNumber = lines[1].trim().getIntValue();

                // 解像度の検証
                if (resNumber != 16 && resNumber != 32 && resNumber != 64 && resNumber != 128 && resNumber != 256) {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                        "Invalid WT File", "Resolution must be 16, 32, 64, 128, or 256.");
                    return;
                }

                resolution = resNumber;
                resCenter = resolution >> 1;

                // デフォルト0.0で配列を初期化（足りない部分は0.0で埋まる）
                std::vector<int> values(sampleCount, resCenter);

                // 3行目以降の値を読み込み、0 ~ Resolution-1 にクランプして格納
                // 書き出しは「サンプル数 / 解像度 / 値...」の順なので、値は lines[2] から。
                for (int i = 0; i < sampleCount; ++i) {
                    if (i + 2 < lines.size()) {
                        int val = lines[i + 2].getIntValue();

                        values[i] = std::clamp(val, 0, resolution - 1);
                    }
                }

                // --- UIの更新 ---
                // 波形を「8: Custom(Draw)」に変更 (ID: 9)
                waveSelector.setSelectedId(9, juce::sendNotification);

                // サンプルサイズを選択
                int sizeId = 1;
                if (sampleCount == 32) sizeId = 1;
                else if (sampleCount == 64) sizeId = 2;
                else if (sampleCount == 128) sizeId = 3;
                else if (sampleCount == 256) sizeId = 4;
                sizeSelector.setSelectedId(sizeId, juce::sendNotification);

                // 解像度を選択
                int resId = 1;
                if (resolution == 16) resId = 1;
                else if (resolution == 32) resId = 2;
                else if (resolution == 64) resId = 3;
                else if (resolution == 128) resId = 4;
                else if (resolution == 256) resId = 5;
                resoSelector.setSelectedId(resId, juce::sendNotification);

                // --- 値をAPVTS(スライダー)に反映 ---
                if (sampleCount == 32) customSliders32.setValues(values);
                else if (sampleCount == 64) customSliders64.setValues(values);
                else if (sampleCount == 128) customSliders128.setValues(values);
                else if (sampleCount == 256) customSliders256.setValues(values);
            }
        });
}

void GuiWt2::exportWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportWavetableFile, defaultDir.getChildFile("custom_wave.wt2"), Io::ExtensionGlob::wt2);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                // 現在のサイズIDを取得
                int sizeId = sizeSelector.getSelectedId();
                std::vector<int> values;

                if (sizeId == 1) values = customSliders32.getValues();
                else if (sizeId == 2) values = customSliders64.getValues();
                else if (sizeId == 3) values = customSliders128.getValues();
                else if (sizeId == 4) values = customSliders256.getValues();

                if (values.empty()) return;

                // 1行目にサンプル数
                juce::String content = juce::String(values.size()) + "\n" + juce::String(resolution) + "\n";

                // 2行目以降に値を書き込む
                for (float v : values) {
                    content += juce::String(v, 6) + "\n"; // 小数点以下6桁まで保存
                }

                file.replaceWithText(content);
            }
        });
}

void GuiWt2::initParams()
{
    this->ctx.audioProcessor.initParams("WT2_");
}

void GuiWt2::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();
    int index = waveSelector.getSelectedId();
    bool visibleCustom = index == 9; // custom

    waveSelector.setVisibleWithLabel(visible);
    formSeparator.setVisible(visible && visibleCustom);
    sizeSelector.setVisibleWithLabel(visible && visibleCustom);
    resoSelector.setVisibleWithLabel(visible && visibleCustom);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &waveSelector.label, .component = &waveSelector });

        if (visibleCustom)
        {
            formSeparator.layoutComponent(rect);
            layoutMain({ .mainRect = rect, .label = &sizeSelector.label, .component = &sizeSelector, });
            layoutMain({ .mainRect = rect, .label = &resoSelector.label, .component = &resoSelector, });
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiWt2::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiWt2::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    customWaveImportBtn.setVisible(visible);
    customWaveExportBtn.setVisible(visible);
    uSep002.setVisible(visible);
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
        layoutMainTwoComps({ .rect = rect, .comp1 = &customWaveImportBtn, .comp2 = &customWaveExportBtn });
        uSep002.layoutComponent(rect);
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

void GuiWt2::setupGraph()
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

void GuiWt2::setGraphMode(GraphMode mode)
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

void GuiWt2::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(Wt2GuiValue::MainGroup::Graph::height + Wt2GuiValue::MainGroup::Separator::height);

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(Wt2GuiValue::MainGroup::Graph::ButtonHeight);
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
void GuiWt2::updateGraph()
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

void GuiWt2::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiWt2::importQualityParam() {
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

void GuiWt2::exportQualityParam() {
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

void GuiWt2::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::wt2Param);
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

                    Io::ParamWriter writer(wt2Format);

                    writeChParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, wt2Format);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                // Level
                levelComponent.readParams(*reader, "level");

                // Form
                sizeSelector.setSelectedItemIndex(reader->getInt("size", sizeSelector.getSelectedItemIndex()), juce::sendNotification);
                resoSelector.setSelectedItemIndex(reader->getInt("reso", resoSelector.getSelectedItemIndex()), juce::sendNotification);
                waveSelector.setSelectedItemIndex(reader->getInt("wave", waveSelector.getSelectedItemIndex()), juce::sendNotification);

                // Modulation
                modComponent.readParams(*reader, "wtMod");
                ampModComponent.readParams(*reader, "wtAmpMod");

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
                lfo.readParams(*reader, "lfo");
                qualityComponent.readParams(*reader, "quality");
                unisonComponent.readParams(*reader, "unison");

                // 自分で描いた波形。並びの長さがそのまま段数になるので、
                // どの入れ物へ入れるかを別の項目から導く必要は無い。
                auto customValues = reader->getIntArray("customWave");

                // 今の解像度からはみ出さないように収める
                const int currentReso = 16 << resoSelector.getSelectedItemIndex();

                for (auto& value : customValues) value = std::clamp(value, 0, currentReso - 1);

                switch ((int)customValues.size())
                {
                case 32:  customSliders32.setValues(customValues); break;
                case 64:  customSliders64.setValues(customValues); break;
                case 128: customSliders128.setValues(customValues); break;
                case 256: customSliders256.setValues(customValues); break;
                default: break;
                }
            }
        });

}

void GuiWt2::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::wt2Param)), Io::saveGlob(Io::Extension::wt2Param));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(wt2Format);
                writeChParams(writer);

                writer.writeTo(file);
            }
        });

}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiWt2::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Form
	int selectedSizeIdx = lines[index++].getIntValue();
	int selectedResoIdx = lines[index++].getIntValue();
	int selectedWaveIdx = lines[index++].getIntValue();

	sizeSelector.setSelectedItemIndex(selectedSizeIdx, juce::sendNotification);
	resoSelector.setSelectedItemIndex(selectedResoIdx, juce::sendNotification);
	waveSelector.setSelectedItemIndex(selectedWaveIdx, juce::sendNotification);

	// Moduration
	modComponent.setImportingBaseParams(lines, index);

	// Components
	fixComponent.setImportingParams(lines, index);
	ampEnvComponent.setImportingParams(lines, index);
	pitchEnvComponent.setImportingParams(lines, index);
	ssgHwEnv.setImportingParams(lines, index);
	ssgSwEnvComponent.setImportingParams(lines, index);
	ssgSwEnv11Component.setImportingParams(lines, index);
	ssgSwPEnv11Component.setImportingParams(lines, index);
	mulDetuneComponent.setImportingParams(lines, index);
	lfo.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	if (selectedWaveIdx == 8)
	{
	    // sizeSelectorの戻り値インデックス(0: 32, 1: 64, 2: 128, 3: 256)に依存してステップ数を変更
	    int sampleCount = 32;
	    if (selectedSizeIdx == 1)      sampleCount = 64;
	    else if (selectedSizeIdx == 2) sampleCount = 128;
	    else if (selectedSizeIdx == 3) sampleCount = 256;

	    // 現在の解像度(resolution)の範囲に合わせて安全にクランプするための最大値上限を計算
	    int currentReso = 16 << selectedResoIdx;

	    std::vector<int> customValues(sampleCount, currentReso >> 1);
	    for (int i = 0; i < sampleCount; ++i)
	    {
	        if (index < lines.size()) {
	            int val = lines[index++].getIntValue();
	            customValues[i] = std::clamp(val, 0, currentReso - 1);
	        }
	    }

	    // 適切なWaveform2Containerへ整数配列を一括反映
	    if (sampleCount == 32)       customSliders32.setValues(customValues);
	    else if (sampleCount == 64)  customSliders64.setValues(customValues);
	    else if (sampleCount == 128) customSliders128.setValues(customValues);
	    else if (sampleCount == 256) customSliders256.setValues(customValues);
	}

	// Modulation Shape は後から追加したパラメータなので、
	// 旧フォーマットとの互換のためファイル末尾から読む。
	modComponent.setImportingShapeParam(lines, index);


}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiWt2::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Form
	writer.set("size", sizeSelector.getSelectedItemIndex());
	writer.set("reso", resoSelector.getSelectedItemIndex());
	writer.set("wave", waveSelector.getSelectedItemIndex());

	// Modulation
	// 名前で持つので、後から足した項目を末尾へ置く必要はない。
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

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
	lfo.writeParams(writer, "lfo");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");

	// 自分で描いた波形。使っている段数のものだけを書く。
	if (waveSelector.getSelectedItemIndex() == customWaveIndex)
	{
	    std::vector<int> customValues;

	    switch (sizeSelector.getSelectedItemIndex())
	    {
	    case 0: customValues = customSliders32.getValues(); break;
	    case 1: customValues = customSliders64.getValues(); break;
	    case 2: customValues = customSliders128.getValues(); break;
	    case 3: customValues = customSliders256.getValues(); break;
	    default: break;
	    }

	    writer.setArray("customWave", customValues);
	}

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiWt2::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiWt2::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}
