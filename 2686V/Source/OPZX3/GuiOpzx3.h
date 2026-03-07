#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../core/GuiValues.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpzx3 : public GuiBase
{
    /*
     * アルゴリズムのオペレータ表記凡例
     * 2026.3.7 CYROSS
     *
     * [C] : キャリアー(出力はオーディオ出力)
     * [M->n] : n番オペレータへ出力するモジュレーター
     * [C:FB] : 自身へフィードバックもするキャリアー
     * [M:FB->n] : 自身へフィードバックもする、n番オペレーターへ出力するモジュレーター
     * [C:FBm] : m番オペレータへフィードバックもするキャリア―
     * [M:FBm->n] : m番オペレータへフィードバックもする、n番オペレーターへ出力するモジュレーター
     * /を挟んでnが複数ある場合: それぞれのオペレータに出力する
     * 複数のnが存在する場合 : 各オペレーターからの出力を足し合わせて、n番のオペレータへ出力
     * -- : 未使用
     */
    static inline const std::array<std::array<juce::String, 4>, 36> algOpPrefix = { {
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}},       // 00: <OPX-00>
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([C])"}},      // 01: <OPX-01>
        {{"([M:FB->2])", "([M])", "([M->4])", "([C])"}},          // 02: <OPX-02>
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])"}},       // 03: <OPX-03>
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}},       // 04: <OPX-04>
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},         // 05: <OPX-05>
        {{"([M->2/3/4])", "([C:FB1])", "([C])", "([C])"}},        // 06: <OPX-06>
        {{"([M->2])", "([C])", "([M->4])", "([C])"}},             // 07: <OPX-07>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C])"}},         // 08: <OPX-08>
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])"}},          // 09: <OPX-09>
        {{"([C:FB])", "([M->3])", "([C])", "([C])"}},             // 10: <OPX-10>
        {{"([M:FB->2])", "([C])", "([C])", "([C])"}},             // 11: <OPX-11>
        {{"([M->2])", "([C:FB1])", "([C])", "([C])"}},            // 12: <OPX-12>
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},         // 13: <OPX-13>
        {{"([C:FB][M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 14: <OPX-14>
        {{"([C:FB])", "([C])", "([C])", "([C])"}},                // 15: <OPX-15>
        {{"([C:FB])", "([C])", "([C])", "(--)"}},                 // 16: <OPX-16>
        {{"([M->2])", "([M:FB1->3])", "([C])", "(--)"}},          // 17: <OPX-17>
        {{"([M:FB->3])", "([C])", "([M->2])", "(--)"}},           // 18: <OPX-18>
        {{"([M:FB->2])", "([C])", "([M->2])", "(--)"}},           // 19: <OPX-19>
        {{"([M:FB->3])", "([C])", "([C])", "(--)"}},              // 20: <OPX-20>
        {{"([M:FB->2])", "([M:FB->3])", "([C])", "(--)"}},        // 21: <OPX-21>
        {{"([C:FB])", "([C][M->3])", "([C])", "(--)"}},           // 22: <OPX-22>
        {{"([C:FB][M:FB->3])", "([C])", "([C])", "(--)"}},        // 23: <OPX-23>
        {{"([M:FB->2])", "([C])", "(--)", "(--)"}},               // 24: <OPX-24>
        {{"([M->2])", "([C:FB1])", "(--)", "(--)"}},              // 25: <OPX-25>
        {{"([C:FB])", "([C])", "([--])", "([--])"}},              // 26: <OPX-26>
        {{"([C:FB][M->2])", "([C])", "([--])", "([--])"}},        // 27: <OPX-27>
        {{"([M:FB->2])", "([C])", "([--])", "([--])"}},           // 28: <MA3-00>
        {{"([C:FB])", "([C])", "([--])", "([--])"}},              // 29: <MA3-01>
        {{"([C:FB])", "([C])", "([C:FB])", "([C])"}},             // 30: <MA3-02>
        {{"([C:FB][M:FB->4])", "([M->3])", "([M->4])", "([C])"}}, // 31: <MA3-03>
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}},       // 32: <MA3-04>
        {{"([M:FB->2])", "([C])", "([M:FB->4])", "([C])"}},       // 33: <MA3-05>
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])"}},          // 34: <MA3-06>
        {{"([C:FB])", "([M->3])", "([C])", "([C])"}}              // 35: <MA3-07>
    }};

    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    // Global
    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiFbSlider feedback2Slider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    GuiCategoryLabel lfoCat;

    // LFO
    GuiSlider lfoFreqSlider;
    GuiSlider lfoAmSmRtSlider;
    GuiComboBox lfoShapeSelector;
    GuiToggleButton lfoPmToggle;
    GuiToggleButton lfoAmToggle;
    GuiComboBox lfoPmsSelector;
    GuiComboBox lfoAmsSelector;
    GuiSlider lfoPmdSlider;
    GuiSlider lfoAmdSlider;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMain;
    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiComboBox, Global::Fm::Op4> dt1;
    std::array<GuiSlider, Global::Fm::Op4> dt2;
    std::array<GuiSlider, Global::Fm::Op4> tl;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiSlider, Global::Fm::Op4> d1r;
    std::array<GuiSlider, Global::Fm::Op4> d1l;
    std::array<GuiSlider, Global::Fm::Op4> d2r;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiComboBox, Global::Fm::Op4> ks;
    std::array<GuiCategoryLabel, Global::Fm::Op4> cafFix;
    std::array<GuiToggleButton, Global::Fm::Op4> fix;
    std::array<GuiSlider, Global::Fm::Op4> freq;
    std::array<GuiTextButton, Global::Fm::Op4> freqToZero;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo05;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo1;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo2;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo440;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catShape;
    std::array<GuiComboBox, Global::Fm::Op4> ws;
    std::array<GuiTextButton, Global::Fm::Op4> loadPcmBtn;
    std::array<GuiTextButton, Global::Fm::Op4> clearPcmBtn;
    std::array<GuiLabel, Global::Fm::Op4> pcmFileNameLabel;
    std::array<GuiSlider, Global::Fm::Op4> pcmOffset;
    std::array<GuiSlider, Global::Fm::Op4> pcmRatio;
    std::array<GuiComboBox, Global::Fm::Op4> se;
    std::array<GuiSlider, Global::Fm::Op4> seFreq;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catLfo;
    std::array<GuiToggleButton, Global::Fm::Op4> pm;  // OPLの vib に相当)
    std::array<GuiComboBox, Global::Fm::Op4> pms;
    std::array<GuiToggleButton, Global::Fm::Op4> am;  // OPMでは AMS-EN に相当)
    std::array<GuiComboBox, Global::Fm::Op4> ams;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMask;
    std::array<GuiToggleButton, Global::Fm::Op4> mask; // Mask
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMml;
    std::array<GuiMmlButton, Global::Fm::Op4> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
	GuiOpzx3(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        qualityCat(context),
        algFbCat(context),
        algSelector(context),
        feedbackSlider(context),
        feedback2Slider(context),
        bitSelector(context),
        rateSelector(context),
        lfoCat(context),
        lfoFreqSlider(context),
        lfoAmSmRtSlider(context),
        lfoShapeSelector(context),
        lfoPmToggle(context),
        lfoAmToggle(context),
        lfoPmsSelector(context),
        lfoAmsSelector(context),
        lfoPmdSlider(context),
        lfoAmdSlider(context),
        mvolCat(context),
        masterVolSlider(context),
        catMain{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt1{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        dt2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        cafFix{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        fix{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        freq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        freqToZero{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo05{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo1{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo2{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo440{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ws{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        loadPcmBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        clearPcmBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        pcmFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
        pcmOffset{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pcmRatio{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        pm{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        pms{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ams{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMml{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) }
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePcmFileName(int opIndex, const juce::String& fileName) {
        pcmFileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateAllPcmFileName(const juce::String& fileName) {
        for (int i = 0; i < 4; i++)
        {
            pcmFileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();
};
