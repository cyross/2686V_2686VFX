#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace SsgPrName
{
	static inline const juce::String tone = " Tone";
	static inline const juce::String toneNoiseMix = " Tone/Noise Mix";
	static inline const juce::String waveform = " Waveform";
	static inline const juce::String bit = " BIT";
	static inline const juce::String rate = " RATE";

	namespace Noise
	{
		static inline const juce::String title = " Noise";
		static inline const juce::String freq = " Noise Freq";
		static inline const juce::String noiseOnNote = " Noise On Note";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
		static inline const juce::String ar = " AR";
		static inline const juce::String dr = " DR";
		static inline const juce::String sl = " SL";
		static inline const juce::String rr = " RR";
	}

	namespace Duty
	{
		static inline const juce::String mode = " Duty Mode";
		static inline const juce::String preset = " Duty Preset";
		static inline const juce::String var = " Duty Var";
		static inline const juce::String invert = " Duty Invert";
	}

	namespace Tri
	{
		static inline const juce::String keyTrack = " Triangle Key Track";
		static inline const juce::String peak = " Triangle Peak";
		static inline const juce::String manualFreq = " Tri Manual Freq";
	}

	namespace HwEnv
	{
		static inline const juce::String enable = " HW Env Enable";
		static inline const juce::String shape = " Env Shape";
		static inline const juce::String period = " Env Period";
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = " PITCH-ENV Bypass";
	};

	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;
}
