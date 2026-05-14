// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace SsgPrKey
{
	static inline const juce::String prefix = "SSG";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String level = "_LEVEL";
	static inline const juce::String bit = "_BIT";
	static inline const juce::String rate = "_RATE";
	static inline const juce::String tone = "_LEVEL";
	static inline const juce::String noise = "_NOISE";
	static inline const juce::String noiseFreq = "_NOISE_FREQ";
	static inline const juce::String noiseOnNote = "_NOISE_ON_NOTE";
	static inline const juce::String mix = "_MIX";
	static inline const juce::String wveform = "_WAVEFORM";

	namespace Duty
	{
		static inline const juce::String mode = "_DUTY_MODE";
		static inline const juce::String preset = "_DUTY_PRESET";
		static inline const juce::String var = "_DUTY_VAR";
		static inline const juce::String inv = "_DUTY_INV";
	};

	namespace Tri
	{
		static inline const juce::String keyTrk = "_TRI_KEYTRK";
		static inline const juce::String peak = "_TRI_PEAK";
		static inline const juce::String freq = "_TRI_FREQ";
	};

	namespace HwEnv
	{
		static inline const juce::String enable = "_ENV_ENABLE";
		static inline const juce::String shape = "_ENV_SHAPE";
		static inline const juce::String period = "_ENV_PERIOD";
	};

	static inline const juce::String bypass = CorePrKey::bypass;
	static inline const juce::String adsr = CorePrKey::adsr;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;

	namespace Adsr
	{
		static inline const juce::String ar = CorePrKey::Adsr::ar;
		static inline const juce::String dr = CorePrKey::Adsr::dr;
		static inline const juce::String sl = CorePrKey::Adsr::sl;
		static inline const juce::String rr = CorePrKey::Adsr::rr;
	};

	namespace PitchAdsr
	{
		static inline const juce::String ar = CorePrKey::PitchAdsr::ar;
		static inline const juce::String dr = CorePrKey::PitchAdsr::dr;
		static inline const juce::String rr = CorePrKey::PitchAdsr::rr;
		static inline const juce::String stl = CorePrKey::PitchAdsr::stl;
		static inline const juce::String atl = CorePrKey::PitchAdsr::atl;
		static inline const juce::String ssl = CorePrKey::PitchAdsr::ssl;
		static inline const juce::String rll = CorePrKey::PitchAdsr::rll;
	};

	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;
};

