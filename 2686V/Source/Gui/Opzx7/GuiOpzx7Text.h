// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace Opzx7GuiText
{
	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String algFb = u8"--- ALGORITHM/FEEDBACK ---";
		static inline const juce::String waveShape = u8"--- WAVE SHAPE ---";
		static inline const juce::String visibleLfo = u8"[■]--- LFO ---";
		static inline const juce::String invisibleLfo = u8"[□]--- LFO ---";
		static inline const juce::String visibleAdsr = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- AMP ENV ---";
		static inline const juce::String visibleSsgEnv = u8"[■]--- SSG HW ENV ---";
		static inline const juce::String invisibleSsgEnv = u8"[□]--- SSG HW ENV ---";
		static inline const juce::String visibleMask = u8"[■]--- MASK ---";
		static inline const juce::String invisibleMask = u8"[□]--- MASK ---";
		static inline const juce::String visiblePanpot = u8"[■]--- PANPOT ---";
		static inline const juce::String invisiblePanpot = u8"[□]--- PANPOT ---";
		static inline const juce::String visibleOptional = u8"[■]--- OPTIONAL ---";
		static inline const juce::String invisibleOptional = u8"[□]--- OPTIONAL ---";
	}

	namespace File
	{
		static inline const juce::String load = u8"Load";
		static inline const juce::String Pcm = u8"PCM";
		static inline const juce::String clear = u8"X";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "OPZX7チャンネル設定";
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
			static inline const juce::String key = juce::String("") + "オペレーター";
			static inline const juce::String Ar = u8"Ar";
			static inline const juce::String Rr = u8"Rr";
			static inline const juce::String D1r = u8"Dr";
			static inline const juce::String D1l = u8"Sl";
			static inline const juce::String D2r = u8"Sr";
			static inline const juce::String Mul = u8"Mul";
			static inline const juce::String MulRatio = u8"M.Ratio";
			static inline const juce::String Tl = u8"Tl";
			static inline const juce::String Dt = u8"Dt";
			static inline const juce::String Dt1 = u8"Dt1";
			static inline const juce::String Dt2 = u8"Dt2";
			static inline const juce::String Dt3 = u8"Dt3";
			static inline const juce::String Ksr = u8"Ksr";
			static inline const juce::String Ksl = u8"Ksl";
			static inline const juce::String KsEn = u8"KsEn";
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
			static inline const juce::String Ws = u8"W.Shape";
			static inline const juce::String Opzx7FreqTo440 = u8"->440";
			static inline const juce::String PcmOffset = u8"Offset";
			static inline const juce::String PcmRatio = u8"Ratio";
			static inline const juce::String RgEn = u8"Register Mode";

			static inline const juce::String sus = u8"Sus";
			static inline const juce::String xof = u8"Xof";
			static inline const juce::String kor = u8"Kor";
			static inline const juce::String bypass = u8"Bypass";
		}
	}
}
