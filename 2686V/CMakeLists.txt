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

set(EDITOR_FILES
    "Source/Core/Editor/PluginEditor.h"
    "Source/Core/Editor/PluginEditor.cpp"
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

set(FM_SAMPLE_CORE_FILES
    "Source/Core/Sample/Fm/Core.h"
    "Source/Core/Sample/Fm/Core.cpp"
)

set(SSG_SAMPLE_CORE_FILES
    "Source/Core/Sample/Ssg/Core.h"
    "Source/Core/Sample/Ssg/Core.cpp"
)

set(SYNTH_FILES
    "Source/Core/Synth/Mode.h"
    "Source/Core/Synth/SynthParams.h"
    "Source/Core/Synth/SynthCore.h"
    "Source/Core/Synth/SynthVoice.h"
    "Source/Core/Synth/SynthVoice.cpp"
    "Source/Core/Synth/SynthHelpers.h"
    "Source/Core/Synth/SynthHelpers.cpp"
)

set(FM_FILES
    "Source/Generator/Fm/RegisterType.h"
    "Source/Generator/Fm/RegisterConverter.h"
    "Source/Generator/Fm/RegisterConverter.cpp"
    "Source/Generator/Fm/SliderRegMap.h"
    "Source/Generator/Fm/SliderRegMap.cpp"
)

set(FM_BASIC_ALG_FILES
    "Source/Generator/Fm/Alg/Basic/Alg.h"
    "Source/Generator/Fm/Alg/Basic/Alg.cpp"
)

set(FM_OPL_ALG_FILES
    "Source/Generator/Fm/Alg/Opl/Alg.h"
    "Source/Generator/Fm/Alg/Opl/Alg.cpp"
)

set(FM_OPL3_ALG_FILES
    "Source/Generator/Fm/Alg/Opl3/Alg.h"
    "Source/Generator/Fm/Alg/Opl3/Alg.cpp"
)

set(FM_OPZX3_ALG_FILES
    "Source/Generator/Fm/Alg/Opzx3/Alg.h"
    "Source/Generator/Fm/Alg/Opzx3/Alg.cpp"
)

set(FM_FIX_FILES
    "Source/Generator/Fm/Fix/Params.h"
    "Source/Generator/Fm/Fix/Fix.h"
    "Source/Generator/Fm/Fix/Fix.cpp"
)

set(LFSR_NOISE_GEN_FILES
    "Source/Generator/Noise/Lfsr/Noise.h"
    "Source/Generator/Noise/Lfsr/Noise.cpp"
)

set(PCM_FILES
    "Source/Generator/Pcm/Pcm.h"
    "Source/Generator/Pcm/Pcm.cpp"
)

set(LFO_COUNTER_FILES
    "Source/Effector/Lfo/Counter/Counter.h"
    "Source/Effector/Lfo/Counter/Counter.cpp"
)

set(GLOBAL_LFO_FILES
    "Source/Effector/Lfo/Global/Params.h"
    "Source/Effector/Lfo/Global/Lfo.h"
    "Source/Effector/Lfo/Global/Lfo.cpp"
)

set(GLOBAL_LFO_UNIT_FILES
    "Source/Effector/Lfo/Global/Unit/Params.h"
    "Source/Effector/Lfo/Global/Unit/Unit.h"
    "Source/Effector/Lfo/Global/Unit/Unit.cpp"
)

set(Opna_LFO_FILES
    "Source/Effector/Lfo/Opna/Params.h"
    "Source/Effector/Lfo/Opna/Lfo.h"
    "Source/Effector/Lfo/Opna/Lfo.cpp"
)

set(Opna_LFO_UNIT_FILES
    "Source/Effector/Lfo/Opna/Unit/Params.h"
    "Source/Effector/Lfo/Opna/Unit/Unit.h"
    "Source/Effector/Lfo/Opna/Unit/Unit.cpp"
)

set(OPL_LFO_FILES
    "Source/Effector/Lfo/Opl/Params.h"
    "Source/Effector/Lfo/Opl/Lfo.h"
    "Source/Effector/Lfo/Opl/Lfo.cpp"
)

set(OPL_LFO_UNIT_FILES
    "Source/Effector/Lfo/Opl/Unit/Params.h"
    "Source/Effector/Lfo/Opl/Unit/Unit.h"
    "Source/Effector/Lfo/Opl/Unit/Unit.cpp"
)

set(OPM_LFO_FILES
    "Source/Effector/Lfo/Opm/Params.h"
    "Source/Effector/Lfo/Opm/Lfo.h"
    "Source/Effector/Lfo/Opm/Lfo.cpp"
)

set(OPM_LFO_UNIT_FILES
    "Source/Effector/Lfo/Opm/Unit/Params.h"
    "Source/Effector/Lfo/Opm/Unit/Unit.h"
    "Source/Effector/Lfo/Opm/Unit/Unit.cpp"
)

set(N88_LFO_FILES
    "Source/Effector/Lfo/N88/Params.h"
    "Source/Effector/Lfo/N88/Lfo.h"
    "Source/Effector/Lfo/N88/Lfo.cpp"
)

set(N88_LFO_UNIT_FILES
    "Source/Effector/Lfo/N88/Unit/Params.h"
    "Source/Effector/Lfo/N88/Unit/Unit.h"
    "Source/Effector/Lfo/N88/Unit/Unit.cpp"
)

set(XB_LFO_FILES
    "Source/Effector/Lfo/Xb/Params.h"
    "Source/Effector/Lfo/Xb/Lfo.h"
    "Source/Effector/Lfo/Xb/Lfo.cpp"
)

set(XB_LFO_UNIT_FILES
    "Source/Effector/Lfo/Xb/Unit/Params.h"
    "Source/Effector/Lfo/Xb/Unit/Unit.h"
    "Source/Effector/Lfo/Xb/Unit/Unit.cpp"
)

set(AMP_ADDDR_ENVELOPE_FILES
    "Source/Effector/Envelope/Amp/Adddr/Params.h"
    "Source/Effector/Envelope/Amp/Adddr/Env.h"
    "Source/Effector/Envelope/Amp/Adddr/Env.cpp"
)

set(AMP_ADSSR_ENVELOPE_FILES
    "Source/Effector/Envelope/Amp/Adssr/Params.h"
    "Source/Effector/Envelope/Amp/Adssr/Env.h"
    "Source/Effector/Envelope/Amp/Adssr/Env.cpp"
)

set(AMP_ADSR_ENVELOPE_FILES
    "Source/Effector/Envelope/Amp/Adsr/Params.h"
    "Source/Effector/Envelope/Amp/Adsr/Env.h"
    "Source/Effector/Envelope/Amp/Adsr/Env.cpp"
)

set(OPL_ENVELOPE_FILES
    "Source/Effector/Envelope/Fm/Opl/Params.h"
    "Source/Effector/Envelope/Fm/Opl/Env.h"
    "Source/Effector/Envelope/Fm/Opl/Env.cpp"
)

set(OPM_ENVELOPE_FILES
    "Source/Effector/Envelope/Fm/Opm/Params.h"
    "Source/Effector/Envelope/Fm/Opm/Env.h"
    "Source/Effector/Envelope/Fm/Opm/Env.cpp"
)

set(OPN_ENVELOPE_FILES
    "Source/Effector/Envelope/Fm/Opn/Params.h"
    "Source/Effector/Envelope/Fm/Opn/Env.h"
    "Source/Effector/Envelope/Fm/Opn/Env.cpp"
)

set(SSG_ENVELOPE_FILES
    "Source/Effector/Envelope/Ssg/Params.h"
    "Source/Effector/Envelope/Ssg/Env.h"
    "Source/Effector/Envelope/Ssg/Env.cpp"
)

set(Opna_PROCESSOR_FILES
    "Source/Processor/Opna/Pr.h"
    "Source/Processor/Opna/Pr.cpp"
)

set(Opna_SYNTH_FILES
    "Source/Synth/Opna/Core.h"
    "Source/Synth/Opna/Core.cpp"
    "Source/Synth/Opna/Params.h"
)

set(Opna_OPERATOR_FILES
    "Source/Synth/Opna/Operator/Core.h"
    "Source/Synth/Opna/Operator/Core.cpp"
    "Source/Synth/Opna/Operator/Params.h"
)

set(Opna_GUI_FILES
    "Source/Gui/Opna/Gui.h"
    "Source/Gui/Opna/Gui.cpp"
)

set(OPN_PROCESSOR_FILES
    "Source/Processor/Opn/Pr.h"
    "Source/Processor/Opn/Pr.cpp"
)

set(OPN_SYNTH_FILES
    "Source/Synth/Opn/Core.h"
    "Source/Synth/Opn/Core.cpp"
    "Source/Synth/Opn/Params.h"
)

set(OPN_OPERATOR_FILES
    "Source/Synth/Opn/Operator/Core.h"
    "Source/Synth/Opn/Operator/Core.cpp"
    "Source/Synth/Opn/Operator/Params.h"
)

set(OPN_GUI_FILES
    "Source/Gui/Opn/Gui.h"
    "Source/Gui/Opn/Gui.cpp"
)

set(OPL_PROCESSOR_FILES
    "Source/Processor/Opl/Pr.h"
    "Source/Processor/Opl/Pr.cpp"
)

set(OPL_SYNTH_FILES
    "Source/Synth/Opl/Core.h"
    "Source/Synth/Opl/Core.cpp"
    "Source/Synth/Opl/Params.h"
)

set(OPL_OPERATOR_FILES
    "Source/Synth/Opl/Operator/Core.h"
    "Source/Synth/Opl/Operator/Core.cpp"
    "Source/Synth/Opl/Operator/Params.h"
)

set(OPL_GUI_FILES
    "Source/Gui/Opl/Gui.h"
    "Source/Gui/Opl/Gui.cpp"
)

set(OPL3_PROCESSOR_FILES
    "Source/Processor/Opl3/Pr.h"
    "Source/Processor/Opl3/Pr.cpp"
)

set(OPL3_SYNTH_FILES
    "Source/Synth/Opl3/Core.h"
    "Source/Synth/Opl3/Core.cpp"
    "Source/Synth/Opl3/Params.h"
)

set(OPL3_OPERATOR_FILES
    "Source/Synth/Opl3/Operator/Core.h"
    "Source/Synth/Opl3/Operator/Core.cpp"
    "Source/Synth/Opl3/Operator/Params.h"
)

set(OPL3_GUI_FILES
    "Source/Gui/Opl3/Gui.h"
    "Source/Gui/Opl3/Gui.cpp"
)

set(OPM_PROCESSOR_FILES
    "Source/Processor/Opm/Pr.h"
    "Source/Processor/Opm/Pr.cpp"
)

set(OPM_SYNTH_FILES
    "Source/Synth/Opm/Core.h"
    "Source/Synth/Opm/Core.cpp"
    "Source/Synth/Opm/Params.h"
)

set(OPM_OPERATOR_FILES
    "Source/Synth/Opm/Operator/Core.h"
    "Source/Synth/Opm/Operator/Core.cpp"
    "Source/Synth/Opm/Operator/Params.h"
)

set(OPM_GUI_FILES
    "Source/Gui/Opm/Gui.h"
    "Source/Gui/Opm/Gui.cpp"
)

set(OPZX3_PROCESSOR_FILES
    "Source/Processor/Opzx3/Pr.h"
    "Source/Processor/Opzx3/Pr.cpp"
)

set(OPZX3_SYNTH_FILES
    "Source/Synth/Opzx3/Core.h"
    "Source/Synth/Opzx3/Core.cpp"
    "Source/Synth/Opzx3/Params.h"
)

set(OPZX3_OPERATOR_FILES
    "Source/Synth/Opzx3/Operator/Core.h"
    "Source/Synth/Opzx3/Operator/Core.cpp"
    "Source/Synth/Opzx3/Operator/Params.h"
)

set(OPZX3_GUI_FILES
    "Source/Gui/Opzx3/Gui.h"
    "Source/Gui/Opzx3/Gui.cpp"
)

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/Pr.h"
    "Source/Processor/Ssg/Pr.cpp"
)

set(SSG_SYNTH_FILES
    "Source/Synth/Ssg/Core.h"
    "Source/Synth/Ssg/Core.cpp"
    "Source/Synth/Ssg/Params.h"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/Gui.h"
    "Source/Gui/Ssg/Gui.cpp"
)

set(WT_PROCESSOR_FILES
    "Source/Processor/Wavetable/Pr.h"
    "Source/Processor/Wavetable/Pr.cpp"
)

set(WT_SYNTH_FILES
    "Source/Synth/Wavetable/Core.h"
    "Source/Synth/Wavetable/Core.cpp"
    "Source/Synth/Wavetable/Params.h"
)

set(WT_GUI_FILES
    "Source/Gui/Wavetable/Gui.h"
    "Source/Gui/Wavetable/Gui.cpp"
)

set(RHYTHM_PROCESSOR_FILES
    "Source/Processor/Rhythm/Pr.h"
    "Source/Processor/Rhythm/Pr.cpp"
)

set(RHYTHM_SYNTH_FILES
    "Source/Synth/Rhythm/Core.h"
    "Source/Synth/Rhythm/Core.cpp"
    "Source/Synth/Rhythm/Params.h"
)

set(RHYTHM_GUI_FILES
    "Source/Gui/Rhythm/Gui.h"
    "Source/Gui/Rhythm/Gui.cpp"
)

set(ADPCM_PROCESSOR_FILES
    "Source/Processor/Adpcm/Pr.h"
    "Source/Processor/Adpcm/Pr.cpp"
)

set(ADPCM_SYNTH_FILES
    "Source/Synth/Adpcm/Core.h"
    "Source/Synth/Adpcm/Core.cpp"
    "Source/Synth/Adpcm/Params.h"
)

set(ADPCM_GUI_FILES
    "Source/Gui/Adpcm/Gui.h"
    "Source/Gui/Adpcm/Gui.cpp"
)

set(BEEP_PROCESSOR_FILES
    "Source/Processor/Beep/Pr.h"
    "Source/Processor/Beep/Pr.cpp"
)

set(BEEP_SYNTH_FILES
    "Source/Synth/Beep/Core.h"
    "Source/Synth/Beep/Core.cpp"
    "Source/Synth/Beep/Params.h"
)

set(BEEP_GUI_FILES
    "Source/Gui/Beep/Gui.h"
    "Source/Gui/Beep/Gui.cpp"
)

set(FX_PROCESSOR_FILES
    "Source/Processor/Fx/Pr.h"
    "Source/Processor/Fx/Pr.cpp"
)

set(FX_EFFECT_FILES
    "Source/Effector/Fx/Core.h"
    "Source/Effector/Fx/Core.cpp"
)

set(FX_GUI_FILES
    "Source/Gui/Fx/Gui.h"
    "Source/Gui/Fx/Gui.cpp"
)

set(PRESET_FILES
    "Source/Gui/Preset/Gui.h"
    "Source/Gui/Preset/Gui.cpp"
)

set(SETTINGS_FILES
    "Source/Gui/Settings/Gui.h"
    "Source/Gui/Settings/Gui.cpp"
)

set(ABOUT_FILES
    "Source/Gui/About/Gui.h"
    "Source/Gui/About/Gui.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("2686V\\Core\\Const" FILES ${CONST_FILES})
source_group("2686V\\Core\\Synth" FILES ${SYNTH_FILES})
source_group("2686V\\Core\\Sample\\Fm" FILES ${FM_SAMPLE_CORE_FILES})
source_group("2686V\\Core\\Sample\\Ssg" FILES ${SSG_SAMPLE_CORE_FILES})
source_group("2686V\\Core\\Gui" FILES ${GUI_FILES})
source_group("2686V\\Core\\Processor" FILES ${PROCESSOR_FILES})
source_group("2686V\\Core\\Editor" FILES ${EDITOR_FILES})
source_group("2686V\\Core\\Docs" FILES ${DOCUMENT_FILES})

source_group("2686V\\Generator\\Noise\\Lfsr" FILES ${LFSR_NOISE_GEN_FILES})
source_group("2686V\\Generator\\Pcm" FILES ${PCM_FILES})
source_group("2686V\\Generator\\Fm" FILES ${FM_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Basic" FILES ${FM_BASIC_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Opl" FILES ${FM_OPL_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Opl3" FILES ${FM_OPL3_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Alg\\Opzx3" FILES ${FM_OPZX3_ALG_FILES})
source_group("2686V\\Generator\\Fm\\Fix" FILES ${FM_FIX_FILES})

source_group("2686V\\Effector\\Lfo\\Counter" FILES ${LFO_COUNTER_FILES})
source_group("2686V\\Effector\\Lfo\\Global" FILES ${GLOBAL_LFO_FILES})
source_group("2686V\\Effector\\Lfo\\Global\\Unit" FILES ${GLOBAL_LFO_UNIT_FILES})
source_group("2686V\\Effector\\Lfo\\Opna" FILES ${Opna_LFO_FILES})
source_group("2686V\\Effector\\Lfo\\Opna\\Unit" FILES ${Opna_LFO_UNIT_FILES})
source_group("2686V\\Effector\\Lfo\\Opl" FILES ${OPL_LFO_FILES})
source_group("2686V\\Effector\\Lfo\\Opl\\Unit" FILES ${OPL_LFO_UNIT_FILES})
source_group("2686V\\Effector\\Lfo\\Opm" FILES ${OPM_LFO_FILES})
source_group("2686V\\Effector\\Lfo\\Opm\\Unit" FILES ${OPM_LFO_UNIT_FILES})
source_group("2686V\\Effector\\Lfo\\N88" FILES ${N88_LFO_FILES})
source_group("2686V\\Effector\\Lfo\\N88\\Unit" FILES ${N88_LFO_UNIT_FILES})
source_group("2686V\\Effector\\Lfo\\Xb" FILES ${XB_LFO_FILES})
source_group("2686V\\Effector\\Lfo\\Xb\\Unit" FILES ${XB_LFO_UNIT_FILES})
source_group("2686V\\Effector\\Envelope\\Amp\\Adddr" FILES ${AMP_ADDDR_ENVELOPE_FILES})
source_group("2686V\\Effector\\Envelope\\Amp\\Adssr" FILES ${AMP_ADSSR_ENVELOPE_FILES})
source_group("2686V\\Effector\\Envelope\\Amp\\Adsr" FILES ${AMP_ADSR_ENVELOPE_FILES})
source_group("2686V\\Effector\\Envelope\\Fm\\Opl" FILES ${OPL_ENVELOPE_FILES})
source_group("2686V\\Effector\\Envelope\\Fm\\Opn" FILES ${OPN_ENVELOPE_FILES})
source_group("2686V\\Effector\\Envelope\\Fm\\Opm" FILES ${OPM_ENVELOPE_FILES})
source_group("2686V\\Effector\\Envelope\\Ssg" FILES ${SSG_ENVELOPE_FILES})

source_group("2686V\\Effector\\Fx" FILES ${FX_EFFECT_FILES})

source_group("2686V\\Processor\\Opna" FILES ${Opna_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opn" FILES ${OPN_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl" FILES ${OPL_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl3" FILES ${OPL3_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opm" FILES ${OPM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opzx3" FILES ${OPZX3_PROCESSOR_FILES})
source_group("2686V\\Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("2686V\\Processor\\Wt" FILES ${WT_PROCESSOR_FILES})
source_group("2686V\\Processor\\Rhythm" FILES ${RHYTHM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Adpcm" FILES ${ADPCM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Beep" FILES ${BEEP_PROCESSOR_FILES})
source_group("2686V\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})

source_group("2686V\\Synth\\Opna" FILES ${Opna_SYNTH_FILES})
source_group("2686V\\Synth\\Opn" FILES ${OPN_SYNTH_FILES})
source_group("2686V\\Synth\\Opl" FILES ${OPL_SYNTH_FILES})
source_group("2686V\\Synth\\Opl3" FILES ${OPL3_SYNTH_FILES})
source_group("2686V\\Synth\\Opm" FILES ${OPM_SYNTH_FILES})
source_group("2686V\\Synth\\Opzx3" FILES ${OPZX3_SYNTH_FILES})
source_group("2686V\\Synth\\Ssg" FILES ${SSG_SYNTH_FILES})
source_group("2686V\\Synth\\Wt" FILES ${WT_SYNTH_FILES})
source_group("2686V\\Synth\\Rhythm" FILES ${RHYTHM_SYNTH_FILES})
source_group("2686V\\Synth\\Adpcm" FILES ${ADPCM_SYNTH_FILES})
source_group("2686V\\Synth\\Beep" FILES ${BEEP_SYNTH_FILES})

source_group("2686V\\Synth\\Opna\\Operator" FILES ${Opna_OPERATOR_FILES})
source_group("2686V\\Synth\\Opn\\Operator" FILES ${OPN_OPERATOR_FILES})
source_group("2686V\\Synth\\Opl\\Operator" FILES ${OPL_OPERATOR_FILES})
source_group("2686V\\Synth\\Opl3\\Operator" FILES ${OPL3_OPERATOR_FILES})
source_group("2686V\\Synth\\Opm\\Operator" FILES ${OPM_OPERATOR_FILES})
source_group("2686V\\Synth\\Opzx3\\Operator" FILES ${OPZX3_OPERATOR_FILES})

source_group("2686V\\Gui\\Opna" FILES ${Opna_GUI_FILES})
source_group("2686V\\Gui\\Opn" FILES ${OPN_GUI_FILES})
source_group("2686V\\Gui\\Opl" FILES ${OPL_GUI_FILES})
source_group("2686V\\Gui\\Opl3" FILES ${OPL3_GUI_FILES})
source_group("2686V\\Gui\\Opm" FILES ${OPM_GUI_FILES})
source_group("2686V\\Gui\\Opzx3" FILES ${OPZX3_GUI_FILES})
source_group("2686V\\Gui\\Ssg" FILES ${SSG_GUI_FILES})
source_group("2686V\\Gui\\Wt" FILES ${WT_GUI_FILES})
source_group("2686V\\Gui\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("2686V\\Gui\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("2686V\\Gui\\Beep" FILES ${BEEP_GUI_FILES})
source_group("2686V\\Gui\\Preset" FILES ${PRESET_GUI_FILES})
source_group("2686V\\Gui\\Fx" FILES ${FX_GUI_FILES})
source_group("2686V\\Gui\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686V\\Gui\\About" FILES ${ABOUT_GUI_FILES})

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
            ${CONST_FILES}
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
            ${CONST_FILES}
            ${SYNTH_FILES}
            ${FM_SAMPLE_CORE_FILES}
            ${SSG_SAMPLE_CORE_FILES}
            ${FM_FILES}
            ${FM_BASIC_ALG_FILES}
            ${FM_OPL_ALG_FILES}
            ${FM_OPL3_ALG_FILES}
            ${FM_OPZX3_ALG_FILES}
            ${FM_FIX_FILES}
            ${LFSR_NOISE_GEN_FILES}
            ${LFO_COUNTER_FILES}
            ${GLOBAL_LFO_FILES}
            ${GLOBAL_LFO_UNIT_FILES}
            ${Opna_LFO_FILES}
            ${Opna_LFO_UNIT_FILES}
            ${OPL_LFO_FILES}
            ${OPL_LFO_UNIT_FILES}
            ${OPM_LFO_FILES}
            ${OPM_LFO_UNIT_FILES}
            ${N88_LFO_FILES}
            ${N88_LFO_UNIT_FILES}
            ${XB_LFO_FILES}
            ${XB_LFO_UNIT_FILES}
            ${ENVELOPE_FILES}
            ${AMP_ADDDR_ENVELOPE_FILES}
            ${AMP_ADSSR_ENVELOPE_FILES}
            ${AMP_ADSR_ENVELOPE_FILES}
            ${OPL_ENVELOPE_FILES}
            ${OPM_ENVELOPE_FILES}
            ${OPN_ENVELOPE_FILES}
            ${SSG_ENVELOPE_FILES}
            ${PCM_FILES}
            ${GUI_FILES}
            ${PROCESSOR_FILES}
            ${EDITOR_FILES}
            ${Opna_PROCESSOR_FILES}
            ${Opna_SYNTH_FILES}
            ${Opna_OPERATOR_FILES}
            ${Opna_GUI_FILES}
            ${OPN_PROCESSOR_FILES}
            ${OPN_SYNTH_FILES}
            ${OPN_OPERATOR_FILES}
            ${OPN_GUI_FILES}
            ${OPL_PROCESSOR_FILES}
            ${OPL_SYNTH_FILES}
            ${OPL_OPERATOR_FILES}
            ${OPL_GUI_FILES}
            ${OPL3_PROCESSOR_FILES}
            ${OPL3_SYNTH_FILES}
            ${OPL3_OPERATOR_FILES}
            ${OPL3_GUI_FILES}
            ${OPM_PROCESSOR_FILES}
            ${OPM_SYNTH_FILES}
            ${OPM_OPERATOR_FILES}
            ${OPM_GUI_FILES}
            ${OPZX3_PROCESSOR_FILES}
            ${OPZX3_SYNTH_FILES}
            ${OPZX3_OPERATOR_FILES}
            ${OPZX3_GUI_FILES}
            ${SSG_PROCESSOR_FILES}
            ${SSG_SYNTH_FILES}
            ${SSG_GUI_FILES}
            ${WT_PROCESSOR_FILES}
            ${WT_SYNTH_FILES}
            ${WT_GUI_FILES}
            ${RHYTHM_FPROCESSOR_ILES}
            ${RHYTHM_SYNTH_FILES}
            ${RHYTHM_GUI_FILES}
            ${ADPCM_PROCESSOR_FILES}
            ${ADPCM_SYNTH_FILES}
            ${ADPCM_GUI_FILES}
            ${BEEP_PROCESSOR_FILES}
            ${BEEP_SYNTH_FILES}
            ${BEEP_GUI_FILES}
            ${FX_PROCESSOR_FILES}
            ${FX_EFFECT_FILES}
            ${FX_GUI_FILES}
            ${PRESET_GUI_FILES}
            ${SETTINGS_GUI_FILES}
            ${ABOUT_GUI_FILES}
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
