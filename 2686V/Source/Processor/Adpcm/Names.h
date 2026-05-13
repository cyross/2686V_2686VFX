#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace PrName
{
	static inline const juce::String pan = " Pan";
	static inline const juce::String rate = " Rate";
	static inline const juce::String level = " Level";
	static inline const juce::String loop = " Loop";
	static inline const juce::String bit = " Bit";
	static inline const juce::String pcmOffset = " PCM Offset";
	static inline const juce::String pcmRatio = " PCM Ratio";

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
		static inline const juce::String ar = " AR";
		static inline const juce::String dr = " DR";
		static inline const juce::String sl = " SL";
		static inline const juce::String rr = " RR";
	}
}
