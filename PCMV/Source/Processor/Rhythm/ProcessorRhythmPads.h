#pragma once

// ============================================================================
// このプラグインが使う RHYTHM のパッドの数
// ============================================================================
// 12 本で共有する RhythmCore は置き場を RhythmPrValue::maxPads だけ持つ。
// そのうちいくつを使うかは、プラグインごとに決める。パラメータの登録や画面は
// この数で並べ、RhythmCore へも SynthVoice からこの数を渡す。
#include "Shared/Processor/Rhythm/ProcessorRhythmValues.h"

namespace RhythmPrValue
{
	inline constexpr int pads = 8;

	static_assert(pads <= maxPads, "パッドの数が置き場を超えている");
}
