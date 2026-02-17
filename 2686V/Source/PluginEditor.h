#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include "PluginProcessor.h"
#include "GlobalConstants.h"
#include "OpConstants.h"
#include "FileConstants.h"
#include "VstLogoForAbout.h"
#include "PresetConstants.h"
#include "LabelConstants.h"
#include "MmlConstants.h"
#include "GuiConstants.h"
#include "GuiStructs.h"
#include "RegisterType.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

// Defined globally as it's used in structs
static const juce::String FontFamily = "Times New Roman";
static const float LogoFontSize = 128.0f;

static const int WindowWidth = 1200;
static const int WindowHeight = 880;

#if defined(BUILD_AS_FX_PLUGIN)
static const int TabNumber = 3;
#else
static const int TabNumber = 13;
#endif

static const int LabelWidth = 40;
static const int LabelHeight = 20;
static const int SliderWidth = 50;
static const int SliderHeight = 20;
static const int SliderValueWidth = 50;
static const int SliderValueHeight = 20;
static const int ComboBoxWidth = 50;
static const int ComboBoxHeight = 20;
static const int ToggleButtonWidth = 80;
static const int ToggleButtonHeight = 20;
static const int TextButtonWidth = 80;
static const int TextButtonHeight = 20;

static const int GlobalPaddingWidth = 20;
static const int GlobalPaddingHeight = 20;
static const int GroupPaddingWidth = 10;
static const int GroupPaddingHeight = 10;
static const int TitlePaddingTop = 20;
static const int TopGroupHeight = 80;

#if !defined(BUILD_AS_FX_PLUGIN)
static const int Fm4Ops = 4;
static const int Fm2Ops = 2;

static const int WtSamples32 = 32;
static const int WtSamples64 = 64;

static const int TopParamWidth = 160;
static const int QualityGroupHeight = 60;
static const int QualityParamWidth = 160;

static const int OpLabelWidth = 40;
static const int OpLabelHeight = 15;
static const int OpSliderWidth = 50;
static const int OpSliderHeight = 15;
static const int OpSliderValueWidth = 50;
static const int OpSliderValueHeight = 15;
static const int OpComboBoxWidth = 80;
static const int OpComboBoxHeight = 15;
static const int OpToggleButtonWidth = 80;
static const int OpToggleButtonHeight = 15;
static const int OpTextButtonWidth = 80;
static const int OpTextButtonHeight = 15;
static const int OpGroupPaddingWidth = 12;
static const int OpGroupPaddingHeight = 8;

static const int FmOpRowH = 36;
static const int FmOpWidth = 240;

static const int SsgLeftWidth = 240;
static const int SsgRightWidth = 800;
static const int SsgHeight = 640;
static const int WtLeftWidth = 240;
static const int WtRightWidth = 800;
static const int WtHeight = 640;
static const int WtCustomSliderWidth = 22;
static const int RhythmPadsAreaHeight = 320;
#endif

static const int FxWidth = 1000;
static const int FxHeight = 720;
static const int FxGlobalBypassHeight = 120;
static const int FxBypassWidth = 120;
static const int FxGroupHeight = 200;
static const int FxKnobAreaWidth = 200;
static const int FxKnobWidth = 120;
static const int FxKnobHeight = 60;
static const int FxMixButtonWidth = 40;

static const Size LabelSize{ LabelWidth, LabelHeight };
static const Size SliderSize{ SliderWidth, SliderHeight };
static const Size SliderValueSize{ SliderValueWidth, SliderValueHeight };
static const Size ComboBoxSize{ ComboBoxWidth, ComboBoxHeight };
static const Size ToggleButtonSize{ ToggleButtonWidth, ToggleButtonHeight };
static const Size TextButtonSize{ TextButtonWidth, TextButtonHeight };
#if !defined(BUILD_AS_FX_PLUGIN)
static const Size OpSliderSize{ OpSliderWidth, OpSliderHeight };
static const Size OpSliderValueSize{ OpSliderValueWidth, OpSliderValueHeight };
static const Size OpComboBoxSize{ OpComboBoxWidth, OpComboBoxHeight };
static const Size OpToggleButtonSize{ OpToggleButtonWidth, OpToggleButtonHeight };
static const Size OpTextButtonSize{ OpTextButtonWidth, OpTextButtonHeight };
static const Size OpLabelSize{ OpLabelWidth, OpLabelHeight };
#endif

static const Flags SliderFlags{ true, false };
static const Flags ComboBoxFlags{ true, false };
static const Flags ToggleButtonFlags{ true, false };
static const Flags TextButtonFlags{ true, false };
#if !defined(BUILD_AS_FX_PLUGIN)
static const Flags OpSliderFlags{ true, false };
static const Flags OpComboBoxFlags{ true, false };
static const Flags OpToggleButtonFlags{ true, false };
static const Flags OpTextButtonFlags{ true, false };
#endif

class CustomTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // アイコンのパスを生成するヘルパー関数
    juce::Path getIconPath(const juce::String& name, juce::Rectangle<float> area)
    {
        juce::Path p;
        auto center = area.getCentre();
        float s = std::min(area.getWidth(), area.getHeight()) * 0.5f; // アイコンサイズ

        if (name == fxTabName) // エフェクトを示すアイコン
        {
            // [FX] Icon
            // F
            juce::Path fPath;
            fPath.addRectangle(center.x - s * 0.8f, center.y - s * 0.6f, s * 0.2f, s * 1.2f); // 縦棒
            fPath.addRectangle(center.x - s * 0.6f, center.y - s * 0.6f, s * 0.5f, s * 0.2f); // 横棒上
            fPath.addRectangle(center.x - s * 0.6f, center.y - s * 0.1f, s * 0.3f, s * 0.2f); // 横棒中
            p.addPath(fPath);

            // X
            juce::Path xPath;
            float xw = s * 0.2f;
            float xh = s * 1.2f;
            float xPivot = center.x + s * 0.4f; // Xの交差点（中心）

            // Xのクロスを、同じ中心位置にある長方形を左右に傾けて表現する
            juce::Path bar1;
            bar1.addRectangle(xPivot - xw * 0.5f, center.y - xh * 0.5f, xw, xh);
            bar1.applyTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 0.15f, xPivot, center.y));

            juce::Path bar2;
            bar2.addRectangle(xPivot - xw * 0.5f, center.y - xh * 0.5f, xw, xh);
            bar2.applyTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::pi * 0.15f, xPivot, center.y));

            p.addPath(bar1);
            p.addPath(bar2);
        }
        else if (name == presetTabName) // プリセットブラウザタブはフォルダーアイコン
        {
            // --- Folder / List Icon ---
            // フォルダーの形
            float w = s * 1.2f;
            float h = s * 0.9f;
            juce::Rectangle<float> folderRect(center.x - w / 2, center.y - h / 2 + s * 0.1f, w, h);
            p.addRoundedRectangle(folderRect, 2.0f);
            // タブ部分
            p.addRectangle(center.x - w / 2, center.y - h / 2 - s * 0.2f, w * 0.4f, s * 0.3f);

            // 中身の線 (リストっぽく)
            p.startNewSubPath(center.x - w * 0.3f, center.y);
            p.lineTo(center.x + w * 0.3f, center.y);
            p.startNewSubPath(center.x - w * 0.3f, center.y + h * 0.3f);
            p.lineTo(center.x + w * 0.3f, center.y + h * 0.3f);
        }
        else if (name == settingsTabName) // 設定タブは歯車アイコン
        {
            // --- Gear Icon ---
            // 歯車
            float rOut = s * 0.6f;
            float rIn = s * 0.45f;
            float rHole = s * 0.2f;

            p.addCentredArc(center.x, center.y, rIn, rIn, 0, 0, juce::MathConstants<float>::twoPi, true);

            // 歯を描く (8個)
            for (int i = 0; i < 8; ++i)
            {
                float angle = i * juce::MathConstants<float>::twoPi / 8.0f;
                juce::Path tooth;
                tooth.addRectangle(center.x - s * 0.12f, center.y - rOut, s * 0.24f, rOut - rIn + 2.0f);
                tooth.applyTransform(juce::AffineTransform::rotation(angle, center.x, center.y));
                p.addPath(tooth);
            }
            // 真ん中の穴
            p.addEllipse(center.x - rHole, center.y - rHole, rHole * 2, rHole * 2);
            p.setUsingNonZeroWinding(false); // 穴を抜く設定
        }
        else if (name == aboutTabName) // Aboutタブは丸囲みiアイコン
        {
            // --- Info Icon (i) ---
            // 丸
            float r = s * 0.6f;
            p.addEllipse(center.x - r, center.y - r, r * 2, r * 2);

            // i の点
            float dotSize = s * 0.15f;
            juce::Path dot;
            dot.addEllipse(center.x - dotSize / 2, center.y - r * 0.5f, dotSize, dotSize);
            p.addPath(dot);

            // i の棒
            juce::Path bar;
            bar.addRectangle(center.x - dotSize / 2, center.y - r * 0.1f, dotSize, r * 0.8f);
            p.addPath(bar);

            p.setUsingNonZeroWinding(false); // 穴を抜く設定
        }
        return p;
    }

    void drawTabButton(juce::TabBarButton & button, juce::Graphics & g, bool isMouseOver, bool isMouseDown) override
    {
        auto area = button.getActiveArea().toFloat();
        juce::String name = button.getButtonText();

        // 1. タブ背景の描画
        // 選択中タブの背景色
        auto selectedBg = juce::Colour::fromFloatRGBA(0.4f, 0.4f, 0.4f, 1.0f);
        // 非選択タブの背景色
        auto unselectedBg = juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f);

        if (button.isFrontTab()) {
            g.setColour(selectedBg);
        }
        else {
            g.setColour(unselectedBg);
            // マウスオーバーで背景も少し明るく
            if (isMouseOver) g.setColour(unselectedBg.brighter(0.1f));
        }
        g.fillRect(area);

        // 2. コンテンツ（テキストまたはアイコン）の色決定
        juce::Colour contentColour;
        if (button.isFrontTab()) {
            // 選択時は完全な白（強調）
            contentColour = juce::Colours::white;
        }
        else {
            // 非選択時はグレー
            contentColour = juce::Colours::grey;
            // マウスオーバー時は少し明るくして反応させる
            if (isMouseOver) contentColour = contentColour.brighter(0.3f);
        }

        // 3. 描画
        g.setColour(contentColour); // 色を確定

        if (name == fxTabName || name == presetTabName || name == settingsTabName || name == aboutTabName)
        {
            // アイコン描画
            juce::Path icon = getIconPath(name, area);
            g.fillPath(icon);
        }
        else
        {
            // テキスト描画
            g.setFont(juce::Font(16.0f).withStyle(button.isFrontTab() ? juce::Font::bold : juce::Font::plain));
            g.drawText(name, area, juce::Justification::centred, true);
        }
    }
};

class ColoredGroupComponent : public juce::GroupComponent
{
public:
    void setBackgroundColor(juce::Colour c)
    {
        backgroundColor = c;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        // 背景色があれば描画 (角丸で少し柔らかく)
        if (!backgroundColor.isTransparent())
        {
            g.setColour(backgroundColor);
            g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
        }

        // 親クラス（枠線とテキスト）の描画処理を呼ぶ
        juce::GroupComponent::paint(g);
    }

private:
    juce::Colour backgroundColor = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.4f);
};

#if !defined(BUILD_AS_FX_PLUGIN)
struct Fm4GuiSet
{
    juce::Component page;

    ColoredGroupComponent globalGroup;
    ColoredGroupComponent qualityGroup;
    std::array<ColoredGroupComponent, Fm4Ops> opGroups;
    ColoredGroupComponent lfoGroup;

    juce::ComboBox algSelector;
    juce::Label algLabel;
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    juce::Slider feedback2Slider;
    juce::Label feedback2Label;
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    juce::Slider lfoFreqSlider;
    juce::Label lfoFreqLabel;
    juce::ComboBox lfoPmsSelector;
    juce::Label lfoPmsLabel;
    juce::ComboBox lfoAmsSelector;
    juce::Label lfoAmsLabel;

    std::array<juce::Label, Fm4Ops> opLabels;

    // 9 columns
    std::array<juce::Slider, Fm4Ops> mul;
    std::array<juce::Slider, Fm4Ops> dt;
    std::array<juce::Slider, Fm4Ops> ar;
    std::array<juce::Slider, Fm4Ops> dr;
    std::array<juce::Slider, Fm4Ops> sr;
    std::array<juce::Slider, Fm4Ops> sl;
    std::array<juce::Slider, Fm4Ops> rr;
    std::array<juce::ToggleButton, Fm4Ops> fix;
    std::array<juce::Slider, Fm4Ops> freq;
    std::array<ColoredGroupComponent, Fm4Ops> freqBtnGroup;
    std::array<juce::TextButton, Fm4Ops> freqToZero;
    std::array<juce::TextButton, Fm4Ops> freqTo440;
    std::array<juce::Slider, Fm4Ops> tl;  // Total Level
    std::array<juce::ComboBox, Fm4Ops> ks; // Key Scale (0-3)
    std::array<juce::ToggleButton, Fm4Ops> am; // AM Enable
    std::array<juce::ComboBox, Fm4Ops> se; // SSG-EG Shape Selector
    std::array<juce::Slider, Fm4Ops> seFreq;
    std::array<juce::ToggleButton, Fm4Ops> mask; // Mask
    
    std::array<juce::TextButton, Fm4Ops> mmlBtn;

    std::array<juce::Label, Fm4Ops> mulLabel;
    std::array<juce::Label, Fm4Ops> dtLabel;
    std::array<juce::Label, Fm4Ops> arLabel;
    std::array<juce::Label, Fm4Ops> drLabel;
    std::array<juce::Label, Fm4Ops> srLabel;
    std::array<juce::Label, Fm4Ops> slLabel;
    std::array<juce::Label, Fm4Ops> rrLabel;
    std::array<juce::Label, Fm4Ops> fixLabel;
    std::array<juce::Label, Fm4Ops> freqLabel;
    std::array<juce::Label, Fm4Ops> freqBtnGroupLabel;
    std::array<juce::Label, Fm4Ops> freqToZeroLabel;
    std::array<juce::Label, Fm4Ops> freqTo440Label;
    std::array<juce::Label, Fm4Ops> tlLabel;  // Total Level
    std::array<juce::Label, Fm4Ops> ksLabel; // Key Scale (0-3)
    std::array<juce::Label, Fm4Ops> amLabel; // AM Enable
    std::array<juce::Label, Fm4Ops> seLabel; // SSG-EG Shape Selector
    std::array<juce::Label, Fm4Ops> seFreqLabel; // SSG-EG Freq
    std::array<juce::Label, Fm4Ops> maskLabel; // Mask

    std::array<juce::Label, Fm4Ops> mmlBtnLabel;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
    std::unique_ptr<SliderAttachment> lfoFreqAtt;
    std::unique_ptr<ComboBoxAttachment> lfoPmsAtt;
    std::unique_ptr<ComboBoxAttachment> lfoAmsAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> dtAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> arAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> drAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> srAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> slAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> rrAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> freqAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqToZeroAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqTo440Att;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> tlAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> ksAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> amAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> seAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> seFreqAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> fixAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> maskAtt; // Mask
};

struct Fm2GuiSet
{
    juce::Component page;

    ColoredGroupComponent globalGroup;
    ColoredGroupComponent qualityGroup;
    std::array<ColoredGroupComponent, Fm2Ops> opGroups;

    juce::ComboBox algSelector; juce::Label algLabel;
    juce::Slider feedbackSlider; juce::Label feedbackLabel;
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    std::array<juce::Label, Fm2Ops> opLabels;

    // Cols: MUL, DT, AR, DR, SL, RR, WS (7 cols)
    std::array<juce::Slider, Fm2Ops> mul;
    std::array<juce::Slider, Fm2Ops> dt;
    std::array<juce::Slider, Fm2Ops> ar;
    std::array<juce::Slider, Fm2Ops> dr;
    std::array<juce::Slider, Fm2Ops> sl;
    std::array<juce::Slider, Fm2Ops> rr;
    std::array<juce::Slider, Fm2Ops> tl;
    std::array<juce::ToggleButton, Fm2Ops> am;
    std::array<juce::ToggleButton, Fm2Ops> vib;
    std::array<juce::ToggleButton, Fm2Ops> egType;
    std::array<juce::ToggleButton, Fm2Ops> ksr;
    std::array<juce::ComboBox, Fm2Ops> ksl; // Key Scale Level
    std::array<juce::ComboBox, Fm2Ops> eg; // Envlope Generator
    std::array<juce::ToggleButton, Fm2Ops> mask; // Mask
    std::array<juce::TextButton, Fm2Ops> mmlBtn;

    std::array<juce::Label, Fm2Ops> mulLabel;
    std::array<juce::Label, Fm2Ops> dtLabel;
    std::array<juce::Label, Fm2Ops> arLabel;
    std::array<juce::Label, Fm2Ops> drLabel;
    std::array<juce::Label, Fm2Ops> slLabel;
    std::array<juce::Label, Fm2Ops> rrLabel;
    std::array<juce::Label, Fm2Ops> tlLabel;
    std::array<juce::Label, Fm2Ops> amLabel;
    std::array<juce::Label, Fm2Ops> vibLabel;
    std::array<juce::Label, Fm2Ops> egTypeLabel;
    std::array<juce::Label, Fm2Ops> ksrLabel;
    std::array<juce::Label, Fm2Ops> kslLabel;
    std::array<juce::Label, Fm2Ops> egLabel;
    std::array<juce::Label, Fm2Ops> maskLabel; // Mask
    std::array<juce::Label, Fm2Ops> mmlBtnLabel;

    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> mulAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> dtAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> arAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> drAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> slAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> rrAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> tlAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm2Ops> amAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm2Ops> vibAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm2Ops> egTypeAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm2Ops> ksrAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm2Ops> kslAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm2Ops> egAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm2Ops> maskAtt; // Mask
};

struct Opl3GuiSet
{
    juce::Component page;

    ColoredGroupComponent globalGroup;
    ColoredGroupComponent qualityGroup;
    std::array<ColoredGroupComponent, Fm4Ops> opGroups; // 4 Operators

    juce::ComboBox algSelector;
    juce::Label algLabel;
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    juce::Slider feedback2Slider;
    juce::Label feedback2Label;
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    // Sliders
    std::array<juce::Slider, Fm4Ops> mul, tl, ar, dr, sl, rr;
    std::array<juce::ToggleButton, Fm4Ops> am;
    std::array<juce::ToggleButton, Fm4Ops> vib;
    std::array<juce::ToggleButton, Fm4Ops> egType;
    std::array<juce::ToggleButton, Fm4Ops> ksr;
    std::array<juce::ComboBox, Fm4Ops> ksl; // Key Scale Level
    std::array<juce::ComboBox, Fm4Ops> eg; // Envlope Generator
    std::array<juce::ToggleButton, Fm4Ops> mask; // Mask
    std::array<juce::TextButton, Fm4Ops> mmlBtn;

    // Labels
    std::array<juce::Label, Fm4Ops> mulLabel, tlLabel, arLabel, drLabel, slLabel, rrLabel;
    std::array<juce::Label, Fm4Ops> amLabel;
    std::array<juce::Label, Fm4Ops> vibLabel;
    std::array<juce::Label, Fm4Ops> egTypeLabel;
    std::array<juce::Label, Fm4Ops> ksrLabel;
    std::array<juce::Label, Fm4Ops> kslLabel;
    std::array<juce::Label, Fm4Ops> egLabel;
    std::array<juce::Label, Fm4Ops> maskLabel; // Mask
    std::array<juce::Label, Fm4Ops> mmlBtnLabel;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt, tlAtt, arAtt, drAtt, slAtt, rrAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> amAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> vibAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> egTypeAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> ksrAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> kslAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> egAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> maskAtt; // Mask
};

struct OpmGuiSet
{
    juce::Component page;

    ColoredGroupComponent globalGroup;
    ColoredGroupComponent lfoGroup;
    ColoredGroupComponent qualityGroup;
    std::array<ColoredGroupComponent, Fm4Ops> opGroups;

    // Global
    juce::ComboBox algSelector;
    juce::Label algLabel;
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    juce::Slider feedback2Slider;
    juce::Label feedback2Label;
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    // OPM LFO
    juce::Slider lfoFreqSlider;
    juce::Label lfoFreqLabel;
    juce::ComboBox lfoWaveSelector;
    juce::Label lfoWaveLabel;
    juce::ComboBox pmsSelector;
    juce::Label pmsLabel;
    juce::ComboBox amsSelector;
    juce::Label amsLabel;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<juce::Slider, Fm4Ops> mul, dt1, dt2, tl, ar, dr, sl, sr, rr;
    std::array<juce::Label, Fm4Ops> mulLabel, dt1Label, dt2Label, tlLabel, arLabel, drLabel, slLabel, srLabel, rrLabel, ksLabel;
    std::array<juce::ComboBox, Fm4Ops> ks;
    std::array<juce::ToggleButton, Fm4Ops> mask; // Mask

    std::array<juce::ToggleButton, Fm4Ops> fix;
    std::array<juce::Slider, Fm4Ops> freq;
    std::array<juce::TextButton, Fm4Ops> freqToZero;
    std::array<juce::TextButton, Fm4Ops> freqTo440;

    std::array<juce::Label, Fm4Ops> fixLabel;
    std::array<juce::Label, Fm4Ops> freqLabel;
    std::array<juce::Label, Fm4Ops> freqToZeroLabel;
    std::array<juce::Label, Fm4Ops> freqTo440Label;
    std::array<juce::Label, Fm4Ops> maskLabel; // Mask

    std::array<juce::TextButton, Fm4Ops> mmlBtn;
    std::array<juce::Label, Fm4Ops> mmlBtnLabel;

    // Attachments
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt, dt1Att, dt2Att, tlAtt, arAtt, drAtt, slAtt, srAtt, rrAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> fixAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> freqAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> ksAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> seAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> maskAtt; // Mask
    std::unique_ptr<ComboBoxAttachment> algAtt, lfoWaveAtt, pmsAtt, amsAtt;
    std::unique_ptr<SliderAttachment> fbAtt, lfoFreqAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqToZeroAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqTo440Att;

};

struct Opzx3GuiSet
{
    juce::Component page;

    ColoredGroupComponent globalGroup;
    ColoredGroupComponent lfoGroup;
    ColoredGroupComponent qualityGroup;
    std::array<ColoredGroupComponent, Fm4Ops> opGroups;

    // Global
    juce::ComboBox algSelector;
    juce::Label algLabel;
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    juce::Slider feedback2Slider;
    juce::Label feedback2Label;
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    // OPM LFO
    juce::Slider lfoFreqSlider;
    juce::Label lfoFreqLabel;
    juce::ComboBox lfoWaveSelector;
    juce::Label lfoWaveLabel;
    juce::ComboBox pmsSelector;
    juce::Label pmsLabel;
    juce::ComboBox amsSelector;
    juce::Label amsLabel;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<juce::Slider, Fm4Ops> mul, dt1, dt2, tl, ar, dr, sl, sr, rr;
    std::array<juce::Label, Fm4Ops> mulLabel, dt1Label, dt2Label, tlLabel, arLabel, drLabel, slLabel, srLabel, rrLabel, ksLabel;
    std::array<juce::ComboBox, Fm4Ops> ks;
    std::array<juce::ToggleButton, Fm4Ops> mask; // Mask

    std::array<juce::ToggleButton, Fm4Ops> fix;
    std::array<juce::Slider, Fm4Ops> freq;
    std::array<juce::TextButton, Fm4Ops> freqToZero;
    std::array<juce::TextButton, Fm4Ops> freqTo440;
    std::array<juce::ComboBox, Fm4Ops> ws;

    std::array<juce::Label, Fm4Ops> fixLabel;
    std::array<juce::Label, Fm4Ops> freqLabel;
    std::array<juce::Label, Fm4Ops> freqToZeroLabel;
    std::array<juce::Label, Fm4Ops> freqTo440Label;
    std::array<juce::Label, Fm4Ops> wsLabel;
    std::array<juce::Label, Fm4Ops> maskLabel; // Mask

    std::array<juce::TextButton, Fm4Ops> mmlBtn;
    std::array<juce::Label, Fm4Ops> mmlBtnLabel;

    // Attachments
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt, dt1Att, dt2Att, tlAtt, arAtt, drAtt, slAtt, srAtt, rrAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> fixAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> freqAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> ksAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> seAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> wsAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> maskAtt; // Mask
    std::unique_ptr<ComboBoxAttachment> algAtt, lfoWaveAtt, pmsAtt, amsAtt;
    std::unique_ptr<SliderAttachment> fbAtt, lfoFreqAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqToZeroAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqTo440Att;

};

struct SsgGuiSet
{
    // --- SSG Page ---
    juce::Component page;

    // Pane Groups
    ColoredGroupComponent voiceGroup;
    ColoredGroupComponent filterGroup;
    ColoredGroupComponent qualityGroup;
    ColoredGroupComponent dutyGroup;
    ColoredGroupComponent triGroup;
    ColoredGroupComponent envGroup;

    juce::Slider levelSlider;
    juce::Label levelLabel;
    juce::Slider noiseSlider;
    juce::Label noiseLabel;
    juce::Slider noiseFreqSlider;
    juce::Label noiseFreqLabel;
    juce::Slider mixSlider;
    juce::Label mixLabel; // Mix Slider
    juce::TextButton mixSetTone;  // 0.0
    juce::Label mixSetToneLabel;
    juce::TextButton mixSetMix;   // 0.5
    juce::Label mixSetMixLabel;
    juce::TextButton mixSetNoise; // 1.0
    juce::Label mixSetNoiseLabel;
    juce::ComboBox waveSelector;
    juce::Label waveLabel; // Waveform Selector
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    // SSG ADSR
    juce::ToggleButton adsrBypassButton; // ADSR Bypass Switch
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label adsrBypassButtonLabel;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    // Duty Controls
    juce::ComboBox dutyModeSelector; // Preset / Variable
	juce::Label dutyModeLabel;
    juce::ComboBox dutyPresetSelector; // 1:1 ... 1:16
    juce::Label dutyPresetLabel;
    juce::Slider dutyVarSlider; // 0.0 - 0.5
    juce::Label dutyVarLabel;
    juce::ToggleButton dutyInvertButton; // Invert Switch
    juce::Label dutyInvertLabel;

    // Triangle Controls
    juce::ToggleButton triKeyTrackButton;
    juce::Label triKeyTrackLabel;
    juce::Slider triFreqSlider;
    juce::Label triFreqLabel;
    juce::Slider triPeakSlider;
    juce::Label triPeakLabel;

    // Peak Preset Buttons
    juce::TextButton triSetSawDown; // 0.0
    juce::Label triSetSawDownLabel;
    juce::TextButton triSetTri;     // 0.5
    juce::Label triSetTriLabel;
    juce::TextButton triSetSawUp;   // 1.0
    juce::Label triSetSawUpLabel;

    // HW Env Controls
    juce::ToggleButton envEnableButton;
    juce::Label envEnableLabel;
    juce::ComboBox shapeSelector;
    juce::Label shapeLabel;
    juce::Slider periodSlider;
    juce::Label periodLabel;

    // SSG Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<SliderAttachment> noiseAtt;
    std::unique_ptr<SliderAttachment> noiseFreqAtt;
    std::unique_ptr<SliderAttachment> mixAtt;
    std::unique_ptr<ButtonAttachment> mixSetAtt;
    std::unique_ptr<ComboBoxAttachment> waveAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
    std::unique_ptr<ButtonAttachment> adsrBypassAtt;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
    std::unique_ptr<ComboBoxAttachment> dutyModeAtt;
    std::unique_ptr<ComboBoxAttachment> dutyPresetAtt;
    std::unique_ptr<SliderAttachment> dutyVarAtt;
    std::unique_ptr<ButtonAttachment> dutyInvertAtt;
    std::unique_ptr<ButtonAttachment> triKeyTrackAtt;
    std::unique_ptr<SliderAttachment> triPeriodAtt;
    std::unique_ptr<SliderAttachment> triPeakAtt;
    std::unique_ptr<SliderAttachment> triFreqAtt;
    std::unique_ptr<ButtonAttachment> envEnableAtt;
    std::unique_ptr<ComboBoxAttachment> shapeAtt;
    std::unique_ptr<SliderAttachment> periodAtt;
};

struct WtGuiSet
{
    // --- Wavetable Page ---
    juce::Component page;

    // Groups
    ColoredGroupComponent filterGroup;
    ColoredGroupComponent propGroup; // Wavetable Property

    // Custom Waveform Editors
    ColoredGroupComponent customGroup;
    std::array<juce::Slider, 32> customSliders32;
    std::array<juce::Slider, 64> customSliders64;

    // Filter (ADSR)
    juce::Slider levelSlider;
    juce::Label levelLabel;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    // Properties
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox rateCombo;
    juce::Label rateLabel;
    juce::ComboBox sizeSelector;
    juce::Label sizeLabel;
    juce::ComboBox waveSelector;
    juce::Label waveLabel; // Waveform Selector

    // Modulation
    juce::ToggleButton modEnableButton;
    juce::Label modEnableLabel;
    juce::Slider modDepthSlider;
    juce::Label modDepthLabel;
    juce::Slider modSpeedSlider;
    juce::Label modSpeedLabel;

    // Wavetable Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
    std::unique_ptr<ComboBoxAttachment> sizeAtt;
    std::unique_ptr<ComboBoxAttachment> waveAtt;
    std::array<std::unique_ptr<SliderAttachment>, 32> customAtts32;
    std::array<std::unique_ptr<SliderAttachment>, 64> customAtts64;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
    std::unique_ptr<ButtonAttachment> modEnableAtt;
    std::unique_ptr<SliderAttachment> modDepthAtt;
    std::unique_ptr<SliderAttachment> modSpeedAtt;
};

struct RhythmPadGui
{
    ColoredGroupComponent group;

    juce::Label fileNameLabel;
    juce::Label noteLabel, modeLabel, rateLabel, panLabel, volLabel, oneShotLabel;

    juce::TextButton loadButton;
    juce::TextButton clearButton;
    juce::Slider noteSlider;
    juce::ComboBox modeCombo;
    juce::ComboBox rateCombo;
    juce::Slider panSlider;
    juce::TextButton btnPanL, btnPanC, btnPanR;
    juce::Slider volSlider;
    juce::ToggleButton oneShotButton;
    juce::Slider rrSlider;
    juce::Label rrLabel;

    std::unique_ptr<SliderAttachment> noteAtt;
    std::unique_ptr<ComboBoxAttachment> modeAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
    std::unique_ptr<SliderAttachment> panAtt;
    std::unique_ptr<SliderAttachment> volAtt;
    std::unique_ptr<ButtonAttachment> oneShotAtt;
    std::unique_ptr<SliderAttachment> rrAtt;
};

struct RhythmGuiSet
{
    // --- Rhythm Page ---
    juce::Component page;

    ColoredGroupComponent group;

    // Master Level
    juce::Slider levelSlider;
    juce::Label levelLabel;
    std::unique_ptr<SliderAttachment> levelAtt;

    // 8 Pads
    std::array<RhythmPadGui, 8> pads;
};

struct AdpcmGuiSet
{
    // --- ADPCM Page ---
    juce::Component page;
    ColoredGroupComponent group;

    juce::ComboBox modeCombo;
    juce::Label modeLabel;

    juce::TextButton loadButton;
    juce::TextButton clearButton;
    juce::Label fileNameLabel;

    juce::Slider levelSlider;
    juce::Label levelLabel;

    juce::Slider panSlider;
    juce::Label panLabel;
    juce::TextButton btnPanL, btnPanC, btnPanR;

    // Loop Button
    juce::ToggleButton loopButton;
    juce::Label loopLabel;

    juce::Slider attackSlider;
    juce::Label attackLabel;

    juce::Slider decaySlider;
    juce::Label decayLabel;

    juce::Slider sustainSlider;
    juce::Label sustainLabel;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    // ADPCM Attachments
    std::unique_ptr<ComboBoxAttachment> modeAtt;
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<SliderAttachment> panAtt;
    std::unique_ptr<ButtonAttachment> loopAtt;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
};
#endif

struct FxGuiSet
{
    juce::Component page;
    ColoredGroupComponent globalGroup;
    ColoredGroupComponent tremGroup;
    ColoredGroupComponent vibGroup;
    ColoredGroupComponent mbcGroup;
    ColoredGroupComponent delayGroup;
    ColoredGroupComponent reverbGroup;
    ColoredGroupComponent rbcGroup;

    juce::ToggleButton bypassToggle;

    // Tremolo
    juce::ToggleButton tBypassBtn;
    juce::Label tRateLabel, tDepthLabel, tMixLabel;
    juce::Slider tRateSlider, tDepthSlider, tMixSlider;
    juce::TextButton tDryBtn, tHalfBtn, tWetBtn;

    // Vibrato
    juce::ToggleButton vBypassBtn;
    juce::Label vRateLabel, vDepthLabel, vMixLabel;
    juce::Slider vRateSlider, vDepthSlider, vMixSlider;
    juce::TextButton vDryBtn, vHalfBtn, vWetBtn;

    // Modern Bit Crusher
    juce::ToggleButton mbcBypassBtn;
    juce::Label mbcRateLabel, mbcBitsLabel, mbcMixLabel;
    juce::Slider mbcRateSlider, mbcBitsSlider, mbcMixSlider;
    juce::TextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // Delay
    juce::ToggleButton dBypassBtn;
    juce::Label dTimeLabel, dFbLabel, dMixLabel;
    juce::Slider dTimeSlider, dFbSlider, dMixSlider;
    juce::TextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    juce::ToggleButton rBypassBtn;
    juce::Label rSizeLabel, rDampLabel, rMixLabel;
    juce::Slider rSizeSlider, rDampSlider, rMixSlider;
    juce::TextButton rDryBtn, rHalfBtn, rWetBtn;

    // Retro Bit Crusher
    juce::ToggleButton rbcBypassBtn;
    juce::Label rbcRateLabel, rbcBitsLabel, rbcMixLabel;
    juce::ComboBox rbcRateCombo, rbcBitsCombo;
    juce::Slider rbcMixSlider;
    juce::TextButton rbcDryBtn, rbcHalfBtn, rbcWetBtn;

    // Attachments
    std::unique_ptr<ButtonAttachment> fxBypassAtt;
    std::unique_ptr<SliderAttachment> tRateAtt, tDepthAtt, tMixAtt;
    std::unique_ptr<SliderAttachment> vRateAtt, vDepthAtt, vMixAtt;
    std::unique_ptr<SliderAttachment> mbcRateAtt, mbcBitsAtt, mbcMixAtt;
    std::unique_ptr<SliderAttachment> dTimeAtt, dFbAtt, dMixAtt;
    std::unique_ptr<SliderAttachment> rSizeAtt, rDampAtt, rMixAtt;
    std::unique_ptr<ComboBoxAttachment> rbcRateAtt, rbcBitsAtt;
    std::unique_ptr<SliderAttachment> rbcMixAtt;
    std::unique_ptr<ButtonAttachment> tBypassAtt, vBypassAtt, mbcBypassAtt, dBypassAtt, rBypassAtt, rbcBypassAtt;
};

#if !defined(BUILD_AS_FX_PLUGIN)
// プリセット1つ分の情報を保持する構造体
struct PresetItem
{
    juce::File file;
    juce::String fileName;
    juce::String name;
    juce::String author;
    juce::String version;
    juce::String comment;
    juce::String modeName;
};

// PresetGuiSetをTableListBoxModel継承に変更
struct PresetGuiSet : public juce::TableListBoxModel
{
    juce::Component page;

    // UI Components
    juce::Label pathLabel; // パス表示用
    juce::TableListBox table; // メタデータ付きリスト

    // Metadata Editors
    ColoredGroupComponent metaGroup;
    juce::Label nameLabel, authorLabel, versionLabel;
    juce::TextEditor nameEditor, authorEditor, versionEditor;
    juce::Label commentLabel;
    juce::TextEditor commentEditor;

    // Buttons
    juce::TextButton initButton;
    juce::TextButton saveButton;
    juce::TextButton loadButton;
    juce::TextButton deleteButton;
    juce::TextButton refreshButton;
    juce::TextButton reflectButton; // Reflect Info
    juce::TextButton copyButton;    // Copy Info to Clipboard

    // Data
    juce::File currentFolder;
    std::vector<PresetItem> items; // 読み込んだプリセット一覧

    // Callback for Editor
    std::function<void(const juce::File&)> onDoubleClicked;

    PresetGuiSet()
    {
        // テーブルのモデルを自分自身に設定
        table.setModel(this);
    }

    // --- TableListBoxModel Overrides ---
    int getNumRows() override { return (int)items.size(); }

    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected) g.fillAll(juce::Colours::lightblue.withAlpha(0.5f));
        else if (rowNumber % 2) g.fillAll(juce::Colours::white.withAlpha(0.1f)); // Stripe
    }

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
        if (rowNumber >= items.size()) return;

        juce::String text;
        const auto& item = items[rowNumber];

        switch (columnId) {
            case 1: text = item.fileName; break;
            case 2: text = item.name; break;
            case 3: text = item.author; break;
            case 4: text = item.version; break;
            case 5: text = item.modeName; break;
        }

        if (rowIsSelected)
        {
            // 選択時は背景が青なので、文字は白
            g.setColour(juce::Colours::white);
        }
        else
        {
            // 通常時は黒 (または背景に合わせて適宜変更)
            g.setColour(juce::Colours::lightgrey);
            // もしダークモードなら juce::Colours::white または lightgrey にしてください
        }

        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override
    {
        if (rowNumber < items.size() && onDoubleClicked) {
            onDoubleClicked(items[rowNumber].file);
        }
    }

    // 選択されたファイルを取得するヘルパー
    juce::File getSelectedFile() const
    {
        int row = table.getSelectedRow();
        if (row >= 0 && row < items.size()) {
            return items[row].file;
        }
        return {};
    }

    // 選択行が変更されたときに呼ばれる
    void selectedRowsChanged(int lastRowSelected) override
    {
        // 選択されている行が1つ以上あるかチェック
        bool hasSelection = table.getNumSelectedRows() > 0;

        // ボタンの有効/無効を切り替え
        deleteButton.setEnabled(hasSelection);
        reflectButton.setEnabled(hasSelection);
        copyButton.setEnabled(hasSelection);
        loadButton.setEnabled(hasSelection);
    }
};
#endif

struct SettingsGuiSet
{
    juce::Component page;
    ColoredGroupComponent group;

    // Wallpaper
    juce::Label wallpaperLabel;
    juce::Label wallpaperPathLabel;
    juce::TextButton wallpaperBrowseBtn;
    juce::TextButton wallpaperClearBtn;

    // Directories
    juce::Label sampleDirLabel;
    juce::Label sampleDirPathLabel;
    juce::TextButton sampleDirBrowseBtn;

    juce::Label presetDirLabel;
    juce::Label presetDirPathLabel;
    juce::TextButton presetDirBrowseBtn;

    // Global Settings I/O
    juce::TextButton saveSettingsBtn;
    juce::TextButton loadSettingsBtn;
    juce::TextButton saveStartupSettingsBtn;

    // Tooltip Visible Switch
    juce::Label tooltipLabel;
    juce::ToggleButton tooltipToggle;

    // For Headroom
    juce::Label useHeadroomLabel;
    juce::ToggleButton useHeadroomToggle;
    juce::Slider headroomGainSlider;
};

struct AboutGuiSet
{
    juce::Component page;

    // Labels
    juce::Label pluginNameLabel;
    juce::Label versionLabel;
    juce::Label copyrightLabel;

    // VST Guideline
    juce::Label vstGuidelineLabel;

    // Logos
    juce::ImageComponent vst3Logo;

    // GPL Licese View
    juce::Label gplNoticeLabel;
    juce::HyperlinkButton gplLinkButton;
};

struct SetupGroupParams
{
    juce::Component& page;
    ColoredGroupComponent& group;
    const juce::String title;
    juce::Colour color = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.4f);
};

struct SetupLabelParams
{
    juce::Component& page;
    juce::Component& parent;
    juce::Label& label;
    const juce::String title;
    int width;
    int height;

    static SetupLabelParams create(
        juce::Component& p,
        juce::Component& par,
        juce::Label& l,
        const juce::String t, 
        int width = LabelWidth,
        int height = LabelHeight
    )
    {
		return SetupLabelParams(p, par, l, t, width, height);
    }

#if !defined(BUILD_AS_FX_PLUGIN)
    static SetupLabelParams createOp(
        juce::Component& p,
        juce::Component& par,
        juce::Label& l,
        const juce::String t,
        int width = OpLabelWidth,
        int height = OpLabelHeight
    )
    {
        return SetupLabelParams(p, par, l, t, width, height);
    }
#endif
};

struct SetupSliderParams
{
    juce::Component& parent;
    juce::Slider& slider;
    juce::Label& label;
    std::unique_ptr<SliderAttachment>& attr;
    const juce::String id;
    const juce::String title;
    int width;
    int height;
    int valueWidth;
    int valueHeight;
    int labelWidth;
    int labelHeight;
    bool isReset;

    RegisterType regType = RegisterType::None;

    static SetupSliderParams create(
        juce::Component& p,
        juce::Slider& s,
        juce::Label& l,
        std::unique_ptr<SliderAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = SliderSize,
        Size valueSize = SliderValueSize,
        Size labelSize = LabelSize,
        Flags flags = SliderFlags,
        RegisterType r = RegisterType::None
    )
    {
        SetupSliderParams prm = SetupSliderParams(p, s, l, a, i, t, size.width, size.height, valueSize.width, valueSize.height, labelSize.width, labelSize.height, flags.isReset);

        prm.regType = r;

        return prm;
    }

#if !defined(BUILD_AS_FX_PLUGIN)
    static SetupSliderParams createOp(
        juce::Component& p,
        juce::Slider& s,
        juce::Label& l,
        std::unique_ptr<SliderAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = OpSliderSize,
        Size valueSize = OpSliderValueSize,
        Size labelSize = OpLabelSize,
        Flags flags = OpSliderFlags,
        RegisterType r = RegisterType::None
    )
    {
        SetupSliderParams prm = SetupSliderParams(p, s, l, a, i, t, size.width, size.height, valueSize.width, valueSize.height, labelSize.width, labelSize.height, flags.isReset);

        prm.regType = r;

        return prm;
    }
#endif
};

struct SetupComboParams
{
    juce::Component& parent;
    juce::ComboBox& combo;
    juce::Label& label;
    std::unique_ptr<ComboBoxAttachment>& attr;
    const juce::String id;
    const juce::String title;
    std::vector<SelectItem>& items;
    int width;
    int height;
    int labelWidth;
    int labelHeight;
    bool isReset;
    bool isResize;

    static SetupComboParams create(
        juce::Component& p,
        juce::ComboBox& c,
        juce::Label& l,
        std::unique_ptr<ComboBoxAttachment>& a,
        const juce::String i,
        const juce::String t,
        std::vector<SelectItem>& items,
        Size size = ComboBoxSize,
        Size labelSize = LabelSize,
        Flags flags = ComboBoxFlags
    )
    {
        return SetupComboParams(p, c, l, a, i, t, items, size.width, size.height, labelSize.width, labelSize.height, flags.isReset, flags.isResize);
    }

#if !defined(BUILD_AS_FX_PLUGIN)
    static SetupComboParams createOp(
        juce::Component& p,
        juce::ComboBox& c,
        juce::Label& l,
        std::unique_ptr<ComboBoxAttachment>& a,
        const juce::String i,
        const juce::String t,
        std::vector<SelectItem>& items,
        Size size = OpComboBoxSize,
        Size labelSize = OpLabelSize,
        Flags flags = OpComboBoxFlags
    )
    {
        return SetupComboParams(p, c, l, a, i, t, items, size.width, size.height, labelSize.width, labelSize.height, flags.isReset, flags.isResize);
    }
#endif
};

struct SetupToggleButtonParams
{
    juce::Component& parent;
    juce::ToggleButton& btn;
    juce::Label& label;
    std::unique_ptr<ButtonAttachment>& attr;
    const juce::String id;
    const juce::String title;
    int width;
    int height;
    bool isReset;
    bool isResize;

    static SetupToggleButtonParams create(
        juce::Component& p,
        juce::ToggleButton& b,
        juce::Label& l,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = ToggleButtonSize,
        Flags flags = ToggleButtonFlags
    )
    {
        return SetupToggleButtonParams(p, b, l, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }

#if !defined(BUILD_AS_FX_PLUGIN)
    static SetupToggleButtonParams createOp(
        juce::Component& p,
        juce::ToggleButton& b,
        juce::Label& l,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = OpToggleButtonSize,
        Flags flags = OpToggleButtonFlags
    )
    {
        return SetupToggleButtonParams(p, b, l, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }
#endif
};

struct SetupTextButtonParams
{
    juce::Component& parent;
    juce::TextButton& btn;
    std::unique_ptr<ButtonAttachment>& attr;
    const juce::String id;
    const juce::String title;
    int width;
    int height;
    bool isReset;
    bool isResize;

    static SetupTextButtonParams create(
        juce::Component& p,
        juce::TextButton& b,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = TextButtonSize,
        Flags flags = TextButtonFlags
    )
    {
        return SetupTextButtonParams(p, b, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }

#if !defined(BUILD_AS_FX_PLUGIN)
    static SetupTextButtonParams createOp(
        juce::Component& p,
        juce::TextButton& b,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = OpTextButtonSize,
        Flags flags = OpTextButtonFlags
    )
    {
        return SetupTextButtonParams(p, b, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }
#endif
};

template <size_t tableSize>
struct SetupCustomWaveTableParams
{
    juce::Component& page;
    std::array<juce::Slider, tableSize>& wts;
    std::array<std::unique_ptr<SliderAttachment>, tableSize>& attrs;
    const juce::String idPrefix;
};

template <size_t opCount>
struct SetupOpGroupsParams
{
    juce::Component& page;
    std::array<ColoredGroupComponent, opCount>& groups;
};

class AudioPlugin2686VEditor :
    public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public juce::ComponentListener,
    public juce::Button::Listener,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    AudioPlugin2686VEditor(AudioPlugin2686V&);
    ~AudioPlugin2686VEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
    void buttonClicked(juce::Button* button) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void showRegisterInput(juce::Component* targetComp, std::function<void(int)> onValueEntered);
#if !defined(BUILD_AS_FX_PLUGIN)
    template <typename T>
    void applyMmlString(const juce::String& mml, T& gui, int opIndex);
#endif
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void updateRhythmFileNames();
private:
    AudioPlugin2686V& audioProcessor;

    // グローバルフッター
    ColoredGroupComponent footerGroup;
    // マスターボリューム用のスライダーとアタッチメント
    juce::Slider masterVolSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolAttachment;
    juce::Label masterVolLabel;

    CustomTabLookAndFeel customTabLF;

    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
	juce::Label logoLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;
#if !defined(BUILD_AS_FX_PLUGIN)
    std::map<juce::Component*, RegisterType> sliderRegMap;
#endif
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

#if !defined(BUILD_AS_FX_PLUGIN)
    Fm4GuiSet opnaGui;  // OPNA
    Fm4GuiSet opnGui; // OPN
	Fm2GuiSet oplGui; // OPL
    Opl3GuiSet opl3Gui; // OPL3
    OpmGuiSet opmGui; // OPM
    Opzx3GuiSet opzx3Gui; // OPZX3
    SsgGuiSet ssgGui; // SSG
	WtGuiSet wtGui; // Wavetable
	RhythmGuiSet rhythmGui; // Rhythm
	AdpcmGuiSet adpcmGui; // ADPCM
    PresetGuiSet presetGui;
#endif
    FxGuiSet fxGui; // FX
    SettingsGuiSet settingsGui;
    AboutGuiSet aboutGui;

    juce::Image backgroundImage; // Cache for wallpaper

    // Helper
    // Draw background
    void drawBg(juce::Graphics& g);
    std::vector<SelectItem> createItems(int size, const juce::String& prefix = "");
    std::vector<SelectItem> createAlgItems(int size);
    void attatchLabelToComponent(juce::Label& label, juce::Component& component);
    void setupLogo();
    void setupTabs(juce::TabbedComponent& tabs);
    void setupGroup(SetupGroupParams& params);
	void setupLabel(SetupLabelParams& params);
	void setupSlider(SetupSliderParams& params);
	void setupCombo(SetupComboParams& params);
	void setupToggleButton(SetupToggleButtonParams& params);
	void setupTextButton(SetupTextButtonParams& params);
	void setupFbSlider(SetupSliderParams& params);
	template <size_t tableSize>
	void setupCustomWaveTable(SetupCustomWaveTableParams<tableSize>& params);
    template <size_t opCount>
    void setupOpGroups(SetupOpGroupsParams<opCount>& params);

#if !defined(BUILD_AS_FX_PLUGIN)
    void setupOpnaGui(Fm4GuiSet& gui);
    void setupOpnGui(Fm4GuiSet& gui);
    void setupOplGui(Fm2GuiSet& gui);
    void setupOpl3Gui(Opl3GuiSet& gui);
    void setupOpmGui(OpmGuiSet& gui);
    void setupOpzx3Gui(Opzx3GuiSet& gui);
    void setupSsgGui(SsgGuiSet& gui);
    void setupWtGui(WtGuiSet& gui);
    void setupRhythmGui(RhythmGuiSet& gui);
    void setupAdpcmGui(AdpcmGuiSet& gui);
    void setupPresetGui(PresetGuiSet& gui);
#endif
    void setupFxGui(FxGuiSet& gui);
    void setupSettingsGui(SettingsGuiSet& gui);
    void setupAboutGui(AboutGuiSet& gui);

#if !defined(BUILD_AS_FX_PLUGIN)
    void layoutOpnaPage(Fm4GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpnPage(Fm4GuiSet& gui, juce::Rectangle<int> content);
    void layoutOplPage(Fm2GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpl3Page(Opl3GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpmPage(OpmGuiSet& gui, juce::Rectangle<int> content);
    void layoutOpzx3Page(Opzx3GuiSet& gui, juce::Rectangle<int> content);
    void layoutSsgPage(SsgGuiSet& gui, juce::Rectangle<int> content);
    void layoutWtPage(WtGuiSet& gui, juce::Rectangle<int> content);
    void layoutRhythmPage(RhythmGuiSet& gui, juce::Rectangle<int> content);
    void layoutAdpcmPage(AdpcmGuiSet& gui, juce::Rectangle<int> content);
    void layoutPresetPage(PresetGuiSet& gui, juce::Rectangle<int> content);
#endif
    void layoutFxPage(FxGuiSet& gui, juce::Rectangle<int> content);
    void layoutSettingsPage(SettingsGuiSet& gui, juce::Rectangle<int> content);
    void layoutAboutPage(AboutGuiSet& gui, juce::Rectangle<int> content);

#if !defined(BUILD_AS_FX_PLUGIN)
    void loadPresetFile(const juce::File& file);
    void saveCurrentPreset();
    void scanPresets();
#endif
    void loadWallpaperImage();
#if !defined(BUILD_AS_FX_PLUGIN)
    void setTooltipState(bool enabled);
    void assignTooltipsRecursive(juce::Component* parentComponent);
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
