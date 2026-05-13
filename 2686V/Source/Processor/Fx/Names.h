#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace PrName
{
	static inline const juce::String masterBypass = "M.Bypass";

	static inline const juce::String rlfo = " Retro LFO";
	static inline const juce::String tremolo = " Tremolo";
	static inline const juce::String vibrato = " Vibrato";
	static inline const juce::String mbc = " Modern BC";
	static inline const juce::String delay = " Delay";
	static inline const juce::String reverb = " Reverb";
	static inline const juce::String rbc = " Retro BC";
	static inline const juce::String filter = " Filter";
	static inline const juce::String softClipper = " Soft Clipper";

	namespace Tremolo
	{
		static inline const juce::String bypass = " Bypass";
		static inline const juce::String rate = " Rate";
		static inline const juce::String depth = " Depth";
		static inline const juce::String mix = " Mix";
	}

	namespace Vibrato
	{
		static inline const juce::String bypass = " Bypass";
		static inline const juce::String rate = " Rate";
		static inline const juce::String depth = " Depth";
		static inline const juce::String mix = " Mix";
	}

	namespace Mbc
	{
		static inline const juce::String bypass = " Bypass";
		static inline const juce::String rate = " Rate";
		static inline const juce::String bit = " Bit";
		static inline const juce::String mix = " Mix";
	}

	namespace Delay
	{
		static inline const juce::String bypass = " Bypass";
		static inline const juce::String time = " Time";
		static inline const juce::String fb = " Feedback";
		static inline const juce::String mix = " Mix";
	}

	namespace Reverb
	{
		static inline const juce::String bypass = " Bypass";
		static inline const juce::String size = " Size";
		static inline const juce::String damp = " Damp";
		static inline const juce::String mix = " Mix";
	}

	namespace Filter
	{
		static inline const juce::String bypass = " Bypass";
		static inline const juce::String type = " Type";
		static inline const juce::String freq = " Freq";
		static inline const juce::String q = " Q";
		static inline const juce::String mix = " Mix";
	}
}
