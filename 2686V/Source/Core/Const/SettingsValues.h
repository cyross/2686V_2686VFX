#pragma once

#include <JuceHeader.h>

namespace SettingsValue
{
	namespace Initial
	{
		static inline constexpr bool showTooltip = true;
		static inline constexpr bool useHeadroom = true;
		static inline constexpr float headroomGain = 0.25f;
		static inline constexpr bool showVirtualKeyboard = true;
	};

	namespace File
	{
		static inline const juce::String glob = "*.xml";

		namespace Name
		{
			static inline const juce::String initial = "init_settings.xml";
			static inline const juce::String def = "2686V_Config.xml";
		}
	}
};
