cmake_minimum_required(VERSION 3.22)

# プロジェクト名 (ソリューション名)
project(2686V_Bundle VERSION 1.2.6 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_VST3_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/vst3sdk" CACHE PATH "Path to the VST3 SDK" FORCE)

# JUCEの読み込み (ここで1回だけJUCEが構成・ビルドされます！)
add_subdirectory(external/JUCE)

add_subdirectory(2686V)
