#include "./ProcessorWt.h"

#include "./ProcessorWtKeys.h"
#include "./ProcessorWtValues.h"
#include "./ProcessorWtNames.h"

void WtProcessor::createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& prefix, const juce::String& name)
{
    for (int i = 0; i < size; ++i)
    {
        auto paramId = prefix + juce::String(i);
        auto paramName = name + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, WtPrValue::Custom::min, WtPrValue::Custom::max, WtPrValue::Custom::initial));
    }
}

template <size_t I>
void WtProcessor::processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix)
{
    // Custom Wave params reading
    for (int i = 0; i < I; ++i)
    {
        auto paramId = prefix + juce::String(i);
        samples[i] = *apvts.getRawParameterValue(paramId);
    }

}

void WtProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = WtPrKey::prefix;
    const juce::String prefixName = WtPrName::prefix;

    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::bit, prefixName + WtPrName::bit, WtPrValue::Bit::min, WtPrValue::Bit::max, WtPrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::rate, prefixName + WtPrName::rate, WtPrValue::Rate::min, WtPrValue::Rate::max, WtPrValue::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64, 2:128, 3:256
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::sampleSize, prefixName + WtPrName::sampleSize, WtPrValue::SammpleSize::min, WtPrValue::SammpleSize::max, WtPrValue::SammpleSize::initial));
    // Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::steps, prefixName + WtPrName::steps, WtPrValue::Steps::min, WtPrValue::Steps::max, WtPrValue::Steps::initial));
    // Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::wave, prefixName + WtPrName::waveform, WtPrValue::WaveForm::min, WtPrValue::WaveForm::max, WtPrValue::WaveForm::initial));

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + WtPrKey::fix, prefixName + WtPrName::fix, WtPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + WtPrKey::fixFreq, prefixName + WtPrName::fixFreq, WtPrValue::FixFreq::min, WtPrValue::FixFreq::max, WtPrValue::FixFreq::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::Unison::voices, prefixName + WtPrName::Unison::voices, WtPrValue::Unison::Voices::min, WtPrValue::Unison::Voices::max, WtPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::Unison::detune, prefixName + WtPrName::Unison::detune, WtPrValue::Unison::Detune::min, WtPrValue::Unison::Detune::max, WtPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + WtPrKey::Unison::spread, prefixName + WtPrName::Unison::spread, WtPrValue::Unison::Spread::min, WtPrValue::Unison::Spread::max, WtPrValue::Unison::Spread::initial));

    createCustomWaveLayout(layout, WtPrValue::customSize1, prefix + WtPrKey::custom32, prefixName + WtPrName::custom32);
    createCustomWaveLayout(layout, WtPrValue::customSize2, prefix + WtPrKey::custom64, prefixName + WtPrName::custom64);
    createCustomWaveLayout(layout, WtPrValue::customSize3, prefix + WtPrKey::custom128, prefixName + WtPrName::custom128);
    createCustomWaveLayout(layout, WtPrValue::customSize4, prefix + WtPrKey::custom256, prefixName + WtPrName::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + WtPrKey::Mod::enable, prefixName + WtPrName::Mod::enable, WtPrValue::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + WtPrKey::Mod::depth, prefixName + WtPrName::Mod::depth, WtPrValue::Mod::Depth::min, WtPrValue::Mod::Depth::max, WtPrValue::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + WtPrKey::Mod::speed, prefixName + WtPrName::Mod::speed, WtPrValue::Mod::Speed::min, WtPrValue::Mod::Speed::max, WtPrValue::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + WtPrKey::level, prefixName + WtPrName::level, WtPrValue::Level::min, WtPrValue::Level::max, WtPrValue::Level::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::mul, prefixName + WtPrName::mul, WtPrValue::Mul::min, WtPrValue::Mul::max, WtPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + WtPrKey::mulRatio, prefixName + WtPrName::mulRatio, WtPrValue::MulRatio::min, WtPrValue::MulRatio::max, WtPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::dt, prefixName + WtPrName::dt1, WtPrValue::Dt1::min, WtPrValue::Dt1::max, WtPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + WtPrKey::dt2, prefixName + WtPrName::dt2, WtPrValue::Dt2::min, WtPrValue::Dt2::max, WtPrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt> (prefix + WtPrKey::dt3, prefixName + WtPrName::dt3, WtPrValue::Dt3::min, WtPrValue::Dt3::max, WtPrValue::Dt3::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + WtPrKey::adsr + WtPrKey::bypass, prefixName + WtPrName::Adsr::bypass, WtPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + WtPrKey::pitchAdsr + WtPrKey::bypass, prefixName + WtPrName::PitchAdsr::bypass, WtPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + WtPrKey::ssgSwEnv + WtPrKey::bypass, prefixName + WtPrName::SsgSwEnv::bypass, WtPrValue::SsgSwEnv::Bypass::initial));

    addEnvParameters(layout, prefix, prefixName);
	addPitchEnvParameters(layout, prefix, prefixName);
    addSsgSwEnvParameters(layout, prefix, prefixName);
    addOpzx7LfoParameters(layout, prefix, prefixName);
}

void WtProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = WtPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + WtPrKey::level);
    pSampleSize = apvts.getRawParameterValue(prefix + WtPrKey::sampleSize);
    pStep = apvts.getRawParameterValue(prefix + WtPrKey::steps);
    pWave = apvts.getRawParameterValue(prefix + WtPrKey::wave);
    pBit = apvts.getRawParameterValue(prefix + WtPrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + WtPrKey::rate);

    for (int i = 0; i < 32; ++i)
    {
        pCustom32[i] = apvts.getRawParameterValue(prefix + WtPrKey::custom32 + juce::String(i));
    }

    for (int i = 0; i < 64; ++i)
    {
        pCustom64[i] = apvts.getRawParameterValue(prefix + WtPrKey::custom64 + juce::String(i));
    }

    for (int i = 0; i < 128; ++i)
    {
        pCustom128[i] = apvts.getRawParameterValue(prefix + WtPrKey::custom128 + juce::String(i));
    }

    for (int i = 0; i < 256; ++i)
    {
        pCustom256[i] = apvts.getRawParameterValue(prefix + WtPrKey::custom256 + juce::String(i));
    }

    pModEnable = apvts.getRawParameterValue(prefix + WtPrKey::Mod::enable);
    pModDepth = apvts.getRawParameterValue(prefix + WtPrKey::Mod::depth);
    pModSpeed = apvts.getRawParameterValue(prefix + WtPrKey::Mod::speed);

    pFixMode = apvts.getRawParameterValue(prefix + WtPrKey::fix);
    pFixFreq = apvts.getRawParameterValue(prefix + WtPrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(prefix + WtPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + WtPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + WtPrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(prefix + WtPrKey::adsr + WtPrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(prefix + WtPrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(prefix + WtPrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(prefix + WtPrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(prefix + WtPrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(prefix + WtPrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(prefix + WtPrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(prefix + WtPrKey::pitchAdsr + WtPrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(prefix + WtPrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(prefix + WtPrKey::ssgSwEnv + WtPrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(prefix + WtPrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(prefix + WtPrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(prefix + WtPrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(prefix + WtPrKey::dt);
    pDetune2 = apvts.getRawParameterValue(prefix + WtPrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(prefix + WtPrKey::dt3);

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

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.wt.bitDepth = (int)pBit->load(std::memory_order_relaxed);
    params.wt.rateIndex = (int)pRate->load(std::memory_order_relaxed);
    params.wt.tableSize = (int)pSampleSize->load(std::memory_order_relaxed);
    params.wt.steps = (int)pStep->load(std::memory_order_relaxed);
    params.wt.waveform = (int)pWave->load(std::memory_order_relaxed);

    params.wt.fixedMode = (pFixMode->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.fixedFreq = pFixFreq->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.wt.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.wt.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.wt.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int i = 0; i < 32; ++i)
    {
        params.wt.customWave32[i] = pCustom32[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 64; ++i)
    {
        params.wt.customWave64[i] = pCustom64[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 128; ++i)
    {
        params.wt.customWave128[i] = pCustom128[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 256; ++i)
    {
        params.wt.customWave256[i] = pCustom256[i]->load(std::memory_order_relaxed);
    }

    params.wt.modEnable = (pModEnable->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.modDepth = pModDepth->load(std::memory_order_relaxed);
    params.wt.modSpeed = pModSpeed->load(std::memory_order_relaxed);
    params.wt.level = pLevel->load(std::memory_order_relaxed);

    params.wt.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.wt.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.wt.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.wt.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.wt.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);
    params.wt.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > WtPrValue::boolThread;

    params.wt.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.ssl = (int)pPitchAdsrSsl->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.wt.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r5 = pSsgSwEnvR5->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.wt.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.wt.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.wt.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.wt.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.wt.detune3 = (int)pDetune3->load(std::memory_order_relaxed);

    params.wt.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.wt.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.wt.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.wt.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.wt.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.wt.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.wt.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.wt.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.wt.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.wt.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.wt.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
}
