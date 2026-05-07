#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiColor.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"

// ==========================================================
// Waveform Drawing Container (Super Lightweight Custom Paint)
// ==========================================================
template <size_t tableSize>
class WaveformContainer :
    public juce::Component,
    public GuiBaseComponent,
    public juce::AudioProcessorValueTreeState::Listener // リスナーを継承
{
    // APVTSのパラメータへの直接ポインタを保持して高速にアクセスする
    std::vector<juce::RangedAudioParameter*> m_params;
    juce::StringArray m_paramIds; // リスナー削除用にIDを格納

    bool isEnabledState = false;
    int hoveredIndex = -1;
    juce::Point<int> lastMousePos;
    juce::ModifierKeys lastModifiers;

public:
    WaveformContainer(const GuiContext& context) : GuiBaseComponent(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    // デストラクタでリスナーを解除しないとクラッシュするため
    ~WaveformContainer() override
    {
        for (const auto& id : m_paramIds) {
            ctx.audioProcessor.apvts.removeParameterListener(id, this);
        }
    }

    // パラメータが外部(Undoなど)から変更されたときに呼ばれる
    void parameterChanged(const juce::String& parameterID, float newValue) override
    {
        // 描画を予約（メッセージスレッドで実行される）
        juce::MessageManager::callAsync([this] { repaint(); });
    }

    struct Config {
        juce::Component& parent;
        juce::String idPrefix;
    };

    void setup(const Config& c)
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

    void applySmoothing()
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

    void setCustomEnabled(bool shouldBeEnabled)
    {
        isEnabledState = shouldBeEnabled;
        repaint();
    }

    void setAllValues(float val)
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
    void setValues(const std::vector<float>& values)
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
    std::vector<float> getValues()
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
    void paint(juce::Graphics& g) override
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
    void mouseMove(const juce::MouseEvent& e) override {
        updateHoverState(e);
    }

    void mouseDown(const juce::MouseEvent& e) override { 
        if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

        // マウスをクリックした瞬間に「新しいUndoの区切り」を作る
        ctx.audioProcessor.undoManager.beginNewTransaction();

        updateSliderValue(e);
        updateHoverState(e);
    }

    void mouseDrag(const juce::MouseEvent& e) override {
        if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

        updateSliderValue(e);
        updateHoverState(e);
    }

    void mouseExit(const juce::MouseEvent& e) override {
        hoveredIndex = -1;
        repaint();
    }

    void updateSliderValue(const juce::MouseEvent& e)
    {
        if (!isEnabledState || m_params.empty()) return;

        float stepWidth = (float)getWidth() / tableSize;
        int index = (int)(e.position.x / stepWidth);
        index = std::clamp(index, 0, (int)tableSize - 1);

        float halfHeight = getHeight() * 0.5f;
        float val = 1.0f - (e.position.y / halfHeight);
        val = std::clamp(val, -1.0f, 1.0f);

        // =======================================================
        // 1, 2, 3, 4, 5 キーの押下状態を取得 (テンキーにも対応)
        // =======================================================
        bool isKey1 = juce::KeyPress::isKeyCurrentlyDown('1') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad1);
        bool isKey2 = juce::KeyPress::isKeyCurrentlyDown('2') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad2);
        bool isKey3 = juce::KeyPress::isKeyCurrentlyDown('3') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad3);
        bool isKey4 = juce::KeyPress::isKeyCurrentlyDown('4') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad4);
        bool isKey5 = juce::KeyPress::isKeyCurrentlyDown('5') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad5);

        // =======================================================
        // 6, 7, 8, 9, 0 キーの押下状態を取得 (テンキーにも対応)
        // =======================================================
        bool isKey0 = juce::KeyPress::isKeyCurrentlyDown('0') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad0);
        bool isKey9 = juce::KeyPress::isKeyCurrentlyDown('9') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad9);
        bool isKey8 = juce::KeyPress::isKeyCurrentlyDown('8') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad8);
        bool isKey7 = juce::KeyPress::isKeyCurrentlyDown('7') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad7);
        bool isKey6 = juce::KeyPress::isKeyCurrentlyDown('6') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad6);

        int steps = 0;
        if (isKey1) steps = 16;      // 16段階 (0〜15)
        else if (isKey2) steps = 32; // 32段階 (0〜31)
        else if (isKey3) steps = 64; // 64段階 (0〜63)
        else if (isKey4) steps = 128; // 128段階 (0〜127)
        else if (isKey5) steps = 256; // 256段階 (0〜255)
        else if (isKey6) steps = -256; // 256段階 (0〜255)
        else if (isKey7) steps = -128; // 128段階 (0〜127)
        else if (isKey8) steps = -64; // 64段階 (0〜63)
        else if (isKey9) steps = -32; // 32段階 (0〜31)
        else if (isKey0) steps = -16; // 16階 (0〜15)

        // 段階スナップ(+)が有効な場合
        if (steps > 0)
        {
            int maxIndex = steps;        // 例: 16段階なら 15
            int zeroIndex = steps / 2;   // 例: 16段階なら 8 (8段階目)

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

            int maxIndex = steps;        // 例: 16段階なら 15
            int zeroIndex = steps / 2;   // 例: 16段階なら 8 (8段階目)

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

    void updateHoverState(const juce::MouseEvent& e)
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

    void paintOverChildren(juce::Graphics& g) override
    {
        if (hoveredIndex >= 0 && hoveredIndex < tableSize && isEnabledState && !m_params.empty())
        {
            float halfHeight = getHeight() * 0.5f;
            float potentialVal = 1.0f - ((float)lastMousePos.y / halfHeight);
            potentialVal = std::clamp(potentialVal, -1.0f, 1.0f);

            // =======================================================
            // 1, 2, 3, 4, 5 キーの押下状態を取得 (テンキーにも対応)
            // =======================================================
            bool isKey1 = juce::KeyPress::isKeyCurrentlyDown('1') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad1);
            bool isKey2 = juce::KeyPress::isKeyCurrentlyDown('2') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad2);
            bool isKey3 = juce::KeyPress::isKeyCurrentlyDown('3') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad3);
            bool isKey4 = juce::KeyPress::isKeyCurrentlyDown('4') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad4);
            bool isKey5 = juce::KeyPress::isKeyCurrentlyDown('5') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad5);

            // =======================================================
            // 6, 7, 8, 9, 0 キーの押下状態を取得 (テンキーにも対応)
            // =======================================================
            bool isKey0 = juce::KeyPress::isKeyCurrentlyDown('0') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad0);
            bool isKey9 = juce::KeyPress::isKeyCurrentlyDown('9') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad9);
            bool isKey8 = juce::KeyPress::isKeyCurrentlyDown('8') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad8);
            bool isKey7 = juce::KeyPress::isKeyCurrentlyDown('7') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad7);
            bool isKey6 = juce::KeyPress::isKeyCurrentlyDown('6') || juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad6);

            int steps = 0;
            if (isKey1) steps = 16;
            else if (isKey2) steps = 32;
            else if (isKey3) steps = 64;
            else if (isKey4) steps = 128; // 128段階 (0〜127)
            else if (isKey5) steps = 256; // 256段階 (0〜255)
            else if (isKey6) steps = -256; // 256段階 (0〜255)
            else if (isKey7) steps = -128; // 128段階 (0〜127)
            else if (isKey8) steps = -64; // 64段階 (0〜63)
            else if (isKey9) steps = -32; // 32段階 (0〜31)
            else if (isKey0) steps = -16; // 16階 (0〜15)

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
            int textW = g.getCurrentFont().getStringWidth(text) + 12;
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
};

class GuiWt : public GuiBase
{
    // Groups
    GuiGroup mainGroup;
    GuiGroup customWaveGroup;

    WaveformContainer<32> customSliders32;
    WaveformContainer<64> customSliders64;
    WaveformContainer<128> customSliders128;
    WaveformContainer<256> customSliders256;

    GuiSlider levelSlider;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel mainCat;
    GuiCategoryLabel modCat;
    GuiCategoryLabel adsrCat;

    // Filter (ADSR)
    GuiToggleButton adsrBypassButton; // ADSR Bypass Switch
    GuiSlider attackSlider;
    GuiSlider decaySlider;
    GuiSlider sustainSlider;
    GuiSlider releaseSlider;

    // Pitch ADSR
    GuiCategoryLabel pitchAdsrCat;
    GuiToggleButton pitchAdsrBypassButton; // ADSR Bypass Switch
    GuiSlider pitchAttackSlider;
    GuiSlider pitchDecaySlider;
    GuiSlider pitchReleaseSlider;
    GuiSlider pitchStartLevelSlider;
    GuiSlider pitchAttackLevelSlider;
    GuiSlider pitchSustainLevelSlider;
    GuiSlider pitchReleaseLevelSlider;

    // Detune
    GuiCategoryLabel detuneCat;
    GuiComboBox dt1;
    GuiSlider dt2;

    GuiComboBox bitSelector;
    GuiComboBox rateSelector;
    GuiComboBox sizeSelector;

    GuiComboBox waveSelector;

    // Moduration
	GuiToggleButton modEnableButton;
    GuiSlider modDepthSlider;
    GuiSlider modSpeedSlider;

	// Custom Waveform Reset
    GuiTextButton customWaveResetTo0Btn;
    GuiTextButton customWaveResetTo1Btn;
    GuiTextButton customWaveResetToM1Btn;

	// Custom Waveform Smoothing
    GuiTextButton customWaveSmoothBtn;

    GuiCategoryLabel waveFileCat;
    GuiTextButton customWaveImportBtn;
    GuiTextButton customWaveExportBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // Master Volume
    GuiCategoryLabel mvolCat;
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly Switch
    GuiToggleButton monoModeToggle;

    // Preset Name Label
    GuiLabel presetNameLabel;
public:
	GuiWt(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        customWaveGroup(context),
        customSliders32(context),
        customSliders64(context),
        customSliders128(context),
        customSliders256(context),
        levelSlider(context),
        monoPolyCat(context),
        presetNameCat(context),
        qualityCat(context),
        mainCat(context),
        modCat(context),
        adsrCat(context),
		adsrBypassButton(context),
        attackSlider(context),
        decaySlider(context),
        sustainSlider(context),
        releaseSlider(context),
        pitchAdsrCat(context),
        pitchAdsrBypassButton(context),
        pitchAttackSlider(context),
        pitchDecaySlider(context),
        pitchReleaseSlider(context),
        pitchStartLevelSlider(context),
        pitchAttackLevelSlider(context),
        pitchSustainLevelSlider(context),
        pitchReleaseLevelSlider(context),
        detuneCat(context),
        dt1(context),
        dt2(context),
        bitSelector(context),
        rateSelector(context),
		waveFileCat(context),
        customWaveSmoothBtn(context),
        customWaveImportBtn(context),
        customWaveExportBtn(context),
        mvolCat(context),
        masterVolSlider(context),
        sizeSelector(context),
        waveSelector(context),
        modEnableButton(context),
        modDepthSlider(context),
        modSpeedSlider(context),
        customWaveResetTo0Btn(context),
        customWaveResetTo1Btn(context),
        customWaveResetToM1Btn(context),
        monoModeToggle(context),
        presetNameLabel(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& presetName);
    void importWavetable();
    void exportWavetable();
};
