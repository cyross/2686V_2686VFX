cmake_minimum_required(VERSION 3.22)

# ソリューション（ワークスペース）全体の名前
project(RetroSynthsBundle VERSION 3.6.3 LANGUAGES C CXX)

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
# 並列度を絞ると出にくくなるが、--parallel 4 でも出たことがあるので確実ではない。
# 出たときはもう一度叩けばたいてい通る (途中まで出来ているぶんは組み直さない)。
# 恒久的に避けたいときは CY_USE_PCH を切る。
option(CY_USE_PCH "先読みヘッダ (PCH) を使ってビルドを速くする" ON)

# JUCE 自身のモジュール実装ファイル。これらは PCH から外すので先に集めておく。
file(GLOB CY_JUCE_MODULE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/modules/*/*.m"
)

# 出す形。macOS では AU も求められることが多いので、そこでだけ足す。
# 音源か効果かは IS_SYNTH から JUCE が判断し、AU の種別を決めてくれる。
#
# 12 本とも同じにしておくこと。下の cy_juce は、この形に合わせて JUCE を組む。
set(CY_PLUGIN_FORMATS VST3 Standalone)

if(APPLE)
    list(APPEND CY_PLUGIN_FORMATS AU)
endif()

# JUCE の機能の入り切り。プラグインと cy_juce で必ず揃える。
set(CY_JUCE_DEFINITIONS
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_CAMERA=0
    JUCE_USE_CDREADER=0
    JUCE_USE_CDBURNER=0
)

# ==============================================================================
# JUCE を 1 本の静的ライブラリにまとめる (cy_juce)
# ==============================================================================
# JUCE のモジュールの実装 (juce_gui_basics.cpp など 18 個) は、どのプラグインでも
# 中身が同じ。それでも juce_add_plugin に任せると、プラグインごとに組み直す
# ので、12 本で 12 回コンパイルしていた。ここで 1 回だけ組んで使い回す。
#
# 仕組みは次のとおり。
#
#   - cy_juce は、プラグインの共有コードと同じ定義でモジュールを組む
#   - プラグインの側では、同じモジュールのファイルに「コンパイルしない」印
#     (HEADER_FILE_ONLY) を付ける。juce_add_plugin はモジュールをソースごと
#     つなぐので、印を付けないと二重に組まれる。印はディレクトリごとに持つ
#     ので、ここ (ルート) で組む cy_juce には効かない
#   - プラグイン名だけはプラグインごとに違うので、関数で尋ねる形にする
#     (cmake/SharedJuce/CyJucePluginName.h)
#
# 共有してよいのは、JUCE のモジュールの実装がプラグインごとの値に頼っていない
# から。頼っているのはプラグイン名と出す形 (CY_PLUGIN_FORMATS) だけで、出す形は
# 12 本とも同じ。JUCE を上げたときは、プラグインの包み (juce_audio_plugin_client)
# の外で JucePlugin_ で始まる名前を使うところが増えていないかを確かめること。
#
# 道具 (ParamDump) とテスト (IoTests) はコンソールアプリとして JUCE を組むので、
# 定義が違う。これらはこれまでどおり自分で組む。
# プラグインの形式ごとの包み (VST3 / Standalone) が組むものは除く。
# こちらは中でプラグイン名などを使うので、プラグインごとに組む。
set(CY_JUCE_SHARED_SOURCES ${CY_JUCE_MODULE_SOURCES})
list(FILTER CY_JUCE_SHARED_SOURCES EXCLUDE REGEX "/juce_audio_plugin_client/")

add_library(cy_juce STATIC)

# モジュールをつなぐと、その実装ファイルがこのライブラリへ入る。
#
# juce_audio_plugin_client はここでは何も組まないが、「使える」という印
# (JUCE_MODULE_AVAILABLE_juce_audio_plugin_client) を立てるためにつなぐ。
# juce_gui_basics はこの印を見て、プラグインの窓の扱いを変える。
target_link_libraries(cy_juce PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_plugin_client
    juce::juce_recommended_config_flags
)

# プラグインの共有コードと同じ定義にする。juce_add_plugin が中で付けている
# もののうち、モジュールの実装が見るものを写す。出す形の印
# (JucePlugin_Build_VST3 など) は JUCE 自身の関数で付ける。
_juce_add_plugin_definitions(cy_juce PRIVATE ${CY_PLUGIN_FORMATS})

target_compile_definitions(cy_juce PRIVATE
    ${CY_JUCE_DEFINITIONS}
    JUCE_SHARED_CODE=1
    JUCE_STANDALONE_APPLICATION=JucePlugin_Build_Standalone
    JucePlugin_Enable_ARA=0
)

# プラグイン名の受け口を、どのファイルよりも先に読ませる
if(MSVC)
    target_compile_options(cy_juce PRIVATE /utf-8 /FS
        "/FI${CMAKE_CURRENT_SOURCE_DIR}/cmake/SharedJuce/CyJucePluginName.h")
else()
    target_compile_options(cy_juce PRIVATE
        -include "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SharedJuce/CyJucePluginName.h")
endif()

# プラグインの共有コードに JUCE が付けているものと揃える
set_target_properties(cy_juce PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    VISIBILITY_INLINES_HIDDEN TRUE
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
)

# JUCE を「プラグインと同じ定義で読む」ための口。共有コード (Shared/) が使う。
#
# cy_juce がモジュールから受け取っている取り込み先と定義を、そのまま写す。
# モジュールそのものはつながないので、実装ファイルが二重に入ることはない。
# JUCE がプラグインの包み (VST3 など) を組むときと同じやり方。
add_library(cy_juce_usage INTERFACE)

target_include_directories(cy_juce_usage INTERFACE
    $<TARGET_PROPERTY:cy_juce,INCLUDE_DIRECTORIES>)

target_compile_definitions(cy_juce_usage INTERFACE
    $<TARGET_PROPERTY:cy_juce,COMPILE_DEFINITIONS>)

target_link_libraries(cy_juce_usage INTERFACE
    cy_juce
    juce::juce_recommended_config_flags
)

# 共有コード (静的ライブラリ)。プラグインより先に作る。
add_subdirectory(Shared)

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

    # 出す形 (CY_PLUGIN_FORMATS) は上で 12 本ぶんまとめて決めてある
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

    target_compile_definitions(${TARGET_NAME} PRIVATE ${CY_JUCE_DEFINITIONS})

    # JUCE は cy_juce で組んだものを使う。自分では組まない。
    target_link_libraries(${TARGET_NAME} PRIVATE cy_juce)

    set_source_files_properties(${CY_JUCE_SHARED_SOURCES}
        TARGET_DIRECTORY ${TARGET_NAME}
        PROPERTIES HEADER_FILE_ONLY ON
    )

    # cy_juce へこのプラグインの名前を返す
    target_sources(${TARGET_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/cmake/SharedJuce/CyJucePluginName.cpp")

    # 12 本で共有するコード (Shared/)
    target_link_libraries(${TARGET_NAME} PRIVATE cy_shared)

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

    # --------------------------------------------------------------------
    # 出来上がりを out/bin へ集める
    # --------------------------------------------------------------------
    # ビルドの置き場は
    #   out/build/<プリセット>/<プロジェクト>/<的>_artefacts/<構成>/...
    # と深いうえ、プロジェクト名と製品名が違うもの (2686VOrigin -> 86V) も
    # あるので、配布物を手で集めるのに向かない。組み上がるたびに
    #   out/bin/<プリセット>/<構成>/
    # へ写しておく。<プリセット> は組み立て先のフォルダ名から採るので、
    # windows-x64-msvc と windows-arm64-msvc が混ざることはない。
    get_filename_component(CY_BUILD_NAME "${CMAKE_BINARY_DIR}" NAME)

    set(CY_BIN_DIR "${CMAKE_SOURCE_DIR}/out/bin/${CY_BUILD_NAME}/$<CONFIG>")

    if(TARGET ${TARGET_NAME}_Standalone)
        add_custom_command(TARGET ${TARGET_NAME}_Standalone POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CY_BIN_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:${TARGET_NAME}_Standalone>" "${CY_BIN_DIR}"
            COMMENT "${CY_PRODUCT_NAME}: スタンドアロンを out/bin へ写す"
            VERBATIM
        )
    endif()

    if(TARGET ${TARGET_NAME}_VST3)
        # VST3 はフォルダごと。出来上がりの場所は JUCE が的へ持たせている。
        #
        # ここは $<TARGET_PROPERTY:...> で読んではいけない。この値自体が
        # 生成式を含んでおり、その中身までは展開されずに文字列のまま渡って
        # しまう。JUCE 自身と同じく、組み立て時に取り出して使う。
        get_target_property(CY_VST3_SRC ${TARGET_NAME}_VST3 JUCE_PLUGIN_ARTEFACT_FILE)

        # 中に前回の残りが混ざらないよう、消してから入れ直す。
        add_custom_command(TARGET ${TARGET_NAME}_VST3 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CY_BIN_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E rm -rf
                "${CY_BIN_DIR}/${CY_PRODUCT_NAME}.vst3"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CY_VST3_SRC}"
                "${CY_BIN_DIR}/${CY_PRODUCT_NAME}.vst3"
            COMMENT "${CY_PRODUCT_NAME}: VST3 を out/bin へ写す"
            VERBATIM
        )

        # フォルダにプラグインの絵を出すための後始末。
        #
        # Windows は desktop.ini を「システム属性の付いたフォルダ」でしか
        # 見に行かない。copy_directory は中身しか写さず属性は落ちるので、
        # JUCE が組み立て先へ付けているのと同じものを付け直す。
        # これを忘れるとただのフォルダの絵になる。
        if(WIN32)
            add_custom_command(TARGET ${TARGET_NAME}_VST3 POST_BUILD
                COMMAND attrib +s "${CY_BIN_DIR}/${CY_PRODUCT_NAME}.vst3/desktop.ini"
                COMMAND attrib +s "${CY_BIN_DIR}/${CY_PRODUCT_NAME}.vst3"
                VERBATIM
            )
        endif()
    endif()
endfunction()

# ビルド対象のプラグイン
add_subdirectory(2686V)
add_subdirectory(2686VLight)
add_subdirectory(2686VOrigin)
add_subdirectory(26V)
add_subdirectory(OPZX7S)
add_subdirectory(2686VFX)

# 音源を絞ったもの (tools/gen_plugin.py が作る)
add_subdirectory(OPNV)
add_subdirectory(OPLV)
add_subdirectory(OPMV)
add_subdirectory(WTV)
add_subdirectory(PCMV)
add_subdirectory(PULSEV)

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
