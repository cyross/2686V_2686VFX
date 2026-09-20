cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_2686VFX
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_2686VFX
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





set(FX_GUI_FILES
    "Source/Gui/Fx/GuiFxText.h"
    "Source/Gui/Fx/GuiFxValues.h"
    "Source/Gui/Fx/GuiFx.h"
    "Source/Gui/Fx/GuiFx.cpp"
    "Source/Gui/Fx/GuiFxKeyAssign.h"
    "Source/Gui/Fx/GuiFxKeyAssign.cpp"
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
source_group("2686VFX\\Constants" FILES ${CONST_FILES})
source_group("2686VFX\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686VFX\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686VFX\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686VFX\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("2686VFX\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686VFX\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686VFX\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("2686VFX\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("2686VFX\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("2686VFX\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686VFX\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})
source_group("2686VFX\\Gui\\Tab\\Colors" FILES ${COLORS_GUI_FILES})

# 音源ではなく、入力された音を加工するエフェクト。
# MIDI は受け取る。鍵盤の押し離しでエンベロープや LFO を動かすため。
create_cyross_effect("2686VFX" "28FX")

