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






























set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("2686VO\\Constants" FILES ${CONST_FILES})
source_group("2686VO\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686VO\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686VO\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686VO\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("2686VO\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686VO\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686VO\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
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

