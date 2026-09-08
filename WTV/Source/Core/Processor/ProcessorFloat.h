#pragma once

#include <JuceHeader.h>

#include "./ProcessorValues.h"

// ============================================================================
// 実数パラメータの作り口
// ============================================================================
// juce::AudioParameterFloat を直に組むと、JUCE が刻みを 0.01 に決めてしまう。
// 新しいパラメータもかならずここを通すこと。刻みの理由は
// ProcessorValues.h の floatInterval に書いてある。
//
// 刻みそのもの (floatInterval) は ProcessorValues.h に置いてある。
// あちらはテストの JuceHeader 差し替えを通るため、本物の JUCE を要る
// ものだけをこちらへ分けている。
namespace CPV
{
	// 刻みを細かくすると、JUCE の既定の表示は「0.5000」のように桁が増える。
	// 末尾の 0 を落として、これまでと同じ見た目に保つ。
	inline juce::String floatToText(float value, int maximumStringLength)
	{
		juce::String text{ value, CPV::floatTextDecimals };

		if (text.containsChar('.'))
		{
			text = text.trimCharactersAtEnd("0").trimCharactersAtEnd(".");
		}

		if (text == "-0")
		{
			text = "0";
		}

		return maximumStringLength > 0 ? text.substring(0, maximumStringLength) : text;
	}

	inline std::unique_ptr<juce::AudioParameterFloat> makeFloat(const juce::String& code, const juce::String& name, float min, float max, float initial)
	{
		return std::make_unique<juce::AudioParameterFloat>(
			juce::ParameterID{ code },
			name,
			juce::NormalisableRange<float>{ min, max, CPV::floatInterval },
			initial,
			juce::AudioParameterFloatAttributes().withStringFromValueFunction(&CPV::floatToText)
		);
	}
}
