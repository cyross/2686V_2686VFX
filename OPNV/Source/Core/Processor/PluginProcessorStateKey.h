#pragma once

#include <JuceHeader.h>

namespace ProcessorStateKey {
	static const juce::String windowMode = "windowViewMode";
	static const juce::String isVisiblePreview = "isVisiblePreview";
	// FM のタブで TARGET が指しているオペレータ。画面を閉じても覚えておく。
	static const juce::String opnaTarget = "opnaTarget";
	static const juce::String opnTarget = "opnTarget";
}
