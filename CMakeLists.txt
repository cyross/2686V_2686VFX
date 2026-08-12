cmake_minimum_required(VERSION 3.22)

set(BUILD_TARGET 3)

# プロジェクト名 (ソリューション名)
if(BUILD_TARGET EQUAL 1)
	project(2686V VERSION 1.0.0 LANGUAGES C CXX)
elseif(BUILD_TARGET EQUAL 2)
	project(2686V VERSION 1.0.0 LANGUAGES C CXX)
else()
	project(2686V VERSION 2.6.0 LANGUAGES C CXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_VST3_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/vst3sdk" CACHE PATH "Path to the VST3 SDK" FORCE)

# JUCEの読み込み (ここで1回だけJUCEが構成・ビルドされます！)
add_subdirectory(external/JUCE)

if(BUILD_TARGET EQUAL 1)
	add_subdirectory(OPZX7S)
elseif(BUILD_TARGET EQUAL 2)
	add_subdirectory(2686VL)
else()
	add_subdirectory(2686V)
endif()
