#pragma once

#include <JuceHeader.h>

namespace ProcessorStateKey {
	static const juce::String windowMode = "windowViewMode";
	static const juce::String isVisiblePreview = "isVisiblePreview";
	// TARGET が指しているもの。画面を閉じても覚えておく。
	// 開き直すたびに 1 番へ戻ると、続きから触れない。
	static const juce::String rhythmTarget = "rhythmTarget";
	// FM のタブで TARGET が指しているオペレータ。画面を閉じても覚えておく。
	static const juce::String opnaTarget = "opnaTarget";
	static const juce::String opnTarget = "opnTarget";
	static const juce::String oplTarget = "oplTarget";
	static const juce::String opl3Target = "opl3Target";
	static const juce::String opmTarget = "opmTarget";
	static const juce::String opzx7Target = "opzx7Target";
}
