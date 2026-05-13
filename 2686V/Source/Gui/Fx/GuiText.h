// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace GuiText
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

	namespace Tab
	{
		static inline const juce::String opna = u8"OPNA";
		static inline const juce::String opn = u8"OPN";
		static inline const juce::String opl = u8"OPL";
		static inline const juce::String opl3 = u8"OPL3";
		static inline const juce::String opm = u8"OPM";
		static inline const juce::String opzx7 = u8"OPZX7";
		static inline const juce::String ssg = u8"SSG";
		static inline const juce::String wt = u8"WT";
		static inline const juce::String rhythm = u8"RHYTHM";
		static inline const juce::String adpcm = u8"ADPCM";
		static inline const juce::String beep = u8"BEEP";
		static inline const juce::String fx = u8"FX";
		static inline const juce::String preset = u8"PRESET";
		static inline const juce::String settings = u8"SETTINGS";
		static inline const juce::String about = u8"ABOUT";
	}

	namespace Category
	{
		static inline const juce::String quality = u8"--- QUALITY ---";
		static inline const juce::String algFb = u8"--- ALGORITHM/FEEDBACK ---";
		static inline const juce::String m = u8"--- PARAMETERS ---";
		static inline const juce::String lfo = u8"--- LFO ---";
		static inline const juce::String hwLfo = u8"--- HW LFO ---";
		static inline const juce::String swLfo = u8"--- SW LFO ---";
		static inline const juce::String n88Lfo = u8"--- N88 LFO ---";
		static inline const juce::String adsr = u8"--- ADSR ---";
		static inline const juce::String pitchAdsr = u8"--- PITCH ENV ---";
		static inline const juce::String fix = u8"--- FIX ---";
		static inline const juce::String visibleFix = u8"[▲]--- FIX ---";
		static inline const juce::String invisibleFix = u8"[▼]--- FIX ---";
		static inline const juce::String mvol = u8"--- MASTER VOLUME ---";
		static inline const juce::String shape = u8"--- SHAPE ---";
		static inline const juce::String mask = u8"--- MASK ---";
		static inline const juce::String visibleMask = u8"[▲]--- MASK ---";
		static inline const juce::String invisibleMask = u8"[▼]--- MASK ---";
		static inline const juce::String mml = u8"--- MML ---";
		static inline const juce::String bypass = u8"--- BYPASS ---";
		static inline const juce::String pan = u8"--- PAN ---";
		static inline const juce::String panpot = u8"--- PANPOT ---";
		static inline const juce::String mix = u8"--- MIX ---";
		static inline const juce::String mod = u8"--- MODULATION ---";
		static inline const juce::String ssgTone = u8"--- TOME ---";
		static inline const juce::String ssgNoise = u8"--- NOISE ---";
		static inline const juce::String invert = u8"--- INVERT ---";
		static inline const juce::String peak = u8"--- PEAK ---";
		static inline const juce::String initialize = u8"--- INITIALIZE ---";
		static inline const juce::String monoMode = u8"--- MONO/POLY ---";
		static inline const juce::String preset = u8"--- PRESET ---";
		static inline const juce::String waveFile = u8"--- WAVE FILE ---";
		static inline const juce::String detune = u8"--- DETUNE ---";
	}

	namespace File
	{
		static inline const juce::String load = u8"Load";
		static inline const juce::String Pcm = u8"PCM";
		static inline const juce::String clear = u8"X";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = u8"FX";
		static inline const juce::String opPrefix = u8"Op";
		static inline const juce::String voice = u8"Voice";
		static inline const juce::String ssgDuty = u8"Pulse Width (Duty)";
		static inline const juce::String ssgTri = u8"Triangle Property";
		static inline const juce::String ssgHwEnv = u8"Hardware Envelope";
		static inline const juce::String wtCustom = u8"Custom Wave";
		static inline const juce::String adpcmParams = u8"ADPCM Parameters";
		static inline const juce::String fxRLfo = u8"Software LFO";
		static inline const juce::String fxTremolo = u8"Tremolo";
		static inline const juce::String fxVibrato = u8"Vibrato";
		static inline const juce::String fxMbc = u8"Modern Bit Crusher";
		static inline const juce::String fxDelay = u8"Delay";
		static inline const juce::String fxReverb = u8"Reverb";
		static inline const juce::String fxRbc = u8"Retro Bit Crusher";
		static inline const juce::String fxFilter = u8"Filter";
		static inline const juce::String fxSoftClipper = u8"Soft Clipper";
		static inline const juce::String settingEnv = u8"Environment Settings";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8" Ar";
		static inline const juce::String dr = u8" Dr";
		static inline const juce::String sl = u8" Sl";
		static inline const juce::String rr = u8" Rr";
		static inline const juce::String ArTo000 = u8"-> 0.00";
		static inline const juce::String ArTo003 = u8"-> 0.03";
		static inline const juce::String RrTo000 = u8"-> 0.00";
		static inline const juce::String RrTo003 = u8"-> 0.03";
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
		static inline const juce::String ArTo000 = u8"-> 0.00";
		static inline const juce::String ArTo003 = u8"-> 0.03";
		static inline const juce::String RrTo000 = u8"-> 0.00";
		static inline const juce::String RrTo003 = u8"-> 0.03";
	}

	namespace Fm
	{
		static inline const juce::String alg = u8"Alg";
		static inline const juce::String fb0 = u8"Fb1";
		static inline const juce::String fb01 = u8"Fb1/2";
		static inline const juce::String fb2 = u8"Fb3";
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
			static inline const juce::String key = u8" Op";
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

			static inline const juce::String phaseOffset = u8"P.Offset";
		}
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
			static inline const juce::String enable = u8"Enable HW Env";
			static inline const juce::String shape = u8"Shape";
			static inline const juce::String speed = u8"Period";
		}
	}

	namespace Wt
	{
		static inline const juce::String level = u8"Level";
		static inline const juce::String form = u8"Form";
		static inline const juce::String size = u8"Size";
		static inline const juce::String sampleSize = u8"Sample Size";
		static inline const juce::String steps = u8"Steps";
		static inline const juce::String waveform = u8"Waveform";
		static inline const juce::String fileImport = u8"Import (.wt)";
		static inline const juce::String fileExport = u8"Export (.wt)";

		namespace Mod
		{
			static inline const juce::String enable = u8"Enable";
			static inline const juce::String depth = u8"Depth";
			static inline const juce::String speed = u8"Speed";
		}

		namespace Custom
		{
			static inline const juce::String to0 = u8"-> 0.0";
			static inline const juce::String to1 = u8"-> 1.0";
			static inline const juce::String toM1 = u8"-> -1.0";
			static inline const juce::String smooth = u8"Smooth Waveform";
		}
	}

	namespace Rhythm
	{
		static inline const juce::String vol = u8"Level";

		namespace Pad
		{
			static inline const juce::String key = u8"Pad";
			static inline const juce::String vol = u8"Level";
			static inline const juce::String note = u8"Note";
			static inline const juce::String quality = u8"Bit";
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String oneShot = u8"One Shot";
			static inline const juce::String pan = u8"Pan";
			static inline const juce::String rr = u8"Rr";
			static inline const juce::String pcmOffset = u8"Offset";
			static inline const juce::String pcmRatio = u8"Ratio";

			namespace Pan
			{
				static inline const juce::String l = u8"L";
				static inline const juce::String c = u8"C";
				static inline const juce::String r = u8"R";
			}
		}
	}

	namespace Adpcm
	{
		static inline const juce::String level = u8"Level";
		static inline const juce::String pan = u8"Pan";
		static inline const juce::String quality = u8"Bit";
		static inline const juce::String rate = u8"Rate";
		static inline const juce::String loop = u8"Loop";
		static inline const juce::String pcmOffset = u8"Offset";
		static inline const juce::String pcmRatio = u8"Ratio";

		namespace Pan
		{
			static inline const juce::String l = u8"L";
			static inline const juce::String c = u8"C";
			static inline const juce::String r = u8"R";
		}

		namespace Adsr
		{
			static inline const juce::String ar = u8"Ar";
			static inline const juce::String dr = u8"Dr";
			static inline const juce::String sl = u8"Sl";
			static inline const juce::String rr = u8"Rr";
		}
	}

	namespace Beep
	{
		static inline const juce::String Level = u8"Level";
		static inline const juce::String Fix = u8"Enable";
		static inline const juce::String FFreq = u8"Freq";
		static inline const juce::String To2k = u8"-> 2k";

		namespace Adsr
		{
			static inline const juce::String Bypass = u8"Bypass";
			static inline const juce::String Ar = u8"Ar";
			static inline const juce::String Dr = u8"Dr";
			static inline const juce::String Sl = u8"Sl";
			static inline const juce::String Rr = u8"Rr";
		}
	}

	namespace Fx
	{
		static inline const juce::String masterBypass = u8"Bypass";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String reset = u8"Reset";
		static inline const juce::String mix = u8"Mix";
		
		namespace Mix
		{
			static inline const juce::String dry = u8"Dry";
			static inline const juce::String mix = u8"50%";
			static inline const juce::String wet = u8"Wet";
		}

		namespace Tremolo
		{
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String depth = u8"Depth";
		}

		namespace Vibrate
		{
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String depth = u8"Depth";
		}

		namespace Mbc
		{
			static inline const juce::String bit = u8"Bit";
			static inline const juce::String rate = u8"Rate";
		}

		namespace Delay
		{
			static inline const juce::String time = u8"Time";
			static inline const juce::String fb = u8"Fb";
		}

		namespace Reverb
		{
			static inline const juce::String size = u8"Size";
			static inline const juce::String damp = u8"Damp";
		}

		namespace Filter
		{
			static inline const juce::String type = u8"Type";
			static inline const juce::String freq = u8"Freq";
			static inline const juce::String q = u8" Q";
		}
	}

	namespace Preview
	{
		static inline const juce::String show = u8">>";
		static inline const juce::String hide = u8"<<";
	}

	namespace Preset
	{
		namespace Dialog
		{
			static inline const juce::String initPreset = u8"Initialize Preset";
			static inline const juce::String initPresetNotice = u8"Are you sure you want to initialize all parameters and unload samples?";
			static inline const juce::String initPresetSuccedBtn = u8"Initialize";
			static inline const juce::String initPresetCancelBtn = u8"Cancel";

			static inline const juce::String deletePreset = u8"Delete Preset";
			static inline const juce::String deletePresetNotice = u8"Are you sure you want to delete ";
			static inline const juce::String deletePresetSuccedBtn = u8"Delete";
			static inline const juce::String deletePresetCancelBtn = u8"Cancel";

			static inline const juce::String reflectPresetToolTipMessage = "Reflect selected preset info to text editors without loading";
		}
	}
}
