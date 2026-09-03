# ==============================================================================
# マニュアル用の道具
# ==============================================================================
# 出荷するものではない。マニュアルに載せる一覧を作り直すときだけ動かす。

# ------------------------------------------------------------------------------
# パラメータの一覧を書き出す
# ------------------------------------------------------------------------------
# 2686V はすべての音源を含むので、これ 1 つで全チャンネルぶんが採れる。
# プロセッサを作って、登録されているものをそのまま吐き出す。
juce_add_console_app(ParamDump PRODUCT_NAME "ParamDump")

juce_generate_juce_header(ParamDump)

file(GLOB_RECURSE PARAM_DUMP_PLUGIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/2686V/Source/*.cpp"
)

target_sources(ParamDump PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ParamDump/ParamDump.cpp"
    ${PARAM_DUMP_PLUGIN_SOURCES}
)

target_include_directories(ParamDump PRIVATE
    "${CMAKE_SOURCE_DIR}/2686V/Source"
)

target_link_libraries(ParamDump PRIVATE
    AppIconForAbout_2686V
    VstLogoForAbout_2686V
    yaml-cpp::yaml-cpp
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
)

target_compile_definitions(ParamDump PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_CAMERA=0
    JUCE_USE_CDREADER=0
    JUCE_USE_CDBURNER=0

    # 名前と版は出力にそのまま載せる
    CY_HAS_CURVE=1

    JucePlugin_Name="2686V"
    JucePlugin_VersionString="3.1.0"
)

target_compile_features(ParamDump PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(ParamDump PRIVATE /utf-8 /FS /bigobj)
endif()

# ------------------------------------------------------------------------------
# 2686VFX のパラメータの一覧を書き出す
# ------------------------------------------------------------------------------
# 書き出す中身はプラグインに依らず、プロセッサの型名も同じなので、
# 見に行くソースを差し替えた 2 つ目の的を立てるだけでよい。
juce_add_console_app(ParamDumpFx PRODUCT_NAME "ParamDumpFx")

juce_generate_juce_header(ParamDumpFx)

file(GLOB_RECURSE PARAM_DUMP_FX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/2686VFX/Source/*.cpp"
)

target_sources(ParamDumpFx PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ParamDump/ParamDump.cpp"
    ${PARAM_DUMP_FX_SOURCES}
)

target_include_directories(ParamDumpFx PRIVATE
    "${CMAKE_SOURCE_DIR}/2686VFX/Source"
)

target_link_libraries(ParamDumpFx PRIVATE
    AppIconForAbout_2686VFX
    VstLogoForAbout_2686VFX
    yaml-cpp::yaml-cpp
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
)

target_compile_definitions(ParamDumpFx PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_CAMERA=0
    JUCE_USE_CDREADER=0
    JUCE_USE_CDBURNER=0

    CY_HAS_CURVE=0

    JucePlugin_Name="2686VFX"
    JucePlugin_VersionString="3.1.0"
)

target_compile_features(ParamDumpFx PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(ParamDumpFx PRIVATE /utf-8 /FS /bigobj)
endif()
