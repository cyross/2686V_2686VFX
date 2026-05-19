#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace BeepPrName
{
	static inline const juce::String level = " Level";
	static inline const juce::String fix = " Fix Enable";
	static inline const juce::String fixFreq = " Fix Freq";

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
		static inline const juce::String ar = " AR";
		static inline const juce::String dr = " DR";
		static inline const juce::String sl = " SL";
		static inline const juce::String rr = " RR";
		static inline const juce::String stl = " STL";
	}
}
