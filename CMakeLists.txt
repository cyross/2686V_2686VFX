cmake_minimum_required(VERSION 3.22)

# ソリューション（ワークスペース）全体の名前
project(RetroSynthsBundle VERSION 3.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_VST3_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/vst3sdk" CACHE PATH "Path to the VST3 SDK" FORCE)

# JUCEの読み込み (ここで1回だけJUCEが構成・ビルドされます)
add_subdirectory(external/JUCE)

# yaml-cpp の読み込み
# パラメータファイルを YAML でも読み書きするために使う。
# 部品と道具は要らないので切っておく。テストも同様。
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/yaml-cpp)

# yaml-cpp は自前の警告設定を持たないので、こちらの厳しい設定に
# 巻き込まれないようにする。外から持ってきたものは直せないため。
if(MSVC)
    target_compile_options(yaml-cpp PRIVATE /W0)
endif()

# ==============================================================================
# プラグイン生成共通関数
# ==============================================================================
# 第3引数で出力名(PRODUCT_NAME)を指定できます。省略するとターゲット名がそのまま使われます。
# VST3の識別子はメーカーコードとプラグインコードから生成されるため、
# 出力名を変えても既存プロジェクトのプラグイン解決には影響しません。
function(create_cyross_plugin TARGET_NAME CY_PLUGIN_CODE)
    if(ARGC GREATER 2)
        set(CY_PRODUCT_NAME ${ARGV2})
    else()
        set(CY_PRODUCT_NAME ${TARGET_NAME})
    endif()

    juce_add_plugin(${TARGET_NAME}
        COMPANY_NAME "CYROSS"
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE "CYRS"
        PLUGIN_CODE ${CY_PLUGIN_CODE}
        FORMATS VST3 Standalone
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
        PRODUCT_NAME ${CY_PRODUCT_NAME}
        VST3_AUTO_MANIFEST FALSE
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /utf-8 /FS)
    endif()

    # ALL_SOURCES は呼び出し元のディレクトリスコープで定義されたものが使われます
    target_sources(${TARGET_NAME} PRIVATE ${ALL_SOURCES})

    if(UNIX AND NOT APPLE)
        target_link_libraries(${TARGET_NAME} PRIVATE
            freetype fontconfig X11 Xcursor Xinerama Xrandr Xcomposite asound GL
        )
    endif()

    # 各ディレクトリで個別定義したバイナリデータ(Target名)をリンクする
    target_link_libraries(${TARGET_NAME} PRIVATE
        AppIconForAbout_${TARGET_NAME}
        VstLogoForAbout_${TARGET_NAME}
        yaml-cpp::yaml-cpp
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_dsp
    )

    target_compile_definitions(${TARGET_NAME} PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_USE_CAMERA=0
        JUCE_USE_CDREADER=0
        JUCE_USE_CDBURNER=0
    )

    juce_generate_juce_header(${TARGET_NAME})
endfunction()

# 4つのプラグインを一括でビルド対象に追加
add_subdirectory(2686V)
add_subdirectory(2686VLight)
add_subdirectory(2686VOrigin)
add_subdirectory(OPZX7S)

# マニュアル用の道具 (出荷するものではない)
add_subdirectory(tools)

# ユニットテスト (JUCE を読み込まずロジック層だけを検証する)
enable_testing()
add_subdirectory(tests)
