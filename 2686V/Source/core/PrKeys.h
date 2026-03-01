// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

namespace PrKey
{
	static inline const juce::String mode = "MODE";
	static inline const juce::String masterVol = "MASTER_VOL";

	namespace Prefix
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
	};

	namespace Innder
	{
		static inline const juce::String op = "_OP";
		static inline const juce::String pad = "_PAD";
		static inline const juce::String adsr = "_ADSR";
		static inline const juce::String custom32 = "_CUSTOM32_";
		static inline const juce::String custom64 = "_CUSTOM64_";

		namespace Fx
		{
			static inline const juce::String vib = "_VIB";
			static inline const juce::String trm = "_TRM";
			static inline const juce::String dly = "_DLY";
			static inline const juce::String rvb = "_RVB";
			static inline const juce::String mbc = "_MBC";
			static inline const juce::String rbc = "_RBC";
			static inline const juce::String fil = "_FIL";
			static inline const juce::String scl = "_SCL";
		};
	};

	namespace Post
	{
		static inline const juce::String bypass = "_BYPASS";

		namespace Adsr
		{
			static inline const juce::String ar = "_AR";
			static inline const juce::String dr = "_DR";
			static inline const juce::String sl = "_SL";
			static inline const juce::String rr = "_RR";
		};

		namespace Fm
		{
			static inline const juce::String alg = "_ALGORITHM";
			static inline const juce::String fb0 = "_FEEDBACK";
			static inline const juce::String fb2 = "_FEEDBACK2";
			static inline const juce::String bit = "_BIT";
			static inline const juce::String rate = "_RATE";

			namespace Lfo
			{
				static inline const juce::String freq = "_LFO_FREQ";
				static inline const juce::String am = "_LFO_AM";
				static inline const juce::String pm = "_LFO_PM";
				static inline const juce::String pms = "_LFO_PMS";
				static inline const juce::String ams = "_LFO_AMS";
				static inline const juce::String pmd = "_LFO_PMD";
				static inline const juce::String amd = "_LFO_AMD";
			};

			namespace Op
			{
				static inline const juce::String mul = "_MUL";

				static inline const juce::String dt = "_DT";

				static inline const juce::String dt1 = "_DT1";
				static inline const juce::String dt2 = "_DT2";

				static inline const juce::String tl = "_TL";

				static inline const juce::String ar = "_AR";
				static inline const juce::String dr = "_DR";
				static inline const juce::String sl = "_SL";
				static inline const juce::String sr = "_SR";
				static inline const juce::String rr = "_RR";

				static inline const juce::String d1r = "_D1R";
				static inline const juce::String d1l = "_D1L";
				static inline const juce::String d2r = "_D2R";

				static inline const juce::String ks = "_KS";
				static inline const juce::String ksr = "_KSR";
				static inline const juce::String ksl = "_KSL";

				static inline const juce::String se = "_SE";
				static inline const juce::String seFreq = "_SEFREQ";

				static inline const juce::String am = "_AM";
				static inline const juce::String vib = "_VIB";

				static inline const juce::String pms = "_PMS";
				static inline const juce::String ams = "_AMS";

				static inline const juce::String ws = "_WS";

				static inline const juce::String eg = "_EG";
				static inline const juce::String egType = "_EG_TYP";

				static inline const juce::String fix = "_FIX";
				static inline const juce::String fixFreq = "_FREQ";
				static inline const juce::String fixFreqTo0 = "_FREQ_TO_0";
				static inline const juce::String fixFreqTo440 = "_FREQ_TO_440";

				static inline const juce::String mask = "_MASK";
			};
		};

		namespace Ssg {
			static inline const juce::String bit = "_BIT";
			static inline const juce::String rate = "_RATE";
			static inline const juce::String tone = "_LEVEL";
			static inline const juce::String noise = "_NOISE";
			static inline const juce::String noiseFreq = "_NOISE_FREQ";
			static inline const juce::String noiseOnNote = "_NOISE_ON_NOTE";
			static inline const juce::String mix = "_MIX";
			static inline const juce::String wveform = "_WAVEFORM";

			namespace Adsr
			{
				static inline const juce::String ar = "_AR";
				static inline const juce::String dr = "_DR";
				static inline const juce::String sl = "_SL";
				static inline const juce::String rr = "_RR";
			};

			namespace Duty
			{
				static inline const juce::String mode = "_DUTY_MODE";
				static inline const juce::String preset = "_DUTY_PRESET";
				static inline const juce::String var = "_DUTY_VAR";
				static inline const juce::String inv = "_DUTY_INV";
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
		};

		namespace Wt {
			static inline const juce::String level = "_LEVEL";
			static inline const juce::String bit = "_BIT";
			static inline const juce::String rate = "_RATE";
			static inline const juce::String wave = "_WAVE";
			static inline const juce::String sampleSize = "_SIZE";

			namespace Mod {
				static inline const juce::String enable = "_MOD_ENABLE";
				static inline const juce::String depth = "_MOD_DEPTH";
				static inline const juce::String speed = "_MOD_SPEED";
			};
		};

		namespace Rhythm
		{
			static inline const juce::String level = "_LEVEL";
			namespace Pad
			{
				static inline const juce::String volume = "_VOL";
				static inline const juce::String mode = "_MODE";
				static inline const juce::String rate = "_RATE";
				static inline const juce::String pan = "_PAN";
				static inline const juce::String note = "_NOTE";
				static inline const juce::String oneShot = "_ONESHOT";
				static inline const juce::String rr = "_RR";
			};
		};

		namespace Adpcm
		{
			static inline const juce::String level = "_LEVEL";
			static inline const juce::String pan = "_PAN";
			static inline const juce::String mode = "_MODE";
			static inline const juce::String rate = "_RATE";
			static inline const juce::String loop = "_LOOP";

			namespace Adsr
			{
				static inline const juce::String ar = "_AR";
				static inline const juce::String dr = "_DR";
				static inline const juce::String sl = "_SL";
				static inline const juce::String rr = "_RR";
			};
		};

		namespace Beep {
			static inline const juce::String level = "_LEVEL";
			static inline const juce::String fix = "_FIX";
			static inline const juce::String fixFreq = "_FREQ";

			namespace Adsr
			{
				static inline const juce::String ar = "_AR";
				static inline const juce::String dr = "_DR";
				static inline const juce::String sl = "_SL";
				static inline const juce::String rr = "_RR";
			};
		};

		namespace Fx {
			static inline const juce::String mix = "_MIX";

			namespace Tremolo
			{
				static inline const juce::String rate = "_RATE";
				static inline const juce::String depth = "_DEPTH";
			};

			namespace Vibrato
			{
				static inline const juce::String rate = "_RATE";
				static inline const juce::String depth = "_DEPTH";
			};

			namespace Delay
			{
				static inline const juce::String time = "_TIME";
				static inline const juce::String fb = "_FB";
			};

			namespace Reverb
			{
				static inline const juce::String size = "_SIZE";
				static inline const juce::String damp = "_DAMP";
			};

			namespace Mbc
			{
				static inline const juce::String rate = "_RATE";
				static inline const juce::String bit = "_BITS";
			};

			namespace Rbc
			{
				static inline const juce::String rate = "_RATE";
				static inline const juce::String bit = "_BITS";
			};

			namespace Filter
			{
				static inline const juce::String type = "_TYPE";
				static inline const juce::String freq = "_FREQ";
				static inline const juce::String q = "_Q";
			};
		};
	}
};

