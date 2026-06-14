#include "./ProcessorWt2.h"

#include "./ProcessorWt2Keys.h"
#include "./ProcessorWt2Values.h"
#include "./ProcessorWt2Names.h"

void Wt2Processor::createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& prefix, const juce::String& name)
{
    for (int i = 0; i < size; ++i)
    {
        auto paramId = prefix + juce::String(i);
        auto paramName = name + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterInt>(paramId, paramName, Wt2PrValue::Custom::min, Wt2PrValue::Custom::max, Wt2PrValue::Custom::initial));
    }
}

template <size_t I>
void Wt2Processor::processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix)
{
    // Custom Wave params reading
    for (int i = 0; i < I; ++i)
    {
        auto paramId = prefix + juce::String(i);
        samples[i] = (int)*apvts.getRawParameterValue(paramId);
    }

}

void Wt2Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = Wt2PrKey::prefix;
    const juce::String prefixName = Wt2PrName::prefix;

    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::bit, prefixName + Wt2PrName::bit, Wt2PrValue::Bit::min, Wt2PrValue::Bit::max, Wt2PrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::rate, prefixName + Wt2PrName::rate, Wt2PrValue::Rate::min, Wt2PrValue::Rate::max, Wt2PrValue::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64, 2:128, 3:256
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::sampleSize, prefixName + Wt2PrName::sampleSize, Wt2PrValue::SammpleSize::min, Wt2PrValue::SammpleSize::max, Wt2PrValue::SammpleSize::initial));
    // Resolution : 0:16, 1:32, 2:64, 3:128, 4:256
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::resolution, prefixName + Wt2PrName::resolution, Wt2PrValue::Resolution::min, Wt2PrValue::Resolution::max, Wt2PrValue::Resolution::initial));
    // Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::wave, prefixName + Wt2PrName::waveform, Wt2PrValue::WaveForm::min, Wt2PrValue::WaveForm::max, Wt2PrValue::WaveForm::initial));

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Wt2PrKey::fix, prefixName + Wt2PrName::fix, Wt2PrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Wt2PrKey::fixFreq, prefixName + Wt2PrName::fixFreq, Wt2PrValue::FixFreq::min, Wt2PrValue::FixFreq::max, Wt2PrValue::FixFreq::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::Unison::voices, prefixName + Wt2PrName::Unison::voices, Wt2PrValue::Unison::Voices::min, Wt2PrValue::Unison::Voices::max, Wt2PrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::Unison::detune, prefixName + Wt2PrName::Unison::detune, Wt2PrValue::Unison::Detune::min, Wt2PrValue::Unison::Detune::max, Wt2PrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Wt2PrKey::Unison::spread, prefixName + Wt2PrName::Unison::spread, Wt2PrValue::Unison::Spread::min, Wt2PrValue::Unison::Spread::max, Wt2PrValue::Unison::Spread::initial));

    createCustomWaveLayout(layout, Wt2PrValue::customSize1, prefix + Wt2PrKey::custom32, prefixName + Wt2PrName::custom32);
    createCustomWaveLayout(layout, Wt2PrValue::customSize2, prefix + Wt2PrKey::custom64, prefixName + Wt2PrName::custom64);
    createCustomWaveLayout(layout, Wt2PrValue::customSize3, prefix + Wt2PrKey::custom128, prefixName + Wt2PrName::custom128);
    createCustomWaveLayout(layout, Wt2PrValue::customSize4, prefix + Wt2PrKey::custom256, prefixName + Wt2PrName::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Wt2PrKey::Mod::enable, prefixName + Wt2PrName::Mod::enable, Wt2PrValue::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Wt2PrKey::Mod::depth, prefixName + Wt2PrName::Mod::depth, Wt2PrValue::Mod::Depth::min, Wt2PrValue::Mod::Depth::max, Wt2PrValue::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Wt2PrKey::Mod::speed, prefixName + Wt2PrName::Mod::speed, Wt2PrValue::Mod::Speed::min, Wt2PrValue::Mod::Speed::max, Wt2PrValue::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Wt2PrKey::level, prefixName + Wt2PrName::level, Wt2PrValue::Level::min, Wt2PrValue::Level::max, Wt2PrValue::Level::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::mul, prefixName + Wt2PrName::mul, Wt2PrValue::Mul::min, Wt2PrValue::Mul::max, Wt2PrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Wt2PrKey::mulRatio, prefixName + Wt2PrName::mulRatio, Wt2PrValue::MulRatio::min, Wt2PrValue::MulRatio::max, Wt2PrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::dt, prefixName + Wt2PrName::dt1, Wt2PrValue::Dt1::min, Wt2PrValue::Dt1::max, Wt2PrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Wt2PrKey::dt2, prefixName + Wt2PrName::dt2, Wt2PrValue::Dt2::min, Wt2PrValue::Dt2::max, Wt2PrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt> (prefix + Wt2PrKey::dt3, prefixName + Wt2PrName::dt3, Wt2PrValue::Dt3::min, Wt2PrValue::Dt3::max, Wt2PrValue::Dt3::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Wt2PrKey::adsr + Wt2PrKey::bypass, prefixName + Wt2PrName::Adsr::bypass, Wt2PrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Wt2PrKey::pitchAdsr + Wt2PrKey::bypass, prefixName + Wt2PrName::PitchAdsr::bypass, Wt2PrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Wt2PrKey::ssgSwEnv + Wt2PrKey::bypass, prefixName + Wt2PrName::SsgSwEnv::bypass, Wt2PrValue::SsgSwEnv::Bypass::initial));

    addEnvParameters(layout, prefix, prefixName);
	addPitchEnvParameters(layout, prefix, prefixName);
    addSsgSwEnvParameters(layout, prefix, prefixName);
    addOpzx7LfoParameters(layout, prefix, prefixName);
}

void Wt2Processor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = Wt2PrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + Wt2PrKey::level);
    pSampleSize = apvts.getRawParameterValue(prefix + Wt2PrKey::sampleSize);
    pResolution = apvts.getRawParameterValue(prefix + Wt2PrKey::resolution);
    pWave = apvts.getRawParameterValue(prefix + Wt2PrKey::wave);
    pBit = apvts.getRawParameterValue(prefix + Wt2PrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + Wt2PrKey::rate);

    for (int i = 0; i < 32; ++i)
    {
        pCustom32[i] = apvts.getRawParameterValue(prefix + Wt2PrKey::custom32 + juce::String(i));
    }

    for (int i = 0; i < 64; ++i)
    {
        pCustom64[i] = apvts.getRawParameterValue(prefix + Wt2PrKey::custom64 + juce::String(i));
    }

    for (int i = 0; i < 128; ++i)
    {
        pCustom128[i] = apvts.getRawParameterValue(prefix + Wt2PrKey::custom128 + juce::String(i));
    }

    for (int i = 0; i < 256; ++i)
    {
        pCustom256[i] = apvts.getRawParameterValue(prefix + Wt2PrKey::custom256 + juce::String(i));
    }

    pModEnable = apvts.getRawParameterValue(prefix + Wt2PrKey::Mod::enable);
    pModDepth = apvts.getRawParameterValue(prefix + Wt2PrKey::Mod::depth);
    pModSpeed = apvts.getRawParameterValue(prefix + Wt2PrKey::Mod::speed);

    pFixMode = apvts.getRawParameterValue(prefix + Wt2PrKey::fix);
    pFixFreq = apvts.getRawParameterValue(prefix + Wt2PrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(prefix + Wt2PrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + Wt2PrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + Wt2PrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(prefix + Wt2PrKey::adsr + Wt2PrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(prefix + Wt2PrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(prefix + Wt2PrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(prefix + Wt2PrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(prefix + Wt2PrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(prefix + Wt2PrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(prefix + Wt2PrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(prefix + Wt2PrKey::pitchAdsr + Wt2PrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(prefix + Wt2PrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(prefix + Wt2PrKey::ssgSwEnv + Wt2PrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(prefix + Wt2PrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(prefix + Wt2PrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(prefix + Wt2PrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(prefix + Wt2PrKey::dt);
    pDetune2 = apvts.getRawParameterValue(prefix + Wt2PrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(prefix + Wt2PrKey::dt3);

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

void Wt2Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.wt2.bitDepth = (int)pBit->load(std::memory_order_relaxed);
    params.wt2.rateIndex = (int)pRate->load(std::memory_order_relaxed);
    params.wt2.tableSize = (int)pSampleSize->load(std::memory_order_relaxed);
    params.wt2.customWaveResolution = (int)pResolution->load(std::memory_order_relaxed);
    params.wt2.waveform = (int)pWave->load(std::memory_order_relaxed);

    params.wt2.fixedMode = (pFixMode->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.fixedFreq = pFixFreq->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.wt2.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.wt2.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.wt2.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int i = 0; i < 32; ++i)
    {
        params.wt2.customWave32[i] = pCustom32[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 64; ++i)
    {
        params.wt2.customWave64[i] = pCustom64[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 128; ++i)
    {
        params.wt2.customWave128[i] = pCustom128[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 256; ++i)
    {
        params.wt2.customWave256[i] = pCustom256[i]->load(std::memory_order_relaxed);
    }

    params.wt2.modEnable = (pModEnable->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.modDepth = pModDepth->load(std::memory_order_relaxed);
    params.wt2.modSpeed = pModSpeed->load(std::memory_order_relaxed);
    params.wt2.level = pLevel->load(std::memory_order_relaxed);

    params.wt2.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.wt2.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.wt2.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.wt2.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.wt2.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);
    params.wt2.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > Wt2PrValue::boolThread;

    params.wt2.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.ssl = (int)pPitchAdsrSsl->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.wt2.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r5 = pSsgSwEnvR5->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.wt2.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.wt2.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.wt2.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.wt2.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.wt2.detune3 = (int)pDetune3->load(std::memory_order_relaxed);

    params.wt2.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.wt2.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.wt2.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.wt2.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.wt2.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.wt2.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.wt2.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.wt2.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.wt2.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.wt2.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.wt2.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
}
