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
set(CORE_FILES
    "Source/core/Global.h"
    "Source/core/FileValues.h"
    "Source/core/MmlKeys.h"
    "Source/core/MmlValues.h"
    "Source/core/PrKeys.h"
    "Source/core/PrNames.h"
    "Source/core/PrValues.h"
    "Source/core/PresetKeys.h"
    "Source/core/PresetValues.h"
    "Source/core/SettingsKeys.h"
    "Source/core/SettingsValues.h"
)

set(SYNTH_FILES
    "Source/synth/Mode.h"
    "Source/synth/Mode.cpp"
    "Source/synth/Pcm.h"
    "Source/synth/Pcm.cpp"
    "Source/synth/SynthHelpers.h"
    "Source/synth/SynthHelpers.cpp"
    "Source/synth/SynthParams.h"
    "Source/synth/SynthVoice.h"
    "Source/synth/SynthVoice.cpp"
)

set(FM_FILES
    "Source/fm/FmCommon.h"
    "Source/fm/FmCommon.cpp"
    "Source/fm/RegisterType.h"
    "Source/fm/RegisterConverter.h"
    "Source/fm/RegisterConverter.cpp"
    "Source/fm/SliderRegMap.h"
    "Source/fm/SliderRegMap.cpp"
)

set(GUI_FILES
    "Source/gui/GuiBase.h"
    "Source/gui/GuiBase.cpp"
    "Source/gui/GuiColor.h"
    "Source/gui/GuiComponents.h"
    "Source/gui/GuiComponents.cpp"
    "Source/gui/GuiContext.h"
    "Source/gui/GuiHelpers.h"
    "Source/gui/GuiHelpers.cpp"
    "Source/gui/GuiLF.h"
    "Source/gui/GuiLF.cpp"
    "Source/gui/GuiStructs.h"
    "Source/gui/GuiText.h"
    "Source/gui/GuiValues.h"
)

set(OPNA_FILES
    "Source/OPNA/GuiOpna.h"
    "Source/OPNA/GuiOpna.cpp"
    "Source/OPNA/OpnaCore.h"
    "Source/OPNA/OpnaCore.cpp"
    "Source/OPNA/PrOpna.h"
    "Source/OPNA/PrOpna.cpp"
)

set(OPN_FILES
    "Source/OPN/GuiOpn.h"
    "Source/OPN/GuiOpn.cpp"
    "Source/OPN/OpnCore.h"
    "Source/OPN/OpnCore.cpp"
    "Source/OPN/PrOpn.h"
    "Source/OPN/PrOpn.cpp"
)

set(OPL_FILES
    "Source/OPL/GuiOpl.h"
    "Source/OPL/GuiOpl.cpp"
    "Source/OPL/OplCore.h"
    "Source/OPL/OplCore.cpp"
    "Source/OPL/PrOpl.h"
    "Source/OPL/PrOpl.cpp"
)

set(OPL3_FILES
    "Source/OPL3/GuiOpl3.h"
    "Source/OPL3/GuiOpl3.cpp"
    "Source/OPL3/Opl3Core.h"
    "Source/OPL3/Opl3Core.cpp"
    "Source/OPL3/PrOpl3.h"
    "Source/OPL3/PrOpl3.cpp"
)

set(OPM_FILES
    "Source/OPM/GuiOpm.h"
    "Source/OPM/GuiOpm.cpp"
    "Source/OPM/OpmCore.h"
    "Source/OPM/OpmCore.cpp"
    "Source/OPM/PrOpm.h"
    "Source/OPM/PrOpm.cpp"
)

set(OPZX3_FILES
    "Source/OPZX3/GuiOpzx3.h"
    "Source/OPZX3/GuiOpzx3.cpp"
    "Source/OPZX3/Opzx3Core.h"
    "Source/OPZX3/Opzx3Core.cpp"
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
)

set(WT_FILES
    "Source/WT/GuiWt.h"
    "Source/WT/GuiWt.cpp"
    "Source/WT/PrWt.h"
    "Source/WT/PrWt.cpp"
    "Source/WT/WtCore.h"
    "Source/WT/WtCore.cpp"
)

set(RHYTHM_FILES
    "Source/RHYTHM/GuiRhythm.h"
    "Source/RHYTHM/GuiRhythm.cpp"
    "Source/RHYTHM/PrRhythm.h"
    "Source/RHYTHM/PrRhythm.cpp"
    "Source/RHYTHM/RhythmCore.h"
    "Source/RHYTHM/RhythmCore.cpp"
)

set(ADPCM_FILES
    "Source/ADPCM/AdpcmCore.h"
    "Source/ADPCM/AdpcmCore.cpp"
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
    "Source/BEEP/PrBeep.h"
    "Source/BEEP/PrBeep.cpp"
)

set(FX_FILES
    "Source/FX/FxCore.h"
    "Source/FX/FxCore.cpp"
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

set(PROCESSOR_FILES
    "Source/processor/PrBase.h"
    "Source/processor/PrBase.cpp"
    "Source/processor/PluginProcessor.h"
    "Source/processor/PluginProcessor.cpp"
)

set(EDITOR_FILES
    "Source/editor/PluginEditor.h"
    "Source/editor/PluginEditor.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("[Core01]Main" FILES ${CORE_FILES})
source_group("[Core02]Synth" FILES ${SYNTH_FILES})
source_group("[Core03]FM" FILES ${FM_FILES})
source_group("[Core04]Gui" FILES ${GUI_FILES})
source_group("[Core05]Processor" FILES ${PROCESSOR_FILES})
source_group("[Core06]Editor" FILES ${EDITOR_FILES})
source_group("[Core07]Docs" FILES ${DOCUMENT_FILES})
source_group("[TAB 01]OPNA" FILES ${OPNA_FILES})
source_group("[TAB 02]OPN" FILES ${OPN_FILES})
source_group("[TAB 03]OPL" FILES ${OPL_FILES})
source_group("[TAB 04]OPL3" FILES ${OPL3_FILES})
source_group("[TAB 05]OPM" FILES ${OPM_FILES})
source_group("[TAB 06]OPZX3" FILES ${OPZX3_FILES})
source_group("[TAB 07]SSG" FILES ${SSG_FILES})
source_group("[TAB 08]WT" FILES ${WT_FILES})
source_group("[TAB 09]RHYTHM" FILES ${RHYTHM_FILES})
source_group("[TAB 10]ADPCM" FILES ${ADPCM_FILES})
source_group("[TAB 11]BEEP" FILES ${BEEP_FILES})
source_group("[TAB 12]PRESET" FILES ${PRESET_FILES})
source_group("[TAB 13]FX" FILES ${FX_FILES})
source_group("[TAB 14]SETTINGS" FILES ${SETTINGS_FILES})
source_group("[TAB 15]ABOUT" FILES ${ABOUT_FILES})

# ==============================================================================
# プラグイン生成関数 (関数化して使い回す)
# ==============================================================================
function(create_cyross_plugin TARGET_NAME IS_FX)
    if(IS_FX)
        set(CY_PLUGIN_CODE "28FX")
        set(CY_IS_SYNTH FALSE)
    else()
        set(CY_PLUGIN_CODE "2686")
        set(CY_IS_SYNTH TRUE)
    endif()

    juce_add_plugin(${TARGET_NAME}
        COMPANY_NAME ${CY_COMPANY_NAME}
        IS_SYNTH ${CY_IS_SYNTH}
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

    # ソースコードの紐付け (FXかどうかで混ぜるファイルを分ける)
    if(IS_FX)
        target_compile_definitions(${TARGET_NAME} PUBLIC BUILD_AS_FX_PLUGIN=1)
        target_sources(${TARGET_NAME} PRIVATE
            ${CORE_FILES}
            ${SYNTH_FILES}
            ${GUI_FILES}
            ${PROCESSOR_FILES}
            ${EDITOR_FILES}
            ${FX_FILES}
            ${SETTINGS_FILES}
            ${ABOUT_FILES}
            ${DOCUMENT_FILES}
        )
    else()
        target_sources(${TARGET_NAME} PRIVATE
            ${CORE_FILES}
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
    endif()

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

create_cyross_plugin("2686V" FALSE)
create_cyross_plugin("2686VFX" TRUE)
