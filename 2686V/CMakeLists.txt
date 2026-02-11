# プラグインの定義
juce_add_plugin(2686V
    COMPANY_NAME "YourName"         # あなたの名前に変更してください
    IS_SYNTH TRUE                   # シンセサイザーとして定義
    NEEDS_MIDI_INPUT TRUE           # MIDI入力を有効化
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE    # ビルド後にVST3フォルダへコピー
    PLUGIN_MANUFACTURER_CODE "2686" # 独自のID
    PLUGIN_CODE "Vsyn"
    FORMATS VST3 Standalone         # VST3とデバッグ用スタンドアロン
    PRODUCT_NAME "2686V"
)

# ソースファイルの指定
target_sources(2686V PRIVATE
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
    Source/SSGCore.h
    Source/RhythmCore.h
    Source/AdpcmCore.h
    Source/WavetableCore.h
    Source/RegisterConverter.h
    Source/SimpleEffect.h
)

# ライブラリのリンク
target_link_libraries(2686V PRIVATE
    # JUCEモジュール
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
)

target_compile_definitions(2686V PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_generate_juce_header(2686V)
