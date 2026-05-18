// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

namespace CorePrKey
{
	static inline const juce::String mode = "MODE";
	static inline const juce::String masterVol = "MASTER_VOL";
	static inline const juce::String monoMode = "MONO_MODE";

	static inline const juce::String op = "_OP";
	static inline const juce::String pitchAdsr = "_PITCHENV";
	static inline const juce::String adsr = "_ADSR";
	static inline const juce::String bypass = "_BYPASS";

	namespace Adsr
	{
		static inline const juce::String ar = "_AR";
		static inline const juce::String dr = "_DR";
		static inline const juce::String sl = "_SL";
		static inline const juce::String rr = "_RR";
	};

	namespace PitchAdsr
	{
		static inline const juce::String enable = "_PITCH_ENABLE";
		static inline const juce::String ar = "_PITCH_AR";
		static inline const juce::String dr = "_PITCH_DR";
		static inline const juce::String rr = "_PITCH_RR";
		static inline const juce::String stl = "_PITCH_STL";
		static inline const juce::String atl = "_PITCH_ATL";
		static inline const juce::String ssl = "_PITCH_SSL";
		static inline const juce::String rll = "_PITCH_RLL";
	};

	namespace Fm
	{
		static inline const juce::String alg = "_ALG";
		static inline const juce::String fb = "_FB1";
		static inline const juce::String bit = "_BIT";
		static inline const juce::String rate = "_RATE";
	};

	namespace Post
	{

		namespace Adsr
		{
			static inline const juce::String ar = "_AR";
			static inline const juce::String dr = "_DR";
			static inline const juce::String sl = "_SL";
			static inline const juce::String rr = "_RR";
		};

		namespace PitchAdsr
		{
			static inline const juce::String enable = "_PITCH_ENABLE";
			static inline const juce::String ar = "_PITCH_AR";
			static inline const juce::String dr = "_PITCH_DR";
			static inline const juce::String rr = "_PITCH_RR";
			static inline const juce::String stl = "_PITCH_STL";
			static inline const juce::String atl = "_PITCH_ATL";
			static inline const juce::String ssl = "_PITCH_SSL";
			static inline const juce::String rll = "_PITCH_RLL";
		};

		namespace Fm
		{

			namespace Lfo
			{
				static inline const juce::String freq = "_LFO_FREQ";
				static inline const juce::String shape = "_LFO_SHAPE";
				static inline const juce::String pgShape = "_LFO_PG_SHAPE";
				static inline const juce::String egShape = "_LFO_EG_SHAPE";
				static inline const juce::String am = "_LFO_AM";
				static inline const juce::String pm = "_LFO_PM";
				static inline const juce::String pms = "_LFO_PMS";
				static inline const juce::String ams = "_LFO_AMS";
				static inline const juce::String pmd = "_LFO_PMD";
				static inline const juce::String amd = "_LFO_AMD";
				static inline const juce::String amSmoothRatio = "_LFO_ASMRT";
				static inline const juce::String syncDelay = "_LFO_SYNC_DELAY";
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

				static inline const juce::String lfoFreq = "_LFO_FREQ";
				static inline const juce::String freqs = "_LFO_FREQS";

				static inline const juce::String am = "_AM";
				static inline const juce::String vib = "_VIB";
				static inline const juce::String amsEn = "_AMS_EN";

				static inline const juce::String pgShape = "_LFO_PG_SHAPE";
				static inline const juce::String egShape = "_LFO_EG_SHAPE";

				static inline const juce::String pms = "_PMS";
				static inline const juce::String ams = "_AMS";
				static inline const juce::String pmd = "_PMD";
				static inline const juce::String amd = "_AMD";

				static inline const juce::String n88Amd = "_N88AMD";
				static inline const juce::String n88Ams = "_N88AMS";
				static inline const juce::String n88Pmd = "_N88PMD";
				static inline const juce::String n88Pms = "_N88PMS";

				static inline const juce::String syncDelay = "_SYNC_DELAY";

				static inline const juce::String ws = "_WS";

				static inline const juce::String eg = "_EG";
				static inline const juce::String egType = "_EG_TYP";

				static inline const juce::String fix = "_FIX";
				static inline const juce::String fixFreq = "_FREQ";
				static inline const juce::String fixFreqTo0 = "_FREQ_TO_0";
				static inline const juce::String fixFreqTo440 = "_FREQ_TO_440";

				static inline const juce::String pcmOffset = "_PCM_OFFSET";
				static inline const juce::String pcmRatio = "_PCM_RATIO";

				static inline const juce::String mask = "_MASK";

				static inline const juce::String rgEn = "_R_EN";
				static inline const juce::String rgAr = "_R_AR";
				static inline const juce::String rgDr = "_R_DR";
				static inline const juce::String rgSl = "_R_SL";
				static inline const juce::String rgSr = "_R_SR";
				static inline const juce::String rgRr = "_R_RR";
				static inline const juce::String rgTl = "_R_TL";
				static inline const juce::String rgD1r = "_R_D1R";
				static inline const juce::String rgD1l = "_R_D1L";
				static inline const juce::String rgD2r = "_R_D2R";
			};
		};
	}
};

