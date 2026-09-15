// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace CoreGuiText
{
	// 大区分の見出し。小区分に同じ名前 (AMP ENV) があるので、括弧で見分ける。
	namespace MajorCategory
	{
		static inline const juce::String ampEnv = u8"[[AMP ENV]]";
		static inline const juce::String pitchEnv = u8"[[PITCH ENV]]";
	}

	namespace MasterVol
	{
		static inline const juce::String unit = u8"dB";
	}
}
