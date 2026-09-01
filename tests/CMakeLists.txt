# ==============================================================================
# ユニットテスト
# ==============================================================================
# 音を決めているロジック (Effect / Generator / Advanced/Curve) を、JUCE を
# 読み込まずにそのままコンパイルして検証する。
#
# これらの層が JUCE から使っているのは juce::MathConstants だけなので、
# tests/shim/JuceHeader.h をインクルードパスの先頭に置いて差し替える。
# テスト対象のソースは出荷されるものと同一ファイルを指す。
#
# 4 プラグインはソースを丸ごと複製しているため、機能が最も多い 2686V を
# 代表としてテストし、複製先とのズレは別途 drift テストで検出する。

set(TEST_TARGET_SOURCE_ROOT "${CMAKE_SOURCE_DIR}/2686V/Source")

# ---- テスト対象 (出荷されるソースそのもの) ----
set(UNDER_TEST_SOURCES
    "${TEST_TARGET_SOURCE_ROOT}/Advanced/Curve/AdvancedCurve.cpp"

    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddr.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssr.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/OplAdsr/EnvOplAdsr.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/SsgHw/EnvSsgHw.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/SsgSw/EnvSsgSw.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.cpp"

    "${TEST_TARGET_SOURCE_ROOT}/Effect/KeyScale/Ma7/KSMa7.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/KeyScale/Opl/KSOpl.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/KeyScale/Opn/KSOpn.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/KeyScale/Opp/KSOpp.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/KeyScale/Ops/KSOps.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/KeyScale/Opz/KSOpz.cpp"

    "${TEST_TARGET_SOURCE_ROOT}/Effect/Detune/Opl/DetuneOpl.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Detune/Opm/DetuneOpm.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Detune/Opn/DetuneOpn.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Detune/Opzx7/DetuneOpzx7.cpp"

    "${TEST_TARGET_SOURCE_ROOT}/Effect/Feedback/Feedback.cpp"

    "${TEST_TARGET_SOURCE_ROOT}/Effect/Lfo/N88/LfoN88.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Lfo/Opm/LfoOpm.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Lfo/Opzx7/LfoOpzx7Unit.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Generator/Noise/Lfsr/GenNoiseLfsr.cpp"

    "${TEST_TARGET_SOURCE_ROOT}/Core/Fm/FmOperator.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Synth/Opzx7/Operator/SynthOpzx7Op.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Effect/Lfo/Opzx7/LfoOpzx7.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Generator/Fm/Fix/FmFix.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Gui/Components/WavePreview/WavePreviewSource.cpp"
)

# ---- テストコード ----
file(GLOB TEST_CASE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cases/*.cpp")

add_executable(RetroSynthsTests
    "${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cpp"
    ${TEST_CASE_SOURCES}
    ${UNDER_TEST_SOURCES}
)

target_include_directories(RetroSynthsTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/shim"     # JuceHeader.h の差し替え。必ず先頭。
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${TEST_TARGET_SOURCE_ROOT}"
    "${CMAKE_SOURCE_DIR}/external/doctest"
)

target_compile_definitions(RetroSynthsTests PRIVATE
    TEST_SOURCE_ROOT="${CMAKE_SOURCE_DIR}"

    # doctest は既定で <tuple> などを取り込まず namespace std へ前方宣言を置く。
    # 標準では禁じられている書き方で、MSVC が C5285 で警告する。
    # doctest 側が用意しているこのスイッチを立てると本物の標準ヘッダを
    # 読むようになり、前方宣言そのものが消える。
    DOCTEST_CONFIG_USE_STD_HEADERS
)

target_compile_features(RetroSynthsTests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(RetroSynthsTests PRIVATE /utf-8 /FS)
endif()

# ------------------------------------------------------------------------------
# CTest への登録
# ------------------------------------------------------------------------------
# doctest_discover_tests を使うと TEST_CASE が 1 件ずつ CTest に並ぶので、
# Visual Studio のテストエクスプローラーでも個別に実行・結果確認ができる。
#
# ただしこの仕組みは、並べる中身を知るために「組んだ実行ファイルをその場で
# 走らせる」。別の CPU 向けに組んでいるときは走らせられず、そこで組み立て
# 全体が止まってしまう。走らせられるときだけ登録する。
#
# ARM64 の Windows は x64 のものも動かせるので、そちらが親なら何でも走る。
# x64 の親から ARM64 を組んだときだけが、走らせられない組み合わせになる。
set(CY_TESTS_RUNNABLE ON)

# 別の OS 向けに組むときは CMake が教えてくれる。Linux の載せ替えは
# こちらで拾える。
if(CMAKE_CROSSCOMPILING)
    set(CY_TESTS_RUNNABLE OFF)
endif()

# Visual Studio で CPU だけを変えるときは、同じ OS のままなので上では
# 拾えない。綴りはプリセットに書いたものがそのまま入る (arm64 とも
# ARM64 とも来る) ので、大文字にそろえてから比べる。
string(TOUPPER "${CMAKE_VS_PLATFORM_NAME}" CY_TARGET_PLATFORM)
string(TOUPPER "${CMAKE_HOST_SYSTEM_PROCESSOR}" CY_HOST_PLATFORM)

if(CY_TARGET_PLATFORM AND NOT CY_HOST_PLATFORM STREQUAL "ARM64")
    if(CY_TARGET_PLATFORM MATCHES "^ARM")
        set(CY_TESTS_RUNNABLE OFF)
    endif()
endif()

if(CY_TESTS_RUNNABLE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/doctest/scripts/cmake")
    include(doctest)

    doctest_discover_tests(RetroSynthsTests)
else()
    message(STATUS
        "テストは組むが CTest へは登録しない: "
        "${CMAKE_VS_PLATFORM_NAME} 向けの実行ファイルを "
        "${CMAKE_HOST_SYSTEM_PROCESSOR} のこの機械では走らせられないため")
endif()

# ==============================================================================
# パラメータファイルの読み書き
# ==============================================================================
# ParamFile は juce::var と yaml-cpp の上に載っているため、上のテストのように
# JUCE を差し替えられない。別の実行ファイルにして本物の JUCE を読み込む。
#
# 見ているのは主に、JSON と YAML のどちらで書いても同じ値へ戻ることと、
# 数に見える文字列が数として読み戻されないこと。
juce_add_console_app(IoTests PRODUCT_NAME "IoTests")

juce_generate_juce_header(IoTests)

target_sources(IoTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/io/ParamFileTests.cpp"
    "${TEST_TARGET_SOURCE_ROOT}/Core/Io/ParamFile.cpp"
)

target_include_directories(IoTests PRIVATE
    "${TEST_TARGET_SOURCE_ROOT}"
    "${CMAKE_SOURCE_DIR}/external/doctest"
)

target_link_libraries(IoTests PRIVATE
    yaml-cpp::yaml-cpp
    juce::juce_gui_basics
    juce::juce_recommended_config_flags
)

target_compile_definitions(IoTests PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    DOCTEST_CONFIG_USE_STD_HEADERS
)

target_compile_features(IoTests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(IoTests PRIVATE /utf-8 /FS)
endif()

if(CY_TESTS_RUNNABLE)
    doctest_discover_tests(IoTests)
endif()
