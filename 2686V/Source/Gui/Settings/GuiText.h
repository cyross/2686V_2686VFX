// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace SettingsGuiText
{
	static inline const juce::String mode = u8"Mode";

	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	static inline const juce::String monoPoly = u8"MonoPhonic";

	namespace File
	{
		static inline const juce::String load = u8"Load";
		static inline const juce::String Pcm = u8"PCM";
		static inline const juce::String clear = u8"X";
	}

	namespace Group
	{
		static inline const juce::String settingEnv = u8"Environment Settings";
	}
}
