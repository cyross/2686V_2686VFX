#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Gui/Components/PitchButtons/PitchButtons.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ViewMode/ViewMode.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/NudgeButtons/NudgeButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpzx7 : public GuiBase
{
    GuiComponentViewModes viewMode = GuiComponentViewModes::Twin;

    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;
    GuiComponentViewMode viewModeComp;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiCategoryLabel algFbCat;

    // AlgMatrix
    GuiComboBox algModeSelector;
    GuiFmAlgMatrix algMatrixComp;
    GuiFmAlgGraph algGraphComp;

    // 従来のアルゴリズム図用のグラフコンポーネント (画像から置き換え)
    GuiFmAlgGraph algStaticGraphComp;

    // Global
    GuiComboBox algSelector;
    NormalSeparator algFbSep;
    GuiComponentNudgeSliderFloat feedback1Slider;
    GuiComponentNudgeButtons feedback1Nudge;
    GuiComponentNudgeSliderFloat feedback2Slider;
    GuiComponentNudgeButtons feedback2Nudge;
    GuiComponentNudgeSliderFloat feedback3Slider;
    GuiComponentNudgeButtons feedback3Nudge;
    GuiComponentNudgeSliderFloat feedback4Slider;
    GuiComponentNudgeButtons feedback4Nudge;
    GuiComponentNudgeSliderFloat feedback5Slider;
    GuiComponentNudgeButtons feedback5Nudge;
    GuiComponentNudgeSliderFloat feedback6Slider;
    GuiComponentNudgeButtons feedback6Nudge;
    GuiComponentNudgeSliderFloat feedback7Slider;
    GuiComponentNudgeButtons feedback7Nudge;
    GuiComponentNudgeSliderFloat feedback8Slider;
    GuiComponentNudgeButtons feedback8Nudge;

    GuiCategoryLabel panCat;
    GuiToggleButton panpotEnableToggle;
    GuiSlider panpotSlider;
    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    // SSG Hw Env
    // チップ全体へ掛かる AMP ENV
    GuiComponentAmpEnv ampEnvComponent;
    GuiComponentSsgHwEnv ssgHwEnv;
    // SSG Sw Env
    GuiComponentSsgSwEnv11 ssgSwEnv11g;

    // LFO
    GuiComponentLfoOpzx7 glLfo;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    NormalSeparator uSep002;
    GuiComponentImportExport ieOpLfo;
    GuiComponentImportExport ieOpDetune;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv;
    GuiComponentImportExport ieOpSsgSwEnv11;
    GuiComponentImportExport ieOpSsgSwPEnv11;
    GuiComponentImportExport ieOpPcmPlay;
    GuiComponentImportExport ieOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep003;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::array<GuiScrollGroup, Opzx7PrValue::ops> opGroups;
    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiComponentMulDetune, Opzx7PrValue::ops> mulDetune;
    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catAmp;
    std::array<GuiSlider, Opzx7PrValue::ops> tl;
    std::array<GuiSlider, Opzx7PrValue::ops> ar;
    std::array<GuiSlider, Opzx7PrValue::ops> d1r;
    std::array<GuiSlider, Opzx7PrValue::ops> d1l;
    std::array<GuiSlider, Opzx7PrValue::ops> d2r;
    std::array<GuiSlider, Opzx7PrValue::ops> rr;

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> ksCat;
    std::array<GuiToggleButton, Opzx7PrValue::ops> ksEn;
    std::array<GuiComboBox, Opzx7PrValue::ops> ksMode;
    std::array<GuiToggleButton, Opzx7PrValue::ops> ksrMA7;
    std::array<GuiComboBox, Opzx7PrValue::ops> kslMA7;
    std::array<GuiSlider, Opzx7PrValue::ops> ksrOPZ;
    std::array<GuiSlider, Opzx7PrValue::ops> kslOPZ;
    std::array<GuiSlider, Opzx7PrValue::ops> ksBp;
    std::array<GuiComboBox, Opzx7PrValue::ops> ksLc;
    std::array<GuiComboBox, Opzx7PrValue::ops> ksRc;
    std::array<GuiSlider, Opzx7PrValue::ops> ksLd;
    std::array<GuiSlider, Opzx7PrValue::ops> ksRd;
    std::array<GuiSlider, Opzx7PrValue::ops> ksRs;

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catOptional;
    std::array<GuiToggleButton, Opzx7PrValue::ops> bypass;
    std::array<GuiComponentFix, Opzx7PrValue::ops> fix;
    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catWaveShape;
    std::array<GuiComboBox, Opzx7PrValue::ops> ws;
    std::array<NormalSeparator, Opzx7PrValue::ops> wsSeparator;
    std::array<GuiTextButton, Opzx7PrValue::ops> loadPcmBtn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearPcmBtn;
    std::array<GuiLabel, Opzx7PrValue::ops> pcmFileNameLabel;
    std::array<GuiSlider, Opzx7PrValue::ops> pcmOffset;
    std::array<GuiSlider, Opzx7PrValue::ops> pcmRatio;
    std::array<GuiToggleButton, Opzx7PrValue::ops> loopPointEnable;
    std::array<GuiSlider, Opzx7PrValue::ops> loopPointStart;
    std::array<GuiSlider, Opzx7PrValue::ops> loopPointEnd;
    std::array<GuiTextButton, Opzx7PrValue::ops> loadWtBtn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearWtBtn;
    std::array<GuiLabel, Opzx7PrValue::ops> wtFileNameLabel;
    std::array<GuiTextButton, Opzx7PrValue::ops> loadWt2Btn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearWt2Btn;
    std::array<GuiLabel, Opzx7PrValue::ops> wt2FileNameLabel;
    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catSsgEnv;
    std::array<GuiComboBox, Opzx7PrValue::ops> se;
    std::array<GuiSlider, Opzx7PrValue::ops> seFreq;

    // LFO
    std::array<GuiComponentLfoOpzx7, Opzx7PrValue::ops> lfo;

    // Pitch ADSR
    std::array<GuiComponentPitchEnv, Opzx7PrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, Opzx7PrValue::ops> ssgSwEnv;

    std::array<GuiComponentSsgSwEnv11, Opzx7PrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, Opzx7PrValue::ops> ssgSwPEnv11;

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catMask;
    std::array<GuiToggleButton, Opzx7PrValue::ops> mask; // Mask
    std::array<NormalSeparator, Opzx7PrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, Opzx7PrValue::ops> mml;

    std::array<GuiToggleButton, Opzx7PrValue::ops> rgEn;
    std::array<NormalSeparator, Opzx7PrValue::ops> rgSeparator;
    std::array<GuiSlider, Opzx7PrValue::ops> rgAr;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD1r;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD2r;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD1l;
    std::array<GuiSlider, Opzx7PrValue::ops> rgRr;
    std::array<GuiSlider, Opzx7PrValue::ops> rgTl;

    std::array<NormalSeparator, Opzx7PrValue::ops> optionalSeparator;
    std::array<GuiToggleButton, Opzx7PrValue::ops> sus; // Sus
    std::array<GuiToggleButton, Opzx7PrValue::ops> xof; // Xof
    std::array<GuiToggleButton, Opzx7PrValue::ops> kor;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, Opzx7PrValue::ops> opGraphs;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, Opzx7PrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, Opzx7PrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpzx7(const GuiContext& context);

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePcmFileName(int opIndex, const juce::String& fileName) {
        pcmFileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateWtFileName(int opIndex, const juce::String& fileName) {
        wtFileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateWt2FileName(int opIndex, const juce::String& fileName) {
        wt2FileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateAllPcmFileName(const juce::String& fileName) {
        for (int i = 0; i < Opzx7PrValue::ops; i++)
        {
            pcmFileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateAllWtFileName(const juce::String& fileName) {
        for (int i = 0; i < Opzx7PrValue::ops; i++)
        {
            wtFileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateAllWt2FileName(const juce::String& fileName) {
        for (int i = 0; i < Opzx7PrValue::ops; i++)
        {
            wt2FileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateOpVisible(int idx, bool visible);
    void updateOpEnable(int idx, bool enable);
    void updateOnWsChange(int idx);
    void updateAlgorithmDisplay();
    void updateAlgorithmMatrixDisplay();
    void updateRgDisplayAsOp(int idx, bool rgMode);
    void updatePresetName(const juce::String& name);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOp(int opIndex, int width, juce::Rectangle<int>& rect);
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanpotCat(juce::Rectangle<int>& rect);
    void layoutOpSsgEnvCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode);
    void layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode);
    void layoutOpWsCat(int opIndex, juce::Rectangle<int>& rect, int selectedWs);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpzx7& copyObj);
    void copyOpParams(int p, CopyOpzx7Op& copyObj);
    void pasteParams(CopyOpzx7& copyObj);
    void pasteOpParams(int p, CopyOpzx7Op& copyObj);
    void importOpLfoParam(int opIndex);
    void exportOpLfoParam(int opIndex);
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnvParam(int opIndex);
    void exportSsgSwEnvParam(int opIndex);
    void importSsgSwEnv11Param(int opIndex);
    void exportSsgSwEnv11Param(int opIndex);
    void importSsgSwPEnv11Param(int opIndex);
    void exportSsgSwPEnv11Param(int opIndex);
    void importDetuneParam(int opIndex);
    void exportDetuneParam(int opIndex);
    void importQualityParam();
    void exportQualityParam();
    void importOpPcmPlayParam(int opIndex);
    void exportOpPcmPlayParam(int opIndex);
    void importChParam();
    void exportChParam();
    void importOpChParam(int opIndex);
    void exportOpChParam(int opIndex);
    void getImportingOpParams(int opIndex, juce::StringArray& lines, int& index);
    juce::String setExportedOpParams(int opIndex);
};
