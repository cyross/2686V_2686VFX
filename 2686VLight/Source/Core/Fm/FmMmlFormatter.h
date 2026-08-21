#pragma once

#include <JuceHeader.h>

namespace FmMml
{
	static const juce::String basicMmlHeader = u8"' BASIC MML\n";
	static const juce::String extMmlHeader = u8"' 2686V MML\n";

	static auto int2dt = [](int dt) {
		if (dt == 0 || dt == 4) {
			return 0;
		}

		if (dt >= 5 && dt <= 7)
		{
			return dt - 4; // 5 -> 1 / 6 -> 2 / 7 -> 3
		}

		return dt - 4; // 1 -> -3 / 2 -> -2 / 3 -> -1
		};
	static auto bool2Int = [](bool val) { return val ? 1 : 0; };
	static auto genMask2 = [](bool mask1, bool mask2) { return (mask1 << 1) + mask2; };
	static auto genMask4 = [](bool mask1, bool mask2, bool mask3, bool mask4) { return (mask1 << 3) + (mask2 << 2) + (mask3 << 1) + mask4; };
};
