cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_26V
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_26V
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



set(OPN_PROCESSOR_FILES
    "Source/Processor/Opn/ProcessorOpnKeys.h"
    "Source/Processor/Opn/ProcessorOpnNames.h"
    "Source/Processor/Opn/ProcessorOpn.h"
    "Source/Processor/Opn/ProcessorOpn.cpp"
)

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/ProcessorSsgKeys.h"
    "Source/Processor/Ssg/ProcessorSsgNames.h"
    "Source/Processor/Ssg/ProcessorSsgValues.h"
    "Source/Processor/Ssg/ProcessorSsg.h"
    "Source/Processor/Ssg/ProcessorSsg.cpp"
)

set(FX_PROCESSOR_FILES
    "Source/Processor/Fx/ProcessorFxKeys.h"
    "Source/Processor/Fx/ProcessorFxNames.h"
    "Source/Processor/Fx/ProcessorFxValues.h"
    "Source/Processor/Fx/ProcessorFx.h"
    "Source/Processor/Fx/ProcessorFx.cpp"
)

set(OPN_GUI_FILES
    "Source/Gui/Opn/GuiOpnText.h"
    "Source/Gui/Opn/GuiOpnValues.h"
    "Source/Gui/Opn/GuiOpn.h"
    "Source/Gui/Opn/GuiOpn.cpp"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/GuiSsgText.h"
    "Source/Gui/Ssg/GuiSsgValues.h"
    "Source/Gui/Ssg/GuiSsg.h"
    "Source/Gui/Ssg/GuiSsg.cpp"
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
source_group("26V\\Constants" FILES ${CONST_FILES})
source_group("26V\\Documents" FILES ${DOCUMENT_FILES})
source_group("26V\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("26V\\Synth\\Fm" FILES ${FM_FILES})
source_group("26V\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("26V\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("26V\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("26V\\Processor\\Opn" FILES ${OPN_PROCESSOR_FILES})
source_group("26V\\Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("26V\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("26V\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("26V\\Gui\\Tab\\Opn" FILES ${OPN_GUI_FILES})
source_group("26V\\Gui\\Tab\\Ssg" FILES ${SSG_GUI_FILES})
source_group("26V\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("26V\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("26V\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("26V\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})
source_group("2686VLight\\Gui\\Tab\\Colors" FILES ${COLORS_GUI_FILES})

create_cyross_plugin("26V" "26VO")

