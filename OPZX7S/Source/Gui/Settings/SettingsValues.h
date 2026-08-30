#pragma once

#include <JuceHeader.h>

namespace SettingsValue
{
	namespace Initial
	{
		static inline constexpr bool showTooltip = true;
		static inline constexpr bool useHeadroom = true;
		static inline constexpr float headroomGain = 0.5f;
		static inline constexpr bool showVirtualKeyboard = true;
	};

	namespace File
	{
		// 読み込みは JSON でも YAML でもよい
		static inline const juce::String glob = "*.settings.json;*.settings.yaml";

		namespace Name
		{
			// 拡張子は書き出す形で決まるので、ここでは名前だけを持つ。
			// 起動時に読むファイルは AudioPlugin2686V::getStartupSettingsFile
			// が、あるほうを選ぶ。
			static inline const juce::String initial = "init.settings";
			static inline const juce::String def = "OPZX7S.settings";
		}
	}
};
