// ============================================================================
// 共有コード (Shared/) を組むときの JuceHeader.h
// ============================================================================
// プラグインでは JUCE がプラグインごとに JuceHeader.h を書き出す。中身は
// モジュールのヘッダーの一覧と、プラグイン名などを持つ ProjectInfo。
//
// 共有コードは 12 本で 1 つなので、自分がどのプラグインに入るのかを知らない。
// そこでモジュールの一覧だけを持ち、ProjectInfo は置かない。共有コードで
// ProjectInfo や JucePlugin_Name を使うと、ここでコンパイルが止まる。
//
// 一覧はプラグインの JuceHeader.h と同じにしておくこと (cy_juce がつなぐ
// モジュールと同じ)。共有コードのヘッダーはプラグインのファイルからも
// 読まれるので、見える宣言が食い違うと困る。
#pragma once

#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_dsp/juce_dsp.h>

#if ! DONT_SET_USING_JUCE_NAMESPACE
 using namespace juce;
#endif
