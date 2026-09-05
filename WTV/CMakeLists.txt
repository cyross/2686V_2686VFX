cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# このファイルは tools/gen_plugin.py が書き出しています。手で直さないでください。
# 直したいときは 2686V か生成スクリプトを直して、流し直してください。
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS")
set(CY_COMPANY_NAME "CYROSS")

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout_WTV
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout_WTV
    HEADER_NAME "AppIconForAbout.h"
    NAMESPACE AppIconAboutForAboutData
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# IDE で元の木の形のまま並べる
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "WTV" FILES ${ALL_SOURCES})

create_cyross_plugin("WTV" "WTVO")
