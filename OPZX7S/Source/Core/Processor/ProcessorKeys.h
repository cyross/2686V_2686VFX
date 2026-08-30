// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

namespace CPK
{
	static inline const juce::String mode = "MODE";

	namespace Midi
	{
		static inline const juce::String monoMode = "MONO_MODE";
		static inline const juce::String useVelocity = "USE_VELICITY";
		static inline const juce::String pitchResetOnLegato = "PITCH_RESET_LEGATO";
		static inline const juce::String fixedVelocity = "FIXED_VELICITY";
	}

	namespace Wt
	{
		static inline const juce::String wave = "_WAVE";
		static inline const juce::String sampleSize = "_SIZE";
		static inline const juce::String steps = "_STEPS";
	}

	namespace Wt2
	{
		static inline const juce::String wave = "_WAVE";
		static inline const juce::String sampleSize = "_SIZE";
		static inline const juce::String resolution = "_RES";
	}

	static inline const juce::String custom32 = "_CUSTOM32_";
	static inline const juce::String custom64 = "_CUSTOM64_";
	static inline const juce::String custom128 = "_CUSTOM128_";
	static inline const juce::String custom256 = "_CUSTOM256_";

	static inline const juce::String op = "_OP";

	static inline const juce::String pitchAdsr = "_PITCHENV";
	static inline const juce::String adsr = "_ADSR";
	static inline const juce::String ssgSwEnv = "_SSGSW";
	static inline const juce::String ssgSwEnv11 = "_SSGSW11";
	static inline const juce::String ssgSwPEnv11 = "_SSGSWP11";

	static inline const juce::String bypass = "_BYPASS";

	static inline const juce::String level = "_LEVEL";
	static inline const juce::String vol = "_VOL";

	static inline const juce::String fix = "_FIX";
	static inline const juce::String fixFreq = "_FREQ";

	static inline const juce::String ssgWaveform = "_WAVEFORM";

	static inline const juce::String pcmOffset = "_PCM_OFFSET";
	static inline const juce::String pcmRatio = "_PCM_RATIO";

	static inline const juce::String lpEnable = "_LOOPPOINT_EN";
	static inline const juce::String lpStart = "_LOOPPOINT_ST";
	static inline const juce::String lpEnd = "_LOOPPOINT_END";

	static inline const juce::String mul = "_MUL";
	static inline const juce::String mulRatio = "_MUL_RATIO";
	static inline const juce::String dt = "_DT";
	static inline const juce::String dt2 = "_DT2";
	static inline const juce::String dt3 = "_DT3";

	static inline const juce::String pan = "_PAN";
	static inline const juce::String panpot = "_PANPOT";
	static inline const juce::String panpot_en = "_PANPOT_EN";

	static inline const juce::String note = "_NOTE";
	static inline const juce::String oneShot = "_ONESHOT";
	static inline const juce::String loop = "_LOOP";

	namespace Tn
	{
		static inline const juce::String tone = "_TONE";
		static inline const juce::String noise = "_NOISE";
		static inline const juce::String freq = "_NOISEFREQ";
		static inline const juce::String nON = "_NOISE_ON_NOTE";
		static inline const juce::String mix = "_NOISEMIX";
	}

	namespace Fm
	{
		static inline const juce::String alg = "_ALG";
		static inline const juce::String fb = "_FB1";
		static inline const juce::String fb1 = "_FB1";
		static inline const juce::String fb2 = "_FB2";
		static inline const juce::String fb3 = "_FB3";
		static inline const juce::String fb4 = "_FB4";
		static inline const juce::String fb5 = "_FB5";
		static inline const juce::String fb6 = "_FB6";
		static inline const juce::String fb7 = "_FB7";
		static inline const juce::String fb8 = "_FB8";

		static inline const juce::String tl = "_TL";

		static inline const juce::String ar = "_AR";
		static inline const juce::String dr = "_DR";
		static inline const juce::String sl = "_SL";
		static inline const juce::String sr = "_SR";
		static inline const juce::String rr = "_RR";

		static inline const juce::String d1r = "_D1R";
		static inline const juce::String d1l = "_D1L";
		static inline const juce::String d2r = "_D2R";

		static inline const juce::String ksEn = "_KS_EN";
		static inline const juce::String ksMode = "_KS_MODE";
		static inline const juce::String ks = "_KS";
		static inline const juce::String ksr = "_KSR";
		static inline const juce::String ksl = "_KSL";
		static inline const juce::String ksrMA7 = "_KSR";
		static inline const juce::String kslMA7 = "_KSL";
		static inline const juce::String ksrOPZ = "_KSR_OPZ";
		static inline const juce::String kslOPZ = "_KSL_OPZ";
		static inline const juce::String ksrOPP = "_KSR_OPP";
		static inline const juce::String kslOPP = "_KSL_OPP";
		static inline const juce::String ksBp = "_KS_BP";
		static inline const juce::String ksLc = "_KS_LC";
		static inline const juce::String ksRc = "_KS_RC";
		static inline const juce::String ksLd = "_KS_LD";
		static inline const juce::String ksRd = "_KS_RD";
		static inline const juce::String ksRs = "_KS_RS";

		static inline const juce::String se = "_SE";
		static inline const juce::String seFreq = "_SEFREQ";

		static inline const juce::String lfoFreq = "_LFO_FREQ";
		static inline const juce::String freqs = "_LFO_FREQS";

		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String vib = "_LFO_VIB";
		static inline const juce::String pm = "_LFO_PM";
		static inline const juce::String amsEn = "_AMS_EN";

		static inline const juce::String pgShape = "_LFO_PG_SHAPE";
		static inline const juce::String egShape = "_LFO_EG_SHAPE";

		static inline const juce::String pmFreq = "_LFO_PM_FREQ";
		static inline const juce::String amFreq = "_LFO_AM_FREQ";
		static inline const juce::String shape = "_LFO_SHAPE";
		static inline const juce::String amSmoothRatio = "_LFO_ASMRT";

		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String ams = "_LFO_AMS";
		static inline const juce::String pmd = "_LFO_PMD";
		static inline const juce::String amd = "_LFO_AMD";

		static inline const juce::String n88Amd = "_N88AMD";
		static inline const juce::String n88Ams = "_N88AMS";
		static inline const juce::String n88Pmd = "_N88PMD";
		static inline const juce::String n88Pms = "_N88PMS";

		static inline const juce::String syncDelay = "_LFO_SYNC_DELAY";
		static inline const juce::String pmSyncDelay = "_LFO_PM_SYNC_DELAY";
		static inline const juce::String amSyncDelay = "_LFO_AM_SYNC_DELAY";

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

		static inline const juce::String sus = "_SUS";
		static inline const juce::String xof = "_XOF";
		static inline const juce::String kor = "_KOR";
		static inline const juce::String bypass = "_AMP_BYPASS";
	};

	namespace Quality {
		static inline const juce::String bit = "_BIT";
		static inline const juce::String rate = "_RATE";
	}

	namespace QualityPcm {
		static inline const juce::String mode = "_MODE";
		static inline const juce::String rate = "_RATE";
		static inline const juce::String interp = "_INTERP";
	}

	namespace Unison
	{
		static inline const juce::String voices = "_UNI_VOICES";
		static inline const juce::String detune = "_UNI_DETUNE";
		static inline const juce::String spread = "_UNI_SPREAD";
		static inline const juce::String arpEnable = "_UNI_ARP";
		static inline const juce::String arpFreq = "_UNI_ARPFREQ";
		static inline const juce::String arpSmooth = "_UNI_ARPSMTH";
		// ボイス単位の設定 (末尾に 1〜7 のボイス番号が付く)
		static inline const juce::String paraDistance = "_UNI_PDIST";
		static inline const juce::String paraDetune = "_UNI_PDET";
	}

	namespace Adsr
	{
		static inline const juce::String ar = "_AR";
		static inline const juce::String dr = "_DR";
		static inline const juce::String sl = "_SL";
		static inline const juce::String rr = "_RR";
		static inline const juce::String stl = "_STL";
		static inline const juce::String kor = "_KOR";
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

	namespace SsgSwEnv
	{
		static inline const juce::String enable = "_SSGSW_ENABLE";
		static inline const juce::String bypass = "_SSGSW_BYPASS";
		static inline const juce::String steps = "_SSGSW_STEPS";
		static inline const juce::String loop = "_SSGSW_LOOP";
		static inline const juce::String loopTo = "_SSGSW_LOOPTO";
		static inline const juce::String loopCount = "_SSGSW_LOOPCNT";
		static inline const juce::String stl = "_SSGSW_STL";
		static inline const juce::String r1 = "_SSGSW_R1";
		static inline const juce::String l1 = "_SSGSW_L1";
		static inline const juce::String r2 = "_SSGSW_R2";
		static inline const juce::String l2 = "_SSGSW_L2";
		static inline const juce::String r3 = "_SSGSW_R3";
		static inline const juce::String l3 = "_SSGSW_L3";
		static inline const juce::String r4 = "_SSGSW_R4";
		static inline const juce::String l4 = "_SSGSW_L4";
		static inline const juce::String r5 = "_SSGSW_R5";
		static inline const juce::String l5 = "_SSGSW_L5";
		static inline const juce::String r6 = "_SSGSW_R6";
		static inline const juce::String l6 = "_SSGSW_L6";
	};

	namespace SsgSwEnv11
	{
		static inline const juce::String enable = "_SSGSW11_ENABLE";
		static inline const juce::String bypass = "_SSGSW11_BYPASS";
		static inline const juce::String steps = "_SSGSW11_STEPS";
		static inline const juce::String loop = "_SSGSW11_LOOP";
		static inline const juce::String loopTo = "_SSGSW11_LOOPTO";
		static inline const juce::String loopCount = "_SSGSW11_LOOPCNT";
		static inline const juce::String stl = "_SSGSW11_STL";
		static inline const juce::String r1 = "_SSGSW11_R1";
		static inline const juce::String l1 = "_SSGSW11_L1";
		static inline const juce::String r2 = "_SSGSW11_R2";
		static inline const juce::String l2 = "_SSGSW11_L2";
		static inline const juce::String r3 = "_SSGSW11_R3";
		static inline const juce::String l3 = "_SSGSW11_L3";
		static inline const juce::String r4 = "_SSGSW11_R4";
		static inline const juce::String l4 = "_SSGSW11_L4";
		static inline const juce::String r5 = "_SSGSW11_R5";
		static inline const juce::String l5 = "_SSGSW11_L5";
		static inline const juce::String r6 = "_SSGSW11_R6";
		static inline const juce::String l6 = "_SSGSW11_L6";
		static inline const juce::String r7 = "_SSGSW11_R7";
		static inline const juce::String l7 = "_SSGSW11_L7";
		static inline const juce::String r8 = "_SSGSW11_R8";
		static inline const juce::String l8 = "_SSGSW11_L8";
		static inline const juce::String r9 = "_SSGSW11_R9";
		static inline const juce::String l9 = "_SSGSW11_L9";
		static inline const juce::String r10 = "_SSGSW11_R10";
		static inline const juce::String l10 = "_SSGSW11_L10";
		static inline const juce::String r11 = "_SSGSW11_R11";
		static inline const juce::String l11 = "_SSGSW11_L11";
	};

	namespace SsgSwPEnv11
	{
		static inline const juce::String enable = "_SSGSWP11_ENABLE";
		static inline const juce::String bypass = "_SSGSWP11_BYPASS";
		static inline const juce::String steps = "_SSGSWP11_STEPS";
		static inline const juce::String loop = "_SSGSWP11_LOOP";
		static inline const juce::String loopTo = "_SSGSWP11_LOOPTO";
		static inline const juce::String loopCount = "_SSGSWP11_LOOPCNT";
		static inline const juce::String stl = "_SSGSWP11_STL";
		static inline const juce::String r1 = "_SSGSWP11_R1";
		static inline const juce::String l1 = "_SSGSWP11_L1";
		static inline const juce::String r2 = "_SSGSWP11_R2";
		static inline const juce::String l2 = "_SSGSWP11_L2";
		static inline const juce::String r3 = "_SSGSWP11_R3";
		static inline const juce::String l3 = "_SSGSWP11_L3";
		static inline const juce::String r4 = "_SSGSWP11_R4";
		static inline const juce::String l4 = "_SSGSWP11_L4";
		static inline const juce::String r5 = "_SSGSWP11_R5";
		static inline const juce::String l5 = "_SSGSWP11_L5";
		static inline const juce::String r6 = "_SSGSWP11_R6";
		static inline const juce::String l6 = "_SSGSWP11_L6";
		static inline const juce::String r7 = "_SSGSWP11_R7";
		static inline const juce::String l7 = "_SSGSWP11_L7";
		static inline const juce::String r8 = "_SSGSWP11_R8";
		static inline const juce::String l8 = "_SSGSWP11_L8";
		static inline const juce::String r9 = "_SSGSWP11_R9";
		static inline const juce::String l9 = "_SSGSWP11_L9";
		static inline const juce::String r10 = "_SSGSWP11_R10";
		static inline const juce::String l10 = "_SSGSWP11_L10";
		static inline const juce::String r11 = "_SSGSWP11_R11";
		static inline const juce::String l11 = "_SSGSWP11_L11";
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

	namespace WtMod {
		static inline const juce::String enable = "_MOD_ENABLE";
		static inline const juce::String depth = "_MOD_DEPTH";
		static inline const juce::String speed = "_MOD_SPEED";
		static inline const juce::String shape = "_MOD_SHAPE";
		// 変調波形そのものはプロセッサが持つので、パラメータは
		// 「どのスロットを使うか」だけ。
		static inline const juce::String waveSlot = "_MOD_WAVE_SLOT";
		static inline const juce::String fdsTable = "_MOD_FDS_";
		static inline const juce::String waveSmooth = "_MOD_WAVE_SMOOTH";
	};

	namespace SsgDuty
	{
		static inline const juce::String mode = "_DUTY_MODE";
		static inline const juce::String preset = "_DUTY_PRESET";
		static inline const juce::String var = "_DUTY_VAR";
		static inline const juce::String inv = "_DUTY_INV";
		static inline const juce::String fc = "_DUTY_FC_MODE";
		static inline const juce::String fcFluc = "_DUTY_FC_FLUC";
	};

	namespace SsgTri
	{
		static inline const juce::String keyTrk = "_TRI_KEYTRK";
		static inline const juce::String peak = "_TRI_PEAK";
		static inline const juce::String freq = "_TRI_FREQ";
	};

	namespace SsgHwEnv
	{
		static inline const juce::String enable = "_SSGHWENV_ENABLE";
		static inline const juce::String shape = "_SSGHWENV_SHAPE";
		static inline const juce::String period = "_SSGHWENV_PERIOD";
		static inline const juce::String min = "_SSGHWENV_MIN";
		static inline const juce::String max = "_SSGHWENV_MAX";
		static inline const juce::String smooth = "_SSGHWENV_SMOOTH";
	};
};
