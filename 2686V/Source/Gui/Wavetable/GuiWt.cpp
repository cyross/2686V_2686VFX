#include <vector>

#include "./GuiWt.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Wavetable/ProcessorWtKeys.h"
#include "../../Processor/Wavetable/ProcessorWtValues.h"
#include "../../Core/Const/ConstFileValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiWtValues.h"
#include "./GuiWtText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiWtHelpers.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
};

static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 33.08kHz", .value = 6 },
    {.name = " 7: 22.05kHz", .value = 7 },
    {.name = " 8: 16kHz",    .value = 8 },
    {.name = " 9: 12kHz",    .value = 9 },
    {.name = "10: 11kHz",    .value = 10 },
    {.name = "11: 8kHz",     .value = 11 },
    {.name = "12: 5.5kHz",   .value = 12 },
    {.name = "13: 4kHz",     .value = 13 },
    {.name = "14: 2kHz",     .value = 14 },
};

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

static std::vector<SelectItem> wtStepsItems = {
    {.name = juce::String("") + "段階なし",   .value =  1 },
    {.name = juce::String("") + " 16段階(  7 /   8)",  .value =  2 },
    {.name = juce::String("") + " 32段階( 15 /  16)",  .value =  3 },
    {.name = juce::String("") + " 64段階( 31 /  32)",  .value =  4 },
    {.name = juce::String("") + "128段階( 63 /  64)", .value =  5 },
    {.name = juce::String("") + "256段階(127 / 128)", .value =  6 },
    {.name = juce::String("") + " 16段階(  8 /   7)",  .value =  7 },
    {.name = juce::String("") + " 32段階( 16 /  15)",  .value =  8 },
    {.name = juce::String("") + " 64段階( 32 /  31)",  .value =  9 },
    {.name = juce::String("") + "128段階( 64 /  63)", .value = 10 },
    {.name = juce::String("") + "256段階(128 / 127)", .value = 11 },
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
WaveformContainer<tableSize>::WaveformContainer(const GuiContext& context) : GuiBaseComponent(context)
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
}

template <size_t tableSize>
WaveformContainer<tableSize>::~WaveformContainer()
{
    // PluginProcessorの完全な定義を読み込んでいるので、エラーなくアクセスできる
    for (const auto& id : m_paramIds) {
        ctx.audioProcessor.apvts.removeParameterListener(id, this);
    }
}

// パラメータが外部(Undoなど)から変更されたときに呼ばれる
template <size_t tableSize>
void WaveformContainer<tableSize>::parameterChanged(const juce::String& parameterID, float newValue)
{
    // 描画を予約（メッセージスレッドで実行される）
    juce::MessageManager::callAsync([this] { repaint(); });
}

template <size_t tableSize>
void WaveformContainer<tableSize>::setup(const Config& c)
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
void WaveformContainer<tableSize>::applySmoothing()
{
    if (!isEnabledState || m_params.empty()) return;

    std::vector<float> temp(tableSize);

    // 1. 加重移動平均の計算
    for (size_t i = 0; i < tableSize; ++i)
    {
        size_t prev = (i == 0) ? tableSize - 1 : i - 1;
        size_t next = (i == tableSize - 1) ? 0 : i + 1;

        // APVTSは 0.0~1.0 の正規化値を持つため、本来の -1.0~1.0 の範囲に戻して計算する
        float valPrev = m_params[prev] ? m_params[prev]->convertFrom0to1(m_params[prev]->getValue()) : 0.0f;
        float valCurr = m_params[i] ? m_params[i]->convertFrom0to1(m_params[i]->getValue()) : 0.0f;
        float valNext = m_params[next] ? m_params[next]->convertFrom0to1(m_params[next]->getValue()) : 0.0f;

        temp[i] = (valPrev * 0.25f) + (valCurr * 0.5f) + (valNext * 0.25f);
    }

    // 2. ホスト(DAW)に通知を送りながらパラメータを更新
    for (size_t i = 0; i < tableSize; ++i)
    {
        if (m_params[i]) m_params[i]->setValueNotifyingHost(m_params[i]->convertTo0to1(temp[i]));
    }
    repaint();
}

template <size_t tableSize>
void WaveformContainer<tableSize>::setCustomEnabled(bool shouldBeEnabled)
{
    isEnabledState = shouldBeEnabled;
    repaint();
}

template <size_t tableSize>
void WaveformContainer<tableSize>::setAllValues(float val)
{
    if (!isEnabledState || m_params.empty()) return;
    for (size_t i = 0; i < tableSize; ++i) {
        if (m_params[i]) m_params[i]->setValueNotifyingHost(m_params[i]->convertTo0to1(val));
    }
    repaint();
}

// =======================================================
// 外部から配列で値を一括セットする
// =======================================================
template <size_t tableSize>
void WaveformContainer<tableSize>::setValues(const std::vector<float>& values)
{
    if (m_params.empty()) return;
    for (size_t i = 0; i < tableSize && i < values.size(); ++i) {
        if (m_params[i]) m_params[i]->setValueNotifyingHost(m_params[i]->convertTo0to1(values[i]));
    }
    repaint();
}

// =======================================================
// 現在の値を配列として取得する
// =======================================================
template <size_t tableSize>
std::vector<float> WaveformContainer<tableSize>::getValues()
{
    std::vector<float> vals(tableSize, 0.0f);
    if (m_params.empty()) return vals;
    for (size_t i = 0; i < tableSize; ++i) {
        if (m_params[i]) vals[i] = m_params[i]->convertFrom0to1(m_params[i]->getValue());
    }
    return vals;
}

// =======================================================
// カスタム描画 (子コンポーネントを使わず一気に描く)
// =======================================================
template <size_t tableSize>
void WaveformContainer<tableSize>::paint(juce::Graphics& g)
{
    // 1. 背景の描画
    g.fillAll(isEnabledState ? juce::Colours::darkgrey.withAlpha(0.3f) : juce::Colours::darkgrey.withAlpha(0.7f));

    if (!isEnabledState || m_params.empty()) return;

    float stepWidth = (float)getWidth() / tableSize;
    float halfHeight = getHeight() * 0.5f;

    // 2. 波形を一気に描画
    for (size_t i = 0; i < tableSize; ++i)
    {
        float x = i * stepWidth;

        // ホバーされている列の背景を黒にする
        if (i == hoveredIndex) {
            g.setColour(juce::Colours::black);
            g.fillRect(x, 0.0f, stepWidth, (float)getHeight());
        }

        // パラメータから現在の値 (-1.0 ~ 1.0) を取得
        float val = m_params[i] ? m_params[i]->convertFrom0to1(m_params[i]->getValue()) : 0.0f;

        // ホバーされている列の波形を赤色、それ以外は通常色
        g.setColour((i == hoveredIndex) ? juce::Colours::red : GuiColor::WaveformContainer::Thumb);

        float drawY = halfHeight - (val * halfHeight);
        float drawH = halfHeight - drawY;

        if (drawH < 0) {
            drawY += drawH;
            drawH = -drawH;
        }

        // バーが潰れないように、幅が十分ある場合は1pxの隙間を空ける
        float drawW = (stepWidth > 2.0f) ? stepWidth - 1.0f : stepWidth;
        g.fillRect(x, drawY, drawW, drawH);

        g.setColour((i == hoveredIndex) ? juce::Colours::magenta : juce::Colours::green);

        g.fillRect(x, halfHeight, drawW, 1.0f);
    }
}

// =======================================================
// マウス操作の自前処理
// =======================================================
template <size_t tableSize>
void WaveformContainer<tableSize>::mouseMove(const juce::MouseEvent& e) {
    updateHoverState(e);
}

template <size_t tableSize>
void WaveformContainer<tableSize>::mouseDown(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

    // マウスをクリックした瞬間に「新しいUndoの区切り」を作る
    ctx.audioProcessor.undoManager.beginNewTransaction();

    updateSliderValue(e);
    updateHoverState(e);
}

template <size_t tableSize>
void WaveformContainer<tableSize>::mouseDrag(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

    updateSliderValue(e);
    updateHoverState(e);
}

template <size_t tableSize>
void WaveformContainer<tableSize>::mouseExit(const juce::MouseEvent& e) {
    hoveredIndex = -1;
    repaint();
}

template <size_t tableSize>
void WaveformContainer<tableSize>::updateSliderValue(const juce::MouseEvent& e)
{
    if (!isEnabledState || m_params.empty()) return;

    float stepWidth = (float)getWidth() / tableSize;
    int index = (int)(e.position.x / stepWidth);
    index = std::clamp(index, 0, (int)tableSize - 1);

    float halfHeight = getHeight() * 0.5f;
    float val = 1.0f - (e.position.y / halfHeight);
    val = std::clamp(val, -1.0f, 1.0f);

    // 段階スナップ(+)が有効な場合
    if (steps > 0)
    {
        int maxIndex = steps - 1;        // 例: 16段階なら 15
        int zeroIndex = steps / 2 - 1;   // 例: 16段階なら 8 (8段階目)

        int step = 0;
        // 1. マウス位置(val)から現在のステップ(段階)を算出
        if (val < 0.0f) {
            step = (int)std::round(val * zeroIndex + zeroIndex);
        }
        else {
            step = (int)std::round(val * (maxIndex - zeroIndex) + zeroIndex);
        }
        step = std::clamp(step, 0, maxIndex);

        // 2. ステップから正確な値(-1.0 〜 1.0)を再計算
        if (step < zeroIndex) {
            val = (float)(step - zeroIndex) / (float)zeroIndex;
        }
        else if (step > zeroIndex) {
            val = (float)(step - zeroIndex) / (float)(maxIndex - zeroIndex);
        }
        else {
            val = 0.0f; // 完全に0.0に固定
        }
    }
    // 段階スナップ(-)が有効な場合
    else if (steps < 0)
    {
        steps = -steps;

        int maxIndex = steps - 1;        // 例: 16段階なら 15
        int zeroIndex = steps / 2 - 1;   // 例: 16段階なら 8 (8段階目)

        int step = 0;
        // 1. マウス位置(val)から現在のステップ(段階)を算出
        if (val < 0.0f) {
            step = (int)std::round(val * (maxIndex - zeroIndex) + zeroIndex);
        }
        else {
            step = (int)std::round(val * zeroIndex + zeroIndex);
        }
        step = std::clamp(step, 0, maxIndex);

        // 2. ステップから正確な値(-1.0 〜 1.0)を再計算
        if (step < zeroIndex) {
            val = (float)(step - zeroIndex) / (float)zeroIndex;
        }
        else if (step > zeroIndex) {
            val = (float)(step - zeroIndex) / (float)(maxIndex - zeroIndex);
        }
        else {
            val = 0.0f; // 完全に0.0に固定
        }
    }
    else
    {
        // 修飾キーによる高精度ドラッグ操作
        bool isAlt = e.mods.isAltDown();
        bool isShift = e.mods.isShiftDown();
        bool isCtrl = e.mods.isCtrlDown() || e.mods.isCommandDown();

        if (isAlt) {
            val = std::round(val / 0.1f) * 0.1f;
        }
        else if (isShift) {
            val = std::round(val / 0.01f) * 0.01f;
        }
        else if (isCtrl) {
            val = std::round(val / 0.05f) * 0.05f;
        }
    }

    if (m_params[index] != nullptr) {
        m_params[index]->setValueNotifyingHost(m_params[index]->convertTo0to1(val));
    }

    repaint();
}

template <size_t tableSize>
void WaveformContainer<tableSize>::updateHoverState(const juce::MouseEvent& e)
{
    if (!isEnabledState) return;

    float stepWidth = (float)getWidth() / tableSize;
    int index = (int)(e.position.x / stepWidth);
    index = std::clamp(index, 0, (int)tableSize - 1);

    if (hoveredIndex != index) {
        hoveredIndex = index;
    }

    lastMousePos = e.position.toInt();
    lastModifiers = e.mods; // ShiftやCtrlの押下状態を記録

    repaint();
}

template <size_t tableSize>
void WaveformContainer<tableSize>::paintOverChildren(juce::Graphics& g)
{
    if (hoveredIndex >= 0 && hoveredIndex < tableSize && isEnabledState && !m_params.empty())
    {
        float halfHeight = getHeight() * 0.5f;
        float potentialVal = 1.0f - ((float)lastMousePos.y / halfHeight);
        potentialVal = std::clamp(potentialVal, -1.0f, 1.0f);

        juce::String text;

        if (steps > 0)
        {
            int maxIndex = steps - 1;
            int zeroIndex = steps / 2 - 1;

            int stepValue = 0;

            // 1. プレビュー用ステップの算出
            if (potentialVal < 0.0f) {
                stepValue = (int)std::round(potentialVal * zeroIndex + zeroIndex);
            }
            else {
                stepValue = (int)std::round(potentialVal * (maxIndex - zeroIndex) + zeroIndex);
            }
            stepValue = std::clamp(stepValue, 0, maxIndex);

            // 2. ツールチップ表示用に正確な位置へスナップ
            if (stepValue < zeroIndex) {
                potentialVal = (float)(stepValue - zeroIndex) / (float)zeroIndex;
            }
            else if (stepValue > zeroIndex) {
                potentialVal = (float)(stepValue - zeroIndex) / (float)(maxIndex - zeroIndex);
            }
            else {
                potentialVal = 0.0f;
            }

            // [index] step/steps (value) でテキストを作成
            text = "[" + juce::String(hoveredIndex) + "] " + juce::String(stepValue + 1) + "/" + juce::String(steps) + "(" + juce::String(potentialVal, 3) + ")";
        }
        else if (steps < 0)
        {
            steps = -steps;

            int maxIndex = steps - 1;
            int zeroIndex = steps / 2 - 1;

            int stepValue = 0;

            // 1. プレビュー用ステップの算出
            if (potentialVal < 0.0f) {
                stepValue = (int)std::round(potentialVal * (maxIndex - zeroIndex) + zeroIndex);
            }
            else {
                stepValue = (int)std::round(potentialVal * zeroIndex + zeroIndex);
            }
            stepValue = std::clamp(stepValue, 0, maxIndex);

            // 2. ツールチップ表示用に正確な位置へスナップ
            if (stepValue < zeroIndex) {
                potentialVal = (float)(stepValue - zeroIndex) / (float)zeroIndex;
            }
            else if (stepValue > zeroIndex) {
                potentialVal = (float)(stepValue - zeroIndex) / (float)(maxIndex - zeroIndex);
            }
            else {
                potentialVal = 0.0f;
            }

            // [index] step/steps (value) でテキストを作成
            text = "[" + juce::String(hoveredIndex) + "] " + juce::String(stepValue + 1) + "/" + juce::String(steps) + "(" + juce::String(potentialVal, 3) + ")";
        }
        else
        {
            // 修飾キーが押されている場合は、そのスナップ値もプレビューに反映させる
            bool isAlt = lastModifiers.isAltDown();
            bool isShift = lastModifiers.isShiftDown();
            bool isCtrl = lastModifiers.isCtrlDown() || lastModifiers.isCommandDown();

            if (isAlt) {
                potentialVal = std::round(potentialVal / 0.1f) * 0.1f;
            }
            else if (isShift) {
                potentialVal = std::round(potentialVal / 0.01f) * 0.01f;
            }
            else if (isCtrl) {
                potentialVal = std::round(potentialVal / 0.05f) * 0.05f;
            }

            // [index] value でテキストを作成
            text = "[" + juce::String(hoveredIndex) + "] " + juce::String(potentialVal, 3);
        }

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

template class WaveformContainer<32>;
template class WaveformContainer<64>;
template class WaveformContainer<128>;
template class WaveformContainer<256>;

void GuiWt::setup()
{
    auto applySteps = [this] {
        int stepsIndex = stepsSelector.getSelectedId() - 1;
        int steps = 0;

        switch (stepsIndex)
        {
        case 1:
            steps = 16;
            break;
        case 2:
            steps = 32;
            break;
        case 3:
            steps = 64;
            break;
        case 4:
            steps = 128;
            break;
        case 5:
            steps = 256;
            break;
        case 6:
            steps = -16;
            break;
        case 7:
            steps = -32;
            break;
        case 8:
            steps = -64;
            break;
        case 9:
            steps = -128;
            break;
        case 10:
            steps = -256;
            break;
        }

        customSliders32.steps = steps;
        customSliders64.steps = steps;
        customSliders128.steps = steps;
        customSliders256.steps = steps;
        };
    const juce::String code = WtPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, WtGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    qualityCat.setupHwCategory({ .parent = *this, .title = WtGuiText::Category::visibleQuality, .invisibleTitle = WtGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + WtPrKey::bit, .title = WtGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + WtPrKey::rate, .title = WtGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    fixComponent.setupComponent(*this, code, tabOrder, "-> 440", 440);

    unisonComponent.setupComponent(*this, code, tabOrder);

    levelSlider.setup({ .parent = *this, .id = code + WtPrKey::level, .title = WtGuiText::Wt::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    // Waveform
    waveSelector.setup({ .parent = *this, .id = code + WtPrKey::wave, .title = WtGuiText::Wt::form, .items = wtWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);
    waveSelector.onChange = [this] {
        updateCustomWaveCatOnChange();

        ctx.editor.resized();
        };

    // Custom Wave Size
    sizeSelector.setup({ .parent = *this, .id = code + WtPrKey::sampleSize, .title = WtGuiText::Wt::size, .items = wtTsItems, .isReset = true, .isResized = true });
    sizeSelector.setWantsKeyboardFocus(true);
    sizeSelector.setExplicitFocusOrder(++tabOrder);

    // Steps
    stepsSelector.setup({ .parent = *this, .id = code + WtPrKey::steps, .title = WtGuiText::Wt::steps, .items = wtStepsItems, .isReset = true, .isResized = true });
    stepsSelector.setWantsKeyboardFocus(true);
    stepsSelector.setExplicitFocusOrder(++tabOrder);
    stepsSelector.onChange = [this] {
        auto applySteps = [this] {
            int stepsIndex = stepsSelector.getSelectedId() - 1;
            int steps = 0;

            switch (stepsIndex)
            {
            case 1:
                steps = 16;
                break;
            case 2:
                steps = 32;
                break;
            case 3:
                steps = 64;
                break;
            case 4:
                steps = 128;
                break;
            case 5:
                steps = 256;
                break;
            case 6:
                steps = -16;
                break;
            case 7:
                steps = -32;
                break;
            case 8:
                steps = -64;
                break;
            case 9:
                steps = -128;
                break;
            case 10:
                steps = -256;
                break;
            }

            customSliders32.steps = steps;
            customSliders64.steps = steps;
            customSliders128.steps = steps;
            customSliders256.steps = steps;
            };

        applySteps();

        ctx.editor.resized();
        };

    modCat.setupHwCategory({ .parent = *this, .title = WtGuiText::Category::visibleMod, .invisibleTitle = WtGuiText::Category::invisibileMod, .enableChangeDetailVisible = true });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + WtPrKey::Mod::enable, .title = WtGuiText::Wt::Mod::enable, .isReset = true, .isResized = true });
    modEnableButton.setWantsKeyboardFocus(true);
    modEnableButton.setExplicitFocusOrder(++tabOrder);

    modDepthSlider.setup({ .parent = *this, .id = code + WtPrKey::Mod::depth, .title = WtGuiText::Wt::Mod::depth, .isReset = true });
    modDepthSlider.setWantsKeyboardFocus(true);
    modDepthSlider.setExplicitFocusOrder(++tabOrder);

    modSpeedSlider.setup({ .parent = *this, .id = code + WtPrKey::Mod::speed, .title = WtGuiText::Wt::Mod::speed, .isReset = true });
    modSpeedSlider.setWantsKeyboardFocus(true);
    modSpeedSlider.setExplicitFocusOrder(++tabOrder);

    ampEnvComponent.setupComponent(*this, code, tabOrder);

    pitchEnvComponent.setupComponent(*this, code, tabOrder, WtPrKey::pitchAdsr + WtPrKey::bypass, WtGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(*this, code, tabOrder, WtPrKey::ssgSwEnv + WtPrKey::bypass, WtGuiText::SsgSwEnv::bypass);

    mulDetuneComponent.setupComponent(*this, code, tabOrder);

    lfoCat.setupSwCategory({ .parent = *this, .title = WtGuiText::Category::visibleLfo, .invisibleTitle = WtGuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

    lfoPmFreqSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pmFreq, .title = WtGuiText::Lfo::pmSpeed, .isReset = true });
    lfoPmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoPmFreqSlider.setWantsKeyboardFocus(true);
    lfoPmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmFreqSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amFreq, .title = WtGuiText::Lfo::amSpeed, .isReset = true });
    lfoAmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoAmFreqSlider.setWantsKeyboardFocus(true);
    lfoAmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amSmoothRatio, .title = WtGuiText::Lfo::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::syncDelay, .title = WtGuiText::Lfo::syncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = *this, .title = "Async", .isReset = false, .isResized = false });
    lfoSyncDelayToZeroBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToZeroBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToZeroBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(0.0f);
        };

    lfoSyncDelayToOneBtn.setup({ .parent = *this, .title = "Sync", .isReset = false, .isResized = false });
    lfoSyncDelayToOneBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToOneBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToOneBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(1.0f);
        };

    lfoPmToggle.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pm, .title = WtGuiText::Lfo::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmShapeSelector.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pmShape, .title = WtGuiText::Lfo::pmShape, .items = lfoPmShapeItems, .isReset = true });
    lfoPmShapeSelector.setWantsKeyboardFocus(true);
    lfoPmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pms, .title = WtGuiText::Lfo::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pmd, .title = WtGuiText::Lfo::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + WtPrKey::Lfo::am, .title = WtGuiText::Lfo::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmShapeSelector.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amShape, .title = WtGuiText::Lfo::amShape, .items = lfoAmShapeItems, .isReset = true });
    lfoAmShapeSelector.setWantsKeyboardFocus(true);
    lfoAmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::ams, .title = WtGuiText::Lfo::ams, .isReset = true });
    lfoAmsSlider.setWantsKeyboardFocus(true);
    lfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amd, .title = WtGuiText::Lfo::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    midiComponent.setupComponent(*this, tabOrder);

    // Custom Wave Group
	customWaveGroup.setup(*this, WtGuiText::Group::wtCustom);

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom64 });
    customSliders128.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom128 });
    customSliders256.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom256 });

	customWaveResetTo0Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::to0, .bgColor = GuiColor::WaveformContainer::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.setWantsKeyboardFocus(true);
    customWaveResetTo0Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0.0f);
        customSliders64.setAllValues(0.0f);
        customSliders128.setAllValues(0.0f);
        customSliders256.setAllValues(0.0f);
        ctx.editor.resized();
    };

    customWaveResetTo1Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::to1, .bgColor = GuiColor::WaveformContainer::ResetBtn::To1, .isReset = false, .isResized = false });
    customWaveResetTo1Btn.setWantsKeyboardFocus(true);
    customWaveResetTo1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo1Btn.onClick = [this] {
        customSliders32.setAllValues(1.0f);
        customSliders64.setAllValues(1.0f);
        customSliders128.setAllValues(1.0f);
        customSliders256.setAllValues(1.0f);
        ctx.editor.resized();
        };

    customWaveResetToM1Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::toM1, .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1, .isReset = false, .isResized = false });
    customWaveResetToM1Btn.setWantsKeyboardFocus(true);
    customWaveResetToM1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetToM1Btn.onClick = [this] {
        customSliders32.setAllValues(-1.0f);
        customSliders64.setAllValues(-1.0f);
        customSliders128.setAllValues(-1.0f);
        customSliders256.setAllValues(-1.0f);
        resized();
        };

    customWaveSmoothBtn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::smooth, .bgColor = juce::Colours::darkcyan, .isReset = false, .isResized = false });
    customWaveSmoothBtn.setWantsKeyboardFocus(true);
    customWaveSmoothBtn.setExplicitFocusOrder(++tabOrder);
    customWaveSmoothBtn.onClick = [this] {
        // 現在選択されている波形サイズに応じてスムージングを実行
        int sizeId = sizeSelector.getSelectedId();
        if (sizeId == 1) customSliders32.applySmoothing();
        else if (sizeId == 2) customSliders64.applySmoothing();
        else if (sizeId == 3) customSliders128.applySmoothing();
        else if (sizeId == 4) customSliders256.applySmoothing();
        };

    waveFileCat.setup({ .parent = *this, .title = WtGuiText::Category::visibleWaveFile, .invisibleTitle = WtGuiText::Category::invisibleWaveFile, .enableChangeDetailVisible = true });

    customWaveImportBtn.setup({ .parent = *this, .title = WtGuiText::Wt::fileImport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveImportBtn.setWantsKeyboardFocus(true);
    customWaveImportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveImportBtn.onClick = [this] { importWavetable(); };

    customWaveExportBtn.setup({ .parent = *this, .title = WtGuiText::Wt::fileExport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveExportBtn.setWantsKeyboardFocus(true);
    customWaveExportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveExportBtn.onClick = [this] { exportWavetable(); };

    applySteps();

    setupGraph();
    updateGraph();
}

void GuiWt::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(WtGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(WtGuiValue::Group::Padding::width, WtGuiValue::Group::Padding::height);
    mRect.removeFromTop(WtGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(WtGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // グラフ用の区画を確保
    layoutGraph(mRect);
    updateGraph();

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider, });

    layoutMain({ .mainRect = mRect, .label = &waveSelector.label, .component = &waveSelector });

    int index = waveSelector.getSelectedId();
    bool visible = index == 9; // custom

    if (visible)
    {
        layoutMain({ .mainRect = mRect, .label = &sizeSelector.label, .component = &sizeSelector, });
        layoutMain({ .mainRect = mRect, .label = &stepsSelector.label, .component = &stepsSelector, });
    }

    layoutModulationCat(mRect);

    ampEnvComponent.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);

    layoutLfoCat(mRect);

    layoutWavefileCat(mRect);

    fixComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

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
    customWaveResetTo0Btn.setEnabled(isCustomWave);
    customWaveResetTo1Btn.setEnabled(isCustomWave);
    customWaveResetToM1Btn.setEnabled(isCustomWave);
    customWaveSmoothBtn.setEnabled(isCustomWave);

    // Custom Wave
    // Custom Mode Layout
    // 中央に32/64本のスライダーを配置
    // 波形選択が Custom 以外の時は Disabled 表示
    auto rightArea = pageArea.removeFromLeft(WtGuiValue::RightWidth);

    customWaveGroup.setBounds(rightArea);

    auto cwRect = customWaveGroup.getBounds().reduced(WtGuiValue::Group::Padding::width, WtGuiValue::Group::Padding::height);
    cwRect.removeFromTop(WtGuiValue::Group::TitlePaddingTop);

    auto containerArea = cwRect.removeFromTop(WtGuiValue::Custom::Slider::Height);

    customSliders32.setBounds(containerArea);
    customSliders64.setBounds(containerArea);
    customSliders128.setBounds(containerArea);
    customSliders256.setBounds(containerArea);

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    int waveSize = 0;

    switch (sizeSelector.getSelectedId())
    {
    case 1:
        waveSize = 32;
        break;
    case 2:
        waveSize = 64;
        break;
    case 3:
        waveSize = 128;
        break;
    case 4:
        waveSize = 256;
        break;
    }

    customWaveGroup.setEnabled(isCustomWave);

    customSliders32.setVisible(false);
    customSliders32.setCustomEnabled(false);
    customSliders64.setVisible(false);
    customSliders64.setCustomEnabled(false);
    customSliders128.setVisible(false);
    customSliders128.setCustomEnabled(false);
    customSliders256.setVisible(false);
    customSliders256.setCustomEnabled(false);

    if (isCustomWave) {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
            customSliders32.setCustomEnabled(true);
        }
        else if (waveSize == 64) {
            customSliders64.setVisible(true);
            customSliders64.setCustomEnabled(true);
        }
        else if (waveSize == 128) {
            customSliders128.setVisible(true);
            customSliders128.setCustomEnabled(true);
        }
        else {
            customSliders256.setVisible(true);
            customSliders256.setCustomEnabled(true);
        }
    }
    else {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
        }
        else if (waveSize == 64) {
            customSliders64.setVisible(true);
        }
        else if (waveSize == 128) {
            customSliders128.setVisible(true);
        }
        else {
            customSliders256.setVisible(true);
        }
    }

    cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::Padding::Top);
    
    auto resetRect = cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::height);
    
    layoutRowWtWaveValueUpdate({
        .rect = resetRect,
        .resetTo0Btn = &customWaveResetTo0Btn,
        .resetTo1Btn = &customWaveResetTo1Btn,
        .resetToM1Btn = &customWaveResetToM1Btn,
        .rowHeight = WtGuiValue::Custom::ResetBtn::height,
        .paddingRight = WtGuiValue::Custom::ResetBtn::Padding::Right
        });

    cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::Padding::Bottom + WtGuiValue::Custom::ResetBtn::Padding::Top);

    auto smoothRect = cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::height);

    customWaveSmoothBtn.setBounds(smoothRect.reduced(2, 0));

    updateCustomWaveCatOnChange();
}

void GuiWt::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiWt::updateCustomWaveCatOnChange()
{
    int index = waveSelector.getSelectedId();
    bool visible = index == 9; // custom

    sizeSelector.setVisibleWithLabel(visible);
    stepsSelector.setVisibleWithLabel(visible);
}

// ==============================================================================
// Import / Export Logic
// ==============================================================================
void GuiWt::importWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importWavetableFile, defaultDir, Io::ExtensionGlob::wavetable);
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

                // デフォルト0.0で配列を初期化（足りない部分は0.0で埋まる）
                std::vector<float> values(sampleCount, 0.0f);

                // 2行目以降の値を読み込み、-1.0 ~ 1.0 にクランプして格納
                for (int i = 0; i < sampleCount; ++i) {
                    if (i + 1 < lines.size()) {
                        float val = lines[i + 1].getFloatValue();
                        values[i] = std::clamp(val, -1.0f, 1.0f);
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

                // --- 値をAPVTS(スライダー)に反映 ---
                if (sampleCount == 32) customSliders32.setValues(values);
                else if (sampleCount == 64) customSliders64.setValues(values);
                else if (sampleCount == 128) customSliders128.setValues(values);
                else if (sampleCount == 256) customSliders256.setValues(values);
            }
        });
}

void GuiWt::exportWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportWavetableFile, defaultDir.getChildFile("custom_wave.wt"), Io::ExtensionGlob::wavetable);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                // 現在のサイズIDを取得
                int sizeId = sizeSelector.getSelectedId();
                std::vector<float> values;

                if (sizeId == 1) values = customSliders32.getValues();
                else if (sizeId == 2) values = customSliders64.getValues();
                else if (sizeId == 3) values = customSliders128.getValues();
                else if (sizeId == 4) values = customSliders256.getValues();

                if (values.empty()) return;

                // 1行目にサンプル数
                juce::String content = juce::String(values.size()) + "\n";

                // 2行目以降に値を書き込む
                for (float v : values) {
                    content += juce::String(v, 6) + "\n"; // 小数点以下6桁まで保存
                }

                file.replaceWithText(content);
            }
        });
}

void GuiWt::initParams()
{
    this->ctx.audioProcessor.initParams("WT_");
}

void GuiWt::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    bitSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void GuiWt::layoutModulationCat(juce::Rectangle<int>& rect)
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

void GuiWt::layoutWavefileCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &waveFileCat });

    bool visible = waveFileCat.isDetailVisible();

    customWaveImportBtn.setVisible(visible);
    customWaveExportBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &customWaveImportBtn });
        layoutMain({ .mainRect = rect, .component = &customWaveExportBtn });
    }
}

void GuiWt::layoutLfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoPmFreqSlider.setVisibleWithLabel(visible);
    lfoAmFreqSlider.setVisibleWithLabel(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmShapeSelector.setVisibleWithLabel(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmShapeSelector.setVisibleWithLabel(visible);
    lfoAmsSlider.setVisibleWithLabel(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoPmFreqSlider.label, .component = &lfoPmFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmFreqSlider.label, .component = &lfoAmFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPmShapeSelector.label, .component = &lfoPmShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoAmShapeSelector.label, .component = &lfoAmShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmsSlider.label, .component = &lfoAmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    }
}

void GuiWt::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    auto repaintGraph = [this]() { updateGraph(); };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator);
    graphSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiWt::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiWt::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(WtGuiValue::MainGroup::Graph::height + WtGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(WtGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(WtGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiWt::updateGraph()
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
    // Amp Env
    // =============================================================
    else {
        ampEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
}
