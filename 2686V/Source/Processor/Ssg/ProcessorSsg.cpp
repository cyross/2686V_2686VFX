#include "./ProcessorSsg.h"

#include "./ProcessorSsgKeys.h"
#include "./ProcessorSsgValues.h"
#include "./ProcessorSsgNames.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = SsgPrKey::prefix;
    const juce::String prefixName = SsgPrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::level, prefixName + SsgPrName::level, SsgPrValue::Level::min, SsgPrValue::Level::max, SsgPrValue::Level::initial));

    // ==========================================
    // SSG Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::tone, prefixName + SsgPrName::tone, SsgPrValue::Tome::min, SsgPrValue::Tome::max, SsgPrValue::Tome::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::noise, prefixName + SsgPrName::Noise::title, SsgPrValue::Noise::min, SsgPrValue::Noise::max, SsgPrValue::Noise::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::noiseFreq, prefixName + SsgPrName::Noise::freq, SsgPrValue::NoiseFreq::min, SsgPrValue::NoiseFreq::max, SsgPrValue::NoiseFreq::initial)); // Noise Frequency Default: 12000Hz (Standard PSG feel)
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::noiseOnNote, prefixName + SsgPrName::Noise::noiseOnNote, SsgPrValue::NoiseOnNote::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::mix, prefixName + SsgPrName::toneNoiseMix, SsgPrValue::NoiseMix::min, SsgPrValue::NoiseMix::max, SsgPrValue::NoiseMix::initial)); // デフォルトは 0.5 (Mix)

    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::wveform, prefixName + SsgPrName::waveform, SsgPrValue::waveForm::min, SsgPrValue::waveForm::max, SsgPrValue::waveForm::initial)); // 0:Pulse, 1:Triangle

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::fix, prefixName + SsgPrName::fix, SsgPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::fixFreq, prefixName + SsgPrName::fixFreq, SsgPrValue::FixFreq::min, SsgPrValue::FixFreq::max, SsgPrValue::FixFreq::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::adsr + SsgPrKey::bypass, prefixName + SsgPrName::Adsr::bypass, SsgPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::pitchAdsr + SsgPrKey::bypass, prefixName + SsgPrName::PitchAdsr::bypass, SsgPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::ssgSwEnv + SsgPrKey::bypass, prefixName + SsgPrName::SsgSwEnv::bypass, SsgPrValue::SsgSwEnv::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::mul, prefixName +SsgPrName::mul, SsgPrValue::Mul::min, SsgPrValue::Mul::max, SsgPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::mulRatio, prefixName + SsgPrName::mulRatio, SsgPrValue::MulRatio::min, SsgPrValue::MulRatio::max, SsgPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::dt, prefixName + SsgPrName::dt1, SsgPrValue::Dt1::min, SsgPrValue::Dt1::max, SsgPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::dt2, prefixName + SsgPrName::dt2, SsgPrValue::Dt2::min, SsgPrValue::Dt2::max, SsgPrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::dt3, prefixName + SsgPrName::dt3, SsgPrValue::Dt3::min, SsgPrValue::Dt3::max, SsgPrValue::Dt3::initial));

    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::Duty::mode, prefixName + SsgPrName::Duty::mode, SsgPrValue::Duty::Mode::min, SsgPrValue::Duty::Mode::max, SsgPrValue::Duty::Mode::initial)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::Duty::preset, prefixName + SsgPrName::Duty::preset, SsgPrValue::Duty::Preset::min, SsgPrValue::Duty::Preset::max, SsgPrValue::Duty::Preset::initial)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::Duty::var, prefixName + SsgPrName::Duty::var, SsgPrValue::Duty::Var::min, SsgPrValue::Duty::Var::max, SsgPrValue::Duty::Var::initial)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::Duty::inv, prefixName + SsgPrName::Duty::invert, SsgPrValue::Duty::Inv::initial)); // Invert
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::Duty::fc, prefixName + SsgPrName::Duty::fc, SsgPrValue::Duty::Fc::initial)); // Famicom Mode
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::Duty::fcFluc, prefixName + SsgPrName::Duty::fcFluc, SsgPrValue::Duty::FcFluc::min, SsgPrValue::Duty::FcFluc::max, SsgPrValue::Duty::FcFluc::initial)); // Famicom Mode Fluctuation: 0.0 ~ 1.0

    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::Tri::keyTrk, prefixName + SsgPrName::Tri::keyTrack, SsgPrValue::Tri::KeyTrack::initial)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::Tri::peak, prefixName + SsgPrName::Tri::peak, SsgPrValue::Tri::Peak::min, SsgPrValue::Tri::Peak::max, SsgPrValue::Tri::Peak::initial)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::Tri::freq, prefixName + SsgPrName::Tri::manualFreq, SsgPrValue::Tri::ManualFreq::min, SsgPrValue::Tri::ManualFreq::max, SsgPrValue::Tri::ManualFreq::initial));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::bit, prefixName + SsgPrName::bit, SsgPrValue::Bit::min, SsgPrValue::Bit::max, SsgPrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::rate, prefixName + SsgPrName::rate, SsgPrValue::Rate::min, SsgPrValue::Rate::max, SsgPrValue::Rate::initial)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + SsgPrKey::HwEnv::enable, prefixName + SsgPrName::HwEnv::enable, SsgPrValue::HwEnv::Enable::initial)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::HwEnv::shape, prefixName + SsgPrName::HwEnv::shape, SsgPrValue::HwEnv::Shape::min, SsgPrValue::HwEnv::Shape::max, SsgPrValue::HwEnv::Shape::initial)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::HwEnv::period, prefixName + SsgPrName::HwEnv::period, SsgPrValue::HwEnv::Period::min, SsgPrValue::HwEnv::Period::max, SsgPrValue::HwEnv::Period::initial)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::Unison::voices, prefixName + SsgPrName::Unison::voices, SsgPrValue::Unison::Voices::min, SsgPrValue::Unison::Voices::max, SsgPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + SsgPrKey::Unison::detune, prefixName + SsgPrName::Unison::detune, SsgPrValue::Unison::Detune::min, SsgPrValue::Unison::Detune::max, SsgPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + SsgPrKey::Unison::spread, prefixName + SsgPrName::Unison::spread, SsgPrValue::Unison::Spread::min, SsgPrValue::Unison::Spread::max, SsgPrValue::Unison::Spread::initial));

    addEnvParameters(layout, prefix, prefixName);
    addPitchEnvParameters(layout, prefix, prefixName);
	addSsgSwEnvParameters(layout, prefix, prefixName);
    addOpzx7LfoParameters(layout, prefix, prefixName);
}

void SsgProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = SsgPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + SsgPrKey::level);

    pTone = apvts.getRawParameterValue(prefix + SsgPrKey::tone);
    pNoise = apvts.getRawParameterValue(prefix + SsgPrKey::noise);
    pNoiseFreq = apvts.getRawParameterValue(prefix + SsgPrKey::noiseFreq);
    pNoiseOnNote = apvts.getRawParameterValue(prefix + SsgPrKey::noiseOnNote);
    pMix = apvts.getRawParameterValue(prefix + SsgPrKey::mix);
    pWaveform = apvts.getRawParameterValue(prefix + SsgPrKey::wveform);
    pBit = apvts.getRawParameterValue(prefix + SsgPrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + SsgPrKey::rate);

    pFixMode = apvts.getRawParameterValue(prefix + SsgPrKey::fix);
    pFixFreq = apvts.getRawParameterValue(prefix + SsgPrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(prefix + SsgPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + SsgPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + SsgPrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(prefix + SsgPrKey::adsr + SsgPrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(prefix + SsgPrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(prefix + SsgPrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(prefix + SsgPrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(prefix + SsgPrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(prefix + SsgPrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(prefix + SsgPrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(prefix + SsgPrKey::pitchAdsr + SsgPrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(prefix + SsgPrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(prefix + SsgPrKey::ssgSwEnv + SsgPrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(prefix + SsgPrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(prefix + SsgPrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(prefix + SsgPrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(prefix + SsgPrKey::dt);
    pDetune2 = apvts.getRawParameterValue(prefix + SsgPrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(prefix + SsgPrKey::dt3);

    pDutyMode = apvts.getRawParameterValue(prefix + SsgPrKey::Duty::mode);
    pDutyPreset = apvts.getRawParameterValue(prefix + SsgPrKey::Duty::preset);
    pDutyVar = apvts.getRawParameterValue(prefix + SsgPrKey::Duty::var);
    pDutyInvert = apvts.getRawParameterValue(prefix + SsgPrKey::Duty::inv);
    pDutyFc = apvts.getRawParameterValue(prefix + SsgPrKey::Duty::fc);
    pDutyFcFluc = apvts.getRawParameterValue(prefix + SsgPrKey::Duty::fcFluc);

    pTriKeyTrk = apvts.getRawParameterValue(prefix + SsgPrKey::Tri::keyTrk);
    pTriPeak = apvts.getRawParameterValue(prefix + SsgPrKey::Tri::peak);
    pTriFreq = apvts.getRawParameterValue(prefix + SsgPrKey::Tri::freq);

    pHwEnvEnable = apvts.getRawParameterValue(prefix + SsgPrKey::HwEnv::enable);
    pHwEnvShape = apvts.getRawParameterValue(prefix + SsgPrKey::HwEnv::shape);
    pHwEnvPeriod = apvts.getRawParameterValue(prefix + SsgPrKey::HwEnv::period);

    pLfoPmSyncDelay = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pmSyncDelay);
    pLfoAmSyncDelay = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amSyncDelay);
    pLfoAmSmoothRatio = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amSmoothRatio);
    pLfoPmFreq = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pmFreq);
    pLfoAmFreq = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amFreq);
    pLfoPmShape = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pgShape);
    pLfoAmShape = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::egShape);
    pLfoPm = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pm);
    pLfoAm = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::am);
    pLfoPmd = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pmd);
    pLfoPms = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pms);
    pLfoAmd = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amd);
    pLfoAms = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::ams);
}

void SsgProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.ssg.level = pLevel->load(std::memory_order_relaxed);

    // --- SSG Parameters ---
    params.ssg.tone = pTone->load(std::memory_order_relaxed);
    params.ssg.noiseLevel = pNoise->load(std::memory_order_relaxed);
    params.ssg.noiseFreq = pNoiseFreq->load(std::memory_order_relaxed);
    params.ssg.noiseOnNote = (pNoiseOnNote->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.mix = pMix->load(std::memory_order_relaxed);
    params.ssg.waveform = (int)pWaveform->load(std::memory_order_relaxed);
    params.ssg.bitDepth = (int)pBit->load(std::memory_order_relaxed);
    params.ssg.rateIndex = (int)pRate->load(std::memory_order_relaxed);

    params.ssg.fixedMode = (pFixMode->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.fixedFreq = pFixFreq->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.ssg.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.ssg.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.ssg.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    params.ssg.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.ssg.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.ssg.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.ssg.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.ssg.detune3 = (int)pDetune3->load(std::memory_order_relaxed);

    params.ssg.dutyMode = (int)pDutyMode->load(std::memory_order_relaxed);
    params.ssg.dutyPreset = (int)pDutyPreset->load(std::memory_order_relaxed);
    params.ssg.dutyVar = pDutyVar->load(std::memory_order_relaxed);
    params.ssg.dutyInvert = (pDutyInvert->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.dutyFc = (pDutyFc->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.dutyFcFluc = pDutyFcFluc->load(std::memory_order_relaxed);

    params.ssg.triKeyTrack = (pTriKeyTrk->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.triPeak = pTriPeak->load(std::memory_order_relaxed);
    params.ssg.triFreq = pTriFreq->load(std::memory_order_relaxed);

    params.ssg.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.ssg.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.ssg.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.ssg.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.ssg.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);
    params.ssg.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > SsgPrValue::boolThread;

    params.ssg.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.ssg.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.ssg.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.ssg.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.ssg.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.ssg.pitchAdsr.ssl = (int)pPitchAdsrSsl->load(std::memory_order_relaxed);
    params.ssg.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.ssg.useHwEnv = (pHwEnvEnable->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.envShape = (int)pHwEnvShape->load(std::memory_order_relaxed);
    params.ssg.envPeriod = pHwEnvPeriod->load(std::memory_order_relaxed);

	params.ssg.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
	params.ssg.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
	params.ssg.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.ssg.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
	params.ssg.ssgSwEnv.r5 = pSsgSwEnvR5->load(std::memory_order_relaxed);
    params.ssg.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.ssg.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.ssg.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.ssg.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.ssg.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.ssg.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.ssg.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.ssg.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.ssg.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > SsgPrValue::boolThread);
    params.ssg.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.ssg.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.ssg.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.ssg.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.ssg.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.ssg.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
}
