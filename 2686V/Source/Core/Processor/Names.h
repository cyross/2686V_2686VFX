#pragma once

#include<JuceHeader.h>

// UI上に表示するタイトル等を管理
namespace CorePrName
{
	static inline const juce::String mode = "Mode";
	static inline const juce::String master_vol = "Master Vol";
	static inline const juce::String monoMode = "Monopnony Mode";

	namespace Adsr
	{
		namespace Post
		{
			static inline const juce::String bypass = " ADSR Bypass";
			static inline const juce::String ar = " AR";
			static inline const juce::String dr = " DR";
			static inline const juce::String sl = " SL";
			static inline const juce::String rr = " RR";
		}
	}

	namespace PitchAdsr
	{
		namespace Post
		{
			static inline const juce::String enable = " PITCH-ENV Enable";
			static inline const juce::String bypass = " PITCH-ENV Bypass";
			static inline const juce::String ar = " PITCH-ENV AR";
			static inline const juce::String dr = " PITCH-ENV DR";
			static inline const juce::String rr = " PITCH-ENV RR";
			static inline const juce::String stl = " PITCH-ENV STL";
			static inline const juce::String atl = " PITCH-ENV ATL";
			static inline const juce::String ssl = " PITCH-ENV SSL";
			static inline const juce::String rll = " PITCH-ENV RLL";
		}
	};

	namespace Fm
	{
		namespace Post
		{
			static inline const juce::String bit = " BIT";
			static inline const juce::String rate = " RATE";
			static inline const juce::String alg = " ALG";
			static inline const juce::String fb0 = " FB1";
			static inline const juce::String fb2 = " FB3";
			static inline const juce::String pan = " PAN";
			static inline const juce::String panpot = " PANPOT";
			static inline const juce::String panpot_en = " PANPOT Eable";

			namespace Lfo
			{
				static inline const juce::String freq = " L.SPEED";
				static inline const juce::String shape = " L.SHAPE";
				static inline const juce::String pgShape = " L.P.SHAPE";
				static inline const juce::String egShape = " L.A.SHAPE";
				static inline const juce::String am = " LFO.AM";
				static inline const juce::String pm = " LFO.PM";
				static inline const juce::String ams = " L.AMS";
				static inline const juce::String pms = " L.PMS";
				static inline const juce::String pmd = " L.PMD";
				static inline const juce::String amd = " L.AMD";
				static inline const juce::String amSmoothRatio = " L.ASMRT";
				static inline const juce::String syncDelay = " L.S.Delay";
			}
		}

		namespace Op
		{
			static inline const juce::String key = " OP";

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
			static inline const juce::String amsEn = " AMS EN";

			static inline const juce::String lfoFreq = " LFO FREQ";
			static inline const juce::String freqs = " LFO FREQS";

			static inline const juce::String pgShape = " L.P.SHAPE";
			static inline const juce::String egShape = " L.A.SHAPE";

			static inline const juce::String pms = " PMS";
			static inline const juce::String ams = " AMS";
			static inline const juce::String pmd = " PMD";
			static inline const juce::String amd = " AMD";

			static inline const juce::String n88Amd = " N88AMD";
			static inline const juce::String n88Ams = " N88AMS";
			static inline const juce::String n88Pmd = " N88PMD";
			static inline const juce::String n88Pms = " N88PMS";

			static inline const juce::String syncDelay = " S.DLY";
			static inline const juce::String egType = " EG TYPE";
			static inline const juce::String eg = " EG";
			static inline const juce::String pcmOffset = " PCM Offset";
			static inline const juce::String pcmRatio = " PCM Ratio";

			static inline const juce::String mask = " MASK";

			static inline const juce::String rgEn = " R_EN";
			static inline const juce::String rgAr = " R_AR";
			static inline const juce::String rgDr = " R_DR";
			static inline const juce::String rgSl = " R_SL";
			static inline const juce::String rgSr = " R_SR";
			static inline const juce::String rgRr = " R_RR";
			static inline const juce::String rgTl = " R_TL";
			static inline const juce::String rgD1r = " R_D1R";
			static inline const juce::String rgD1l = " R_D1L";
			static inline const juce::String rgD2r = " R_D2R";

			static inline const juce::String sus = " SUS";
			static inline const juce::String xof = " XOF";

			static inline const juce::String phaseOffset = "PHASE OFFSET";
		}
	}
}
