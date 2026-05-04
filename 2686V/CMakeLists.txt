cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout
    HEADER_NAME "AppIconForAbout.h"
    NAMESPACE AppIconAboutForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
)

# 定義
set(CONST_FILES
    "Source/Core/Const/Global.h"
    "Source/Core/Const/FileValues.h"
    "Source/Core/Const/MmlKeys.h"
    "Source/Core/Const/MmlValues.h"
    "Source/Core/Const/PrKeys.h"
    "Source/Core/Const/PrNames.h"
    "Source/Core/Const/PrValues.h"
    "Source/Core/Const/PresetKeys.h"
    "Source/Core/Const/PresetValues.h"
    "Source/Core/Const/SettingsKeys.h"
    "Source/Core/Const/SettingsValues.h"
)

set(SYNTH_FILES
    "Source/Core/Synth/Mode.h"
    "Source/Core/Synth/Mode.cpp"
    "Source/Core/Synth/Pcm.h"
    "Source/Core/Synth/Pcm.cpp"
    "Source/Core/Synth/SynthCore.h"
    "Source/Core/Synth/SynthCore.cpp"
    "Source/Core/Synth/AdsrParams.h"
    "Source/Core/Synth/SynthParams.h"
    "Source/Core/Synth/SynthVoice.h"
    "Source/Core/Synth/SynthVoice.cpp"
    "Source/Core/Synth/SynthHelpers.h"
    "Source/Core/Synth/SynthHelpers.cpp"
)

set(FM_FILES
    "Source/Core/Fm/FmCore.h"
    "Source/Core/Fm/FmCore.cpp"
    "Source/Core/Fm/FmOperator.h"
    "Source/Core/Fm/FmOperator.cpp"
    "Source/Core/Fm/FmOpParams.h"
    "Source/Core/Fm/RegisterType.h"
    "Source/Core/Fm/RegisterConverter.h"
    "Source/Core/Fm/RegisterConverter.cpp"
    "Source/Core/Fm/SliderRegMap.h"
    "Source/Core/Fm/SliderRegMap.cpp"
)

set(GUI_FILES
    "Source/Core/Gui/GuiBase.h"
    "Source/Core/Gui/GuiBase.cpp"
    "Source/Core/Gui/GuiColor.h"
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
)

set(PROCESSOR_FILES
    "Source/Core/Processor/PrBase.h"
    "Source/Core/Processor/PrBase.cpp"
    "Source/Core/Processor/PluginProcessor.h"
    "Source/Core/Processor/PluginProcessor.cpp"
)

set(EDITOR_FILES
    "Source/Core/Editor/PluginEditor.h"
    "Source/Core/Editor/PluginEditor.cpp"
)

set(OPNA_SYNTH_FILES
    "Source/Synth/Opna/OpnaCore.h"
    "Source/Synth/Opna/OpnaCore.cpp"
    "Source/Synth/Opna/OpnaParams.h"
)

set(OPNA_OPERATOR_FILES
    "Source/Synth/Opna/Operator/OpnaOperator.h"
    "Source/Synth/Opna/Operator/OpnaOperator.cpp"
)

set(OPNA_PROCESSOR_FILES
    "Source/Processor/Opna/PrOpna.h"
    "Source/Processor/Opna/PrOpna.cpp"
)

set(OPNA_GUI_FILES
    "Source/Gui/Opna/GuiOpna.h"
    "Source/Gui/Opna/GuiOpna.cpp"
)

set(OPN_SYNTH_FILES
    "Source/Synth/Opn/OpnCore.h"
    "Source/Synth/Opn/OpnCore.cpp"
    "Source/Synth/Opn/OpnParams.h"
)

set(OPN_OPERATOR_FILES
    "Source/Synth/Opn/Operator/OpnOperator.h"
    "Source/Synth/Opn/Operator/OpnOperator.cpp"
)

set(OPN_PROCESSOR_FILES
    "Source/Processor/Opn/PrOpn.h"
    "Source/Processor/Opn/PrOpn.cpp"
)

set(OPN_GUI_FILES
    "Source/Gui/Opn/GuiOpn.h"
    "Source/Gui/Opn/GuiOpn.cpp"
)

set(OPL_SYNTH_FILES
    "Source/Synth/Opl/OplCore.h"
    "Source/Synth/Opl/OplCore.cpp"
    "Source/Synth/Opl/OplParams.h"
)

set(OPL_OPERATOR_FILES
    "Source/Synth/Opl/Operator/OplOperator.h"
    "Source/Synth/Opl/Operator/OplOperator.cpp"
)

set(OPL_PROCESSOR_FILES
    "Source/Processor/Opl/PrOpl.h"
    "Source/Processor/Opl/PrOpl.cpp"
)

set(OPL_GUI_FILES
    "Source/Gui/Opl/GuiOpl.h"
    "Source/Gui/Opl/GuiOpl.cpp"
)

set(OPL3_SYNTH_FILES
    "Source/Synth/Opl3/Opl3Core.h"
    "Source/Synth/Opl3/Opl3Core.cpp"
    "Source/Synth/Opl3/Opl3Params.h"
)

set(OPL3_OPERATOR_FILES
    "Source/Synth/Opl3/Operator/Opl3Operator.h"
    "Source/Synth/Opl3/Operator/Opl3Operator.cpp"
)

set(OPL3_PROCESSOR_FILES
    "Source/Processor/Opl3/PrOpl3.h"
    "Source/Processor/Opl3/PrOpl3.cpp"
)

set(OPL3_GUI_FILES
    "Source/Gui/Opl3/GuiOpl3.h"
    "Source/Gui/Opl3/GuiOpl3.cpp"
)

set(OPM_SYNTH_FILES
    "Source/Synth/Opm/OpmCore.h"
    "Source/Synth/Opm/OpmCore.cpp"
    "Source/Synth/Opm/OpmParams.h"
)

set(OPM_OPERATOR_FILES
    "Source/Synth/Opm/Operator/OpmOperator.h"
    "Source/Synth/Opm/Operator/OpmOperator.cpp"
)

set(OPM_PROCESSOR_FILES
    "Source/Processor/Opm/PrOpm.h"
    "Source/Processor/Opm/PrOpm.cpp"
)

set(OPM_GUI_FILES
    "Source/Gui/Opm/GuiOpm.h"
    "Source/Gui/Opm/GuiOpm.cpp"
)

set(OPZX3_SYNTH_FILES
    "Source/Synth/Opzx3/Opzx3Core.h"
    "Source/Synth/Opzx3/Opzx3Core.cpp"
    "Source/Synth/Opzx3/Opzx3Params.h"
)

set(OPZX3_OPERATOR_FILES
    "Source/Synth/Opzx3/Operator/Opzx3Operator.h"
    "Source/Synth/Opzx3/Operator/Opzx3Operator.cpp"
)

set(OPZX3_PROCESSOR_FILES
    "Source/Processor/Opzx3/PrOpzx3.h"
    "Source/Processor/Opzx3/PrOpzx3.cpp"
)

set(OPZX3_GUI_FILES
    "Source/Gui/Opzx3/GuiOpzx3.h"
    "Source/Gui/Opzx3/GuiOpzx3.cpp"
)

set(SSG_SYNTH_FILES
    "Source/Synth/Ssg/SsgCore.h"
    "Source/Synth/Ssg/SsgCore.cpp"
    "Source/Synth/Ssg/SsgParams.h"
)

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/PrSsg.h"
    "Source/Processor/Ssg/PrSsg.cpp"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/GuiSsg.h"
    "Source/Gui/Ssg/GuiSsg.cpp"
)

set(WT_SYNTH_FILES
    "Source/Synth/Wavetable/WtCore.h"
    "Source/Synth/Wavetable/WtCore.cpp"
    "Source/Synth/Wavetable/WtParams.h"
)

set(WT_PROCESSOR_FILES
    "Source/Processor/Wavetable/PrWt.h"
    "Source/Processor/Wavetable/PrWt.cpp"
)

set(WT_GUI_FILES
    "Source/Gui/Wavetable/GuiWt.h"
    "Source/Gui/Wavetable/GuiWt.cpp"
)

set(RHYTHM_SYNTH_FILES
    "Source/Synth/Rhythm/RhythmCore.h"
    "Source/Synth/Rhythm/RhythmCore.cpp"
    "Source/Synth/Rhythm/RhythmParams.h"
)

set(RHYTHM_PROCESSOR_FILES
    "Source/Processor/Rhythm/PrRhythm.h"
    "Source/Processor/Rhythm/PrRhythm.cpp"
)

set(RHYTHM_GUI_FILES
    "Source/Gui/Rhythm/GuiRhythm.h"
    "Source/Gui/Rhythm/GuiRhythm.cpp"
)

set(ADPCM_SYNTH_FILES
    "Source/Synth/Adpcm/AdpcmCore.h"
    "Source/Synth/Adpcm/AdpcmCore.cpp"
    "Source/Synth/Adpcm/AdpcmParams.h"
)

set(ADPCM_PROCESSOR_FILES
    "Source/Processor/Adpcm/PrAdpcm.h"
    "Source/Processor/Adpcm/PrAdpcm.cpp"
)

set(ADPCM_GUI_FILES
    "Source/Gui/Adpcm/GuiAdpcm.h"
    "Source/Gui/Adpcm/GuiAdpcm.cpp"
)

set(BEEP_SYNTH_FILES
    "Source/Synth/Beep/BeepCore.h"
    "Source/Synth/Beep/BeepCore.cpp"
    "Source/Synth/Beep/BeepParams.h"
)

set(BEEP_PROCESSOR_FILES
    "Source/Processor/Beep/PrBeep.h"
    "Source/Processor/Beep/PrBeep.cpp"
)

set(BEEP_GUI_FILES
    "Source/Gui/Beep/GuiBeep.h"
    "Source/Gui/Beep/GuiBeep.cpp"
)

set(FX_EFFECT_FILES
    "Source/Effect/Fx/FxCore.h"
    "Source/Effect/Fx/FxCore.cpp"
    "Source/Effect/Fx/FxParams.h"
)

set(FX_PROCESSOR_FILES
    "Source/Processor/Fx/PrFx.h"
    "Source/Processor/Fx/PrFx.cpp"
)

set(FX_GUI_FILES
    "Source/Gui/Fx/GuiFx.h"
    "Source/Gui/Fx/GuiFx.cpp"
)

set(PRESET_GUI_FILES
    "Source/Gui/PRESET/GuiPreset.h"
    "Source/Gui/PRESET/GuiPreset.cpp"
)

set(SETTINGS_GUI_FILES
    "Source/Gui/SETTINGS/GuiSettings.h"
    "Source/Gui/SETTINGS/GuiSettings.cpp"
)

set(ABOUT_GUI_FILES
    "Source/Gui/About/GuiAbout.h"
    "Source/Gui/About/GuiAbout.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("Core\\Const" FILES ${CONST_FILES})
source_group("Core\\Synth" FILES ${SYNTH_FILES})
source_group("Core\\Fm" FILES ${FM_FILES})
source_group("Core\\Gui" FILES ${GUI_FILES})
source_group("Core\\Processor" FILES ${PROCESSOR_FILES})
source_group("Core\\Editor" FILES ${EDITOR_FILES})
source_group("Core\\Docs" FILES ${DOCUMENT_FILES})
source_group("Synth\\Opna" FILES ${OPNA_SYNTH_FILES})
source_group("Synth\\Opn" FILES ${OPN_SYNTH_FILES})
source_group("Synth\\Opl" FILES ${OPL_SYNTH_FILES})
source_group("Synth\\Opl3" FILES ${OPL3_SYNTH_FILES})
source_group("Synth\\Opm" FILES ${OPM_SYNTH_FILES})
source_group("Synth\\Opzx3" FILES ${OPZX3_SYNTH_FILES})
source_group("Synth\\Opna\\Operator" FILES ${OPNA_OPERATOR_FILES})
source_group("Synth\\Opn\\Operator" FILES ${OPN_OPERATOR_FILES})
source_group("Synth\\Opl\\Operator" FILES ${OPL_OPERATOR_FILES})
source_group("Synth\\Opl3\\Operator" FILES ${OPL3_OPERATOR_FILES})
source_group("Synth\\Opm\\Operator" FILES ${OPM_OPERATOR_FILES})
source_group("Synth\\Opzx3\\Operator" FILES ${OPZX3_OPERATOR_FILES})
source_group("Synth\\Ssg" FILES ${SSG_SYNTH_FILES})
source_group("Synth\\Wavetable" FILES ${WT_SYNTH_FILES})
source_group("Synth\\Rhythm" FILES ${RHYTHM_SYNTH_FILES})
source_group("Synth\\Adpcm" FILES ${ADPCM_SYNTH_FILES})
source_group("Synth\\Beep" FILES ${BEEP_SYNTH_FILES})
source_group("Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("Processor\\Opna" FILES ${OPNA_PROCESSOR_FILES})
source_group("Processor\\Opn" FILES ${OPN_PROCESSOR_FILES})
source_group("Processor\\Opl" FILES ${OPL_PROCESSOR_FILES})
source_group("Processor\\Opl3" FILES ${OPL3_PROCESSOR_FILES})
source_group("Processor\\Opm" FILES ${OPM_PROCESSOR_FILES})
source_group("Processor\\Opzx3" FILES ${OPZX3_PROCESSOR_FILES})
source_group("Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("Processor\\Wavetable" FILES ${WT_PROCESSOR_FILES})
source_group("Processor\\Rhythm" FILES ${RHYTHM_PROCESSOR_FILES})
source_group("Processor\\Adpcm" FILES ${ADPCM_PROCESSOR_FILES})
source_group("Processor\\Beep" FILES ${BEEP_PROCESSOR_FILES})
source_group("Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("Gui\\Opna" FILES ${OPNA_GUI_FILES})
source_group("Gui\\Opn" FILES ${OPN_GUI_FILES})
source_group("Gui\\Opl" FILES ${OPL_GUI_FILES})
source_group("Gui\\Opl3" FILES ${OPL3_GUI_FILES})
source_group("Gui\\Opm" FILES ${OPM_GUI_FILES})
source_group("Gui\\Opzx3" FILES ${OPZX3_GUI_FILES})
source_group("Gui\\Ssg" FILES ${SSG_GUI_FILES})
source_group("Gui\\Wavetable" FILES ${WT_GUI_FILES})
source_group("Gui\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("Gui\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("Gui\\Beep" FILES ${BEEP_GUI_FILES})
source_group("Gui\\Fx" FILES ${FX_GUI_FILES})
source_group("Gui\\Preset" FILES ${PRESET_GUI_FILES})
source_group("Gui\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("Gui\\About" FILES ${ABOUT_GUI_FILES})

# ==============================================================================
# プラグイン生成関数 (関数化して使い回す)
# ==============================================================================
function(create_cyross_plugin TARGET_NAME)
    set(CY_PLUGIN_CODE "2686")

    juce_add_plugin(${TARGET_NAME}
        COMPANY_NAME ${CY_COMPANY_NAME}
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE ${CY_MANUFACTURER_CODE}
        PLUGIN_CODE ${CY_PLUGIN_CODE}
        FORMATS VST3 Standalone
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
        PRODUCT_NAME ${TARGET_NAME}
    )

    target_sources(${TARGET_NAME} PRIVATE
        ${CONST_FILES}
        ${SYNTH_FILES}
        ${FM_FILES}
        ${GUI_FILES}
        ${PROCESSOR_FILES}
        ${EDITOR_FILES}
        ${OPNA_SYNTH_FILES}
        ${OPN_SYNTH_FILES}
        ${OPL_SYNTH_FILES}
        ${OPL3_SYNTH_FILES}
        ${OPM_SYNTH_FILES}
        ${OPZX3_SYNTH_FILES}
        ${OPNA_OPERATOR_FILES}
        ${OPN_OPERATOR_FILES}
        ${OPL_OPERATOR_FILES}
        ${OPL3_OPERATOR_FILES}
        ${OPM_OPERATOR_FILES}
        ${OPZX3_OPERATOR_FILES}
        ${SSG_SYNTH_FILES}
        ${WT_SYNTH_FILES}
        ${RHYTHM_SYNTH_FILES}
        ${ADPCM_SYNTH_FILES}
        ${BEEP_SYNTH_FILES}
        ${FX_EFFECT_FILES}
        ${OPNA_PROCESSOR_FILES}
        ${OPN_PROCESSOR_FILES}
        ${OPL_PROCESSOR_FILES}
        ${OPL3_PROCESSOR_FILES}
        ${OPM_PROCESSOR_FILES}
        ${OPZX3_PROCESSOR_FILES}
        ${SSG_PROCESSOR_FILES}
        ${WT_PROCESSOR_FILES}
        ${RHYTHM_PROCESSOR_FILES}
        ${ADPCM_PROCESSOR_FILES}
        ${BEEP_PROCESSOR_FILES}
        ${FX_PROCESSOR_FILES}
        ${OPNA_GUI_FILES}
        ${OPN_GUI_FILES}
        ${OPL_GUI_FILES}
        ${OPL3_GUI_FILES}
        ${OPM_GUI_FILES}
        ${OPZX3_GUI_FILES}
        ${SSG_GUI_FILES}
        ${WT_GUI_FILES}
        ${RHYTHM_GUI_FILES}
        ${ADPCM_GUI_FILES}
        ${BEEP_GUI_FILES}
        ${FX_GUI_FILES}
        ${PRESET_GUI_FILES}
        ${SETTINGS_GUI_FILES}
        ${ABOUT_GUI_FILES}
        ${DOCUMENT_FILES}
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        AppIconForAbout
        VstLogoForAbout
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_dsp
    )

    target_compile_definitions(${TARGET_NAME} PRIVATE JUCE_VST3_CAN_REPLACE_VST2=0)

    juce_generate_juce_header(${TARGET_NAME})
endfunction()

create_cyross_plugin("2686V")
