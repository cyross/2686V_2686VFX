#pragma once

#include<JuceHeader.h>

// UI上に表示するタイトル等を管理
namespace PrName
{
	static inline const juce::String mode = "Mode";
	static inline const juce::String master_vol = "Master Vol";

	namespace Adsr
	{
		namespace Post
		{
			static inline const juce::String ar = " AR";
			static inline const juce::String dr = " DR";
			static inline const juce::String sl = " SL";
			static inline const juce::String rr = " RR";
		}
	}

	namespace Fm
	{
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
				static inline const juce::String shape = " L.SHAPE";
				static inline const juce::String am = " LFO.AM";
				static inline const juce::String pm = " LFO.PM";
				static inline const juce::String ams = " L.AMS";
				static inline const juce::String pms = " L.PMS";
				static inline const juce::String pmd = " L.PMD";
				static inline const juce::String amd = " L.AMD";
				static inline const juce::String amSmoothRatio = " L.ASMRT";
			}
		}

		namespace Op
		{
			static inline const juce::String key = " OP";

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
				static inline const juce::String pms = " PMS";
				static inline const juce::String ams = " AMS";
				static inline const juce::String pmd = " PMD";
				static inline const juce::String amd = " AMD";
				static inline const juce::String egType = " EG TYPE";
				static inline const juce::String eg = " EG";
				static inline const juce::String pcmOffset = " PCM Offset";
				static inline const juce::String pcmRatio = " PCM Ratio";

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
			static inline const juce::String bit = " BIT";
			static inline const juce::String rate = " RATE";

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
				static inline const juce::String period = " Env Period";
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
			static inline const juce::String bit = " BIT";
			static inline const juce::String rate = " RATE";

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
		static inline const juce::String vol = " Vol";

		namespace Pad
		{
			static inline const juce::String key = " Pad";

			namespace Post
			{
				static inline const juce::String vol = " Vol";
				static inline const juce::String note = " Note";
				static inline const juce::String rate = " Rate";
				static inline const juce::String bit = " Bit";
				static inline const juce::String oneShot = " One Shot";
				static inline const juce::String pan = " Pan";
				static inline const juce::String rr = " RR";
				static inline const juce::String pcmOffset = " PCM Offset";
				static inline const juce::String pcmRatio = " PCM Ratio";
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

			static inline const juce::String pan = " Pan";
			static inline const juce::String rate = " Rate";
			static inline const juce::String level = " Level";
			static inline const juce::String loop = " Loop";
			static inline const juce::String bit = " Bit";
			static inline const juce::String pcmOffset = " PCM Offset";
			static inline const juce::String pcmRatio = " PCM Ratio";
		}
	}

	namespace Beep
	{
		namespace Post
		{
			static inline const juce::String level = " Level";
			static inline const juce::String fix = " FIX";
			static inline const juce::String fixFreq = " FREQ";

			namespace Adsr
			{
				static inline const juce::String bypass = " ADSR Bypass";
				static inline const juce::String ar = " AR";
				static inline const juce::String dr = " DR";
				static inline const juce::String sl = " SL";
				static inline const juce::String rr = " RR";
			}
		}
	}

	namespace Fx
	{
		static inline const juce::String masterBypass = "M.Bypass";

		namespace Post
		{
			static inline const juce::String rlfo = " Retro LFO";
			static inline const juce::String tremolo = " Tremolo";
			static inline const juce::String vibrato = " Vibrato";
			static inline const juce::String mbc = " Modern BC";
			static inline const juce::String delay = " Delay";
			static inline const juce::String reverb = " Reverb";
			static inline const juce::String rbc = " Retro BC";
			static inline const juce::String filter = " Filter";
			static inline const juce::String softClipper = " Soft Clipper";

			namespace RetroLfo
			{
				static inline const juce::String bypass = " Bypass";
				static inline const juce::String wave = " Wave";
				static inline const juce::String freq = " Freq";
				static inline const juce::String ams = " Ams";
				static inline const juce::String pms = " Pms";
				static inline const juce::String amd = " Amd";
				static inline const juce::String pmd = " Pmd";
				static inline const juce::String mix = " Mix";
			}

			namespace Tremolo
			{
				static inline const juce::String bypass = " Bypass";
				static inline const juce::String rate = " Rate";
				static inline const juce::String depth = " Depth";
				static inline const juce::String mix = " Mix";
			}

			namespace Vibrato
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
				static inline const juce::String bit = " Bit";
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
				static inline const juce::String bit = " Bit";
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
