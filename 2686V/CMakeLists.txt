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

set(OPNA_FILES
    "Source/OPNA/GuiOpna.h"
    "Source/OPNA/GuiOpna.cpp"
    "Source/OPNA/OpnaCore.h"
    "Source/OPNA/OpnaCore.cpp"
    "Source/OPNA/OpnaOperator.h"
    "Source/OPNA/OpnaOperator.cpp"
    "Source/OPNA/OpnaParams.h"
    "Source/OPNA/PrOpna.h"
    "Source/OPNA/PrOpna.cpp"
)

set(OPN_FILES
    "Source/OPN/GuiOpn.h"
    "Source/OPN/GuiOpn.cpp"
    "Source/OPN/OpnCore.h"
    "Source/OPN/OpnCore.cpp"
    "Source/OPN/OpnOperator.h"
    "Source/OPN/OpnOperator.cpp"
    "Source/OPN/OpnParams.h"
    "Source/OPN/PrOpn.h"
    "Source/OPN/PrOpn.cpp"
)

set(OPL_FILES
    "Source/OPL/GuiOpl.h"
    "Source/OPL/GuiOpl.cpp"
    "Source/OPL/OplCore.h"
    "Source/OPL/OplCore.cpp"
    "Source/OPL/OplOperator.h"
    "Source/OPL/OplOperator.cpp"
    "Source/OPL/OplParams.h"
    "Source/OPL/PrOpl.h"
    "Source/OPL/PrOpl.cpp"
)

set(OPL3_FILES
    "Source/OPL3/GuiOpl3.h"
    "Source/OPL3/GuiOpl3.cpp"
    "Source/OPL3/Opl3Core.h"
    "Source/OPL3/Opl3Core.cpp"
    "Source/OPL3/Opl3Operator.h"
    "Source/OPL3/Opl3Operator.cpp"
    "Source/OPL3/Opl3Params.h"
    "Source/OPL3/PrOpl3.h"
    "Source/OPL3/PrOpl3.cpp"
)

set(OPM_FILES
    "Source/OPM/GuiOpm.h"
    "Source/OPM/GuiOpm.cpp"
    "Source/OPM/OpmCore.h"
    "Source/OPM/OpmCore.cpp"
    "Source/OPM/OpmOperator.h"
    "Source/OPM/OpmOperator.cpp"
    "Source/OPM/OpmParams.h"
    "Source/OPM/PrOpm.h"
    "Source/OPM/PrOpm.cpp"
)

set(OPZX3_FILES
    "Source/OPZX3/GuiOpzx3.h"
    "Source/OPZX3/GuiOpzx3.cpp"
    "Source/OPZX3/Opzx3Core.h"
    "Source/OPZX3/Opzx3Core.cpp"
    "Source/OPZX3/Opzx3Operator.h"
    "Source/OPZX3/Opzx3Operator.cpp"
    "Source/OPZX3/Opzx3Params.h"
    "Source/OPZX3/PrOpzx3.h"
    "Source/OPZX3/PrOpzx3.cpp"
)

set(SSG_FILES
    "Source/SSG/GuiSsg.h"
    "Source/SSG/GuiSsg.cpp"
    "Source/SSG/PrSsg.h"
    "Source/SSG/PrSsg.cpp"
    "Source/SSG/SsgCore.h"
    "Source/SSG/SsgCore.cpp"
    "Source/SSG/SsgParams.h"
)

set(WT_FILES
    "Source/WT/GuiWt.h"
    "Source/WT/GuiWt.cpp"
    "Source/WT/PrWt.h"
    "Source/WT/PrWt.cpp"
    "Source/WT/WtCore.h"
    "Source/WT/WtCore.cpp"
    "Source/WT/WtParams.h"
)

set(RHYTHM_FILES
    "Source/RHYTHM/GuiRhythm.h"
    "Source/RHYTHM/GuiRhythm.cpp"
    "Source/RHYTHM/PrRhythm.h"
    "Source/RHYTHM/PrRhythm.cpp"
    "Source/RHYTHM/RhythmCore.h"
    "Source/RHYTHM/RhythmCore.cpp"
    "Source/RHYTHM/RhythmParams.h"
)

set(ADPCM_FILES
    "Source/ADPCM/AdpcmCore.h"
    "Source/ADPCM/AdpcmCore.cpp"
    "Source/ADPCM/AdpcmParams.h"
    "Source/ADPCM/GuiAdpcm.h"
    "Source/ADPCM/GuiAdpcm.cpp"
    "Source/ADPCM/PrAdpcm.h"
    "Source/ADPCM/PrAdpcm.cpp"
)

set(BEEP_FILES
    "Source/BEEP/GuiBeep.h"
    "Source/BEEP/GuiBeep.cpp"
    "Source/BEEP/BeepCore.h"
    "Source/BEEP/BeepCore.cpp"
    "Source/BEEP/BeepParams.h"
    "Source/BEEP/PrBeep.h"
    "Source/BEEP/PrBeep.cpp"
)

set(FX_FILES
    "Source/FX/FxCore.h"
    "Source/FX/FxCore.cpp"
    "Source/FX/FxParams.h"
    "Source/FX/GuiFx.h"
    "Source/FX/GuiFx.cpp"
    "Source/FX/PrFx.h"
    "Source/FX/PrFx.cpp"
)

set(PRESET_FILES
    "Source/PRESET/GuiPreset.h"
    "Source/PRESET/GuiPreset.cpp"
)

set(SETTINGS_FILES
    "Source/SETTINGS/GuiSettings.h"
    "Source/SETTINGS/GuiSettings.cpp"
)

set(ABOUT_FILES
    "Source/ABOUT/GuiAbout.h"
    "Source/ABOUT/GuiAbout.cpp"
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
source_group("Tab\\OPNA" FILES ${OPNA_FILES})
source_group("Tab\\OPN" FILES ${OPN_FILES})
source_group("Tab\\OPL" FILES ${OPL_FILES})
source_group("Tab\\OPL3" FILES ${OPL3_FILES})
source_group("Tab\\OPM" FILES ${OPM_FILES})
source_group("Tab\\OPZX3" FILES ${OPZX3_FILES})
source_group("Tab\\SSG" FILES ${SSG_FILES})
source_group("Tab\\WaveTable" FILES ${WT_FILES})
source_group("Tab\\RHYTHM" FILES ${RHYTHM_FILES})
source_group("Tab\\ADPCM" FILES ${ADPCM_FILES})
source_group("Tab\\BEEP" FILES ${BEEP_FILES})
source_group("Tab\\PRESET" FILES ${PRESET_FILES})
source_group("Tab\\FX" FILES ${FX_FILES})
source_group("Tab\\SETTINGS" FILES ${SETTINGS_FILES})
source_group("Tab\\ABOUT" FILES ${ABOUT_FILES})

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
        ${OPNA_FILES}
        ${OPN_FILES}
        ${OPL_FILES}
        ${OPL3_FILES}
        ${OPM_FILES}
        ${OPZX3_FILES}
        ${SSG_FILES}
        ${WT_FILES}
        ${RHYTHM_FILES}
        ${ADPCM_FILES}
        ${BEEP_FILES}
        ${FX_FILES}
        ${PRESET_FILES}
        ${SETTINGS_FILES}
        ${ABOUT_FILES}
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
