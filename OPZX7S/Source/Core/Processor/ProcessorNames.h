#pragma once

#include<JuceHeader.h>

// UI上に表示するタイトル等を管理
namespace CPN
{
	static inline const juce::String mode = "Mode";

	namespace Midi
	{
		static inline const juce::String monoMode = "Monophony Mode";
		static inline const juce::String useVelocity = "Use Velocity";
		static inline const juce::String pitchResetOnLegato = "Pitch Reset On Legato";
		static inline const juce::String fixedVelocity = "Fixed Velocity";
	}

	namespace Wt
	{
		static inline const juce::String waveform = " Waveform";
		static inline const juce::String sampleSize = " Sample Size";
		static inline const juce::String steps = " Steps";
	}

	namespace Wt2
	{
		static inline const juce::String waveform = " Waveform";
		static inline const juce::String sampleSize = " Sample Size";
		static inline const juce::String resolution = " Resolution";
	}

	static inline const juce::String level = " Level";
	static inline const juce::String vol = " Vol";

	static inline const juce::String op = " Op";

	static inline const juce::String mul = " MUL";
	static inline const juce::String mulRatio = " MUL Ratio";
	static inline const juce::String dt = " DT";
	static inline const juce::String dt1 = " DT1";
	static inline const juce::String dt2 = " DT2";
	static inline const juce::String dt3 = " DT3";
	static inline const juce::String fix = " Fix Enable";
	static inline const juce::String fixFreq = " Fix Freq";

	static inline const juce::String pan = " Pan";
	static inline const juce::String panpot = " Panpot";
	static inline const juce::String panpot_en = " Panpot Eable";

	static inline const juce::String pcmOffset = " PCM Offset";
	static inline const juce::String pcmRatio = " PCM Ratio";

	static inline const juce::String lpEnable = " Loop-point Enable";
	static inline const juce::String lpStart = " Loop-point Start";
	static inline const juce::String lpEnd = " Loop-point End";

	static inline const juce::String custom32 = " Custom32 ";
	static inline const juce::String custom64 = " Custom64 ";
	static inline const juce::String custom128 = " Custom128 ";
	static inline const juce::String custom256 = " Custom256 ";

	static inline const juce::String note = " Note";
	static inline const juce::String oneShot = " One Shot";
	static inline const juce::String loop = " Loop";

	static inline const juce::String ssgWaveform = " Waveform";

	namespace Tn
	{
		static inline const juce::String tone = " Tone";
		static inline const juce::String noise = " Noise";
		static inline const juce::String freq = " Noise Freq";
		static inline const juce::String nON = " Noise On Note";
		static inline const juce::String mix = " Tone/Noise Mix";
	}

	namespace Fm
	{
		static inline const juce::String alg = " Algorithm";
		static inline const juce::String fb = " Feedback";
		static inline const juce::String fb1 = " Feedback1";
		static inline const juce::String fb2 = " Feedback2";
		static inline const juce::String fb3 = " Feedback3";
		static inline const juce::String fb4 = " Feedback4";
		static inline const juce::String fb5 = " Feedback5";
		static inline const juce::String fb6 = " Feedback6";
		static inline const juce::String fb7 = " Feedback7";
		static inline const juce::String fb8 = " Feedback8";

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
		static inline const juce::String dt3 = " DT3";
		static inline const juce::String se = " SSG Env";
		static inline const juce::String seFreq = " SSG Env Freq";

		static inline const juce::String ksEn = " KS Enable";
		static inline const juce::String ksMode = "KS Mode";
		static inline const juce::String ks = " KS";
		static inline const juce::String ksr = " KSR";
		static inline const juce::String ksl = " KSL";
		static inline const juce::String ksrMA7 = " KSR(MA7)";
		static inline const juce::String kslMA7 = "KSL(MA7)";
		static inline const juce::String ksrOPP = " KSR(OPP)";
		static inline const juce::String kslOPP = " KSL(OPP)";
		static inline const juce::String ksrOPZ = " KSR(OPZ)";
		static inline const juce::String kslOPZ = " KSL(OPZ)";
		static inline const juce::String ksBp = " KS BreakPoint";
		static inline const juce::String ksLc = " KS Left Curve";
		static inline const juce::String ksRc = " KS Right Curve";
		static inline const juce::String ksLd = " KS Left Depth";
		static inline const juce::String ksRd = " KS Right Depth";
		static inline const juce::String ksRs = " KS Rate Scaling";

		static inline const juce::String fix = " Fix Enable";
		static inline const juce::String fixFreq = " Fix Freq";
		static inline const juce::String ws = " Wave Shape";

		static inline const juce::String am = " LFO AM";
		static inline const juce::String vib = " LFO PM";
		static inline const juce::String amsEn = " LFO AMS Enable";

		static inline const juce::String lfoFreq = " LFO Freq";
		static inline const juce::String pmFreq = " LFO PM Freq";
		static inline const juce::String amFreq = " LFO AM Freq";
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
		static inline const juce::String pmSyncDelay = " LFO PM Sync Delay";
		static inline const juce::String amSyncDelay = " LFO AM ync Delay";

		static inline const juce::String amSmoothRatio = " LFO ASMRT";

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
		static inline const juce::String kor = " KOR";
		static inline const juce::String bypass = " AMP Bypass";
	}

	namespace Quality {
		static inline const juce::String bit = " Bit";
		static inline const juce::String rate = " Rate";
	}

	namespace QualityPcm {
		static inline const juce::String bit = " Bit";
		static inline const juce::String rate = " Rate";
		static inline const juce::String interp = " Interpolation Mode";
	}

	namespace Unison
	{
		static inline const juce::String voices = " Unison Voices";
		static inline const juce::String detune = " Unison Detune";
		static inline const juce::String spread = " Unison Spread";
		static inline const juce::String arpEnable = " Unison Arpeggio";
		static inline const juce::String arpFreq = " Unison Arpeggio Freq";
		static inline const juce::String arpSmooth = " Unison Arpeggio Smooth";
		static inline const juce::String paraDistance = " Unison Para Distance ";
		static inline const juce::String paraDetune = " Unison Para Detune ";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
		static inline const juce::String ar = " Ar";
		static inline const juce::String dr = " Dr";
		static inline const juce::String sl = " Sl";
		static inline const juce::String rr = " Rr";
		static inline const juce::String stl = " Stl";
		static inline const juce::String kor = " KOR";
	}

	namespace PitchAdsr
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
	};

	namespace SsgSwEnv
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

	namespace SsgSwEnv11
	{
		static inline const juce::String enable = " SSG SwEnv11 Enable";
		static inline const juce::String bypass = " SSG SwEnv11 Bypass";
		static inline const juce::String steps = " SSG SwEnv11 Steps";
		static inline const juce::String loop = " SSG SwEnv11 Loop";
		static inline const juce::String loopTo = " SSG SwEnv11 Loop To";
		static inline const juce::String loopCount = " SSG SwEnv11 Loop Count";
		static inline const juce::String stl = " SSG SwEnv11 STL";
		static inline const juce::String r1 = " SSG SwEnv11 R1";
		static inline const juce::String l1 = " SSG SwEnv11 L1";
		static inline const juce::String r2 = " SSG SwEnv11 R2";
		static inline const juce::String l2 = " SSG SwEnv11 L2";
		static inline const juce::String r3 = " SSG SwEnv11 R3";
		static inline const juce::String l3 = " SSG SwEnv11 L3";
		static inline const juce::String r4 = " SSG SwEnv11 R4";
		static inline const juce::String l4 = " SSG SwEnv11 L4";
		static inline const juce::String r5 = " SSG SwEnv11 R5";
		static inline const juce::String l5 = " SSG SwEnv11 L5";
		static inline const juce::String r6 = " SSG SwEnv11 R6";
		static inline const juce::String l6 = " SSG SwEnv11 L6";
		static inline const juce::String r7 = " SSG SwEnv11 R7";
		static inline const juce::String l7 = " SSG SwEnv11 L7";
		static inline const juce::String r8 = " SSG SwEnv11 R8";
		static inline const juce::String l8 = " SSG SwEnv11 L8";
		static inline const juce::String r9 = " SSG SwEnv11 R9";
		static inline const juce::String l9 = " SSG SwEnv11 L9";
		static inline const juce::String r10 = " SSG SwEnv11 R10";
		static inline const juce::String l10 = " SSG SwEnv11 L10";
		static inline const juce::String r11 = " SSG SwEnv11 R11";
		static inline const juce::String l11 = " SSG SwEnv11 L11";
	};

	namespace SsgSwPEnv11
	{
		static inline const juce::String enable = " SSG SwPEnv11 Enable";
		static inline const juce::String bypass = " SSG SwPEnv11 Bypass";
		static inline const juce::String steps = " SSG SwPEnv11 Steps";
		static inline const juce::String loop = " SSG SwPEnv11 Loop";
		static inline const juce::String loopTo = " SSG SwPEnv11 Loop To";
		static inline const juce::String loopCount = " SSG SwPEnv11 Loop Count";
		static inline const juce::String stl = " SSG SwPEnv11 STL";
		static inline const juce::String r1 = " SSG SwPEnv11 R1";
		static inline const juce::String l1 = " SSG SwPEnv11 L1";
		static inline const juce::String r2 = " SSG SwPEnv11 R2";
		static inline const juce::String l2 = " SSG SwPEnv11 L2";
		static inline const juce::String r3 = " SSG SwPEnv11 R3";
		static inline const juce::String l3 = " SSG SwPEnv11 L3";
		static inline const juce::String r4 = " SSG SwPEnv11 R4";
		static inline const juce::String l4 = " SSG SwPEnv11 L4";
		static inline const juce::String r5 = " SSG SwPEnv11 R5";
		static inline const juce::String l5 = " SSG SwPEnv11 L5";
		static inline const juce::String r6 = " SSG SwPEnv11 R6";
		static inline const juce::String l6 = " SSG SwPEnv11 L6";
		static inline const juce::String r7 = " SSG SwPEnv11 R7";
		static inline const juce::String l7 = " SSG SwPEnv11 L7";
		static inline const juce::String r8 = " SSG SwPEnv11 R8";
		static inline const juce::String l8 = " SSG SwPEnv11 L8";
		static inline const juce::String r9 = " SSG SwPEnv11 R9";
		static inline const juce::String l9 = " SSG SwPEnv11 L9";
		static inline const juce::String r10 = " SSG SwPEnv11 R10";
		static inline const juce::String l10 = " SSG SwPEnv11 L10";
		static inline const juce::String r11 = " SSG SwPEnv11 R11";
		static inline const juce::String l11 = " SSG SwPEnv11 L11";
	};

	namespace Opzx7Lfo
	{
		static inline const juce::String pmFreq = "_LFO_PM_FREQ";
		static inline const juce::String amFreq = "_LFO_AM_FREQ";
		static inline const juce::String shape = "_LFO_SHAPE";
		static inline const juce::String pgShape = "_LFO_PG_SHAPE";
		static inline const juce::String egShape = "_LFO_EG_SHAPE";
		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String pm = "_LFO_PM";
		static inline const juce::String vib = "_LFO_VIB";
		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String ams = "_LFO_AMS";
		static inline const juce::String pmd = "_LFO_PMD";
		static inline const juce::String amd = "_LFO_AMD";
		static inline const juce::String amSmoothRatio = "_LFO_ASMRT";
		static inline const juce::String pmSyncDelay = "_LFO_PM_SYNC_DELAY";
		static inline const juce::String amSyncDelay = "_LFO_AM_SYNC_DELAY";
	};

	namespace OplLfo {
		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String vib = "_LFO_VIB";
		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String ams = "_LFO_AMS";
		static inline const juce::String pmd = "_LFO_PMD";
		static inline const juce::String amd = "_LFO_AMD";
	}

	namespace OpmLfo {
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
		static inline const juce::String amsEn = "_AMS_EN";
	}

	namespace OpnaLfo {
		static inline const juce::String freqs = "_LFO_FREQS";
		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String vib = "_LFO_VIB";
		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String ams = "_LFO_AMS";
		static inline const juce::String syncDelay = "_LFO_SYNC_DELAY";
	}

	namespace N88Lfo {
		static inline const juce::String freq = "_LFO_FREQ";
		static inline const juce::String shape = "_LFO_SHAPE";
		static inline const juce::String pgShape = "_LFO_PG_SHAPE";
		static inline const juce::String egShape = "_LFO_EG_SHAPE";
		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String pm = "_LFO_PM";
		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String pmd = "_LFO_PMD";
		static inline const juce::String amd = "_LFO_AMD";
		static inline const juce::String amSmoothRatio = "_LFO_ASMRT";
		static inline const juce::String syncDelay = "_LFO_SYNC_DELAY";
		static inline const juce::String ams = "_N88AMS";
	}

	namespace WtMod
	{
		static inline const juce::String enable = " Mod Enable";
		static inline const juce::String depth = " Mod Depth";
		static inline const juce::String speed = " Mod Speed";
	}

	namespace SsgDuty
	{
		static inline const juce::String mode = " Duty Mode";
		static inline const juce::String preset = " Duty Preset";
		static inline const juce::String var = " Duty Var";
		static inline const juce::String invert = " Duty Invert";
		static inline const juce::String fc = " Duty FC Mode";
		static inline const juce::String fcFluc = " Duty FC Fluctuation";
	}

	namespace SsgTri
	{
		static inline const juce::String keyTrack = " Triangle Key Track";
		static inline const juce::String peak = " Triangle Peak";
		static inline const juce::String manualFreq = " Tri Manual Freq";
	}

	namespace SsgHwEnv
	{
		static inline const juce::String enable = " Hardware Env Enable";
		static inline const juce::String shape = " Hardware Env Shape";
		static inline const juce::String period = " Hardware Env Period";
		static inline const juce::String min = " Hardware Env Min";
		static inline const juce::String max = " Hardware Env Max";
		static inline const juce::String smooth = " Hardware Env Smooth";
	}
}
