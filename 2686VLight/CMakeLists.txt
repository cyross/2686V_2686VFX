cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_2686VLight
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_2686VLight
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


set(PROCESSOR_FILES
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




























set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("2686VL\\Constants" FILES ${CONST_FILES})
source_group("2686VL\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686VL\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686VL\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686VL\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("2686VL\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686VL\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686VL\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("2686VL\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("2686VL\\Gui\\Tab\\Opna" FILES ${OPNA_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Opn" FILES ${OPN_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Opl" FILES ${OPL_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Opl3" FILES ${OPL3_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Opm" FILES ${OPM_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Opzx7" FILES ${OPZX7_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Ssg" FILES ${SSG_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Wt" FILES ${WT_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Wt2" FILES ${WT2_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("2686VL\\Gui\\Tab\\Beep" FILES ${BEEP_GUI_FILES})
source_group("2686VL\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("2686VL\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("2686VL\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686VL\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})
source_group("2686VLight\\Gui\\Tab\\Colors" FILES ${COLORS_GUI_FILES})

create_cyross_plugin("2686VLight" "28VL")

