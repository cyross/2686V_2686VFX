#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace RhythmPrName
{
	static inline const juce::String vol = " Vol";
	static inline const juce::String pad = " Pad";

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrName::Unison::voices;
		static inline const juce::String detune = CorePrName::Unison::detune;
		static inline const juce::String spread = CorePrName::Unison::spread;
	}

	namespace Pad
	{
		static inline const juce::String vol = " Vol";
		static inline const juce::String note = " Note";
		static inline const juce::String rate = " Rate";
		static inline const juce::String bit = " Bit";
		static inline const juce::String oneShot = " One Shot";
		static inline const juce::String pan = " Pan";
		static inline const juce::String rr = " RR";
		static inline const juce::String pcmOffset = " PCM Offset";
		static inline const juce::String pcmRatio = " PCM Ratio";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
		static inline const juce::String ar = " AR";
		static inline const juce::String dr = " DR";
		static inline const juce::String sl = " SL";
		static inline const juce::String rr = " RR";
		static inline const juce::String kor = " KOR";
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = " PitchEnv Enable";
		static inline const juce::String bypass = " PitchEnv Bypass";
		static inline const juce::String ar = " PitchEnv Ar";
		static inline const juce::String dr = " PitchEnv Dr";
		static inline const juce::String rr = " PitchEnv Rr";
		static inline const juce::String stl = " PitchEnv Stl";
		static inline const juce::String atl = " PitchEnv Atl";
		static inline const juce::String ssl = " PitchEnv Ssl";
		static inline const juce::String rll = " PitchEnv Rll";
	};
}
