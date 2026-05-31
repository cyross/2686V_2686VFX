// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace SsgGuiText
{
	static inline const juce::String mode = u8"Mode";

	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	static inline const juce::String monoPoly = u8"MonoPhonic";

	namespace MasterVol
	{
		static inline const juce::String title = u8"M.Vol";
		static inline const juce::String unit = u8" dB";
	}

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String visibleLfo = u8"[■]--- LFO ---";
		static inline const juce::String invisibleLfo = u8"[□]--- LFO ---";
		static inline const juce::String visibleAdsr = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- AMP ENV ---";
		static inline const juce::String visiblePitchAdsr = u8"[■]--- PITCH ENV ---";
		static inline const juce::String invisiblePitchAdsr = u8"[□]--- PITCH ENV ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String shape = u8"--- ENVELOPE ---";
		static inline const juce::String visibleHwEnv = u8"[■]--- HW ENV ---";
		static inline const juce::String invisibleHwEnv = u8"[□]--- HW ENV ---";
		static inline const juce::String visibleSwEnv = u8"[■]--- SW ENV ---";
		static inline const juce::String invisibleSwEnv = u8"[□]--- SW ENV ---";
		static inline const juce::String mix = u8"--- MIX ---";
		static inline const juce::String ssgTone = u8"--- TONE ---";
		static inline const juce::String ssgNoise = u8"--- NOISE ---";
		static inline const juce::String invert = u8"--- INVERT ---";
		static inline const juce::String peak = u8"--- PEAK ---";
		static inline const juce::String visibleMonoMode = u8"[■]--- MONO/POLY ---";
		static inline const juce::String invisibleMonoMode = u8"[□]--- MONO/POLY ---";
		static inline const juce::String visibleDetune = u8"[■]--- DETUNE ---";
		static inline const juce::String invisibleDetune = u8"[□]--- DETUNE ---";
		static inline const juce::String visibleUnison = u8"[■]--- UNISON/HARMONY ---";
		static inline const juce::String invisibleUnison = u8"[□]--- UNISON/HARMONY ---";
	}

	namespace Unison
	{
		static inline const juce::String voices = "Voices";
		static inline const juce::String detune = "Detune";
		static inline const juce::String spread = "Spread";
	}

	namespace File
	{
		static inline const juce::String load = u8"Load";
		static inline const juce::String Pcm = u8"PCM";
		static inline const juce::String clear = u8"X";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = u8"Main";
		static inline const juce::String voice = u8"Voice";
		static inline const juce::String ssgDuty = u8"Pulse Width (Duty)";
		static inline const juce::String ssgTri = u8"Triangle Property";
		static inline const juce::String ssgHwEnv = u8"Hardware Envelope";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8" Ar";
		static inline const juce::String dr = u8" Dr";
		static inline const juce::String sl = u8" Sl";
		static inline const juce::String rr = u8" Rr";
		static inline const juce::String stl = u8"Stl";
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = u8"Enable";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8" Ar";
		static inline const juce::String dr = u8" Dr";
		static inline const juce::String rr = u8" Rr";
		static inline const juce::String stl = u8" Stl";
		static inline const juce::String atl = u8" Atl";
		static inline const juce::String ssl = u8" Ssl";
		static inline const juce::String rll = u8" Rll";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String steps = u8" Steps";
		static inline const juce::String loop = u8" Loop";
		static inline const juce::String loopTo = u8" Loop To";
		static inline const juce::String loopCount = u8" Loop Cnt";
		static inline const juce::String stl = u8" Stl";
		static inline const juce::String r1 = u8" R1";
		static inline const juce::String l1 = u8" L1";
		static inline const juce::String r2 = u8" R2";
		static inline const juce::String l2 = u8" L2";
		static inline const juce::String r3 = u8" R3";
		static inline const juce::String l3 = u8" L3";
		static inline const juce::String r4 = u8" R4";
		static inline const juce::String l4 = u8" L4";
		static inline const juce::String r5 = u8" R5";
		static inline const juce::String l5 = u8" L5";
		static inline const juce::String r6 = u8" R6";
		static inline const juce::String l6 = u8" L6";
	}

	namespace Lfo
	{
		static inline const juce::String pmSpeed = u8"P.Speed";
		static inline const juce::String amSpeed = u8"A.Speed";
		static inline const juce::String pmShape = u8"P.Shape";
		static inline const juce::String amShape = u8"A.Shape";
		static inline const juce::String syncDelay = u8"Delay";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"Ams";
		static inline const juce::String pms = u8"Pms";
		static inline const juce::String amd = u8"Amd";
		static inline const juce::String pmd = u8"Pmd";
		static inline const juce::String amSmoothRatio = u8"As Ratio";
	}

	namespace Ssg
	{
		namespace Voice
		{
			static inline const juce::String form = u8"Form";
			static inline const juce::String tone = u8"Tone";
			static inline const juce::String noise = u8"Noise";
			static inline const juce::String noiseFreq = u8"Freq";
			static inline const juce::String noiseOnNote = u8"Noise On Note";
			static inline const juce::String mix = u8"Mix";
		}

		namespace Duty
		{
			static inline const juce::String mode = u8"Mode";
			static inline const juce::String preset = u8"Preset";
			static inline const juce::String var = u8"Ratio";
			static inline const juce::String invert = u8"Invert Phrase";
		}

		namespace Tri
		{
			static inline const juce::String keyTrack = u8"Key Track (Pitch)";
			static inline const juce::String peak = u8"Peak";
			static inline const juce::String manualFreq = u8"Freq";
			static inline const juce::String peakTo00 = u8"0.0 (Down)";
			static inline const juce::String peakTo05 = u8"0.5 (Tri)";
			static inline const juce::String peakTo10 = u8"1.0 (Up)";

		}

		namespace HwEnv
		{
			static inline const juce::String enable = u8"Enable";
			static inline const juce::String shape = u8"Shape";
			static inline const juce::String speed = u8"Period";
		}

		namespace Detune
		{
			static inline const juce::String Mul = u8"Mul";
			static inline const juce::String MulRatio = u8"M.Ratio";
			static inline const juce::String Dt1 = u8"Dt1";
			static inline const juce::String Dt2 = u8"Dt2";
		}
	}
}
