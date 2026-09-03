#pragma once

#include "../../Generator/Fds/GenFdsModTable.h"
#include <JuceHeader.h>
#include <array>

#include "./ProcessorStructs.h"
#include "./ProcessorKeys.h"
#include "./ProcessorNames.h"
#include "./ProcessorValues.h"

#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/OplAdsr/EnvOplAdsrParams.h"
#include "../../Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddrParams.h"
#include "../../Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssrParams.h"
#include "../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Detune/Opl/DetuneOplParams.h"
#include "../../Effect/Detune/Opm/DetuneOpmParams.h"
#include "../../Effect/Detune/Opn/DetuneOpnParams.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../Effect/Lfo/Opl/LfoOplParams.h"
#include "../../Effect/Lfo/Opm/LfoOpmParams.h"
#include "../../Effect/Lfo/N88/LfoN88Params.h"
#include "../../Effect/Lfo/Opna/LfoOpnaParams.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Generator/Fm/Fix/FmFixParams.h"
#include "../../Synth/Adpcm/SynthAdpcmParams.h"
#include "../../Synth/Beep/SynthBeepParams.h"
#include "../../Synth/Opl/SynthOplParams.h"
#include "../../Synth/Opl3/SynthOpl3Params.h"
#include "../../Synth/Opm/SynthOpmParams.h"
#include "../../Synth/Opn/SynthOpnParams.h"
#include "../../Synth/Opna/SynthOpnaParams.h"
#include "../../Synth/Opzx7/SynthOpzx7Params.h"
#include "../../Synth/Rhythm/SynthRhythmParams.h"
#include "../../Synth/Ssg/SynthSsgParams.h"
#include "../../Synth/Wavetable/SynthWtParams.h"
#include "../../Synth/Wt2/SynthWt2Params.h"

namespace PrHelper {
	static inline bool getBool(std::atomic<float>* ptr){
		return ptr->load(std::memory_order_relaxed) > CPV::boolThread;
	}

	static inline int getInt(std::atomic<float>* ptr){
		return (int)(ptr->load(std::memory_order_relaxed));
	}

	static inline float getFloat(std::atomic<float>* ptr){
		return ptr->load(std::memory_order_relaxed);
	}

	static inline bool floatToBool(float value){
		return value > CPV::boolThread;
	}

	static inline int floatToBoolToInt(float value){
		return (value > CPV::boolThread) ? 1 : 0;
	}

	static inline std::atomic<float>* setupLevelPtr(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix){
		return apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline std::atomic<float>* setupOpMaskPtr(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix){
		return apvts.getRawParameterValue(prefix + CPK::Fm::mask);
	}

	static inline std::atomic<float>* setupOpOpmAmsEnPtr(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix){
		return apvts.getRawParameterValue(prefix + CPK::OpmLfo::amsEn);
	}

	static inline std::atomic<float>* setupOpN88AmsPtr(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix){
		return apvts.getRawParameterValue(prefix + CPK::N88Lfo::ams);
	}

	static inline void setupWtCustom32(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, std::array<std::atomic<float>*, 32>& pCustom){
		juce::String p = prefix + CPK::custom32;

		for (int i = 0; i < 32; ++i)
		{
			pCustom[i] = apvts.getRawParameterValue(p + juce::String(i));
		}
	}

	static inline void setupWtCustom64(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, std::array<std::atomic<float>*, 64>& pCustom){
		juce::String p = prefix + CPK::custom64;

		for (int i = 0; i < 64; ++i)
		{
			pCustom[i] = apvts.getRawParameterValue(p + juce::String(i));
		}
	}

	static inline void setupWtCustom128(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, std::array<std::atomic<float>*, 128>& pCustom){
		juce::String p = prefix + CPK::custom128;

		for (int i = 0; i < 128; ++i)
		{
			pCustom[i] = apvts.getRawParameterValue(p + juce::String(i));
		}
	}

	static inline void setupWtCustom256(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, std::array<std::atomic<float>*, 256>& pCustom){
		juce::String p = prefix + CPK::custom256;

		for (int i = 0; i < 256; ++i)
		{
			pCustom[i] = apvts.getRawParameterValue(p + juce::String(i));
		}
	}

	static inline void setupQualityPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsQuality& ptPtrs){
		ptPtrs.depth = apvts.getRawParameterValue(prefix + CPK::Quality::bit);
		ptPtrs.rate = apvts.getRawParameterValue(prefix + CPK::Quality::rate);
	}

	static inline void setupQualityPcmPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsQualityPcm& ptPtrs){
		ptPtrs.mode = apvts.getRawParameterValue(prefix + CPK::QualityPcm::mode);
		ptPtrs.rate = apvts.getRawParameterValue(prefix + CPK::QualityPcm::rate);
		ptPtrs.interp = apvts.getRawParameterValue(prefix + CPK::QualityPcm::interp);
	}

	static inline void setupAlgFbPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsAlgFb& ptPtrs){
		ptPtrs.alg = apvts.getRawParameterValue(prefix + CPK::Fm::alg);
		ptPtrs.fb = apvts.getRawParameterValue(prefix + CPK::Fm::fb);
	}

	static inline void setupOpzx7AlgFbPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpzx7AlgFb& ptPtrs) {
		ptPtrs.alg = apvts.getRawParameterValue(prefix + CPK::Fm::alg);
		ptPtrs.fb1 = apvts.getRawParameterValue(prefix + CPK::Fm::fb1);
		ptPtrs.fb2 = apvts.getRawParameterValue(prefix + CPK::Fm::fb2);
		ptPtrs.fb3 = apvts.getRawParameterValue(prefix + CPK::Fm::fb3);
		ptPtrs.fb4 = apvts.getRawParameterValue(prefix + CPK::Fm::fb4);
		ptPtrs.fb5 = apvts.getRawParameterValue(prefix + CPK::Fm::fb5);
		ptPtrs.fb6 = apvts.getRawParameterValue(prefix + CPK::Fm::fb6);
		ptPtrs.fb7 = apvts.getRawParameterValue(prefix + CPK::Fm::fb7);
		ptPtrs.fb8 = apvts.getRawParameterValue(prefix + CPK::Fm::fb8);
	}

	static inline void setupAdsrAmpEnvPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsAdsrAmpEnv& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::adsr + CPK::bypass);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::Adsr::stl);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::Adsr::ar);
		ptPtrs.dr = apvts.getRawParameterValue(prefix + CPK::Adsr::dr);
		ptPtrs.sl = apvts.getRawParameterValue(prefix + CPK::Adsr::sl);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::Adsr::rr);
		ptPtrs.kor = apvts.getRawParameterValue(prefix + CPK::Adsr::kor);
	}

	static inline void setupSsgSwEnvPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgSwEnv& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::ssgSwEnv + CPK::bypass);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::steps);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::loop);
		ptPtrs.loopTo = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::loopTo);
		ptPtrs.loopCount = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::loopCount);
		ptPtrs.r1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r1);
		ptPtrs.r2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r2);
		ptPtrs.r3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r3);
		ptPtrs.r4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r4);
		ptPtrs.r5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r5);
		ptPtrs.r6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r6);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::stl);
		ptPtrs.l1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l1);
		ptPtrs.l2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l2);
		ptPtrs.l3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l3);
		ptPtrs.l4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l4);
		ptPtrs.l5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l5);
		ptPtrs.l6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l6);
	}

	static inline void setupSsgSwEnvPtrsOp(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgSwEnvOp& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::enable);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::steps);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::loop);
		ptPtrs.loopTo = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::loopTo);
		ptPtrs.loopCount = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::loopCount);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::stl);
		ptPtrs.r1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r1);
		ptPtrs.l1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l1);
		ptPtrs.r2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r2);
		ptPtrs.l2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l2);
		ptPtrs.r3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r3);
		ptPtrs.l3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l3);
		ptPtrs.r4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r4);
		ptPtrs.l4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l4);
		ptPtrs.r5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r5);
		ptPtrs.l5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l5);
		ptPtrs.r6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::r6);
		ptPtrs.l6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv::l6);
	}

	static inline void setupSsgSwEnv11Ptrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgSwEnv11& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::bypass);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::steps);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::loop);
		ptPtrs.loopTo = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::loopTo);
		ptPtrs.loopCount = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::loopCount);
		ptPtrs.r1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r1);
		ptPtrs.r2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r2);
		ptPtrs.r3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r3);
		ptPtrs.r4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r4);
		ptPtrs.r5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r5);
		ptPtrs.r6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r6);
		ptPtrs.r7 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r7);
		ptPtrs.r8 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r8);
		ptPtrs.r9 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r9);
		ptPtrs.r10 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r10);
		ptPtrs.r11 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r11);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::stl);
		ptPtrs.l1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l1);
		ptPtrs.l2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l2);
		ptPtrs.l3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l3);
		ptPtrs.l4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l4);
		ptPtrs.l5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l5);
		ptPtrs.l6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l6);
		ptPtrs.l7 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l7);
		ptPtrs.l8 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l8);
		ptPtrs.l9 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l9);
		ptPtrs.l10 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l10);
		ptPtrs.l11 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l11);
	}

	static inline void setupSsgSwEnv11PtrsOp(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgSwEnv11Op& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::enable);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::steps);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::loop);
		ptPtrs.loopTo = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::loopTo);
		ptPtrs.loopCount = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::loopCount);
		ptPtrs.r1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r1);
		ptPtrs.r2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r2);
		ptPtrs.r3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r3);
		ptPtrs.r4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r4);
		ptPtrs.r5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r5);
		ptPtrs.r6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r6);
		ptPtrs.r7 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r7);
		ptPtrs.r8 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r8);
		ptPtrs.r9 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r9);
		ptPtrs.r10 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r10);
		ptPtrs.r11 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::r11);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::stl);
		ptPtrs.l1 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l1);
		ptPtrs.l2 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l2);
		ptPtrs.l3 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l3);
		ptPtrs.l4 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l4);
		ptPtrs.l5 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l5);
		ptPtrs.l6 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l6);
		ptPtrs.l7 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l7);
		ptPtrs.l8 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l8);
		ptPtrs.l9 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l9);
		ptPtrs.l10 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l10);
		ptPtrs.l11 = apvts.getRawParameterValue(prefix + CPK::SsgSwEnv11::l11);
	}

	static inline void setupPitchEnvPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsPitchEnv& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::pitchAdsr + CPK::bypass);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::ar);
		ptPtrs.dr = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::dr);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::rr);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::stl);
		ptPtrs.atl = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::atl);
		ptPtrs.ssl = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::ssl);
		ptPtrs.rll = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::rll);
	}

	static inline void setupPitchEnvPtrsOp(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsPitchEnvOp& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::enable);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::ar);
		ptPtrs.dr = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::dr);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::rr);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::stl);
		ptPtrs.atl = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::atl);
		ptPtrs.ssl = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::ssl);
		ptPtrs.rll = apvts.getRawParameterValue(prefix + CPK::PitchAdsr::rll);
	}

	static inline void setupSsgSwPEnv11Ptrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgSwPEnv11& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::bypass);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::steps);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::loop);
		ptPtrs.loopTo = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::loopTo);
		ptPtrs.loopCount = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::loopCount);
		ptPtrs.r1 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r1);
		ptPtrs.r2 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r2);
		ptPtrs.r3 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r3);
		ptPtrs.r4 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r4);
		ptPtrs.r5 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r5);
		ptPtrs.r6 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r6);
		ptPtrs.r7 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r7);
		ptPtrs.r8 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r8);
		ptPtrs.r9 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r9);
		ptPtrs.r10 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r10);
		ptPtrs.r11 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r11);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::stl);
		ptPtrs.l1 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l1);
		ptPtrs.l2 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l2);
		ptPtrs.l3 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l3);
		ptPtrs.l4 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l4);
		ptPtrs.l5 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l5);
		ptPtrs.l6 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l6);
		ptPtrs.l7 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l7);
		ptPtrs.l8 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l8);
		ptPtrs.l9 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l9);
		ptPtrs.l10 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l10);
		ptPtrs.l11 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l11);
	}

	static inline void setupSsgSwPEnv11PtrsOp(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgSwPEnv11Op& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::enable);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::steps);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::loop);
		ptPtrs.loopTo = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::loopTo);
		ptPtrs.loopCount = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::loopCount);
		ptPtrs.r1 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r1);
		ptPtrs.r2 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r2);
		ptPtrs.r3 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r3);
		ptPtrs.r4 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r4);
		ptPtrs.r5 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r5);
		ptPtrs.r6 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r6);
		ptPtrs.r7 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r7);
		ptPtrs.r8 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r8);
		ptPtrs.r9 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r9);
		ptPtrs.r10 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r10);
		ptPtrs.r11 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::r11);
		ptPtrs.stl = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::stl);
		ptPtrs.l1 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l1);
		ptPtrs.l2 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l2);
		ptPtrs.l3 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l3);
		ptPtrs.l4 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l4);
		ptPtrs.l5 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l5);
		ptPtrs.l6 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l6);
		ptPtrs.l7 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l7);
		ptPtrs.l8 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l8);
		ptPtrs.l9 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l9);
		ptPtrs.l10 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l10);
		ptPtrs.l11 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l1);
	}

	static inline void setupOplDetunePtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOplDetune& ptPtrs){
		ptPtrs.mutiple = apvts.getRawParameterValue(prefix + CPK::mul);
	}

	static inline void setupOpmDetunePtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpmDetune& ptPtrs){
		ptPtrs.multiple = apvts.getRawParameterValue(prefix + CPK::mul);
		ptPtrs.mulRatio = apvts.getRawParameterValue(prefix + CPK::mulRatio);
		ptPtrs.detune = apvts.getRawParameterValue(prefix + CPK::dt);
		ptPtrs.detune2 = apvts.getRawParameterValue(prefix + CPK::dt2);
	}

	static inline void setupOpnDetunePtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpnDetune& ptPtrs){
		ptPtrs.multiple = apvts.getRawParameterValue(prefix + CPK::mul);
		ptPtrs.detune = apvts.getRawParameterValue(prefix + CPK::dt);
	}

	static inline void setupOpzx7DetunePtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpzx7Detune& ptPtrs){
		ptPtrs.multiple = apvts.getRawParameterValue(prefix + CPK::mul);
		ptPtrs.mulRatio = apvts.getRawParameterValue(prefix + CPK::mulRatio);
		ptPtrs.detune = apvts.getRawParameterValue(prefix + CPK::dt);
		ptPtrs.detune2 = apvts.getRawParameterValue(prefix + CPK::dt2);
		ptPtrs.detune3 = apvts.getRawParameterValue(prefix + CPK::dt3);
	}

	static inline void setupOplLfoPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOplLfo& ptPtrs){
		ptPtrs.pmEnable = apvts.getRawParameterValue(prefix + CPK::OplLfo::vib);
		ptPtrs.pms = apvts.getRawParameterValue(prefix + CPK::OplLfo::pms);
		ptPtrs.pmd = apvts.getRawParameterValue(prefix + CPK::OplLfo::pmd);
		ptPtrs.amEnable = apvts.getRawParameterValue(prefix + CPK::OplLfo::am);
		ptPtrs.ams = apvts.getRawParameterValue(prefix + CPK::OplLfo::ams);
		ptPtrs.amd = apvts.getRawParameterValue(prefix + CPK::OplLfo::amd);
	}

	static inline void setupOpmLfoPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpmLfo& ptPtrs){
		ptPtrs.freq = apvts.getRawParameterValue(prefix + CPK::OpmLfo::freq);
		ptPtrs.syncDelay = apvts.getRawParameterValue(prefix + CPK::OpmLfo::syncDelay);
		ptPtrs.pmWave = apvts.getRawParameterValue(prefix + CPK::OpmLfo::pgShape);
		ptPtrs.amWave = apvts.getRawParameterValue(prefix + CPK::OpmLfo::egShape);
		ptPtrs.amSmRt = apvts.getRawParameterValue(prefix + CPK::OpmLfo::amSmoothRatio);
		ptPtrs.amEnable = apvts.getRawParameterValue(prefix + CPK::OpmLfo::am);
		ptPtrs.pms = apvts.getRawParameterValue(prefix + CPK::OpmLfo::pms);
		ptPtrs.pmd = apvts.getRawParameterValue(prefix + CPK::OpmLfo::pmd);
		ptPtrs.pmEnable = apvts.getRawParameterValue(prefix + CPK::OpmLfo::pm);
		ptPtrs.ams = apvts.getRawParameterValue(prefix + CPK::OpmLfo::ams);
		ptPtrs.amd = apvts.getRawParameterValue(prefix + CPK::OpmLfo::amd);
	}

	static inline void setupOpnaLfoPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpnaLfo& ptPtrs){
		ptPtrs.freqs = apvts.getRawParameterValue(prefix + CPK::OpnaLfo::freqs);
		ptPtrs.syncDelay = apvts.getRawParameterValue(prefix + CPK::OpnaLfo::syncDelay);
		ptPtrs.pmEnable = apvts.getRawParameterValue(prefix + CPK::OpnaLfo::vib);
		ptPtrs.pms = apvts.getRawParameterValue(prefix + CPK::OpnaLfo::pms);
		ptPtrs.amEnable = apvts.getRawParameterValue(prefix + CPK::OpnaLfo::am);
		ptPtrs.ams = apvts.getRawParameterValue(prefix + CPK::OpnaLfo::ams);
	}

	static inline void setupOpzx7LfoPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpzx7Lfo& ptPtrs){
		ptPtrs.pmSyncDelay = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::pmSyncDelay);
		ptPtrs.amSyncDelay = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::amSyncDelay);
		ptPtrs.amSmoothRatio = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::amSmoothRatio);
		ptPtrs.pmFreq = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::pmFreq);
		ptPtrs.amFreq = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::amFreq);
		ptPtrs.pmShape = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::pgShape);
		ptPtrs.amShape = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::egShape);
		ptPtrs.pm = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::pm);
		ptPtrs.am = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::am);
		ptPtrs.pmd = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::pmd);
		ptPtrs.pms = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::pms);
		ptPtrs.amd = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::amd);
		ptPtrs.ams = apvts.getRawParameterValue(prefix + CPK::Opzx7Lfo::ams);
	}

	static inline void setupN88LfoPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsN88Lfo& ptPtrs){
		ptPtrs.freq = apvts.getRawParameterValue(prefix + CPK::N88Lfo::freq);
		ptPtrs.syncDelay = apvts.getRawParameterValue(prefix + CPK::N88Lfo::syncDelay);
		ptPtrs.shape = apvts.getRawParameterValue(prefix + CPK::N88Lfo::shape);
		ptPtrs.pmEnable = apvts.getRawParameterValue(prefix + CPK::N88Lfo::pm);
		ptPtrs.pmd = apvts.getRawParameterValue(prefix + CPK::N88Lfo::pmd);
		ptPtrs.pms = apvts.getRawParameterValue(prefix + CPK::N88Lfo::pms);
		ptPtrs.amEnable = apvts.getRawParameterValue(prefix + CPK::N88Lfo::am);
		ptPtrs.amSmRt = apvts.getRawParameterValue(prefix + CPK::N88Lfo::amSmoothRatio);
		ptPtrs.amd = apvts.getRawParameterValue(prefix + CPK::N88Lfo::amd);
	}

	static inline void setupFixPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsFix& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::fix);
		ptPtrs.freq = apvts.getRawParameterValue(prefix + CPK::fixFreq);
	}

	static inline void setupUnisonPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsUnison& ptPtrs){
		ptPtrs.voices = apvts.getRawParameterValue(prefix + CPK::Unison::voices);
		ptPtrs.detuneCents = apvts.getRawParameterValue(prefix + CPK::Unison::detune);
		ptPtrs.spread = apvts.getRawParameterValue(prefix + CPK::Unison::spread);
		ptPtrs.arpEnable = apvts.getRawParameterValue(prefix + CPK::Unison::arpEnable);
		ptPtrs.arpFreq = apvts.getRawParameterValue(prefix + CPK::Unison::arpFreq);
		ptPtrs.arpSmooth = apvts.getRawParameterValue(prefix + CPK::Unison::arpSmooth);

		// ボイス単位の設定 (キー末尾は 1〜7 のボイス番号)
		for (int i = 0; i < Global::unisonParaVoices; ++i) {
			const juce::String no = juce::String(i + 1);
			ptPtrs.paraDistance[i] = apvts.getRawParameterValue(prefix + CPK::Unison::paraDistance + no);
			ptPtrs.paraDetune[i] = apvts.getRawParameterValue(prefix + CPK::Unison::paraDetune + no);
		}
	}

	static inline void setupToneNoise(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsToneNoise& ptPtrs){
		ptPtrs.tone = apvts.getRawParameterValue(prefix + CPK::Tn::tone);
		ptPtrs.noise = apvts.getRawParameterValue(prefix + CPK::Tn::noise);
		ptPtrs.noiseFreq = apvts.getRawParameterValue(prefix + CPK::Tn::freq);
		ptPtrs.noiseOnNote = apvts.getRawParameterValue(prefix + CPK::Tn::nON);
		ptPtrs.mix = apvts.getRawParameterValue(prefix + CPK::Tn::mix);
	}

	static inline void setupPcm(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsPcm& ptPtrs){
		ptPtrs.offset = apvts.getRawParameterValue(prefix + CPK::pcmOffset);
		ptPtrs.ratio = apvts.getRawParameterValue(prefix + CPK::pcmRatio);
	}

	static inline void setupLp(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsLp& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::lpEnable);
		ptPtrs.start = apvts.getRawParameterValue(prefix + CPK::lpStart);
		ptPtrs.end = apvts.getRawParameterValue(prefix + CPK::lpEnd);
	}

	static inline void setupWtMod(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWtMod& ptPtrs, WtModWaveStore& store){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::WtMod::enable);
		ptPtrs.depth = apvts.getRawParameterValue(prefix + CPK::WtMod::depth);
		ptPtrs.speed = apvts.getRawParameterValue(prefix + CPK::WtMod::speed);
		ptPtrs.shape = apvts.getRawParameterValue(prefix + CPK::WtMod::shape);
		ptPtrs.waveSlot = apvts.getRawParameterValue(prefix + CPK::WtMod::waveSlot);

		// このチャンネルの持ち分を押さえる。無ければここで作られる。
		ptPtrs.slots = &store[prefix];

		for (int i = 0; i < CPV::WtMod::FdsTable::size; ++i) {
			ptPtrs.fdsTable[i] = apvts.getRawParameterValue(prefix + CPK::WtMod::fdsTable + juce::String(i));
		}
	}

	// 変調波形はピッチ版と別に持てるよう、置き場所の鍵に尻尾を付ける。
	static inline void setupWtAmpMod(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWtAmpMod& ptPtrs, WtModWaveStore& store){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::enable);
		ptPtrs.depth = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::depth);
		ptPtrs.speed = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::speed);
		ptPtrs.shape = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::shape);
		ptPtrs.waveSlot = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::waveSlot);
		ptPtrs.min = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::min);
		ptPtrs.max = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::max);

		ptPtrs.slots = &store[prefix + CPK::WtAmpMod::waveStoreSuffix];

		for (int i = 0; i < CPV::WtAmpMod::FdsTable::size; ++i) {
			ptPtrs.fdsTable[i] = apvts.getRawParameterValue(prefix + CPK::WtAmpMod::fdsTable + juce::String(i));
		}
	}

	static inline void setupSsgDuty(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgDuty& ptPtrs){
		ptPtrs.mode = apvts.getRawParameterValue(prefix + CPK::SsgDuty::mode);
		ptPtrs.preset = apvts.getRawParameterValue(prefix + CPK::SsgDuty::preset);
		ptPtrs.var = apvts.getRawParameterValue(prefix + CPK::SsgDuty::var);
		ptPtrs.invert = apvts.getRawParameterValue(prefix + CPK::SsgDuty::inv);
		ptPtrs.fc = apvts.getRawParameterValue(prefix + CPK::SsgDuty::fc);
		ptPtrs.fcFluc = apvts.getRawParameterValue(prefix + CPK::SsgDuty::fcFluc);
	}

	static inline void setupSsgTri(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgTri& ptPtrs){
		ptPtrs.keyTrk = apvts.getRawParameterValue(prefix + CPK::SsgTri::keyTrk);
		ptPtrs.peak = apvts.getRawParameterValue(prefix + CPK::SsgTri::peak);
		ptPtrs.freq = apvts.getRawParameterValue(prefix + CPK::SsgTri::freq);
	}

	static inline void setupSsgHwEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgHwEnv& ptPtrs) {
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::SsgHwEnv::enable);
		ptPtrs.shape = apvts.getRawParameterValue(prefix + CPK::SsgHwEnv::shape);
		ptPtrs.period = apvts.getRawParameterValue(prefix + CPK::SsgHwEnv::period);
		ptPtrs.min = apvts.getRawParameterValue(prefix + CPK::SsgHwEnv::min);
		ptPtrs.max = apvts.getRawParameterValue(prefix + CPK::SsgHwEnv::max);
		ptPtrs.smooth = apvts.getRawParameterValue(prefix + CPK::SsgHwEnv::smooth);
	}

	static inline void setupSsgHwPEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgHwPEnv& ptPtrs) {
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::SsgHwPEnv::enable);
		ptPtrs.shape = apvts.getRawParameterValue(prefix + CPK::SsgHwPEnv::shape);
		ptPtrs.period = apvts.getRawParameterValue(prefix + CPK::SsgHwPEnv::period);
		ptPtrs.min = apvts.getRawParameterValue(prefix + CPK::SsgHwPEnv::min);
		ptPtrs.max = apvts.getRawParameterValue(prefix + CPK::SsgHwPEnv::max);
		ptPtrs.smooth = apvts.getRawParameterValue(prefix + CPK::SsgHwPEnv::smooth);
	}

	static inline void setupPanpot(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsPanpot& ptPtrs){
		ptPtrs.enable = apvts.getRawParameterValue(prefix + CPK::panpot_en);
		ptPtrs.panpot = apvts.getRawParameterValue(prefix + CPK::panpot);
	}

	static inline void setupWtCustom(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWtCustomWave& ptPtrs){
		PrHelper::setupWtCustom32(apvts, prefix, ptPtrs.c32);
		PrHelper::setupWtCustom64(apvts, prefix, ptPtrs.c64);
		PrHelper::setupWtCustom128(apvts, prefix, ptPtrs.c128);
		PrHelper::setupWtCustom256(apvts, prefix, ptPtrs.c256);
	}

	static inline void setupAdpcmBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsAdpcmBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.pan = apvts.getRawParameterValue(prefix + CPK::pan);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::loop);
	}

	static inline void setupBeepBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsBeepBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline void setupOplBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOplBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline void setupOpl3BasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpl3Basic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline void setupOpmBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpmBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.pan = apvts.getRawParameterValue(prefix + CPK::pan);
	}

	static inline void setupOpnBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpnBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline void setupOpnaBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpnaBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.pan = apvts.getRawParameterValue(prefix + CPK::pan);
	}

	static inline void setupOpzx7BasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpzx7Basic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline void setupRhythmBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsRhythmBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
	}

	static inline void setupRhythmPadBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsRhythmPadBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::vol);
		ptPtrs.pan = apvts.getRawParameterValue(prefix + CPK::pan);
		ptPtrs.noteNumber = apvts.getRawParameterValue(prefix + CPK::note);
		ptPtrs.isOneShot = apvts.getRawParameterValue(prefix + CPK::oneShot);
	}

	static inline void setupSsgBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.waveform = apvts.getRawParameterValue(prefix + CPK::ssgWaveform);
	}

	static inline void setupWtBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWtBasic& ptPtrs) {
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.wave = apvts.getRawParameterValue(prefix + CPK::Wt::wave);
		ptPtrs.sampleSize = apvts.getRawParameterValue(prefix + CPK::Wt::sampleSize);
		ptPtrs.step = apvts.getRawParameterValue(prefix + CPK::Wt::steps);
		ptPtrs.interpolate = apvts.getRawParameterValue(prefix + CPK::Wt::interpolate);
	}

	static inline void setupWtPlusBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWtPlusBasic& ptPtrs) {
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.slot = apvts.getRawParameterValue(prefix + CPK::Wt::slot);
		ptPtrs.steps = apvts.getRawParameterValue(prefix + CPK::Wt::steps);
		ptPtrs.interpolate = apvts.getRawParameterValue(prefix + CPK::Wt::interpolate);
	}

	static inline void setupWt2BasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWt2Basic& ptPtrs) {
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.wave = apvts.getRawParameterValue(prefix + CPK::Wt2::wave);
		ptPtrs.sampleSize = apvts.getRawParameterValue(prefix + CPK::Wt2::sampleSize);
		ptPtrs.resolution = apvts.getRawParameterValue(prefix + CPK::Wt2::resolution);
		ptPtrs.interpolate = apvts.getRawParameterValue(prefix + CPK::Wt::interpolate);
	}

	static inline void setupOplAdsrPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOplAdsr& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::Fm::bypass);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::Fm::rgAr);
		ptPtrs.dr = apvts.getRawParameterValue(prefix + CPK::Fm::rgDr);
		ptPtrs.sl = apvts.getRawParameterValue(prefix + CPK::Fm::rgSl);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::Fm::rgRr);
		ptPtrs.tl = apvts.getRawParameterValue(prefix + CPK::Fm::rgTl);

		ptPtrs.ksr = apvts.getRawParameterValue(prefix + CPK::Fm::ksr);
		ptPtrs.ksl = apvts.getRawParameterValue(prefix + CPK::Fm::ksl);

		ptPtrs.egType = apvts.getRawParameterValue(prefix + CPK::Fm::egType);
		ptPtrs.sus = apvts.getRawParameterValue(prefix + CPK::Fm::sus);
		ptPtrs.xof = apvts.getRawParameterValue(prefix + CPK::Fm::xof);
		ptPtrs.kor = apvts.getRawParameterValue(prefix + CPK::Fm::kor);
	}

	static inline void setupOpl3AdsrPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpl3Adsr& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::Fm::bypass);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::Fm::rgAr);
		ptPtrs.dr = apvts.getRawParameterValue(prefix + CPK::Fm::rgDr);
		ptPtrs.sl = apvts.getRawParameterValue(prefix + CPK::Fm::rgSl);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::Fm::rgRr);
		ptPtrs.tl = apvts.getRawParameterValue(prefix + CPK::Fm::rgTl);

		ptPtrs.ksr = apvts.getRawParameterValue(prefix + CPK::Fm::ksr);
		ptPtrs.ksl = apvts.getRawParameterValue(prefix + CPK::Fm::ksl);

		ptPtrs.egType = apvts.getRawParameterValue(prefix + CPK::Fm::egType);
		ptPtrs.xof = apvts.getRawParameterValue(prefix + CPK::Fm::xof);
		ptPtrs.kor = apvts.getRawParameterValue(prefix + CPK::Fm::kor);
	}

	static inline void setupOpmAdsrPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpmAdsr& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::Fm::bypass);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::Fm::rgAr);
		ptPtrs.d1r = apvts.getRawParameterValue(prefix + CPK::Fm::rgD1r);
		ptPtrs.d1l = apvts.getRawParameterValue(prefix + CPK::Fm::rgD1l);
		ptPtrs.d2r = apvts.getRawParameterValue(prefix + CPK::Fm::rgD2r);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::Fm::rgRr);
		ptPtrs.tl = apvts.getRawParameterValue(prefix + CPK::Fm::rgTl);

		ptPtrs.ksMode = apvts.getRawParameterValue(prefix + CPK::Fm::ksMode);
		ptPtrs.ks = apvts.getRawParameterValue(prefix + CPK::Fm::ks);
		ptPtrs.ksrOPP = apvts.getRawParameterValue(prefix + CPK::Fm::ksrOPP);
		ptPtrs.kslOPP = apvts.getRawParameterValue(prefix + CPK::Fm::kslOPP);

		ptPtrs.xof = apvts.getRawParameterValue(prefix + CPK::Fm::xof);
		ptPtrs.kor = apvts.getRawParameterValue(prefix + CPK::Fm::kor);
	}

	static inline void setupOpnAdsrPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpnAdsr& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::Fm::bypass);
		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::Fm::rgAr);
		ptPtrs.dr = apvts.getRawParameterValue(prefix + CPK::Fm::rgDr);
		ptPtrs.sl = apvts.getRawParameterValue(prefix + CPK::Fm::rgSl);
		ptPtrs.sr = apvts.getRawParameterValue(prefix + CPK::Fm::rgSr);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::Fm::rgRr);
		ptPtrs.tl = apvts.getRawParameterValue(prefix + CPK::Fm::rgTl);

		ptPtrs.ks = apvts.getRawParameterValue(prefix + CPK::Fm::ks);

		ptPtrs.xof = apvts.getRawParameterValue(prefix + CPK::Fm::xof);
		ptPtrs.kor = apvts.getRawParameterValue(prefix + CPK::Fm::kor);
	}

	static inline void setupOpzx7AdsrPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpzx7Adsr& ptPtrs){
		ptPtrs.bypass = apvts.getRawParameterValue(prefix + CPK::Fm::bypass);
		ptPtrs.rgEnable = apvts.getRawParameterValue(prefix + CPK::Fm::rgEn);

		ptPtrs.realAr = apvts.getRawParameterValue(prefix + CPK::Fm::ar);
		ptPtrs.realD1r = apvts.getRawParameterValue(prefix + CPK::Fm::d1r);
		ptPtrs.realD1l = apvts.getRawParameterValue(prefix + CPK::Fm::d1l);
		ptPtrs.realD2r = apvts.getRawParameterValue(prefix + CPK::Fm::d2r);
		ptPtrs.realRr = apvts.getRawParameterValue(prefix + CPK::Fm::rr);
		ptPtrs.realTl = apvts.getRawParameterValue(prefix + CPK::Fm::tl);

		ptPtrs.ar = apvts.getRawParameterValue(prefix + CPK::Fm::rgAr);
		ptPtrs.d1r = apvts.getRawParameterValue(prefix + CPK::Fm::rgD1r);
		ptPtrs.d1l = apvts.getRawParameterValue(prefix + CPK::Fm::rgD1l);
		ptPtrs.d2r = apvts.getRawParameterValue(prefix + CPK::Fm::rgD2r);
		ptPtrs.rr = apvts.getRawParameterValue(prefix + CPK::Fm::rgRr);
		ptPtrs.tl = apvts.getRawParameterValue(prefix + CPK::Fm::rgTl);

		ptPtrs.ksEn = apvts.getRawParameterValue(prefix + CPK::Fm::ksEn);
		ptPtrs.ksMode = apvts.getRawParameterValue(prefix + CPK::Fm::ksMode);
		ptPtrs.ksrMA7 = apvts.getRawParameterValue(prefix + CPK::Fm::ksrMA7);
		ptPtrs.kslMA7 = apvts.getRawParameterValue(prefix + CPK::Fm::kslMA7);
		ptPtrs.ksrOPZ = apvts.getRawParameterValue(prefix + CPK::Fm::ksrOPZ);
		ptPtrs.kslOPZ = apvts.getRawParameterValue(prefix + CPK::Fm::kslOPZ);
		ptPtrs.ksBp = apvts.getRawParameterValue(prefix + CPK::Fm::ksBp);
		ptPtrs.ksLc = apvts.getRawParameterValue(prefix + CPK::Fm::ksLc);
		ptPtrs.ksRc = apvts.getRawParameterValue(prefix + CPK::Fm::ksRc);
		ptPtrs.ksLd = apvts.getRawParameterValue(prefix + CPK::Fm::ksLd);
		ptPtrs.ksRd = apvts.getRawParameterValue(prefix + CPK::Fm::ksRd);
		ptPtrs.ksRs = apvts.getRawParameterValue(prefix + CPK::Fm::ksRs);

		ptPtrs.sus = apvts.getRawParameterValue(prefix + CPK::Fm::sus);
		ptPtrs.xof = apvts.getRawParameterValue(prefix + CPK::Fm::xof);
		ptPtrs.kor = apvts.getRawParameterValue(prefix + CPK::Fm::kor);
	}

	static inline void setupOplEgPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOplEg& ptPtrs){
		ptPtrs.eg = apvts.getRawParameterValue(prefix + CPK::Fm::eg);
	}

	static inline void setupSsgEgPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgEg& ptPtrs){
		ptPtrs.ssgEg = apvts.getRawParameterValue(prefix + CPK::Fm::se);
		ptPtrs.ssgEgFreq = apvts.getRawParameterValue(prefix + CPK::Fm::seFreq);
	}

	static inline void setupWsPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsWs& ptPtrs){
		ptPtrs.waveSelect = apvts.getRawParameterValue(prefix + CPK::Fm::ws);
	}

	static inline void applyQuality(PrPtrsQuality& ptPtrs, QualityParams& params){
		params.bit = getInt(ptPtrs.depth);
		params.rate = getInt(ptPtrs.rate);
	}

	static inline void applyQualityPcm(PrPtrsQualityPcm& ptPtrs, QualityPcmParams& params){
		params.mode = getInt(ptPtrs.mode);
		params.rate = getInt(ptPtrs.rate);
		params.interp = getInt(ptPtrs.interp);
	}

	static inline void applyAlgFb(PrPtrsAlgFb& ptPtrs, AlgFbParams& params){
		params.algorithm = getInt(ptPtrs.alg);
		params.feedback = getInt(ptPtrs.fb);
	}

	static inline void applyOpzx7AlgFb(PrPtrsOpzx7AlgFb& ptPtrs, Opzx7AlgFbParams& params) {
		params.algorithm = getInt(ptPtrs.alg);
		params.feedback1 = getInt(ptPtrs.fb1);
		params.feedback2 = getInt(ptPtrs.fb2);
		params.feedback3 = getInt(ptPtrs.fb3);
		params.feedback4 = getInt(ptPtrs.fb4);
		params.feedback5 = getInt(ptPtrs.fb5);
		params.feedback6 = getInt(ptPtrs.fb6);
		params.feedback7 = getInt(ptPtrs.fb7);
		params.feedback8 = getInt(ptPtrs.fb8);
	}

	static inline void applyAdsrAmpEnv(PrPtrsAdsrAmpEnv& ptPtrs, AmpAdsrParams& params){
		params.bypass = getBool(ptPtrs.bypass);
		params.stl = getFloat(ptPtrs.stl);
		params.ar = getFloat(ptPtrs.ar);
		params.dr = getFloat(ptPtrs.dr);
		params.sl = getFloat(ptPtrs.sl);
		params.rr = getFloat(ptPtrs.rr);
		params.kor = getFloat(ptPtrs.kor);
	}

	static inline void applySsgSwEnv(PrPtrsSsgSwEnv& ptPtrs, SsgSwEnvParams& params){
		params.bypass = getBool(ptPtrs.bypass);
		params.steps = getInt(ptPtrs.steps);
		params.loop = getBool(ptPtrs.loop);
		params.loopTo = getInt(ptPtrs.loopTo);
		params.loopCount = getInt(ptPtrs.loopCount);
		params.r1 = getFloat(ptPtrs.r1);
		params.r2 = getFloat(ptPtrs.r2);
		params.r3 = getFloat(ptPtrs.r3);
		params.r4 = getFloat(ptPtrs.r4);
		params.r5 = getFloat(ptPtrs.r5);
		params.r6 = getFloat(ptPtrs.r6);
		params.stl = getFloat(ptPtrs.stl);
		params.l1 = getFloat(ptPtrs.l1);
		params.l2 = getFloat(ptPtrs.l2);
		params.l3 = getFloat(ptPtrs.l3);
		params.l4 = getFloat(ptPtrs.l4);
		params.l5 = getFloat(ptPtrs.l5);
		params.l6 = getFloat(ptPtrs.l6);
	}

	static inline void applySsgSwEnvOp(PrPtrsSsgSwEnvOp& ptPtrs, SsgSwEnvParams& params, bool& enable){
		enable = getBool(ptPtrs.enable);
		params.bypass = false;
		params.steps = getInt(ptPtrs.steps);
		params.loop = getBool(ptPtrs.loop);
		params.loopTo = getInt(ptPtrs.loopTo);
		params.loopCount = getInt(ptPtrs.loopCount);
		params.r1 = getFloat(ptPtrs.r1);
		params.r2 = getFloat(ptPtrs.r2);
		params.r3 = getFloat(ptPtrs.r3);
		params.r4 = getFloat(ptPtrs.r4);
		params.r5 = getFloat(ptPtrs.r5);
		params.r6 = getFloat(ptPtrs.r6);
		params.stl = getFloat(ptPtrs.stl);
		params.l1 = getFloat(ptPtrs.l1);
		params.l2 = getFloat(ptPtrs.l2);
		params.l3 = getFloat(ptPtrs.l3);
		params.l4 = getFloat(ptPtrs.l4);
		params.l5 = getFloat(ptPtrs.l5);
		params.l6 = getFloat(ptPtrs.l6);
	}

	static inline void applySsgSwEnv11(PrPtrsSsgSwEnv11& ptPtrs, SsgSwEnv11Params& params){
		params.bypass = getBool(ptPtrs.bypass);
		params.steps = getInt(ptPtrs.steps);
		params.loop = getBool(ptPtrs.loop);
		params.loopTo = getInt(ptPtrs.loopTo);
		params.loopCount = getInt(ptPtrs.loopCount);
		params.r1 = getFloat(ptPtrs.r1);
		params.r2 = getFloat(ptPtrs.r2);
		params.r3 = getFloat(ptPtrs.r3);
		params.r4 = getFloat(ptPtrs.r4);
		params.r5 = getFloat(ptPtrs.r5);
		params.r6 = getFloat(ptPtrs.r6);
		params.r7 = getFloat(ptPtrs.r7);
		params.r8 = getFloat(ptPtrs.r8);
		params.r9 = getFloat(ptPtrs.r9);
		params.r10 = getFloat(ptPtrs.r10);
		params.r11 = getFloat(ptPtrs.r11);
		params.stl = getFloat(ptPtrs.stl);
		params.l1 = getFloat(ptPtrs.l1);
		params.l2 = getFloat(ptPtrs.l2);
		params.l3 = getFloat(ptPtrs.l3);
		params.l4 = getFloat(ptPtrs.l4);
		params.l5 = getFloat(ptPtrs.l5);
		params.l6 = getFloat(ptPtrs.l6);
		params.l7 = getFloat(ptPtrs.l7);
		params.l8 = getFloat(ptPtrs.l8);
		params.l9 = getFloat(ptPtrs.l9);
		params.l10 = getFloat(ptPtrs.l10);
		params.l11 = getFloat(ptPtrs.l11);
	}

	static inline void applySsgSwEnv11Op(PrPtrsSsgSwEnv11Op& ptPtrs, SsgSwEnv11Params& params, bool& enable){
		enable = getBool(ptPtrs.enable);
		params.bypass = false;
		params.steps = getInt(ptPtrs.steps);
		params.loop = getBool(ptPtrs.loop);
		params.loopTo = getInt(ptPtrs.loopTo);
		params.loopCount = getInt(ptPtrs.loopCount);
		params.r1 = getFloat(ptPtrs.r1);
		params.r2 = getFloat(ptPtrs.r2);
		params.r3 = getFloat(ptPtrs.r3);
		params.r4 = getFloat(ptPtrs.r4);
		params.r5 = getFloat(ptPtrs.r5);
		params.r6 = getFloat(ptPtrs.r6);
		params.r7 = getFloat(ptPtrs.r7);
		params.r8 = getFloat(ptPtrs.r8);
		params.r9 = getFloat(ptPtrs.r9);
		params.r10 = getFloat(ptPtrs.r10);
		params.r11 = getFloat(ptPtrs.r11);
		params.stl = getFloat(ptPtrs.stl);
		params.l1 = getFloat(ptPtrs.l1);
		params.l2 = getFloat(ptPtrs.l2);
		params.l3 = getFloat(ptPtrs.l3);
		params.l4 = getFloat(ptPtrs.l4);
		params.l5 = getFloat(ptPtrs.l5);
		params.l6 = getFloat(ptPtrs.l6);
		params.l7 = getFloat(ptPtrs.l7);
		params.l8 = getFloat(ptPtrs.l8);
		params.l9 = getFloat(ptPtrs.l9);
		params.l10 = getFloat(ptPtrs.l10);
		params.l11 = getFloat(ptPtrs.l11);
	}

	static inline void applyPitchEnv(PrPtrsPitchEnv& ptPtrs, PitchAdsrParams& params){
		params.bypass = getBool(ptPtrs.bypass);
		params.ar = getFloat(ptPtrs.ar);
		params.dr = getFloat(ptPtrs.dr);
		params.rr = getFloat(ptPtrs.rr);
		params.stl = getInt(ptPtrs.stl);
		params.atl = getInt(ptPtrs.atl);
		params.ssl = getInt(ptPtrs.ssl);
		params.rll = getInt(ptPtrs.rll);
	}

	static inline void applyPitchEnvOp(PrPtrsPitchEnvOp& ptPtrs, PitchAdsrParams& params, bool& enable){
		enable = getBool(ptPtrs.enable);
		params.bypass = false;
		params.ar = getFloat(ptPtrs.ar);
		params.dr = getFloat(ptPtrs.dr);
		params.rr = getFloat(ptPtrs.rr);
		params.stl = getInt(ptPtrs.stl);
		params.atl = getInt(ptPtrs.atl);
		params.ssl = getInt(ptPtrs.ssl);
		params.rll = getInt(ptPtrs.rll);
	}

	static inline void applySsgSwPEnv11(PrPtrsSsgSwPEnv11& ptPtrs, SsgSwPEnv11Params& params){
		params.bypass = getBool(ptPtrs.bypass);
		params.steps = getInt(ptPtrs.steps);
		params.loop = getBool(ptPtrs.loop);
		params.loopTo = getInt(ptPtrs.loopTo);
		params.loopCount = getInt(ptPtrs.loopCount);
		params.r1 = getFloat(ptPtrs.r1);
		params.r2 = getFloat(ptPtrs.r2);
		params.r3 = getFloat(ptPtrs.r3);
		params.r4 = getFloat(ptPtrs.r4);
		params.r5 = getFloat(ptPtrs.r5);
		params.r6 = getFloat(ptPtrs.r6);
		params.r7 = getFloat(ptPtrs.r7);
		params.r8 = getFloat(ptPtrs.r8);
		params.r9 = getFloat(ptPtrs.r9);
		params.r10 = getFloat(ptPtrs.r10);
		params.r11 = getFloat(ptPtrs.r11);
		params.stl = getInt(ptPtrs.stl);
		params.l1 = getInt(ptPtrs.l1);
		params.l2 = getInt(ptPtrs.l2);
		params.l3 = getInt(ptPtrs.l3);
		params.l4 = getInt(ptPtrs.l4);
		params.l5 = getInt(ptPtrs.l5);
		params.l6 = getInt(ptPtrs.l6);
		params.l7 = getInt(ptPtrs.l7);
		params.l8 = getInt(ptPtrs.l8);
		params.l9 = getInt(ptPtrs.l9);
		params.l10 = getInt(ptPtrs.l10);
		params.l11 = getInt(ptPtrs.l11);
	}

	static inline void applySsgSwPEnv11Op(PrPtrsSsgSwPEnv11Op& ptPtrs, SsgSwPEnv11Params& params, bool& enable){
		enable = getBool(ptPtrs.enable);
		params.bypass = false;
		params.steps = getInt(ptPtrs.steps);
		params.loop = getBool(ptPtrs.loop);
		params.loopTo = getInt(ptPtrs.loopTo);
		params.loopCount = getInt(ptPtrs.loopCount);
		params.r1 = getFloat(ptPtrs.r1);
		params.r2 = getFloat(ptPtrs.r2);
		params.r3 = getFloat(ptPtrs.r3);
		params.r4 = getFloat(ptPtrs.r4);
		params.r5 = getFloat(ptPtrs.r5);
		params.r6 = getFloat(ptPtrs.r6);
		params.r7 = getFloat(ptPtrs.r7);
		params.r8 = getFloat(ptPtrs.r8);
		params.r9 = getFloat(ptPtrs.r9);
		params.r10 = getFloat(ptPtrs.r10);
		params.r11 = getFloat(ptPtrs.r11);
		params.stl = getInt(ptPtrs.stl);
		params.l1 = getInt(ptPtrs.l1);
		params.l2 = getInt(ptPtrs.l2);
		params.l3 = getInt(ptPtrs.l3);
		params.l4 = getInt(ptPtrs.l4);
		params.l5 = getInt(ptPtrs.l5);
		params.l6 = getInt(ptPtrs.l6);
		params.l7 = getInt(ptPtrs.l7);
		params.l8 = getInt(ptPtrs.l8);
		params.l9 = getInt(ptPtrs.l9);
		params.l10 = getInt(ptPtrs.l10);
		params.l11 = getInt(ptPtrs.l11);
	}

	static inline void applyOplDetune(PrPtrsOplDetune& ptPtrs, OplDetuneParams& params){
		params.multiple = getInt(ptPtrs.mutiple);
	}

	static inline void applyOpmDetune(PrPtrsOpmDetune& ptPtrs, OpmDetuneParams& params){
		params.multiple = getInt(ptPtrs.multiple);
		params.mulRatio = getFloat(ptPtrs.mulRatio);
		params.detune = getInt(ptPtrs.detune);
		params.detune2 = getInt(ptPtrs.detune2);
	}

	static inline void applyOpnDetune(PrPtrsOpnDetune& ptPtrs, OpnDetuneParams& params){
		params.multiple = getInt(ptPtrs.multiple);
		params.detune = getInt(ptPtrs.detune);
	}

	static inline void applyOpzx7Detune(PrPtrsOpzx7Detune& ptPtrs, Opzx7DetuneParams& params){
		params.multiple = getInt(ptPtrs.multiple);
		params.mulRatio = getFloat(ptPtrs.mulRatio);
		params.detune = getInt(ptPtrs.detune);
		params.detune2 = getInt(ptPtrs.detune2);
		params.detune3 = getInt(ptPtrs.detune3);
	}

	static inline void applyOplLfo(PrPtrsOplLfo& ptPtrs, LfoOplParams& params){
		params.pm = getBool(ptPtrs.pmEnable);
		params.am = getBool(ptPtrs.amEnable);
		params.ams = getFloat(ptPtrs.ams);
		params.amd = getFloat(ptPtrs.amd);
		params.pms = getFloat(ptPtrs.pms);
		params.pmd = getFloat(ptPtrs.pmd);
	}

	static inline void applyOpmLfo(PrPtrsOpmLfo& ptPtrs, LfoOpmParams& params){
		params.syncDelay = getFloat(ptPtrs.syncDelay);
		params.pm = getBool(ptPtrs.pmEnable);
		params.pmFreq = getFloat(ptPtrs.freq);
		params.pgIndex = getInt(ptPtrs.pmWave);
		params.pmsIndex = getInt(ptPtrs.pms);
		params.pmd = getInt(ptPtrs.pmd);
		params.am = getBool(ptPtrs.amEnable);
		params.amFreq = params.pmFreq;
		params.amSmoothRate = getFloat(ptPtrs.amSmRt);
		params.egIndex = getInt(ptPtrs.amWave);
		params.amsIndex = getInt(ptPtrs.ams);
		params.amd = getInt(ptPtrs.amd);
	}

	static inline void applyOpnaLfo(PrPtrsOpnaLfo& ptPtrs, LfoOpnaParams& params){
		params.syncDelay = getInt(ptPtrs.syncDelay);
		params.pmFreqIndex = getInt(ptPtrs.freqs);
		params.pm = getBool(ptPtrs.pmEnable);
		params.pms = getInt(ptPtrs.pms);
		params.amFreqIndex = params.pmFreqIndex;
		params.am = getBool(ptPtrs.amEnable);
		params.ams = getInt(ptPtrs.ams);
	}

	static inline void applyOpzx7Lfo(PrPtrsOpzx7Lfo& ptPtrs, LfoOpzx7Params& params){
		params.pmFreq = getFloat(ptPtrs.pmFreq);
		params.amFreq = getFloat(ptPtrs.amFreq);
		params.pgIndex = getInt(ptPtrs.pmShape);
		params.egIndex = getInt(ptPtrs.amShape);
		params.amSmoothRate = getFloat(ptPtrs.amSmoothRatio);
		params.amEnable = getBool(ptPtrs.am);
		params.pmEnable = getBool(ptPtrs.pm);
		params.pms = getFloat(ptPtrs.pms);
		params.ams = getFloat(ptPtrs.ams);
		params.pmd = getFloat(ptPtrs.pmd);
		params.amd = getFloat(ptPtrs.amd);
		params.pmSyncDelay = getFloat(ptPtrs.pmSyncDelay);
		params.amSyncDelay = getFloat(ptPtrs.amSyncDelay);
	}

	static inline void applyN88Lfo(PrPtrsN88Lfo& ptPtrs, LfoN88Params& params){
		// Int(0〜16383) として読み込み、Hzに変換する
		float n88Speed = getFloat(ptPtrs.freq);
		// ソフトウェアLFOの周波数変換公式 (60Hz割り込み、16bitカウンター)
		// f = (Speed * 60Hz) / 65536
		params.syncDelay = getInt(ptPtrs.syncDelay);
		params.pm = getBool(ptPtrs.pmEnable);
		params.pmIndex = getInt(ptPtrs.shape);
		params.pmFreq = (n88Speed * 60.0f) / 65536.0f;
		params.pms = getInt(ptPtrs.pms);
		params.pmd = getInt(ptPtrs.pmd);
		params.am = getBool(ptPtrs.amEnable);
		params.amIndex = params.pmIndex;
		params.amFreq = params.pmFreq;
		params.amSmoothRate = getFloat(ptPtrs.amSmRt);
		params.amd = getInt(ptPtrs.amd);
	}

	static inline void applyFix(PrPtrsFix& ptPtrs, FixModeParams& params){
		params.enable = getBool(ptPtrs.enable);
		params.freq = getFloat(ptPtrs.freq);
	}

	static inline void applyUnison(PrPtrsUnison& ptPtrs, UnisonParams& params){
		params.voices = getInt(ptPtrs.voices);
		params.detuneCents = getInt(ptPtrs.detuneCents);
		params.spread = getFloat(ptPtrs.spread);
		params.arpEnable = getBool(ptPtrs.arpEnable);
		params.arpFreq = getInt(ptPtrs.arpFreq);
		params.arpSmooth = getBool(ptPtrs.arpSmooth);

		for (int i = 0; i < Global::unisonParaVoices; ++i) {
			params.paraDistance[i] = getFloat(ptPtrs.paraDistance[i]);
			params.paraDetune[i] = getInt(ptPtrs.paraDetune[i]);
		}
	}

	static inline void applyToneNoise(PrPtrsToneNoise& ptPtrs, ToneNoiseParams& params){
		params.tone = getFloat(ptPtrs.tone);
		params.noiseLevel = getFloat(ptPtrs.noise);
		params.noiseFreq = getFloat(ptPtrs.noiseFreq);
		params.noiseOnNote = getBool(ptPtrs.noiseOnNote);
		params.mix = getFloat(ptPtrs.mix);
	}

	static inline void applyPcm(PrPtrsPcm& ptPtrs, PcmParams& params){
		params.offset = getFloat(ptPtrs.offset);
		params.ratio = getFloat(ptPtrs.ratio);
	}

	static inline void applyLp(PrPtrsLp& ptPtrs, LoopPointParams& params){
		params.enable = getBool(ptPtrs.enable);
		params.start = getFloat(ptPtrs.start);
		params.end = getFloat(ptPtrs.end);
	}

	static inline void applyWtMod(PrPtrsWtMod& ptPtrs, WtModParams& params){
		params.enable = getBool(ptPtrs.enable);
		params.depth = getFloat(ptPtrs.depth);
		params.speed = getFloat(ptPtrs.speed);
		params.shape = getInt(ptPtrs.shape);

		// 選ばれているスロットの波形を渡す。読み込んでいなければ
		// 中身は 0 のままで、音源側では変調が掛からない。
		if (ptPtrs.slots != nullptr) {
			int slot = std::clamp(getInt(ptPtrs.waveSlot), 0, Global::WtMod::slots - 1);

			params.wave = (*ptPtrs.slots)[slot].data;
		}

		for (int i = 0; i < CPV::WtMod::FdsTable::size; ++i) {
			params.fdsTable[i] = getInt(ptPtrs.fdsTable[i]);
		}
	}

	static inline void applyWtAmpMod(PrPtrsWtAmpMod& ptPtrs, WtAmpModParams& params){
		params.enable = getBool(ptPtrs.enable);
		params.depth = getFloat(ptPtrs.depth);
		params.speed = getFloat(ptPtrs.speed);
		params.shape = getInt(ptPtrs.shape);
		params.min = getFloat(ptPtrs.min);
		params.max = getFloat(ptPtrs.max);

		if (ptPtrs.slots != nullptr) {
			int slot = std::clamp(getInt(ptPtrs.waveSlot), 0, Global::WtMod::slots - 1);

			params.wave = (*ptPtrs.slots)[slot].data;
		}

		for (int i = 0; i < CPV::WtAmpMod::FdsTable::size; ++i) {
			params.fdsTable[i] = getInt(ptPtrs.fdsTable[i]);
		}
	}

	static inline void applySsgDuty(PrPtrsSsgDuty& ptPtrs, SsgDutyParams& params){
		params.mode = getInt(ptPtrs.mode);
		params.preset = getInt(ptPtrs.preset);
		params.var = getFloat(ptPtrs.var);
		params.invert = getBool(ptPtrs.invert);
		params.fc = getBool(ptPtrs.fc);
		params.fcFluc = getFloat(ptPtrs.fcFluc);
	}

	static inline void applySsgTri(PrPtrsSsgTri& ptPtrs, SsgTriParams& params){
		params.keyTrack = getBool(ptPtrs.keyTrk);
		params.peak = getFloat(ptPtrs.peak);
		params.freq = getFloat(ptPtrs.freq);
	}

	static inline void applySsgHwEnv(PrPtrsSsgHwEnv& ptPtrs, SsgHwEnvParams& params) {
		params.enable = getBool(ptPtrs.enable);
		params.shape = getInt(ptPtrs.shape);
		params.period = getFloat(ptPtrs.period);
		params.min = getFloat(ptPtrs.min);
		params.max = getFloat(ptPtrs.max);
		params.smooth = getBool(ptPtrs.smooth);
	}

	static inline void applySsgHwPEnv(PrPtrsSsgHwPEnv& ptPtrs, SsgHwPEnvParams& params) {
		params.enable = getBool(ptPtrs.enable);
		params.shape = getInt(ptPtrs.shape);
		params.period = getFloat(ptPtrs.period);
		params.min = getInt(ptPtrs.min);
		params.max = getInt(ptPtrs.max);
		params.smooth = getBool(ptPtrs.smooth);
	}

	static inline void applyPanpot(PrPtrsPanpot& ptPtrs, PanpotParams& params){
		params.enable = getBool(ptPtrs.enable);
		params.pan = getInt(ptPtrs.panpot);
	}

	static inline void applyAdpcmBasic(PrPtrsAdpcmBasic& ptPtrs, AdpcmParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.loop = PrHelper::getBool(ptPtrs.loop);
		params.pan = PrHelper::getFloat(ptPtrs.pan);
	}

	static inline void applyBeepBasic(PrPtrsBeepBasic& ptPtrs, BeepParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.antiAlias = PrHelper::getBool(ptPtrs.antiAlias);
		params.timerClock = PrHelper::getInt(ptPtrs.timerClock);
	}

	static inline void applyOplBasic(PrPtrsOplBasic& ptPtrs, OplParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
	}

	static inline void applyOpl3Basic(PrPtrsOpl3Basic& ptPtrs,Opl3Params& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
	}

	static inline void applyOpmBasic(PrPtrsOpmBasic& ptPtrs, OpmParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.pan = PrHelper::getInt(ptPtrs.pan);
	}

	static inline void applyOpnBasic(PrPtrsOpnBasic& ptPtrs, OpnParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
	}

	static inline void applyOpnaBasic(PrPtrsOpnaBasic& ptPtrs, OpnaParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.pan = PrHelper::getInt(ptPtrs.pan);
	}

	static inline void applyOpzx7Basic(PrPtrsOpzx7Basic& ptPtrs, Opzx7Params& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
	}

	static inline void applyRhythmBasic(PrPtrsRhythmBasic& ptPtrs, RhythmParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
	}

	static inline void applyRhythmPadBasic(PrPtrsRhythmPadBasic& ptPtrs, RhythmPadParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.pan = PrHelper::getFloat(ptPtrs.pan);
		params.noteNumber = PrHelper::getInt(ptPtrs.noteNumber);
		params.isOneShot = PrHelper::getBool(ptPtrs.isOneShot);
	}

	static inline void applySsgBasic(PrPtrsSsgBasic& ptPtrs, SsgParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.waveform = PrHelper::getInt(ptPtrs.waveform);
	}

	static inline void applyWtBasic(PrPtrsWtBasic& ptPtrs, WtParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.waveform = PrHelper::getInt(ptPtrs.wave);
		params.tableSize = PrHelper::getInt(ptPtrs.sampleSize);
		params.steps = PrHelper::getInt(ptPtrs.step);
		params.interpolate = PrHelper::getBool(ptPtrs.interpolate);
	}

	static inline void applyWtPlusBasic(PrPtrsWtPlusBasic& ptPtrs, WtPlusParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.slot = PrHelper::getInt(ptPtrs.slot);
		params.steps = PrHelper::getInt(ptPtrs.steps);
		params.interpolate = PrHelper::getBool(ptPtrs.interpolate);
	}

	static inline void applyWt2Basic(PrPtrsWt2Basic& ptPtrs, Wt2Params& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.waveform = PrHelper::getInt(ptPtrs.wave);
		params.tableSize = PrHelper::getInt(ptPtrs.sampleSize);
		params.customWaveResolution = PrHelper::getInt(ptPtrs.resolution);
		params.interpolate = PrHelper::getBool(ptPtrs.interpolate);
	}

	static inline void applyOplAdsr(PrPtrsOplAdsr& ptPtrs, OplAdsrParams& params){
		params.bypass = PrHelper::getBool(ptPtrs.bypass);

		params.ar = PrHelper::getInt(ptPtrs.ar);
		params.dr = PrHelper::getInt(ptPtrs.dr);
		params.sl = PrHelper::getInt(ptPtrs.sl);
		params.rr = PrHelper::getInt(ptPtrs.rr);
		params.tl = PrHelper::getInt(ptPtrs.tl);
		params.egType = PrHelper::getBool(ptPtrs.egType);

		params.ksOPL.ksr = PrHelper::getBool(ptPtrs.ksr);
		params.ksOPL.ksl = PrHelper::getInt(ptPtrs.ksl);

		params.sus = PrHelper::getBool(ptPtrs.sus);
		params.xof = PrHelper::getBool(ptPtrs.xof);
		params.kor = PrHelper::getBool(ptPtrs.kor);
	}

	static inline void applyOpl3Adsr(PrPtrsOpl3Adsr& ptPtrs, OplAdsrParams& params){
		params.bypass = PrHelper::getBool(ptPtrs.bypass);

		params.ar = PrHelper::getInt(ptPtrs.ar);
		params.dr = PrHelper::getInt(ptPtrs.dr);
		params.sl = PrHelper::getInt(ptPtrs.sl);
		params.rr = PrHelper::getInt(ptPtrs.rr);
		params.tl = PrHelper::getInt(ptPtrs.tl);
		params.egType = PrHelper::getBool(ptPtrs.egType);

		params.ksOPL.ksr = PrHelper::getBool(ptPtrs.ksr);
		params.ksOPL.ksl = PrHelper::getInt(ptPtrs.ksl);

		params.xof = PrHelper::getBool(ptPtrs.xof);
		params.kor = PrHelper::getBool(ptPtrs.kor);
	}

	static inline void applyOpmAdsr(PrPtrsOpmAdsr& ptPtrs, FmRgAdddrParams& params){
		params.bypass = PrHelper::getBool(ptPtrs.bypass);

		params.ar = PrHelper::getInt(ptPtrs.ar);
		params.d1r = PrHelper::getInt(ptPtrs.d1r);
		params.d1l = PrHelper::getInt(ptPtrs.d1l);
		params.d2r = PrHelper::getInt(ptPtrs.d2r);
		params.rr = PrHelper::getInt(ptPtrs.rr);
		params.tl = PrHelper::getInt(ptPtrs.tl);

		params.ksMode = (FmRgAdddrKeyScaleMode)PrHelper::getInt(ptPtrs.ksMode);
		params.ksOPM.ks = PrHelper::getInt(ptPtrs.ks);
		params.ksOPP.ksr = PrHelper::getInt(ptPtrs.ksrOPP);
		params.ksOPP.ksl = PrHelper::getInt(ptPtrs.kslOPP);

		params.xof = PrHelper::getBool(ptPtrs.xof);
		params.kor = PrHelper::getBool(ptPtrs.kor);
	}

	static inline void applyOpnAdsr(PrPtrsOpnAdsr& ptPtrs, FmRgAdssrParams& params){
		params.bypass = PrHelper::getBool(ptPtrs.bypass);

		params.ar = PrHelper::getInt(ptPtrs.ar);
		params.dr = PrHelper::getInt(ptPtrs.dr);
		params.sl = PrHelper::getInt(ptPtrs.sl);
		params.sr = PrHelper::getInt(ptPtrs.sr);
		params.rr = PrHelper::getInt(ptPtrs.rr);
		params.tl = PrHelper::getInt(ptPtrs.tl);

		params.ksOPN.ks = PrHelper::getInt(ptPtrs.ks);

		params.xof = PrHelper::getBool(ptPtrs.xof);
		params.kor = PrHelper::getBool(ptPtrs.kor);
	}

	static inline void applyOpzx7Adsr(PrPtrsOpzx7Adsr& ptPtrs, Opzx7AdddrParams& params){
		params.bypass = PrHelper::getBool(ptPtrs.bypass);
		params.rgEnable = PrHelper::getBool(ptPtrs.rgEnable);

		params.real.ar = PrHelper::getFloat(ptPtrs.realAr);
		params.real.d1r = PrHelper::getFloat(ptPtrs.realD1r);
		params.real.d1l = PrHelper::getFloat(ptPtrs.realD1l);
		params.real.d2r = PrHelper::getFloat(ptPtrs.realD2r);
		params.real.rr = PrHelper::getFloat(ptPtrs.realRr);
		params.real.tl = PrHelper::getFloat(ptPtrs.realTl);

		params.rg.ar = PrHelper::getInt(ptPtrs.ar);
		params.rg.d1r = PrHelper::getInt(ptPtrs.d1r);
		params.rg.d1l = PrHelper::getInt(ptPtrs.d1l);
		params.rg.d2r = PrHelper::getInt(ptPtrs.d2r);
		params.rg.rr = PrHelper::getInt(ptPtrs.rr);
		params.rg.tl = PrHelper::getInt(ptPtrs.tl);

		params.ksEn = PrHelper::getBool(ptPtrs.ksEn);
		params.ksMode = (Opzx7AdddrKeyScaleMode)PrHelper::getInt(ptPtrs.ksMode);
		params.ksMA7.ksr = PrHelper::getBool(ptPtrs.ksrMA7);
		params.ksMA7.ksl = PrHelper::getInt(ptPtrs.kslMA7);
		params.ksOPZ.ksr = PrHelper::getInt(ptPtrs.ksrOPZ);
		params.ksOPZ.ksl = PrHelper::getInt(ptPtrs.kslOPZ);
		params.ksOPS.ksBp = PrHelper::getInt(ptPtrs.ksBp);
		params.ksOPS.ksLc = PrHelper::getInt(ptPtrs.ksLc);
		params.ksOPS.ksRc = PrHelper::getInt(ptPtrs.ksRc);
		params.ksOPS.ksLd = PrHelper::getFloat(ptPtrs.ksLd);
		params.ksOPS.ksRd = PrHelper::getFloat(ptPtrs.ksRd);
		params.ksOPS.ksRs = PrHelper::getInt(ptPtrs.ksRs);

		params.sus = PrHelper::getBool(ptPtrs.sus);
		params.xof = PrHelper::getBool(ptPtrs.xof);
		params.kor = PrHelper::getBool(ptPtrs.kor);
	}

	static inline void applySsgEg(PrPtrsSsgEg& ptPtrs, SsgEgParams& params){
		params.eg = PrHelper::getInt(ptPtrs.ssgEg);
		params.freq = PrHelper::getFloat(ptPtrs.ssgEgFreq);
	}

	static inline void applyWtCustomWave(PrPtrsWtCustomWave& ptPtrs, WtParams& params){
		for (int i = 0; i < 32; ++i)
		{
			params.customWave32[i] = PrHelper::getFloat(ptPtrs.c32[i]);
		}

		for (int i = 0; i < 64; ++i)
		{
			params.customWave64[i] = PrHelper::getFloat(ptPtrs.c64[i]);
		}

		for (int i = 0; i < 128; ++i)
		{
			params.customWave128[i] = PrHelper::getFloat(ptPtrs.c128[i]);
		}

		for (int i = 0; i < 256; ++i)
		{
			params.customWave256[i] = PrHelper::getFloat(ptPtrs.c256[i]);
		}
	}

	static inline void applyWt2CustomWave(PrPtrsWtCustomWave& ptPtrs, Wt2Params& params){
		for (int i = 0; i < 32; ++i)
		{
			params.customWave32[i] = PrHelper::getInt(ptPtrs.c32[i]);
		}

		for (int i = 0; i < 64; ++i)
		{
			params.customWave64[i] = PrHelper::getInt(ptPtrs.c64[i]);
		}

		for (int i = 0; i < 128; ++i)
		{
			params.customWave128[i] = PrHelper::getInt(ptPtrs.c128[i]);
		}

		for (int i = 0; i < 256; ++i)
		{
			params.customWave256[i] = PrHelper::getInt(ptPtrs.c256[i]);
		}
	}

	static inline void addFloat(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name, float min, float max, float ini) {
		layout.add(std::make_unique<juce::AudioParameterFloat>(code, name, min, max, ini));
	}

	static inline void addInt(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name, int min, int max, int ini) {
		layout.add(std::make_unique<juce::AudioParameterInt>(code, name, min, max, ini));
	}

	static inline void addBool(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name, bool ini) {
		layout.add(std::make_unique<juce::AudioParameterBool>(code, name, ini));
	}

	static inline void addPitchEnvRate(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::PitchAdsr::R::min, CPV::PitchAdsr::R::max, CPV::PitchAdsr::R::initial);
	}

	static inline void addPitchEnvLevel(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addInt(layout, code, name, CPV::PitchAdsr::L::min, CPV::PitchAdsr::L::max, CPV::PitchAdsr::L::initial);
	}

	static inline void addSsgSwEnvRate(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwEnv::R::min, CPV::SsgSwEnv::R::max, CPV::SsgSwEnv::R::initial);
	}

	static inline void addSsgSwEnvLevel(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwEnv::L::min, CPV::SsgSwEnv::L::max, CPV::SsgSwEnv::L::initial);
	}

	static inline void addSsgSwEnvSRLevel(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwEnv::SRL::min, CPV::SsgSwEnv::SRL::max, CPV::SsgSwEnv::SRL::initial);
	}

	static inline void addSsgSwEnv11Rate(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwEnv11::R::min, CPV::SsgSwEnv11::R::max, CPV::SsgSwEnv11::R::initial);
	}

	static inline void addSsgSwEnv11Level(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwEnv11::L::min, CPV::SsgSwEnv11::L::max, CPV::SsgSwEnv11::L::initial);
	}

	static inline void addSsgSwEnv11SRLevel(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwEnv11::SRL::min, CPV::SsgSwEnv11::SRL::max, CPV::SsgSwEnv11::SRL::initial);
	}

	static inline void addSsgSwPenv11Rate(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addFloat(layout, code, name, CPV::SsgSwPEnv11::R::min, CPV::SsgSwPEnv11::R::max, CPV::SsgSwPEnv11::R::initial);
	}

	static inline void addSsgSwPenv11Level(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& code, const juce::String& name)
	{
		PrHelper::addInt(layout, code, name, CPV::SsgSwPEnv11::L::min, CPV::SsgSwPEnv11::L::max, CPV::SsgSwPEnv11::L::initial);
	}

	static inline void addLevelParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addFloat(
			layout,
			prefix + CPK::level,
			prefixName + CPN::level,
			CPV::Level::min, CPV::Level::max, CPV::Level::initial
		);
	}

	static inline void addRhythmLevelParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addFloat(
			layout,
			prefix + CPK::level, 
			prefixName + CPN::vol,
			CPV::Level::min, CPV::Level::max, CPV::Level::initial
		);
	}

	static inline void addRhythmPadVolParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addFloat(
			layout, 
			prefix + CPK::vol, 
			prefixName + CPN::vol, 
			CPV::Vol::min, CPV::Vol::max, CPV::Vol::initial
		);
	}

	static inline void addOpOplAmpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgAr, 
			namePrefix + CPN::Fm::rgAr, 
			CPV::OplRg::Ar::min, CPV::OplRg::Ar::max, CPV::OplRg::Ar::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::Fm::rgDr, 
			namePrefix + CPN::Fm::rgDr, 
			CPV::OplRg::Dr::min, CPV::OplRg::Dr::max, CPV::OplRg::Dr::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::Fm::rgSl,
			namePrefix + CPN::Fm::rgSl, 
			CPV::OplRg::Sl::min, CPV::OplRg::Sl::max, CPV::OplRg::Sl::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgRr, 
			namePrefix + CPN::Fm::rgRr, 
			CPV::OplRg::Rr::min, CPV::OplRg::Rr::max, CPV::OplRg::Rr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgTl, 
			namePrefix + CPN::Fm::rgTl, 
			CPV::OplRg::Tl::min, CPV::OplRg::Tl::max, CPV::OplRg::Tl::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::ksr, 
			namePrefix + CPN::Fm::ksr, 
			CPV::OplKs::Ksr::initial
		); // 0=Low, 1=High
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksl, 
			namePrefix + CPN::Fm::ksl, 
			CPV::OplKs::Ksl::min, CPV::OplKs::Ksl::max, CPV::OplKs::Ksl::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::egType, 
			namePrefix + CPN::Fm::egType, 
			CPV::OplRg::EgType::initial
		); // 1=Sustain, 0=Decay
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::sus, 
			namePrefix + CPN::Fm::sus,
			CPV::OplRg::Sus::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::xof, 
			namePrefix + CPN::Fm::xof,
			CPV::OplRg::Xof::initial
		); // Xof (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::kor, 
			namePrefix + CPN::Fm::kor, 
			CPV::OplRg::Kor::initial
		); // Kor (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::bypass, 
			namePrefix + CPN::Fm::bypass, 
			CPV::OplRg::Bypass::initial
		); // Bypass (Switch)
	}

	static inline void addOpOpl3AmpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgAr,
			namePrefix + CPN::Fm::rgAr, 
			CPV::OplRg::Ar::min, CPV::OplRg::Ar::max, CPV::OplRg::Ar::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgDr,
			namePrefix + CPN::Fm::rgDr, 
			CPV::OplRg::Dr::min, CPV::OplRg::Dr::max, CPV::OplRg::Dr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgSl, 
			namePrefix + CPN::Fm::rgSl, 
			CPV::OplRg::Sl::min, CPV::OplRg::Sl::max, CPV::OplRg::Sl::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgRr, 
			namePrefix + CPN::Fm::rgRr, 
			CPV::OplRg::Rr::min, CPV::OplRg::Rr::max, CPV::OplRg::Rr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgTl, 
			namePrefix + CPN::Fm::rgTl, 
			CPV::OplRg::Tl::min, CPV::OplRg::Tl::max, CPV::OplRg::Tl::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::ksr, 
			namePrefix + CPN::Fm::ksr, 
			CPV::OplKs::Ksr::initial
		); // 0=Low, 1=High
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksl, 
			namePrefix + CPN::Fm::ksl, 
			CPV::OplKs::Ksl::min, CPV::OplKs::Ksl::max, CPV::OplKs::Ksl::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::egType, 
			namePrefix + CPN::Fm::egType, 
			CPV::OplRg::EgType::initial
		); // 1=Sustain, 0=Decay
		PrHelper::addBool(
			layout,
			prefix + CPK::Fm::xof, 
			namePrefix + CPN::Fm::xof, 
			CPV::OplRg::Xof::initial
		); // Xof (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::kor,
			namePrefix + CPN::Fm::kor,
			CPV::OplRg::Kor::initial
		); // Kor (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::bypass, 
			namePrefix + CPN::Fm::bypass, 
			CPV::OplRg::Bypass::initial
		); // Bypass (Switch)
	}

	static inline void addOpOpmAmpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout,
			prefix + CPK::Fm::rgAr, 
			namePrefix + CPN::Fm::rgAr, 
			CPV::OpmRg::Ar::min, CPV::OpmRg::Ar::max, CPV::OpmRg::Ar::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgD1r, 
			namePrefix + CPN::Fm::rgD1r, 
			CPV::OpmRg::D1r::min, CPV::OpmRg::D1r::max, CPV::OpmRg::D1r::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgD1l, 
			namePrefix + CPN::Fm::rgD1l, 
			CPV::OpmRg::D1l::min, CPV::OpmRg::D1l::max, CPV::OpmRg::D1l::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgD2r, 
			namePrefix + CPN::Fm::rgD2r, 
			CPV::OpmRg::D2r::min, CPV::OpmRg::D2r::max, CPV::OpmRg::D2r::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgRr, 
			namePrefix + CPN::Fm::rgRr, 
			CPV::OpmRg::Rr::min, CPV::OpmRg::Rr::max, CPV::OpmRg::Rr::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::Fm::rgTl,
			namePrefix + CPN::Fm::rgTl, 
			CPV::OpmRg::Tl::min, CPV::OpmRg::Tl::max, CPV::OpmRg::Tl::initial
		);

		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksMode, 
			namePrefix + CPN::Fm::ksMode, 
			CPV::OpmKs::KsMode::min, CPV::OpmKs::KsMode::max, CPV::OpmKs::KsMode::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ks, 
			namePrefix + CPN::Fm::ks, 
			CPV::OpmKs::Ks::min, CPV::OpmKs::Ks::max, CPV::OpmKs::Ks::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksrOPP, 
			namePrefix + CPN::Fm::ksrOPP, 
			CPV::OpmKs::KsrOPP::min, CPV::OpmKs::KsrOPP::max, CPV::OpmKs::KsrOPP::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::kslOPP, 
			namePrefix + CPN::Fm::kslOPP, 
			CPV::OpmKs::KslOPP::min, CPV::OpmKs::KslOPP::max, CPV::OpmKs::KslOPP::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::xof, 
			namePrefix + CPN::Fm::xof, 
			CPV::OpmRg::Xof::initial
		); // Xof (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::kor, 
			namePrefix + CPN::Fm::kor, 
			CPV::OpmRg::Kor::initial
		); // Kor (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::bypass, 
			namePrefix + CPN::Fm::bypass, 
			CPV::OpmRg::Bypass::initial
		); // Bypass (Switch)
	}

	static inline void addOpOpnAmpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgAr, 
			namePrefix + CPN::Fm::rgAr, 
			CPV::OpnRg::Ar::min, CPV::OpnRg::Ar::max, CPV::OpnRg::Ar::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgDr, 
			namePrefix + CPN::Fm::rgDr, 
			CPV::OpnRg::Dr::min, CPV::OpnRg::Dr::max, CPV::OpnRg::Dr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgSl, 
			namePrefix + CPN::Fm::rgSl, 
			CPV::OpnRg::Sl::min, CPV::OpnRg::Sl::max, CPV::OpnRg::Sl::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgSr, 
			namePrefix + CPN::Fm::rgSr, 
			CPV::OpnRg::Sr::min, CPV::OpnRg::Sr::max, CPV::OpnRg::Sr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgRr, 
			namePrefix + CPN::Fm::rgRr, 
			CPV::OpnRg::Rr::min, CPV::OpnRg::Rr::max, CPV::OpnRg::Rr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgTl, 
			namePrefix + CPN::Fm::rgTl, 
			CPV::OpnRg::Tl::min, CPV::OpnRg::Tl::max, CPV::OpnRg::Tl::initial
		);

		PrHelper::addInt(
			layout,
			prefix + CPK::Fm::ks,
			namePrefix + CPN::Fm::ks,
			CPV::OpnKs::Ks::min, CPV::OpnKs::Ks::max, CPV::OpnKs::Ks::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::xof, 
			namePrefix + CPN::Fm::xof, 
			CPV::OpnRg::Xof::initial
		); // Xof (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::kor, 
			namePrefix + CPN::Fm::kor, 
			CPV::OpnRg::Kor::initial
		); // Kor (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::bypass, 
			namePrefix + CPN::Fm::bypass, 
			CPV::OpnRg::Bypass::initial
		); // Bypass (Switch)
	}

	static inline void addOpOpzx7AmpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addBool(
			layout,
			prefix + CPK::Fm::rgEn, 
			namePrefix + CPN::Fm::rgEn, 
			CPV::Opzx7Rg::Enable::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgAr, 
			namePrefix + CPN::Fm::rgAr, 
			CPV::Opzx7Rg::Ar::min, CPV::Opzx7Rg::Ar::max, CPV::Opzx7Rg::Ar::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgD1r, 
			namePrefix + CPN::Fm::rgD1r, 
			CPV::Opzx7Rg::D1r::min, CPV::Opzx7Rg::D1r::max, CPV::Opzx7Rg::D1r::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgD1l, 
			namePrefix + CPN::Fm::rgD1l, 
			CPV::Opzx7Rg::D1l::min, CPV::Opzx7Rg::D1l::max, CPV::Opzx7Rg::D1l::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgD2r, 
			namePrefix + CPN::Fm::rgD2r, 
			CPV::Opzx7Rg::D2r::min, CPV::Opzx7Rg::D2r::max, CPV::Opzx7Rg::D2r::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgRr, 
			namePrefix + CPN::Fm::rgRr, 
			CPV::Opzx7Rg::Rr::min, CPV::Opzx7Rg::Rr::max, CPV::Opzx7Rg::Rr::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::rgTl, 
			namePrefix + CPN::Fm::rgTl, 
			CPV::Opzx7Rg::Tl::min, CPV::Opzx7Rg::Tl::max, CPV::Opzx7Rg::Tl::initial
		);

		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::ar, 
			namePrefix + CPN::Fm::ar, 
			CPV::Opzx7Real::Ar::min, CPV::Opzx7Real::Ar::max, CPV::Opzx7Real::Ar::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::d1r, 
			namePrefix + CPN::Fm::d1r, 
			CPV::Opzx7Real::D1r::min, CPV::Opzx7Real::D1r::max, CPV::Opzx7Real::D1r::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::d1l, 
			namePrefix + CPN::Fm::d1l, 
			CPV::Opzx7Real::D1l::min, CPV::Opzx7Real::D1l::max, CPV::Opzx7Real::D1l::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::d2r, 
			namePrefix + CPN::Fm::d2r, 
			CPV::Opzx7Real::D2r::min, CPV::Opzx7Real::D2r::max, CPV::Opzx7Real::D2r::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::rr, 
			namePrefix + CPN::Fm::rr, 
			CPV::Opzx7Real::Rr::min, CPV::Opzx7Real::Rr::max, CPV::Opzx7Real::Rr::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::tl, 
			namePrefix + CPN::Fm::tl, 
			CPV::Opzx7Real::Tl::min, CPV::Opzx7Real::Tl::max, CPV::Opzx7Real::Tl::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::ksEn, 
			namePrefix + CPN::Fm::ksEn, 
			CPV::Opzx7Ks::KsEn::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksMode, 
			namePrefix + CPN::Fm::ksMode, 
			CPV::Opzx7Ks::KsMode::min, CPV::Opzx7Ks::KsMode::max, CPV::Opzx7Ks::KsMode::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::ksrMA7, 
			namePrefix + CPN::Fm::ksrMA7, 
			CPV::Opzx7Ks::KsMA7::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::kslMA7, 
			namePrefix + CPN::Fm::kslMA7, 
			CPV::Opzx7Ks::KslMA7::min, CPV::Opzx7Ks::KslMA7::max, CPV::Opzx7Ks::KslMA7::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksrOPZ, 
			namePrefix + CPN::Fm::ksrOPZ, 
			CPV::Opzx7Ks::KsrOPZ::min, CPV::Opzx7Ks::KsrOPZ::max, CPV::Opzx7Ks::KsrOPZ::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::kslOPZ, 
			namePrefix + CPN::Fm::kslOPZ, 
			CPV::Opzx7Ks::KslOPZ::min, CPV::Opzx7Ks::KslOPZ::max, CPV::Opzx7Ks::KslOPZ::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksBp, 
			namePrefix + CPN::Fm::ksBp, 
			CPV::Opzx7Ks::KsBp::min, CPV::Opzx7Ks::KsBp::max, CPV::Opzx7Ks::KsBp::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksLc, 
			namePrefix + CPN::Fm::ksLc, 
			CPV::Opzx7Ks::KsLc::min, CPV::Opzx7Ks::KsLc::max, CPV::Opzx7Ks::KsLc::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksRc, 
			namePrefix + CPN::Fm::ksRc, 
			CPV::Opzx7Ks::KsRc::min, CPV::Opzx7Ks::KsRc::max, CPV::Opzx7Ks::KsRc::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::ksLd, 
			namePrefix + CPN::Fm::ksLd, 
			CPV::Opzx7Ks::KsLd::min, CPV::Opzx7Ks::KsLd::max, CPV::Opzx7Ks::KsLd::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::ksRd, 
			namePrefix + CPN::Fm::ksRd, 
			CPV::Opzx7Ks::KsRd::min, CPV::Opzx7Ks::KsRd::max, CPV::Opzx7Ks::KsRd::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ksRs, 
			namePrefix + CPN::Fm::ksRs, 
			CPV::Opzx7Ks::KsRs::min, CPV::Opzx7Ks::KsRs::max, CPV::Opzx7Ks::KsRs::initial
		);

		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::sus, 
			namePrefix + CPN::Fm::sus, 
			CPV::Opzx7Rg::Sus::initial
		); // Sus (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::xof, 
			namePrefix + CPN::Fm::xof, 
			CPV::Opzx7Rg::Xof::initial
		); // Xof (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::kor, 
			namePrefix + CPN::Fm::kor, 
			CPV::Opzx7Rg::Kor::initial
		); // Kor (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::bypass, 
			namePrefix + CPN::Fm::bypass, 
			CPV::Opzx7Rg::Bypass::initial
		); // Bypass (Switch)
	}

	static inline void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::ar, 
			prefixName + CPN::Adsr::ar, 
			CPV::Adsr::Ar::min, CPV::Adsr::Ar::max, CPV::Adsr::Ar::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::dr, 
			prefixName + CPN::Adsr::dr, 
			CPV::Adsr::Dr::min, CPV::Adsr::Dr::max, CPV::Adsr::Dr::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::sl, 
			prefixName + CPN::Adsr::sl, 
			CPV::Adsr::Sl::min, CPV::Adsr::Sl::max, CPV::Adsr::Sl::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::rr, 
			prefixName + CPN::Adsr::rr, 
			CPV::Adsr::Rr::min, CPV::Adsr::Rr::max, CPV::Adsr::Rr::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::stl, 
			prefixName + CPN::Adsr::stl, 
			CPV::Adsr::Stl::min, CPV::Adsr::Stl::max, CPV::Adsr::Stl::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Adsr::kor, 
			prefixName + CPN::Adsr::kor, 
			CPV::Adsr::Kor::initial
		);
	}

	static inline void addOpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::ar, 
			namePrefix + CPN::Adsr::ar, 
			CPV::Adsr::Ar::min, CPV::Adsr::Ar::max, CPV::Adsr::Ar::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::dr, 
			namePrefix + CPN::Adsr::dr, 
			CPV::Adsr::Dr::min, CPV::Adsr::Dr::max, CPV::Adsr::Dr::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::sl, 
			namePrefix + CPN::Adsr::sl, 
			CPV::Adsr::Sl::min, CPV::Adsr::Sl::max, CPV::Adsr::Sl::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::rr, 
			namePrefix + CPN::Adsr::rr, 
			CPV::Adsr::Rr::min, CPV::Adsr::Rr::max, CPV::Adsr::Rr::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Adsr::stl, 
			namePrefix + CPN::Adsr::stl, 
			CPV::Adsr::Stl::min, CPV::Adsr::Stl::max, CPV::Adsr::Stl::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Adsr::kor, 
			namePrefix + CPN::Adsr::kor, 
			CPV::Adsr::Kor::initial
		);
	}

	static inline void addPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addPitchEnvRate(layout, prefix + CPK::PitchAdsr::ar, prefixName + CPN::PitchAdsr::ar);
		PrHelper::addPitchEnvRate(layout, prefix + CPK::PitchAdsr::dr, prefixName + CPN::PitchAdsr::dr);
		PrHelper::addPitchEnvRate(layout, prefix + CPK::PitchAdsr::rr, prefixName + CPN::PitchAdsr::rr);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::stl, prefixName + CPN::PitchAdsr::stl);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::atl, prefixName + CPN::PitchAdsr::atl);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::ssl, prefixName + CPN::PitchAdsr::ssl);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::rll, prefixName + CPN::PitchAdsr::rll);
	}

	static inline void addOpPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addPitchEnvRate(layout, prefix + CPK::PitchAdsr::ar, namePrefix + CPN::PitchAdsr::ar);
		PrHelper::addPitchEnvRate(layout, prefix + CPK::PitchAdsr::dr, namePrefix + CPN::PitchAdsr::dr);
		PrHelper::addPitchEnvRate(layout, prefix + CPK::PitchAdsr::rr, namePrefix + CPN::PitchAdsr::rr);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::stl, namePrefix + CPN::PitchAdsr::stl);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::atl, namePrefix + CPN::PitchAdsr::atl);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::ssl, namePrefix + CPN::PitchAdsr::ssl);
		PrHelper::addPitchEnvLevel(layout, prefix + CPK::PitchAdsr::rll, namePrefix + CPN::PitchAdsr::rll);
	}

	static inline void addSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv::steps, 
			prefixName + CPN::SsgSwEnv::steps, 
			CPV::SsgSwEnv::Steps::min, CPV::SsgSwEnv::Steps::max, CPV::SsgSwEnv::Steps::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwEnv::loop, 
			prefixName + CPN::SsgSwEnv::loop, 
			CPV::SsgSwEnv::Loop::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv::loopTo, 
			prefixName + CPN::SsgSwEnv::loopTo, 
			CPV::SsgSwEnv::LoopTo::min, CPV::SsgSwEnv::LoopTo::max, CPV::SsgSwEnv::LoopTo::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv::loopCount, 
			prefixName + CPN::SsgSwEnv::loopCount, 
			CPV::SsgSwEnv::LoopCount::min, CPV::SsgSwEnv::LoopCount::max, CPV::SsgSwEnv::LoopCount::initial
		);
		PrHelper::addSsgSwEnvSRLevel(layout, prefix + CPK::SsgSwEnv::stl, prefixName + CPN::SsgSwEnv::stl);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r1, prefixName + CPN::SsgSwEnv::r1);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l1, prefixName + CPN::SsgSwEnv::l1);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r2, prefixName + CPN::SsgSwEnv::r2);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l2, prefixName + CPN::SsgSwEnv::l2);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r3, prefixName + CPN::SsgSwEnv::r3);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l3, prefixName + CPN::SsgSwEnv::l3);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r4, prefixName + CPN::SsgSwEnv::r4);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l4, prefixName + CPN::SsgSwEnv::l4);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r5, prefixName + CPN::SsgSwEnv::r5);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l5, prefixName + CPN::SsgSwEnv::l5);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r6, prefixName + CPN::SsgSwEnv::r6);
		PrHelper::addSsgSwEnvSRLevel(layout, prefix + CPK::SsgSwEnv::l6, prefixName + CPN::SsgSwEnv::l6);
	}

	static inline void addOpSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addInt(
			layout,
			prefix + CPK::SsgSwEnv::steps, 
			namePrefix + CPN::SsgSwEnv::steps, 
			CPV::SsgSwEnv::Steps::min, CPV::SsgSwEnv::Steps::max, CPV::SsgSwEnv::Steps::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwEnv::loop, 
			namePrefix + CPN::SsgSwEnv::loop, 
			CPV::SsgSwEnv::Loop::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv::loopTo, 
			namePrefix + CPN::SsgSwEnv::loopTo, 
			CPV::SsgSwEnv::LoopTo::min, CPV::SsgSwEnv::LoopTo::max, CPV::SsgSwEnv::LoopTo::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv::loopCount, 
			namePrefix + CPN::SsgSwEnv::loopCount, 
			CPV::SsgSwEnv::LoopCount::min, CPV::SsgSwEnv::LoopCount::max, CPV::SsgSwEnv::LoopCount::initial
		);
		PrHelper::addSsgSwEnvSRLevel(layout, prefix + CPK::SsgSwEnv::stl, namePrefix + CPN::SsgSwEnv::stl);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r1, namePrefix + CPN::SsgSwEnv::r1);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l1, namePrefix + CPN::SsgSwEnv::l1);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r2, namePrefix + CPN::SsgSwEnv::r2);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l2, namePrefix + CPN::SsgSwEnv::l2);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r3, namePrefix + CPN::SsgSwEnv::r3);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l3, namePrefix + CPN::SsgSwEnv::l3);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r4, namePrefix + CPN::SsgSwEnv::r4);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l4, namePrefix + CPN::SsgSwEnv::l4);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r5, namePrefix + CPN::SsgSwEnv::r5);
		PrHelper::addSsgSwEnvLevel(layout, prefix + CPK::SsgSwEnv::l5, namePrefix + CPN::SsgSwEnv::l5);
		PrHelper::addSsgSwEnvRate(layout, prefix + CPK::SsgSwEnv::r6, namePrefix + CPN::SsgSwEnv::r6);
		PrHelper::addSsgSwEnvSRLevel(layout, prefix + CPK::SsgSwEnv::l6, namePrefix + CPN::SsgSwEnv::l6);
	}

	static inline void addSsgSwEnv11Parameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv11::steps, 
			prefixName + CPN::SsgSwEnv11::steps, 
			CPV::SsgSwEnv11::Steps::min, CPV::SsgSwEnv11::Steps::max, CPV::SsgSwEnv11::Steps::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwEnv11::loop, 
			prefixName + CPN::SsgSwEnv11::loop, 
			CPV::SsgSwEnv11::Loop::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv11::loopTo, 
			prefixName + CPN::SsgSwEnv11::loopTo, 
			CPV::SsgSwEnv11::LoopTo::min, CPV::SsgSwEnv11::LoopTo::max, CPV::SsgSwEnv11::LoopTo::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv11::loopCount, 
			prefixName + CPN::SsgSwEnv11::loopCount, 
			CPV::SsgSwEnv11::LoopCount::min, CPV::SsgSwEnv11::LoopCount::max, CPV::SsgSwEnv11::LoopCount::initial
		);
		PrHelper::addSsgSwEnv11SRLevel(layout, prefix + CPK::SsgSwEnv11::stl, prefixName + CPN::SsgSwEnv11::stl);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r1, prefixName + CPN::SsgSwEnv11::r1);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l1, prefixName + CPN::SsgSwEnv11::l1);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r2, prefixName + CPN::SsgSwEnv11::r2);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l2, prefixName + CPN::SsgSwEnv11::l2);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r3, prefixName + CPN::SsgSwEnv11::r3);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l3, prefixName + CPN::SsgSwEnv11::l3);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r4, prefixName + CPN::SsgSwEnv11::r4);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l4, prefixName + CPN::SsgSwEnv11::l4);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r5, prefixName + CPN::SsgSwEnv11::r5);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l5, prefixName + CPN::SsgSwEnv11::l5);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r6, prefixName + CPN::SsgSwEnv11::r6);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l6, prefixName + CPN::SsgSwEnv11::l6);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r7, prefixName + CPN::SsgSwEnv11::r7);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l7, prefixName + CPN::SsgSwEnv11::l7);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r8, prefixName + CPN::SsgSwEnv11::r8);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l8, prefixName + CPN::SsgSwEnv11::l8);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r9, prefixName + CPN::SsgSwEnv11::r9);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l9, prefixName + CPN::SsgSwEnv11::l9);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r10, prefixName + CPN::SsgSwEnv11::r10);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l10, prefixName + CPN::SsgSwEnv11::l10);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r11, prefixName + CPN::SsgSwEnv11::r11);
		PrHelper::addSsgSwEnv11SRLevel(layout, prefix + CPK::SsgSwEnv11::l11, prefixName + CPN::SsgSwEnv11::l11);
	}

	static inline void addOpSsgSwEnv11Parameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv11::steps, 
			namePrefix + CPN::SsgSwEnv11::steps, 
			CPV::SsgSwEnv11::Steps::min, CPV::SsgSwEnv11::Steps::max, CPV::SsgSwEnv11::Steps::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwEnv11::loop, 
			namePrefix + CPN::SsgSwEnv11::loop, 
			CPV::SsgSwEnv11::Loop::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv11::loopTo, 
			namePrefix + CPN::SsgSwEnv11::loopTo, 
			CPV::SsgSwEnv11::LoopTo::min, CPV::SsgSwEnv11::LoopTo::max, CPV::SsgSwEnv11::LoopTo::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwEnv11::loopCount, 
			namePrefix + CPN::SsgSwEnv11::loopCount, 
			CPV::SsgSwEnv11::LoopCount::min, CPV::SsgSwEnv11::LoopCount::max, CPV::SsgSwEnv11::LoopCount::initial
		);
		PrHelper::addSsgSwEnv11SRLevel(layout, prefix + CPK::SsgSwEnv11::stl, namePrefix + CPN::SsgSwEnv11::stl);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r1, namePrefix + CPN::SsgSwEnv11::r1);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l1, namePrefix + CPN::SsgSwEnv11::l1);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r2, namePrefix + CPN::SsgSwEnv11::r2);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l2, namePrefix + CPN::SsgSwEnv11::l2);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r3, namePrefix + CPN::SsgSwEnv11::r3);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l3, namePrefix + CPN::SsgSwEnv11::l3);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r4, namePrefix + CPN::SsgSwEnv11::r4);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l4, namePrefix + CPN::SsgSwEnv11::l4);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r5, namePrefix + CPN::SsgSwEnv11::r5);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l5, namePrefix + CPN::SsgSwEnv11::l5);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r6, namePrefix + CPN::SsgSwEnv11::r6);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l6, namePrefix + CPN::SsgSwEnv11::l6);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r7, namePrefix + CPN::SsgSwEnv11::r7);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l7, namePrefix + CPN::SsgSwEnv11::l7);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r8, namePrefix + CPN::SsgSwEnv11::r8);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l8, namePrefix + CPN::SsgSwEnv11::l8);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r9, namePrefix + CPN::SsgSwEnv11::r9);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l9, namePrefix + CPN::SsgSwEnv11::l9);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r10, namePrefix + CPN::SsgSwEnv11::r10);
		PrHelper::addSsgSwEnv11Level(layout, prefix + CPK::SsgSwEnv11::l10, namePrefix + CPN::SsgSwEnv11::l10);
		PrHelper::addSsgSwEnv11Rate(layout, prefix + CPK::SsgSwEnv11::r11, namePrefix + CPN::SsgSwEnv11::r11);
		PrHelper::addSsgSwEnv11SRLevel(layout, prefix + CPK::SsgSwEnv11::l11, namePrefix + CPN::SsgSwEnv11::l11);
	}

	static inline void addSsgSwPEnv11Parameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addInt(
			layout,
			prefix + CPK::SsgSwPEnv11::steps, 
			prefixName + CPN::SsgSwPEnv11::steps, 
			CPV::SsgSwPEnv11::Steps::min, CPV::SsgSwPEnv11::Steps::max, CPV::SsgSwPEnv11::Steps::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwPEnv11::loop, 
			prefixName + CPN::SsgSwPEnv11::loop, 
			CPV::SsgSwPEnv11::Loop::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwPEnv11::loopTo, 
			prefixName + CPN::SsgSwPEnv11::loopTo, 
			CPV::SsgSwPEnv11::LoopTo::min, CPV::SsgSwPEnv11::LoopTo::max, CPV::SsgSwPEnv11::LoopTo::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwPEnv11::loopCount, 
			prefixName + CPN::SsgSwPEnv11::loopCount, 
			CPV::SsgSwPEnv11::LoopCount::min, CPV::SsgSwEnv11::LoopCount::max, CPV::SsgSwEnv11::LoopCount::initial
		);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::stl, prefixName + CPN::SsgSwPEnv11::stl);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r1, prefixName + CPN::SsgSwPEnv11::r1);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l1, prefixName + CPN::SsgSwPEnv11::l1);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r2, prefixName + CPN::SsgSwPEnv11::r2);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l2, prefixName + CPN::SsgSwPEnv11::l2);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r3, prefixName + CPN::SsgSwPEnv11::r3);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l3, prefixName + CPN::SsgSwPEnv11::l3);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r4, prefixName + CPN::SsgSwPEnv11::r4);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l4, prefixName + CPN::SsgSwPEnv11::l4);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r5, prefixName + CPN::SsgSwPEnv11::r5);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l5, prefixName + CPN::SsgSwPEnv11::l5);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r6, prefixName + CPN::SsgSwPEnv11::r6);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l6, prefixName + CPN::SsgSwPEnv11::l6);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r7, prefixName + CPN::SsgSwPEnv11::r7);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l7, prefixName + CPN::SsgSwPEnv11::l7);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r8, prefixName + CPN::SsgSwPEnv11::r8);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l8, prefixName + CPN::SsgSwPEnv11::l8);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r9, prefixName + CPN::SsgSwPEnv11::r9);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l9, prefixName + CPN::SsgSwPEnv11::l9);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r10, prefixName + CPN::SsgSwPEnv11::r10);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l10, prefixName + CPN::SsgSwPEnv11::l10);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r11, prefixName + CPN::SsgSwPEnv11::r11);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l11, prefixName + CPN::SsgSwPEnv11::l11);
	}

	static inline void addOpSsgSwPEnv11Parameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwPEnv11::steps, 
			namePrefix + CPN::SsgSwPEnv11::steps, 
			CPV::SsgSwPEnv11::Steps::min, CPV::SsgSwPEnv11::Steps::max, CPV::SsgSwPEnv11::Steps::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwPEnv11::loop, 
			namePrefix + CPN::SsgSwPEnv11::loop, 
			CPV::SsgSwPEnv11::Loop::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwPEnv11::loopTo, 
			namePrefix + CPN::SsgSwPEnv11::loopTo, 
			CPV::SsgSwPEnv11::LoopTo::min, CPV::SsgSwPEnv11::LoopTo::max, CPV::SsgSwPEnv11::LoopTo::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgSwPEnv11::loopCount, 
			namePrefix + CPN::SsgSwPEnv11::loopCount, 
			CPV::SsgSwPEnv11::LoopCount::min, CPV::SsgSwPEnv11::LoopCount::max, CPV::SsgSwPEnv11::LoopCount::initial
		);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::stl, namePrefix + CPN::SsgSwPEnv11::stl);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r1, namePrefix + CPN::SsgSwPEnv11::r1);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l1, namePrefix + CPN::SsgSwPEnv11::l1);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r2, namePrefix + CPN::SsgSwPEnv11::r2);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l2, namePrefix + CPN::SsgSwPEnv11::l2);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r3, namePrefix + CPN::SsgSwPEnv11::r3);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l3, namePrefix + CPN::SsgSwPEnv11::l3);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r4, namePrefix + CPN::SsgSwPEnv11::r4);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l4, namePrefix + CPN::SsgSwPEnv11::l4);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r5, namePrefix + CPN::SsgSwPEnv11::r5);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l5, namePrefix + CPN::SsgSwPEnv11::l5);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r6, namePrefix + CPN::SsgSwPEnv11::r6);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l6, namePrefix + CPN::SsgSwPEnv11::l6);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r7, namePrefix + CPN::SsgSwPEnv11::r7);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l7, namePrefix + CPN::SsgSwPEnv11::l7);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r8, namePrefix + CPN::SsgSwPEnv11::r8);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l8, namePrefix + CPN::SsgSwPEnv11::l8);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r9, namePrefix + CPN::SsgSwPEnv11::r9);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l9, namePrefix + CPN::SsgSwPEnv11::l9);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r10, namePrefix + CPN::SsgSwPEnv11::r10);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l10, namePrefix + CPN::SsgSwPEnv11::l10);
		PrHelper::addSsgSwPenv11Rate(layout, prefix + CPK::SsgSwPEnv11::r11, namePrefix + CPN::SsgSwPEnv11::r11);
		PrHelper::addSsgSwPenv11Level(layout, prefix + CPK::SsgSwPEnv11::l11, namePrefix + CPN::SsgSwPEnv11::l11);
	}

	static inline void addOpmLfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::freq,
			prefixName + CPN::OpmLfo::freq, 
			CPV::OpmLfo::Freq::min, CPV::OpmLfo::Freq::max, CPV::OpmLfo::Freq::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::OpmLfo::amSmoothRatio, 
			prefixName + CPN::OpmLfo::amSmoothRatio, 
			CPV::OpmLfo::AmSmRt::min, CPV::OpmLfo::AmSmRt::max, CPV::OpmLfo::AmSmRt::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::pgShape, 
			prefixName + CPN::OpmLfo::pgShape, 
			CPV::OpmLfo::PgShape::min, CPV::OpmLfo::PgShape::max, CPV::OpmLfo::PgShape::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::egShape, 
			prefixName + CPN::OpmLfo::egShape, 
			CPV::OpmLfo::EgShape::min, CPV::OpmLfo::EgShape::max, CPV::OpmLfo::EgShape::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::OpmLfo::am, 
			prefixName + CPN::OpmLfo::am, 
			CPV::OpmLfo::Am::initial
		); // AM Enable (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::OpmLfo::pm, 
			prefixName + CPN::OpmLfo::pm, 
			CPV::OpmLfo::Pm::initial
		); // PM Enable (Switch)
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::pms, 
			prefixName + CPN::OpmLfo::pms, 
			CPV::OpmLfo::Pms::min, CPV::OpmLfo::Pms::max, CPV::OpmLfo::Pms::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::ams, 
			prefixName + CPN::OpmLfo::ams, 
			CPV::OpmLfo::Ams::min, CPV::OpmLfo::Ams::max, CPV::OpmLfo::Ams::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::pmd, 
			prefixName + CPN::OpmLfo::pmd, 
			CPV::OpmLfo::Pmd::min, CPV::OpmLfo::Pmd::max, CPV::OpmLfo::Pmd::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::amd, 
			prefixName + CPN::OpmLfo::amd, 
			CPV::OpmLfo::Amd::min, CPV::OpmLfo::Amd::max, CPV::OpmLfo::Amd::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpmLfo::syncDelay, 
			prefixName + CPN::OpmLfo::syncDelay, 
			CPV::OpmLfo::SyncDelay::min, CPV::OpmLfo::SyncDelay::max, CPV::OpmLfo::SyncDelay::initial
		);
	}

	static inline void addOpzx7LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::pmFreq, 
			prefixName + CPN::Opzx7Lfo::pmFreq, 
			CPV::Opzx7Lfo::PmFreq::min, CPV::Opzx7Lfo::PmFreq::max, CPV::Opzx7Lfo::PmFreq::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::amFreq, 
			prefixName + CPN::Opzx7Lfo::amFreq, 
			CPV::Opzx7Lfo::AmFreq::min, CPV::Opzx7Lfo::AmFreq::max, CPV::Opzx7Lfo::AmFreq::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::pmSyncDelay, 
			prefixName + CPN::Opzx7Lfo::pmSyncDelay, 
			CPV::Opzx7Lfo::SyncDelay::min, CPV::Opzx7Lfo::SyncDelay::max, CPV::Opzx7Lfo::SyncDelay::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::amSyncDelay, 
			prefixName + CPN::Opzx7Lfo::amSyncDelay, 
			CPV::Opzx7Lfo::SyncDelay::min, CPV::Opzx7Lfo::SyncDelay::max, CPV::Opzx7Lfo::SyncDelay::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::amSmoothRatio, 
			prefixName + CPN::Opzx7Lfo::amSmoothRatio, 
			CPV::Opzx7Lfo::AmSmRt::min, CPV::Opzx7Lfo::AmSmRt::max, CPV::Opzx7Lfo::AmSmRt::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::pgShape, 
			prefixName + CPN::Opzx7Lfo::pgShape, 
			CPV::Opzx7Lfo::PmShape::min, CPV::Opzx7Lfo::PmShape::max, CPV::Opzx7Lfo::PmShape::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::egShape, 
			prefixName + CPN::Opzx7Lfo::egShape, 
			CPV::Opzx7Lfo::AmShape::min, CPV::Opzx7Lfo::AmShape::max, CPV::Opzx7Lfo::AmShape::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Opzx7Lfo::am, 
			prefixName + CPN::Opzx7Lfo::am, 
			CPV::Opzx7Lfo::Am::initial
		); // AM Enable (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Opzx7Lfo::pm, 
			prefixName + CPN::Opzx7Lfo::pm, 
			CPV::Opzx7Lfo::Pm::initial
		); // PM Enable (Switch)
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::pms, 
			prefixName + CPN::Opzx7Lfo::pms, 
			CPV::Opzx7Lfo::Pms::min, CPV::Opzx7Lfo::Pms::max, CPV::Opzx7Lfo::Pms::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::ams, 
			prefixName + CPN::Opzx7Lfo::ams, 
			CPV::Opzx7Lfo::Ams::min, CPV::Opzx7Lfo::Ams::max, CPV::Opzx7Lfo::Ams::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::pmd, 
			prefixName + CPN::Opzx7Lfo::pmd, 
			CPV::Opzx7Lfo::Pmd::min, CPV::Opzx7Lfo::Pmd::max, CPV::Opzx7Lfo::Pmd::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::amd, 
			prefixName + CPN::Opzx7Lfo::amd, 
			CPV::Opzx7Lfo::Amd::min, CPV::Opzx7Lfo::Amd::max, CPV::Opzx7Lfo::Amd::initial
		);
	}

	static inline void addN88LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::freq, 
			prefixName + CPN::N88Lfo::freq, 
			CPV::N88Lfo::Freq::min, CPV::N88Lfo::Freq::max, CPV::N88Lfo::Freq::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::shape, 
			prefixName + CPN::N88Lfo::shape, 
			CPV::N88Lfo::Shape::min, CPV::N88Lfo::Shape::max, CPV::N88Lfo::Shape::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::N88Lfo::amSmoothRatio, 
			prefixName + CPN::N88Lfo::amSmoothRatio, 
			CPV::N88Lfo::AmSmRt::min, CPV::N88Lfo::AmSmRt::max, CPV::N88Lfo::AmSmRt::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::N88Lfo::am, 
			prefixName + CPN::N88Lfo::am, 
			CPV::N88Lfo::Am::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::N88Lfo::pm, 
			prefixName + CPN::N88Lfo::pm, 
			CPV::N88Lfo::Pm::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::pms, 
			prefixName + CPN::N88Lfo::pms, 
			CPV::N88Lfo::Pms::min, CPV::N88Lfo::Pms::max, CPV::N88Lfo::Pms::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::pmd, 
			prefixName + CPN::N88Lfo::pmd, 
			CPV::N88Lfo::Pmd::min, CPV::N88Lfo::Pmd::max, CPV::N88Lfo::Pmd::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::amd, 
			prefixName + CPN::N88Lfo::amd, 
			CPV::N88Lfo::Amd::min, CPV::N88Lfo::Amd::max, CPV::N88Lfo::Amd::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::syncDelay, 
			prefixName + CPN::N88Lfo::syncDelay, 
			CPV::N88Lfo::SyncDelay::min, CPV::N88Lfo::SyncDelay::max, CPV::N88Lfo::SyncDelay::initial
		);
	}

	static inline void addOpOplLfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::OplLfo::am, 
			namePrefix + CPN::OplLfo::am, 
			CPV::OplLfo::Am::initial
		); // AM Enable (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::OplLfo::vib, 
			namePrefix + CPN::OplLfo::vib, 
			CPV::OplLfo::Vib::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::OplLfo::ams, 
			namePrefix + CPN::OplLfo::ams, 
			CPV::OplLfo::Ams::min, CPV::OplLfo::Ams::max, CPV::OplLfo::Ams::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::OplLfo::amd, 
			namePrefix + CPN::OplLfo::amd, 
			CPV::OplLfo::Amd::min, CPV::OplLfo::Amd::max, CPV::OplLfo::Amd::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::OplLfo::pms, 
			namePrefix + CPN::OplLfo::pms, 
			CPV::OplLfo::Pms::min, CPV::OplLfo::Pms::max, CPV::OplLfo::Pms::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::OplLfo::pmd, 
			namePrefix + CPN::OplLfo::pmd, 
			CPV::OplLfo::Pmd::min, CPV::OplLfo::Pmd::max, CPV::OplLfo::Pmd::initial
		);
	}

	static inline void addOpOpmLfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::OpmLfo::amsEn, 
			namePrefix + CPN::OpmLfo::amsEn, 
			CPV::OpmLfo::AmsEn::initial
		);
	}

	static inline void addOpOpnaLfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpnaLfo::freqs, 
			namePrefix + CPN::OpnaLfo::freqs, 
			CPV::OpnaLfo::Freqs::min, CPV::OpnaLfo::Freqs::max, CPV::OpnaLfo::Freqs::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::OpnaLfo::am, 
			namePrefix + CPN::OpnaLfo::am, 
			CPV::OpnaLfo::Am::initial
		); // AM Enable (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::OpnaLfo::vib, 
			namePrefix + CPN::OpnaLfo::vib, 
			CPV::OpnaLfo::Vib::initial
		); // PM Enable (Switch)
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpnaLfo::pms, 
			namePrefix + CPN::OpnaLfo::pms, 
			CPV::OpnaLfo::Pms::min, CPV::OpnaLfo::Pms::max, CPV::OpnaLfo::Pms::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpnaLfo::syncDelay, 
			namePrefix + CPN::OpnaLfo::syncDelay, 
			CPV::OpnaLfo::SyncDelay::min, CPV::OpnaLfo::SyncDelay::max, CPV::OpnaLfo::SyncDelay::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::OpnaLfo::ams, 
			namePrefix + CPN::OpnaLfo::ams, 
			CPV::OpnaLfo::Ams::min, CPV::OpnaLfo::Ams::max, CPV::OpnaLfo::Ams::initial
		);
	}

	static inline void addOpOpzx7LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::pmFreq, 
			namePrefix + CPN::Opzx7Lfo::pmFreq, 
			CPV::Opzx7Lfo::PmFreq::min, CPV::Opzx7Lfo::PmFreq::max, CPV::Opzx7Lfo::PmFreq::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::amFreq, 
			namePrefix + CPN::Opzx7Lfo::amFreq, 
			CPV::Opzx7Lfo::PmFreq::min, CPV::Opzx7Lfo::PmFreq::max, CPV::Opzx7Lfo::PmFreq::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::pmSyncDelay, 
			namePrefix + CPN::Opzx7Lfo::pmSyncDelay, 
			CPV::Opzx7Lfo::SyncDelay::min, CPV::Opzx7Lfo::SyncDelay::max, CPV::Opzx7Lfo::SyncDelay::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::amSyncDelay, 
			namePrefix + CPN::Opzx7Lfo::amSyncDelay, 
			CPV::Opzx7Lfo::SyncDelay::min, CPV::Opzx7Lfo::SyncDelay::max, CPV::Opzx7Lfo::SyncDelay::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::amSmoothRatio, 
			namePrefix + CPN::Opzx7Lfo::amSmoothRatio, 
			CPV::Opzx7Lfo::AmSmRt::min, CPV::Opzx7Lfo::AmSmRt::max, CPV::Opzx7Lfo::AmSmRt::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::pgShape, 
			namePrefix + CPN::Opzx7Lfo::pgShape, 
			CPV::Opzx7Lfo::PmShape::min, CPV::Opzx7Lfo::PmShape::max, CPV::Opzx7Lfo::PmShape::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Opzx7Lfo::egShape, 
			namePrefix + CPN::Opzx7Lfo::egShape, 
			CPV::Opzx7Lfo::AmShape::min, CPV::Opzx7Lfo::AmShape::max, CPV::Opzx7Lfo::AmShape::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Opzx7Lfo::am, 
			namePrefix + CPN::Opzx7Lfo::am, 
			CPV::Opzx7Lfo::Am::initial
		); // AM Enable (Switch)
		PrHelper::addBool(
			layout, 
			prefix + CPK::Opzx7Lfo::pm, 
			namePrefix + CPN::Opzx7Lfo::pm, 
			CPV::Opzx7Lfo::Pm::initial
		); // PM Enable (Switch)
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::pms, 
			namePrefix + CPN::Opzx7Lfo::pms, 
			CPV::Opzx7Lfo::Pms::min, CPV::Opzx7Lfo::Pms::max, CPV::Opzx7Lfo::Pms::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::ams, 
			namePrefix + CPN::Opzx7Lfo::ams, 
			CPV::Opzx7Lfo::Ams::min, CPV::Opzx7Lfo::Ams::max, CPV::Opzx7Lfo::Ams::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::pmd, 
			namePrefix + CPN::Opzx7Lfo::pmd, 
			CPV::Opzx7Lfo::Pmd::min, CPV::Opzx7Lfo::Pmd::max, CPV::Opzx7Lfo::Pmd::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Opzx7Lfo::amd, 
			namePrefix + CPN::Opzx7Lfo::amd, 
			CPV::Opzx7Lfo::Amd::min, CPV::Opzx7Lfo::Amd::max, CPV::Opzx7Lfo::Amd::initial
		);
	}

	static inline void addOpN88LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::N88Lfo::ams, 
			namePrefix + CPN::N88Lfo::ams, 
			CPV::N88Lfo::Ams::min, CPV::N88Lfo::Ams::max, CPV::N88Lfo::Ams::initial
		);
	}

	static inline void addUnisonParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		// ユニゾン・ハーモニー用
		PrHelper::addInt(
			layout, 
			prefix + CPK::Unison::voices, 
			prefixName + CPN::Unison::voices, 
			CPV::Unison::Voices::min, CPV::Unison::Voices::max, CPV::Unison::Voices::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Unison::detune, 
			prefixName + CPN::Unison::detune, 
			CPV::Unison::Detune::min, CPV::Unison::Detune::max, CPV::Unison::Detune::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Unison::spread, 
			prefixName + CPN::Unison::spread, 
			CPV::Unison::Spread::min, CPV::Unison::Spread::max, CPV::Unison::Spread::initial
		);
		PrHelper::addBool(
			layout,
			prefix + CPK::Unison::arpEnable,
			prefixName + CPN::Unison::arpEnable,
			CPV::Unison::ArpEnable::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::Unison::arpFreq,
			prefixName + CPN::Unison::arpFreq,
			CPV::Unison::ArpFreq::min, CPV::Unison::ArpFreq::max, CPV::Unison::ArpFreq::initial
		);
		PrHelper::addBool(
			layout,
			prefix + CPK::Unison::arpSmooth,
			prefixName + CPN::Unison::arpSmooth,
			CPV::Unison::ArpSmooth::initial
		);

		// ボイス単位の設定 (ボイス0はメインなので 1〜7 のみ)
		for (int i = 0; i < Global::unisonParaVoices; ++i) {
			const juce::String no = juce::String(i + 1);
			PrHelper::addFloat(
				layout,
				prefix + CPK::Unison::paraDistance + no,
				prefixName + CPN::Unison::paraDistance + no,
				CPV::Unison::ParaDistance::min, CPV::Unison::ParaDistance::max, CPV::Unison::ParaDistance::initial
			);
			PrHelper::addInt(
				layout,
				prefix + CPK::Unison::paraDetune + no,
				prefixName + CPN::Unison::paraDetune + no,
				CPV::Unison::ParaDetune::min, CPV::Unison::ParaDetune::max, CPV::Unison::ParaDetune::initial
			);
		}
	}

	static inline void addEnvBypassParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::adsr + CPK::bypass, 
			prefixName + CPN::Adsr::bypass, 
			CPV::Adsr::Bypass::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::pitchAdsr + CPK::bypass, 
			prefixName + CPN::PitchAdsr::bypass, 
			CPV::PitchAdsr::Bypass::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::ssgSwEnv + CPK::bypass, 
			prefixName + CPN::SsgSwEnv::bypass, 
			CPV::SsgSwEnv::Bypass::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::ssgSwEnv11 + CPK::bypass, 
			prefixName + CPN::SsgSwEnv11::bypass, 
			CPV::SsgSwEnv11::Bypass::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::ssgSwPEnv11 + CPK::bypass, 
			prefixName + CPN::SsgSwPEnv11::bypass, 
			CPV::SsgSwPEnv11::Bypass::initial
		);
	}

	// AMP ENV のバイパスだけを、初期値を指定して登録する。
	// FM 音源はオペレータごとに独自のエンベロープを持っているため、
	// チップ全体へ掛ける AMP ENV は既定でバイパスにしておく。
	// そうしないと既存のプリセットの鳴りが変わってしまう。
	static inline void addAdsrBypassParameter(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, bool initial) {
		PrHelper::addBool(
			layout,
			prefix + CPK::adsr + CPK::bypass,
			prefixName + CPN::Adsr::bypass,
			initial
		);
	}

	// SSG SW PENV11 のバイパスだけを、初期値を指定して登録する。
	// FM 音源はオペレータごとに同じものを持っているため、チップ全体へ
	// 掛けるぶんは既定でバイパスにしておく。
	static inline void addSsgSwPEnv11BypassParameter(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, bool initial) {
		PrHelper::addBool(
			layout,
			prefix + CPK::ssgSwPEnv11 + CPK::bypass,
			prefixName + CPN::SsgSwPEnv11::bypass,
			initial
		);
	}

	static inline void addSsgSwEnv11BypassParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout,
			prefix + CPK::ssgSwEnv11 + CPK::bypass,
			prefixName + CPN::SsgSwEnv11::bypass,
			CPV::SsgSwEnv11::Bypass::initial
		);
	}

	static inline void addOpEnvEnableParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::PitchAdsr::enable, 
			namePrefix + CPN::PitchAdsr::enable, 
			CPV::PitchAdsr::Enable::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwEnv::enable, 
			namePrefix + CPN::SsgSwEnv::enable, 
			CPV::SsgSwEnv::Enable::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwEnv11::enable, 
			namePrefix + CPN::SsgSwEnv11::enable, 
			CPV::SsgSwEnv11::Enable::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgSwPEnv11::enable, 
			namePrefix + CPN::SsgSwPEnv11::enable, 
			CPV::SsgSwPEnv11::Enable::initial
		);
	}

	static inline void addOpOplDetuneParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::mul, 
			namePrefix + CPN::mul, 
			CPV::OplDetune::Mul::min, CPV::OplDetune::Mul::max, CPV::OplDetune::Mul::initial
		);
	}

	static inline void addOpOpmDetuneParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::mul, 
			namePrefix + CPN::mul, 
			CPV::OpmDetune::Mul::min, CPV::OpmDetune::Mul::max, CPV::OpmDetune::Mul::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::mulRatio, 
			namePrefix + CPN::mulRatio, 
			CPV::OpmDetune::MulRatio::min, CPV::OpmDetune::MulRatio::max, CPV::OpmDetune::MulRatio::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::dt, 
			namePrefix + CPN::dt1, 
			CPV::OpmDetune::Dt1::min, CPV::OpmDetune::Dt1::max, CPV::OpmDetune::Dt1::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::dt2, 
			namePrefix + CPN::dt2, 
			CPV::OpmDetune::Dt2::min, CPV::OpmDetune::Dt2::max, CPV::OpmDetune::Dt2::initial
		);
	}

	static inline void addOpOpnDetuneParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::mul, 
			namePrefix + CPN::mul, 
			CPV::OpnDetune::Mul::min, CPV::OpnDetune::Mul::max, CPV::OpnDetune::Mul::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::dt, 
			namePrefix + CPN::dt, 
			CPV::OpnDetune::Dt::min, CPV::OpnDetune::Dt::max, CPV::OpnDetune::Dt::initial
		);
	}

	static inline void addOpzx7DetuneParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		// Detune
		PrHelper::addInt(
			layout, 
			prefix + CPK::mul, 
			prefixName + CPN::mul, 
			CPV::Opzx7Detune::Mul::min, CPV::Opzx7Detune::Mul::max, CPV::Opzx7Detune::Mul::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::mulRatio, 
			prefixName + CPN::mulRatio, 
			CPV::Opzx7Detune::MulRatio::min, CPV::Opzx7Detune::MulRatio::max, CPV::Opzx7Detune::MulRatio::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::dt, 
			prefixName + CPN::dt1, 
			CPV::Opzx7Detune::Dt1::min, CPV::Opzx7Detune::Dt1::max, CPV::Opzx7Detune::Dt1::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::dt2, 
			prefixName + CPN::dt2, 
			CPV::Opzx7Detune::Dt2::min, CPV::Opzx7Detune::Dt2::max, CPV::Opzx7Detune::Dt2::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::dt3, 
			prefixName + CPN::dt3, 
			CPV::Opzx7Detune::Dt3::min, CPV::Opzx7Detune::Dt3::max, CPV::Opzx7Detune::Dt3::initial
		);
	}

	static inline void addOpOplEgParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::eg, 
			namePrefix + CPN::Fm::eg, 
			CPV::OplEg::min, CPV::OplEg::max, CPV::OplEg::initial
		);
	}

	static inline void addOpOpl3EgParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::eg, 
			namePrefix + CPN::Fm::eg, 
			CPV::Opl3Eg::min, CPV::Opl3Eg::max, CPV::Opl3Eg::initial
		);
	}

	static inline void addOpOpmSeParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::seFreq, 
			namePrefix + CPN::Fm::seFreq, 
			CPV::OpmSeFreq::min, CPV::OpmSeFreq::max, CPV::OpmSeFreq::initial
		);
	}

	static inline void addOpOpnaSeParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::se, 
			namePrefix + CPN::Fm::se, 
			CPV::OpnaSe::Se::min, CPV::OpnaSe::Se::max, CPV::OpnaSe::Se::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Fm::seFreq, 
			namePrefix + CPN::Fm::seFreq, 
			CPV::OpnaSe::Freq::min, CPV::OpnaSe::Freq::max, CPV::OpnaSe::Freq::initial
		);
	}

	static inline void addOpOpzx7WsParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::ws, 
			namePrefix + CPN::Fm::ws, 
			CPV::Opzx7Ws::min, CPV::Opzx7Ws::max, CPV::Opzx7Ws::initial
		);
	}

	static inline void addFixParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		PrHelper::addBool(
			layout, 
			prefix + CPK::fix, 
			prefixName + CPN::fix, 
			CPV::FixMode::Enable::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::fixFreq, 
			prefixName + CPN::fixFreq, 
			CPV::FixMode::Freq::min, 
			CPV::FixMode::Freq::max, 
			CPV::FixMode::Freq::initial
		);
	}

	static inline void addOpFixParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
	{
		PrHelper::addBool(
			layout, 
			prefix + CPK::fix, 
			namePrefix + CPN::fix, 
			CPV::FixMode::Enable::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::fixFreq, 
			namePrefix + CPN::fixFreq, 
			CPV::FixMode::Freq::min, CPV::FixMode::Freq::max, CPV::FixMode::Freq::initial
		);
	}

	static inline void addAlgFbParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, int maxAlg, int initAlg) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::alg, 
			prefixName + CPN::Fm::alg, 
			CPV::Alg::min, maxAlg, initAlg
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Fm::fb, 
			prefixName + CPN::Fm::fb, 
			CPV::Fb::min, CPV::Fb::max, CPV::Fb::initial
		);
	}

	static inline void addOpzx7AlgFbParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, int maxAlg, int initAlg) {
		PrHelper::addInt(
			layout,
			prefix + CPK::Fm::alg,
			prefixName + CPN::Fm::alg,
			CPV::Alg::min, maxAlg, initAlg
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb1,
			prefixName + CPN::Fm::fb1,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb2,
			prefixName + CPN::Fm::fb2,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb3,
			prefixName + CPN::Fm::fb3,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb4,
			prefixName + CPN::Fm::fb4,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb5,
			prefixName + CPN::Fm::fb5,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb6,
			prefixName + CPN::Fm::fb6,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb7,
			prefixName + CPN::Fm::fb7,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::Fm::fb8,
			prefixName + CPN::Fm::fb8,
			CPV::Opzx7Fb::min, CPV::Opzx7Fb::max, CPV::Opzx7Fb::initial
		);
	}

	static inline void addQualityParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::Quality::bit, 
			prefixName + CPN::Quality::bit, 
			CPV::Quality::Bit::min, CPV::Quality::Bit::max, CPV::Quality::Bit::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::Quality::rate, 
			prefixName + CPN::Quality::rate, 
			CPV::Quality::Rate::min, CPV::Quality::Rate::max, CPV::Quality::Rate::initial
		);
	}

	static inline void addQualityPcmParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::QualityPcm::mode, 
			prefixName + CPN::QualityPcm::bit, 
			CPV::QualityPcm::Bit::min, CPV::QualityPcm::Bit::max, CPV::QualityPcm::Bit::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::QualityPcm::rate, 
			prefixName + CPN::QualityPcm::rate, 
			CPV::QualityPcm::Rate::min, CPV::QualityPcm::Rate::max, CPV::QualityPcm::Rate::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::QualityPcm::interp, 
			prefixName + CPN::QualityPcm::interp, 
			CPV::QualityPcm::Interp::min, CPV::QualityPcm::Interp::max, CPV::QualityPcm::Interp::initial
		);
	}

	static inline void addOpMaskParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::Fm::mask, 
			namePrefix + CPN::Fm::mask, 
			CPV::OpMask::initial
		); // OP Mask (Switch)
	}

	static inline void addPcmParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addFloat(
			layout, 
			prefix + CPK::pcmOffset, 
			prefixName + CPN::pcmOffset, 
			CPV::Pcm::Offset::min, CPV::Pcm::Offset::max, CPV::Pcm::Offset::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::pcmRatio, 
			prefixName + CPN::pcmRatio, 
			CPV::Pcm::Ratio::min, CPV::Pcm::Ratio::max, CPV::Pcm::Ratio::initial
		);
	}

	static inline void addLPParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::lpEnable, 
			prefixName + CPN::lpEnable, 
			CPV::Lp::Enable::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::lpStart, 
			prefixName + CPN::lpStart, 
			CPV::Lp::Start::min, CPV::Lp::Start::max, CPV::Lp::Start::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::lpEnd, 
			prefixName + CPN::lpEnd, 
			CPV::Lp::End::min, CPV::Lp::End::max, CPV::Lp::End::initial
		);
	}

	static inline void addOpPcmParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addFloat(
			layout, 
			prefix + CPK::pcmOffset, 
			namePrefix + CPN::pcmOffset, 
			CPV::Pcm::Offset::min, CPV::Pcm::Offset::max, CPV::Pcm::Offset::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::pcmRatio, 
			namePrefix + CPN::pcmRatio, 
			CPV::Pcm::Ratio::min, CPV::Pcm::Ratio::max, CPV::Pcm::Ratio::initial
		);
	}

	static inline void addOpLPParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::lpEnable, 
			namePrefix + CPN::lpEnable, 
			CPV::Lp::Enable::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::lpStart, 
			namePrefix + CPN::lpStart, 
			CPV::Lp::Start::min, CPV::Lp::Start::max, CPV::Lp::Start::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::lpEnd, 
			namePrefix + CPN::lpEnd, 
			CPV::Lp::End::min, CPV::Lp::End::max, CPV::Lp::End::initial
		);
	}

	static inline void addOpmPanParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::pan, 
			prefixName + CPN::pan, 
			CPV::OpmPan::min, CPV::OpmPan::max, CPV::OpmPan::initial
		);
	}

	static inline void addOpnaPanParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::pan, 
			prefixName + CPN::pan, 
			CPV::OpnaPan::min, CPV::OpnaPan::max, CPV::OpnaPan::initial
		);
	}

	static inline void addOpzx7PanpotParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::panpot_en, 
			prefixName + CPN::panpot_en, 
			CPV::Panpot::Enable::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::panpot, 
			prefixName + CPN::panpot, 
			CPV::Panpot::Panpot::min, CPV::Panpot::Panpot::max, CPV::Panpot::Panpot::initial
		);
	}

	static inline void addAdpcmPanParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addFloat(
			layout, 
			prefix + CPK::pan, 
			prefixName + CPN::pan, 
			CPV::PcmPan::min, CPV::PcmPan::max, CPV::PcmPan::initial
		);
	}

	static inline void addTnParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Tn::tone, 
			prefixName + CPN::Tn::tone, 
			CPV::Tn::Tone::min, CPV::Tn::Tone::max, CPV::Tn::Tone::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Tn::noise, 
			prefixName + CPN::Tn::noise, 
			CPV::Tn::Noise::min, CPV::Tn::Noise::max, CPV::Tn::Noise::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Tn::freq, 
			prefixName + CPN::Tn::freq, 
			CPV::Tn::Freq::min, CPV::Tn::Freq::max, CPV::Tn::Freq::initial
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::Tn::nON, 
			prefixName + CPN::Tn::nON, 
			CPV::Tn::NON::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::Tn::mix, 
			prefixName + CPN::Tn::mix, 
			CPV::Tn::Mix::min, CPV::Tn::Mix::max, CPV::Tn::Mix::initial
		);
	}

	static inline void addAdpcmBasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::loop, 
			prefixName + CPN::loop, 
			CPV::Loop::initial
		);
	}

	static inline void addRhythmPadBasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, int note) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::note, 
			prefix + CPN::note, 
			CPV::Note::min, CPV::Note::max, note
		);
		PrHelper::addBool(
			layout, 
			prefix + CPK::oneShot, 
			prefixName + CPN::oneShot, 
			CPV::OneShot::initial
		);
	}

	static inline void addWtModParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::WtMod::enable, 
			prefixName + CPN::WtMod::enable, 
			CPV::WtMod::Enable::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::WtMod::depth, 
			prefixName + CPN::WtMod::depth, 
			CPV::WtMod::Depth::min, CPV::WtMod::Depth::max, CPV::WtMod::Depth::initial
		);
		PrHelper::addFloat(
			layout, 
			prefix + CPK::WtMod::speed, 
			prefixName + CPN::WtMod::speed, 
			CPV::WtMod::Speed::min, CPV::WtMod::Speed::max, CPV::WtMod::Speed::initial
		);
		PrHelper::addInt(
			layout, 
			prefix + CPK::WtMod::shape, 
			prefixName + CPN::WtMod::shape, 
			CPV::WtMod::Shape::min, CPV::WtMod::Shape::max, CPV::WtMod::Shape::initial
		);
		// 32 点へ落とすときの方法 (音声側では使わず、読み込み時にだけ参照する)
		PrHelper::addBool(
			layout, 
			prefix + CPK::WtMod::waveSmooth, 
			prefixName + CPN::WtMod::waveSmooth, 
			CPV::WtMod::WaveSmooth::initial
		);
		// HuC6280 モードで使う変調波形スロットの番号。
		// 波形そのものはプロセッサが持つので、ここには出てこない。
		PrHelper::addInt(
			layout,
			prefix + CPK::WtMod::waveSlot,
			prefixName + CPN::WtMod::waveSlot,
			CPV::WtMod::WaveSlot::min, CPV::WtMod::WaveSlot::max, CPV::WtMod::WaveSlot::initial
		);
		// FdsUser モード用の変調テーブル (32 エントリ / 3bit のレジスタ値)。
		// 初期値は FdsMod の対称三角テーブル。
		for (int i = 0; i < CPV::WtMod::FdsTable::size; ++i)
		{
			PrHelper::addInt(
				layout,
				prefix + CPK::WtMod::fdsTable + juce::String(i),
				prefixName + CPN::WtMod::fdsTable + juce::String(i),
				CPV::WtMod::FdsTable::min, CPV::WtMod::FdsTable::max, FdsMod::tables[0][i]
			);
		}
	}

	// WT AMP MOD。中身はピッチ版と同じ並びで、MIN / MAX だけが増える。
	static inline void addWtAmpModParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout,
			prefix + CPK::WtAmpMod::enable,
			prefixName + CPN::WtAmpMod::enable,
			CPV::WtAmpMod::Enable::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::WtAmpMod::depth,
			prefixName + CPN::WtAmpMod::depth,
			CPV::WtAmpMod::Depth::min, CPV::WtAmpMod::Depth::max, CPV::WtAmpMod::Depth::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::WtAmpMod::speed,
			prefixName + CPN::WtAmpMod::speed,
			CPV::WtAmpMod::Speed::min, CPV::WtAmpMod::Speed::max, CPV::WtAmpMod::Speed::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::WtAmpMod::shape,
			prefixName + CPN::WtAmpMod::shape,
			CPV::WtAmpMod::Shape::min, CPV::WtAmpMod::Shape::max, CPV::WtAmpMod::Shape::initial
		);
		// 出力の下端と上端。波形スロットをまたいで 1 組だけ持つ。
		PrHelper::addFloat(
			layout,
			prefix + CPK::WtAmpMod::min,
			prefixName + CPN::WtAmpMod::min,
			CPV::WtAmpMod::Min::min, CPV::WtAmpMod::Min::max, CPV::WtAmpMod::Min::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::WtAmpMod::max,
			prefixName + CPN::WtAmpMod::max,
			CPV::WtAmpMod::Max::min, CPV::WtAmpMod::Max::max, CPV::WtAmpMod::Max::initial
		);
		PrHelper::addBool(
			layout,
			prefix + CPK::WtAmpMod::waveSmooth,
			prefixName + CPN::WtAmpMod::waveSmooth,
			CPV::WtAmpMod::WaveSmooth::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::WtAmpMod::waveSlot,
			prefixName + CPN::WtAmpMod::waveSlot,
			CPV::WtAmpMod::WaveSlot::min, CPV::WtAmpMod::WaveSlot::max, CPV::WtAmpMod::WaveSlot::initial
		);
		for (int i = 0; i < CPV::WtAmpMod::FdsTable::size; ++i)
		{
			PrHelper::addInt(
				layout,
				prefix + CPK::WtAmpMod::fdsTable + juce::String(i),
				prefixName + CPN::WtAmpMod::fdsTable + juce::String(i),
				CPV::WtAmpMod::FdsTable::min, CPV::WtAmpMod::FdsTable::max, FdsMod::tables[0][i]
			);
		}
	}

	static inline void addWtBasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		// Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt::wave, 
			prefixName + CPN::Wt::waveform, 
			CPV::Wt::WaveForm::min, CPV::Wt::WaveForm::max, CPV::Wt::WaveForm::initial
		);
		// Size: 0:32, 1:64, 2:128, 3:256
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt::sampleSize, 
			prefixName + CPN::Wt::sampleSize, 
			CPV::Wt::SammpleSize::min, CPV::Wt::SammpleSize::max, CPV::Wt::SammpleSize::initial
		);
		// Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt::steps, 
			prefixName + CPN::Wt::steps, 
			CPV::Wt::Steps::min, CPV::Wt::Steps::max, CPV::Wt::Steps::initial
		);
		// 波形テーブルの読み出しを線形補間するかどうか
		PrHelper::addBool(
			layout, 
			prefix + CPK::Wt::interpolate, 
			prefixName + CPN::Wt::interpolate, 
			CPV::Wt::Interpolate::initial
		);
	}

	static inline void addWtPlusBasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		// 鳴らす波形メモリのスロット。オートメーションで振れる。
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt::slot, 
			prefixName + CPN::Wt::slot, 
			CPV::Wt::Slot::min, CPV::Wt::Slot::max, CPV::Wt::Slot::initial
		);
		// Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt::steps, 
			prefixName + CPN::Wt::steps, 
			CPV::Wt::Steps::min, CPV::Wt::Steps::max, CPV::Wt::Steps::initial
		);
		// 波形テーブルの読み出しを線形補間するかどうか
		PrHelper::addBool(
			layout, 
			prefix + CPK::Wt::interpolate, 
			prefixName + CPN::Wt::interpolate, 
			CPV::Wt::Interpolate::initial
		);
	}

	static inline void addWt2BasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		// Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt2::wave, 
			prefixName + CPN::Wt2::waveform, 
			CPV::Wt2::WaveForm::min, CPV::Wt2::WaveForm::max, CPV::Wt2::WaveForm::initial
		);
		// Size: 0:32, 1:64, 2:128, 3:256
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt2::sampleSize, 
			prefixName + CPN::Wt2::sampleSize, 
			CPV::Wt2::SammpleSize::min, CPV::Wt2::SammpleSize::max, CPV::Wt2::SammpleSize::initial
		);
		// Resolution : 0:16, 1:32, 2:64, 3:128, 4:256
		PrHelper::addInt(
			layout, 
			prefix + CPK::Wt2::resolution, 
			prefixName + CPN::Wt2::resolution, 
			CPV::Wt2::Resolution::min, CPV::Wt2::Resolution::max, CPV::Wt2::Resolution::initial
		);
		// 波形テーブルの読み出しを線形補間するかどうか
		PrHelper::addBool(
			layout, 
			prefix + CPK::Wt::interpolate, 
			prefixName + CPN::Wt::interpolate, 
			CPV::Wt::Interpolate::initial
		);
	}

	static inline void createWtCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& prefix, const juce::String& name)
	{
		for (int i = 0; i < size; ++i)
		{
			auto paramId = prefix + juce::String(i);
			auto paramName = name + juce::String(i);
			PrHelper::addFloat(
				layout,
				paramId,
				paramName,
				CPV::Wt::CustomValue::min, CPV::Wt::CustomValue::max, CPV::Wt::CustomValue::initial
			);
		}
	}

	template <size_t I>
	static inline void processWtCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix)
	{
		// Custom Wave params reading
		for (int i = 0; i < I; ++i)
		{
			auto paramId = prefix + juce::String(i);
			samples[i] = *apvts.getRawParameterValue(paramId);
		}

	}

	static inline void createWt2CustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& prefix, const juce::String& name)
	{
		for (int i = 0; i < size; ++i)
		{
			auto paramId = prefix + juce::String(i);
			auto paramName = name + juce::String(i);
			PrHelper::addInt(
				layout,
				paramId,
				paramName,
				CPV::Wt2::CustomValue::min, CPV::Wt2::CustomValue::max, CPV::Wt2::CustomValue::initial
			);
		}
	}

	template <size_t I>
	static inline void processWt2CustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix)
	{
		// Custom Wave params reading
		for (int i = 0; i < I; ++i)
		{
			auto paramId = prefix + juce::String(i);
			samples[i] = (int)*apvts.getRawParameterValue(paramId);
		}

	}

	static inline void addWtCustomParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::createWtCustomWaveLayout(layout, CPV::Wt::CustomSize32, prefix + CPK::custom32, prefixName + CPN::custom32);
		PrHelper::createWtCustomWaveLayout(layout, CPV::Wt::CustomSize64, prefix + CPK::custom64, prefixName + CPN::custom64);
		PrHelper::createWtCustomWaveLayout(layout, CPV::Wt::CustomSize128, prefix + CPK::custom128, prefixName + CPN::custom128);
		PrHelper::createWtCustomWaveLayout(layout, CPV::Wt::CustomSize256, prefix + CPK::custom256, prefixName + CPN::custom256);
	}

	static inline void addWt2CustomParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::createWt2CustomWaveLayout(layout, CPV::Wt::CustomSize32, prefix + CPK::custom32, prefixName + CPN::custom32);
		PrHelper::createWt2CustomWaveLayout(layout, CPV::Wt::CustomSize64, prefix + CPK::custom64, prefixName + CPN::custom64);
		PrHelper::createWt2CustomWaveLayout(layout, CPV::Wt::CustomSize128, prefix + CPK::custom128, prefixName + CPN::custom128);
		PrHelper::createWt2CustomWaveLayout(layout, CPV::Wt::CustomSize256, prefix + CPK::custom256, prefixName + CPN::custom256);
	}

	static inline void addSsgBasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::ssgWaveform, 
			prefixName + CPN::ssgWaveform, 
			CPV::SsgWaveForm::min, CPV::SsgWaveForm::max, CPV::SsgWaveForm::initial
		); // 0:Pulse, 1:Triangle
	}

	static inline void addSsgDutyParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgDuty::mode, 
			prefixName + CPN::SsgDuty::mode, 
			CPV::SsgDuty::Mode::min, CPV::SsgDuty::Mode::max, CPV::SsgDuty::Mode::initial
		); // Duty Mode: 0=Preset, 1=Variable
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgDuty::preset, 
			prefixName + CPN::SsgDuty::preset, 
			CPV::SsgDuty::Preset::min, CPV::SsgDuty::Preset::max, CPV::SsgDuty::Preset::initial
		); // Preset: 0~8
		PrHelper::addFloat(
			layout, 
			prefix + CPK::SsgDuty::var, 
			prefixName + CPN::SsgDuty::var, 
			CPV::SsgDuty::Var::min, CPV::SsgDuty::Var::max, CPV::SsgDuty::Var::initial
		); // Variable: 0.0 ~ 0.5
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgDuty::inv, 
			prefixName + CPN::SsgDuty::invert, 
			CPV::SsgDuty::Inv::initial
		); // Invert
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgDuty::fc, 
			prefixName + CPN::SsgDuty::fc, 
			CPV::SsgDuty::Fc::initial
		); // Famicom Mode
		PrHelper::addFloat(
			layout, 
			prefix + CPK::SsgDuty::fcFluc, 
			prefixName + CPN::SsgDuty::fcFluc, 
			CPV::SsgDuty::FcFluc::min, CPV::SsgDuty::FcFluc::max, CPV::SsgDuty::FcFluc::initial
		); // Famicom Mode Fluctuation: 0.0 ~ 1.0
	}

	static inline void addSsgTriParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgTri::keyTrk, 
			prefixName + CPN::SsgTri::keyTrack, 
			CPV::SsgTri::KeyTrack::initial
		); // Key Track Switch (Default ON)
		PrHelper::addFloat(
			layout, 
			prefix + CPK::SsgTri::peak, 
			prefixName + CPN::SsgTri::peak, 
			CPV::SsgTri::Peak::min, CPV::SsgTri::Peak::max, CPV::SsgTri::Peak::initial
		); // Triangle Peak: 0.0 ~ 1.0
		// Triangle Manual Frequency
		// Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
		PrHelper::addFloat(
			layout, 
			prefix + CPK::SsgTri::freq, 
			prefixName + CPN::SsgTri::manualFreq, 
			CPV::SsgTri::ManualFreq::min, CPV::SsgTri::ManualFreq::max, CPV::SsgTri::ManualFreq::initial
		);
	}

	static inline void addSsgHwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		// Hardware Envelope Parameters
		PrHelper::addBool(
			layout, 
			prefix + CPK::SsgHwEnv::enable, 
			prefixName + CPN::SsgHwEnv::enable, 
			CPV::SsgHwEnv::Enable::initial
		); // HW Env Enable(Bool)
		PrHelper::addInt(
			layout, 
			prefix + CPK::SsgHwEnv::shape, 
			prefixName + CPN::SsgHwEnv::shape, 
			CPV::SsgHwEnv::Shape::min, CPV::SsgHwEnv::Shape::max, CPV::SsgHwEnv::Shape::initial
		); // Shape: 0-7
		// 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
		PrHelper::addFloat(
			layout, 
			prefix + CPK::SsgHwEnv::period, 
			prefixName + CPN::SsgHwEnv::period, 
			CPV::SsgHwEnv::Period::min, CPV::SsgHwEnv::Period::max, CPV::SsgHwEnv::Period::initial
		); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)
		PrHelper::addFloat(
			layout,
			prefix + CPK::SsgHwEnv::min,
			prefixName + CPN::SsgHwEnv::min,
			CPV::SsgHwEnv::Min::min, CPV::SsgHwEnv::Min::max, CPV::SsgHwEnv::Min::initial
		);
		PrHelper::addFloat(
			layout,
			prefix + CPK::SsgHwEnv::max,
			prefixName + CPN::SsgHwEnv::max,
			CPV::SsgHwEnv::Max::min, CPV::SsgHwEnv::Max::max, CPV::SsgHwEnv::Max::initial
		);
		// Period を大きくしたときのブツブツ音を和らげるスムース処理の ON/OFF
		PrHelper::addBool(
			layout,
			prefix + CPK::SsgHwEnv::smooth,
			prefixName + CPN::SsgHwEnv::smooth,
			CPV::SsgHwEnv::Smooth::initial
		);
	}

	// SSG HW PITCH ENV。波形スロットは音量版と同じで、Min / Max だけが
	// セント値になる。音量版と同じチャンネル・オペレーターへ同時に置ける。
	static inline void addSsgHwPEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addBool(
			layout,
			prefix + CPK::SsgHwPEnv::enable,
			prefixName + CPN::SsgHwPEnv::enable,
			CPV::SsgHwPEnv::Enable::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::SsgHwPEnv::shape,
			prefixName + CPN::SsgHwPEnv::shape,
			CPV::SsgHwPEnv::Shape::min, CPV::SsgHwPEnv::Shape::max, CPV::SsgHwPEnv::Shape::initial
		);
		// 音量版と同じく、値が大きいほど速くなる周波数(Hz)として扱う
		PrHelper::addFloat(
			layout,
			prefix + CPK::SsgHwPEnv::period,
			prefixName + CPN::SsgHwPEnv::period,
			CPV::SsgHwPEnv::Period::min, CPV::SsgHwPEnv::Period::max, CPV::SsgHwPEnv::Period::initial
		);
		// Min / Max はセント。1200 セントが 1 オクターブ。
		PrHelper::addInt(
			layout,
			prefix + CPK::SsgHwPEnv::min,
			prefixName + CPN::SsgHwPEnv::min,
			CPV::SsgHwPEnv::Min::min, CPV::SsgHwPEnv::Min::max, CPV::SsgHwPEnv::Min::initial
		);
		PrHelper::addInt(
			layout,
			prefix + CPK::SsgHwPEnv::max,
			prefixName + CPN::SsgHwPEnv::max,
			CPV::SsgHwPEnv::Max::min, CPV::SsgHwPEnv::Max::max, CPV::SsgHwPEnv::Max::initial
		);
		// 段差がそのまま音程の飛びになるので、鈍らせるスイッチを付けておく
		PrHelper::addBool(
			layout,
			prefix + CPK::SsgHwPEnv::smooth,
			prefixName + CPN::SsgHwPEnv::smooth,
			CPV::SsgHwPEnv::Smooth::initial
		);
	}
}
