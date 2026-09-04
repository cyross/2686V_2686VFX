cmake_minimum_required(VERSION 3.22)

# ソリューション（ワークスペース）全体の名前
project(RetroSynthsBundle VERSION 3.1.0 LANGUAGES C CXX)

# Debug ビルドの起動が遅すぎるので、インライン展開だけは許す。
# 既定の /Ob0 では JUCE の Component や String のような小さなヘッダ関数が
# 一切展開されず、GUI の生成に数十秒かかっていた。
# JUCE や yaml-cpp にも効かせたいので add_subdirectory より前に置く。
if(MSVC)
    add_compile_options($<$<CONFIG:Debug>:/Ob1>)
endif()

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

# ==============================================================================
# 先読みヘッダ (PCH) の準備
# ==============================================================================
# 自前のソースはほぼ全部が JuceHeader.h ごしに JUCE を丸ごと読み込む。
# その読み込みだけで 1 ファイルにつき 5 秒強かかっていた。中身はどの
# ファイルでも同じなので、一度だけ解釈して使い回す。
# 手元 (28 並列・Release) で 2686VFX の 80 ファイルが 41 秒から 19 秒
# (PCH の作成込み) になった。
#
# 代わりに 1 つ 400MB 前後 (実測 373MB) の PCH が的とビルド構成の数だけ出来る。
# 置き場所が厳しいときは切る。
#
# もう 1 つ、並列度を上げすぎたときの注意がある。プラグインは 6 本あり、
# 同時に走るコンパイラがそれぞれ PCH をメモリへ載せるため、上限なしの
# 並列 (cmake --build ... --parallel) で全部を一度に組むと、
#
#   error C3859: PCH の仮想メモリを作成できませんでした
#   error C1076: コンパイラの制限: 内部ヒープの上限に達しました
#
# が出ることがある。物理メモリ 64GB でも起きた。決まって出るわけではなく、
# 直前のビルドの後などメモリが詰まっているときに出やすい。
#
# 出たときは並列度を絞れば通る (--parallel 4 で確認)。もう一度叩くだけでも
# 通ることが多い。恒久的に避けたいときは CY_USE_PCH を切る。
option(CY_USE_PCH "先読みヘッダ (PCH) を使ってビルドを速くする" ON)

# JUCE 自身のモジュール実装ファイル。これらは PCH から外すので先に集めておく。
file(GLOB CY_JUCE_MODULE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.m"
)

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
# 音源として作る (既定)。中身は create_cyross_target が持つ。
function(create_cyross_plugin TARGET_NAME CY_PLUGIN_CODE)
    if(ARGC GREATER 2)
        create_cyross_target(${TARGET_NAME} ${CY_PLUGIN_CODE} TRUE ${ARGV2})
    else()
        create_cyross_target(${TARGET_NAME} ${CY_PLUGIN_CODE} TRUE)
    endif()
endfunction()

# エフェクトとして作る。入力された音を加工するので IS_SYNTH は立てない。
# MIDI は受け取る。鍵盤の押し離しでエンベロープや LFO を動かすため。
function(create_cyross_effect TARGET_NAME CY_PLUGIN_CODE)
    if(ARGC GREATER 2)
        create_cyross_target(${TARGET_NAME} ${CY_PLUGIN_CODE} FALSE ${ARGV2})
    else()
        create_cyross_target(${TARGET_NAME} ${CY_PLUGIN_CODE} FALSE)
    endif()
endfunction()

function(create_cyross_target TARGET_NAME CY_PLUGIN_CODE CY_IS_SYNTH)
    if(ARGC GREATER 3)
        set(CY_PRODUCT_NAME ${ARGV3})
    else()
        set(CY_PRODUCT_NAME ${TARGET_NAME})
    endif()

    # 出す形。macOS では AU も求められることが多いので、そこでだけ足す。
    # 音源か効果かは IS_SYNTH から JUCE が判断し、AU の種別を決めてくれる。
    set(CY_PLUGIN_FORMATS VST3 Standalone)

    if(APPLE)
        list(APPEND CY_PLUGIN_FORMATS AU)
    endif()

    juce_add_plugin(${TARGET_NAME}
        COMPANY_NAME "CYROSS"
        IS_SYNTH ${CY_IS_SYNTH}
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE "CYRS"
        PLUGIN_CODE ${CY_PLUGIN_CODE}
        FORMATS ${CY_PLUGIN_FORMATS}
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
        PRODUCT_NAME ${CY_PRODUCT_NAME}
        VST3_AUTO_MANIFEST FALSE
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /utf-8 /FS)
    endif()

    # ALL_SOURCES は呼び出し元のディレクトリスコープで定義されたものが使われます
    target_sources(${TARGET_NAME} PRIVATE ${ALL_SOURCES})

    # Linux で必要になる部品。JUCE が挙げているものをそのまま並べる。
    # Xext と Xrender は他の部品づてに入ることが多いが、それに頼ると
    # 素の環境で結びつけに失敗する。
    if(UNIX AND NOT APPLE)
        target_link_libraries(${TARGET_NAME} PRIVATE
            freetype fontconfig
            X11 Xext Xrender Xcursor Xinerama Xrandr Xcomposite
            asound GL
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

    # 先読みヘッダを当てる。自前のソースはどれも JuceHeader.h から始まるので、
    # 全ファイルに効く。
    if(CY_USE_PCH)
        target_precompile_headers(${TARGET_NAME} PRIVATE "<JuceHeader.h>")

        # JUCE のモジュール実装は「他のヘッダより先に置くこと」を前提に書かれて
        # いて、先頭で二重取り込みを #error で弾く。PCH は各ファイルの頭へ
        # 差し込まれるのでそのまま当たってしまう。ここだけ外す。
        set_source_files_properties(${CY_JUCE_MODULE_SOURCES}
            TARGET_DIRECTORY ${TARGET_NAME}
            PROPERTIES SKIP_PRECOMPILE_HEADERS ON
        )
    endif()
endfunction()

# 4つのプラグインを一括でビルド対象に追加
add_subdirectory(2686V)
add_subdirectory(2686VLight)
add_subdirectory(2686VOrigin)
add_subdirectory(26V)
add_subdirectory(OPZX7S)
add_subdirectory(2686VFX)

# マニュアル用の道具 (出荷するものではない)
add_subdirectory(tools)

# ユニットテスト (JUCE を読み込まずロジック層だけを検証する)
#
# 別の CPU 向けに組むときは、出来た実行ファイルをこの機械で走らせられない。
# それでも組むこと自体には意味がある (その CPU で通らない書き方に気づける)
# ので、既定では組んで、走らせる登録だけを飛ばす。まるごと外したいときは
# CY_BUILD_TESTS を切る。
option(CY_BUILD_TESTS "ユニットテストを組む" ON)

if(CY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
