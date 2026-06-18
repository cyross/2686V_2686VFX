// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorKeys.h"

namespace FxPrKey
{
	static inline const juce::String prefix = "FX";

	static inline const juce::String bypass = CorePrKey::bypass;

	static inline const juce::String vib = "_VIB";
	static inline const juce::String trm = "_TRM";
	static inline const juce::String dly = "_DLY";
	static inline const juce::String rvb = "_RVB";
	static inline const juce::String mbc = "_MBC";
	static inline const juce::String fil = "_FIL";
	static inline const juce::String scl = "_SCL";
	static inline const juce::String eq3b = "_EQ3B";
	static inline const juce::String sfcEcho = "_SFCECHO";

	static inline const juce::String mix = "_MIX";

	namespace Tremolo
	{
		static inline const juce::String rate = "_RATE";
		static inline const juce::String depth = "_DEPTH";
	};

	namespace Vibrato
	{
		static inline const juce::String rate = "_RATE";
		static inline const juce::String depth = "_DEPTH";
	};

	namespace Delay
	{
		static inline const juce::String time = "_TIME";
		static inline const juce::String fb = "_FB";
	};

	namespace Reverb
	{
		static inline const juce::String size = "_SIZE";
		static inline const juce::String damp = "_DAMP";
	};

	namespace Mbc
	{
		static inline const juce::String rate = "_RATE";
		static inline const juce::String bit = "_BITS";
	};

	namespace Filter
	{
		static inline const juce::String type = "_TYPE";
		static inline const juce::String freq = "_FREQ";
		static inline const juce::String q = "_Q";
	};

	namespace Eq3b
	{
		static inline const juce::String lowGainDb = "_LGDB";
		static inline const juce::String midFreq = "_MFREQ";
		static inline const juce::String midGainDb = "_MGDB";
		static inline const juce::String highGainDb = "_HGDB";
	}

	namespace SfcEcho
	{
		static inline const juce::String time = "_TIME";
		static inline const juce::String fb = "_FB";
		static inline const juce::String firCoef0 = "_FIR_COEF0";
		static inline const juce::String firCoef1 = "_FIR_COEF1";
		static inline const juce::String firCoef2 = "_FIR_COEF2";
		static inline const juce::String firCoef3 = "_FIR_COEF3";
		static inline const juce::String firCoef4 = "_FIR_COEF4";
		static inline const juce::String firCoef5 = "_FIR_COEF5";
		static inline const juce::String firCoef6 = "_FIR_COEF6";
		static inline const juce::String firCoef7 = "_FIR_COEF7";
	}
};

