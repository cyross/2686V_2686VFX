// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace BeepGuiText
{
	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "ビープ音チャンネル設定";
	}

	namespace Beep
	{
		static inline const juce::String Level = u8"Level";

		namespace Adsr
		{
			static inline const juce::String Bypass = u8"Bypass";
		}
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = u8"Bypass";
	}
}
