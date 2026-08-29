#include <vector>

#include "./GuiWt.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Wavetable/ProcessorWtKeys.h"
#include "../../Processor/Wavetable/ProcessorWtValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiWtValues.h"
#include "./GuiWtText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiWtHelpers.h"

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
WaveformContainer<tableSize>::WaveformContainer(const GuiContext& context) : ParamBarEditorBase(context)
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
}

template <size_t tableSize>
void WaveformContainer<tableSize>::setup(const Config& c)
{
    c.parent.addAndMakeVisible(this);

    attachParams(c.idPrefix, (int)tableSize);
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

        paintHoverText(g, text);
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

    mainGroup.setup(*this, WtGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = WtGuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    // Waveform
    waveSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Wt::wave, .title = WtGuiText::Wt::form, .items = wtWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);
    waveSelector.onChange = [this] {
        ctx.editor.resized();
        };

    formSeparator.setupComponent(mainGroup.contentCanvas);

    // Custom Wave Size
    sizeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Wt::sampleSize, .title = WtGuiText::Wt::size, .items = wtTsItems, .isReset = true, .isResized = true });
    sizeSelector.setWantsKeyboardFocus(true);
    sizeSelector.setExplicitFocusOrder(++tabOrder);

    // Steps
    stepsSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Wt::steps, .title = WtGuiText::Wt::steps, .items = wtStepsItems, .isReset = true, .isResized = true });
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

    // 波形メモリのチャンネル自身の機能なのでハード扱いにする
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, ctx.audioProcessor.wtModWavePath, GuiColor::Category::HwBg);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, WtGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, WtGuiText::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, WtGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, WtGuiText::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfo.setupComponent(
        mainGroup.contentCanvas,
        code,
        tabOrder
    );

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = WtGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = WtGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    customWaveImportBtn.setup({ .parent = mainGroup.contentCanvas, .title = WtGuiText::Wt::fileImport, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveImportBtn.setWantsKeyboardFocus(true);
    customWaveImportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveImportBtn.onClick = [this] { importWavetable(); };

    customWaveExportBtn.setup({ .parent = mainGroup.contentCanvas, .title = WtGuiText::Wt::fileExport, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkgrey.darker(0.8f), .isReset = false, .isResized = false });
    customWaveExportBtn.setWantsKeyboardFocus(true);
    customWaveExportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveExportBtn.onClick = [this] { exportWavetable(); };

    uSep002.setupComponent(mainGroup.contentCanvas);

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", lfo);

    ieDetune.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Detune", mulDetuneComponent);

    ieAmpEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);

    iePitchEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Pitch Env", pitchEnvComponent);

    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);

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

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    // Custom Wave Group
	customWaveGroup.setup(*this, WtGuiText::Group::wtCustom);

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + CPK::custom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + CPK::custom64 });
    customSliders128.setup({ .parent = *this, .idPrefix = code + CPK::custom128 });
    customSliders256.setup({ .parent = *this, .idPrefix = code + CPK::custom256 });

	customWaveResetTo0Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::to0, .font = labelFont, .bgColor = GuiColor::WaveformContainer::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.setWantsKeyboardFocus(true);
    customWaveResetTo0Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0.0f);
        customSliders64.setAllValues(0.0f);
        customSliders128.setAllValues(0.0f);
        customSliders256.setAllValues(0.0f);
        ctx.editor.resized();
    };

    customWaveResetTo1Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::to1, .font = labelFont, .bgColor = GuiColor::WaveformContainer::ResetBtn::To1, .isReset = false, .isResized = false });
    customWaveResetTo1Btn.setWantsKeyboardFocus(true);
    customWaveResetTo1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo1Btn.onClick = [this] {
        customSliders32.setAllValues(1.0f);
        customSliders64.setAllValues(1.0f);
        customSliders128.setAllValues(1.0f);
        customSliders256.setAllValues(1.0f);
        ctx.editor.resized();
        };

    customWaveResetToM1Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::toM1, .font = labelFont, .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1, .isReset = false, .isResized = false });
    customWaveResetToM1Btn.setWantsKeyboardFocus(true);
    customWaveResetToM1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetToM1Btn.onClick = [this] {
        customSliders32.setAllValues(-1.0f);
        customSliders64.setAllValues(-1.0f);
        customSliders128.setAllValues(-1.0f);
        customSliders256.setAllValues(-1.0f);
        resized();
        };

    customWaveSmoothBtn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::smooth, .font = labelFont, .bgColor = juce::Colours::darkcyan, .isReset = false, .isResized = false });
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

    applySteps();

    setupGraph();
    updateGraph();
}

void GuiWt::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(WtGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(WtGuiValue::Group::Padding::width, WtGuiValue::Group::Padding::height);
    mmRect.removeFromTop(WtGuiValue::Group::TitlePaddingTop);

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

    ssgHwEnv.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    ssgSwEnv11Component.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwPEnv11Component.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);

    modComponent.layoutComponent(mRect);

    lfo.layoutComponent(mRect);

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
}

void GuiWt::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

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
                    content += juce::String(v, Global::floatDecimalPlaces) + "\n"; // 小数点以下6桁まで保存
                }

                file.replaceWithText(content);
            }
        });
}

void GuiWt::initParams()
{
    this->ctx.audioProcessor.initParams("WT_");
}

void GuiWt::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    int index = waveSelector.getSelectedId();
    bool visibleCustom = index == 9; // custom

    waveSelector.setVisibleWithLabel(visible);
    formSeparator.setVisible(visible && visibleCustom);
    sizeSelector.setVisibleWithLabel(visible && visibleCustom);
    stepsSelector.setVisibleWithLabel(visible && visibleCustom);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &waveSelector.label, .component = &waveSelector });

        if (visibleCustom)
        {
            formSeparator.layoutComponent(rect);
            layoutMain({ .mainRect = rect, .label = &sizeSelector.label, .component = &sizeSelector, });
            layoutMain({ .mainRect = rect, .label = &stepsSelector.label, .component = &stepsSelector, });

            rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        }
    }
}

void GuiWt::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiWt::layoutUtilityCat(juce::Rectangle<int>& rect)
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
    ieWtMod.setVisible(visible);
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
        rect.removeFromTop(4);
        ieWtMod.layoutComponent(rect);
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

void GuiWt::setupGraph()
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

        this->isUpdatingGraph = true;
        this->updateGraph();
        this->isUpdatingGraph = false;
        };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    ssgSwEnv11Component.setupGraph(repaintGraph);

    ssgSwPEnv11Component.setupGraph(repaintGraph);

    graphSeparator.setupComponent(*this);
}

void GuiWt::setGraphMode(GraphMode mode)
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

void GuiWt::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(WtGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(WtGuiValue::MainGroup::Graph::ButtonHeight);
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
void GuiWt::updateGraph()
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

void GuiWt::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiWt::importQualityParam() {
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

void GuiWt::exportQualityParam() {
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

void GuiWt::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::wtParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {
                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                // Level
                levelComponent.setImportingParams(lines, index);

                // Form
                int selectedSizeIdx = lines[index++].getIntValue();
                int selectedStepsIdx = lines[index++].getIntValue();
                int selectedWaveIdx = lines[index++].getIntValue();

                sizeSelector.setSelectedItemIndex(selectedSizeIdx, juce::sendNotification);
                stepsSelector.setSelectedItemIndex(selectedStepsIdx, juce::sendNotification);
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
                    int sampleCount = 32;
                    if (selectedSizeIdx == 1)      sampleCount = 64;
                    else if (selectedSizeIdx == 2) sampleCount = 128;
                    else if (selectedSizeIdx == 3) sampleCount = 256;

                    std::vector<float> customValues(sampleCount, 0.0f);
                    for (int i = 0; i < sampleCount; ++i)
                    {
                        if (index < lines.size()) {
                            customValues[i] = std::clamp(lines[index++].getFloatValue(), -1.0f, 1.0f);
                        }
                    }

                    // 適切なWaveformContainerへ値を一括反映
                    if (sampleCount == 32)       customSliders32.setValues(customValues);
                    else if (sampleCount == 64)  customSliders64.setValues(customValues);
                    else if (sampleCount == 128) customSliders128.setValues(customValues);
                    else if (sampleCount == 256) customSliders256.setValues(customValues);
                }

                // Modulation Shape は後から追加したパラメータなので、
                // 旧フォーマットとの互換のためファイル末尾から読む。
                modComponent.setImportingShapeParam(lines, index);

            }
        });

}

void GuiWt::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::wtParam), Io::ExtensionGlob::wtParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                // Level
                content += levelComponent.getExportedParams();

                // Form
                int selectedSizeIdx = sizeSelector.getSelectedItemIndex();
                int selectedWaveIdx = waveSelector.getSelectedItemIndex();

                content += juce::String(selectedSizeIdx) + "\n";
                content += juce::String(stepsSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(selectedWaveIdx) + "\n";

                // Moduration
                content += modComponent.getExportedBaseParams();

                // Components
                content += fixComponent.getExportedParams();
                content += ampEnvComponent.getExportedParams();
                content += pitchEnvComponent.getExportedParams();
                content += ssgHwEnv.getExportedParams();
                content += ssgSwEnvComponent.getExportedParams();
                content += ssgSwEnv11Component.getExportedParams();
                content += ssgSwPEnv11Component.getExportedParams();
                content += mulDetuneComponent.getExportedParams();
                content += lfo.getExportedParams();
                content += qualityComponent.getExportedParams();
                content += unisonComponent.getExportedParams();

                if (selectedWaveIdx == 8)
                {
                    std::vector<float> customValues;
                    if (selectedSizeIdx == 0)      customValues = customSliders32.getValues();
                    else if (selectedSizeIdx == 1) customValues = customSliders64.getValues();
                    else if (selectedSizeIdx == 2) customValues = customSliders128.getValues();
                    else if (selectedSizeIdx == 3) customValues = customSliders256.getValues();

                    for (float val : customValues)
                    {
                        content += juce::String(val, Global::floatDecimalPlaces) + "\n";
                    }
                }

                // Modulation Shape (旧フォーマットと互換を保つため末尾に置く)
                content += modComponent.getExportedShapeParam();

                file.replaceWithText(content);
            }
        });

}
