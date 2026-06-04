#pragma once

#include<JuceHeader.h>

// UI上に表示するタイトル等を管理
namespace CorePrName
{
	static inline const juce::String mode = "Mode";
	static inline const juce::String master_vol = "Master Vol";
	static inline const juce::String monoMode = "Monophony Mode";
	static inline const juce::String useVelocity = "Use Velocity";

	namespace Unison
	{
		static inline const juce::String voices = " Unison Voices";
		static inline const juce::String detune = " Unison Detune";
		static inline const juce::String spread = " Unison Spread";
	}

	namespace Adsr
	{
		namespace Post
		{
			static inline const juce::String bypass = " ADSR Bypass";
			static inline const juce::String ar = " Ar";
			static inline const juce::String dr = " Dr";
			static inline const juce::String sl = " Sl";
			static inline const juce::String rr = " Rr";
			static inline const juce::String stl = " Stl";
		}
	}

	namespace PitchAdsr
	{
		namespace Post
		{
			static inline const juce::String enable = " PitchEnv Enable";
			static inline const juce::String bypass = " PitchEnv Bypass";
			static inline const juce::String ar = " PitchEnv Ar";
			static inline const juce::String dr = " PitchEnv Dr";
			static inline const juce::String rr = " PitchEnv Rr";
			static inline const juce::String stl = " PitchEnv Stl";
			static inline const juce::String atl = " PitchEnv Atl";
			static inline const juce::String ssl = " PitchEnv Ssl";
			static inline const juce::String rll = " PitchEnv Rll";
		}
	};

	namespace SsgSwEnv
	{
		namespace Post
		{
			static inline const juce::String enable = " SSG SwEnv Enable";
			static inline const juce::String bypass = " SSG SwEnv Bypass";
			static inline const juce::String steps = " SSG SwEnv Steps";
			static inline const juce::String loop = " SSG SwEnv Loop";
			static inline const juce::String loopTo = " SSG SwEnv Loop To";
			static inline const juce::String loopCount = " SSG SwEnv Loop Count";
			static inline const juce::String stl = " SSG SwEnv STL";
			static inline const juce::String r1 = " SSG SwEnv R1";
			static inline const juce::String l1 = " SSG SwEnv L1";
			static inline const juce::String r2 = " SSG SwEnv R2";
			static inline const juce::String l2 = " SSG SwEnv L2";
			static inline const juce::String r3 = " SSG SwEnv R3";
			static inline const juce::String l3 = " SSG SwEnv L3";
			static inline const juce::String r4 = " SSG SwEnv R4";
			static inline const juce::String l4 = " SSG SwEnv L4";
			static inline const juce::String r5 = " SSG SwEnv R5";
			static inline const juce::String l5 = " SSG SwEnv L5";
			static inline const juce::String r6 = " SSG SwEnv R6";
			static inline const juce::String l6 = " SSG SwEnv L6";
		}
	}

	namespace Fm
	{
		namespace Post
		{
			static inline const juce::String bit = " Bit";
			static inline const juce::String rate = " Rate";
			static inline const juce::String alg = " Algorithm";
			static inline const juce::String fb = " Feedback";
			static inline const juce::String pan = " Pan";
			static inline const juce::String panpot = " Panpot";
			static inline const juce::String panpot_en = " Panpot Eable";

			namespace Lfo
			{
				static inline const juce::String freq = " LFO Speed";
				static inline const juce::String shape = " LFO Shape";
				static inline const juce::String pgShape = " LFO PM Shape";
				static inline const juce::String egShape = " LFO AM Shape";
				static inline const juce::String am = " LFO AM Enable";
				static inline const juce::String pm = " LFO PM Enable";
				static inline const juce::String ams = " LFO AMS";
				static inline const juce::String pms = " LFO PMS";
				static inline const juce::String pmd = " LFO PMD";
				static inline const juce::String amd = " LFO AMD";
				static inline const juce::String amSmoothRatio = " LFO ASMRT";
				static inline const juce::String syncDelay = " LFO Sync Delay";
			}
		}

		namespace Op
		{
			static inline const juce::String key = " Op";

			static inline const juce::String ar = " AR";
			static inline const juce::String dr = " DR";
			static inline const juce::String sl = " SL";
			static inline const juce::String rr = " RR";
			static inline const juce::String sr = " SR";
			static inline const juce::String d1r = " D1R";
			static inline const juce::String d1l = " D1L";
			static inline const juce::String d2r = " D2R";
			static inline const juce::String mul = " MUL";
			static inline const juce::String tl = " TL";
			static inline const juce::String dt = " DT";
			static inline const juce::String dt1 = " DT1";
			static inline const juce::String dt2 = " DT2";
			static inline const juce::String se = " SSG Env";
			static inline const juce::String seFreq = " SSG Env Freq";
			static inline const juce::String ks = " KS";
			static inline const juce::String ksr = " KSR";
			static inline const juce::String ksl = " KSL";
			static inline const juce::String fix = " Fix Enable";
			static inline const juce::String fixFreq = " Fix Freq";
			static inline const juce::String ws = " Wave Shape";

			static inline const juce::String am = " LFO AM";
			static inline const juce::String vib = " LFO PM";
			static inline const juce::String amsEn = " LFO AMS Enable";

			static inline const juce::String lfoFreq = " LFO Freq";
			static inline const juce::String freqs = " LFO Freqs";

			static inline const juce::String pgShape = " LFO PM Shape";
			static inline const juce::String egShape = " LFO AM Shape";

			static inline const juce::String pms = " LFO PMS";
			static inline const juce::String ams = " LFO AMS";
			static inline const juce::String pmd = " LFO PMD";
			static inline const juce::String amd = " LFO AMD";

			static inline const juce::String n88Amd = " N88 LFO AMD";
			static inline const juce::String n88Ams = " N88 LFO AMS";
			static inline const juce::String n88Pmd = " N88 LFO PMD";
			static inline const juce::String n88Pms = " N88 LFO PMS";

			static inline const juce::String syncDelay = " LFO Sync Delay";
			static inline const juce::String egType = " EG Type";
			static inline const juce::String eg = " EG Shape";
			static inline const juce::String pcmOffset = " PCM Offset";
			static inline const juce::String pcmRatio = " PCM Ratio";

			static inline const juce::String mask = " Mask";

			static inline const juce::String rgEn = " Register Mode Enable";
			static inline const juce::String rgAr = " Register Mode AR";
			static inline const juce::String rgDr = " Register Mode DR";
			static inline const juce::String rgSl = " Register Mode SL";
			static inline const juce::String rgSr = " Register Mode SR";
			static inline const juce::String rgRr = " Register Mode RR";
			static inline const juce::String rgTl = " Register Mode TL";
			static inline const juce::String rgD1r = " Register Mode D1R";
			static inline const juce::String rgD1l = " Register Mode D1L";
			static inline const juce::String rgD2r = " Register Mode D2R";

			static inline const juce::String sus = " SUS";
			static inline const juce::String xof = " XOF";

			static inline const juce::String phaseOffset = "Phase Offset";
		}
	}
}
