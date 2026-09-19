# ==============================================================================
# プラグインで共有するコード
# ==============================================================================
# 各プラグインの Source に 1 つずつ同じものを持っていたコードを、ここへ 1 つに
# まとめて静的ライブラリとして組む。12 本はここで組んだものをつなぐ。
#
# 置き方は各プラグインの Source と同じ形にしてある (Shared/Generator、
# Shared/Core/Synth など)。そのため Shared の中のファイルどうしは、これまで
# どおり相対で読み込み合える。プラグインの側からは、リポジトリの根からの
# 道筋で読み込む。
#
#   #include "Shared/Generator/Pcm/Adpcm/GenAdpcm.h"
#
# ここに置くコードは、どのプラグインに入るのかを知らない。プラグインごとに
# 違う値 (JucePlugin_Name、ProjectInfo、音源の有無、初期値など) は使わないこと。
# 違いが要るときは、名前を分けた実装を両方ここへ置き、どちらを使うかは
# プラグインの側が実行時に選ぶ。#if では切り替えない。

# ライブラリを 1 つ作る。
#   DIRS  … 中身にするディレクトリ (Shared からの相対)
#   LINKS … 頼る共有ライブラリ
function(cy_add_shared_library name)
    cmake_parse_arguments(ARG "" "" "DIRS;LINKS" ${ARGN})

    set(sources)

    foreach(dir IN LISTS ARG_DIRS)
        file(GLOB_RECURSE found CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.h"
        )
        list(APPEND sources ${found})
    endforeach()

    add_library(${name} STATIC ${sources})

    # IDE では Shared の木の形のまま並べる
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Shared" FILES ${sources})

    # JUCE はプラグインと同じ定義で読む (cy_juce_usage)。共有コード用の
    # JuceHeader.h は ProjectInfo を持たない。
    target_link_libraries(${name} PRIVATE cy_juce_usage)
    target_link_libraries(${name} PUBLIC ${ARG_LINKS})

    target_include_directories(${name}
        PRIVATE "${CMAKE_SOURCE_DIR}/cmake/SharedJuce/include"
        PUBLIC "${CMAKE_SOURCE_DIR}"
    )

    target_compile_features(${name} PRIVATE cxx_std_20)

    if(MSVC)
        target_compile_options(${name} PRIVATE /utf-8 /FS)
    endif()

    set_target_properties(${name} PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
        VISIBILITY_INLINES_HIDDEN TRUE
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
    )

    if(CY_USE_PCH)
        target_precompile_headers(${name} PRIVATE "<JuceHeader.h>")
    endif()

    set_property(GLOBAL APPEND PROPERTY CY_SHARED_LIBRARIES ${name})
endfunction()

# ---- 土台。型や定数、小さな道具。パラメータの範囲と初期値 (CPV) もここ
#
# Core/Fm は FM のオペレーターで、エンベロープ (Effect) に頼るので音源の側へ置く。
cy_add_shared_library(cy_shared_core DIRS Core/Const Core/Processor Core/Synth Processor)

# ---- 音を作るもの (FM の FIX、ノイズ、PCM の符号化、波形メモリの変調など)
cy_add_shared_library(cy_shared_generator DIRS Generator LINKS cy_shared_core)

# ---- カーブ編集 (Curve)
#
# Curve を持たないプラグインにも入る。エンベロープなどはカーブが渡されて
# いない (CurveCore が空) ときに、カーブを持たない作りと同じ計算を通るので、
# 使うかどうかはプラグインが渡すかどうかで決まる。
cy_add_shared_library(cy_shared_advanced DIRS Advanced LINKS cy_shared_core)

# ---- 音にかける効果 (エンベロープ、LFO、デチューン、キースケール、フィードバック)
#
# エフェクター (Effect/Fx) はまだプラグインの側にある。音源のパラメータ全体
# (SynthParams) に頼っているため、プロセッサと一緒に移す。
cy_add_shared_library(cy_shared_effect DIRS Effect
    LINKS cy_shared_core cy_shared_generator cy_shared_advanced)

# ---- 音源の本体 (OPNA・OPM・SSG・WT・RHYTHM など) と FM のオペレーター
#
# どの音源を鳴らすか (OscMode) はプラグインごとに番号が違うので、ここには
# 置かない。音源の本体はパラメータを SynthCoreParams で受け取り、mode は
# 各プラグインの SynthParams が持つ。
cy_add_shared_library(cy_shared_synth DIRS Core/Fm Synth
    LINKS cy_shared_core cy_shared_generator cy_shared_advanced cy_shared_effect)

# プラグインはこれ 1 つをつなげば、共有コードをすべて使える
get_property(CY_SHARED_LIBRARY_LIST GLOBAL PROPERTY CY_SHARED_LIBRARIES)
add_library(cy_shared INTERFACE)
target_link_libraries(cy_shared INTERFACE ${CY_SHARED_LIBRARY_LIST})
