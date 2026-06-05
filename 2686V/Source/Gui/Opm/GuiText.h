// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace OpmGuiText
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
		static inline const juce::String algFb = u8"--- ALGORITHM/FEEDBACK ---";
		static inline const juce::String visibleHwLfo = u8"[■]--- HW LFO ---";
		static inline const juce::String invisibileHwLfo = u8"[□]--- HW LFO ---";
		static inline const juce::String visibleFix = u8"[■]--- FIX ---";
		static inline const juce::String invisibleFix = u8"[□]--- FIX ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String visibleAdsr = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- AMP ENV ---";
		static inline const juce::String visiblePitchAdsr = u8"[■]--- PITCH ENV ---";
		static inline const juce::String invisiblePitchAdsr = u8"[□]--- PITCH ENV ---";
		static inline const juce::String visibleSwEnv = u8"[■]--- SSG SW ENV ---";
		static inline const juce::String invisibleSwEnv = u8"[□]--- SSG SW ENV ---";
		static inline const juce::String visibleMask = u8"[■]--- MASK ---";
		static inline const juce::String invisibleMask = u8"[□]--- MASK ---";
		static inline const juce::String mml = u8"--- MML ---";
		static inline const juce::String visiblePan = u8"[■]--- PAN ---";
		static inline const juce::String invisiblePan = u8"[□]--- PAN ---";
		static inline const juce::String visibleMonoMode = u8"[■]--- MONO/POLY ---";
		static inline const juce::String invisibleMonoMode = u8"[□]--- MONO/POLY ---";
		static inline const juce::String visibleOptional = u8"[■]--- OPTIONAL ---";
		static inline const juce::String invisibleOptional = u8"[□]--- OPTIONAL ---";
		static inline const juce::String visibleUnison = u8"[■]--- UNISON/HARMONY ---";
		static inline const juce::String invisibleUnison = u8"[□]--- UNISON/HARMONY ---";
	}

	namespace Unison
	{
		static inline const juce::String voices = "Voices";
		static inline const juce::String detune = "Detune";
		static inline const juce::String spread = "Spread";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = u8"Main";
		static inline const juce::String opPrefix = u8"Op";
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = u8"Enable";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8"Ar";
		static inline const juce::String dr = u8"Dr";
		static inline const juce::String rr = u8"Rr";
		static inline const juce::String stl = u8"Stl";
		static inline const juce::String atl = u8"Atl";
		static inline const juce::String ssl = u8"Ssl";
		static inline const juce::String rll = u8"Rll";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String enable = u8"Enable";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String steps = u8"Steps";
		static inline const juce::String loop = u8"Loop";
		static inline const juce::String loopTo = u8"Loop To";
		static inline const juce::String loopCount = u8"Loop Cnt";
		static inline const juce::String stl = u8"Stl";
		static inline const juce::String r1 = u8"R1";
		static inline const juce::String l1 = u8"L1";
		static inline const juce::String r2 = u8"R2";
		static inline const juce::String l2 = u8"L2";
		static inline const juce::String r3 = u8"R3";
		static inline const juce::String l3 = u8"L3";
		static inline const juce::String r4 = u8"R4";
		static inline const juce::String l4 = u8"L4";
		static inline const juce::String r5 = u8"R5";
		static inline const juce::String l5 = u8"L5";
		static inline const juce::String r6 = u8"R6";
		static inline const juce::String l6 = u8"L6";
	}

	namespace Fm
	{
		static inline const juce::String alg = u8"Alg";
		static inline const juce::String fb = u8"Fb";
		static inline const juce::String lfoFreq = u8"Freq";
		static inline const juce::String lfoSpeed = u8"Speed";
		static inline const juce::String lfoShape = u8"Shape";
		static inline const juce::String lfoPgShape = u8"P.Shape";
		static inline const juce::String lfoEgShape = u8"A.Shape";
		static inline const juce::String lfoSyncDelay = u8"Delay";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"Ams";
		static inline const juce::String pms = u8"Pms";
		static inline const juce::String amd = u8"Amd";
		static inline const juce::String pmd = u8"Pmd";
		static inline const juce::String amSmoothRatio = u8"As Ratio";
		static inline const juce::String initLfoToOpl = u8"Init Lfo: OPL";
		static inline const juce::String initLfoToOpll = u8"Init Lfo: OPLL";
		static inline const juce::String pan = u8"Pan";
		static inline const juce::String panpot = u8"Panpot";
		static inline const juce::String panpotEnable = u8"Enable";

		namespace Pan
		{
			static inline const juce::String l = u8"L";
			static inline const juce::String c = u8"C";
			static inline const juce::String r = u8"R";
		}

		namespace Op
		{
			static inline const juce::String key = u8"Op";
			static inline const juce::String Ar = u8"Ar";
			static inline const juce::String Dr = u8"Dr";
			static inline const juce::String Sl = u8"Sl";
			static inline const juce::String Rr = u8"Rr";
			static inline const juce::String Sr = u8"Sr";
			static inline const juce::String D1r = u8"D1r";
			static inline const juce::String D1l = u8"D1l";
			static inline const juce::String D2r = u8"D2r";
			static inline const juce::String Mul = u8"Mul";
			static inline const juce::String Tl = u8"Tl";
			static inline const juce::String Dt = u8"Dt";
			static inline const juce::String Dt1 = u8"Dt1";
			static inline const juce::String Dt2 = u8"Dt2";
			static inline const juce::String Ks = u8"Ks";
			static inline const juce::String Ksr = u8"Ksr";
			static inline const juce::String Ksl = u8"Ksl";
			static inline const juce::String Mask = u8"Mask";
			static inline const juce::String Fix = u8"Enable";
			static inline const juce::String FFreq = u8"Freq";
			static inline const juce::String SEnv = u8"Env";
			static inline const juce::String SFreq = u8"Freq";
			static inline const juce::String Freqs = u8"Freq";
			static inline const juce::String SyncDelay = u8"Delay";
			static inline const juce::String Am = u8"Am";
			static inline const juce::String Pm = u8"Pm";
			static inline const juce::String AmEn = u8"AM Enable";
			static inline const juce::String PmEn = u8"PM Enable";
			static inline const juce::String AmsEn = u8"AMS Enable";
			static inline const juce::String PgShape = u8"P.Shape";
			static inline const juce::String EgShape = u8"A.Shape";
			static inline const juce::String Vib = u8"Vib";
			static inline const juce::String Ams = u8"Ams";
			static inline const juce::String Pms = u8"Pms";
			static inline const juce::String Amd = u8"Amd";
			static inline const juce::String Pmd = u8"Pmd";
			static inline const juce::String EgType = u8"EgType";
			static inline const juce::String Eg = u8"Eg";
			static inline const juce::String Ws = u8"Ws";
			static inline const juce::String FreqTo0 = u8"Freq -> 0Hz";
			static inline const juce::String FreqTo440 = u8"Freq -> 440Hz";
			static inline const juce::String Opzx7FreqTo0 = u8"->0";
			static inline const juce::String Opzx7FreqTo05 = u8"->0.5";
			static inline const juce::String Opzx7FreqTo1 = u8"->1";
			static inline const juce::String Opzx7FreqTo2 = u8"->2";
			static inline const juce::String Opzx7FreqTo440 = u8"->440";
			static inline const juce::String ArTo000 = u8"-> 0.00";
			static inline const juce::String ArTo003 = u8"-> 0.03";
			static inline const juce::String RrTo000 = u8"-> 0.00";
			static inline const juce::String RrTo003 = u8"-> 0.03";
			static inline const juce::String PcmOffset = u8"Offset";
			static inline const juce::String PcmRatio = u8"Ratio";
			static inline const juce::String RgEn = u8"Register Mode";

			static inline const juce::String sus = u8"Sus";
			static inline const juce::String xof = u8"Xof";
			static inline const juce::String kor = u8"Kor";
			static inline const juce::String bypass = u8"Bypass";

			static inline const juce::String phaseOffset = u8"P.Offset";
		}
	}
}
