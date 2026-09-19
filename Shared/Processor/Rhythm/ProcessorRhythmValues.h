#pragma once

#include <JuceHeader.h>

namespace RhythmPrValue
{
	// パッドの置き場の数。12 本で共有する音源の本体 (RhythmCore) は、この数だけ
	// パッドを持つ。いくつ使うかはプラグインが決める (ProcessorRhythmPads.h の pads)。
	inline constexpr int maxPads = 8;
}
