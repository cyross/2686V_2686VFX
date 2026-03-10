// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace GuiText
{
	static inline const juce::String mode = "Mode";

	static inline const juce::String bit = "BIT";
	static inline const juce::String rate = "RATE";

	static inline const juce::String monoPoly = "MonoPhonic";

	namespace MasterVol
	{
		static inline const juce::String title = "M.Vol";
		static inline const juce::String unit = " dB";
	}

	namespace Tab
	{
		static inline const juce::String opna = "OPNA";
		static inline const juce::String opn = "OPN";
		static inline const juce::String opl = "OPL";
		static inline const juce::String opl3 = "OPL3";
		static inline const juce::String opm = "OPM";
		static inline const juce::String opzx3 = "OPZX3";
		static inline const juce::String ssg = "SSG";
		static inline const juce::String wt = "WT";
		static inline const juce::String rhythm = "RHYTHM";
		static inline const juce::String adpcm = "ADPCM";
		static inline const juce::String beep = "BEEP";
		static inline const juce::String fx = "FX";
		static inline const juce::String preset = "PRESET";
		static inline const juce::String settings = "SETTINGS";
		static inline const juce::String about = "ABOUT";
	}

	namespace Category
	{
		static inline const juce::String quality = "--- QUALITY ---";
		static inline const juce::String algFb = "--- ALGORITHM/FEEDBACK ---";
		static inline const juce::String m = "--- PARAMETERS ---";
		static inline const juce::String lfo = "--- LFO ---";
		static inline const juce::String adsr = "--- ADSR ---";
		static inline const juce::String fix = "--- FIX ---";
		static inline const juce::String mvol = "--- MASTER VOLUME ---";
		static inline const juce::String shape = "--- SHAPE ---";
		static inline const juce::String mask = "--- MASK ---";
		static inline const juce::String mml = "--- MML ---";
		static inline const juce::String bypass = "--- BYPASS ---";
		static inline const juce::String pan = "--- PAN ---";
		static inline const juce::String mix = "--- MIX ---";
		static inline const juce::String mod = "--- MODULATION ---";
		static inline const juce::String ssgTone = "--- TOME ---";
		static inline const juce::String ssgNoise = "--- NOISE ---";
		static inline const juce::String invert = "--- INVERT ---";
		static inline const juce::String peak = "--- PEAK ---";
		static inline const juce::String initialize = "--- INITIALIZE ---";
		static inline const juce::String monoMode = "--- MONO/POLY ---";
		static inline const juce::String preset = "--- PRESET ---";
	}

	namespace File
	{
		static inline const juce::String load = "Load";
		static inline const juce::String Pcm = "PCM";
		static inline const juce::String clear = "X";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = "Main";
		static inline const juce::String opPrefix = "OP";
		static inline const juce::String voice = "Voice";
		static inline const juce::String ssgDuty = "Pulse Width (Duty)";
		static inline const juce::String ssgTri = "Triangle Property";
		static inline const juce::String ssgHwEnv = "Hardware Envelope";
		static inline const juce::String wtCustom = "Custom Wave";
		static inline const juce::String adpcmParams = "ADPCM Parameters";
		static inline const juce::String fxRLfo = "Software LFO";
		static inline const juce::String fxTremolo = "Tremolo";
		static inline const juce::String fxVibrato = "Vibrato";
		static inline const juce::String fxMbc = "Modern Bit Crusher";
		static inline const juce::String fxDelay = "Delay";
		static inline const juce::String fxReverb = "Reverb";
		static inline const juce::String fxRbc = "Retro Bit Crusher";
		static inline const juce::String fxFilter = "Filter";
		static inline const juce::String fxSoftClipper = "Soft Clipper";
		static inline const juce::String settingEnv = "Environment Settings";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = "Bypass ADSR";
		static inline const juce::String ar = " AR";
		static inline const juce::String dr = " DR";
		static inline const juce::String sl = " SL";
		static inline const juce::String rr = " RR";
	}

	namespace Fm
	{
		static inline const juce::String alg = "ALG";
		static inline const juce::String fb0 = "FB1";
		static inline const juce::String fb01 = "FB1/2";
		static inline const juce::String fb2 = "FB3";
		static inline const juce::String lfoFreq = "L.FREQ";
		static inline const juce::String lfoShape = "L.SHAPE";
		static inline const juce::String amEn = "L.AM-EN";
		static inline const juce::String pmEn = "L.PM-EN";
		static inline const juce::String ams = "L.AMS";
		static inline const juce::String pms = "L.PMS";
		static inline const juce::String amd = "L.AMD";
		static inline const juce::String pmd = "L.PMD";
		static inline const juce::String amSmoothRatio = "L.ASMRT";
		static inline const juce::String initLfoToOpl = "Init Lfo: OPL";
		static inline const juce::String initLfoToOpll = "Init Lfo: OPLL";

		namespace Op
		{
			static inline const juce::String key = " OP";

			static inline const juce::String Ar = "AR";
			static inline const juce::String Dr = "DR";
			static inline const juce::String Sl = "SL";
			static inline const juce::String Rr = "RR";
			static inline const juce::String Sr = "SR";
			static inline const juce::String D1r = "D1R";
			static inline const juce::String D1l = "D1L";
			static inline const juce::String D2r = "D2R";
			static inline const juce::String Mul = "MUL";
			static inline const juce::String Tl = "TL";
			static inline const juce::String Dt = "DT";
			static inline const juce::String Dt1 = "DT1";
			static inline const juce::String Dt2 = "DT2";
			static inline const juce::String Ks = "KS";
			static inline const juce::String Ksr = "KSR";
			static inline const juce::String Ksl = "KSL";
			static inline const juce::String Mask = "MASK";
			static inline const juce::String Fix = "FIX";
			static inline const juce::String FFreq = "FFREQ";
			static inline const juce::String SEnv = "SENV";
			static inline const juce::String SFreq = "SFREQ";
			static inline const juce::String Am = "AM";
			static inline const juce::String Pm = "PM";
			static inline const juce::String AmEn = "AM-EN";
			static inline const juce::String PmEn = "PM-EN";
			static inline const juce::String Vib = "VIB";
			static inline const juce::String Ams = "AMS";
			static inline const juce::String Pms = "PMS";
			static inline const juce::String Amd = "AMD";
			static inline const juce::String Pmd = "PMD";
			static inline const juce::String EgType = "EGTYPE";
			static inline const juce::String Eg = "EG";
			static inline const juce::String Ws = "WS";
			static inline const juce::String FreqTo0 = "Freq -> 0Hz";
			static inline const juce::String FreqTo440 = "Freq -> 440Hz";
			static inline const juce::String Opzx3FreqTo0 = "->0";
			static inline const juce::String Opzx3FreqTo05 = "->0.5";
			static inline const juce::String Opzx3FreqTo1 = "->1";
			static inline const juce::String Opzx3FreqTo2 = "->2";
			static inline const juce::String Opzx3FreqTo440 = "->440";
			static inline const juce::String PcmOffset = "OFFSET";
			static inline const juce::String PcmRatio = "RATIO";
			static inline const juce::String RgEn = "RG-EN";

			static inline const juce::String sus = "SUS";
		}
	}

	namespace Ssg
	{
		namespace Voice
		{
			static inline const juce::String form = "Form";
			static inline const juce::String tone = "Tone";
			static inline const juce::String noise = "Noise";
			static inline const juce::String noiseFreq = "Freq";
			static inline const juce::String noiseOnNote = "Noise On Note";
			static inline const juce::String mix = "Mix";
		}

		namespace Duty
		{
			static inline const juce::String mode = "Mode";
			static inline const juce::String preset = "Preset";
			static inline const juce::String var = "Ratio";
			static inline const juce::String invert = "Invert Phrase";
		}

		namespace Tri
		{
			static inline const juce::String keyTrack = "Key Track (Pitch)";
			static inline const juce::String peak = "Peak";
			static inline const juce::String manualFreq = "Freq";
			static inline const juce::String peakTo00 = "0.0 (Down)";
			static inline const juce::String peakTo05 = "0.5 (Tri)";
			static inline const juce::String peakTo10 = "1.0 (Up)";

		}

		namespace HwEnv
		{
			static inline const juce::String enable = "Enable HW Env";
			static inline const juce::String shape = "Shape";
			static inline const juce::String speed = "Period";
		}
	}

	namespace Wt
	{
		static inline const juce::String level = "Level";
		static inline const juce::String form = "Form";
		static inline const juce::String size = "Size";
		static inline const juce::String sampleSize = "Sample Size";
		static inline const juce::String waveform = "Waveform";

		namespace Mod
		{
			static inline const juce::String enable = "Mod";
			static inline const juce::String depth = "Depth";
			static inline const juce::String speed = "Speed";
		}

		namespace Custom
		{
			static inline const juce::String to0 = "-> 0.0";
			static inline const juce::String to1 = "-> 1.0";
			static inline const juce::String toM1 = "-> -1.0";
		}
	}

	namespace Rhythm
	{
		static inline const juce::String vol = "Level";

		namespace Pad
		{
			static inline const juce::String key = "Pad";

			static inline const juce::String vol = "Level";
			static inline const juce::String note = "Note";
			static inline const juce::String quality = "Bit";
			static inline const juce::String rate = "Rate";
			static inline const juce::String oneShot = "One Shot";
			static inline const juce::String pan = "Pan";
			static inline const juce::String rr = "RR";
			static inline const juce::String pcmOffset = "OFFSET";
			static inline const juce::String pcmRatio = "RATIO";

			namespace Pan
			{
				static inline const juce::String l = "L";
				static inline const juce::String c = "C";
				static inline const juce::String r = "R";
			}
		}
	}

	namespace Adpcm
	{
		static inline const juce::String level = "Level";
		static inline const juce::String pan = "Pan";
		static inline const juce::String quality = "Bit";
		static inline const juce::String rate = "Rate";
		static inline const juce::String loop = "Loop";
		static inline const juce::String pcmOffset = "OFFSET";
		static inline const juce::String pcmRatio = "RATIO";

		namespace Pan
		{
			static inline const juce::String l = "L";
			static inline const juce::String c = "C";
			static inline const juce::String r = "R";
		}

		namespace Adsr
		{
			static inline const juce::String ar = "AR";
			static inline const juce::String dr = "DR";
			static inline const juce::String sl = "SL";
			static inline const juce::String rr = "RR";
		}
	}

	namespace Beep
	{
		static inline const juce::String Level = "Level";
		static inline const juce::String Fix = "Fix";
		static inline const juce::String FFreq = "FFreq";
		static inline const juce::String To2k = "-> 2k";

		namespace Adsr
		{
			static inline const juce::String Bypass = "Bypass ADSR";
			static inline const juce::String Ar = "AR";
			static inline const juce::String Dr = "DR";
			static inline const juce::String Sl = "SL";
			static inline const juce::String Rr = "RR";
		}
	}

	namespace Fx
	{
		static inline const juce::String masterBypass = "M.Bypass";
		static inline const juce::String bypass = "Bypass";
		static inline const juce::String mix = "Mix";
		
		namespace Mix
		{
			static inline const juce::String dry = "Dry";
			static inline const juce::String mix = "50%";
			static inline const juce::String wet = "Wet";
		}

		namespace Tremolo
		{
			static inline const juce::String rate = "Rate";
			static inline const juce::String depth = "Depth";
		}

		namespace Vibrate
		{
			static inline const juce::String rate = "Rate";
			static inline const juce::String depth = "Depth";
		}

		namespace Mbc
		{
			static inline const juce::String bit = "Bit";
			static inline const juce::String rate = "Rate";
		}

		namespace Delay
		{
			static inline const juce::String time = "Time";
			static inline const juce::String fb = "Fb";
		}

		namespace Reverb
		{
			static inline const juce::String size = "Size";
			static inline const juce::String damp = "Damp";
		}

		namespace Rbc
		{
			static inline const juce::String quality = "Bit";
			static inline const juce::String rate = "Rate";
		}

		namespace Filter
		{
			static inline const juce::String type = "Type";
			static inline const juce::String freq = "Freq";
			static inline const juce::String q = " Q";
		}

		namespace Rlfo
		{
			static inline const juce::String wave = "Wave";
			static inline const juce::String freq = "Freq";
			static inline const juce::String pms = "Pms";
			static inline const juce::String pmd = "Pmd";
			static inline const juce::String ams = "Ams";
			static inline const juce::String amd = "Amd";
		}
	}

	namespace Preview
	{
		static inline const juce::String show = ">>";
		static inline const juce::String hide = "<<";
	}
}
