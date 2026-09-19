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
	// 86V は PC-9801-86 に合わせて 6 つ (BD / SD / TOP / HH / TOM / RIM)
	inline constexpr int pads = 6;

	static_assert(pads <= maxPads, "パッドの数が置き場を超えている");
}
