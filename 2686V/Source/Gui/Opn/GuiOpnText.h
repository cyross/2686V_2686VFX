// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace OpnGuiText
{
	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String algFb = u8"--- ALGORITHM/FEEDBACK ---";
		static inline const juce::String visibleDetune = u8"[■]--- DETUNE ---";
		static inline const juce::String invisibleDetune = u8"[□]--- DETUNE ---";
		static inline const juce::String visibleAmpEnv = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAmpEnv = u8"[□]--- AMP ENV ---";
		static inline const juce::String visibleN88Lfo = u8"[■]--- N88 LFO ---";
		static inline const juce::String invisibleN88Lfo = u8"[□]--- N88 LFO ---";
		static inline const juce::String visibleKs = u8"[■]--- KEY SCALE ---";
		static inline const juce::String invisibleKs = u8"[□]--- KEY SCALE ---";
		static inline const juce::String visibleMask = u8"[■]--- MASK ---";
		static inline const juce::String invisibleMask = u8"[□]--- MASK ---";
		static inline const juce::String visibleOptional = u8"[■]--- OPTIONAL ---";
		static inline const juce::String invisibleOptional = u8"[□]--- OPTIONAL ---";
		static inline const juce::String visibleUtil = u8"[■]--- UTILITY ---";
		static inline const juce::String invisibleUtil = u8"[□]--- UTILITY ---";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "OPNチャンネル設定";
		static inline const juce::String opPrefix = juce::String("") + "オペレーター";
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = u8"Enable";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String enable = u8"Enable";
	}

	namespace Fm
	{
		static inline const juce::String level = u8"LV";
		static inline const juce::String alg = u8"Alg";
		static inline const juce::String fb = u8"Fb";
		static inline const juce::String lfoFreq = u8"FQ";
		static inline const juce::String lfoSpeed = u8"SP";
		static inline const juce::String lfoShape = u8"SH";
		static inline const juce::String lfoPgShape = u8"P.SH";
		static inline const juce::String lfoEgShape = u8"A.SH";
		static inline const juce::String lfoSyncDelay = u8"Delay";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"Ams";
		static inline const juce::String pms = u8"Pms";
		static inline const juce::String amd = u8"Amd";
		static inline const juce::String pmd = u8"Pmd";
		static inline const juce::String amSmoothRatio = u8"AM SR";

		namespace Pan
		{
			static inline const juce::String l = u8"L";
			static inline const juce::String c = u8"C";
			static inline const juce::String r = u8"R";
		}

		namespace Op
		{
			static inline const juce::String key = juce::String("") + "オペレーター";
			static inline const juce::String Ar = u8"Ar";
			static inline const juce::String Dr = u8"Dr";
			static inline const juce::String Sl = u8"Sl";
			static inline const juce::String Rr = u8"Rr";
			static inline const juce::String Sr = u8"Sr";
			static inline const juce::String Mul = u8"Mul";
			static inline const juce::String Tl = u8"Tl";
			static inline const juce::String Dt = u8"Dt";
			static inline const juce::String Dt1 = u8"Dt1";
			static inline const juce::String Ks = u8"Ks";
			static inline const juce::String Mask = u8"Mask";
			static inline const juce::String Fix = u8"Enable";
			static inline const juce::String FFreq = u8"FQ";
			static inline const juce::String SEnv = u8"Env";
			static inline const juce::String SFreq = u8"FQ";
			static inline const juce::String Freqs = u8"FQ";
			static inline const juce::String SyncDelay = u8"Delay";
			static inline const juce::String Am = u8"Am";
			static inline const juce::String Pm = u8"Pm";
			static inline const juce::String AmEn = u8"AM Enable";
			static inline const juce::String PmEn = u8"PM Enable";
			static inline const juce::String AmsEn = u8"AMS Enable";
			static inline const juce::String PgShape = u8"P.SH";
			static inline const juce::String EgShape = u8"A.SH";
			static inline const juce::String Vib = u8"Vib";
			static inline const juce::String Ams = u8"Ams";
			static inline const juce::String Pms = u8"Pms";
			static inline const juce::String Amd = u8"Amd";
			static inline const juce::String Pmd = u8"Pmd";
			static inline const juce::String EgType = u8"EgType";
			static inline const juce::String Eg = u8"Eg";
			static inline const juce::String Ws = u8"Ws";
			static inline const juce::String Opzx7FreqTo440 = u8"->440";

			static inline const juce::String sus = u8"Sus";
			static inline const juce::String xof = u8"Xof";
			static inline const juce::String kor = u8"Kor";
			static inline const juce::String bypass = u8"Bypass";
		}
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Level -> All Ch";
		static inline const juce::String lfoFileImport = u8"Import LFO Param (.lfoN88)";
		static inline const juce::String lfoFileExport = u8"Export LFO Param (.lfoN88)";
		static inline const juce::String pitchEnvFileImport = u8"Import Pitch Env Param (.pitchEnv)";
		static inline const juce::String pitchEnvFileExport = u8"Export Pitch Env Param (.pitchEnv)";
		static inline const juce::String ssgSwEnvFileImport = u8"Import SSG SW Env Param (.ssgSwEnv)";
		static inline const juce::String ssgSwEnvFileExport = u8"Export SSG SW Env Param (.ssgSwEnv)";
		static inline const juce::String unisonFileImport = u8"Import Unison Param (.unison)";
		static inline const juce::String unisonFileExport = u8"Export Unison Param (.unison)";
	}
}
