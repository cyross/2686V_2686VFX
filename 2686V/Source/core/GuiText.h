// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace GuiText
{
	static inline const juce::String mode = "Mode";

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

	namespace Group
	{
		static inline const juce::String mainGroup = "Main";
		static inline const juce::String opPrefix = "Operator ";

		namespace Fm
		{
			static inline const juce::String bit = "BIT";
			static inline const juce::String rate = "RATE";
			static inline const juce::String alg = "ALG";
			static inline const juce::String fb0 = "FB0";
			static inline const juce::String fb2 = "FB2";
			static inline const juce::String lfoFreq = "L.FREQ";
			static inline const juce::String amEn = "L.AM-EN";
			static inline const juce::String pmEn = "L.PM-EN";
			static inline const juce::String ams = "L.AMS";
			static inline const juce::String pms = "L.PMS";
			static inline const juce::String amd = "L.AMD";
			static inline const juce::String pmd = "L.PMD";

			namespace Post
			{
				static inline const juce::String bit = " BIT";
				static inline const juce::String rate = " RATE";
				static inline const juce::String alg = " ALG";
				static inline const juce::String fb0 = " FB0";
				static inline const juce::String fb2 = " FB2";

				namespace Lfo
				{
					static inline const juce::String freq = " L.FREQ";
					static inline const juce::String ams = " L.AMS";
					static inline const juce::String pms = " L.PMS";
				}
			}

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
				static inline const juce::String Tl= "TL";
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
				static inline const juce::String EgType = "EGTYPE";
				static inline const juce::String Eg = "EG";
				static inline const juce::String Ws = "WS";
				static inline const juce::String Pcm = "PCM";
				static inline const juce::String PcmClear = "X";
				static inline const juce::String FreqTo0 = "Freq -> 0Hz";
				static inline const juce::String FreqTo440 = "Freq -> 440Hz";
				static inline const juce::String Opzx3FreqTo0 = "->0";
				static inline const juce::String Opzx3FreqTo05 = "->0.5";
				static inline const juce::String Opzx3FreqTo1 = "->1";
				static inline const juce::String Opzx3FreqTo2 = "->2";
				static inline const juce::String Opzx3FreqTo440 = "->440";

				namespace Post
				{
					static inline const juce::String ar = " AR";
					static inline const juce::String dr = " DR";
					static inline const juce::String sl = " SL";
					static inline const juce::String rr = " RR";
					static inline const juce::String sr = " SR";
					static inline const juce::String d1r = " D1R";
					static inline const juce::String d1l = " D1L";
					static inline const juce::String d2r = " D2R";
					static inline const juce::String mul = " MUL";
					static inline const juce::String tl = " Tl";
					static inline const juce::String dt = " Dt";
					static inline const juce::String dt1 = " Dt1";
					static inline const juce::String dt2 = " Dt2";
					static inline const juce::String se = " SE";
					static inline const juce::String seFreq = " SEFREQ";
					static inline const juce::String ks = " KS";
					static inline const juce::String ksr = " KSR";
					static inline const juce::String ksl = " KSL";
					static inline const juce::String fix = " FIX";
					static inline const juce::String fixFreq = " FREQ";
					static inline const juce::String ws = " WS";
					static inline const juce::String am = " AM";
					static inline const juce::String vib = " VIB";
					static inline const juce::String egType = " EG TYPE";
					static inline const juce::String eg = " EG";

					static inline const juce::String mask = " MASK";
				}
			}
		}

		namespace Ssg
		{
			namespace Post
			{
				static inline const juce::String tone = " Tone";
				static inline const juce::String toneNoiseMix = " Tone/Noise Mix";
				static inline const juce::String waveform = " Waveform";

				namespace Noise
				{
					static inline const juce::String title = " Noise";
					static inline const juce::String freq = " Noise Freq";
					static inline const juce::String noiseOnNote = " Noise On Note";
				}

				namespace Adsr
				{
					static inline const juce::String bypass = " ADSR Bypass";
					static inline const juce::String ar = " AR";
					static inline const juce::String dr = " DR";
					static inline const juce::String sl = " SL";
					static inline const juce::String rr = " RR";
				}

				namespace Duty
				{
					static inline const juce::String mode = " Duty Mode";
					static inline const juce::String preset = " Duty Preset";
					static inline const juce::String var = " Duty Var";
					static inline const juce::String invert = " Duty Invert";
				}

				namespace Tri
				{
					static inline const juce::String keyTrack = " Triangle Key Track";
					static inline const juce::String peak = " Triangle Peak";
					static inline const juce::String manualFreq = " Tri Manual Freq";
				}

				namespace HwEnv
				{
					static inline const juce::String enable = " HW Env Enable";
					static inline const juce::String shape = " Env Shape";
					static inline const juce::String speed = " Env Speed";
				}
			}

		}

		namespace Wt
		{
			namespace Key
			{
				static inline const juce::String custom32 = " Custom32 ";
				static inline const juce::String custom64 = " Custom64 ";
			}

			namespace Post
			{
				static inline const juce::String level = " Level";
				static inline const juce::String sampleSize = " Sample Size";
				static inline const juce::String waveform = " Waveform";

				namespace Mod
				{
					static inline const juce::String enable = " Mod Enable";
					static inline const juce::String depth = " Mod Depth";
					static inline const juce::String speed = " Mod Speed";
				}
			}
		}

		namespace Rhythm
		{

			namespace Pad
			{
				static inline const juce::String key = " Pad";

				namespace Post
				{
					static inline const juce::String vol = " Vol";
					static inline const juce::String note = " Note";
					static inline const juce::String rate = " Rate";
					static inline const juce::String quality = " Quality";
					static inline const juce::String oneShot = " One Shot";
					static inline const juce::String pan = " Pan";
					static inline const juce::String rr = " RR";
				}
			}
		}

		namespace Adpcm
		{
			namespace Post
			{
				namespace Adsr
				{
					static inline const juce::String ar = " AR";
					static inline const juce::String dr = " DR";
					static inline const juce::String sl = " SL";
					static inline const juce::String rr = " RR";
				}

				static inline const juce::String pan= " Pan";
				static inline const juce::String rate = " Rate";
				static inline const juce::String level = " Level";
				static inline const juce::String loop = " Loop";
				static inline const juce::String quality = " Quality";
			}
		}

		namespace Beep
		{
			static inline const juce::String Level = "LEVEL";
			static inline const juce::String Fix = "FIX";
			static inline const juce::String FFreq = "FFREQ";
			static inline const juce::String To2k = "-> 2K";
			
			namespace Adsr
			{
				static inline const juce::String Bypass = "Bypass ADSR";
				static inline const juce::String Ar = "AR";
				static inline const juce::String Dr = "DR";
				static inline const juce::String Sl = "SL";
				static inline const juce::String Rr = "RR";
			}

			namespace Post
			{
				namespace Adsr
				{
					static inline const juce::String ar = " AR";
					static inline const juce::String dr = " DR";
					static inline const juce::String sl = " SL";
					static inline const juce::String rr = " RR";
				}

				static inline const juce::String level = " Level";
				static inline const juce::String fix = " FIX";
				static inline const juce::String fixFreq = " FREQ";
			}
		}

		namespace Fx
		{
			static inline const juce::String masterBypass = "M.Bypass";

			namespace Post
			{
				static inline const juce::String tremolo = " Tremolo";
				static inline const juce::String vibrato = " Vibrato";
				static inline const juce::String mbc = " Modern BC";
				static inline const juce::String delay = " Delay";
				static inline const juce::String reverb = " Reverb";
				static inline const juce::String rbc = " Retro BC";
				static inline const juce::String filter = " Filter";
				static inline const juce::String softClipper = " Soft Clipper";

				namespace Tremolo
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String rate = " Rate";
					static inline const juce::String depth = " Depth";
					static inline const juce::String mix = " Mix";
				}

				namespace Vibrator
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String rate = " Rate";
					static inline const juce::String depth = " Depth";
					static inline const juce::String mix = " Mix";
				}

				namespace Mbc
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String rate = " Rate";
					static inline const juce::String depth = " Depth";
					static inline const juce::String mix = " Mix";
				}

				namespace Delay
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String time = " Time";
					static inline const juce::String fb = " Feedback";
					static inline const juce::String mix = " Mix";
				}

				namespace Reverb
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String size = " Size";
					static inline const juce::String damp = " Damp";
					static inline const juce::String mix = " Mix";
				}

				namespace Rbc
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String rate = " Rate";
					static inline const juce::String quality = " Quality";
					static inline const juce::String mix = " Mix";
				}

				namespace Filter
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String type = " Type";
					static inline const juce::String freq = " Freq";
					static inline const juce::String q = " Q";
					static inline const juce::String mix = " Mix";
				}

				namespace SoftClipper
				{
					static inline const juce::String bypass = " Bypass";
					static inline const juce::String mix = " Mix";
				}
			}
		}
	}

	namespace Preview
	{
		static inline const juce::String show = ">>";
		static inline const juce::String hide = "<<";
	}
}
