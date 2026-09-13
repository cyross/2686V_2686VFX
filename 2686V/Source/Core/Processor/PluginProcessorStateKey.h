#pragma once

#include <JuceHeader.h>

namespace ProcessorStateKey {
	static const juce::String windowMode = "windowViewMode";
	static const juce::String opzx7ViewMode = "opzx7ViewMode";
	static const juce::String rhythmViewMode = "rhythmViewMode";
	static const juce::String isVisiblePreview = "isVisiblePreview";
	// TARGET が指しているもの。画面を閉じても覚えておく。
	// 開き直すたびに 1 番へ戻ると、続きから触れない。
	static const juce::String rhythmTarget = "rhythmTarget";
}
