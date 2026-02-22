#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

// ==========================================================
// Waveform Drawing Container
// ==========================================================
template <size_t tableSize>
class WaveformContainer : public juce::Component, public GuiBaseComponent
{
    std::vector<std::unique_ptr<GuiSlider>> wts;

    bool isEnabledState = false;
    int sliderWidth = 0;
public:
    WaveformContainer(const GuiContext& context) : GuiBaseComponent(context)
    {
        for (size_t i = 0; i < tableSize; ++i)
        {
            wts.push_back(std::make_unique<GuiSlider>(context));

            // このコンテナの子にする
            addAndMakeVisible(wts.back().get());
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
                .trackColor = juce::Colours::white.withAlpha(0.5f),
                .isReset = true,
                .labelFont = std::nullopt,
                .labelJustification = juce::Justification::centred,
                .labelColor = juce::Colours::white
				});
            // バーグラフ風の見た目にする
            slider->setSliderStyle(juce::Slider::LinearBarVertical);
            slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider->setRange(-1.0, 1.0);
        }
    }

    void resized() override
    {
        if (wts.empty()) return;

        sliderWidth = (float)getWidth() / tableSize;

        for (size_t i = 0; i < tableSize; ++i)
        {
            int x = (int)(i * sliderWidth);
            int w = (int)((i + 1) * sliderWidth) - x;
            wts[i]->setBounds(x, 0, w, getHeight());
        }
    }

    void mouseDown(const juce::MouseEvent& e) override { updateSliderValue(e); }
    void mouseDrag(const juce::MouseEvent& e) override { updateSliderValue(e); }

    void updateSliderValue(const juce::MouseEvent& e)
    {
        if (!isEnabledState) return;

        int index = (int)(e.position.x / sliderWidth);
        index = std::clamp(index, 0, (int)tableSize - 1);

        // Y座標から -1.0 ~ 1.0 を計算
        float normalizedY = 1.0f - (e.position.y / (float)getHeight());
        float newValue = std::clamp(normalizedY * 2.0f - 1.0f, -1.0f, 1.0f);

        wts[index]->setValue(newValue, juce::sendNotification);
    }

    void setCustomEnabled(bool shouldBeEnabled)
    {
        isEnabledState = shouldBeEnabled;
        for (auto& slider : wts) {
            slider->setEnabled(shouldBeEnabled);
        }
    }

    void resetValues()
    {
        for (auto& slider : wts) {
            slider->setValue(0.0, juce::sendNotification);
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

    GuiSlider levelSlider;

    // Filter (ADSR)
    GuiSlider attackSlider;

    GuiSlider decaySlider;

    GuiSlider sustainSlider;

    GuiSlider releaseSlider;

	GuiComboBox bitSelector;

    GuiComboBox rateSelector;

    GuiComboBox sizeSelector;

    GuiComboBox waveSelector;

	GuiToggleButton modEnableButton;

    GuiSlider modDepthSlider;

    GuiSlider modSpeedSlider;

	GuiTextButton customWaveResetBtn;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;
public:
	GuiWt(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        customWaveGroup(context),
        customSliders32(context),
        customSliders64(context),
        levelSlider(context),
        attackSlider(context),
        decaySlider(context),
        sustainSlider(context),
        releaseSlider(context),
        bitSelector(context),
        rateSelector(context),
        masterVolSlider(context),
        sizeSelector(context),
        waveSelector(context),
        modEnableButton(context),
        modDepthSlider(context),
        modSpeedSlider(context),
        customWaveResetBtn(context)
    {
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
