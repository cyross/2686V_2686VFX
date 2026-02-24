# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# ==============================================================================
# ビルドモード設定スイッチ
# ON にするとエフェクト版 (2686VFX)、OFF にするとシンセ版 (2686V) になります
option(BUILD_AS_FX "Build as FX Plugin" OFF)

# スイッチの状態によって変数をセット
if(BUILD_AS_FX)
    set(CY_PLUGIN_NAME "2686VFX")     # ファイル名: 2686VFX.vst3
    set(CY_PLUGIN_CODE "28FX")        # プラグインID (必ず変える必要があります！)
    set(CY_IS_SYNTH FALSE)            # インストゥルメント扱いしない
    set(CY_CATEGORIES "Fx" "Distortion" "BitCrusher")
else()
    set(CY_PLUGIN_NAME "2686V")       # ファイル名: 2686V.vst3
    set(CY_PLUGIN_CODE "2686")        # プラグインID
    set(CY_IS_SYNTH TRUE)             # インストゥルメント扱い
    set(CY_CATEGORIES "Instrument" "Synth")
endif()
# ==============================================================================

# プラグインの定義
juce_add_plugin(${CY_PLUGIN_NAME}
    COMPANY_NAME ${CY_COMPANY_NAME}         # あなたの名前に変更してください
    IS_SYNTH ${CY_IS_SYNTH}                   # シンセサイザーとして定義
    NEEDS_MIDI_INPUT TRUE           # MIDI入力を有効化
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE    # ビルド後にVST3フォルダへコピー
    PLUGIN_MANUFACTURER_CODE ${CY_MANUFACTURER_CODE} # 独自のID
    PLUGIN_CODE ${CY_PLUGIN_CODE}
    FORMATS VST3 Standalone         # VST3とデバッグ用スタンドアロン
    PRODUCT_NAME ${CY_PLUGIN_NAME}
)

if(BUILD_AS_FX)
    target_compile_definitions(${CY_PLUGIN_NAME} PUBLIC BUILD_AS_FX_PLUGIN=1)
endif()

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

if(BUILD_AS_FX)
# 定義
set(CONST_FILES
    Source/const/GlobalConstants.h
    Source/const/FileConstants.h
    Source/const/PresetConstants.h
)

set(CORE_FILES
    Source/core/PrBase.h
    Source/core/PrBase.cpp
)

set(SYNTH_FILES
    Source/synth/Mode.h
    Source/synth/Mode.cpp
    Source/synth/Pcm.h
    Source/synth/Pcm.cpp
    Source/synth/SynthHelpers.h
    Source/synth/SynthHelpers.cpp
    Source/synth/SynthParams.h
    Source/synth/SynthVoice.h
    Source/synth/SynthVoice.cpp
)

set(GUI_FILES
    Source/gui/GuiFile.h
    Source/gui/GuiFile.cpp
    Source/gui/GuiBase.h
    Source/gui/GuiBase.cpp
    Source/gui/GuiColor.h
    Source/gui/GuiComponents.h
    Source/gui/GuiComponents.cpp
    Source/gui/GuiConstants.h
    Source/gui/GuiContext.h
    Source/gui/GuiHelpers.h
    Source/gui/GuiHelpers.cpp
    Source/gui/GuiLF.h
    Source/gui/GuiLF.cpp
    Source/gui/GuiSelectItems.h
    Source/gui/GuiStructs.h
    Source/gui/GuiStateManager.h
    Source/gui/GuiStateManager.cpp
    Source/gui/LabelConstants.h
)

set(FX_FILES
    Source/FX/FxCore.h
    Source/FX/FxCore.cpp
    Source/FX/PrFx.h
    Source/FX/PrFx.cpp
    Source/FX/GuiFx.h
    Source/FX/GuiFx.cpp
)

set(SETTINGS_FILES
    Source/SETTINGS/GuiSettings.h
    Source/SETTINGS/GuiSettings.cpp
)

set(ABOUT_FILES
    Source/ABOUT/GuiAbout.h
    Source/ABOUT/GuiAbout.cpp
)

set(PROCESSOR_FILES
    Source/processor/PluginProcessor.h
    Source/processor/PluginProcessor.cpp
)

set(EDITOR_FILES
    Source/editor/PluginEditor.h
    Source/editor/PluginEditor.cpp
)

set(DOCUMENT_FILES ../readme.md)

# ソースグループの設定
source_group("Core 1: Main" FILES ${CORE_FILES})
source_group("Core 2: Const" FILES ${CONST_FILES})
source_group("Core 3: Synth" FILES ${SYNTH_FILES})
source_group("Core 4: Gui" FILES ${GUI_FILES})
source_group("Core 5: Processor" FILES ${PROCESSOR_FILES})
source_group("Core 6: Editor" FILES ${EDITOR_FILES})
source_group("Tab 1: FX" FILES ${FX_FILES})
source_group("Tab 2: SETTINGS" FILES ${SETTINGS_FILES})
source_group("Tab 3: ABOUT" FILES ${ABOUT_FILES})
source_group("Doc" FILES ${DOCUMENT_FILES})

# ソースファイルの指定
target_sources(${CY_PLUGIN_NAME} PRIVATE
    ${CORE_FILES}
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
# 定義
set(CONST_FILES
    Source/const/FileConstants.h
    Source/const/GlobalConstants.h
    Source/const/PresetConstants.h
)

set(CORE_FILES
    Source/core/PrBase.h
    Source/core/PrBase.cpp
)

set(SYNTH_FILES
    Source/synth/Mode.h
    Source/synth/Mode.cpp
    Source/synth/Pcm.h
    Source/synth/Pcm.cpp
    Source/synth/SynthHelpers.h
    Source/synth/SynthHelpers.cpp
    Source/synth/SynthParams.h
    Source/synth/SynthVoice.h
    Source/synth/SynthVoice.cpp
)

set(FM_FILES
    Source/fm/FmCommon.h
    Source/fm/FmCommon.cpp
    Source/fm/MmlConstants.h
    Source/fm/OpConstants.h
    Source/fm/OpValueRange.h
    Source/fm/RegisterType.h
    Source/fm/RegisterConverter.h
    Source/fm/RegisterConverter.cpp
    Source/fm/SliderRegMap.h
    Source/fm/SliderRegMap.cpp
)

set(GUI_FILES
    Source/gui/GuiFile.h
    Source/gui/GuiFile.cpp
    Source/gui/GuiBase.h
    Source/gui/GuiBase.cpp
    Source/gui/GuiColor.h
    Source/gui/GuiComponents.h
    Source/gui/GuiComponents.cpp
    Source/gui/GuiConstants.h
    Source/gui/GuiContext.h
    Source/gui/GuiHelpers.h
    Source/gui/GuiHelpers.cpp
    Source/gui/GuiLF.h
    Source/gui/GuiLF.cpp
    Source/gui/GuiSelectItems.h
    Source/gui/GuiStructs.h
    Source/gui/GuiStateManager.h
    Source/gui/GuiStateManager.cpp
    Source/gui/LabelConstants.h
)

set(OPNA_FILES
    Source/OPNA/GuiOpna.h
    Source/OPNA/GuiOpna.cpp
    Source/OPNA/OpnaCore.h
    Source/OPNA/OpnaCore.cpp
    Source/OPNA/PrOpna.h
    Source/OPNA/PrOpna.cpp
)

set(OPN_FILES
    Source/OPN/GuiOpn.h
    Source/OPN/GuiOpn.cpp
    Source/OPN/OpnCore.h
    Source/OPN/OpnCore.cpp
    Source/OPN/PrOpn.h
    Source/OPN/PrOpn.cpp
)

set(OPL_FILES
    Source/OPL/GuiOpl.h
    Source/OPL/GuiOpl.cpp
    Source/OPL/OplCore.h
    Source/OPL/OplCore.cpp
    Source/OPL/PrOpl.h
    Source/OPL/PrOpl.cpp
)

set(OPL3_FILES
    Source/OPL3/GuiOpl3.h
    Source/OPL3/GuiOpl3.cpp
    Source/OPL3/Opl3Core.h
    Source/OPL3/Opl3Core.cpp
    Source/OPL3/PrOpl3.h
    Source/OPL3/PrOpl3.cpp
)

set(OPM_FILES
    Source/OPM/GuiOpm.h
    Source/OPM/GuiOpm.cpp
    Source/OPM/OpmCore.h
    Source/OPM/OpmCore.cpp
    Source/OPM/PrOpm.h
    Source/OPM/PrOpm.cpp
)

set(OPZX3_FILES
    Source/OPZX3/GuiOpzx3.h
    Source/OPZX3/GuiOpzx3.cpp
    Source/OPZX3/Opzx3Core.h
    Source/OPZX3/Opzx3Core.cpp
    Source/OPZX3/PrOpzx3.h
    Source/OPZX3/PrOpzx3.cpp
)

set(SSG_FILES
    Source/SSG/GuiSsg.h
    Source/SSG/GuiSsg.cpp
    Source/SSG/PrSsg.h
    Source/SSG/PrSsg.cpp
    Source/SSG/SsgCore.h
    Source/SSG/SsgCore.cpp
)

set(WT_FILES
    Source/WT/GuiWt.h
    Source/WT/GuiWt.cpp
    Source/WT/PrWt.h
    Source/WT/PrWt.cpp
    Source/WT/WtCore.h
    Source/WT/WtCore.cpp
)

set(RHYTHM_FILES
    Source/RHYTHM/GuiRhythm.h
    Source/RHYTHM/GuiRhythm.cpp
    Source/RHYTHM/PrRhythm.h
    Source/RHYTHM/PrRhythm.cpp
    Source/RHYTHM/RhythmCore.h
    Source/RHYTHM/RhythmCore.cpp
)

set(ADPCM_FILES
    Source/ADPCM/AdpcmCore.h
    Source/ADPCM/AdpcmCore.cpp
    Source/ADPCM/GuiAdpcm.h
    Source/ADPCM/GuiAdpcm.cpp
    Source/ADPCM/PrAdpcm.h
    Source/ADPCM/PrAdpcm.cpp
)

set(FX_FILES
    Source/FX/FxCore.h
    Source/FX/FxCore.cpp
    Source/FX/GuiFx.h
    Source/FX/GuiFx.cpp
    Source/FX/PrFx.h
    Source/FX/PrFx.cpp
)

set(PRESET_FILES
    Source/PRESET/GuiPreset.h
    Source/PRESET/GuiPreset.cpp
)

set(SETTINGS_FILES
    Source/SETTINGS/GuiSettings.h
    Source/SETTINGS/GuiSettings.cpp
)

set(ABOUT_FILES
    Source/ABOUT/GuiAbout.h
    Source/ABOUT/GuiAbout.cpp
)

set(PROCESSOR_FILES
    Source/processor/PluginProcessor.h
    Source/processor/PluginProcessor.cpp
)

set(EDITOR_FILES
    Source/editor/PluginEditor.h
    Source/editor/PluginEditor.cpp
)

set(DOCUMENT_FILES ../readme.md)

# ソースグループの設定
source_group("Core 1: Main" FILES ${CORE_FILES})
source_group("Core 2: Const" FILES ${CONST_FILES})
source_group("Core 3: Synth" FILES ${SYNTH_FILES})
source_group("Core 4: FM" FILES ${FM_FILES})
source_group("Core 5: Gui" FILES ${GUI_FILES})
source_group("Core 6: Processor" FILES ${PROCESSOR_FILES})
source_group("Core 7: Editor" FILES ${EDITOR_FILES})
source_group("Tab 01: OPNA" FILES ${OPNA_FILES})
source_group("Tab 02: OPN" FILES ${OPN_FILES})
source_group("Tab 03: OPL" FILES ${OPL_FILES})
source_group("Tab 04: OPL3" FILES ${OPL3_FILES})
source_group("Tab 05: OPM" FILES ${OPM_FILES})
source_group("Tab 06: OPZX3" FILES ${OPZX3_FILES})
source_group("Tab 07: SSG" FILES ${SSG_FILES})
source_group("Tab 08: WT" FILES ${WT_FILES})
source_group("Tab 09: RHYTHM" FILES ${RHYTHM_FILES})
source_group("Tab 10: ADPCM" FILES ${ADPCM_FILES})
source_group("Tab 11: FX" FILES ${FX_FILES})
source_group("Tab 12: PRESET" FILES ${PRESET_FILES})
source_group("Tab 13: SETTINGS" FILES ${SETTINGS_FILES})
source_group("Tab 14: ABOUT" FILES ${ABOUT_FILES})
source_group("Docs" FILES ${DOCUMENT_FILES})

# ソースファイルの指定
target_sources(${CY_PLUGIN_NAME} PRIVATE
    ${CORE_FILES}
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
    ${FX_FILES}
    ${PRESET_FILES}
    ${SETTINGS_FILES}
    ${ABOUT_FILES}
    ${DOCUMENT_FILES}
)
endif()

# ライブラリのリンク
target_link_libraries(${CY_PLUGIN_NAME} PRIVATE
    # VSTロゴ
    VstLogoForAbout
    # JUCEモジュール
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
)

target_compile_definitions(${CY_PLUGIN_NAME} PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_generate_juce_header(${CY_PLUGIN_NAME})
