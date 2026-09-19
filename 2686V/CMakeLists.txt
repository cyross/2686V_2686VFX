cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_2686V
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_2686V
    HEADER_NAME "AppIconForAbout.h"
    NAMESPACE AppIconAboutForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
)

# 定義
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

set(CONST_FILES
    "Source/Core/Const/ConstFileValues.h"
    "Source/Core/Const/ConstMmlKeys.h"
    "Source/Core/Const/ConstMmlValues.h"
)

set(SYNTH_FILES
    "Source/Core/Synth/SynthMode.h"
    "Source/Core/Synth/SynthParams.h"
    "Source/Core/Synth/SynthVoice.h"
    "Source/Core/Synth/SynthVoice.cpp"
)






set(FM_FILES
    "Source/Core/Fm/FmRegisterConverter.h"
    "Source/Core/Fm/FmRegisterConverter.cpp"
    "Source/Core/Fm/FmMmlFormatter.h"
)

set(GUI_FILES
    "Source/Core/Gui/GuiBase.h"
    "Source/Core/Io/ParamFile.h"
    "Source/Core/Io/ParamFile.cpp"
    "Source/Core/Gui/GuiRefresh.h"
    "Source/Core/Gui/GuiRefresh.cpp"
    "Source/Core/Gui/GuiColor.h"
    "Source/Core/Gui/GuiColorJuceNames.h"
    "Source/Core/Gui/GuiColor.cpp"
    "Source/Core/Gui/GuiI18n.h"
    "Source/Core/Gui/GuiI18n.cpp"
    "Source/Core/Gui/GuiComponents.h"
    "Source/Core/Gui/GuiComponents.cpp"
    "Source/Core/Gui/GuiContext.h"
    "Source/Core/Gui/GuiHelpers.h"
    "Source/Core/Gui/GuiHelpers.cpp"
    "Source/Core/Gui/GuiLF.h"
    "Source/Core/Gui/GuiLF.cpp"
    "Source/Core/Gui/GuiStructs.h"
    "Source/Core/Gui/GuiText.h"
    "Source/Core/Gui/GuiValues.h"
    "Source/Core/Gui/GuiEnvelopeGraph.h"
    "Source/Core/Gui/GuiGraphValues.h"
    "Source/Core/Gui/GuiEnvelopeGraph.cpp"
    "Source/Core/Gui/GuiCopyObj.h"
)

set(PROCESSOR_FILES
    "Source/Core/Processor/ProcessorKeys.h"
    "Source/Core/Processor/ProcessorNames.h"
    "Source/Core/Processor/ProcessorFloat.h"
    "Source/Core/Processor/ProcessorBase.h"
    "Source/Core/Processor/ProcessorStructs.h"
    "Source/Core/Processor/ProcessorHelper.h"
    "Source/Core/Processor/PluginProcessor.h"
    "Source/Core/Processor/PluginProcessor.cpp"
    "Source/Core/Processor/PluginProcessorStateKey.h"
)

set(EDITOR_FILES
    "Source/Core/Editor/EditorGuiText.h"
    "Source/Core/Editor/EditorGuiValues.h"
    "Source/Core/Editor/PluginEditor.h"
    "Source/Core/Editor/PluginEditor.cpp"
)

set(EFFECT_CORE_FILES
)







set(FX_EFFECT_FILES
    "Source/Effect/Fx/Fx.h"
    "Source/Effect/Fx/Fx.cpp"
)














set(OPNA_PROCESSOR_FILES
    "Source/Processor/Opna/ProcessorOpnaKeys.h"
    "Source/Processor/Opna/ProcessorOpnaNames.h"
    "Source/Processor/Opna/ProcessorOpna.h"
    "Source/Processor/Opna/ProcessorOpna.cpp"
)

set(OPN_PROCESSOR_FILES
    "Source/Processor/Opn/ProcessorOpnKeys.h"
    "Source/Processor/Opn/ProcessorOpnNames.h"
    "Source/Processor/Opn/ProcessorOpn.h"
    "Source/Processor/Opn/ProcessorOpn.cpp"
)

set(OPL_PROCESSOR_FILES
    "Source/Processor/Opl/ProcessorOplKeys.h"
    "Source/Processor/Opl/ProcessorOplNames.h"
    "Source/Processor/Opl/ProcessorOpl.h"
    "Source/Processor/Opl/ProcessorOpl.cpp"
)

set(OPL3_PROCESSOR_FILES
    "Source/Processor/Opl3/ProcessorOpl3Keys.h"
    "Source/Processor/Opl3/ProcessorOpl3Names.h"
    "Source/Processor/Opl3/ProcessorOpl3.h"
    "Source/Processor/Opl3/ProcessorOpl3.cpp"
)

set(OPM_PROCESSOR_FILES
    "Source/Processor/Opm/ProcessorOpmKeys.h"
    "Source/Processor/Opm/ProcessorOpmNames.h"
    "Source/Processor/Opm/ProcessorOpm.h"
    "Source/Processor/Opm/ProcessorOpm.cpp"
)

set(OPZX7_PROCESSOR_FILES
    "Source/Processor/Opzx7/ProcessorOpzx7Keys.h"
    "Source/Processor/Opzx7/ProcessorOpzx7Names.h"
    "Source/Processor/Opzx7/ProcessorOpzx7.h"
    "Source/Processor/Opzx7/ProcessorOpzx7.cpp"
)

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/ProcessorSsgKeys.h"
    "Source/Processor/Ssg/ProcessorSsgNames.h"
    "Source/Processor/Ssg/ProcessorSsgValues.h"
    "Source/Processor/Ssg/ProcessorSsg.h"
    "Source/Processor/Ssg/ProcessorSsg.cpp"
)

set(WT_PROCESSOR_FILES
    "Source/Processor/Wavetable/ProcessorWtKeys.h"
    "Source/Processor/Wavetable/ProcessorWtNames.h"
    "Source/Processor/Wavetable/ProcessorWtValues.h"
    "Source/Processor/Wavetable/ProcessorWt.h"
    "Source/Processor/Wavetable/ProcessorWt.cpp"
)

set(WT2_PROCESSOR_FILES
    "Source/Processor/Wt2/ProcessorWt2Keys.h"
    "Source/Processor/Wt2/ProcessorWt2Names.h"
    "Source/Processor/Wt2/ProcessorWt2Values.h"
    "Source/Processor/Wt2/ProcessorWt2.h"
    "Source/Processor/Wt2/ProcessorWt2.cpp"
)

set(RHYTHM_PROCESSOR_FILES
    "Source/Processor/Rhythm/ProcessorRhythmKeys.h"
    "Source/Processor/Rhythm/ProcessorRhythmNames.h"
    "Source/Processor/Rhythm/ProcessorRhythm.h"
    "Source/Processor/Rhythm/ProcessorRhythm.cpp"
)

set(ADPCM_PROCESSOR_FILES
    "Source/Processor/Adpcm/ProcessorAdpcmKeys.h"
    "Source/Processor/Adpcm/ProcessorAdpcmNames.h"
    "Source/Processor/Adpcm/ProcessorAdpcmValues.h"
    "Source/Processor/Adpcm/ProcessorAdpcm.h"
    "Source/Processor/Adpcm/ProcessorAdpcm.cpp"
)

set(BEEP_PROCESSOR_FILES
    "Source/Processor/Beep/ProcessorBeepKeys.h"
    "Source/Processor/Beep/ProcessorBeepNames.h"
    "Source/Processor/Beep/ProcessorBeepValues.h"
    "Source/Processor/Beep/ProcessorBeep.h"
    "Source/Processor/Beep/ProcessorBeep.cpp"
)

set(CURVE_PROCESSOR_FILES
    "Source/Processor/Curve/ProcessorCurveKeys.h"
    "Source/Processor/Curve/ProcessorCurveNames.h"
    "Source/Processor/Curve/ProcessorCurve.h"
    "Source/Processor/Curve/ProcessorCurve.cpp"
)

set(FX_PROCESSOR_FILES
    "Source/Processor/Fx/ProcessorFxKeys.h"
    "Source/Processor/Fx/ProcessorFxNames.h"
    "Source/Processor/Fx/ProcessorFxValues.h"
    "Source/Processor/Fx/ProcessorFx.h"
    "Source/Processor/Fx/ProcessorFx.cpp"
)

set(OPNA_GUI_FILES
    "Source/Gui/Opna/GuiOpnaText.h"
    "Source/Gui/Opna/GuiOpnaValues.h"
    "Source/Gui/Opna/GuiOpna.h"
    "Source/Gui/Opna/GuiOpna.cpp"
)

set(OPN_GUI_FILES
    "Source/Gui/Opn/GuiOpnText.h"
    "Source/Gui/Opn/GuiOpnValues.h"
    "Source/Gui/Opn/GuiOpn.h"
    "Source/Gui/Opn/GuiOpn.cpp"
)

set(OPL_GUI_FILES
    "Source/Gui/Opl/GuiOplText.h"
    "Source/Gui/Opl/GuiOplValues.h"
    "Source/Gui/Opl/GuiOpl.h"
    "Source/Gui/Opl/GuiOpl.cpp"
)

set(OPL3_GUI_FILES
    "Source/Gui/Opl3/GuiOpl3Text.h"
    "Source/Gui/Opl3/GuiOpl3Values.h"
    "Source/Gui/Opl3/GuiOpl3.h"
    "Source/Gui/Opl3/GuiOpl3.cpp"
)

set(OPM_GUI_FILES
    "Source/Gui/Opm/GuiOpmText.h"
    "Source/Gui/Opm/GuiOpmValues.h"
    "Source/Gui/Opm/GuiOpm.h"
    "Source/Gui/Opm/GuiOpm.cpp"
)

set(OPZX7_GUI_FILES
    "Source/Gui/Opzx7/GuiOpzx7Helpers.h"
    "Source/Gui/Opzx7/GuiOpzx7Helpers.cpp"
    "Source/Gui/Opzx7/GuiOpzx7Text.h"
    "Source/Gui/Opzx7/GuiOpzx7Values.h"
    "Source/Gui/Opzx7/GuiOpzx7.h"
    "Source/Gui/Opzx7/GuiOpzx7.cpp"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/GuiSsgText.h"
    "Source/Gui/Ssg/GuiSsgValues.h"
    "Source/Gui/Ssg/GuiSsg.h"
    "Source/Gui/Ssg/GuiSsg.cpp"
)

set(WT_GUI_FILES
    "Source/Gui/Wavetable/GuiWtHelpers.h"
    "Source/Gui/Wavetable/GuiWtHelpers.cpp"
    "Source/Gui/Wavetable/GuiWtText.h"
    "Source/Gui/Wavetable/GuiWtValues.h"
    "Source/Gui/Wavetable/GuiWt.h"
    "Source/Gui/Wavetable/GuiWt.cpp"
)

set(WT2_GUI_FILES
    "Source/Gui/Wt2/GuiWt2Helpers.h"
    "Source/Gui/Wt2/GuiWt2Helpers.cpp"
    "Source/Gui/Wt2/GuiWt2Text.h"
    "Source/Gui/Wt2/GuiWt2Values.h"
    "Source/Gui/Wt2/GuiWt2.h"
    "Source/Gui/Wt2/GuiWt2.cpp"
)

set(RHYTHM_GUI_FILES
    "Source/Gui/Rhythm/GuiRhythmHelpers.h"
    "Source/Gui/Rhythm/GuiRhythmHelpers.cpp"
    "Source/Gui/Rhythm/GuiRhythmText.h"
    "Source/Gui/Rhythm/GuiRhythmValues.h"
    "Source/Gui/Rhythm/GuiRhythm.h"
    "Source/Gui/Rhythm/GuiRhythm.cpp"
)

set(ADPCM_GUI_FILES
    "Source/Gui/Adpcm/GuiAdpcmHelpers.h"
    "Source/Gui/Adpcm/GuiAdpcmHelpers.cpp"
    "Source/Gui/Adpcm/GuiAdpcmText.h"
    "Source/Gui/Adpcm/GuiAdpcmValues.h"
    "Source/Gui/Adpcm/GuiAdpcm.h"
    "Source/Gui/Adpcm/GuiAdpcm.cpp"
)

set(BEEP_GUI_FILES
    "Source/Gui/Beep/GuiBeepText.h"
    "Source/Gui/Beep/GuiBeepValues.h"
    "Source/Gui/Beep/GuiBeep.h"
    "Source/Gui/Beep/GuiBeep.cpp"
)

set(FX_GUI_FILES
    "Source/Gui/Fx/GuiFxText.h"
    "Source/Gui/Fx/GuiFxValues.h"
    "Source/Gui/Fx/GuiFx.h"
    "Source/Gui/Fx/GuiFx.cpp"
)

set(PRESET_GUI_FILES
    "Source/Gui/Preset/GuiPresetText.h"
    "Source/Gui/Preset/GuiPresetValues.h"
    "Source/Gui/Preset/PresetKeys.h"
    "Source/Gui/Preset/PresetValues.h"
    "Source/Gui/Preset/GuiPreset.h"
    "Source/Gui/Preset/GuiPreset.cpp"
)

set(SETTINGS_GUI_FILES
    "Source/Gui/Settings/GuiSettingsHelpers.h"
    "Source/Gui/Settings/GuiSettingsHelpers.cpp"
    "Source/Gui/Settings/GuiSettingsText.h"
    "Source/Gui/Settings/GuiSettingsValues.h"
    "Source/Gui/Settings/SettingsKeys.h"
    "Source/Gui/Settings/SettingsValues.h"
    "Source/Gui/Settings/GuiSettings.h"
    "Source/Gui/Settings/GuiSettings.cpp"
)

set(COLORS_GUI_FILES
    "Source/Gui/Colors/GuiColors.h"
    "Source/Gui/Colors/GuiColors.cpp"
    "Source/Gui/Colors/GuiColorsText.h"
    "Source/Gui/Colors/GuiColorsValues.h"
    "Source/Gui/Colors/RetroPalette.h"
    "Source/Gui/Colors/RetroPalette.cpp"
)

set(ABOUT_GUI_FILES
    "Source/Gui/About/GuiAboutText.h"
    "Source/Gui/About/GuiAboutValues.h"
    "Source/Gui/About/GuiAbout.h"
    "Source/Gui/About/GuiAbout.cpp"
)

set(CURVE_GUI_FILES
    "Source/Gui/Curve/GuiCurveText.h"
    "Source/Gui/Curve/GuiCurveValues.h"
    "Source/Gui/Curve/GuiCurveGraph.h"
    "Source/Gui/Curve/GuiCurveGraph.cpp"
    "Source/Gui/Curve/GuiCurve.h"
    "Source/Gui/Curve/GuiCurve.cpp"
)

set(GUI_COMPONENT_UNISON_FILES
    "Source/Gui/Components/Unison/Unison.h"
    "Source/Gui/Components/Unison/Unison.cpp"
)

set(GUI_COMPONENT_FIX_FILES
    "Source/Gui/Components/Fix/Fix.h"
    "Source/Gui/Components/Fix/Fix.cpp"
)

set(GUI_COMPONENT_N88LFO_FILES
    "Source/Gui/Components/N88Lfo/N88Lfo.h"
    "Source/Gui/Components/N88Lfo/N88Lfo.cpp"
    "Source/Gui/Components/N88Lfo/N88LfoOp.h"
    "Source/Gui/Components/N88Lfo/N88LfoOp.cpp"
)

set(GUI_COMPONENT_MULDETUNE_FILES
    "Source/Gui/Components/MulDetune/MulDetune.h"
    "Source/Gui/Components/MulDetune/MulDetune.cpp"
)

set(GUI_COMPONENT_AMPENV_FILES
    "Source/Gui/Components/AmpEnv/AmpEnv.h"
    "Source/Gui/Components/AmpEnv/AmpEnv.cpp"
)

set(GUI_COMPONENT_PITCHENV_FILES
    "Source/Gui/Components/PitchEnv/PitchEnv.h"
    "Source/Gui/Components/PitchEnv/PitchEnv.cpp"
)

set(GUI_COMPONENT_SSGSWENV_FILES
    "Source/Gui/Components/SsgSwEnv/SsgSwEnv.h"
    "Source/Gui/Components/SsgSwEnv/SsgSwEnv.cpp"
)

set(GUI_COMPONENT_SSGSWENV11_FILES
    "Source/Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
    "Source/Gui/Components/SsgSwEnv11/SsgSwEnv11.cpp"
)

set(GUI_COMPONENT_SSGSWPENV11_FILES
    "Source/Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
    "Source/Gui/Components/SsgSwPEnv11/SsgSwPEnv11.cpp"
)

set(GUI_COMPONENT_PARAMBAREDITOR_FILES
    "Source/Gui/Components/ParamBarEditor/ParamBarEditor.h"
    "Source/Gui/Components/ParamBarEditor/ParamBarEditor.cpp"
)

set(GUI_COMPONENT_WAVEPREVIEW_FILES
    "Source/Gui/Components/TargetCell/TargetCell.h"
    "Source/Gui/Components/TargetCell/TargetCell.cpp"
    "Source/Gui/Components/WavePreview/WavePreview.h"
    "Source/Gui/Components/WavePreview/WavePreview.cpp"
    "Source/Gui/Components/WavePreview/WavePreviewSource.h"
    "Source/Gui/Components/WavePreview/WavePreviewSource.cpp"
)

set(GUI_COMPONENT_WTMOD_FILES
    "Source/Gui/Components/WtMod/WtMod.h"
    "Source/Gui/Components/WtMod/WtMod.cpp"
)

set(GUI_COMPONENT_SSGHWENV_FILES
    "Source/Gui/Components/SsgHwEnv/SsgHwEnv.h"
    "Source/Gui/Components/SsgHwEnv/SsgHwEnv.cpp"
)

set(GUI_COMPONENT_PITCHBUTTONS_FILES
    "Source/Gui/Components/PitchButtons/PitchButtons.h"
    "Source/Gui/Components/PitchButtons/PitchButtons.cpp"
)

set(GUI_COMPONENT_SSGSWBUTTONS_FILES
    "Source/Gui/Components/SsgSwButtons/SsgSwButtons.h"
    "Source/Gui/Components/SsgSwButtons/SsgSwButtons.cpp"
)

set(GUI_COMPONENT_MIDI_FILES
    "Source/Gui/Components/Midi/Midi.h"
    "Source/Gui/Components/Midi/Midi.cpp"
)

set(GUI_COMPONENT_LFO_OPZX7_FILES
    "Source/Gui/Components/LfoOpzx7/LfoOpzx7.h"
    "Source/Gui/Components/LfoOpzx7/LfoOpzx7.cpp"
)

set(GUI_COMPONENT_PRESET_NAME_FILES
    "Source/Gui/Components/PresetName/PresetName.h"
    "Source/Gui/Components/PresetName/PresetName.cpp"
)

set(GUI_COMPONENT_IMPORT_EXPORT_FILES
    "Source/Gui/Components/ImportExport/ImportExport.h"
    "Source/Gui/Components/ImportExport/ImportExport.cpp"
)

set(GUI_COMPONENT_IMPORT_FILES
    "Source/Gui/Components/Import/Import.h"
    "Source/Gui/Components/Import/Import.cpp"
)

set(GUI_COMPONENT_PARAMBROWSER_FILES
    "Source/Gui/Components/ParamBrowser/ParamBrowser.h"
    "Source/Gui/Components/ParamBrowser/ParamBrowser.cpp"
)

set(GUI_COMPONENT_GENWAVE_FILES
    "Source/Gui/Components/GenWave/GenWaveRender.h"
    "Source/Gui/Components/GenWave/GenWaveRender.cpp"
    "Source/Gui/Components/GenWave/GenWave.h"
    "Source/Gui/Components/GenWave/GenWave.cpp"
)

set(GUI_COMPONENT_LOADING_FILES
    "Source/Gui/Components/Loading/GuiLoading.h"
    "Source/Gui/Components/Loading/GuiLoading.cpp"
)

set(GUI_COMPONENT_LEVEL_FILES
    "Source/Gui/Components/Level/Level.h"
    "Source/Gui/Components/Level/Level.cpp"
    "Source/Gui/Components/Level/LevelNudge.h"
    "Source/Gui/Components/Level/LevelNudge.cpp"
    "Source/Gui/Components/FmToOpzx7/FmToOpzx7.h"
    "Source/Gui/Components/FmToOpzx7/FmToOpzx7.cpp"
    "Source/Gui/Components/FmToOpzx7/FmToOpzx7Tables.h"
)

set(GUI_COMPONENT_SEPARATOR_FILES
    "Source/Gui/Components/Separator/NormalSeparator.h"
    "Source/Gui/Components/Separator/NormalSeparator.cpp"
    "Source/Gui/Components/Separator/ShortSeparator.h"
    "Source/Gui/Components/Separator/ShortSeparator.cpp"
)

set(GUI_COMPONENT_QUALITY_FILES
    "Source/Gui/Components/Quality/Quality.h"
    "Source/Gui/Components/Quality/Quality.cpp"
    "Source/Gui/Components/Quality/QualityPcm.h"
    "Source/Gui/Components/Quality/QualityPcm.cpp"
)

set(GUI_COMPONENT_ALG_MATRIX_FILES
    "Source/Gui/Components/AlgMatrix/FmAlgState.h"
    "Source/Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
    "Source/Gui/Components/AlgMatrix/GuiFmAlgRouting.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("2686V\\Constants" FILES ${CONST_FILES})
source_group("2686V\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686V\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686V\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686V\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("2686V\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686V\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686V\\Processor\\Opna" FILES ${OPNA_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opn" FILES ${OPN_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl" FILES ${OPL_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl3" FILES ${OPL3_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opm" FILES ${OPM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opzx7" FILES ${OPZX7_PROCESSOR_FILES})
source_group("2686V\\Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("2686V\\Processor\\Wt" FILES ${WT_PROCESSOR_FILES})
source_group("2686V\\Processor\\Wt2" FILES ${WT2_PROCESSOR_FILES})
source_group("2686V\\Processor\\Rhythm" FILES ${RHYTHM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Adpcm" FILES ${ADPCM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Beep" FILES ${BEEP_PROCESSOR_FILES})
source_group("2686V\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("2686V\\Processor\\Curve" FILES ${CURVE_PROCESSOR_FILES})
source_group("2686V\\Gui\\Core" FILES ${GUI_FILES})
source_group("2686V\\Gui\\Components\\Unison" FILES ${GUI_COMPONENT_UNISON_FILES})
source_group("2686V\\Gui\\Components\\Fix" FILES ${GUI_COMPONENT_FIX_FILES})
source_group("2686V\\Gui\\Components\\N88Lfo" FILES ${GUI_COMPONENT_N88LFO_FILES})
source_group("2686V\\Gui\\Components\\MulDetune" FILES ${GUI_COMPONENT_MULDETUNE_FILES})
source_group("2686V\\Gui\\Components\\AmpEnv" FILES ${GUI_COMPONENT_AMPENV_FILES})
source_group("2686V\\Gui\\Components\\PitchEnv" FILES ${GUI_COMPONENT_PITCHENV_FILES})
source_group("2686V\\Gui\\Components\\SsgSwEnv" FILES ${GUI_COMPONENT_SSGSWENV_FILES})
source_group("2686V\\Gui\\Components\\Lfo\\Opzx7" FILES ${GUI_COMPONENT_LFO_OPZX7_FILES})
source_group("2686V\\Gui\\Components\\Midi" FILES ${GUI_COMPONENT_MIDI_FILES})
source_group("2686V\\Gui\\Components\\PitchButtons" FILES ${GUI_COMPONENT_PITCHBUTTONS_FILES})
source_group("2686V\\Gui\\Components\\SsgSwButtons" FILES ${GUI_COMPONENT_SSGSWBUTTONS_FILES})
source_group("2686V\\Gui\\Components\\PresetName" FILES ${GUI_COMPONENT_PRESET_NAME_FILES})
source_group("2686V\\Gui\\Components\\ImportExport" FILES ${GUI_COMPONENT_IMPORT_EXPORT_FILES})
source_group("2686V\\Gui\\Components\\Import" FILES ${GUI_COMPONENT_IMPORT_FILES})
source_group("2686V\\Gui\\Components\\ParamBrowser" FILES ${GUI_COMPONENT_PARAMBROWSER_FILES})
source_group("2686V\\Gui\\Components\\GenWave" FILES ${GUI_COMPONENT_GENWAVE_FILES})
source_group("2686V\\Gui\\Components\\Loading" FILES ${GUI_COMPONENT_LOADING_FILES})
source_group("2686V\\Gui\\Components\\Level" FILES ${GUI_COMPONENT_LEVEL_FILES})
source_group("2686V\\Gui\\Components\\Separator" FILES ${GUI_COMPONENT_SEPARATOR_FILES})
source_group("2686V\\Gui\\Components\\Quality" FILES ${GUI_COMPONENT_QUALITY_FILES})
source_group("2686V\\Gui\\Components\\SsgSwPEnv11" FILES ${GUI_COMPONENT_SSGSWPENV11_FILES})
source_group("2686V\\Gui\\Components\\SsgSwEnv11" FILES ${GUI_COMPONENT_SSGSWENV11_FILES})
source_group("2686V\\Gui\\Components\\ParamBarEditor" FILES ${GUI_COMPONENT_PARAMBAREDITOR_FILES})
source_group("2686V\\Gui\\Components\\WavePreview" FILES ${GUI_COMPONENT_WAVEPREVIEW_FILES})
source_group("2686V\\Gui\\Components\\WtMod" FILES ${GUI_COMPONENT_WTMOD_FILES})
source_group("2686V\\Gui\\Components\\SsgHwEnv" FILES ${GUI_COMPONENT_SSGHWENV_FILES})
source_group("2686V\\Gui\\Components\\AlgMatrix" FILES ${GUI_COMPONENT_ALG_MATRIX_FILES})
source_group("2686V\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("2686V\\Gui\\Tab\\Opna" FILES ${OPNA_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opn" FILES ${OPN_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opl" FILES ${OPL_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opl3" FILES ${OPL3_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opm" FILES ${OPM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opzx7" FILES ${OPZX7_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Ssg" FILES ${SSG_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Wt" FILES ${WT_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Wt2" FILES ${WT2_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Beep" FILES ${BEEP_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Curve" FILES ${CURVE_GUI_FILES})
source_group("2686V\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Colors" FILES ${COLORS_GUI_FILES})

create_cyross_plugin("2686V" "2686")

