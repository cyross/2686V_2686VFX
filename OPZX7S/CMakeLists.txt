cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_OPZX7S
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_OPZX7S
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
    "Source/Core/Const/ConstGlobal.h"
    "Source/Core/Const/ConstFileValues.h"
    "Source/Core/Const/ConstMmlKeys.h"
    "Source/Core/Const/ConstMmlValues.h"
)

set(SYNTH_FILES
    "Source/Core/Synth/SynthMode.h"
    "Source/Core/Synth/SynthCore.h"
    "Source/Core/Synth/SynthParams.h"
    "Source/Core/Synth/SynthVoice.h"
    "Source/Core/Synth/SynthVoice.cpp"
    "Source/Core/Synth/SynthHelpers.h"
    "Source/Core/Synth/SynthHelpers.cpp"
    "Source/Core/Synth/UnisonParams.h"
    "Source/Core/Synth/UnisonState.h"
    "Source/Core/Synth/CommonParams.h"
)

set(FM_FIX_FILES
    "Source/Generator/Fm/Fix/FmFixParams.h"
    "Source/Generator/Fm/Fix/FmFix.h"
    "Source/Generator/Fm/Fix/FmFix.cpp"
)

set(LFSR_NOISE_GEN_FILES
    "Source/Generator/Noise/Lfsr/GenNoiseLfsrParams.h"
    "Source/Generator/Noise/Lfsr/GenNoiseLfsr.h"
    "Source/Generator/Noise/Lfsr/GenNoiseLfsr.cpp"
)

set(SSG_NOISE_GEN_FILES
    "Source/Generator/Noise/Ssg/GenNoiseSsgParams.h"
    "Source/Generator/Noise/Ssg/GenNoiseSsg.h"
    "Source/Generator/Noise/Ssg/GenNoiseSsg.cpp"
)

set(PCM_GENERATOR_FILES
    "Source/Generator/Pcm/Adpcm/GenAdpcm.h"
    "Source/Generator/Pcm/Adpcm/GenAdpcm.cpp"
    "Source/Generator/Pcm/Dpcm/GenDpcm.h"
    "Source/Generator/Pcm/Dpcm/GenDpcm.cpp"
    "Source/Generator/Pcm/Helper/GenPcmHelper.h"
    "Source/Generator/Pcm/Helper/GenPcmHelper.cpp"
)

set(FM_FILES
    "Source/Core/Fm/FmCore.h"
    "Source/Core/Fm/FmOperator.h"
    "Source/Core/Fm/FmOperator.cpp"
    "Source/Core/Fm/FmOpParams.h"
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
    "Source/Core/Gui/GuiEnvelopeGraph.cpp"
    "Source/Core/Gui/GuiCopyObj.h"
)

set(PROCESSOR_FILES
    "Source/Core/Processor/ProcessorKeys.h"
    "Source/Core/Processor/ProcessorNames.h"
    "Source/Core/Processor/ProcessorValues.h"
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

set(AMP_ENVELOPE_FILES
    "Source/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"
    "Source/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.h"
    "Source/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.cpp"
    "Source/Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
    "Source/Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
    "Source/Effect/Envelope/Amp/SsgSw/EnvSsgSw.cpp"
    "Source/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
    "Source/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
    "Source/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.cpp"
    "Source/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
    "Source/Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
    "Source/Effect/Envelope/Amp/SsgHw/EnvSsgHw.cpp"
    "Source/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
    "Source/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
    "Source/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.cpp"
)

set(PITCH_ENVELOPE_FILES
    "Source/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
    "Source/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
    "Source/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.cpp"
    "Source/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
    "Source/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
    "Source/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.cpp"
)

set(KEY_SCALE_FILES
    "Source/Effect/KeyScale/Ops/KSOpsParams.h"
    "Source/Effect/KeyScale/Ops/KSOps.h"
    "Source/Effect/KeyScale/Ops/KSOps.cpp"
    "Source/Effect/KeyScale/Opz/KSOpzParams.h"
    "Source/Effect/KeyScale/Opz/KSOpz.h"
    "Source/Effect/KeyScale/Opz/KSOpz.cpp"
    "Source/Effect/KeyScale/Ma7/KSMa7Params.h"
    "Source/Effect/KeyScale/Ma7/KSMa7.h"
    "Source/Effect/KeyScale/Ma7/KSMa7.cpp"
)

set(DETUNE_FILES
    "Source/Effect/Detune/Opzx7/DetuneOpzx7Params.h"
    "Source/Effect/Detune/Opzx7/DetuneOpzx7.h"
    "Source/Effect/Detune/Opzx7/DetuneOpzx7.cpp"
)

set(FEEDBACK_FILES
    "Source/Effect/Feedback/FeedbackParams.h"
    "Source/Effect/Feedback/Feedback.h"
    "Source/Effect/Feedback/Feedback.cpp"
)

set(LFO_FILES
    "Source/Effect/Lfo/Opzx7/LfoOpzx7Params.h"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7.h"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7.cpp"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7Unit.h"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7Unit.cpp"
)

set(FX_EFFECT_FILES
    "Source/Effect/Fx/Fx.h"
    "Source/Effect/Fx/Fx.cpp"
)

set(OPZX7_SYNTH_FILES
    "Source/Synth/Opzx7/SynthOpzx7.h"
    "Source/Synth/Opzx7/SynthOpzx7.cpp"
    "Source/Synth/Opzx7/SynthOpzx7Params.h"
    "Source/Synth/Opzx7/Operator/SynthOpzx7Op.h"
    "Source/Synth/Opzx7/Operator/SynthOpzx7Op.cpp"
    "Source/Synth/Opzx7/Operator/SynthOpzx7OpParams.h"
)

set(CURVE_FILES
    "Source/Advanced/Curve/AdvancedCurve.h"
    "Source/Advanced/Curve/AdvancedCurve.cpp"
    "Source/Advanced/Curve/AdvancedCurveParams.h"
)

set(OPZX7_PROCESSOR_FILES
    "Source/Processor/Opzx7/ProcessorOpzx7Keys.h"
    "Source/Processor/Opzx7/ProcessorOpzx7Names.h"
    "Source/Processor/Opzx7/ProcessorOpzx7Values.h"
    "Source/Processor/Opzx7/ProcessorOpzx7.h"
    "Source/Processor/Opzx7/ProcessorOpzx7.cpp"
)

set(CURVE_PROCESSOR_FILES
    "Source/Processor/Curve/ProcessorCurveKeys.h"
    "Source/Processor/Curve/ProcessorCurveNames.h"
    "Source/Processor/Curve/ProcessorCurveValues.h"
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

set(OPZX7_GUI_FILES
    "Source/Gui/Opzx7/GuiOpzx7Helpers.h"
    "Source/Gui/Opzx7/GuiOpzx7Helpers.cpp"
    "Source/Gui/Opzx7/GuiOpzx7Text.h"
    "Source/Gui/Opzx7/GuiOpzx7Values.h"
    "Source/Gui/Opzx7/GuiOpzx7.h"
    "Source/Gui/Opzx7/GuiOpzx7.cpp"
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

set(GENERATOR_WTMOD_FILES
    "Source/Generator/WtMod/GenWtModulator.h"
)

set(GENERATOR_FDS_FILES
    "Source/Generator/Fds/GenFdsModTable.h"
)

set(GUI_COMPONENT_PARAMBAREDITOR_FILES
    "Source/Gui/Components/ParamBarEditor/ParamBarEditor.h"
    "Source/Gui/Components/ParamBarEditor/ParamBarEditor.cpp"
)

set(GUI_COMPONENT_WAVEPREVIEW_FILES
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

set(GUI_COMPONENT_MULDETUNE_FILES
    "Source/Gui/Components/MulDetune/MulDetune.h"
    "Source/Gui/Components/MulDetune/MulDetune.cpp"
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

set(GUI_COMPONENT_VIEW_MODE_FILES
    "Source/Gui/Components/ViewMode/ViewMode.h"
    "Source/Gui/Components/ViewMode/ViewMode.cpp"
)

set(GUI_COMPONENT_IMPORT_EXPORT_FILES
    "Source/Gui/Components/ImportExport/ImportExport.h"
    "Source/Gui/Components/ImportExport/ImportExport.cpp"
)

set(GUI_COMPONENT_IMPORT_FILES
    "Source/Gui/Components/Import/Import.h"
    "Source/Gui/Components/Import/Import.cpp"
)

set(GUI_COMPONENT_LEVEL_FILES
    "Source/Gui/Components/Level/Level.h"
    "Source/Gui/Components/Level/Level.cpp"
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
)

set(GUI_COMPONENT_ALG_MATRIX_FILES
    "Source/Gui/Components/AlgMatrix/FmAlgState.h"
    "Source/Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
    "Source/Gui/Components/AlgMatrix/GuiFmAlgRouting.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("OPZX7S\\Constants" FILES ${CONST_FILES})
source_group("OPZX7S\\Documents" FILES ${DOCUMENT_FILES})
source_group("OPZX7S\\Advanced\\Curve" FILES ${CURVE_FILES})
source_group("OPZX7S\\Generator\\Pcm" FILES ${PCM_GENERATOR_FILES})
source_group("OPZX7S\\Generator\\Noise\\Lfsr" FILES ${LFSR_NOISE_GEN_FILES})
source_group("OPZX7S\\Generator\\Noise\\Ssg" FILES ${SSG_NOISE_GEN_FILES})
source_group("OPZX7S\\Generator\\Fm\\Fix" FILES ${FM_FIX_FILES})
source_group("OPZX7S\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("OPZX7S\\Synth\\Fm" FILES ${FM_FILES})
source_group("OPZX7S\\Synth\\Opzx7" FILES ${OPZX7_SYNTH_FILES})
source_group("OPZX7S\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("OPZX7S\\Effect\\Envelope\\Amp" FILES ${AMP_ENVELOPE_FILES})
source_group("OPZX7S\\Effect\\Envelope\\Pitch" FILES ${PITCH_ENVELOPE_FILES})
source_group("OPZX7S\\Effect\\Detune" FILES ${DETUNE_FILES})
source_group("OPZX7S\\Effect\\Lfo" FILES ${LFO_FILES})
source_group("OPZX7S\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("OPZX7S\\Effect\\Feedback" FILES ${FEEDBACK_FILES})
source_group("OPZX7S\\Effect\\KeyScale" FILES ${KEY_SCALE_FILES})
source_group("OPZX7S\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("OPZX7S\\Processor\\Opzx7" FILES ${OPZX7_PROCESSOR_FILES})
source_group("OPZX7S\\Processor\\Curve" FILES ${CURVE_PROCESSOR_FILES})
source_group("OPZX7S\\Gui\\Core" FILES ${GUI_FILES})
source_group("OPZX7S\\Generator\\WtMod" FILES ${GENERATOR_WTMOD_FILES})
source_group("OPZX7S\\Generator\\Fds" FILES ${GENERATOR_FDS_FILES})
source_group("OPZX7S\\Gui\\Components\\ParamBarEditor" FILES ${GUI_COMPONENT_PARAMBAREDITOR_FILES})
source_group("OPZX7S\\Gui\\Components\\WavePreview" FILES ${GUI_COMPONENT_WAVEPREVIEW_FILES})
source_group("OPZX7S\\Gui\\Components\\WtMod" FILES ${GUI_COMPONENT_WTMOD_FILES})
source_group("OPZX7S\\Gui\\Components\\Unison" FILES ${GUI_COMPONENT_UNISON_FILES})
source_group("OPZX7S\\Gui\\Components\\Fix" FILES ${GUI_COMPONENT_FIX_FILES})
source_group("OPZX7S\\Gui\\Components\\MulDetune" FILES ${GUI_COMPONENT_MULDETUNE_FILES})
source_group("OPZX7S\\Gui\\Components\\AmpEnv" FILES ${GUI_COMPONENT_AMPENV_FILES})
source_group("OPZX7S\\Gui\\Components\\PitchEnv" FILES ${GUI_COMPONENT_PITCHENV_FILES})
source_group("OPZX7S\\Gui\\Components\\SsgSwEnv" FILES ${GUI_COMPONENT_SSGSWENV_FILES})
source_group("OPZX7S\\Gui\\Components\\Lfo\\Opzx7" FILES ${GUI_COMPONENT_LFO_OPZX7_FILES})
source_group("OPZX7S\\Gui\\Components\\Midi" FILES ${GUI_COMPONENT_MIDI_FILES})
source_group("OPZX7S\\Gui\\Components\\PitchButtons" FILES ${GUI_COMPONENT_PITCHBUTTONS_FILES})
source_group("OPZX7S\\Gui\\Components\\SsgSwButtons" FILES ${GUI_COMPONENT_SSGSWBUTTONS_FILES})
source_group("OPZX7S\\Gui\\Components\\PresetName" FILES ${GUI_COMPONENT_PRESET_NAME_FILES})
source_group("OPZX7S\\Gui\\Components\\ViewMode" FILES ${GUI_COMPONENT_VIEW_MODE_FILES})
source_group("OPZX7S\\Gui\\Components\\ImportExport" FILES ${GUI_COMPONENT_IMPORT_EXPORT_FILES})
source_group("OPZX7S\\Gui\\Components\\Import" FILES ${GUI_COMPONENT_IMPORT_FILES})
source_group("OPZX7S\\Gui\\Components\\Level" FILES ${GUI_COMPONENT_LEVEL_FILES})
source_group("OPZX7S\\Gui\\Components\\Separator" FILES ${GUI_COMPONENT_SEPARATOR_FILES})
source_group("OPZX7S\\Gui\\Components\\Quality" FILES ${GUI_COMPONENT_QUALITY_FILES})
source_group("OPZX7S\\Gui\\Components\\SsgSwPEnv11" FILES ${GUI_COMPONENT_SSGSWPENV11_FILES})
source_group("OPZX7S\\Gui\\Components\\SsgSwEnv11" FILES ${GUI_COMPONENT_SSGSWENV11_FILES})
source_group("OPZX7S\\Gui\\Components\\SsgHwEnv" FILES ${GUI_COMPONENT_SSGHWENV_FILES})
source_group("OPZX7S\\Gui\\Components\\AlgMatrix" FILES ${GUI_COMPONENT_ALG_MATRIX_FILES})
source_group("OPZX7S\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("OPZX7S\\Gui\\Tab\\Opzx7" FILES ${OPZX7_GUI_FILES})
source_group("OPZX7S\\Gui\\Tab\\Curve" FILES ${CURVE_GUI_FILES})
source_group("OPZX7S\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("OPZX7S\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("OPZX7S\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("OPZX7S\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})
source_group("OPZX7S\\Gui\\Tab\\Colors" FILES ${COLORS_GUI_FILES})

create_cyross_plugin("OPZX7S" "ZX7S")
