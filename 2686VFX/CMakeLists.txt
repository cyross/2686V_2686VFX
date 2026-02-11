juce_add_plugin(2686VFX
    COMPANY_NAME "YourName"
    IS_SYNTH FALSE                  # エフェクトとして定義
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "2686"
    PLUGIN_CODE "Vfx1"
    FORMATS VST3 Standalone
    PRODUCT_NAME "2686VFX"
)

# VSTロゴの登録
juce_add_binary_data(VstLogoForFXAbout
    HEADER_NAME "VstLogoForFXAbout.h"
    NAMESPACE VstLogoForFXAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# ドキュメントの定義
set(DOCUMENT_FILES
    ../readme.md
    ../document/2686VFX仕様.md
)

target_sources(2686VFX PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    ${DOCUMENT_FILES}
)

target_link_libraries(2686VFX PRIVATE
    # VSTロゴ
    VstLogoForFXAbout
    # JUCEモジュール
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
)

target_compile_definitions(2686VFX PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_generate_juce_header(2686VFX)

source_group("Documentation" FILES ../readme.md ../document/2686VFX仕様.md)
