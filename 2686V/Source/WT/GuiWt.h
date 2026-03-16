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
    // ==========================================================
        // ★追加: スライダーの描画をカスタマイズする専用 LookAndFeel
        // ==========================================================
    class WaveformSliderLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        // ホバーされているスライダーのインデックスを親から教えてもらうための変数
        int hoveredIndex = -1;
        int myIndex = -1;

        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos, float minSliderPos, float maxSliderPos,
            const juce::Slider::SliderStyle style, juce::Slider& slider) override
        {
            // ホバーされているスライダーなら背景を黒色、そうでないなら通常色
            juce::Colour trackColor;
            if (slider.isEnabled()) {
                trackColor = (hoveredIndex == myIndex) ? juce::Colours::cyan.withAlpha(0.5f) : GuiColor::WaveformContainer::Track;
            }
            else {
                trackColor = juce::Colours::darkgrey.withAlpha(0.5f);
            }

            // ホバーされているスライダーなら赤色、そうでないなら通常色
            juce::Colour thumbColor;
            if (slider.isEnabled()) {
                thumbColor = (hoveredIndex == myIndex) ? juce::Colours::red : GuiColor::WaveformContainer::Thumb;
            }
            else {
                thumbColor = juce::Colours::grey;
            }

            // --- 枠線を無くし、背景（トラック）を描画 ---
            g.setColour(trackColor);
            g.fillRect(x, y, width, height);

            // --- 実際の波形値（中身）を描画 ---
            // Y軸の中央を 0 (ゼロ) とする
            float zeroPos = y + height * 0.5f;

            g.setColour(thumbColor);

            // sliderPos が zeroPos より上（プラス値）か下（マイナス値）かで描画の起点を変える
            if (sliderPos < zeroPos) {
                // プラスの値（上向きに伸びる）
                g.fillRect((float)x, sliderPos, (float)width, zeroPos - sliderPos);
            }
            else {
                // マイナスの値（下向きに伸びる）
                g.fillRect((float)x, zeroPos, (float)width, sliderPos - zeroPos);
            }
        }
    };

    std::vector<std::unique_ptr<GuiSlider>> wts;
    std::vector<std::unique_ptr<WaveformSliderLookAndFeel>> wtsLnF;
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
            wtsLnF.push_back(std::make_unique<WaveformSliderLookAndFeel>());
            wtsLnF.back()->myIndex = (int)i; // 自分が何番目のスライダーか教える
            wts.back()->setLookAndFeel(wtsLnF.back().get()); // L&Fを適用

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

            p01Btns.back()->addMouseListener(this, false);
            p001Btns.back()->addMouseListener(this, false);
            maxBtns.back()->addMouseListener(this, false);
            halfMaxBtns.back()->addMouseListener(this, false);
            zeroBtns.back()->addMouseListener(this, false);
            halfMinBtns.back()->addMouseListener(this, false);
            minBtns.back()->addMouseListener(this, false);
            m001Btns.back()->addMouseListener(this, false);
            m01Btns.back()->addMouseListener(this, false);

            // スライダー個別の操作を無効化し、親（このコンテナ）でマウス入力をまとめて受け取る
            wts.back()->setInterceptsMouseClicks(false, false);
        }
    }

    ~WaveformContainer() override
    {
        // コンポーネント破棄時にL&Fの適用を解除する（JUCEの安全な作法）
        for (auto& slider : wts) {
            slider->setLookAndFeel(nullptr);
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
    void mouseMove(const juce::MouseEvent& e) override {
        // e.originalComponentがボタンだった場合でも、コンテナ基準の座標に直して処理する
        updateHoverState(e.getEventRelativeTo(this).position);
    }

    void mouseDown(const juce::MouseEvent& e) override {
        // スライダーエリアを直接クリックした時のみ値を変更する(ボタンクリック時は無視)
        if (e.originalComponent == this) updateSliderValue(e.getEventRelativeTo(this).position);
        updateHoverState(e.getEventRelativeTo(this).position);
    }

    void mouseDrag(const juce::MouseEvent& e) override {
        if (e.originalComponent == this) updateSliderValue(e.getEventRelativeTo(this).position);
        updateHoverState(e.getEventRelativeTo(this).position);
    }

    void mouseExit(const juce::MouseEvent& e) override {
        // isMouseOver(true) は、このコンテナ自身 または 子コンポーネント(ボタン) の
        // 領域内にマウスがあるかどうかを判定します。中にあるならホバー解除しない！
        if (isMouseOver(true)) return;

        setHoveredIndex(-1); // 完全に外に出たら非表示にする
        repaint();
    }

    // 引数を MouseEvent から Point<float> に変更
    void updateSliderValue(juce::Point<float> pos)
    {
        if (!isEnabledState) return;

        int index = (int)(pos.x / sliderWidth);
        index = std::clamp(index, 0, (int)tableSize - 1);

        float sliderH = (float)wts[0]->getHeight();
        float normalizedY = 1.0f - (pos.y / sliderH);
        float newValue = std::clamp(normalizedY * 2.0f - 1.0f, -1.0f, 1.0f);

        wts[index]->setValue(newValue, juce::sendNotification);
    }

    // ヘルパー関数 (前回追加したもの)
    void setHoveredIndex(int index)
    {
        if (hoveredIndex != index) {
            hoveredIndex = index;

            for (size_t i = 0; i < tableSize; ++i) {
                wtsLnF[i]->hoveredIndex = hoveredIndex;
                wts[i]->repaint();
            }
        }
    }

    // 引数を MouseEvent から Point<float> に変更
    void updateHoverState(juce::Point<float> pos)
    {
        if (!isEnabledState) return;

        float sliderH = (float)wts[0]->getHeight();

        int index = (int)(pos.x / sliderWidth);
        index = std::clamp(index, 0, (int)tableSize - 1);

        // ボタンエリア（下部）にいる時はポップアップ(ツールチップ)を更新しない
        if (pos.y > sliderH || pos.y < 0) {
            setHoveredIndex(index);
            repaint();
            return;
        }

        setHoveredIndex(index);
        lastMousePos = pos.toInt();

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

    void applySmoothing()
    {
        if (!isEnabledState) return;

        std::vector<float> temp(tableSize);

        // 1. 全てのポイントの平滑化された値を一旦バッファ(temp)に計算する
        for (size_t i = 0; i < tableSize; ++i)
        {
            // 波形はループしている前提なので、端の次は反対側の端を参照する
            size_t prev = (i == 0) ? tableSize - 1 : i - 1;
            size_t next = (i == tableSize - 1) ? 0 : i + 1;

            float valPrev = (float)wts[prev]->getValue();
            float valCurr = (float)wts[i]->getValue();
            float valNext = (float)wts[next]->getValue();

            // 加重移動平均 (隣25% + 自分50% + 隣25%) で角を丸める
            temp[i] = (valPrev * 0.25f) + (valCurr * 0.5f) + (valNext * 0.25f);
        }

        // 2. 計算結果をスライダーに適用して通知を飛ばす
        for (size_t i = 0; i < tableSize; ++i)
        {
            wts[i]->setValue(temp[i], juce::sendNotification);
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

    GuiTextButton customWaveSmoothBtn;

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
        customSliders256(context),
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
        customWaveSmoothBtn(context),
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
