#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiColor.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"
#include "../gui/GuiValues.h"

// ==========================================================
// Waveform Drawing Container
// ==========================================================
template <size_t tableSize>
class WaveformContainer : 
    public juce::Component,
    public GuiBaseComponent
{
    std::vector<std::unique_ptr<GuiSlider>> wts;
    std::vector<std::unique_ptr<GuiTextButton>> p01Btns;     // +0.1
    std::vector<std::unique_ptr<GuiTextButton>> p001Btns;     // +0.01
    std::vector<std::unique_ptr<GuiTextButton>> maxBtns;     // 1.0
    std::vector<std::unique_ptr<GuiTextButton>> halfMaxBtns; // 0.5
    std::vector<std::unique_ptr<GuiTextButton>> zeroBtns;    // 0.0
    std::vector<std::unique_ptr<GuiTextButton>> halfMinBtns; // -0.5
    std::vector<std::unique_ptr<GuiTextButton>> minBtns;     // -1.0
    std::vector<std::unique_ptr<GuiTextButton>> m001Btns;     // -0.01
    std::vector<std::unique_ptr<GuiTextButton>> m01Btns;     // -0.1

    bool isEnabledState = false;
    float sliderWidth = 0.0f;

    // リアルタイム描画用の状態保存変数
    int hoveredIndex = -1;
    juce::Point<int> lastMousePos;
public:
    WaveformContainer(const GuiContext& context) : GuiBaseComponent(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);

        for (size_t i = 0; i < tableSize; ++i)
        {
            wts.push_back(std::make_unique<GuiSlider>(context));
            p01Btns.push_back(std::make_unique<GuiTextButton>(context));
            p001Btns.push_back(std::make_unique<GuiTextButton>(context));
            maxBtns.push_back(std::make_unique<GuiTextButton>(context));
            halfMaxBtns.push_back(std::make_unique<GuiTextButton>(context));
            zeroBtns.push_back(std::make_unique<GuiTextButton>(context));
            halfMinBtns.push_back(std::make_unique<GuiTextButton>(context));
            minBtns.push_back(std::make_unique<GuiTextButton>(context));
            m001Btns.push_back(std::make_unique<GuiTextButton>(context));
            m01Btns.push_back(std::make_unique<GuiTextButton>(context));

            // このコンテナの子にする
            addAndMakeVisible(wts.back().get());
            addAndMakeVisible(p01Btns.back().get());
            addAndMakeVisible(p001Btns.back().get());
            addAndMakeVisible(maxBtns.back().get());
            addAndMakeVisible(halfMaxBtns.back().get());
            addAndMakeVisible(zeroBtns.back().get());
            addAndMakeVisible(halfMinBtns.back().get());
            addAndMakeVisible(minBtns.back().get());
            addAndMakeVisible(m001Btns.back().get());
            addAndMakeVisible(m01Btns.back().get());

            // スライダー個別の操作を無効化し、親（このコンテナ）でマウス入力をまとめて受け取る
            wts.back()->setInterceptsMouseClicks(false, false);
        }
    }
    struct Config {
        juce::Component& parent;
        juce::String idPrefix;
    };

    void setup(const Config& c)
    {
        c.parent.addAndMakeVisible(*this);

        for (size_t i = 0; i < tableSize; ++i)
        {
            GuiSlider* slider = wts[i].get();

            slider->setup({
                .parent = *this,
                .id = c.idPrefix + juce::String(i),
                .title = "",
                .trackColor = GuiColor::WaveformContainer::Track,
                .thumbColor = GuiColor::WaveformContainer::Thumb,
                .isReset = true,
                .labelFont = std::nullopt,
                .labelJustification = juce::Justification::centred,
                .labelColor = juce::Colours::white
				});
            // バーグラフ風の見た目にする
            slider->setWantsKeyboardFocus(false);
            slider->setSliderStyle(juce::Slider::LinearBarVertical);
            slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider->setRange(-1.0, 1.0);

            // 9つのボタンの色設定と、押された時の動作
            p01Btns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::P01 });
            p01Btns[i]->setWantsKeyboardFocus(false);
            p01Btns[i]->onClick = [this, i] { wts[i]->setValue(wts[i]->getValue() + 0.1f, juce::sendNotification); };

            p001Btns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::P001 });
            p001Btns[i]->setWantsKeyboardFocus(false);
            p001Btns[i]->onClick = [this, i] { wts[i]->setValue(wts[i]->getValue() + 0.01f, juce::sendNotification); };

            maxBtns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::To1 });
            maxBtns[i]->setWantsKeyboardFocus(false);
            maxBtns[i]->onClick = [this, i] { wts[i]->setValue(1.0f, juce::sendNotification); };

            halfMaxBtns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::To05 });
            halfMaxBtns[i]->setWantsKeyboardFocus(false);
            halfMaxBtns[i]->onClick = [this, i] { wts[i]->setValue(0.5f, juce::sendNotification); };

            zeroBtns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::To0 });
            zeroBtns[i]->setWantsKeyboardFocus(false);
            zeroBtns[i]->onClick = [this, i] { wts[i]->setValue(0.0f, juce::sendNotification); };

            halfMinBtns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM05 });
            halfMinBtns[i]->setWantsKeyboardFocus(false);
            halfMinBtns[i]->onClick = [this, i] { wts[i]->setValue(-0.5f, juce::sendNotification); };

            minBtns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1 });
            minBtns[i]->setWantsKeyboardFocus(false);
            minBtns[i]->onClick = [this, i] { wts[i]->setValue(-1.0f, juce::sendNotification); };

            m001Btns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::M001 });
            m001Btns[i]->setWantsKeyboardFocus(false);
            m001Btns[i]->onClick = [this, i] { wts[i]->setValue(wts[i]->getValue() - 0.01f, juce::sendNotification); };

            m01Btns[i]->setup({ .parent = *this, .title = "", .bgColor = GuiColor::WaveformContainer::ResetBtn::M01 });
            m01Btns[i]->setWantsKeyboardFocus(false);
            m01Btns[i]->onClick = [this, i] { wts[i]->setValue(wts[i]->getValue() - 0.1f, juce::sendNotification); };
        }
    }

    void resized() override
    {
        if (wts.empty()) return;

        sliderWidth = (float)getWidth() / tableSize;

        int btnHeight = GuiValue::Wt::Custom::SetBtn::height;
        int sliderHeight = getHeight() - ((btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 9 + 8);

        for (size_t i = 0; i < tableSize; ++i)
        {
            int x = (int)(i * sliderWidth);
            int w = (int)((i + 1) * sliderWidth) - x;

            wts[i]->setBounds(x, 0, w, sliderHeight);

            p01Btns[i]->setBounds(x, sliderHeight, w, btnHeight);
            p001Btns[i]->setBounds(x, sliderHeight + btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom, w, btnHeight);

            maxBtns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 2 + 4, w, btnHeight);
            halfMaxBtns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 3 + 4, w, btnHeight);
            zeroBtns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 4 + 4, w, btnHeight);
            halfMinBtns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 5 + 4, w, btnHeight);
            minBtns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 6 + 4, w, btnHeight);

            m001Btns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 7 + 8, w, btnHeight);
            m01Btns[i]->setBounds(x, sliderHeight + (btnHeight + GuiValue::Wt::Custom::SetBtn::paddingBottom) * 8 + 8, w, btnHeight);
        }
    }

    // マウス操作時に常に状態を更新して再描画(repaint)を呼ぶ
    void mouseMove(const juce::MouseEvent& e) override { updateHoverState(e); }
    void mouseDown(const juce::MouseEvent& e) override { updateSliderValue(e); updateHoverState(e); }
    void mouseDrag(const juce::MouseEvent& e) override { updateSliderValue(e); updateHoverState(e); }

    void mouseExit(const juce::MouseEvent& e) override {
        hoveredIndex = -1; // カーソルが外れたら非表示にする
        repaint();
    }

    void updateSliderValue(const juce::MouseEvent& e)
    {
        if (!isEnabledState) return;

        int index = (int)(e.position.x / sliderWidth);
        index = std::clamp(index, 0, (int)tableSize - 1);

        float sliderH = (float)wts[0]->getHeight();
        float normalizedY = 1.0f - (e.position.y / sliderH);
        float newValue = std::clamp(normalizedY * 2.0f - 1.0f, -1.0f, 1.0f);

        wts[index]->setValue(newValue, juce::sendNotification);
    }

    // マウスの位置と選択中のインデックスを記憶する
    void updateHoverState(const juce::MouseEvent& e)
    {
        if (!isEnabledState) return;

        float sliderH = (float)wts[0]->getHeight();
        // ボタンエリア（下部）にいる時はポップアップを消す
        if (e.position.y > sliderH || e.position.y < 0) {
            if (hoveredIndex != -1) { hoveredIndex = -1; repaint(); }
            return;
        }

        int index = (int)(e.position.x / sliderWidth);
        index = std::clamp(index, 0, (int)tableSize - 1);

        hoveredIndex = index;
        lastMousePos = e.getPosition();

        repaint(); // 画面を更新して paintOverChildren を呼び出す
    }

    // スライダーたちの上から、自前でツールチップ風のラベルを描画する
    void paintOverChildren(juce::Graphics& g) override
    {
        if (hoveredIndex >= 0 && hoveredIndex < tableSize && isEnabledState)
        {
            // 現在の値を文字列化
            float currentValue = (float)wts[hoveredIndex]->getValue();
            juce::String text = "[" + juce::String(hoveredIndex) + "]" + juce::String(currentValue, 2); // 小数点第2位まで

            // フォントとサイズの設定
            g.setFont(14.0f);
            int textW = g.getCurrentFont().getStringWidth(text) + 12;
            int textH = 22;

            // マウスの右上に描画する計算
            int drawX = lastMousePos.x + 12;
            int drawY = lastMousePos.y - 24;

            // 画面の右端や上端からはみ出さないように調整
            if (drawX + textW > getWidth()) drawX = getWidth() - textW;
            if (drawY < 0) drawY = lastMousePos.y + 12;

            // 半透明の黒い背景（角丸）
            g.setColour(juce::Colours::black.withAlpha(0.8f));
            g.fillRoundedRectangle((float)drawX, (float)drawY, (float)textW, (float)textH, 4.0f);

            // 白いテキスト
            g.setColour(juce::Colours::white);
            g.drawText(text, drawX, drawY, textW, textH, juce::Justification::centred, false);
        }
    }

    void setCustomEnabled(bool shouldBeEnabled)
    {
        isEnabledState = shouldBeEnabled;
        for (size_t i = 0; i < tableSize; ++i) {
            wts[i]->setEnabled(shouldBeEnabled);
            maxBtns[i]->setEnabled(shouldBeEnabled);
            halfMaxBtns[i]->setEnabled(shouldBeEnabled);
            zeroBtns[i]->setEnabled(shouldBeEnabled);
            halfMinBtns[i]->setEnabled(shouldBeEnabled);
            minBtns[i]->setEnabled(shouldBeEnabled);
        }
    }

    void setAllValues(float val)
    {
        for (auto& slider : wts) {
            slider->setValue(val, juce::sendNotification);
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

    GuiSlider levelSlider;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel mainCat;
    GuiCategoryLabel modCat;
    GuiCategoryLabel adsrCat;

    // Filter (ADSR)
    GuiSlider attackSlider;

    GuiTextButton arTo000Button;

    GuiTextButton arTo003Button;

    GuiSlider decaySlider;

    GuiSlider sustainSlider;

    GuiSlider releaseSlider;

    GuiTextButton rrTo000Button;

    GuiTextButton rrTo003Button;

	GuiComboBox bitSelector;

    GuiComboBox rateSelector;

    GuiComboBox sizeSelector;

    GuiComboBox waveSelector;

	GuiToggleButton modEnableButton;

    GuiSlider modDepthSlider;

    GuiSlider modSpeedSlider;

    GuiTextButton customWaveResetTo0Btn;
    GuiTextButton customWaveResetTo1Btn;
    GuiTextButton customWaveResetToM1Btn;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
public:
	GuiWt(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        customWaveGroup(context),
        customSliders32(context),
        customSliders64(context),
        customSliders128(context),
        levelSlider(context),
        monoPolyCat(context),
        presetNameCat(context),
        qualityCat(context),
        mainCat(context),
        modCat(context),
        adsrCat(context),
        attackSlider(context),
        arTo000Button(context),
        arTo003Button(context),
        decaySlider(context),
        sustainSlider(context),
        releaseSlider(context),
        rrTo000Button(context),
        rrTo003Button(context),
        bitSelector(context),
        rateSelector(context),
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
};
