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
		static inline const juce::String stl = " STL";
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

	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String mulRatio = " MUL Ratio";
	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = " SSG SwEnv Bypass";
		static inline const juce::String steps = " SSG SwEnv Steps";
		static inline const juce::String loop = " SSG SwEnv Loop";
		static inline const juce::String loopTo = " SSG SwEnv Loop To";
		static inline const juce::String loopCount = " SSG SwEnv Loop Count";
		static inline const juce::String stl = " SSG SwEnv STL";
		static inline const juce::String r1 = " SSG SwEnv R1";
		static inline const juce::String l1 = " SSG SwEnv L1";
		static inline const juce::String r2 = " SSG SwEnv R2";
		static inline const juce::String l2 = " SSG SwEnv L2";
		static inline const juce::String r3 = " SSG SwEnv R3";
		static inline const juce::String l3 = " SSG SwEnv L3";
		static inline const juce::String r4 = " SSG SwEnv R4";
		static inline const juce::String l4 = " SSG SwEnv L4";
		static inline const juce::String r5 = " SSG SwEnv R5";
		static inline const juce::String l5 = " SSG SwEnv L5";
		static inline const juce::String r6 = " SSG SwEnv R6";
		static inline const juce::String l6 = " SSG SwEnv L6";
	}

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
