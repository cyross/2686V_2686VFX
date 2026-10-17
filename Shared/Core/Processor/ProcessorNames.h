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
		static inline const juce::String interpolate = " Interpolate";
		static inline const juce::String slot = " Wave Slot";
	}

	namespace AdpcmPlus
	{
		static inline const juce::String slot = " PCM Slot";
	}

	namespace Wt2
	{
		static inline const juce::String waveform = " Waveform";
		static inline const juce::String sampleSize = " Sample Size";
		static inline const juce::String resolution = " Resolution";
	}

	static inline const juce::String level = " Level";
	static inline const juce::String speed = " Speed";
	static inline const juce::String delay = " Delay";
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
	static inline const juce::String lpCount = " Loop-point Count";

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
		static inline const juce::String toneValueOn = " Tone Value Enable";
		static inline const juce::String toneValue = " Tone Value";
		static inline const juce::String noiseValueOn = " Noise Value Enable";
		static inline const juce::String noiseValue = " Noise Value";
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
		static inline const juce::String interp = " Interpolation Mode";
	}

	namespace QualityPcm {
		static inline const juce::String bit = " Bit";
		static inline const juce::String rate = " Rate";
		static inline const juce::String interp = " Interpolation Mode";
		static inline const juce::String nrResample = " NR Clean Resample";
		static inline const juce::String nrGate = " NR Gate";
		static inline const juce::String nrGateLevel = " NR Gate Level";
		static inline const juce::String nrLpf = " NR Low-pass";
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
		static inline const juce::String endl = " EndL";
		static inline const juce::String endlEnable = " EndL Enable";
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
		static inline const juce::String endl = " PitchEnv EndL";
		static inline const juce::String endlEnable = " PitchEnv EndL Enable";
		static inline const juce::String keep = " PitchEnv Keep";
	};

	namespace SsgSwEnv256
	{
		static inline const juce::String enable = " SSG SwEnv256 Enable";
		static inline const juce::String bypass = " SSG SwEnv256 Bypass";
		static inline const juce::String steps = " SSG SwEnv256 Steps";
		static inline const juce::String loop = " SSG SwEnv256 Loop";
		static inline const juce::String loopTo = " SSG SwEnv256 Loop To";
		static inline const juce::String loopCount = " SSG SwEnv256 Loop Count";
		static inline const juce::String stl = " SSG SwEnv256 STL";
		static inline const juce::String lr = " SSG SwEnv256 LR";
		static inline const juce::String rr = " SSG SwEnv256 RR";
		static inline const juce::String endl = " SSG SwEnv256 EndL";
		static inline const juce::String endlEnable = " SSG SwEnv256 EndL Enable";
		static inline const juce::String keep = " SSG SwEnv256 Keep";
	};

	namespace SsgSwPEnv256
	{
		static inline const juce::String enable = " SSG SwPEnv256 Enable";
		static inline const juce::String bypass = " SSG SwPEnv256 Bypass";
		static inline const juce::String steps = " SSG SwPEnv256 Steps";
		static inline const juce::String loop = " SSG SwPEnv256 Loop";
		static inline const juce::String loopTo = " SSG SwPEnv256 Loop To";
		static inline const juce::String loopCount = " SSG SwPEnv256 Loop Count";
		static inline const juce::String stl = " SSG SwPEnv256 STL";
		static inline const juce::String lr = " SSG SwPEnv256 LR";
		static inline const juce::String rr = " SSG SwPEnv256 RR";
		static inline const juce::String endl = " SSG SwPEnv256 EndL";
		static inline const juce::String endlEnable = " SSG SwPEnv256 EndL Enable";
		static inline const juce::String keep = " SSG SwPEnv256 Keep";
	};

	namespace Opzx7Lfo
	{
		static inline const juce::String pmFreq = " LFO PM Freq";
		static inline const juce::String amFreq = " LFO AM Freq";
		static inline const juce::String shape = " LFO Shape";
		static inline const juce::String pgShape = " LFO PG Shape";
		static inline const juce::String egShape = " LFO EG Shape";
		static inline const juce::String am = " LFO AM";
		static inline const juce::String pm = " LFO PM";
		static inline const juce::String vib = " LFO Vibrato";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
		static inline const juce::String amSmoothRatio = " LFO AM Smooth Ratio";
		static inline const juce::String pmSyncDelay = " LFO PM Sync Delay";
		static inline const juce::String amSyncDelay = " LFO AM Sync Delay";

		// ホールドと部分再生。尻尾は CPN::WaveHold。
		static inline const juce::String pmHoldPrefix = " LFO PM";
		static inline const juce::String amHoldPrefix = " LFO AM";
	};

	namespace OplLfo {
		static inline const juce::String am = " LFO AM";
		static inline const juce::String vib = " LFO Vibrato";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
	}

	namespace OpmLfo {
		static inline const juce::String freq = " LFO Freq";
		static inline const juce::String shape = " LFO Shape";
		static inline const juce::String pgShape = " LFO PG Shape";
		static inline const juce::String egShape = " LFO EG Shape";
		static inline const juce::String am = " LFO AM";
		static inline const juce::String pm = " LFO PM";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
		static inline const juce::String amSmoothRatio = " LFO AM Smooth Ratio";
		static inline const juce::String syncDelay = " LFO Sync Delay";
		static inline const juce::String amsEn = " AMS Enable";
	}

	namespace OpnaLfo {
		static inline const juce::String freqs = " LFO Freqs";
		static inline const juce::String am = " LFO AM";
		static inline const juce::String vib = " LFO Vibrato";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String syncDelay = " LFO Sync Delay";
	}

	namespace N88Lfo {
		static inline const juce::String freq = " LFO Freq";
		static inline const juce::String shape = " LFO Shape";
		static inline const juce::String pgShape = " LFO PG Shape";
		static inline const juce::String egShape = " LFO EG Shape";
		static inline const juce::String am = " LFO AM";
		static inline const juce::String pm = " LFO PM";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
		static inline const juce::String amSmoothRatio = " LFO AM Smooth Ratio";
		static inline const juce::String syncDelay = " LFO Sync Delay";
		static inline const juce::String ams = " N88 AMS";
	}

	// ホールドと部分再生。holdPrefix と組み合わせて使う。
	namespace WaveHold {
		static inline const juce::String holdEnable = " Hold Enable";
		static inline const juce::String holdCount = " Hold Count";
		static inline const juce::String holdTarget = " Hold Target";
		static inline const juce::String holdMin = " Hold Min";
		static inline const juce::String holdMax = " Hold Max";
		static inline const juce::String keepEnable = " Keep Enable";
		static inline const juce::String waveStart = " Wave Start";
		static inline const juce::String keepStart = " Keep Start";
		static inline const juce::String waveEnd = " Wave End";
		static inline const juce::String keepEnd = " Keep End";
	}
	namespace WtMod
	{
		static inline const juce::String enable = " Mod Enable";
		static inline const juce::String depth = " Mod Depth";
		static inline const juce::String speed = " Mod Speed";
		static inline const juce::String shape = " Mod Shape";
		static inline const juce::String waveSlot = " Mod Wave Slot";
		static inline const juce::String fdsTable = " Mod FDS Table ";
		static inline const juce::String waveSmooth = " Mod Wave Smooth";
		// ホールドと部分再生。尻尾は CPN::WaveHold が持つ。
		static inline const juce::String holdPrefix = " Mod";
	}

	namespace WtAmpMod
	{
		static inline const juce::String enable = " Amp Mod Enable";
		static inline const juce::String depth = " Amp Mod Depth";
		static inline const juce::String speed = " Amp Mod Speed";
		static inline const juce::String shape = " Amp Mod Shape";
		static inline const juce::String waveSlot = " Amp Mod Wave Slot";
		static inline const juce::String fdsTable = " Amp Mod FDS Table ";
		static inline const juce::String waveSmooth = " Amp Mod Wave Smooth";
		static inline const juce::String min = " Amp Mod Min";
		static inline const juce::String max = " Amp Mod Max";
		// ホールドと部分再生。尻尾は CPN::WaveHold が持つ。
		static inline const juce::String holdPrefix = " Amp Mod";
	}

	namespace SsgDuty
	{
		static inline const juce::String mode = " Duty Mode";
		static inline const juce::String preset = " Duty Preset";
		static inline const juce::String var = " Duty Var";
		static inline const juce::String invert = " Duty Invert";
		static inline const juce::String fc = " Duty FC Mode";
		static inline const juce::String fcFluc = " Duty FC Fluctuation";
		static inline const juce::String moreFc = " Duty More Real FC";
		static inline const juce::String moreNes = " Duty More Real NES";
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
		static inline const juce::String valueOn = " Hardware Env Value Enable";
		static inline const juce::String value = " Hardware Env Value";

		// ホールドと部分再生。尻尾は CPN::WaveHold が持つ。
		static inline const juce::String holdPrefix = " Hardware Env";
	}

	namespace SsgHwPEnv
	{
		static inline const juce::String enable = " Hardware Pitch Env Enable";
		static inline const juce::String shape = " Hardware Pitch Env Shape";
		static inline const juce::String period = " Hardware Pitch Env Period";
		static inline const juce::String min = " Hardware Pitch Env Min";
		static inline const juce::String max = " Hardware Pitch Env Max";
		static inline const juce::String smooth = " Hardware Pitch Env Smooth";
		static inline const juce::String valueOn = " Hardware Pitch Env Value Enable";
		static inline const juce::String value = " Hardware Pitch Env Value";

		// ホールドと部分再生。尻尾は CPN::WaveHold が持つ。
		static inline const juce::String holdPrefix = " Hardware Pitch Env";
	}
}
