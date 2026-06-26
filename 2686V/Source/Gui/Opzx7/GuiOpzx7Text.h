// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace Opzx7GuiText
{
	static inline const juce::String bit = u8"BIT";
	static inline const juce::String rate = u8"RATE";

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String algFb = u8"--- ALGORITHM/FEEDBACK ---";
		static inline const juce::String waveShape = u8"--- WAVE SHAPE ---";
		static inline const juce::String visibleWaveShape = u8"[■]--- WAVE SHAPE ---";
		static inline const juce::String invisibleWaveShape = u8"[□]--- WAVE SHAPE ---";
		static inline const juce::String visibleAmpEnv = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAmpEnv = u8"[□]--- AMP ENV ---";
		static inline const juce::String visibleLfo = u8"[■]--- LFO ---";
		static inline const juce::String invisibleLfo = u8"[□]--- LFO ---";
		static inline const juce::String visibleSsgEnv = u8"[■]--- SSG HW ENV ---";
		static inline const juce::String invisibleSsgEnv = u8"[□]--- SSG HW ENV ---";
		static inline const juce::String visibleKs = u8"[■]--- KEY SCALE ---";
		static inline const juce::String invisibleKs = u8"[□]--- KEY SCALE ---";
		static inline const juce::String visibleMask = u8"[■]--- MASK ---";
		static inline const juce::String invisibleMask = u8"[□]--- MASK ---";
		static inline const juce::String visiblePanpot = u8"[■]--- PANPOT ---";
		static inline const juce::String invisiblePanpot = u8"[□]--- PANPOT ---";
		static inline const juce::String visibleOptional = u8"[■]--- OPTIONAL ---";
		static inline const juce::String invisibleOptional = u8"[□]--- OPTIONAL ---";
		static inline const juce::String visibleUtil = u8"[■]--- UTILITY ---";
		static inline const juce::String invisibleUtil = u8"[□]--- UTILITY ---";
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
		static inline const juce::String level = u8"Level";
		static inline const juce::String alg = u8"ALG";
		static inline const juce::String fb = u8"FB";
		static inline const juce::String lfoFreq = u8"FQ";
		static inline const juce::String lfoSpeed = u8"SP";
		static inline const juce::String lfoShape = u8"SP";
		static inline const juce::String lfoPgShape = u8"P.SH";
		static inline const juce::String lfoEgShape = u8"A.SH";
		static inline const juce::String lfoSyncDelay = u8"SDLY";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"AMS";
		static inline const juce::String pms = u8"PMS";
		static inline const juce::String amd = u8"AMD";
		static inline const juce::String pmd = u8"PMD";
		static inline const juce::String amSmoothRatio = u8"AMSR";
		static inline const juce::String panpot = u8"PNPT";
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
			static inline const juce::String Ar = u8"AR";
			static inline const juce::String Rr = u8"RR";
			static inline const juce::String D1r = u8"DR";
			static inline const juce::String D1l = u8"SL";
			static inline const juce::String D2r = u8"SR";
			static inline const juce::String Mul = u8"MUL";
			static inline const juce::String MulRatio = u8"MURT";
			static inline const juce::String Tl = u8"TL";
			static inline const juce::String Dt = u8"DT";
			static inline const juce::String Dt1 = u8"DT1";
			static inline const juce::String Dt2 = u8"DT2";
			static inline const juce::String Dt3 = u8"DT3";
			static inline const juce::String KsEn = u8"Enable";
			static inline const juce::String KsMode = u8"MODE";
			static inline const juce::String Ksr = u8"KSR";
			static inline const juce::String Ksl = u8"KSL";
			static inline const juce::String KsBp = u8"KSBP";
			static inline const juce::String KsLc = u8"KSLC";
			static inline const juce::String KsRc = u8"KSRC";
			static inline const juce::String KsLd = u8"KSLD";
			static inline const juce::String KsRd = u8"KSRD";
			static inline const juce::String KsRs = u8"KSRS";
			static inline const juce::String Mask = u8"MASK";
			static inline const juce::String Fix = u8"Enable";
			static inline const juce::String FFreq = u8"FQ";
			static inline const juce::String SEnv = u8"ENV";
			static inline const juce::String SFreq = u8"FQ";
			static inline const juce::String Freqs = u8"FQ";
			static inline const juce::String SyncDelay = u8"SDLY";
			static inline const juce::String Am = u8"AM";
			static inline const juce::String Pm = u8"PM";
			static inline const juce::String AmEn = u8"AM Enable";
			static inline const juce::String PmEn = u8"PM Enable";
			static inline const juce::String AmsEn = u8"AMS Enable";
			static inline const juce::String PgShape = u8"P.SH";
			static inline const juce::String EgShape = u8"A.SH";
			static inline const juce::String Vib = u8"VIB";
			static inline const juce::String Ams = u8"AMS";
			static inline const juce::String Pms = u8"PMS";
			static inline const juce::String Amd = u8"AMD";
			static inline const juce::String Pmd = u8"PMD";
			static inline const juce::String EgType = u8"EGTP";
			static inline const juce::String Eg = u8"EG";
			static inline const juce::String Ws = u8"WS";
			static inline const juce::String Opzx7FreqTo440 = u8"->440";
			static inline const juce::String PcmOffset = u8"POFF";
			static inline const juce::String PcmRatio = u8"PRT";
			static inline const juce::String loopPointEnable = u8"Loop Point Enable";
			static inline const juce::String loopPointStart = u8"LPST";
			static inline const juce::String loopPointEnd = u8"LPED";
			static inline const juce::String RgEn = u8"Register Mode";

			static inline const juce::String sus = u8"SUS";
			static inline const juce::String xof = u8"XOF";
			static inline const juce::String kor = u8"KOR";
			static inline const juce::String bypass = u8"Bypass";
		}
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Level -> All Ch";
		static inline const juce::String opLfoFileImport = u8"[IM]OP LFO";
		static inline const juce::String opLfoFileExport = u8"[EX]OP LFO";
		static inline const juce::String lfoFileImport = u8"[IM]LFO";
		static inline const juce::String lfoFileExport = u8"[EX]LFO";
		static inline const juce::String ampEnvFileImport = u8"[IM]Amp Env";
		static inline const juce::String ampEnvFileExport = u8"[EX]Amp Env";
		static inline const juce::String pitchEnvFileImport = u8"[IM]Pitch Env";
		static inline const juce::String pitchEnvFileExport = u8"[EX]Pitch Env";
		static inline const juce::String ssgSwEnvFileImport = u8"[IM]SSG SW Env";
		static inline const juce::String ssgSwEnvFileExport = u8"[EX]SSG SW Env";
		static inline const juce::String detuneFileImport = u8"[IM]Detune";
		static inline const juce::String detuneFileExport = u8"[EX]Detune";
		static inline const juce::String unisonFileImport = u8"[IM]Unison";
		static inline const juce::String unisonFileExport = u8"[EX]Unison";
		static inline const juce::String qualityFileImport = u8"[IM]Quality";
		static inline const juce::String qualityFileExport = u8"[EX]Quality";
		static inline const juce::String pcmPlayFileImport = u8"[IM]PCM Play";
		static inline const juce::String pcmPlayFileExport = u8"[EX]PCM Play";
		static inline const juce::String toneNoiseFileImport = u8"[IM]Tone/Noise";
		static inline const juce::String toneNoiseFileExport = u8"[EX]Tone/Noise";
	}
}
