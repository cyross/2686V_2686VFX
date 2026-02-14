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

# ドキュメントの定義
set(DOCUMENT_FILES
    ../readme.md
    ../document/2686V仕様.md
)

# ソースファイルの指定
target_sources(${CY_PLUGIN_NAME} PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Mode.h
    Source/SynthVoice.h
    Source/FmCommon.h
    Source/OpnaCore.h
    Source/OpnCore.h
    Source/OplCore.h
    Source/OpllCore.h
    Source/Opl3Core.h
    Source/OpmCore.h
    Source/Opzx3Core.h
    Source/SSGCore.h
    Source/RhythmCore.h
    Source/AdpcmCore.h
    Source/WavetableCore.h
    Source/RegisterConverter.h
    Source/SimpleEffects.h
    ${DOCUMENT_FILES}
)

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

source_group("Documentation" FILES ../readme.md ../document/2686V仕様.md)
