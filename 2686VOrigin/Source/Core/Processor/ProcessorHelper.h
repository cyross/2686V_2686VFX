#pragma once

#include "../../Generator/Fds/GenFdsModTable.h"
#include <JuceHeader.h>
#include <array>

#include "./ProcessorStructs.h"
#include "./ProcessorKeys.h"
#include "./ProcessorNames.h"
#include "./ProcessorValues.h"

#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssrParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Detune/Opn/DetuneOpnParams.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../Effect/Lfo/N88/LfoN88Params.h"
#include "../../Effect/Lfo/Opna/LfoOpnaParams.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Generator/Fm/Fix/FmFixParams.h"
#include "../../Synth/Adpcm/SynthAdpcmParams.h"
#include "../../Synth/Opna/SynthOpnaParams.h"
#include "../../Synth/Rhythm/SynthRhythmParams.h"
#include "../../Synth/Ssg/SynthSsgParams.h"

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
		ptPtrs.l11 = apvts.getRawParameterValue(prefix + CPK::SsgSwPEnv11::l11);
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

	static inline void setupSsgHwEnv(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsSsgHwEnv& ptPtrs){
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

	static inline void setupAdpcmBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsAdpcmBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.pan = apvts.getRawParameterValue(prefix + CPK::pan);
		ptPtrs.loop = apvts.getRawParameterValue(prefix + CPK::loop);
	}

	static inline void setupOpnaBasicPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, PrPtrsOpnaBasic& ptPtrs){
		ptPtrs.level = apvts.getRawParameterValue(prefix + CPK::level);
		ptPtrs.pan = apvts.getRawParameterValue(prefix + CPK::pan);
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

	static inline void applySsgHwEnv(PrPtrsSsgHwEnv& ptPtrs, SsgHwEnvParams& params){
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

	static inline void applyOpnaBasic(PrPtrsOpnaBasic& ptPtrs, OpnaParams& params){
		params.level = PrHelper::getFloat(ptPtrs.level);
		params.pan = PrHelper::getInt(ptPtrs.pan);
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

	static inline void applySsgEg(PrPtrsSsgEg& ptPtrs, SsgEgParams& params){
		params.eg = PrHelper::getInt(ptPtrs.ssgEg);
		params.freq = PrHelper::getFloat(ptPtrs.ssgEgFreq);
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

	static inline void addSsgBasicParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName) {
		PrHelper::addInt(
			layout, 
			prefix + CPK::ssgWaveform, 
			prefixName + CPN::ssgWaveform, 
			CPV::SsgWaveForm::min, CPV::SsgWaveForm::max, CPV::SsgWaveForm::initial
		); // 0:Pulse, 1:Triangle
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
