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
)

set(SYNTH_FILES
    "Source/Core/Synth/Mode.h"
    "Source/Core/Synth/Mode.cpp"
    "Source/Core/Synth/SynthCore.h"
    "Source/Core/Synth/SynthCore.cpp"
    "Source/Core/Synth/SynthParams.h"
    "Source/Core/Synth/SynthVoice.h"
    "Source/Core/Synth/SynthVoice.cpp"
    "Source/Core/Synth/SynthHelpers.h"
    "Source/Core/Synth/SynthHelpers.cpp"
)

set(FM_BASIC_ALG_FILES
    "Source/Generator/Fm/Alg/Basic/Core.h"
    "Source/Generator/Fm/Alg/Basic/Core.cpp"
)

set(FM_OPL_ALG_FILES
    "Source/Generator/Fm/Alg/Opl/Core.h"
    "Source/Generator/Fm/Alg/Opl/Core.cpp"
)

set(FM_OPL3_ALG_FILES
    "Source/Generator/Fm/Alg/Opl3/Core.h"
    "Source/Generator/Fm/Alg/Opl3/Core.cpp"
)

set(FM_OPZX7_ALG_FILES
    "Source/Generator/Fm/Alg/Opzx7/Core.h"
    "Source/Generator/Fm/Alg/Opzx7/Core.cpp"
)

set(FM_FIX_FILES
    "Source/Generator/Fm/Fix/Params.h"
    "Source/Generator/Fm/Fix/Core.h"
    "Source/Generator/Fm/Fix/Core.cpp"
)

set(LFSR_NOISE_GEN_FILES
    "Source/Generator/Noise/Lfsr/Core.h"
    "Source/Generator/Noise/Lfsr/Core.cpp"
)

set(SSG_NOISE_GEN_FILES
    "Source/Generator/Noise/Ssg/Core.h"
    "Source/Generator/Noise/Ssg/Core.cpp"
)

set(PCM_GENERATOR_FILES
    "Source/Generator/Pcm/Core.h"
    "Source/Generator/Pcm/Core.cpp"
)

set(FM_FILES
    "Source/Core/Fm/FmClipboard.h"
    "Source/Core/Fm/FmClipboard.cpp"
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
    "Source/Core/Fm/FmMmlFormatter.h"
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
    "Source/Core/Processor/Keys.h"
    "Source/Core/Processor/Names.h"
    "Source/Core/Processor/Values.h"
    "Source/Core/Processor/Base.h"
    "Source/Core/Processor/Base.cpp"
    "Source/Core/Processor/PluginProcessor.h"
    "Source/Core/Processor/PluginProcessor.cpp"
)

set(EDITOR_FILES
    "Source/Core/Editor/PluginEditor.h"
    "Source/Core/Editor/PluginEditor.cpp"
)

set(AMP_ADSR_ENVELOPE_FILES
    "Source/Effect/Envelope/Amp/Adsr/Params.h"
    "Source/Effect/Envelope/Amp/Adsr/Core.h"
    "Source/Effect/Envelope/Amp/Adsr/Core.cpp"
)

set(PITCH_ADSR_ENVELOPE_FILES
    "Source/Effect/Envelope/Pitch/Adsr/Params.h"
    "Source/Effect/Envelope/Pitch/Adsr/Core.h"
    "Source/Effect/Envelope/Pitch/Adsr/Core.cpp"
)

set(OPN_DETUNE_FILES
    "Source/Effect/Detune/Opn/Core.h"
    "Source/Effect/Detune/Opn/Core.cpp"
)

set(OPM_DETUNE_FILES
    "Source/Effect/Detune/Opm/Core.h"
    "Source/Effect/Detune/Opm/Core.cpp"
)

set(OPZX7_DETUNE_FILES
    "Source/Effect/Detune/Opzx7/Core.h"
    "Source/Effect/Detune/Opzx7/Core.cpp"
)

set(OPNA_SYNTH_FILES
    "Source/Synth/Opna/Core.h"
    "Source/Synth/Opna/Core.cpp"
    "Source/Synth/Opna/Params.h"
)

set(OPNA_OPERATOR_FILES
    "Source/Synth/Opna/Operator/Core.h"
    "Source/Synth/Opna/Operator/Core.cpp"
)

set(OPNA_PROCESSOR_FILES
    "Source/Processor/Opna/Keys.h"
    "Source/Processor/Opna/Names.h"
    "Source/Processor/Opna/Values.h"
    "Source/Processor/Opna/Core.h"
    "Source/Processor/Opna/Core.cpp"
)

set(OPNA_GUI_FILES
    "Source/Gui/Opna/GuiText.h"
    "Source/Gui/Opna/GuiValues.h"
    "Source/Gui/Opna/Core.h"
    "Source/Gui/Opna/Core.cpp"
)

set(OPN_SYNTH_FILES
    "Source/Synth/Opn/Core.h"
    "Source/Synth/Opn/Core.cpp"
    "Source/Synth/Opn/Params.h"
)

set(OPN_OPERATOR_FILES
    "Source/Synth/Opn/Operator/Core.h"
    "Source/Synth/Opn/Operator/Core.cpp"
)

set(OPN_PROCESSOR_FILES
    "Source/Processor/Opn/Keys.h"
    "Source/Processor/Opn/Names.h"
    "Source/Processor/Opn/Values.h"
    "Source/Processor/Opn/Core.h"
    "Source/Processor/Opn/Core.cpp"
)

set(OPN_GUI_FILES
    "Source/Gui/Opn/GuiText.h"
    "Source/Gui/Opn/GuiValues.h"
    "Source/Gui/Opn/Core.h"
    "Source/Gui/Opn/Core.cpp"
)

set(OPL_SYNTH_FILES
    "Source/Synth/Opl/Core.h"
    "Source/Synth/Opl/Core.cpp"
    "Source/Synth/Opl/Params.h"
)

set(OPL_OPERATOR_FILES
    "Source/Synth/Opl/Operator/Core.h"
    "Source/Synth/Opl/Operator/Core.cpp"
)

set(OPL_PROCESSOR_FILES
    "Source/Processor/Opl/Keys.h"
    "Source/Processor/Opl/Names.h"
    "Source/Processor/Opl/Values.h"
    "Source/Processor/Opl/Core.h"
    "Source/Processor/Opl/Core.cpp"
)

set(OPL_GUI_FILES
    "Source/Gui/Opl/GuiText.h"
    "Source/Gui/Opl/GuiValues.h"
    "Source/Gui/Opl/Core.h"
    "Source/Gui/Opl/Core.cpp"
)

set(OPL3_SYNTH_FILES
    "Source/Synth/Opl3/Core.h"
    "Source/Synth/Opl3/Core.cpp"
    "Source/Synth/Opl3/Params.h"
)

set(OPL3_OPERATOR_FILES
    "Source/Synth/Opl3/Operator/Core.h"
    "Source/Synth/Opl3/Operator/Core.cpp"
)

set(OPL3_PROCESSOR_FILES
    "Source/Processor/Opl3/Keys.h"
    "Source/Processor/Opl3/Names.h"
    "Source/Processor/Opl3/Values.h"
    "Source/Processor/Opl3/Core.h"
    "Source/Processor/Opl3/Core.cpp"
)

set(OPL3_GUI_FILES
    "Source/Gui/Opl3/GuiText.h"
    "Source/Gui/Opl3/GuiValues.h"
    "Source/Gui/Opl3/Core.h"
    "Source/Gui/Opl3/Core.cpp"
)

set(OPM_SYNTH_FILES
    "Source/Synth/Opm/Core.h"
    "Source/Synth/Opm/Core.cpp"
    "Source/Synth/Opm/Params.h"
)

set(OPM_OPERATOR_FILES
    "Source/Synth/Opm/Operator/Core.h"
    "Source/Synth/Opm/Operator/Core.cpp"
)

set(OPM_PROCESSOR_FILES
    "Source/Processor/Opm/Keys.h"
    "Source/Processor/Opm/Names.h"
    "Source/Processor/Opm/Values.h"
    "Source/Processor/Opm/Core.h"
    "Source/Processor/Opm/Core.cpp"
)

set(OPM_GUI_FILES
    "Source/Gui/Opm/GuiText.h"
    "Source/Gui/Opm/GuiValues.h"
    "Source/Gui/Opm/Core.h"
    "Source/Gui/Opm/Core.cpp"
)

set(OPZX7_SYNTH_FILES
    "Source/Synth/Opzx7/Core.h"
    "Source/Synth/Opzx7/Core.cpp"
    "Source/Synth/Opzx7/Params.h"
)

set(OPZX7_OPERATOR_FILES
    "Source/Synth/Opzx7/Operator/Core.h"
    "Source/Synth/Opzx7/Operator/Core.cpp"
)

set(OPZX7_PROCESSOR_FILES
    "Source/Processor/Opzx7/Keys.h"
    "Source/Processor/Opzx7/Names.h"
    "Source/Processor/Opzx7/Values.h"
    "Source/Processor/Opzx7/Core.h"
    "Source/Processor/Opzx7/Core.cpp"
)

set(OPZX7_GUI_FILES
    "Source/Gui/Opzx7/GuiText.h"
    "Source/Gui/Opzx7/GuiValues.h"
    "Source/Gui/Opzx7/Core.h"
    "Source/Gui/Opzx7/Core.cpp"
)

set(SSG_SYNTH_FILES
    "Source/Synth/Ssg/Core.h"
    "Source/Synth/Ssg/Core.cpp"
    "Source/Synth/Ssg/Params.h"
)

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/Keys.h"
    "Source/Processor/Ssg/Names.h"
    "Source/Processor/Ssg/Values.h"
    "Source/Processor/Ssg/Core.h"
    "Source/Processor/Ssg/Core.cpp"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/GuiText.h"
    "Source/Gui/Ssg/GuiValues.h"
    "Source/Gui/Ssg/Core.h"
    "Source/Gui/Ssg/Core.cpp"
)

set(WT_SYNTH_FILES
    "Source/Synth/Wavetable/Core.h"
    "Source/Synth/Wavetable/Core.cpp"
    "Source/Synth/Wavetable/Params.h"
)

set(WT_PROCESSOR_FILES
    "Source/Processor/Wavetable/Keys.h"
    "Source/Processor/Wavetable/Names.h"
    "Source/Processor/Wavetable/Values.h"
    "Source/Processor/Wavetable/Core.h"
    "Source/Processor/Wavetable/Core.cpp"
)

set(WT_GUI_FILES
    "Source/Gui/Wavetable/GuiText.h"
    "Source/Gui/Wavetable/GuiValues.h"
    "Source/Gui/Wavetable/Core.h"
    "Source/Gui/Wavetable/Core.cpp"
)

set(RHYTHM_SYNTH_FILES
    "Source/Synth/Rhythm/Core.h"
    "Source/Synth/Rhythm/Core.cpp"
    "Source/Synth/Rhythm/Params.h"
)

set(RHYTHM_PROCESSOR_FILES
    "Source/Processor/Rhythm/Keys.h"
    "Source/Processor/Rhythm/Names.h"
    "Source/Processor/Rhythm/Values.h"
    "Source/Processor/Rhythm/Core.h"
    "Source/Processor/Rhythm/Core.cpp"
)

set(RHYTHM_GUI_FILES
    "Source/Gui/Rhythm/GuiText.h"
    "Source/Gui/Rhythm/GuiValues.h"
    "Source/Gui/Rhythm/Core.h"
    "Source/Gui/Rhythm/Core.cpp"
)

set(ADPCM_SYNTH_FILES
    "Source/Synth/Adpcm/Core.h"
    "Source/Synth/Adpcm/Core.cpp"
    "Source/Synth/Adpcm/Params.h"
)

set(ADPCM_PROCESSOR_FILES
    "Source/Processor/Adpcm/Keys.h"
    "Source/Processor/Adpcm/Names.h"
    "Source/Processor/Adpcm/Values.h"
    "Source/Processor/Adpcm/Core.h"
    "Source/Processor/Adpcm/Core.cpp"
)

set(ADPCM_GUI_FILES
    "Source/Gui/Adpcm/GuiText.h"
    "Source/Gui/Adpcm/GuiValues.h"
    "Source/Gui/Adpcm/Core.h"
    "Source/Gui/Adpcm/Core.cpp"
)

set(BEEP_SYNTH_FILES
    "Source/Synth/Beep/Core.h"
    "Source/Synth/Beep/Core.cpp"
    "Source/Synth/Beep/Params.h"
)

set(BEEP_PROCESSOR_FILES
    "Source/Processor/Beep/Keys.h"
    "Source/Processor/Beep/Names.h"
    "Source/Processor/Beep/Values.h"
    "Source/Processor/Beep/Core.h"
    "Source/Processor/Beep/Core.cpp"
)

set(BEEP_GUI_FILES
    "Source/Gui/Beep/GuiText.h"
    "Source/Gui/Beep/GuiValues.h"
    "Source/Gui/Beep/Core.h"
    "Source/Gui/Beep/Core.cpp"
)

set(FX_EFFECT_FILES
    "Source/Effect/Fx/Core.h"
    "Source/Effect/Fx/Core.cpp"
)

set(FX_PROCESSOR_FILES
    "Source/Processor/Fx/Keys.h"
    "Source/Processor/Fx/Names.h"
    "Source/Processor/Fx/Values.h"
    "Source/Processor/Fx/Core.h"
    "Source/Processor/Fx/Core.cpp"
)

set(FX_GUI_FILES
    "Source/Gui/Fx/GuiText.h"
    "Source/Gui/Fx/GuiValues.h"
    "Source/Gui/Fx/Core.h"
    "Source/Gui/Fx/Core.cpp"
)

set(PRESET_GUI_FILES
    "Source/Gui/Preset/GuiText.h"
    "Source/Gui/Preset/GuiValues.h"
    "Source/Gui/Preset/Keys.h"
    "Source/Gui/Preset/Values.h"
    "Source/Gui/Preset/Core.h"
    "Source/Gui/Preset/Core.cpp"
)

set(SETTINGS_GUI_FILES
    "Source/Gui/Settings/Keys.h"
    "Source/Gui/Settings/Values.h"
    "Source/Gui/Settings/Core.h"
    "Source/Gui/Settings/Core.cpp"
)

set(ABOUT_GUI_FILES
    "Source/Gui/About/GuiText.h"
    "Source/Gui/About/GuiValues.h"
    "Source/Gui/About/Core.h"
    "Source/Gui/About/Core.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("2686V\\Constants" FILES ${CONST_FILES})
source_group("2686V\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686V\\Generator\\Pcm" FILES ${PCM_GENERATOR_FILES})
source_group("2686V\\Generator\\Noise\\Lfsr" FILES ${LFSR_NOISE_GEN_FILES})
source_group("2686V\\Generator\\Noise\\Ssg" FILES ${SSG_NOISE_GEN_FILES})
source_group("2686V\\Generator\\Fm" FILES ${FM_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Basic" FILES ${FM_BASIC_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Opl" FILES ${FM_OPL_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Opl3" FILES ${FM_OPL3_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Opzx7" FILES ${FM_OPZX7_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Fix" FILES ${FM_FIX_FILES})
source_group("2686V\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686V\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686V\\Synth\\Opna" FILES ${OPNA_SYNTH_FILES})
source_group("2686V\\Synth\\Opn" FILES ${OPN_SYNTH_FILES})
source_group("2686V\\Synth\\Opl" FILES ${OPL_SYNTH_FILES})
source_group("2686V\\Synth\\Opl3" FILES ${OPL3_SYNTH_FILES})
source_group("2686V\\Synth\\Opm" FILES ${OPM_SYNTH_FILES})
source_group("2686V\\Synth\\Opzx7" FILES ${OPZX7_SYNTH_FILES})
source_group("2686V\\Synth\\Opna\\Operator" FILES ${OPNA_OPERATOR_FILES})
source_group("2686V\\Synth\\Opn\\Operator" FILES ${OPN_OPERATOR_FILES})
source_group("2686V\\Synth\\Opl\\Operator" FILES ${OPL_OPERATOR_FILES})
source_group("2686V\\Synth\\Opl3\\Operator" FILES ${OPL3_OPERATOR_FILES})
source_group("2686V\\Synth\\Opm\\Operator" FILES ${OPM_OPERATOR_FILES})
source_group("2686V\\Synth\\Opzx7\\Operator" FILES ${OPZX7_OPERATOR_FILES})
source_group("2686V\\Synth\\Ssg" FILES ${SSG_SYNTH_FILES})
source_group("2686V\\Synth\\Wavetable" FILES ${WT_SYNTH_FILES})
source_group("2686V\\Synth\\Rhythm" FILES ${RHYTHM_SYNTH_FILES})
source_group("2686V\\Synth\\Adpcm" FILES ${ADPCM_SYNTH_FILES})
source_group("2686V\\Synth\\Beep" FILES ${BEEP_SYNTH_FILES})
source_group("2686V\\Effect\\Envelope\\Amp\\Adsr" FILES ${AMP_ADSR_ENVELOPE_FILES})
source_group("2686V\\Effect\\Envelope\\Pitch\\Adsr" FILES ${PITCH_ADSR_ENVELOPE_FILES})
source_group("2686V\\Effect\\Detune\\Opm" FILES ${OPM_DETUNE_FILES})
source_group("2686V\\Effect\\Detune\\Opn" FILES ${OPN_DETUNE_FILES})
source_group("2686V\\Effect\\Detune\\Opzx7" FILES ${OPZX7_DETUNE_FILES})
source_group("2686V\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686V\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686V\\Processor\\Opna" FILES ${OPNA_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opn" FILES ${OPN_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl" FILES ${OPL_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl3" FILES ${OPL3_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opm" FILES ${OPM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opzx7" FILES ${OPZX7_PROCESSOR_FILES})
source_group("2686V\\Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("2686V\\Processor\\Wavetable" FILES ${WT_PROCESSOR_FILES})
source_group("2686V\\Processor\\Rhythm" FILES ${RHYTHM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Adpcm" FILES ${ADPCM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Beep" FILES ${BEEP_PROCESSOR_FILES})
source_group("2686V\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("2686V\\Gui\\Core" FILES ${GUI_FILES})
source_group("2686V\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("2686V\\Gui\\Tab\\Opna" FILES ${OPNA_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opn" FILES ${OPN_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opl" FILES ${OPL_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opl3" FILES ${OPL3_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opm" FILES ${OPM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opzx7" FILES ${OPZX7_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Ssg" FILES ${SSG_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Wavetable" FILES ${WT_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Beep" FILES ${BEEP_GUI_FILES})
source_group("2686V\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})

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

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
    endif()

    target_sources(${TARGET_NAME} PRIVATE
        ${CONST_FILES}
        ${SYNTH_FILES}
        ${FM_BASIC_ALG_FILES}
        ${FM_OPL_ALG_FILES}
        ${FM_OPL3_ALG_FILES}
        ${FM_OPZX7_ALG_FILES}
        ${FM_FIX_FILES}
        ${LFSR_NOISE_GEN_FILES}
        ${SSG_NOISE_GEN_FILES}
        ${PCM_GENERATOR_FILES}
        ${FM_FILES}
        ${GUI_FILES}
        ${PROCESSOR_FILES}
        ${EDITOR_FILES}
        ${AMP_ADSR_ENVELOPE_FILES}
        ${PITCH_ADSR_ENVELOPE_FILES}
        ${OPM_DETUNE_FILES}
        ${OPN_DETUNE_FILES}
        ${OPZX7_DETUNE_FILES}
        ${OPNA_SYNTH_FILES}
        ${OPN_SYNTH_FILES}
        ${OPL_SYNTH_FILES}
        ${OPL3_SYNTH_FILES}
        ${OPM_SYNTH_FILES}
        ${OPZX7_SYNTH_FILES}
        ${OPNA_OPERATOR_FILES}
        ${OPN_OPERATOR_FILES}
        ${OPL_OPERATOR_FILES}
        ${OPL3_OPERATOR_FILES}
        ${OPM_OPERATOR_FILES}
        ${OPZX7_OPERATOR_FILES}
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
        ${OPZX7_PROCESSOR_FILES}
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
        ${OPZX7_GUI_FILES}
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
