#include <vector>

#include "./GuiWt2.h"

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
Waveform2Container<tableSize>::Waveform2Container(const GuiContext& context) : GuiBaseComponent(context)
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
}

template <size_t tableSize>
Waveform2Container<tableSize>::~Waveform2Container()
{
    // PluginProcessorの完全な定義を読み込んでいるので、エラーなくアクセスできる
    for (const auto& id : m_paramIds) {
        ctx.audioProcessor.apvts.removeParameterListener(id, this);
    }
}

// パラメータが外部(Undoなど)から変更されたときに呼ばれる
template <size_t tableSize>
void Waveform2Container<tableSize>::parameterChanged(const juce::String& parameterID, float newValue)
{
    // 描画を予約（メッセージスレッドで実行される）
    juce::MessageManager::callAsync([this] { repaint(); });
}

template <size_t tableSize>
void Waveform2Container<tableSize>::setup(const Config& c)
{
    c.parent.addAndMakeVisible(this);

    m_params.clear();
    for (size_t i = 0; i < tableSize; ++i)
    {
        juce::String paramId = c.idPrefix + juce::String(i);

        // PluginProcessor側のAPVTSからパラメータの直接ポインタを取得
        auto* param = ctx.audioProcessor.apvts.getParameter(paramId);

        if (param != nullptr) {
            m_params.push_back(param);
            m_paramIds.add(paramId);

            // パラメータごとにリスナーを登録
            ctx.audioProcessor.apvts.addParameterListener(paramId, this);
        }
    }
}

template <size_t tableSize>
void Waveform2Container<tableSize>::setCustomEnabled(bool shouldBeEnabled)
{
    isEnabledState = shouldBeEnabled;

    repaint();
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

// =======================================================
// マウス操作の自前処理
// =======================================================
template <size_t tableSize>
void Waveform2Container<tableSize>::mouseMove(const juce::MouseEvent& e) {
    updateHoverState(e);
}

template <size_t tableSize>
void Waveform2Container<tableSize>::mouseDown(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

    // マウスをクリックした瞬間に「新しいUndoの区切り」を作る
    ctx.audioProcessor.undoManager.beginNewTransaction();

    updateSliderValue(e);
    updateHoverState(e);
}

template <size_t tableSize>
void Waveform2Container<tableSize>::mouseDrag(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

    updateSliderValue(e);
    updateHoverState(e);
}

template <size_t tableSize>
void Waveform2Container<tableSize>::mouseExit(const juce::MouseEvent& e) {
    hoveredIndex = -1;
    repaint();
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

        g.setFont(14.0f);

        int textW = (int)juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), juce::StringRef(text)) + 12;
        int textH = 22;
        int drawX = lastMousePos.x + 12;
        int drawY = lastMousePos.y - 24;

        if (drawX + textW > getWidth()) drawX = getWidth() - textW;
        if (drawY < 0) drawY = lastMousePos.y + 12;

        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.fillRoundedRectangle((float)drawX, (float)drawY, (float)textW, (float)textH, 4.0f);

        g.setColour(juce::Colours::white);
        g.drawText(text, drawX, drawY, textW, textH, juce::Justification::centred, false);
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

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, Wt2GuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Category::visibleForm, .invisibleTitle = Wt2GuiText::Category::invisibleForm, .detailVisible = true, .enableChangeDetailVisible = true });

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

    modCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Category::visibleMod, .invisibleTitle = Wt2GuiText::Category::invisibileMod, .enableChangeDetailVisible = true });

    // Modulation
    modEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::enable, .title = Wt2GuiText::Wt::Mod::enable, .isReset = true, .isResized = true });
    modEnableButton.setWantsKeyboardFocus(true);
    modEnableButton.setExplicitFocusOrder(++tabOrder);

    modDepthSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::depth, .title = Wt2GuiText::Wt::Mod::depth, .isReset = true });
    modDepthSlider.setWantsKeyboardFocus(true);
    modDepthSlider.setExplicitFocusOrder(++tabOrder);

    modSpeedSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::speed, .title = Wt2GuiText::Wt::Mod::speed, .isReset = true });
    modSpeedSlider.setWantsKeyboardFocus(true);
    modSpeedSlider.setExplicitFocusOrder(++tabOrder);

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

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Category::visibleUtil, .invisibleTitle = Wt2GuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    customWaveImportBtn.setup({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Wt::fileImport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveImportBtn.setWantsKeyboardFocus(true);
    customWaveImportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveImportBtn.onClick = [this] { importWavetable(); };

    customWaveExportBtn.setup({ .parent = mainGroup.contentCanvas, .title = Wt2GuiText::Wt::fileExport, .bgColor = juce::Colours::darkgrey.darker(0.8f), .isReset = false, .isResized = false});
    customWaveExportBtn.setWantsKeyboardFocus(true);
    customWaveExportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveExportBtn.onClick = [this] { exportWavetable(); };

    uSep002.setupComponent(mainGroup.contentCanvas);

    ieLfo.setupComponent(mainGroup.contentCanvas, tabOrder, "LFO");
    ieLfo.onClickImport = [this] { importLfoParam(); };
    ieLfo.onClickExport = [this] { exportLfoParam(); };

    ieDetune.setupComponent(mainGroup.contentCanvas, tabOrder, "Detune");
    ieDetune.onClickImport = [this] { importDetuneParam(); };
    ieDetune.onClickExport = [this] { exportDetuneParam(); };

    ieAmpEnv.setupComponent(mainGroup.contentCanvas, tabOrder, "Amp Env");
    ieAmpEnv.onClickImport = [this] { importAmpEnvParam(); };
    ieAmpEnv.onClickExport = [this] { exportAmpEnvParam(); };

    iePitchEnv.setupComponent(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    iePitchEnv.onClickImport = [this] { importPitchEnvParam(); };
    iePitchEnv.onClickExport = [this] { exportPitchEnvParam(); };

    ieSsgSwEnv.setupComponent(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieSsgSwEnv.onClickImport = [this] { importSsgSwEnvParam(); };
    ieSsgSwEnv.onClickExport = [this] { exportSsgSwEnvParam(); };

    ieSsgSwEnv11.setupComponent(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieSsgSwEnv11.onClickImport = [this] { importSsgSwEnv11Param(); };
    ieSsgSwEnv11.onClickExport = [this] { exportSsgSwEnv11Param(); };

    ieSsgSwPEnv11.setupComponent(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieSsgSwPEnv11.onClickImport = [this] { importSsgSwPEnv11Param(); };
    ieSsgSwPEnv11.onClickExport = [this] { exportSsgSwPEnv11Param(); };

    ieUnison.setupComponent(mainGroup.contentCanvas, tabOrder, "Unison");
    ieUnison.onClickImport = [this] { importUnisonParam(); };
    ieUnison.onClickExport = [this] { exportUnisonParam(); };

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

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
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutFormCat(mRect);

    ampEnvComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    ssgSwEnv11Component.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwPEnv11Component.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);

    layoutModulationCat(mRect);

    lfo.layoutComponent(mRect);

    fixComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    bool isMod = modEnableButton.getToggleState();
    modDepthSlider.setEnabledWithLabel(isMod);
    modSpeedSlider.setEnabledWithLabel(isMod);

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

// ==============================================================================
// Import / Export Logic
// ==============================================================================
void GuiWt2::importWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
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
                for (int i = 0; i < sampleCount; ++i) {
                    if (i + 1 < lines.size()) {
                        int val = lines[i + 1].getIntValue();

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
                else if (sampleCount == 64) resId = 3;
                else if (sampleCount == 128) resId = 4;
                else if (sampleCount == 256) resId = 5;
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
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
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
    }
}

void GuiWt2::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiWt2::layoutModulationCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &modCat });

    bool visible = modCat.isDetailVisible();

    modEnableButton.setVisible(visible);
    modDepthSlider.setVisibleWithLabel(visible);
    modSpeedSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &modEnableButton });
        layoutMain({ .mainRect = rect, .label = &modDepthSlider.label, .component = &modDepthSlider });
        layoutMain({ .mainRect = rect, .label = &modSpeedSlider.label, .component = &modSpeedSlider, });
    }
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
    ieSsgSwEnv.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);

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

    auto repaintGraph = [this]() { updateGraph(); };

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

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

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

void GuiWt2::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiWt2::importLfoParam() {
    lfo.importParams();
}

void GuiWt2::exportLfoParam() {
    lfo.exportParams();
}

void GuiWt2::importAmpEnvParam() {
    ampEnvComponent.importParams();
}

void GuiWt2::exportAmpEnvParam() {
    ampEnvComponent.exportParams();
}

void GuiWt2::importPitchEnvParam() {
    pitchEnvComponent.importParams();
}

void GuiWt2::exportPitchEnvParam() {
    pitchEnvComponent.exportParams();
}

void GuiWt2::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void GuiWt2::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void GuiWt2::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void GuiWt2::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void GuiWt2::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiWt2::exportUnisonParam() {
    unisonComponent.exportParams();
}

void GuiWt2::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 2) return;

                qualityComponent.setBit(lines[0].getIntValue());
                qualityComponent.setRate(lines[1].getIntValue());
            }
        });
}

void GuiWt2::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default.quality"), Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(qualityComponent.getBit()) + "\n";
                content += juce::String(qualityComponent.getRate()) + "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiWt2::importSsgSwEnv11Param() {
    ssgSwEnv11Component.importParams();
}

void GuiWt2::exportSsgSwEnv11Param() {
    ssgSwEnv11Component.exportParams();
}

void GuiWt2::importSsgSwPEnv11Param() {
    ssgSwPEnv11Component.importParams();
}

void GuiWt2::exportSsgSwPEnv11Param() {
    ssgSwPEnv11Component.exportParams();
}
