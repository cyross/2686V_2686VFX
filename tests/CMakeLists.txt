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
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/doctest/scripts/cmake")
include(doctest)

doctest_discover_tests(RetroSynthsTests)
