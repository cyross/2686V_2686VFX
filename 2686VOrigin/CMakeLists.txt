cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_2686VOrigin
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_2686VOrigin
    HEADER_NAME "AppIconForAbout.h"
    NAMESPACE AppIconAboutForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
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

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/ProcessorSsgKeys.h"
    "Source/Processor/Ssg/ProcessorSsgNames.h"
    "Source/Processor/Ssg/ProcessorSsgValues.h"
    "Source/Processor/Ssg/ProcessorSsg.h"
    "Source/Processor/Ssg/ProcessorSsg.cpp"
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
    "Source/Gui/Opna/ProcessorOpnValues.h"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/GuiSsgText.h"
    "Source/Gui/Ssg/GuiSsgValues.h"
    "Source/Gui/Ssg/GuiSsg.h"
    "Source/Gui/Ssg/GuiSsg.cpp"
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

set(GUI_COMPONENT_SSGHWENV_FILES
    "Source/Gui/Components/SsgHwEnv/SsgHwEnv.h"
    "Source/Gui/Components/SsgHwEnv/SsgHwEnv.cpp"
)

set(GUI_COMPONENT_SSGSWPENV11_FILES
    "Source/Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
    "Source/Gui/Components/SsgSwPEnv11/SsgSwPEnv11.cpp"
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
source_group("2686VO\\Constants" FILES ${CONST_FILES})
source_group("2686VO\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686VO\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686VO\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686VO\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("2686VO\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686VO\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686VO\\Processor\\Opna" FILES ${OPNA_PROCESSOR_FILES})
source_group("2686VO\\Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("2686VO\\Processor\\Rhythm" FILES ${RHYTHM_PROCESSOR_FILES})
source_group("2686VO\\Processor\\Adpcm" FILES ${ADPCM_PROCESSOR_FILES})
source_group("2686VO\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("2686VO\\Gui\\Core" FILES ${GUI_FILES})
source_group("2686VO\\Gui\\Components\\ParamBarEditor" FILES ${GUI_COMPONENT_PARAMBAREDITOR_FILES})
source_group("2686VO\\Gui\\Components\\WavePreview" FILES ${GUI_COMPONENT_WAVEPREVIEW_FILES})
source_group("2686VO\\Gui\\Components\\WtMod" FILES ${GUI_COMPONENT_WTMOD_FILES})
source_group("2686VO\\Gui\\Components\\Unison" FILES ${GUI_COMPONENT_UNISON_FILES})
source_group("2686VO\\Gui\\Components\\Fix" FILES ${GUI_COMPONENT_FIX_FILES})
source_group("2686VO\\Gui\\Components\\N88Lfo" FILES ${GUI_COMPONENT_N88LFO_FILES})
source_group("2686VO\\Gui\\Components\\MulDetune" FILES ${GUI_COMPONENT_MULDETUNE_FILES})
source_group("2686VO\\Gui\\Components\\AmpEnv" FILES ${GUI_COMPONENT_AMPENV_FILES})
source_group("2686VO\\Gui\\Components\\PitchEnv" FILES ${GUI_COMPONENT_PITCHENV_FILES})
source_group("2686VO\\Gui\\Components\\SsgSwEnv" FILES ${GUI_COMPONENT_SSGSWENV_FILES})
source_group("2686VO\\Gui\\Components\\Lfo\\Opzx7" FILES ${GUI_COMPONENT_LFO_OPZX7_FILES})
source_group("2686VO\\Gui\\Components\\Midi" FILES ${GUI_COMPONENT_MIDI_FILES})
source_group("2686VO\\Gui\\Components\\PitchButtons" FILES ${GUI_COMPONENT_PITCHBUTTONS_FILES})
source_group("2686VO\\Gui\\Components\\SsgSwButtons" FILES ${GUI_COMPONENT_SSGSWBUTTONS_FILES})
source_group("2686VO\\Gui\\Components\\PresetName" FILES ${GUI_COMPONENT_PRESET_NAME_FILES})
source_group("2686VO\\Gui\\Components\\ImportExport" FILES ${GUI_COMPONENT_IMPORT_EXPORT_FILES})
source_group("2686VO\\Gui\\Components\\Import" FILES ${GUI_COMPONENT_IMPORT_FILES})
source_group("2686VO\\Gui\\Components\\ParamBrowser" FILES ${GUI_COMPONENT_PARAMBROWSER_FILES})
source_group("2686VO\\Gui\\Components\\GenWave" FILES ${GUI_COMPONENT_GENWAVE_FILES})
source_group("2686VO\\Gui\\Components\\Loading" FILES ${GUI_COMPONENT_LOADING_FILES})
source_group("2686VO\\Gui\\Components\\Level" FILES ${GUI_COMPONENT_LEVEL_FILES})
source_group("2686VO\\Gui\\Components\\Separator" FILES ${GUI_COMPONENT_SEPARATOR_FILES})
source_group("2686VO\\Gui\\Components\\Quality" FILES ${GUI_COMPONENT_QUALITY_FILES})
source_group("2686VO\\Gui\\Components\\SsgSwPEnv11" FILES ${GUI_COMPONENT_SSGSWPENV11_FILES})
source_group("2686VO\\Gui\\Components\\SsgSwEnv11" FILES ${GUI_COMPONENT_SSGSWENV11_FILES})
source_group("2686VO\\Gui\\Components\\SsgHwEnv" FILES ${GUI_COMPONENT_SSGHWENV_FILES})
source_group("2686VO\\Gui\\Components\\AlgMatrix" FILES ${GUI_COMPONENT_ALG_MATRIX_FILES})
source_group("2686VO\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("2686VO\\Gui\\Tab\\Opna" FILES ${OPNA_GUI_FILES})
source_group("2686VO\\Gui\\Tab\\Ssg" FILES ${SSG_GUI_FILES})
source_group("2686VO\\Gui\\Tab\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("2686VO\\Gui\\Tab\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("2686VO\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("2686VO\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("2686VO\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686VO\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})
source_group("2686VOrigin\\Gui\\Tab\\Colors" FILES ${COLORS_GUI_FILES})

create_cyross_plugin("2686VOrigin" "28VO" "86V")

