// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorKeys.h"

namespace SsgPrKey
{
	static inline const juce::String prefix = "SSG";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String level = "_LEVEL";
	static inline const juce::String bit = "_BIT";
	static inline const juce::String rate = "_RATE";
	static inline const juce::String tone = "_TONE";
	static inline const juce::String noise = "_NOISE";
	static inline const juce::String noiseFreq = "_NOISE_FREQ";
	static inline const juce::String noiseOnNote = "_NOISE_ON_NOTE";
	static inline const juce::String mix = "_MIX";
	static inline const juce::String wveform = "_WAVEFORM";
	static inline const juce::String fix = CorePrKey::Post::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrKey::Post::Fm::Op::fixFreq;

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrKey::Unison::voices;
		static inline const juce::String detune = CorePrKey::Unison::detune;
		static inline const juce::String spread = CorePrKey::Unison::spread;
	}

	namespace Duty
	{
		static inline const juce::String mode = "_DUTY_MODE";
		static inline const juce::String preset = "_DUTY_PRESET";
		static inline const juce::String var = "_DUTY_VAR";
		static inline const juce::String inv = "_DUTY_INV";
		static inline const juce::String fc = "_DUTY_FC_MODE";
		static inline const juce::String fcFluc = "_DUTY_FC_FLUC";
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
	static inline const juce::String ssgSwEnv = CorePrKey::ssgSwEnv;

	namespace Adsr
	{
		static inline const juce::String ar = CorePrKey::Adsr::ar;
		static inline const juce::String dr = CorePrKey::Adsr::dr;
		static inline const juce::String sl = CorePrKey::Adsr::sl;
		static inline const juce::String rr = CorePrKey::Adsr::rr;
		static inline const juce::String stl = CorePrKey::Adsr::stl;
		static inline const juce::String kor = "_KOR";
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

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String mulRatio = "_MUL_RATIO";
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;
	static inline const juce::String dt3 = CorePrKey::Post::Fm::Op::dt3;

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = "_SSGSW_BYPASS";
		static inline const juce::String steps = "_SSGSW_STEPS";
		static inline const juce::String stl = "_SSGSW_STL";
		static inline const juce::String loop = "_SSGSW_LOOP";
		static inline const juce::String loopTo = "_SSGSW_LOOPTO";
		static inline const juce::String loopCount = "_SSGSW_LOOPCNT";
		static inline const juce::String r1 = "_SSGSW_R1";
		static inline const juce::String l1 = "_SSGSW_L1";
		static inline const juce::String r2 = "_SSGSW_R2";
		static inline const juce::String l2 = "_SSGSW_L2";
		static inline const juce::String r3 = "_SSGSW_R3";
		static inline const juce::String l3 = "_SSGSW_L3";
		static inline const juce::String r4 = "_SSGSW_R4";
		static inline const juce::String l4 = "_SSGSW_L4";
		static inline const juce::String r5 = "_SSGSW_R5";
		static inline const juce::String l5 = "_SSGSW_L5";
		static inline const juce::String r6 = "_SSGSW_R6";
		static inline const juce::String l6 = "_SSGSW_L6";
	};

	namespace Lfo
	{
		static inline const juce::String pmFreq = "_LFO_PM_FREQ";
		static inline const juce::String amFreq = "_LFO_AM_FREQ";
		static inline const juce::String shape = "_LFO_SHAPE";
		static inline const juce::String pmShape = "_LFO_PM_SHAPE";
		static inline const juce::String amShape = "_LFO_AM_SHAPE";
		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String pm = "_LFO_PM";
		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String ams = "_LFO_AMS";
		static inline const juce::String pmd = "_LFO_PMD";
		static inline const juce::String amd = "_LFO_AMD";
		static inline const juce::String amSmoothRatio = "_LFO_ASMRT";
		static inline const juce::String pmSyncDelay = "_LFO_PM_SYNC_DELAY";
		static inline const juce::String amSyncDelay = "_LFO_AM_SYNC_DELAY";
	};
};

