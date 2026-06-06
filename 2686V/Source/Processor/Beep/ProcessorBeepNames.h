#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/ProcessorNames.h"

// UI上に表示するタイトル等を管理
namespace BeepPrName
{
	static inline const juce::String level = " Level";
	static inline const juce::String fix = " Fix Enable";
	static inline const juce::String fixFreq = " Fix Freq";

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrName::Unison::voices;
		static inline const juce::String detune = CorePrName::Unison::detune;
		static inline const juce::String spread = CorePrName::Unison::spread;
	}

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
		static inline const juce::String ar = " AR";
		static inline const juce::String dr = " DR";
		static inline const juce::String sl = " SL";
		static inline const juce::String rr = " RR";
		static inline const juce::String stl = " STL";
		static inline const juce::String kor = " KOR";
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = " PitchEnv Bypass";
	};

	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String mulRatio = " MUL Ratio";
	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;
	static inline const juce::String dt3 = CorePrName::Fm::Op::dt3;

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
}
