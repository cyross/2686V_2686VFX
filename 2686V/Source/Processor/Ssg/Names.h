#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace SsgPrName
{
	static inline const juce::String tone = " Tone";
	static inline const juce::String toneNoiseMix = " Tone/Noise Mix";
	static inline const juce::String waveform = " Waveform";
	static inline const juce::String bit = " Bit";
	static inline const juce::String rate = " Rate";

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
		static inline const juce::String enable = " Hardware Env Enable";
		static inline const juce::String shape = " Hardware Env Shape";
		static inline const juce::String period = " Hardware Env Period";
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = " PitchEnv Bypass";
	};

	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;

	namespace Lfo
	{
		static inline const juce::String pmFreq = " LFO PM Speed";
		static inline const juce::String amFreq = " LFO AM Speed";
		static inline const juce::String pmShape = " LFO PM Shape";
		static inline const juce::String amShape = " LFO AM Shape";
		static inline const juce::String am = " LFO Am Enable";
		static inline const juce::String pm = " LFO Pm Enable";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
		static inline const juce::String amSmoothRatio = " LFO AM Smooth Rate";
		static inline const juce::String syncDelay = " LFO Sync Delay";
	}
}
