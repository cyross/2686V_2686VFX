cmake_minimum_required(VERSION 3.22)

option(PROJECT_NAME "Project Name" "2686V")

# プロジェクト名とバージョン
project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES C CXX)

# C++20標準を使用 (最新のJUCEとymfmの互換性のため)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# VST3 SDKの設定
# JUCEをadd_subdirectoryする前に設定する必要があります
# -----------------------------------------------------------------------------
set(JUCE_VST3_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/vst3sdk" CACHE PATH "Path to the VST3 SDK" FORCE)

# -----------------------------------------------------------------------------
# JUCEの読み込み
# -----------------------------------------------------------------------------
add_subdirectory(external/JUCE)

# -----------------------------------------------------------------------------
# サブプロジェクトの追加
# -----------------------------------------------------------------------------
add_subdirectory(2686V)
